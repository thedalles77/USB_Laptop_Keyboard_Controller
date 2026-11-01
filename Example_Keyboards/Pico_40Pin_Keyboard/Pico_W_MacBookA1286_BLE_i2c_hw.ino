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
#include <Wire.h>

// I2C Configuration (Pico I2C1 hardware)
#define I2C_SDA 2
#define I2C_SCL 3
#define MCP_ADDR 0x20

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

// --- HARDWARE I2C FUNCTIONS FOR THE MCP23018 PORT EXPANDER ---
// GP2 (SDA) and GP3 (SCL) are connected to the Pico's I2C1 hardware block, so
// the Wire1 object must be used. (Wire is I2C0, which can't use GP2/GP3 - the
// Pico stops with a fatal error if you try.)
// In master mode the Wire library polls the I2C hardware. It does not use
// interrupts or DMA, so it does not compete with the Bluetooth radio.

// MCP23018 registers (IOCON.BANK = 0, so each A register is followed by its B register)
#define MCP_IODIRA 0x00  // direction: 1 = input, 0 = output
#define MCP_GPPUA  0x0C  // pull-up resistors: 1 = on
#define MCP_GPIOA  0x12  // pin read
#define MCP_OLATA  0x14  // output latch

// function to load the low 8 bits of a 16 bit value into the location pointed to by the address register "reg"
// and then load the high 8 bits into the next higher register (via auto increment in the port expander).
bool mcp_write_16bit(uint8_t reg, uint16_t value) {
  Wire1.beginTransmission(MCP_ADDR);  // start sequence + port expander address with write bit
  Wire1.write(reg);                   // register address pointer
  Wire1.write(value & 0xFF);          // low 8 bits (GPA register)
  Wire1.write((value >> 8) & 0xFF);   // high 8 bits (GPB register, auto increment)
  return (Wire1.endTransmission() == 0); // stop sequence, 0 = all bytes were ACKed
}

// Function to read 16 bits from the port expander (GPIOA then GPIOB)
uint16_t mcp_read_pins() {
  Wire1.beginTransmission(MCP_ADDR);
  Wire1.write(MCP_GPIOA);                                  // point to the GPIOA register
  if (Wire1.endTransmission() != 0) return 0xFFFF;         // all 1's if NACK
  if (Wire1.requestFrom(MCP_ADDR, 2) != 2) return 0xFFFF;  // read GPIOA and GPIOB, all 1's if it failed
  uint8_t gpioa = Wire1.read();
  uint8_t gpiob = Wire1.read();
  return ((uint16_t)gpiob << 8) | gpioa; // shift the b-byte left by 8 and "or" it with the a-byte
}

