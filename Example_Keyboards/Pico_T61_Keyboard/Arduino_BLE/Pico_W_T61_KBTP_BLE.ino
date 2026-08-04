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

const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { '`',          '1',          'q',          KEY_TAB,      'a',          KEY_ESC,       'z',          0 },
  { KEY_F1,       '2',          'w',          KEY_CAPS_LOCK,'s',          0,             'x',          0 },
  { KEY_F2,       '3',          'e',          KEY_F3,       'd',          KEY_F4,        'c',          0 },
  { '5',          '4',          'r',          't',          'f',          'g',           'v',          'b' },
  { '6',          '7',          'u',          'y',          'j',          'h',           'm',          'n' },
  { '=',          '8',          'i',          ']',          'k',          KEY_F6,        ',',          0 },
  { KEY_F8,       '9',          'o',          KEY_F7,       'l',          0,             '.',          0 },
  { '-',          '0',          'p',          '[',          ';',          '\'',          0,            '/' },
  { KEY_F9,       KEY_F10,      0,            KEY_BACKSPACE,'\\',         KEY_F5,        KEY_RETURN,   ' ' },
  { KEY_INSERT,   KEY_F12,      0,            0,            0,            0,             0,            KEY_RIGHT_ARROW },
  { KEY_DELETE,   KEY_F11,      TOKEN_VOL_UP, TOKEN_VOL_DOWN, TOKEN_MUTE_KEY, 0,         0,            KEY_DOWN_ARROW }, 
  { KEY_PAGE_UP,  KEY_PAGE_DOWN,KEY_LEFT_GUI,  0,            0,            0,             0,            0 },
  { KEY_HOME,     KEY_END,      0,            0,            0,            KEY_UP_ARROW,  0,            KEY_LEFT_ARROW },
  { 0,            KEY_PRINT_SCREEN,0,         0,            0,            KEY_LEFT_ALT,  0,            KEY_RIGHT_ALT },
  { 0,            0,            0,            KEY_LEFT_SHIFT,0,           0,             KEY_RIGHT_SHIFT,0 },
  { KEY_LEFT_CTRL,0,            0,            0,            0,            0,             KEY_RIGHT_CTRL,0 }
};

bool lastKeyState[NUM_ROWS][NUM_COLS] = {false};

// --- Trackpoint Configurations ---
const int PS2_DATA_PIN  = 0;
const int PS2_CLOCK_PIN = 1;

volatile uint16_t bitBuffer = 0;
volatile int bitCount = 0;
volatile bool packetReady = false;

uint8_t ps2Packet[3]; 
int packetIndex = 0;
volatile unsigned long lastBitTime = 0;

// Standard PS/2 Open-Drain Writing Implementation
void writePS2Byte(uint8_t value) {
  pinMode(PS2_CLOCK_PIN, OUTPUT);
  digitalWrite(PS2_CLOCK_PIN, LOW);
  delayMicroseconds(200); 

  pinMode(PS2_DATA_PIN, OUTPUT);
  digitalWrite(PS2_DATA_PIN, LOW);
  
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP); 

  uint8_t parity = 1; 
  
  for (int i = 0; i < 8; i++) {
    while (digitalRead(PS2_CLOCK_PIN) == HIGH); 
    uint8_t bit = (value >> i) & 0x01;
    
    if (bit == 0) {
      pinMode(PS2_DATA_PIN, OUTPUT);
      digitalWrite(PS2_DATA_PIN, LOW);
    } else {
      pinMode(PS2_DATA_PIN, INPUT_PULLUP); 
    }
    parity ^= bit;
    while (digitalRead(PS2_CLOCK_PIN) == LOW); 
  }

  while (digitalRead(PS2_CLOCK_PIN) == HIGH);
  if (parity == 0) {
    pinMode(PS2_DATA_PIN, OUTPUT);
    digitalWrite(PS2_DATA_PIN, LOW);
  } else {
    pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  }
  while (digitalRead(PS2_CLOCK_PIN) == LOW);

  while (digitalRead(PS2_CLOCK_PIN) == HIGH);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  while (digitalRead(PS2_CLOCK_PIN) == LOW);

  unsigned long timeout = millis();
  while (digitalRead(PS2_DATA_PIN) == HIGH && (millis() - timeout < 250)); 
  while (digitalRead(PS2_CLOCK_PIN) == HIGH && (millis() - timeout < 250)); 
  while ((digitalRead(PS2_DATA_PIN) == LOW || digitalRead(PS2_CLOCK_PIN) == LOW) && (millis() - timeout < 250)); 
}

