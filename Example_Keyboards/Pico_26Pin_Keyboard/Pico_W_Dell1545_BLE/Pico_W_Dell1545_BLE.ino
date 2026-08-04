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

const int NUM_ROWS = 17; // number of row signals
const int NUM_COLS = 8; // number of column signals

// column and row GPIO numbers attached to the column and row pins of the keyboard 
const int colPins[NUM_COLS] = {1, 2, 3, 4, 5, 6, 27, 28};
const int rowPins[NUM_ROWS] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26};

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { 0,              0,              KEY_PAGE_DOWN,  KEY_LEFT_GUI,     0,              0,            0,              KEY_PAGE_UP },
  { 0,              KEY_RIGHT_CTRL, 0,              0,                0,              0,            KEY_LEFT_CTRL,  0 },
  { KEY_RIGHT_ALT,  0,              0,              KEY_PRINT_SCREEN, 0,              KEY_LEFT_ALT, 0,              0 },
  { 0,              'z',            'a',            '1',              KEY_TAB,        KEY_ESC,      '`',            'q' },
  { 0,              'c',            'd',            '3',              KEY_F3,         KEY_F4,       KEY_F2,         'e' },
  { ' ',            KEY_RETURN,     '\\',           KEY_F10,          KEY_BACKSPACE,  KEY_F5,       KEY_F9,         0 },
  { 0,              ',',            'k',            '8',              ']',            KEY_F6,       '=',            'i' },
  { 0,              '.',            'l',            '9',              KEY_F7,         0,            KEY_F8,         'o' },
  { KEY_LEFT_ARROW, 0,              0,              KEY_END,          0,              KEY_UP_ARROW, KEY_HOME,       0 },
  { KEY_RIGHT_ARROW,0,              0,              KEY_F12,          0,              0,            KEY_INSERT,     0 },
  { KEY_DOWN_ARROW, 0,              0,              KEY_F11,          0,              0,            KEY_DELETE,     0 }, 
  { '/',            0,              ';',            '0',              '[',            '\'',         '-',            'p' },
  { 'n',            'm',            'j',            '7',              'y',            'h',          '6',            'u' },
  { 'b',            'v',            'f',            '4',              't',            'g',          '5',            'r' },
  { 0,              'x',            's',            '2',              KEY_CAPS_LOCK,  0,            KEY_F1,         'w' },
  { 0,              KEY_RIGHT_SHIFT,0,              0,                KEY_LEFT_SHIFT, 0,            0,              0 },
  { 0,              0,              0,              0,                0,              0,            0,              0 }
};
//
bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

void setup() {
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); // Make the row gpio's inputs with pullups
  }
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT); // Make the column gpio's float by making them inputs w/o pullups to start.
  }

  KeyboardBLE.begin("Dell1545 Keyboard", "Dell Mod"); // This defines the Device Name that
  // computers, phones, and tablets will display in their Bluetooth settings when scanning for new devices.
  // "Dell Mod" is the manufacturer name (aka vendor name Dell with Mod to indicate it's been modified).
  delay(5000); // This delay gives the Raspberry Pi Pico W’s wireless hardware stack enough time to boot up
  // and initialize the Bluetooth stack before the matrix scanning starts.
}

void loop() {
  // Single Pass Matrix Scan drives each column low, one at a time and reads all the rows
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], OUTPUT); // make the selected column pin an output (it was floating as an input)
    digitalWrite(colPins[c], LOW); // drive the column pin low 
    delayMicroseconds(10); // give it some time to settle out

    for (int r = 0; r < NUM_ROWS; r++) { // read each row, looking for a low that indicates the key is pushed
      bool currentPressed = (digitalRead(rowPins[r]) == LOW);

      if (currentPressed != lastKeyState[r][c]) { // check if current key state is not the same as the last loop
        uint8_t standardKey = keyMap[r][c]; // save the name of the newly pressed (or released) key from the matrix

        if (standardKey != 0) { // non zero key code in the array is selected
          if (currentPressed) { // high indicates the key was just pressed
            KeyboardBLE.press(standardKey); // send the key press
          } else { // key was just released
            KeyboardBLE.release(standardKey); // send the key release
          }
        }
        
        lastKeyState[r][c] = currentPressed; // save state of the key to the last key state array for next loop
      }
    }
    pinMode(colPins[c], INPUT); // return column that was driven low to an input so it floats 
  }
  delay(8); // overall 8 msec keyboard scan rate is slow enough to eliminate any chance of key bounce
}