void setup() {
  // Start the I2C1 hardware on GP2/GP3 at 400KHz (4.7K pullups R1 & R2 on the board)
  Wire1.setSDA(I2C_SDA);
  Wire1.setSCL(I2C_SCL);
  Wire1.begin();
  Wire1.setClock(400000);
  // If a transfer ever fails, give up after 5 msec and reset the I2C block instead
  // of stalling the keyboard scan (the default wait is much longer).
  Wire1.setTimeout(5, true);

// make all Pico row and column GPIO's inputs with pullups
  for (int i = 0; i < NUM_ROWS; i++) {
    if (row_pins[i] < 30) pinMode(row_pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NUM_COLS; i++) {
    if (col_pins[i] < 30) pinMode(col_pins[i], INPUT_PULLUP);
  }
// Set all 16 output latches (OLAT) to 0. They stay 0, so a port expander row is driven low just by
// making it an output, and released by making it an input again (one I2C write each).
  mcp_write_16bit(MCP_OLATA, 0x0000);
// Set all 16 pins (GPIOA & GPIOB) of the port expander as inputs by writing 0xFFFF to the IODIR register (0x00)
  mcp_write_16bit(MCP_IODIRA, 0xFFFF);
// Enable internal pull-up resistors on all 16 pins by writing 0xFFFF to the GPPU register (0x0C)
  mcp_write_16bit(MCP_GPPUA, 0xFFFF);
// Power Up the CYW43439 Radio Chip. Configures the BLE HID Keyboard Profile. Set the Device and Model Names
  KeyboardBLE.begin("MacBook Pro HID", "Apple Wireless Mod");
}

void loop() {
  yield(); //keep background core processes like BLE running so they don't timeout

  // Single Pass Matrix Scan drives each row low, one at a time and reads all the columns with pull ups 
  for (int r = 0; r < NUM_ROWS; r++) {
    if (row_pins[r] <=30) { // check if current row pin is the Pico or the Port Expander
      pinMode(row_pins[r], OUTPUT); // make the selected Pico row pin an output (it was an input)
      digitalWrite(row_pins[r], LOW); // drive the Pico row pin low
    } 
    else {
// make only the current row_pins[r] an output. Its output latch is already 0 so it is driven low.
      uint8_t pinNum = row_pins[r] - 100; // Translate 100-115 to bit index 0-15
      uint16_t ioDirMask = 0xFFFF & ~(1 << pinNum); // Set targeted pin to 0 (Output), others stay 1 (Input)
      mcp_write_16bit(MCP_IODIRA, ioDirMask); // Update directions
    }
    delayMicroseconds(20); // 10us sometimes causes multiple keys b/c slow rise time from 50K ohm pullups in RP2040

    uint16_t mcpInputSnapshot = mcp_read_pins(); //read all 16 bits of the port expander and save it for the loop below

    for (int c = 0; c < NUM_COLS; c++) { // read each column, looking for a low that indicates the key is pushed
      bool currentPressed = LOW;
      if (col_pins[c] <=30) {  // check if column pin is a Pico GPIO
        currentPressed = (digitalRead(col_pins[c]) == LOW); // read Pico GPIO pin
      }
      else { //pull out the one bit from the 16 that you just read from the port expander and check if it's low
        uint8_t pinNum = col_pins[c] - 100; // Translate 100-115 to bit index 0-15
        currentPressed = ((mcpInputSnapshot & (1 << pinNum)) == 0); // 0 is pressed     
      }     

      if (currentPressed != lastKeyState[r][c]) { // check if current key state is not the same as the last loop
        // --- INTERCEPT FN KEY (Row 7, Col 5) ---
        if (r == 7 && c == 5) {
          fn_pressed = currentPressed; // Set to true if pressed, false if released
          lastKeyState[r][c] = currentPressed;
          continue; // Skip standard keyboard processing for this key slot
        }
        // --- INTERCEPT F10 KEY (Row 8, Col 4) WHEN FN IS HELD --------MUTE
        if (r == 8 && c == 4 && fn_pressed) {
          if (currentPressed) { // Only trigger on the downward press
            KeyboardBLE.consumerPress(KEY_MUTE);
            delay(20); // Give the BLE radio stack time to safely transmit the state change
            KeyboardBLE.consumerRelease(); // Releases all active consumer/media key presses
          }
          lastKeyState[r][c] = currentPressed;
          continue; // Skip sending standard KEY_F10
        }
        // --- INTERCEPT F11 KEY (Row 0, Col 4) WHEN FN IS HELD --------VOLUME DECREASE
        if (r == 0 && c == 4 && fn_pressed) {
          if (currentPressed) { // Only trigger on the downward press
            KeyboardBLE.consumerPress(KEY_VOLUME_DECREMENT);
            delay(20); // Give the BLE radio stack time to safely transmit the state change
            KeyboardBLE.consumerRelease(); // Releases all active consumer/media key presses
          }
          lastKeyState[r][c] = currentPressed;
          continue; // Skip sending standard KEY_F10
        }
        // --- INTERCEPT F12 KEY (Row 0, Col 6) WHEN FN IS HELD --------VOLUME INCREASE
        if (r == 0 && c == 6 && fn_pressed) {
          if (currentPressed) { // Only trigger on the downward press
            KeyboardBLE.consumerPress(KEY_VOLUME_INCREMENT);
            delay(20); // Give the BLE radio stack time to safely transmit the state change
            KeyboardBLE.consumerRelease(); // Releases all active consumer/media key presses
          }
          lastKeyState[r][c] = currentPressed;
          continue; // Skip sending standard KEY_F10
        }
        // --- STANDARD MATRIX KEY PROCESSING ---
        uint8_t standardKey = keymap[r][c]; // save the name of the newly pressed (or released) key from the matrix
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
    if (row_pins[r] <=30) { 
      pinMode(row_pins[r], INPUT_PULLUP); // return Pico row that was driven low to an input with pullup
    }
    else {
      mcp_write_16bit(MCP_IODIRA, 0xFFFF); // Set all 16 pins (GPIOA & GPIOB) of the port expander back to inputs
    } 
  }
  delay(8); // overall 8 msec keyboard scan rate is slow enough to eliminate any chance of key bounce
}
