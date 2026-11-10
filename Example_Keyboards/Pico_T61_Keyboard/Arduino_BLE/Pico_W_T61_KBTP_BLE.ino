/*
    Copyright 2026 Frank Adams

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://apache.org

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

This code makes a Lenovo T61 laptop keyboard and trackpoint work as a Bluetooth (BLE)
keyboard and mouse using a Raspberry Pi Pico W and the Earle Philhower Arduino Pico core.
The keyboard matrix is scanned directly by Pico GPIOs. Holding Fn turns the arrow keys
into Stop (Up), Play/Pause (Down), Previous (Left) and Next (Right). The trackpoint is a PS/2 device
on GP0 (data) and GP1 (clock), read with a clock interrupt.
*/

#include <KeyboardBLE.h> 
#include <MouseBLE.h>    // Both libraries initialize together under the shared BLE stack

// --- Matrix Keyboard Configurations ---
const int NUM_ROWS = 16;
const int NUM_COLS = 8;

const int colPins[NUM_COLS] = {11, 22, 10, 21, 9, 20, 8, 7};
const int rowPins[NUM_ROWS] = {27, 26, 17, 14, 12, 19, 18, 15, 13, 6, 16, 5, 28, 2, 4, 3};

#define TOKEN_MUTE_KEY 0xFE
#define TOKEN_VOL_UP   0xFD
#define TOKEN_VOL_DOWN 0xFC
#define TOKEN_FN       0xFB   // Fn key: sends nothing itself, turns the arrows into media keys

const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { '`',           '1',              'q',          KEY_TAB,        'a',            KEY_ESC,      'z',             0                },
  { KEY_F1,        '2',              'w',          KEY_CAPS_LOCK,  's',            0,            'x',             0                },
  { KEY_F2,        '3',              'e',          KEY_F3,         'd',            KEY_F4,       'c',             0                },
  { '5',           '4',              'r',          't',            'f',            'g',          'v',             'b'              },
  { '6',           '7',              'u',          'y',            'j',            'h',          'm',             'n'              },
  { '=',           '8',              'i',          ']',            'k',            KEY_F6,       ',',             0                },
  { KEY_F8,        '9',              'o',          KEY_F7,         'l',            0,            '.',             0                },
  { '-',           '0',              'p',          '[',            ';',            '\'',         0,               '/'              },
  { KEY_F9,        KEY_F10,          0,            KEY_BACKSPACE,  '\\',           KEY_F5,       KEY_RETURN,      ' '              },
  { KEY_INSERT,    KEY_F12,          0,            0,              TOKEN_FN,       0,            0,               KEY_RIGHT_ARROW  },
  { KEY_DELETE,    KEY_F11,          TOKEN_VOL_UP, TOKEN_VOL_DOWN, TOKEN_MUTE_KEY, 0,            0,               KEY_DOWN_ARROW   },
  { KEY_PAGE_UP,   KEY_PAGE_DOWN,    KEY_LEFT_GUI, 0,              KEY_MENU,       0,            0,               0                },
  { KEY_HOME,      KEY_END,          0,            0,              0,              KEY_UP_ARROW, 0,               KEY_LEFT_ARROW   },
  { 0,             KEY_PRINT_SCREEN, KEY_NUM_LOCK, 0,              0,              KEY_LEFT_ALT, 0,               KEY_RIGHT_ALT    },
  { 0,             0,                0,            KEY_LEFT_SHIFT, 0,              0,            KEY_RIGHT_SHIFT, 0                },
  { KEY_LEFT_CTRL, 0,                0,            0,              0,              0,            KEY_RIGHT_CTRL,  0                }
};

bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // key states from the last scan, so each change is sent once

// --- Fn layer: with Fn held, the arrow keys send media (consumer) keys ---
bool fnHeld = false;                        // true while the Fn key is down
bool sentAsMedia[NUM_ROWS][NUM_COLS] = {false}; // arrow was pressed with Fn, so release it as a media key

struct FnMediaKey {
  int row;
  int col;
  uint16_t consumerCode;
};
const FnMediaKey fnMediaKeys[] = {
  {12, 5, KEY_STOP},           // Fn + Up    (row GP28, column GP20)
  {10, 7, KEY_PLAY_PAUSE},     // Fn + Down  (row GP16, column GP7)
  {12, 7, KEY_SCAN_PREVIOUS},  // Fn + Left  (row GP28, column GP7)
  { 9, 7, KEY_SCAN_NEXT}       // Fn + Right (row GP6,  column GP7)
};
const int NUM_FN_MEDIA_KEYS = sizeof(fnMediaKeys) / sizeof(fnMediaKeys[0]);

