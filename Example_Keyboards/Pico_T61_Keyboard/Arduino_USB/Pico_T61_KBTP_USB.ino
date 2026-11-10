/*
    Copyright 2026 Frank Adams

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

This code makes a Lenovo T61 laptop keyboard and trackpoint work as a USB keyboard
and mouse using a Raspberry Pi Pico (or Pico W, Pico 2, Pico 2 W) and the Adafruit
TinyUSB library. The mute, volume up and volume down keys are sent as USB media
(consumer control) keys. The trackpoint is a PS/2 device on GP0 (data) and GP1 (clock),
read with a clock interrupt.
Arduino IDE settings: Tools > USB Stack = Adafruit TinyUSB
*/

#include <Adafruit_TinyUSB.h>

// HID report IDs: normal keys, media (consumer control) keys, and the trackpoint mouse
enum {
  RID_KEYBOARD = 1,
  RID_CONSUMER_CONTROL,
  RID_MOUSE
};

// HID report descriptor: keyboard + consumer control (mute, volume) + mouse
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE))
};

// Create the TinyUSB HID object instance
Adafruit_USBD_HID usb_hid;

const int NUM_ROWS = 16; // number of row signals
const int NUM_COLS = 8; // number of column signals

// column and row GPIO numbers attached to the column and row pins of the keyboard
const int colPins[NUM_COLS] = {11, 22, 10, 21, 9, 20, 8, 7};
const int rowPins[NUM_ROWS] = {27, 26, 17, 14, 12, 19, 18, 15, 13, 6, 16, 5, 28, 2, 4, 3};

// Placeholder tokens for the media keys. They are sent as consumer control codes, not keyboard keys.
#define TOKEN_MUTE_KEY 0xFE
#define TOKEN_VOL_UP   0xFD
#define TOKEN_VOL_DOWN 0xFC

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  //  GP11                  GP22                  GP10              GP21                   GP9                GP20                GP8                    GP7
  { HID_KEY_GRAVE,        HID_KEY_1,            HID_KEY_Q,        HID_KEY_TAB,           HID_KEY_A,         HID_KEY_ESCAPE,     HID_KEY_Z,             0                    },  // GP27
  { HID_KEY_F1,           HID_KEY_2,            HID_KEY_W,        HID_KEY_CAPS_LOCK,     HID_KEY_S,         0,                  HID_KEY_X,             0                    },  // GP26
  { HID_KEY_F2,           HID_KEY_3,            HID_KEY_E,        HID_KEY_F3,            HID_KEY_D,         HID_KEY_F4,         HID_KEY_C,             0                    },  // GP17
  { HID_KEY_5,            HID_KEY_4,            HID_KEY_R,        HID_KEY_T,             HID_KEY_F,         HID_KEY_G,          HID_KEY_V,             HID_KEY_B            },  // GP14
  { HID_KEY_6,            HID_KEY_7,            HID_KEY_U,        HID_KEY_Y,             HID_KEY_J,         HID_KEY_H,          HID_KEY_M,             HID_KEY_N            },  // GP12
  { HID_KEY_EQUAL,        HID_KEY_8,            HID_KEY_I,        HID_KEY_BRACKET_RIGHT, HID_KEY_K,         HID_KEY_F6,         HID_KEY_COMMA,         0                    },  // GP19
  { HID_KEY_F8,           HID_KEY_9,            HID_KEY_O,        HID_KEY_F7,            HID_KEY_L,         0,                  HID_KEY_PERIOD,        0                    },  // GP18
  { HID_KEY_MINUS,        HID_KEY_0,            HID_KEY_P,        HID_KEY_BRACKET_LEFT,  HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, 0,                     HID_KEY_SLASH        },  // GP15
  { HID_KEY_F9,           HID_KEY_F10,          0,                HID_KEY_BACKSPACE,     HID_KEY_BACKSLASH, HID_KEY_F5,         HID_KEY_ENTER,         HID_KEY_SPACE        },  // GP13
  { HID_KEY_INSERT,       HID_KEY_F12,          0,                0,                     0,                 0,                  0,                     HID_KEY_ARROW_RIGHT  },  // GP6
  { HID_KEY_DELETE,       HID_KEY_F11,          TOKEN_VOL_UP,     TOKEN_VOL_DOWN,        TOKEN_MUTE_KEY,    0,                  0,                     HID_KEY_ARROW_DOWN   },  // GP16
  { HID_KEY_PAGE_UP,      HID_KEY_PAGE_DOWN,    HID_KEY_GUI_LEFT, 0,                     0,                 0,                  0,                     0                    },  // GP5
  { HID_KEY_HOME,         HID_KEY_END,          0,                0,                     0,                 HID_KEY_ARROW_UP,   0,                     HID_KEY_ARROW_LEFT   },  // GP28
  { 0,                    HID_KEY_PRINT_SCREEN, 0,                0,                     0,                 HID_KEY_ALT_LEFT,   0,                     HID_KEY_ALT_RIGHT    },  // GP2
  { 0,                    0,                    0,                HID_KEY_SHIFT_LEFT,    0,                 0,                  HID_KEY_SHIFT_RIGHT,   0                    },  // GP4
  { HID_KEY_CONTROL_LEFT, 0,                    0,                0,                     0,                 0,                  HID_KEY_CONTROL_RIGHT, 0                    }   // GP3
};

