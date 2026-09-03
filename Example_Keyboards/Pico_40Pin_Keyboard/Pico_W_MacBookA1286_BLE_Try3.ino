#include <Arduino.h>
#include <KeyboardBLE.h> 

// Low-Level I2C Configuration
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
  101, 20, 112, 9, 102, 10, 103, 19, 111, 11, 110, 18, 105, 106, 107, 15
};
// Column pins stored in an array
const uint8_t col_pins[NUM_COLS] = {
  100, 7, 113, 8, 17, 109, 16, 106, 112
};
// key names stored in a two dimensional array. 
const uint8_t keymap[NUM_ROWS][NUM_COLS] = {
//  GPA0=100    7           GPB5=113    8             17              GPB1=109        16                GPA6=106          GPB4=112     
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
  { 0,          0,          0,          0,            0,              0,              0,                0,                KEY_LEFT_SHIFT }, //GPA6=106
  { 0,          0,          0,          0,            0,              0,              0,                0,                KEY_LEFT_ALT },   //GPA7=107
  { 0,          0,          0,          0,            0,              0,              0,                0,                KEY_LEFT_CTRL }   //15
};
//
bool lastKeyState[NUM_ROWS][NUM_COLS] = {false}; // this creates a 2D array filled with zero's. It will be used to 
// keep track of when a key has been pressed so it only sends the key once and not constantly until released. 

// --- BIT-BANG OPEN-DRAIN DRIVER FUNCTIONS ---
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
    if (row_pins[i] < 30) pinMode(row_pins[i], INPUT_PULLUP);
  }

  for (int i = 0; i < NUM_COLS; i++) {
    if (col_pins[i] < 30) pinMode(col_pins[i], INPUT_PULLUP);
  }
// Set all 16 pins (GPIOA & GPIOB) of the port expander as inputs by writing 0xFFFF to the IODIR register (0x00)
  bb_write_mcp_16bit(0x00, 0xFFFF);
// Enable internal pull-up resistors on all 16 pins by writing 0xFFFF to the GPPU register (0x0C)
  bb_write_mcp_16bit(0x0C, 0xFFFF);
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
// make only the current row_pins[r] an outout and drive it low
      uint8_t pinNum = row_pins[r] - 100; // Translate 100-115 to bit index 0-15
      uint16_t ioDirMask = 0xFFFF & ~(1 << pinNum); // Set targeted pin to 0 (Output), others stay 1 (Input)
      uint16_t latchMask = 0xFFFF & ~(1 << pinNum); // Drive targeted pin to 0 (LOW)
      bb_write_mcp_16bit(0x00, ioDirMask); // Update directions
      bb_write_mcp_16bit(0x14, latchMask); // Write OLAT (0x14) to output the LOW state
    }
    delayMicroseconds(20); // 10us sometimes causes multiple keys b/c slow rise time from 50K ohm pullups in RP2040

    uint16_t mcpInputSnapshot = bb_read_mcp_pins(); //read all 16 bits of the port expander and save it for the loop below

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
      bb_write_mcp_16bit(0x00, 0xFFFF); // Set all 16 pins (GPIOA & GPIOB) of the port expander as inputs
      bb_write_mcp_16bit(0x0C, 0xFFFF); // Enable internal pull-up resistors on all 16 pins by writing 0xFFFF to the GPPU register (0x0C)     
    } 
  }
  delay(8); // overall 8 msec keyboard scan rate is slow enough to eliminate any chance of key bounce
}







