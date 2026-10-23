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

#include <Arduino.h>
#include <KeyboardBT.h>
#include <Wire.h>  // Included for native hardware I2C support
extern "C" void gap_set_local_name(const char *name);

// Hardware I2C Configuration
#define I2C_SDA 2
#define I2C_SCL 3
#define MCP_ADDR 0x20
#define I2C_FREQ 400000  // Target frequency of 400KHz

// Row and Column count
#define NUM_ROWS 16
#define NUM_COLS 9

// The Pico GPIO bits are denoted as 0, 1, 4 – 22, 26 – 28. GPIO 2 and 3 are used for I2C and 23 – 25 are for internal Pico use. 
// The port expander first 8 bits are GPA pins and denoted as 100 - 107. The second 8 bits are GPB pins and denoted as 108 - 115.  

// Row pins stored in an array
const uint8_t row_pins[NUM_ROWS] = {
  101, 20, 112, 9, 102, 10, 103, 19, 111, 11, 110, 18, 105, 14, 107, 15
};
// Column pins stored in an array
const uint8_t col_pins[NUM_COLS] = {
  100, 7, 113, 8, 17, 109, 16, 106, 108
};
// key names stored in a two dimensional array. 
const uint8_t keymap[NUM_ROWS][NUM_COLS] = {
//  GPA0=100    7           GPB5=113    8             17              GPB1=109        16                GPA6=106          GPB0=108        
  { 'q',        '1',        'a',        'z',          KEY_F11,        KEY_BACKSPACE,  KEY_F12,          KEY_F1,           0 },              //GPA1=101
  { 'w',        '2',        's',        'x',          KEY_ESC,        KEY_TAB,        '`',              KEY_F2,           0 },              //20
  { 'e',        '3',        'd',        'c',          0,              0,              0,                KEY_F3,           0 },              //GPB4=112
  { 'y',        '6',        'h',        'n',          '/',            ' ',            '=',              KEY_F6,           0 },              //9
  { 'r',        '4',        'f',        'v',          0,              0,              ']',              KEY_F4,           0 },              //GPA2=102
  { 0,          0,          0,          0,            KEY_RETURN,     0,              0,                KEY_RIGHT_SHIFT,  0 },              //10
  { 0,          0,          0,          KEY_LEFT_GUI, '0',            0,              KEY_RIGHT_GUI,    0,                0 },              //GPA3=103
  { 't',        '5',        'g',        'b',          '-',            0,              '[',              KEY_F5,           0 },              //19
  { 0,          0,          0,          0,            KEY_F10,        KEY_RIGHT_ALT,  0,                0,                0 },              //GPB3=111
  { 0,          0,          0,          0,            'p',            KEY_CAPS_LOCK,  0,                0,                0 },              //11
  { 'u',        '7',        'j',        'm',          ';',            '\'',           0,                KEY_F7,           0 },              //GPB2=110
  { 'i',        '8',        'k',        ',',          0,              KEY_LEFT_ARROW, KEY_RIGHT_ARROW,  KEY_F8,           0 },              //18
  { 'o',        '9',        'l',        '.',          '\\',           KEY_DOWN_ARROW, KEY_UP_ARROW,     KEY_F9,           0 },              //GPA5=105
  { 0,          0,          0,          0,            0,              0,              0,                0,                KEY_LEFT_SHIFT }, //14
  { 0,          0,          0,          0,            0,              0,              0,                0,                KEY_LEFT_ALT },   //GPA7=107
  { 0,          0,          0,          0,            0,              0,              0,                0,                KEY_LEFT_CTRL }   //15
};
//
bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

bool fn_pressed = false; // Tracks if the Fn key is currently held down

// --- HARDWARE I2C DRIVER FUNCTIONS ---

// Replaces bb_write_mcp_16bit
bool hw_write_mcp_16bit(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(MCP_ADDR);
  Wire.write(reg);               // Set the register pointer
  Wire.write(value & 0xFF);      // Send low byte (GPIOA)
  Wire.write((value >> 8) & 0xFF); // Send high byte (GPIOB)
  
  if (Wire.endTransmission() != 0) {
    return false; // Return false if NACK or bus error occurs
  }
  return true;
}

// Replaces bb_read_mcp_pins
uint16_t hw_read_mcp_pins() {
  Wire.beginTransmission(MCP_ADDR);
  Wire.write(0x12); // Pointer register set to GPIOA bank
  
  // endTransmission(false) sends a Repeated Start, keeping the bus active
  if (Wire.endTransmission(false) != 0) {
    return 0xFFFF; // Return all 1's if error
  }
  
  // Request 2 bytes from the port expander (GPIOA and GPIOB)
  uint8_t bytesReceived = Wire.requestFrom(MCP_ADDR, (uint8_t)2);
  if (bytesReceived != 2) {
    return 0xFFFF;
  }
  
  uint8_t gpioa = Wire.read(); // Read GPIOA
  uint8_t gpiob = Wire.read(); // Read GPIOB
  
  return (gpiob << 8) | gpioa; // Combine into a 16-bit value
}