// Current state of every key from the latest scan (true = pressed)
bool keyState[NUM_ROWS][NUM_COLS] = {false};

// Remember the last reports we transmitted so we can suppress duplicates
uint8_t lastModifier = 0;
uint8_t lastReport[6] = {0, 0, 0, 0, 0, 0};
uint16_t lastConsumer = 0;

// --- Trackpoint Configurations ---
const int PS2_DATA_PIN  = 0;
const int PS2_CLOCK_PIN = 1;

// Each PS/2 byte is 11 bits: start(0), 8 data bits (LSB first), odd parity, stop(1).
// A trackpoint movement packet is 3 bytes: status, X movement, Y movement.
// The interrupt routine below decodes complete packets and ADDS the movement to
// tpAccumX / tpAccumY. The loop sends the accumulated movement whenever USB is
// ready, so no movement is lost and the interrupt can never overrun a buffer.
volatile uint16_t bitBuffer = 0;        // bits of the byte being received
volatile int bitCount = 0;              // number of bits received for this byte
volatile unsigned long lastBitTime = 0; // micros() of the last clock edge
volatile unsigned long lastByteTime = 0;// micros() of the last complete byte

volatile uint8_t tpPacket[3];           // packet being assembled
volatile int tpIndex = 0;               // next byte position in tpPacket

volatile int32_t tpAccumX = 0;          // movement not yet sent over USB
volatile int32_t tpAccumY = 0;
volatile uint8_t tpButtons = 0;         // latest button bits: 0x01 left, 0x02 right, 0x04 middle
volatile uint8_t tpButtonsLatched = 0;  // buttons pressed since the loop last looked, so a quick
                                        // click is never missed between reports

uint8_t sentButtons = 0;                // button bits last sent over USB

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

// Wait (up to 20 msec) for USB to be ready for the next report
static bool waitUsbReady() {
  unsigned long start = millis();
  while (!usb_hid.ready()) {
    if (millis() - start > 20) return false;
    delay(1);
  }
  return true;
}

void setup() {
  // Start the USB HID device first. The Pico core starts USB before setup()
  // runs, so the HID interface must be added as early as possible.
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("T61 USB Keyboard");
  usb_hid.begin();

  // If the PC already enumerated the Pico without the keyboard, reconnect so it sees it
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the col gpio's inputs with pullups
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); // Make the row gpio's inputs with pullups
  }
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);

  // Wait until the PC has finished connecting the keyboard and mouse
  while (!TinyUSBDevice.mounted()) delay(1);

  // --- Wake Trackpoint --- (each PS/2 write has a timeout, so this can't hang)
  initTrackpoint();

  // Attach the Trackpoint clock interrupt
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
}

