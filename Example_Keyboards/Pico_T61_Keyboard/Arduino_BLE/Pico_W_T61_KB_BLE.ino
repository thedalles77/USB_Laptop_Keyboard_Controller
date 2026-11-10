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

This code makes a Lenovo T61 laptop keyboard work as a Bluetooth (BLE) keyboard using a
Raspberry Pi Pico W and the Earle Philhower Arduino Pico core.
Holding Fn turns the arrow keys into Stop (Up), Play/Pause (Down), Previous (Left)
and Next (Right).
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
#define TOKEN_FN       0xFB   // Fn key: sends nothing itself, turns the arrows into media keys

// Base Keyboard Map. A zero indicates no key at that position.
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

bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

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

void setup() {
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the col gpio's inputs with pullups
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); // Make the row gpio's inputs with pullups (same as the other BLE keyboards)
  }

  KeyboardBLE.begin("T61 BLE Keyboard", "Lenovo Mod"); // "T61 BLE Keyboard" defines the Device Name that
  // computers, phones, and tablets will display in their Bluetooth settings when scanning for new devices.
  // "Lenovo Mod" is the manufacturer name (aka vendor name Lenovo with Mod to indicate it's been modified).
  delay(5000); // This delay gives the Raspberry Pi Pico W’s wireless hardware stack enough time to boot up
  // and initialize the Bluetooth stack before the matrix scanning starts.
}

void loop() {
  yield(); // keep background core processes like BLE running so they don't timeout

  // Single Pass Matrix Scan drives each row low, one at a time and reads all the columns
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT); // make the selected row pin an output
    digitalWrite(rowPins[r], LOW); // drive the row pin low 
    delayMicroseconds(20); // give it some time before sampling due to weak 50K pullups in RP2040. 10usec is marginal

    for (int c = 0; c < NUM_COLS; c++) { // read each column, looking for a low that indicates the key is pushed
      bool currentPressed = (digitalRead(colPins[c]) == LOW);

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
              KeyboardBLE.consumerPress(KEY_VOLUME_INCREMENT); // Triggers Volume Increase (increment)
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
          else if (standardKey == TOKEN_FN) {
            fnHeld = currentPressed; // remember if Fn is held. Fn itself sends nothing
          }
          // Process the standard laptop keys normally (Fn + arrow = media key)
          else {
            uint16_t media = fnHeld ? fnMediaCode(r, c) : 0; // non zero if this is an arrow key with Fn held
            if (currentPressed) { // high indicates the key was just pressed
              if (media != 0) {
                KeyboardBLE.consumerPress(media); // send the media key press
                sentAsMedia[r][c] = true; // remember to release it as a media key
              } else {
                KeyboardBLE.press(standardKey); // send the key press
              }
            } else { // key was just released. Release it the same way it was pressed, even if Fn was let go first
              if (sentAsMedia[r][c]) {
                KeyboardBLE.consumerRelease(); // send the media key release
                sentAsMedia[r][c] = false;
              } else {
                KeyboardBLE.release(standardKey); // send the key release
              }
            }
          }
        }
        
        lastKeyState[r][c] = currentPressed; // save state of the key to the last key state array for next loop
      }
    }
    pinMode(rowPins[r], INPUT_PULLUP); // return row that was driven low to an input with pullup
  }
  delay(8); // overall 8 msec keyboard scan rate is slow enough to eliminate any chance of key bounce
}
