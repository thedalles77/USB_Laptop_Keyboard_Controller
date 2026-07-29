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
#include <KeyboardBLE.h>

const int NUM_ROWS = 16; // number of row signals
const int NUM_COLS = 8; // number of column signals

// column and row GPIO numbers attached to the column and row pins of the keyboard 
const int colPins[NUM_COLS] = {11, 22, 10, 21, 9, 20, 8, 7};
const int rowPins[NUM_ROWS] = {27, 26, 17, 14, 12, 19, 18, 15, 13, 6, 16, 5, 28, 2, 4, 3};

// Custom placeholder tokens for the volume media keys that must be treated differently than a normal key
#define TOKEN_MUTE_KEY 0xFE
#define TOKEN_VOL_UP   0xFD
#define TOKEN_VOL_DOWN 0xFC

// Base Keyboard Map. A zero indicates no key at that position.
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

bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

void setup() {
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); // Make the row gpio's inputs with pullups
  }
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], OUTPUT);
    digitalWrite(colPins[c], HIGH); // Make the column gpio's outputs, all driven high to start.
  }

  KeyboardBLE.begin("T61 BLE Keyboard", "Lenovo Mod"); // "T61 BLE Keyboard" defines the Device Name that
  // computers, phones, and tablets will display in their Bluetooth settings when scanning for new devices.
  // "Lenovo Mod" is the manufacturer name (aka vendor name Lenovo with Mod to indicate it's been modified).
  delay(5000); // This delay gives the Raspberry Pi Pico W’s wireless hardware stack enough time to boot up
  // and initialize the Bluetooth stack before the matrix scanning starts.
}

void loop() {
  // Single Pass Matrix Scan drives each column low, one at a time and reads all the rows
  for (int c = 0; c < NUM_COLS; c++) {
    digitalWrite(colPins[c], LOW); // drive the column low
    delayMicroseconds(10); // give it some time to settle out

    for (int r = 0; r < NUM_ROWS; r++) { // read each row, looking for a low that indicates the key is pushed
      bool currentPressed = (digitalRead(rowPins[r]) == LOW);

      if (currentPressed != lastKeyState[r][c]) { // check if current key state is not the same as the last loop
        uint8_t standardKey = keyMap[r][c]; // save the name of the newly pressed (or released) key from the matrix

        if (standardKey != 0) { // non zero key code in the array is selected
          // Intercept the special media tokens
          if (standardKey == TOKEN_MUTE_KEY) {
            if (currentPressed) { // mute was just pressed
              KeyboardBLE.consumerPress(KEY_MUTE); // send the Mute key press
            } else { // mute was just released
              KeyboardBLE.consumerRelease(); // send the release (for all consumer keys)
            }
          } 
          else if (standardKey == TOKEN_VOL_UP) {
            if (currentPressed) { // volume up was just pressed
              KeyboardBLE.consumerPress(KEY_VOLUME); // Triggers Volume Increase
            } else { // volume up was just released
              KeyboardBLE.consumerRelease(); // send the release (for all consumer keys)
            }
          } 
          else if (standardKey == TOKEN_VOL_DOWN) {
            if (currentPressed) { // volume down was just pressed
              KeyboardBLE.consumerPress(KEY_VOLUME_DECREMENT); // Triggers Volume Decrease (decrement)
            } else { // volume down was just released
              KeyboardBLE.consumerRelease(); // send the release (for all consumer keys)
            }
          }
          // Process the standard laptop keys normally
          else {
            if (currentPressed) { // high indicates the key was just pressed
              KeyboardBLE.press(standardKey); // send the key press
            } else { // key was just released
              KeyboardBLE.release(standardKey); // send the key release
            }
          }
        }
        
        lastKeyState[r][c] = currentPressed; // save state of the key to the last key state array for next loop
      }
    }
    digitalWrite(colPins[c], HIGH); // return column that was just read to high 
  }
  delay(8); // overall 8 msec keyboard scan rate is slow enough to eliminate any chance of key bounce
}