void setup() {
  // PHASE 1: Boot the Bluetooth Radio and lock system multiplexers first
  // This allows the background CYW43439 firmware loop to safely stabilize.
  gap_set_local_name("MacBook Pro HID"); // Set the local name via global GAP call
  KeyboardBT.begin();
  
  // Give the radio firmware stack a solid window to completely spin up
  delay(500);

  // PHASE 2: Allow the MCP23017 chip time to reach stable operating voltages
  delay(100);

  // PHASE 3: Clear the physical lines of any residual slave lockups
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(I2C_SCL, OUTPUT);
  for (int i = 0; i < 9; i++) {
    digitalWrite(I2C_SCL, LOW);
    delayMicroseconds(5);
    digitalWrite(I2C_SCL, HIGH);
    delayMicroseconds(5);
  }

  // PHASE 4: Safe Hardware I2C Engine Startup
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();
  Wire.setClock(I2C_FREQ);
  Wire.setTimeout(50); // Prevent blocking lockups if a physical line glitches

  // PHASE 5: Standard matrix input setups
  for (int i = 0; i < NUM_ROWS; i++) {
    if (row_pins[i] < 30) pinMode(row_pins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < NUM_COLS; i++) {
    if (col_pins[i] < 30) pinMode(col_pins[i], INPUT_PULLUP); // Fixed variable indexing context safely
  }

  // PHASE 6: Expander Configurations (Buffered with small timing gaps)
  delay(10);
  hw_write_mcp_16bit(0x00, 0xFFFF); // Define Port Expander pins as Inputs
  delay(10);
  hw_write_mcp_16bit(0x0C, 0xFFFF); // Turn on internal pull-up resistors
  delay(10);
}

void loop() {
  // TEMPORARY PAIRING WINDOW:
  // For the first 30 seconds after powering up, freeze the matrix scan
  // so the background Bluetooth stack has 100% of the CPU to pair cleanly.
  if (millis() < 30000) {
    yield();
    delay(10);
    return; // Exit loop early, skipping I2C reads completely
  }

  // 1. Prioritize background Bluetooth pairing requests at the start of every sweep
  yield(); 

  // 2. Single Pass Matrix Scan
  for (int r = 0; r < NUM_ROWS; r++) {
    
    // Switch row pin to output and pull low
    if (row_pins[r] <= 30) {
      pinMode(row_pins[r], OUTPUT);
      digitalWrite(row_pins[r], LOW);
    } 
    else {
      uint8_t pinNum = row_pins[r] - 100;
      uint16_t ioDirMask = 0xFFFF & ~(1 << pinNum);
      hw_write_mcp_16bit(0x00, ioDirMask); 
      
      uint16_t latchMask = 0xFFFF & ~(1 << pinNum);
      hw_write_mcp_16bit(0x14, latchMask);
    }

    // Use a tiny 20 microsecond settling delay (never blocks the BT stack)
    delayMicroseconds(20); 

    // Capture the column states from the port expander
    uint16_t mcpInputSnapshot = hw_read_mcp_pins();
    // 3. Loop through columns to process key states
    for (int c = 0; c < NUM_COLS; c++) { 
      bool currentPressed = (col_pins[c] <= 30) 
        ? (digitalRead(col_pins[c]) == LOW) // Native Pico GPIO
        : ((mcpInputSnapshot & (1 << (col_pins[c] - 100))) == 0); // I2C expander

      if (currentPressed != lastKeyState[r][c]) {
        // --- INTERCEPT FN KEY (Row 7, Col 5) ---
        if (r == 7 && c == 5) {
          fn_pressed = currentPressed; 
          lastKeyState[r][c] = currentPressed;
          continue; 
        }

        // --- INTERCEPT MEDIA KEYS ---
        // Handles Fn-combined media shortcuts for volume control.
        if (fn_pressed && currentPressed && ((r == 8 && c == 4) || (r == 0 && c == 4) || (r == 0 && c == 6))) {
          uint16_t mKey = (r == 8 && c == 4) ? KEY_MUTE : (r == 0 && c == 4) ? KEY_VOLUME_DECREMENT : KEY_VOLUME_INCREMENT;
          KeyboardBT.consumerPress(mKey);
          delay(20);
          KeyboardBT.consumerRelease();
        }
        if ((r == 8 && c == 4) || (r == 0 && c == 4) || (r == 0 && c == 6)) {
          lastKeyState[r][c] = currentPressed;
          continue;
        }
        uint8_t standardKey = keymap[r][c]; 
        if (standardKey != 0) { 
          if (currentPressed) { 
            KeyboardBT.press(standardKey); 
          } else { 
            KeyboardBT.release(standardKey); 
          }
        }
        lastKeyState[r][c] = currentPressed; 
      }
    }
    // Reset row pins back to neutral
    if (row_pins[r] <= 30) {
      pinMode(row_pins[r], INPUT_PULLUP); 
    } else {
      hw_write_mcp_16bit(0x00, 0xFFFF); 
      hw_write_mcp_16bit(0x0C, 0xFFFF); 
    }
  }
  // Safe 5ms delay that lets the BT stack process pairing requests
  delay(5); 
}
