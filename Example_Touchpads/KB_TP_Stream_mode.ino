/* Copyright 2022 Frank Adams
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
// This software creates a USB keyboard and touchpad.
// The touchpad is PS/2 and the code uses "stream mode" instead of "remote mode".
// 
// define the pins on the Teensy used for the touchpad ps2 bus
#define TP_DATA 11 // Touchpad ps/2 data connected to Teensy I/O pin 
#define TP_CLK 12  // Touchpad ps/2 clock connected to Teensy I/O pin 
#define CAPS_LOCK_N 8 // driven low to turn on the CAPS LED
#define NUM_LOCK_N 13 // driven low to turn on the NUM LED (Note the LED on the Teensy 3.2 is also connected to this I/O and has opposite polarity)
//
#define MODIFIERKEY_FN 0x8f   // give the Fn key a HID code so it can be handled like any other key
//
const byte rows_max = 15; // sets the number of rows in the keyboard matrix
const byte cols_max = 8; // sets the number of columns in the keyboard matrix
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
//
int normal[rows_max][cols_max] = {
  {KEY_END,KEY_RIGHT,0,KEY_PRINTSCREEN,KEY_DELETE,KEY_HOME,KEY_PAGE_UP,KEY_PAGE_DOWN},
  {KEY_QUOTE,KEY_ENTER,KEY_PAUSE,0,KEY_EQUAL,KEY_BACKSPACE,KEY_RIGHT_BRACE,KEY_BACKSLASH},  
  {KEY_SLASH,KEY_UP,KEY_MINUS,KEY_F12,KEY_0,KEY_P,KEY_LEFT_BRACE,KEY_SEMICOLON},  
  {0,0,0,0,0,0,0,0},
  {0,0,KEY_F8,KEY_F9,0,0,KEY_MENU,KEY_LEFT},
  {KEY_COMMA,KEY_INSERT,KEY_F7,KEY_F6,KEY_F5,KEY_8,KEY_I,KEY_K},  
  {0,0,0,0,0,0,0,0},  
  {KEY_PERIOD,KEY_DOWN,0,KEY_F11,KEY_F10,KEY_9,KEY_O,KEY_L},
  {KEY_M,KEY_N,KEY_H,KEY_Y,KEY_6,KEY_7,KEY_U,KEY_J},
  {KEY_V,KEY_B,KEY_G,KEY_T,KEY_5,KEY_4,KEY_R,KEY_F},  
  {KEY_X,KEY_Z,KEY_F2,KEY_F1,KEY_ESC,KEY_2,KEY_W,KEY_S},  
  {KEY_C,KEY_SPACE,KEY_F3,KEY_F4,KEY_CAPS_LOCK,KEY_3,KEY_E,KEY_D},
  {0,0,0,0,0,0,0,0},
  {0,0,0,KEY_TAB,KEY_TILDE,KEY_1,KEY_Q,KEY_A},  
  {0,0,0,0,0,0,0,0}
};
// Load the numlock key matrix with key names at the correct row-column location. 
// This matrix is the same as the normal matrix except for the number pad keys
// A zero indicates no numlock key at that location.
int numlock[rows_max][cols_max] = {
  {KEY_END,KEY_RIGHT,0,KEY_PRINTSCREEN,KEY_DELETE,KEY_HOME,KEY_PAGE_UP,KEY_PAGE_DOWN},
  {KEY_QUOTE,KEY_ENTER,KEY_PAUSE,0,KEY_EQUAL,KEY_BACKSPACE,KEY_RIGHT_BRACE,KEY_BACKSLASH},  
  {KEYPAD_SLASH,KEY_UP,KEY_MINUS,KEY_F12,KEYPAD_ASTERIX,KEYPAD_MINUS,KEY_LEFT_BRACE,KEYPAD_PLUS},  
  {0,0,0,0,0,0,0,0},
  {0,0,KEY_F8,KEY_F9,0,0,KEY_MENU,KEY_LEFT},
  {KEY_COMMA,KEY_INSERT,KEY_F7,KEY_F6,KEY_F5,KEYPAD_8,KEYPAD_5,KEYPAD_2},  
  {0,0,0,0,0,0,0,0},  
  {KEYPAD_PERIOD,KEY_DOWN,0,KEY_F11,KEY_F10,KEYPAD_9,KEYPAD_6,KEYPAD_3},
  {KEYPAD_0,KEY_N,KEY_H,KEY_Y,KEY_6,KEYPAD_7,KEYPAD_4,KEYPAD_1},
  {KEY_V,KEY_B,KEY_G,KEY_T,KEY_5,KEY_4,KEY_R,KEY_F},  
  {KEY_X,KEY_Z,KEY_F2,KEY_F1,KEY_ESC,KEY_2,KEY_W,KEY_S},  
  {KEY_C,KEY_SPACE,KEY_F3,KEY_F4,KEY_CAPS_LOCK,KEY_3,KEY_E,KEY_D},
  {0,0,0,0,0,0,0,0},
  {0,0,0,KEY_TAB,KEY_TILDE,KEY_1,KEY_Q,KEY_A},  
  {0,0,0,0,0,0,0,0}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_LEFT_CTRL,0,0,MODIFIERKEY_RIGHT_CTRL,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {MODIFIERKEY_RIGHT_SHIFT,MODIFIERKEY_LEFT_SHIFT,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},    
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {MODIFIERKEY_RIGHT_ALT,MODIFIERKEY_LEFT_ALT,0,0,0,0,0,0},
  {0,0,MODIFIERKEY_FN,0,0,0,0,0},
  {0,MODIFIERKEY_GUI,0,0,0,0,0,0}
};
// Load the media key matrix with Fn key names at the correct row-column location. 
// A zero indicates no media key at that location.
int media[rows_max][cols_max] = {
  {0,0,0,0,KEY_SCROLL_LOCK,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,KEY_NUM_LOCK,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,KEY_MEDIA_MUTE,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,KEY_MEDIA_VOLUME_DEC,KEY_MEDIA_VOLUME_INC,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0}  
};
// Initialize the old_key matrix with one's (no keys pressed). 
// 1 = key not pressed, 0 = key is pressed
boolean old_key[rows_max][cols_max] = {
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1}
};
//
// Define the Teensy 3.2 I/O numbers that are connected to the keyboard FPC connector pins
// Row FPC pin # 09,10,11,12,13,14,15,16,17,18,19,20,21,22,23
// Teensy I/O  # 18,33,19,20,21,22,23,24,25,26,27,28,07,06,05
int Row_IO[rows_max] = {18,33,19,20,21,22,23,24,25,26,27,28,7,6,5}; // Teensy 3.2 I/O numbers for rows 0 thru 14
//
// Column FPC pin # 01,02,03,04,05,06,07,08
// Teensy I/O     # 14,29,15,30,16,31,17,32
int Col_IO[cols_max] = {14,29,15,30,16,31,17,32};  // Teensy 3.2 I/O numbers for columns 0 thru 7
//
// Declare global variables that will be used by functions
//
boolean slots_full = LOW; // Goes high when USB slots 1 thru 6 contain normal keys
boolean touchpad_error = LOW; // set high if the touchpad fails to give the correct power-up response
char mstat; // touchpad status 8 bit register = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
char mx; // touchpad x movement = 8 data bits. The sign bit is in the status register to 
           // make a 9 bit 2's complement value. Left to right on the touchpad gives a positive value. 
char my; // touchpad y movement also 8 bits plus sign bit in status reg. Touchpad movement away from the user gives a positive value.
//
// slot 1 thru slot 6 hold the normal key values to be sent over USB. 
int slot1 = 0; //value of 0 means the slot is empty and can be used.  
int slot2 = 0; 
int slot3 = 0; 
int slot4 = 0; 
int slot5 = 0; 
int slot6 = 0;
//
int mod_shift_l = 0; // These variables are sent over USB as modifier keys.
int mod_shift_r = 0; // Each is either set to 0 or MODIFIER_ ... 
int mod_ctrl_l = 0;   
int mod_ctrl_r = 0;
int mod_alt_l = 0;
int mod_alt_r = 0;
int mod_gui = 0;
//
// ********************Common Functions**********************************************
//
// Function to set a pin to high impedance (acts like open drain output)
void go_z(int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}
//
// Function to set a pin as an input with a pullup
void go_pu(int pin)
{
  pinMode(pin, INPUT_PULLUP);
  digitalWrite(pin, HIGH);
}
//
// Function to send a pin to a logic low
void go_0(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}
//
// Function to send a pin to a logic high
void go_1(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}
//
// ********************Functions for keyboard scanning*******************************
//
// Function to load the key name into the first available slot
void load_slot(int key) {
  if (!slot1)  {   // check if slot 1 is empty
    slot1 = key;   // and load it with the key name
  }
  else if (!slot2) { // otherwise go thru all the other slots looking for an empty to load
    slot2 = key;
  }
  else if (!slot3) {
    slot3 = key;
  }
  else if (!slot4) {
    slot4 = key;
  }
  else if (!slot5) {
    slot5 = key;
  }
  else if (!slot6) {
    slot6 = key;
  }
  if (!slot1 || !slot2 || !slot3 || !slot4 || !slot5 || !slot6)  { // are any slots empty?
    slots_full = LOW; // slots are not full
  }
  else {
    slots_full = HIGH; // slots are full
  } 
}
//
// Function to clear the slot that contains the key name
void clear_slot(int key) {
  if (slot1 == key) {   // check if slot 1 contains the key that needs to be cleared
    slot1 = 0;         // and clear it
  }
  else if (slot2 == key) {  // otherwise keep checking the other slots for a match
    slot2 = 0;
  }
  else if (slot3 == key) {
    slot3 = 0;
  }
  else if (slot4 == key) {
    slot4 = 0;
  }
  else if (slot5 == key) {
    slot5 = 0;
  }
  else if (slot6 == key) {
    slot6 = 0;
  }
  if (!slot1 || !slot2 || !slot3 || !slot4 || !slot5 || !slot6)  { // are any slots empty?
    slots_full = LOW; // slots are not full
  }
  else {
    slots_full = HIGH; // slots are full
  }
}
//
// Function to load the modifier key name into the appropriate mod variable
void load_mod(int m_key) {
  if (m_key == MODIFIERKEY_LEFT_SHIFT)  {  // Figure out which modifier key was pushed
    mod_shift_l = m_key;                   // and load it into 1 of 7 possible variables
  }
  else if (m_key == MODIFIERKEY_RIGHT_SHIFT)  {
    mod_shift_r = m_key;
  }
  else if (m_key == MODIFIERKEY_LEFT_CTRL)  {
    mod_ctrl_l = m_key;
  }
  else if (m_key == MODIFIERKEY_RIGHT_CTRL)  {
    mod_ctrl_r = m_key;
  }
  else if (m_key == MODIFIERKEY_LEFT_ALT)  {
    mod_alt_l = m_key;
  }
  else if (m_key == MODIFIERKEY_RIGHT_ALT)  {
    mod_alt_r = m_key;
  }
  else if (m_key == MODIFIERKEY_GUI)  {
    mod_gui = m_key;
  }
}
//
// Function to load 0 into the appropriate mod variable
void clear_mod(int m_key) {
  if (m_key == MODIFIERKEY_LEFT_SHIFT)  {  // Figure out which modifier key was released
    mod_shift_l = 0;                       // and clear the matching variable
  }
  else if (m_key == MODIFIERKEY_RIGHT_SHIFT)  {
    mod_shift_r = 0;
  }
  else if (m_key == MODIFIERKEY_LEFT_CTRL)  {
    mod_ctrl_l = 0;
  }
  else if (m_key == MODIFIERKEY_RIGHT_CTRL)  {
    mod_ctrl_r = 0;
  }
  else if (m_key == MODIFIERKEY_LEFT_ALT)  {
    mod_alt_l = 0;
  }
  else if (m_key == MODIFIERKEY_RIGHT_ALT)  {
    mod_alt_r = 0;
  }
  else if (m_key == MODIFIERKEY_GUI)  {
    mod_gui = 0;
  }
}
//
// Function to send the modifier keys over usb
void send_mod() {
  Keyboard.set_modifier(mod_shift_l | mod_shift_r | mod_ctrl_l | mod_ctrl_r | mod_alt_l | mod_alt_r | mod_gui);
  Keyboard.send_now(); // sends all modifier values including the one that just changed (and all normal slots)
}
//
// Function to send the normal keys in the 6 slots over usb
void send_normals() {
  Keyboard.set_key1(slot1); // load each slot into the cooresponding key number
  Keyboard.set_key2(slot2);
  Keyboard.set_key3(slot3);
  Keyboard.set_key4(slot4);
  Keyboard.set_key5(slot5);
  Keyboard.set_key6(slot6);
  Keyboard.send_now(); // sends all slots including the one that just changed (and all modifier values)
}
//
// *****************Functions for Touchpad***************************
//
// Function to send the touchpad a byte of data (command)
//
void tp_write(char send_data)  
{
  unsigned int timeout = 200; // break out of loop if watchdog over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the PS/2 bus by floating the clock and data
  go_pu(TP_CLK); //
  go_pu(TP_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TP_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TP_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_pu(TP_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop because tp did not respond
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_pu(TP_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TP_DATA); // send a 0 to TP
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH;
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH;
        break; // break out of infinite loop
      }
    }  
    send_data = send_data >> 1; // shift data right by 1 to prepare for next loop
  }
// send the parity bit
  if (odd_parity & 1) {  //check if lsb of parity is set
    go_0(TP_DATA); // already odd so send a 0 to TP
  }
  else {
    go_pu(TP_DATA); // send a 1 to TP to make parity odd
  }   
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
  go_pu(TP_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TP_DATA)) { // Ack bit s/b low if good transfer
  }
  while ((digitalRead(TP_CLK) == LOW) || (digitalRead(TP_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when the Teensy is listening
  go_0(TP_CLK);
}
//
// Function to get a byte of data from the touchpad
//
char tp_read(void)
{
  unsigned int timeout = 200; // break out of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_pu(TP_CLK); // release the clock
  go_pu(TP_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    } 
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH;
        break; // break out of infinite loop
      }
    }
    if (digitalRead(TP_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH;
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    touchpad_error = HIGH;
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
  // send bad stop bit to future error handler
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH;
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when the Teensy is listening
  go_0(TP_CLK);
  return rcv_data; // pass the received data back
}
//
// Function to decode 3 bytes of data from the touchpad = status, X Delta, Y Delta
//
char tp_packet(void)           
{
// ******************************Receive Byte 0 = Status***********************
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
//
  unsigned int time_leftover = 24; // breakout of tp loop if timer is over this value in msec
  elapsedMillis tp_timer; // zero the tp timer clock
//
  go_pu(TP_CLK); // release the clock
  go_pu(TP_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low 
    if (tp_timer >= time_leftover) { //check if ready for next keyboard scan due to no tp activity
      go_0(TP_CLK); // Inhibit the bus so the tp won't try to send data
      return 1; // go back to main routine with a 1 signifying no tp data was received
    }
  }  
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    }
    if (digitalRead(TP_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    // parity bit not correct - put error handler here if desired
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
    // stop bit not correct - put error handler here if desired
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
  mstat = rcv_data; // save data result in status byte
// ******************************Receive Byte 1 = Delta X**************************  
  rcv_data = 0; // initialize to zero
  mask = 1; // shift a 1 across the 8 bits to select where to load the data
  rcv_parity = 0; // count the ones received
  delayMicroseconds(5); // delay to let the clock stop ringing
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    }
    if (digitalRead(TP_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    // parity bit not correct - put error handler here if desired
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
    // stop bit not correct - put error handler here if desired
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
  mx = rcv_data; // save data result in delta x byte
// Receive Byte 1 = Delta Y  
// ******************************Receive Byte 2 = Delta Y**************************  
  rcv_data = 0; // initialize to zero
  mask = 1; // shift a 1 across the 8 bits to select where to load the data
  rcv_parity = 0; // count the ones received
  delayMicroseconds(5); // delay to let the clock stop ringing
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    }
    if (digitalRead(TP_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    // parity bit not correct - put error handler here if desired
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
    // stop bit not correct - put error handler here if desired
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
  my = rcv_data; // save data result in delta y byte
//
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);

return 0;
}
//
void touchpad_init()
{
  touchpad_error = LOW; // start with no error
  go_pu(TP_CLK); // float the clock and data to touchpad
  go_pu(TP_DATA);
  //  Sending reset command to touchpad
  tp_write(0xff);
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
  delay(1000); // wait 1 second so tp can run its self diagnostic
  //  verify proper response from tp
  if (tp_read() != 0xaa) { // verify basic assurance test passed
    touchpad_error = HIGH;
  } 
  if (tp_read() != 0x00) { // verify basic assurance test passed
    touchpad_error = HIGH;
  }
  //  Send touchpad disable code so that the mode byte can be loaded next
  tp_write(0xf5);  // tp disable 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
  // Load Mode Byte with 00 using the following special sequence from page 38 of Synaptics Interfaceing Guide.
  // Send set resolution to 0 four times followed by a set sample rate to 0x14
  // The resolution and sample rate are not actually changed but instead it loads the mode byte.
// #1 set resolution
  tp_write(0xe8);  // set resolution (actually part of setting mode byte)
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// #2 set resolution (actually part of setting mode byte)
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// #3 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// #4 set resolution (actually part of setting mode byte)
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// Set sample rate (actually part of setting mode byte)
  tp_write(0xf3);  // set sample rate 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  tp_write(0x14);  // to 14 hex 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
// This completes the mode byte load
  // set the resolution for real
  tp_write(0xe8); //  Sending resolution command
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  tp_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  // set the sample rate for real
  tp_write(0xf3); //  Sending sample rate command
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  tp_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  //  Load stream mode command so tp will send data whenever it's available
  tp_write(0xea);  // stream mode 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  //  Send touchpad enable code 
  tp_write(0xf4);  // tp enable 
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
}
//
//----------------------------------Setup before main loop-------------------------------------------
//
void setup() {
  for (int a = 0; a < cols_max; a++) {  // loop thru all keyboard column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all keyboard row pins 
    go_z(Row_IO[b]); // set each row pin as a floating output
  }  
  touchpad_init(); // reset tp and check that self diagnostic passed. Put tp in stream mode and enable it 
//
}
// 
boolean Fn_pressed = HIGH; // Active low, shows when the Fn key is pressed
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status.
// declare and initialize touchpad variables  
boolean over_flow; // Active high, set if x or y movement values are bad due to overflow
boolean left_button = 0; // Active high, on/off variable for left button is set if bit 0 of mstat is set
boolean right_button = 0; // Active high, on/off variable for right button is set if bit 1 of mstat is set
boolean old_left_button = 0; // Active high, on/off variable for left button status from the previous polling cycle
boolean old_right_button = 0; // Active high, on/off variable for right button status from the previous polling cycle
boolean button_change = 0; // Active high, shows when a touchpad left or right button has changed since the last polling cycle
//
//---------------------------------Main Loop---------------------------------------------
//
void loop() {   
// Scan keyboard matrix with an outer loop that drives each row low and an inner loop that reads every column (with pull ups).
// The routine looks at each key's present state (by reading the column input pin) and also the previous state from the last scan
// that was 30msec ago. The status of a key that was just pressed or just released is sent over USB and the state is saved in the old_key matrix. 
// The keyboard keys will read as logic low if they are pressed (negative logic).
// The old_key matrix also uses negative logic (low=pressed). 
//
  for (int x = 0; x < rows_max; x++) {   // loop thru the rows
    go_0(Row_IO[x]); // Activate Row (send it low)
    delayMicroseconds(10); // give the row time to go low and settle out
    for (int y = 0; y < cols_max; y++) {   // loop thru the columns
//
// **********Modifier keys including the Fn special case
      if (modifier[x][y] != 0) {  // check if modifier key exists at this location in the array (a non-zero value)
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) {  // Read column to see if key is low (pressed) and was previously not pressed
          if (modifier[x][y] != MODIFIERKEY_FN) {   // Exclude Fn modifier key because Fn is not sent to USB host  
            load_mod(modifier[x][y]); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable   
            send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
            old_key[x][y] = LOW; // Save state of key as "pressed"
          }
          else {   // keep track of Fn key being pressed
            Fn_pressed = LOW; // Fn status variable is active low
            old_key[x][y] = LOW; // old_key state is "pressed" (active low)
          }
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) {  //check if key is not pressed and was previously pressed
          if (modifier[x][y] != MODIFIERKEY_FN) { // Exclude Fn modifier key 
            clear_mod(modifier[x][y]); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
            send_mod(); // function sends all mod's over usb including the one that was just released
            old_key[x][y] = HIGH; // Save state of key as "not pressed"
          }
          else {   // keep track of Fn key being released
            Fn_pressed = HIGH; // Fn is no longer active
            old_key[x][y] = HIGH; // old_key state is "not pressed" 
          }
        }
      } 
// ***********end of modifier section
//
// ***********Normal keys, num lock, and media keys in this section
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {  // check if normal or media key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn is not pressed and normal or num lock key needs to be sent
            if (keyboard_leds & 1) {  // test if Num Lock is turned on
              load_slot(numlock[x][y]); //update first available slot with key name from numlock matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
            else {  // Num Lock is not turned on so send normal key
              load_slot(normal[x][y]); //update first available slot with normal key name
              send_normals(); // send all slots over USB including the key that just got pressed
            }
          }
          else if (media[x][y] != 0) { // Fn is pressed so send media if a key exists in the matrix
            Keyboard.press(media[x][y]); // media key is sent using keyboard.press function per PJRC instructions    
            delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
            Keyboard.release(media[x][y]); // send media key release
          }
        }          
// Key release        
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed 
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          if (Fn_pressed) {  // only clear the slot if the Fn key is not pressed
            if (keyboard_leds & 1) {  // test if Num Lock is turned on
              clear_slot(numlock[x][y]); //clear slot with key name from numlock matrix
              send_normals(); // send all slots over USB including the key that just got released
            }
            else {  
              clear_slot(normal[x][y]); //clear the slot that contains the normal key name
              send_normals(); // send all slots over USB including the key that was just released 
            }
          }
        }
      } 
// **************end of normal, media, and backlight section 
//
    } // end of column loop
    go_z(Row_IO[x]); // De-activate the current Row (send it to hi-z) so the next row can be activated
  } // end of row loop
//
// **********Keyboard scan complete
//
// ***********Touchpad Section
//
  if (touchpad_error == LOW) { // Only proceed if the touchpad was initialized correctly and passed its self test
//
    if (tp_packet() == 0x00) { // tp_packet returned zero so data was received in mstat, mx, and my variables
      if (((0x80 & mstat) == 0x80) || ((0x40 & mstat) == 0x40))  {   // x or y overflow bits set?
      over_flow = 1; // set the overflow flag
      }   
// change the x data from 9 bit to 8 bit 2's complement
      mx = mx & 0x7f; // mask off 8th bit 
      if ((0x10 & mstat) == 0x10) {   // move the sign into 
        mx = 0x80 | mx;              // the 8th bit position
      } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
      my = my & 0x7f; // mask off 8th bit
      if ((0x20 & mstat) == 0x20) {   // move the sign into 
        my = 0x80 | my;              // the 8th bit position
      } 
      my = (~my + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx and my if over_flow is set
      if (over_flow) { 
        mx = 0x00;       // data is bad due to the overflow so zero it out
        my = 0x00;
      } 
// send the x and y data back via usb if either one is non-zero
      if ((mx != 0x00) || (my != 0x00)) {
        Mouse.move(mx,my);
      }
//
// read the left and right button bits in the PS/2 status byte from the touchpad
      if ((0x01 & mstat) == 0x01) {   // if left button is set in the status byte 
        left_button = 1;   // set the left button variable
      }
      else {   // otherwise clear the left button variable
        left_button = 0;   
      }
      if ((0x02 & mstat) == 0x02) {   // if right button is set in the status byte
        right_button = 1;   // set the right button variable
      } 
      else {   // otherwise clear right button variable
        right_button = 0;  
      }
// Determine if the left or right touchpad buttons have changed since last polling cycle using xor
      button_change = (left_button ^ old_left_button) | (right_button ^ old_right_button);
// Don't send button status if there's no change since last time. 
      if (button_change){
        Mouse.set_buttons(left_button, 0, right_button); // send left, middle, and right button status
      }
      old_left_button = left_button; // remember button status for next polling cycle
      old_right_button = right_button;
    }
  }
//
// End of touchpad routine
//
// Turn on or off the LEDs for Num Lock and Caps Lock based on bits 0 and 1 from the  
// keyboard_leds variable returned by the host computer over USB. Sending the I/O pin low turns on the   
// LED. 
//
  if (keyboard_leds & 1) {  // mask off all bits but D0 and test if set
    go_0(NUM_LOCK_N); // turn on the Num Lock LED
  }
  else {
    go_1(NUM_LOCK_N); // turn off the Num Lock LED
  }
//
//
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_0(CAPS_LOCK_N); // turn on the Caps Lock LED
  }
  else {
    go_1(CAPS_LOCK_N); // turn off the Caps Lock LED
  }
//
// repeat loop at 30msec scan rate 
}
