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
*/
#include <Adafruit_TinyUSB.h>

// Define the HID report descriptor for a standard Boot Keyboard
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// Create the TinyUSB HID object instance
Adafruit_USBD_HID usb_hid;


const int NUM_ROWS = 17; // number of row signals
const int NUM_COLS = 8; // number of column signals

// column and row GPIO numbers attached to the column and row pins of the keyboard 
const int colPins[NUM_COLS] = {1, 2, 3, 4, 5, 6, 27, 28};
const int rowPins[NUM_ROWS] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26};

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { 0,                  0,                      HID_KEY_PAGE_DOWN,  HID_KEY_GUI_LEFT,     0,                      0,                  0,                    HID_KEY_PAGE_UP },
  { 0,                  HID_KEY_CONTROL_RIGHT,  0,                  0,                    0,                      0,                  HID_KEY_CONTROL_LEFT, 0 },
  { HID_KEY_ALT_RIGHT,  0,                      0,                  HID_KEY_PRINT_SCREEN, 0,                      HID_KEY_ALT_LEFT,   0,                    0 },
  { 0,                  HID_KEY_Z,              HID_KEY_A,          HID_KEY_1,            HID_KEY_TAB,            HID_KEY_ESCAPE,     HID_KEY_GRAVE,        HID_KEY_Q },
  { 0,                  HID_KEY_C,              HID_KEY_D,          HID_KEY_3,            HID_KEY_F3,             HID_KEY_F4,         HID_KEY_F2,           HID_KEY_E },
  { HID_KEY_SPACE,      HID_KEY_ENTER,          HID_KEY_BACKSLASH,  HID_KEY_F10,          HID_KEY_BACKSPACE,      HID_KEY_F5,         HID_KEY_F9,           0 },
  { 0,                  HID_KEY_COMMA,          HID_KEY_K,          HID_KEY_8,            HID_KEY_BRACKET_RIGHT,  HID_KEY_F6,         HID_KEY_EQUAL,        HID_KEY_I },
  { 0,                  HID_KEY_PERIOD,         HID_KEY_L,          HID_KEY_9,            HID_KEY_F7,             0,                  HID_KEY_F8,           HID_KEY_O },
  { HID_KEY_ARROW_LEFT, 0,                      0,                  HID_KEY_END,          0,                      HID_KEY_ARROW_UP,   HID_KEY_HOME,         0 },
  { HID_KEY_ARROW_RIGHT,0,                      0,                  HID_KEY_F12,          0,                      0,                  HID_KEY_INSERT,       0 },
  { HID_KEY_ARROW_DOWN, 0,                      0,                  HID_KEY_F11,          0,                      0,                  HID_KEY_DELETE,       0 }, 
  { HID_KEY_SLASH,      0,                      HID_KEY_SEMICOLON,  HID_KEY_0,            HID_KEY_BRACKET_LEFT,   HID_KEY_APOSTROPHE, HID_KEY_MINUS,        HID_KEY_P },
  { HID_KEY_N,          HID_KEY_M,              HID_KEY_J,          HID_KEY_7,            HID_KEY_Y,              HID_KEY_H,          HID_KEY_6,            HID_KEY_U },
  { HID_KEY_B,          HID_KEY_V,              HID_KEY_F,          HID_KEY_4,            HID_KEY_T,              HID_KEY_G,          HID_KEY_5,            HID_KEY_R },
  { 0,                  HID_KEY_X,              HID_KEY_S,          HID_KEY_2,            HID_KEY_CAPS_LOCK,      0,                  HID_KEY_F1,           HID_KEY_W },
  { 0,                  HID_KEY_SHIFT_RIGHT,    0,                  0,                    HID_KEY_SHIFT_LEFT,     0,                  0,                    0 },
  { 0,                  0,                      0,                  0,                    0,                      0,                  0,                    0 }
};
//
bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

void setup() {
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT); // Make the row gpio's float by making them inputs w/o pullups to start.
  }
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the column gpio's inputs with pullups
  }

  // Initialize USB HID with the layout descriptor
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

// Wait until the Pico is plugged in and recognized by the PC
  while ( !TinyUSBDevice.mounted() ) {
    delay(1);
  }
  delay(2000); // Short safety buffer for the host OS to finish enumeration
}

void loop() {
  uint8_t keycodeReport[6] = {0, 0, 0, 0, 0, 0};
  uint8_t modifierByte = 0;
  int keyCount = 0;
  bool stateChanged = false;

  // 1. Scan the whole matrix to find all active keys
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(20);

    for (int c = 0; c < NUM_COLS; c++) {
      bool currentPressed = (digitalRead(colPins[c]) == LOW);

      // Track if anything changed to minimize spamming reports
      if (currentPressed != lastKeyState[r][c]) {
        stateChanged = true;
        lastKeyState[r][c] = currentPressed;
      }

      // If key is held, evaluate its type
      if (currentPressed) {
        uint8_t key = keyMap[r][c];
        if (key != 0) {
          // Check if this key is a modifier flag
          if (key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT) {
            // Convert the keycode into its corresponding bitwise position (0-7)
            modifierByte |= (1 << (key - HID_KEY_CONTROL_LEFT));
          } else {
            // Add normal keys to our 6-position rollover report array
            if (keyCount < 6) {
              keycodeReport[keyCount] = key;
              keyCount++;
            }
          }
        }
      }
    }
    pinMode(rowPins[r], INPUT); // Let row pin float again
  }

  // 2. Only send a single aggregated HID update if a key state changed
  if (stateChanged && usb_hid.ready()) {
    usb_hid.keyboardReport(0, modifierByte, keycodeReport);
  }

  delay(8); // Stabilizing delay to prevent erratic contact bounce
}
