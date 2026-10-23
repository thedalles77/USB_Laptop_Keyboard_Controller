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
#include <KeyboardBLE.h>
#include <Wire.h>  // Included for native hardware I2C support

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
  // 1. Give the MCP23017 chip 100ms to fully boot up after power is applied
  delay(100);

  // 2. Clear a stuck I2C bus manually before initializing the hardware module
  pinMode(I2C_SDA, INPUT_PULLUP);
  pinMode(I2C_SCL, OUTPUT);
  for (int i = 0; i < 9; i++) {
    digitalWrite(I2C_SCL, LOW);
    delayMicroseconds(5);
    digitalWrite(I2C_SCL, HIGH);
    delayMicroseconds(5);
  }

  // 3. Configure and start the native RP2040 hardware I2C engine
  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();
  Wire.setClock(I2C_FREQ);
  Wire.setTimeout(50); // Abort transaction instead of freezing if a spike happens

  // 4. Configure native Pico row and column pins as inputs with pullups
  for (int i = 0; i < NUM_ROWS; i++) {
    if (row_pins[i] < 30) pinMode(row_pins[i], INPUT_PULLUP);
  }
  for (int i = 0; i < NUM_COLS; i++) {
    if (col_pins[i] < 30) pinMode(col_pins[i], INPUT_PULLUP);
  }

  // 5. Safely configure MCP23017 pins with brief stabilization delays
  delay(10);
  hw_write_mcp_16bit(0x00, 0xFFFF); // Set pins as inputs (IODIR register)
  delay(10);
  hw_write_mcp_16bit(0x0C, 0xFFFF); // Enable internal pull-ups (GPPU register)
  delay(10);

  // 6. Initialize the BLE HID Keyboard profile
  KeyboardBLE.begin("MacBook Pro HID", "Apple Wireless Mod");
}


void loop() {
  // CRITICAL: Feeds background Bluetooth/Wi-Fi core watchdogs to prevent internal radio crashes
  yield(); 

  // Single Pass Matrix Scan
  for (int r = 0; r < NUM_ROWS; r++) {
    
    // --- COMPLETE ROW ACTIVATION LOGIC ---
    if (row_pins[r] <= 30) {
      // If it's a native Pico pin, switch to output and pull low
      pinMode(row_pins[r], OUTPUT);
      digitalWrite(row_pins[r], LOW);
    } 
    else {
      // If it's an expander pin, set its IODIR bit to 0 (Output)
      uint8_t pinNum = row_pins[r] - 100;
      uint16_t ioDirMask = 0xFFFF & ~(1 << pinNum);
      hw_write_mcp_16bit(0x00, ioDirMask); // Write to IODIR
      
      // Pull that specific output latch pin to LOW state (0x14 = OLAT register)
      uint16_t latchMask = 0xFFFF & ~(1 << pinNum);
      hw_write_mcp_16bit(0x14, latchMask);
    }

    delayMicroseconds(20); // Wait for signals to settle stably

    // Capture the 16-bit state update via our hardware I2C reader wrapper
    uint16_t mcpInputSnapshot = hw_read_mcp_pins(); 

    // Loop through columns
    for (int c = 0; c < NUM_COLS; c++) { 
      bool currentPressed = LOW;
      
      if (col_pins[c] <= 30) { 
        // Read native Pico GPIO pin
        currentPressed = (digitalRead(col_pins[c]) == LOW); 
      }
      else { 
        // Extract the target bit from our hardware I2C snapshot
        uint8_t pinNum = col_pins[c] - 100; 
        currentPressed = ((mcpInputSnapshot & (1 << pinNum)) == 0); 
      }

      if (currentPressed != lastKeyState[r][c]) {
        // --- INTERCEPT FN KEY (Row 7, Col 5) ---
        if (r == 7 && c == 5) {
          fn_pressed = currentPressed; 
          lastKeyState[r][c] = currentPressed;
          continue; 
        }

        // --- INTERCEPT MEDIA KEYS ---
        if (fn_pressed && currentPressed && ((r == 8 && c == 4) || (r == 0 && c == 4) || (r == 0 && c == 6))) {
          uint16_t key = (r == 8 && c == 4) ? KEY_MUTE : (r == 0 && c == 4) ? KEY_VOLUME_DECREMENT : KEY_VOLUME_INCREMENT;
          KeyboardBLE.consumerPress(key);
          delay(20);
          KeyboardBLE.consumerRelease();
        }
        if ((r == 8 && c == 4) || (r == 0 && c == 4) || (r == 0 && c == 6)) {
          lastKeyState[r][c] = currentPressed; 
          continue;
        }

        // --- STANDARD MATRIX KEY PROCESSING ---
        uint8_t standardKey = keymap[r][c]; 
        if (standardKey != 0) { 
          if (currentPressed) { 
            KeyboardBLE.press(standardKey); 
          } else { 
            KeyboardBLE.release(standardKey); 
          }
        }
        lastKeyState[r][c] = currentPressed; 
      }
    }

    // Reset row pins back to high-impedance inputs with pull-ups
    if (row_pins[r] <= 30) {
      pinMode(row_pins[r], INPUT_PULLUP); 
    }
    else {
      hw_write_mcp_16bit(0x00, 0xFFFF); 
      hw_write_mcp_16bit(0x0C, 0xFFFF); 
    }
  }

  delay(8); // Overall scan delay for key debouncing
}