// Returns the media code for the key at row r, column c when Fn is held (0 = not an Fn media key)
uint16_t fnMediaCode(int r, int c) {
  for (int i = 0; i < NUM_FN_MEDIA_KEYS; i++) {
    if (fnMediaKeys[i].row == r && fnMediaKeys[i].col == c) return fnMediaKeys[i].consumerCode;
  }
  return 0;
}

// --- Trackpoint Configurations ---
const int PS2_DATA_PIN  = 0;
const int PS2_CLOCK_PIN = 1;

// Each PS/2 byte is 11 bits: start(0), 8 data bits (LSB first), odd parity, stop(1).
// A trackpoint movement packet is 3 bytes: status, X movement, Y movement.
// The interrupt routine below decodes complete packets and ADDS the movement to
// tpAccumX / tpAccumY. The loop sends the accumulated movement whenever the BLE link
// is free. BLE can only send one report per connection interval (often 15-30 msec)
// while the trackpoint sends a packet every 10 msec, so this keeps every movement
// without the interrupt ever overrunning a buffer.
volatile uint16_t bitBuffer = 0;        // bits of the byte being received
volatile int bitCount = 0;              // number of bits received for this byte
volatile unsigned long lastBitTime = 0; // micros() of the last clock edge
volatile unsigned long lastByteTime = 0;// micros() of the last complete byte

volatile uint8_t tpPacket[3];           // packet being assembled
volatile int tpIndex = 0;               // next byte position in tpPacket

volatile int32_t tpAccumX = 0;          // movement not yet sent over BLE
volatile int32_t tpAccumY = 0;
volatile uint8_t tpButtons = 0;         // latest button bits: 0x01 left, 0x02 right, 0x04 middle
volatile uint8_t tpButtonsLatched = 0;  // buttons pressed since the loop last looked, so a quick
                                        // click is never missed while a BLE send is in progress

uint8_t sentButtons = 0;                // button bits last sent over BLE

const unsigned long PS2_PACKET_GAP_US = 4000;  // gap that means a new packet is starting

// Wait for the PS/2 clock to reach "level". Returns false after timeoutMs.
bool waitPS2Clock(int level, unsigned long start, unsigned long timeoutMs) {
  while (digitalRead(PS2_CLOCK_PIN) != level) {
    if (millis() - start > timeoutMs) return false;
  }
  return true;
}

// Standard PS/2 Open-Drain Writing Implementation (host to trackpoint).
// Every wait has a timeout so a missing or unresponsive trackpoint can't hang the keyboard.
// Returns true if the trackpoint clocked in the byte and sent its acknowledge bit.
bool writePS2Byte(uint8_t value) {
  const unsigned long TIMEOUT_MS = 30;
  unsigned long start = millis();
  bool ok = true;

  // Request to send: hold clock low at least 100 usec, then pull data low, then release clock
  pinMode(PS2_CLOCK_PIN, OUTPUT);
  digitalWrite(PS2_CLOCK_PIN, LOW);
  delayMicroseconds(200);

  pinMode(PS2_DATA_PIN, OUTPUT);
  digitalWrite(PS2_DATA_PIN, LOW);

  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);

  uint8_t parity = 1;

  // 8 data bits, LSB first. The trackpoint reads each bit on the rising clock edge.
  for (int i = 0; i < 8 && ok; i++) {
    ok = waitPS2Clock(LOW, start, TIMEOUT_MS);
    uint8_t bit = (value >> i) & 0x01;
    if (bit == 0) {
      pinMode(PS2_DATA_PIN, OUTPUT);
      digitalWrite(PS2_DATA_PIN, LOW);
    } else {
      pinMode(PS2_DATA_PIN, INPUT_PULLUP);
    }
    parity ^= bit;
    ok = ok && waitPS2Clock(HIGH, start, TIMEOUT_MS);
  }

  // Parity bit
  ok = ok && waitPS2Clock(LOW, start, TIMEOUT_MS);
  if (parity == 0) {
    pinMode(PS2_DATA_PIN, OUTPUT);
    digitalWrite(PS2_DATA_PIN, LOW);
  } else {
    pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  }
  ok = ok && waitPS2Clock(HIGH, start, TIMEOUT_MS);

  // Stop bit: release data
  ok = ok && waitPS2Clock(LOW, start, TIMEOUT_MS);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  ok = ok && waitPS2Clock(HIGH, start, TIMEOUT_MS);

  // Acknowledge: the trackpoint pulls data low for one clock, then releases both lines
  if (ok) {
    while (digitalRead(PS2_DATA_PIN) == HIGH && (millis() - start < TIMEOUT_MS));
    while (digitalRead(PS2_CLOCK_PIN) == HIGH && (millis() - start < TIMEOUT_MS));
    ok = (digitalRead(PS2_DATA_PIN) == LOW);
    while ((digitalRead(PS2_DATA_PIN) == LOW || digitalRead(PS2_CLOCK_PIN) == LOW) && (millis() - start < TIMEOUT_MS));
  }

  // Always leave both lines released
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  return ok;
}

