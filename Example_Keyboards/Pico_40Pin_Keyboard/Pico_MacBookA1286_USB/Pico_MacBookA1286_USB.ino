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


// Low-Level I2C Configuration
#define I2C_SDA 2
#define I2C_SCL 3
#define MCP_ADDR 0x20

// Row and Column count
const int NUM_ROWS = 16; // number of row signals
const int NUM_COLS = 9; // number of column signals

// The Pico GPIO bits are denoted as 0, 1, 4 – 22, 26 – 28. GPIO 2 and 3 are used for I2C and 23 – 25 are for internal Pico use. 
// The port expander first 8 bits are GPA pins and denoted as 100 - 107. The second 8 bits are GPB pins and denoted as 108 - 115.  

// Row pins stored in an array
const int rowPins[NUM_ROWS] = {101, 20, 112, 9, 102, 10, 103, 19, 111, 11, 110, 18, 105, 14, 107, 15};
// Column pins stored in an array
const int colPins[NUM_COLS] = {100, 7, 113, 8, 17, 109, 16, 106, 108};

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
//  GPA0=100    7           GPB5=113    8                 17                  GPB1=109            16                      GPA6=106              GPB0=108        
  { HID_KEY_Q,  HID_KEY_1,  HID_KEY_A,  HID_KEY_Z,        HID_KEY_F11,        HID_KEY_BACKSPACE,  HID_KEY_F12,            HID_KEY_F1,           0 },                    //GPA1=101
  { HID_KEY_W,  HID_KEY_2,  HID_KEY_S,  HID_KEY_X,        HID_KEY_ESCAPE,     HID_KEY_TAB,        HID_KEY_GRAVE,          HID_KEY_F2,           0 },                    //20
  { HID_KEY_E,  HID_KEY_3,  HID_KEY_D,  HID_KEY_C,        0,                  0,                  0,                      HID_KEY_F3,           0 },                    //GPB4=112
  { HID_KEY_Y,  HID_KEY_6,  HID_KEY_H,  HID_KEY_N,        HID_KEY_SLASH,      HID_KEY_SPACE,      HID_KEY_EQUAL,          HID_KEY_F6,           0 },                    //9
  { HID_KEY_R,  HID_KEY_4,  HID_KEY_F,  HID_KEY_V,        0,                  0,                  HID_KEY_BRACKET_RIGHT,  HID_KEY_F4,           0 },                    //GPA2=102
  { 0,          0,          0,          0,                HID_KEY_ENTER,      0,                  0,                      HID_KEY_SHIFT_RIGHT,  0 },                    //10
  { 0,          0,          0,          HID_KEY_GUI_LEFT, HID_KEY_0,          0,                  HID_KEY_GUI_RIGHT,      0,                    0 },                    //GPA3=103
  { HID_KEY_T,  HID_KEY_5,  HID_KEY_G,  HID_KEY_B,        HID_KEY_MINUS,      0,                  HID_KEY_BRACKET_LEFT,   HID_KEY_F5,           0 },                    //19
  { 0,          0,          0,          0,                HID_KEY_F10,        HID_KEY_ALT_RIGHT,  0,                      0,                    0 },                    //GPB3=111
  { 0,          0,          0,          0,                HID_KEY_P,          HID_KEY_CAPS_LOCK,  0,                      0,                    0 },                    //11
  { HID_KEY_U,  HID_KEY_7,  HID_KEY_J,  HID_KEY_M,        HID_KEY_SEMICOLON,  HID_KEY_APOSTROPHE, 0,                      HID_KEY_F7,           0 },                    //GPB2=110
  { HID_KEY_I,  HID_KEY_8,  HID_KEY_K,  HID_KEY_COMMA,    0,                  HID_KEY_ARROW_LEFT, HID_KEY_ARROW_RIGHT,    HID_KEY_F8,           0 },                    //18
  { HID_KEY_O,  HID_KEY_9,  HID_KEY_L,  HID_KEY_PERIOD,   HID_KEY_BACKSLASH,  HID_KEY_ARROW_DOWN, HID_KEY_ARROW_UP,       HID_KEY_F9,           0 },                    //GPA5=105
  { 0,          0,          0,          0,                0,                  0,                  0,                      0,                    HID_KEY_SHIFT_LEFT },   //14
  { 0,          0,          0,          0,                0,                  0,                  0,                      0,                    HID_KEY_ALT_LEFT },     //GPA7=107
  { 0,          0,          0,          0,                0,                  0,                  0,                      0,                    HID_KEY_CONTROL_LEFT }  //15
};
//
bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

// --- I2C BIT-BANG OPEN-DRAIN DRIVER FUNCTIONS ---
void i2c_delay() { 
  delayMicroseconds(4); // sets the delay value
}