// ---------------------------------------------------------------------
// Loop:
//   PART 1 - Keyboard: scans the full 16x8 matrix, builds the keyboard report
//            and the media (consumer) report, and sends whichever changed.
//   PART 2 - Trackpoint: sends the buttons and the movement collected by the
//            interrupt since the last mouse report.
// ---------------------------------------------------------------------
void loop() {
  // ==================== PART 1: KEYBOARD ====================
  uint8_t  modifierByte      = 0;
  uint8_t  keycodeReport[6]  = {0, 0, 0, 0, 0, 0};
  int      keyCount          = 0;
  bool     rolloverExceeded  = false;
  uint16_t consumerCode      = 0;

  // 1) Full-matrix scan --------------------------------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);                      // drive this row low
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(20);                            // settle (weak 50K pullups)

    for (int c = 0; c < NUM_COLS; c++) {
      keyState[r][c] = (digitalRead(colPins[c]) == LOW);
    }

    pinMode(rowPins[r], INPUT_PULLUP);                // release the row
  }

  // 2) Build the reports from all held keys --------------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      if (!keyState[r][c]) continue;
      uint8_t key = keyMap[r][c];
      if (key == 0) continue;                         // empty position

      if (key == TOKEN_MUTE_KEY || key == TOKEN_VOL_UP || key == TOKEN_VOL_DOWN) {
        if (consumerCode == 0) {                      // one media key at a time
          if (key == TOKEN_MUTE_KEY)    consumerCode = HID_USAGE_CONSUMER_MUTE;
          else if (key == TOKEN_VOL_UP) consumerCode = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
          else                          consumerCode = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        }
      } else if (key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT) {
        // Modifier: fold into the modifier byte
        modifierByte |= (uint8_t)(1u << (key - HID_KEY_CONTROL_LEFT));
      } else if (keyCount < 6) {
        keycodeReport[keyCount++] = key;              // Normal key (6KRO)
      } else {
        rolloverExceeded = true;
      }
    }
  }

  // HID spec: fill all 6 slots with ErrorRollOver (0x01) when more than 6 normal keys are held.
  if (rolloverExceeded) {
    for (int i = 0; i < 6; i++) keycodeReport[i] = 0x01;
  }

  // 3) Send reports that changed ---------------------------------------------
  // A report is only marked as sent if USB accepted it, so a change is never lost.
  bool reportDiffers = (modifierByte != lastModifier);
  for (int i = 0; i < 6 && !reportDiffers; i++) {
    if (keycodeReport[i] != lastReport[i]) reportDiffers = true;
  }
  if (reportDiffers && waitUsbReady()) {
    if (usb_hid.keyboardReport(RID_KEYBOARD, modifierByte, keycodeReport)) {
      lastModifier = modifierByte;
      for (int i = 0; i < 6; i++) lastReport[i] = keycodeReport[i];
    }
  }

  // Media key press (code) or release (0)
  if (consumerCode != lastConsumer && waitUsbReady()) {
    if (usb_hid.sendReport16(RID_CONSUMER_CONTROL, consumerCode)) {
      lastConsumer = consumerCode;
    }
  }

  // ==================== PART 2: TRACKPOINT ====================
  // Take the movement and buttons collected by the interrupt
  noInterrupts();
  int32_t moveX = tpAccumX;
  int32_t moveY = tpAccumY;
  uint8_t latched = tpButtonsLatched;
  // Buttons held now, plus any pressed and already released since the last report.
  // A latched click is sent as a press now and a release in the next report.
  uint8_t buttons = (tpButtons | latched) & 0x03;   // left and right buttons
  interrupts();

  // A USB mouse report holds -127 to +127. Send that much now and keep the rest for next time.
  if (moveX > 127) moveX = 127;
  if (moveX < -127) moveX = -127;
  if (moveY > 127) moveY = 127;
  if (moveY < -127) moveY = -127;

  // Send one report with the buttons and the movement (PS/2 Y is up-positive, the PC is down-positive).
  // Only remove what was sent once USB has accepted the report, so nothing is lost.
  if ((buttons != sentButtons || moveX != 0 || moveY != 0) && waitUsbReady()) {
    if (usb_hid.mouseReport(RID_MOUSE, buttons, (int8_t)moveX, (int8_t)(-moveY), 0, 0)) {
      noInterrupts();
      tpAccumX -= moveX;
      tpAccumY -= moveY;
      tpButtonsLatched &= ~latched;
      interrupts();
      sentButtons = buttons;
    }
  }

  checkTrackpointReset();

  delay(8); // ~125 Hz scan rate - slow enough to eliminate key bounce
}