// Trackpoint Hardware Interrupt Service Routine (ISR) - runs on each falling clock edge
void clkInterruptSR() {
  unsigned long now = micros();

  // A long gap means any partly received byte is stale (lost clock edge): start over
  if ((now - lastBitTime) > 3000) {
    bitCount = 0;
    bitBuffer = 0;
  }
  lastBitTime = now;

  bitBuffer |= ((uint16_t)digitalRead(PS2_DATA_PIN) << bitCount);
  bitCount++;
  if (bitCount < 11) return;

  // A complete 11-bit frame has arrived
  uint16_t frame = bitBuffer;
  bitCount = 0;
  bitBuffer = 0;

  // Check start bit = 0, stop bit = 1 and odd parity over the 8 data bits + parity bit.
  // A bad frame means a bit was lost or corrupted: drop the partial packet and resync.
  bool startBitOK = ((frame & 0x001) == 0);
  bool stopBitOK  = ((frame & 0x400) != 0);
  bool parityOK   = (__builtin_popcount((frame >> 1) & 0x1FF) & 1) == 1;
  if (!startBitOK || !stopBitOK || !parityOK) {
    tpIndex = 0;
    return;
  }
  uint8_t data = (frame >> 1) & 0xFF;

  // Bytes of one packet arrive back to back. A gap means a new packet is starting.
  if ((now - lastByteTime) > PS2_PACKET_GAP_US) tpIndex = 0;
  lastByteTime = now;

  // The first byte of every packet has bit 3 set. Skip bytes until one does (resync).
  if (tpIndex == 0 && (data & 0x08) == 0) return;

  tpPacket[tpIndex++] = data;
  if (tpIndex < 3) return;
  tpIndex = 0;

  // Complete packet: status, X, Y
  uint8_t status = tpPacket[0];
  tpButtonsLatched |= (status & 0x07) & ~tpButtons;  // remember new presses
  tpButtons = status & 0x07;
  if (status & 0xC0) return;  // X or Y overflow: movement is not valid, keep the buttons only

  int dx = tpPacket[1];
  int dy = tpPacket[2];
  if (status & 0x10) dx -= 256;  // 9-bit two's complement sign bits
  if (status & 0x20) dy -= 256;
  tpAccumX += dx;
  tpAccumY += dy;
}

// Reset the trackpoint and turn on streaming. The interrupt must not be attached.
bool initTrackpoint() {
  bool ok = writePS2Byte(0xFF); // Reset (replies FA, then AA 00 when its self test is done)
  delay(600);
  ok = writePS2Byte(0xF4) && ok; // Enable Streaming
  delay(20);
  return ok;
}

// If the trackpoint resets itself (power glitch, cable bump), it sends AA 00 and then
// stops streaming. With the packet logic above, those two bytes sit in tpPacket waiting
// for a third byte that never comes. Detect that and turn streaming back on.
void checkTrackpointReset() {
  noInterrupts();
  bool resetSeen = (tpIndex == 2 && tpPacket[0] == 0xAA && tpPacket[1] == 0x00 &&
                    (micros() - lastByteTime) > 50000);
  interrupts();
  if (!resetSeen) return;

  detachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN));
  writePS2Byte(0xF4);           // Enable Streaming
  noInterrupts();
  tpIndex = 0;
  bitCount = 0;
  bitBuffer = 0;
  interrupts();
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
}

// Send a button change over BLE (one report per change)
void updateMouseButton(uint8_t buttons, uint8_t mask, uint8_t bleButton) {
  if ((buttons & mask) == (sentButtons & mask)) return;
  if (buttons & mask) MouseBLE.press(bleButton); else MouseBLE.release(bleButton);
}