// scl_high function lets the i2c clock go high via pullups and checks if the port expander is pulling it low (clock stretching).
// The i2c clock signal is normally open drain (when high) so the Pico GPIO pin (which is not open drain) is floated by making it an input. 
// The Pico's internal 50K pullup is turned on and is in parallel with the external 4.7K pullup on the board to give about 4.3K ohms. 
bool scl_high() {
  pinMode(I2C_SCL, INPUT_PULLUP); // the i2c clock line is made an input with a pullup
  unsigned long timeout = micros(); // Captures the current timestamp in microseconds and stores it in a variable named timeout
  while (digitalRead(I2C_SCL) == LOW) { // loop as long as the clock is being held low by the port expander
    // But if 500 microseconds pass and SCL is still LOW, it timed out (failed)
    if (micros() - timeout > 500) {
      return false; // return of false is a timeout
    }
  }
    return true; // If SCL went HIGH, the port expander is not clock stretching and the function can return with true (passed)
}

void scl_low() { // function to drive the i2c clock low
  pinMode(I2C_SCL, OUTPUT); 
  digitalWrite(I2C_SCL, LOW); 
}

void sda_high() { // function to float the i2c data line and let the 4.7K & 50K pull the signal high
  pinMode(I2C_SDA, INPUT_PULLUP); 
}

void sda_low() {  // function to drive the i2c data low
  pinMode(I2C_SDA, OUTPUT); 
  digitalWrite(I2C_SDA, LOW); 
}

void i2c_start() { // function to create the i2c start sequence
  sda_high(); 
  scl_high(); 
  i2c_delay(); 
  sda_low(); 
  i2c_delay(); 
  scl_low(); 
  i2c_delay(); 
}

void i2c_stop() { // function to create the i2c stop sequence
  sda_low(); 
  scl_high(); 
  i2c_delay(); 
  sda_high(); 
  i2c_delay(); 
}

bool i2c_write_byte(uint8_t byte) { // function send 8 bits over i2c to port expander
  for (int i = 0; i < 8; i++) { // loop 8 times for sending each bit in the byte
    if (byte & 0x80) sda_high(); else sda_low(); // mask the msb and send it 
    i2c_delay(); scl_high(); i2c_delay(); scl_low(); // toggle the clk high and then low
    byte <<= 1; // shift the data left 1 bit for the next loop
  }
  sda_high(); i2c_delay(); scl_high(); i2c_delay(); // finish with the data and clock high
  bool ack = (digitalRead(I2C_SDA) == LOW); // read ack. Port Expander should hold data line low.
  scl_low(); i2c_delay(); // finish with the clock low
  return ack;
}

uint8_t i2c_read_byte(bool ack) { // function to read 8 bits from the port expander over i2c
// The Ack bit in i2c is active low. The caller sends ack low (true) if there are more bytes to read after this one.
// If Ack is high (false), this is the last byte that will be read and the port expander must release the bus when done. 
  sda_high(); // start with data high
  uint8_t byte = 0; // start with the holding byte all zero's, then fill each bit.
  for (int i = 0; i < 8; i++) { // loop 8 times to fill each bit in the byte
    byte <<= 1; // shift left 1 bit
    scl_high(); i2c_delay(); // send clock high
    if (digitalRead(I2C_SDA) == HIGH) { // check if data signal is high
      byte = byte | 0x01; // put a 1 in the lsb
    }
    scl_low(); i2c_delay(); // send clock low
  }
  if (ack) sda_low(); else sda_high(); // the caller of this function decides to send ack as a 1 or a 0.
  scl_high(); i2c_delay(); scl_low(); sda_high(); i2c_delay(); // send clock high, then low and data high
  return byte;
}

// function to load the low 8 bits of a 16 bit value into the location pointed to by the address register "reg"
// and then load the high 8 bits into the next higher register (via auto increment in the port expander).
bool bb_write_mcp_16bit(uint8_t reg, uint16_t value) {
  i2c_start(); // start sequence tells port expander to listen for its address

  // i2c has a 7 bit address in d7 to d1. d0 is the r/w* bit. 
  // Shift 0x20 left 1 bit so d0 is low for a write and send the 8 bits on the bus. 
  // Port expander should return an ACK but if it doesn't, 
  // then send a stop sequence and return a 0 for fail.
  if (!i2c_write_byte(MCP_ADDR << 1)) { i2c_stop(); return false; } 

  // port expander has many internal registers accessed by an address pointer at the "reg" location.
  // load the address of the register that will be writen to.  
  if (!i2c_write_byte(reg)) { i2c_stop(); return false; } // return a zero if NACK

  // write the low 8 bits of the 16 bit data value to the location pointed to by "reg". 
  if (!i2c_write_byte(value & 0xFF)) { i2c_stop(); return false; } // return a zero if NACK

  // write the high 8 bits of the 16 bit value. Port expander auto increments the "reg" pointer address.
  if (!i2c_write_byte((value >> 8) & 0xFF)) { i2c_stop(); return false; } // return a zero if NACK

  i2c_stop(); // send stop sequence
  return true; // return a 1 because all went well
}