// Trackpoint Hardware Interrupt Service Routine (ISR)
void clkInterruptSR() {
  unsigned long now = micros();
  
  if ((now - lastBitTime) > 3000) {
    bitCount = 0;
    bitBuffer = 0;
  }
  lastBitTime = now;

  uint8_t bitVal = digitalRead(PS2_DATA_PIN);
  bitBuffer |= (bitVal << bitCount);
  bitCount++;

  if (bitCount >= 11) {
    uint8_t completedByte = (bitBuffer >> 1) & 0xFF; 
    
    ps2Packet[packetIndex] = completedByte;
    packetIndex++;
    
    if (packetIndex >= 3) {
      packetReady = true; 
    }
    
    bitCount = 0;
    bitBuffer = 0;
  }
}

void setup() {
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the col gpio's inputs with pullups
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT); // Make the row gpio's float by making them inputs w/o pullups to start.
  }
  
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);

  // --- Initialize Bluetooth Core Profiles ---
  KeyboardBLE.begin("T61 BLE Controller", "Lenovo Mod");
  MouseBLE.begin(); // Inherits the shared BLE device link from the Keyboard setup
  delay(5000); 

  // --- Wake Trackpoint ---
  writePS2Byte(0xFF); // Reset
  delay(600);         
  writePS2Byte(0xF4); // Enable Streaming
  delay(200);

  // Attach the Trackpoint clock interrupt
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
}

void loop() {
  // ==================== PART 1: KEYBOARD MATRIX SCANNING ====================
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT); // make the selected row pin an output instead of floating as an input
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
            if (currentPressed) KeyboardBLE.consumerPress(KEY_VOLUME); else KeyboardBLE.consumerRelease();
          } 
          else if (standardKey == TOKEN_VOL_DOWN) {
            if (currentPressed) KeyboardBLE.consumerPress(KEY_VOLUME_DECREMENT); else KeyboardBLE.consumerRelease();
          }
          else {
            if (currentPressed) KeyboardBLE.press(standardKey); else KeyboardBLE.release(standardKey);
          }
        }
        lastKeyState[r][c] = currentPressed;
      }
    }
    pinMode(rowPins[r], INPUT); // return row that was driven low to an input so it floats 
  }

  // ==================== PART 2: TRACKPOINT PACKET PROCESSING ====================
  if (packetReady) {
    uint8_t statusByte = ps2Packet[0];
    int16_t movementX  = ps2Packet[1];
    int16_t movementY  = ps2Packet[2];

    if ((statusByte & 0x08) == 0) {
      noInterrupts();
      packetIndex = 0;
      packetReady = false;
      interrupts();
      return; // Skip bad packets
    }

    if (statusByte & 0x10) movementX |= 0xFF00; 
    if (statusByte & 0x20) movementY |= 0xFF00; 

    bool leftClick  = (statusByte & 0x01);
    bool rightClick = (statusByte & 0x02);

    // Left Click Handling
    if (leftClick) {
      if (!MouseBLE.isPressed(MOUSE_LEFT)) MouseBLE.press(MOUSE_LEFT);
    } else {
      if (MouseBLE.isPressed(MOUSE_LEFT)) MouseBLE.release(MOUSE_LEFT);
    }

    // Right Click Handling
    if (rightClick) {
      if (!MouseBLE.isPressed(MOUSE_RIGHT)) MouseBLE.press(MOUSE_RIGHT);
    } else {
      if (MouseBLE.isPressed(MOUSE_RIGHT)) MouseBLE.release(MOUSE_RIGHT);
    }

    // Send Move Report via Mouse profile
    if (movementX != 0 || movementY != 0) {
      MouseBLE.move(movementX, -movementY, 0);
    }

    noInterrupts();
    packetIndex = 0;
    packetReady = false;
    interrupts();
  }

  delay(5); // Baseline loop stabilization debounce
}