void setup() {
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the col gpio's inputs with pullups
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); // Make the row gpio's inputs with pullups (same as the other BLE keyboards)
  }

  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);

  // --- Initialize Bluetooth Core Profiles ---
  // KeyboardBLE.begin starts one BLE HID device with both keyboard and mouse reports
  KeyboardBLE.begin("T61 BLE Controller", "Lenovo Mod");
  MouseBLE.begin(); // Inherits the shared BLE device link from the Keyboard setup
  delay(5000);

  // --- Wake Trackpoint --- (if it doesn't answer, the keyboard still works)
  initTrackpoint();

  // Attach the Trackpoint clock interrupt
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
}

void loop() {
  yield(); // keep background core processes like BLE running so they don't timeout

  // ==================== PART 1: KEYBOARD MATRIX SCANNING ====================
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT); // make the selected row pin an output
    digitalWrite(rowPins[r], LOW); // drive the row pin low
    delayMicroseconds(20); // give it some time before sampling due to weak 50K pullups in RP2040. 10usec is marginal

    for (int c = 0; c < NUM_COLS; c++) { // read each column, looking for a low that indicates the key is pushed
      bool currentPressed = (digitalRead(colPins[c]) == LOW);

      if (currentPressed != lastKeyState[r][c]) {
        uint8_t standardKey = keyMap[r][c];

        if (standardKey != 0) {
          if (standardKey == TOKEN_MUTE_KEY) {
            if (currentPressed) KeyboardBLE.consumerPress(KEY_MUTE); else KeyboardBLE.consumerRelease();
          }
          else if (standardKey == TOKEN_VOL_UP) {
            if (currentPressed) KeyboardBLE.consumerPress(KEY_VOLUME_INCREMENT); else KeyboardBLE.consumerRelease();
          }
          else if (standardKey == TOKEN_VOL_DOWN) {
            if (currentPressed) KeyboardBLE.consumerPress(KEY_VOLUME_DECREMENT); else KeyboardBLE.consumerRelease();
          }
          else if (standardKey == TOKEN_FN) {
            fnHeld = currentPressed; // Fn itself sends nothing
          }
          else {
            uint16_t media = fnHeld ? fnMediaCode(r, c) : 0; // Fn + arrow = media key
            if (currentPressed) {
              if (media != 0) {
                KeyboardBLE.consumerPress(media);
                sentAsMedia[r][c] = true;
              } else {
                KeyboardBLE.press(standardKey);
              }
            } else {
              // Release the key the same way it was pressed, even if Fn was let go first
              if (sentAsMedia[r][c]) {
                KeyboardBLE.consumerRelease();
                sentAsMedia[r][c] = false;
              } else {
                KeyboardBLE.release(standardKey);
              }
            }
          }
        }
        lastKeyState[r][c] = currentPressed;
      }
    }
    pinMode(rowPins[r], INPUT_PULLUP); // return row that was driven low to an input with pullup
  }

  // ==================== PART 2: TRACKPOINT ====================
  // Take the movement collected by the interrupt since the last send
  noInterrupts();
  int32_t moveX = tpAccumX;
  int32_t moveY = tpAccumY;
  // Buttons held now, plus any pressed and already released since the last loop.
  // A latched click is sent as a press now and a release on the next loop.
  uint8_t buttons = tpButtons | tpButtonsLatched;
  tpButtonsLatched = 0;
  // A BLE mouse report holds -127 to +127. Send that much now and keep the rest for next time.
  if (moveX > 127) moveX = 127;
  if (moveX < -127) moveX = -127;
  if (moveY > 127) moveY = 127;
  if (moveY < -127) moveY = -127;
  tpAccumX -= moveX;
  tpAccumY -= moveY;
  interrupts();

  // Button changes (left and right, same as before)
  if (buttons != sentButtons) {
    updateMouseButton(buttons, 0x01, MOUSE_LEFT);
    updateMouseButton(buttons, 0x02, MOUSE_RIGHT);
    sentButtons = buttons;
  }

  // Send Move Report via Mouse profile (PS/2 Y is up-positive, the PC is down-positive)
  if (moveX != 0 || moveY != 0) {
    MouseBLE.move(moveX, -moveY, 0);
  }

  checkTrackpointReset();

  delay(5); // Baseline loop stabilization debounce
}