// Function to read 16 bits from the port expander
uint16_t bb_read_mcp_pins() {
  i2c_start(); // start sequence tells port expander to listen for its address

// Shift 0x20 left 1 bit so d0 is low for a write and send it on the bus.
  if (!i2c_write_byte(MCP_ADDR << 1)) { i2c_stop(); return 0xFFFF; } // return all 1's if NACK

// load pointer register with 0x12 which points to the GPIOA bank
  if (!i2c_write_byte(0x12)) { i2c_stop(); return 0xFFFF; } // return all 1's if NACK

  i2c_start(); // repeated start so now we can switch over to reading registers

// shift port expander address left 1 bit and or in a 1 at the r/w* position for a read 
  if (!i2c_write_byte((MCP_ADDR << 1) | 1)) { i2c_stop(); return 0xFFFF; } // return all 1's if NACK

// read a byte and store it in gpioa.  
  uint8_t gpioa = i2c_read_byte(true); // true sends ack low to not release the bus

// read a byte and store it in gpiob. The port expander auto incremented to point to 0x13
  uint8_t gpiob = i2c_read_byte(false); // false sends ack high to release the bus

  i2c_stop(); // stop sequence

  return (gpiob << 8) | gpioa; // shift the b-byte left by 8 and "or" it with the a-byte for 16 bit return value
}

void setup() {
  pinMode(I2C_SDA, INPUT_PULLUP); // start out with i2c data and clock as floating with pullups
  pinMode(I2C_SCL, INPUT_PULLUP);
// make all Pico row and column GPIO's inputs with pullups
  for (int i = 0; i < NUM_ROWS; i++) {
    if (rowPins[i] < 30) pinMode(rowPins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NUM_COLS; i++) {
    if (colPins[i] < 30) pinMode(colPins[i], INPUT_PULLUP);
  }
// Set all 16 pins (GPIOA & GPIOB) of the port expander as inputs by writing 0xFFFF to the IODIR register (0x00)
  bb_write_mcp_16bit(0x00, 0xFFFF);
// Enable internal pull-up resistors on all 16 pins by writing 0xFFFF to the GPPU register (0x0C)
  bb_write_mcp_16bit(0x0C, 0xFFFF);

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
    if (rowPins[r] <=30) { // check if current row pin is the Pico or the Port Expander
      pinMode(rowPins[r], OUTPUT); // make the selected Pico row pin an output (it was an input)
      digitalWrite(rowPins[r], LOW); // drive the Pico row pin low
    } 
    else {
      // on the port expander, make only the current row_pins[r] an outout and drive it low
      uint8_t pinNum = rowPins[r] - 100; // Translate 100-115 to bit index 0-15
      uint16_t ioDirMask = 0xFFFF & ~(1 << pinNum); // Set targeted pin to 0 (Output), others stay 1 (Input)
      uint16_t latchMask = 0xFFFF & ~(1 << pinNum); // Drive targeted pin to 0 (LOW)
      bb_write_mcp_16bit(0x00, ioDirMask); // Update directions
      bb_write_mcp_16bit(0x14, latchMask); // Write OLAT (0x14) to output the LOW state
    }
    delayMicroseconds(20); 

    uint16_t mcpInputSnapshot = bb_read_mcp_pins(); //read all 16 bits of the port expander and save it for the loop below

    for (int c = 0; c < NUM_COLS; c++) { // read each column, looking for a low that indicates the key is pushed
      bool currentPressed = LOW;
      if (colPins[c] <=30) {  // check if column pin is a Pico GPIO
        currentPressed = (digitalRead(colPins[c]) == LOW); // read Pico GPIO pin
      }
      else { //pull out the one bit from the 16 that you just read from the port expander and check if it's low
        uint8_t pinNum = colPins[c] - 100; // Translate 100-115 to bit index 0-15
        currentPressed = ((mcpInputSnapshot & (1 << pinNum)) == 0); // 0 is pressed     
      }     

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
    if (rowPins[r] <=30) { 
      pinMode(rowPins[r], INPUT_PULLUP); // return Pico row that was driven low to an input with pullup
    }
    else {
      bb_write_mcp_16bit(0x00, 0xFFFF); // Set all 16 pins (GPIOA & GPIOB) of the port expander as inputs
      bb_write_mcp_16bit(0x0C, 0xFFFF); // Enable internal pull-up resistors on all 16 pins by writing 0xFFFF to the GPPU register (0x0C)     
    } 
  // 2. Only send a single aggregated HID update if a key state changed
    if (stateChanged && usb_hid.ready()) {
      usb_hid.keyboardReport(0, modifierByte, keycodeReport);
    }
  }
  delay(8); // overall 8 msec keyboard scan rate is slow enough to eliminate any chance of key bounce
}







