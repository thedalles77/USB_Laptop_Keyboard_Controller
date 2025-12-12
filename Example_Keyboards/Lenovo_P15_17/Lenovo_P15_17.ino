/* Copyright 2025 Frank Adams
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
// This software controls a Lenovo ThinkPad P15 & P17 Gen 1 Laptop Keyboard and Trackpoint. 
// The PS/2 code for the Trackpoint uses Stream mode instead of Remote mode.
//
// Revision History
// Rev New - Dec 6 , 2025 - Original Release
//
// Keyboard LEDs
#define LED_PWR 25    // Anode side of LEDs always driven high to 3.3 volts
#define CAPS_LED 31   // Active low signal to Caps lock LED cathode (with 100 ohm dropping resistor) 
#define NUM_LED 38    // Active low signal to Num lock LED cathode (with 100 ohm dropping resistor)
#define MUTE_LED 27   // Active low signal to Mute LED cathode (with 100 ohm dropping resistor)
#define FN_LED 26     // Active low signal to show when fn key is pressed (with 100 ohm resistor)
#define BACKLIGHT 13   // The backlight PWM control input pin
// Keyboard Fn key (aka HOTKEY)
#define HOTKEY 29       // Fn key
#define HOTKEY_RTN 30   // Fn key return (always driven low)
// TP buttons and clk/data
#define TP_BUTTON_RTN 32 // always driven low
#define TP_LEFT 33 // "mouse" buttons
#define TP_RIGHT 34
#define TP_MIDDLE 35 
#define TP_DATA 39 // Trackpoint ps/2 data connected to Teensy I/O pin 
#define TP_CLK 41  // Trackpoint ps/2 clock connected to Teensy I/O pin
#define TP_RESET 40  // Trackpoint active high reset 
// Set the keyboard row & column size
const byte rows_max = 18; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
// 
int brightness = 0;   // Variable for the keyboard backlight brightness to store the PWM duty cycle (0=off, 255=max) 
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {KEY_TAB,0,KEY_Z,KEY_A,KEY_1,KEY_Q,KEY_TILDE,KEY_ESC},
  {KEY_Y,KEY_N,KEY_M,KEY_J,KEY_7,KEY_U,KEY_6,KEY_H},
  {KEY_F3,0,KEY_C,KEY_D,KEY_3,KEY_E,KEY_F2,KEY_F4},
  {KEY_CAPS_LOCK,0,KEY_X,KEY_S,KEY_2,KEY_W,KEY_F1,0},
  {KEY_T,KEY_B,KEY_V,KEY_F,KEY_4,KEY_R,KEY_5,KEY_G},
  {KEY_F7,0,KEY_PERIOD,KEY_L,KEY_9,KEY_O,KEY_F8,0},
  {KEY_LEFT_BRACE,KEY_SLASH,0,KEY_SEMICOLON,KEY_0,KEY_P,KEY_MINUS,KEY_QUOTE},
  {KEY_RIGHT_BRACE,0,KEY_COMMA,KEY_K,KEY_8,KEY_I,KEY_EQUAL,KEY_F6},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,KEY_PRINTSCREEN,0,0,0},
  {0,KEY_RIGHT,0,0,KEY_F12,0,KEY_INSERT,0},
  {0,KEY_LEFT,0,0,KEY_END,0,KEY_HOME,KEY_UP},
  {0,KEY_DOWN,0,0,KEY_F11,0,KEY_DELETE,0},
  {KEY_BACKSPACE,KEY_SPACE,KEY_ENTER,KEY_BACKSLASH,KEY_F10,0,KEY_F9,KEY_F5},
  {0,0,0,0,KEY_PAGE_DOWN,0,KEY_PAGE_UP,0},
  {KEY_A,KEY_B,KEY_C,KEY_D,KEY_E,KEY_F,KEY_G,KEY_H}, //REPLACE WITH KEYPAD_
  {KEY_I,KEY_J,KEY_K,KEY_L,KEY_M,KEY_N,KEY_O,KEY_P}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {MODIFIERKEY_LEFT_SHIFT,0,MODIFIERKEY_RIGHT_SHIFT,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,MODIFIERKEY_RIGHT_CTRL,0,0,0,MODIFIERKEY_LEFT_CTRL,0},
  {0,MODIFIERKEY_RIGHT_ALT,0,0,0,0,0,MODIFIERKEY_LEFT_ALT}, 
  {MODIFIERKEY_GUI,0,0,0,0,0,0,0},    
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
// Load the media key matrix with key names at the correct row-column location. 
// A zero indicates no media key at that location.
int media[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_MEDIA_VOLUME_INC,0,0,0,0,0,KEY_MEDIA_VOLUME_DEC,0},
  {0,0,0,0,0,0,KEY_MEDIA_MUTE,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
// Initialize the old_key matrix with one's. 
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
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1}
};
//
// Define the Teensy 4.1 I/O numbers 
//
// Row FPC pin # 4,7, 10,11,13,14,15,16,17,18,19,20,21,22,23,24
// Teensy I/O  # 1,20,4, 18,17,6, 16,7, 15,8, 14,9, 10,11,12,24
int Row_IO[rows_max] = {1,20,4,18,17,6,16,7,15,8,14,9,10,11,12,24}; // Teensy 4.1 I/O numbers for rows
//
// Column FPC pin # 1, 2,3, 5, 6,8,9, 12
// Teensy I/O     # 23,0,22,21,2,3,19,5
int Col_IO[cols_max] = {23,0,22,21,2,3,19,5};  // Teensy 4.1 I/O numbers for columns
//
// Declare variables that will be used by functions
boolean slots_full = LOW; // Goes high when slots 1 thru 6 contain normal keys
boolean trackpoint_error = LOW; // set high if the trackpoint fails to give the correct response
boolean tp_speed = 1; // 1 = high speed, 0 = low speed movement of trackpoint
boolean mute = 0; // variable to turn on mute led when a 1
char mstat; // trackpoint status 8 bit register = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
char mx; // trackpoint x movement = 8 data bits. The sign bit is in the status register to 
           // make a 9 bit 2's complement value. Left to right on the trackpoint gives a positive value. 
char my; // trackpoint y movement also 8 bits plus sign bit in status reg. trackpoint movement toward the display gives a positive value.
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
// **************Functions common to keyboard, trackpoint, and trackpoint**************************
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
// *****************Functions for Keyboard*****************************
// Function to load the key name into the first available slot
void load_slot(int key) {
  if (!slot1)  {
    slot1 = key;
  }
  else if (!slot2) {
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
  if (!slot1 || !slot2 || !slot3 || !slot4 || !slot5 || !slot6)  {
    slots_full = LOW; // slots are not full
  }
  else {
    slots_full = HIGH; // slots are full
  } 
}
//
// Function to clear the slot that contains the key name
void clear_slot(int key) {
  if (slot1 == key) {
    slot1 = 0;
  }
  else if (slot2 == key) {
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
  if (!slot1 || !slot2 || !slot3 || !slot4 || !slot5 || !slot6)  {
    slots_full = LOW; // slots are not full
  }
  else {
    slots_full = HIGH; // slots are full
  } 
}
//
// Function to load the modifier key name into the appropriate mod variable
void load_mod(int m_key) {
  if (m_key == MODIFIERKEY_LEFT_SHIFT)  {
    mod_shift_l = m_key;
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
  if (m_key == MODIFIERKEY_LEFT_SHIFT)  {
    mod_shift_l = 0;
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
  Keyboard.send_now();
}
//
// Function to send the normal keys in the 6 slots over usb
void send_normals() {
  Keyboard.set_key1(slot1);
  Keyboard.set_key2(slot2);
  Keyboard.set_key3(slot3);
  Keyboard.set_key4(slot4);
  Keyboard.set_key5(slot5);
  Keyboard.set_key6(slot6);
  Keyboard.send_now();
}
//
// *****************Functions for trackpoint***************************
//
// ********Function to send the trackpoint a byte of data (command)*********
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
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
// send the 8 bits in send_data variable
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
        trackpoint_error = HIGH;
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH;
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
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
  go_pu(TP_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TP_DATA)) { // Ack bit s/b low if good transfer
    //
  }
  while ((digitalRead(TP_CLK) == LOW) || (digitalRead(TP_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when the Teensy is listening
  go_0(TP_CLK);
}
//
// *********Function to get a byte of data from the trackpoint**********
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
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    } 
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH;
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
        trackpoint_error = HIGH;
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    trackpoint_error = HIGH;
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
  // send bad stop bit to future error handler
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH;
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when the Teensy is listening
  go_0(TP_CLK);
  return rcv_data; // pass the received data back
}
//
//
// ********Function to decode a 3 byte packet of data from the trackpoint (status, X Delta, Y Delta)********
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
// ************************************TP Initialization Setup**********************************************
//
void trackpoint_init()
{
  trackpoint_error = LOW; // start with no error
  go_pu(TP_CLK); // float the clock and data to trackpoint
  go_pu(TP_DATA);
  // pulse the reset signal high, then low
  go_0(TP_RESET); //
  delay(1000);
  go_1(TP_RESET); //
  delay(1000);
  go_0(TP_RESET); // 
  delay(1000);  
  //  Sending reset command to trackpoint
  tp_write(0xff);
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  delay(1000); // wait 1 second so tp can run its self diagnostic
  //  verify proper response from tp
  if (tp_read() != 0xaa) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  } 
  if (tp_read() != 0x00) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  }
  //  Send trackpoint disable code so that the mode byte can be loaded next
  tp_write(0xf5);  // tp disable 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  // Load Mode Byte with 00 using the following special sequence from page 38 of Synaptics Interfaceing Guide.
  // Send set resolution to 0 four times followed by a set sample rate to 0x14
  // The resolution and sample rate are not actually changed but instead it loads the mode byte.
// #1 set resolution
  tp_write(0xe8);  // set resolution (actually part of setting mode byte)
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #2 set resolution (actually part of setting mode byte)
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #3 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #4 set resolution (actually part of setting mode byte)
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// Set sample rate (actually part of setting mode byte)
  tp_write(0xf3);  // set sample rate 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  tp_write(0x14);  // to 14 hex 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
// This completes the mode byte load
  // set the resolution for real
  tp_write(0xe8); //  Sending resolution command
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  tp_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  // set the sample rate for real
  tp_write(0xf3); //  Sending sample rate command
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  tp_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  //  Load stream mode command so tp will send data whenever it's available
  tp_write(0xea);  // stream mode 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  //  Send trackpoint enable code 
  tp_write(0xf4);  // tp enable 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
}
//
//************************************Setup*******************************************
void setup() {
// ************keyboard setup
  for (int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as a floating output
  }
  trackpoint_init(); // reset tp and check that self diagnostic passed. Put tp in stream mode and enable it 
  if (trackpoint_error) {
    delay(500); // wait 1/2 sec
    trackpoint_init(); // try one more time to initialize the trackpoint
  }  
//
  go_pu(HOTKEY);    // Pull up the Fn "Hotkey" input so it can be read. If key pressed, it will be low.
  go_0(HOTKEY_RTN); // Instead of grounding this pin, it is driven low always
//
  pinMode(BACKLIGHT, OUTPUT); // Set the Backlight PWM control pin as an output
  analogWriteFrequency(BACKLIGHT,400); // Set the Backlight PWM at 400 Hz
//
  go_1(LED_PWR);    // Puts 3.3 volts on anodes of all LEDs
  go_1(MUTE_LED);   // Start with mute led off
//
  go_0(TP_BUTTON_RTN); // Keep the TP return always low (instead of tying it to ground)
  go_pu(TP_LEFT); // Set the TP buttons as inputs with pullups
  go_pu(TP_RIGHT);
  go_pu(TP_MIDDLE);
}
//
// **********declare and initialize keyboard variables
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status. 
char blink_count = 0; // Blink loop counter
boolean blinky = LOW; // Blink LED state
// TP variables
boolean over_flow = 0; // Active high, set if x or y movement values are bad due to overflow
boolean left_button = 0; // Active high, on/off variable for left button 
boolean right_button = 0; // Active high, on/off variable for right button 
boolean middle_button = 0; // Active high, on/off variable for middle button
boolean old_left_button = 0; // Active high, on/off variable for left button status from the previous polling cycle
boolean old_right_button = 0; // Active high, on/off variable for right button status from the previous polling cycle
boolean old_middle_button = 0; // Active high, on/off variable for middle button status from the previous polling cycle
boolean button_change = 0; // Active high, shows when any trackpoint button has changed since the last polling cycle
//
//*********************************Main Loop********************************************
//
void loop() {   
// *************Keyboard Main**************
// Read the Fn key (aka Hotkey) which is not part of the key matrix
  if (!digitalRead(HOTKEY)) {
    Fn_pressed = LOW; // Fn key is pressed (active low)
    go_0(FN_LED); // turn on FN LED
  }
  else  {
    Fn_pressed = HIGH; // Fn key is not pressed
    go_1(FN_LED); // turn OFF FN LED
  }
// Set the backlight brightbness
  analogWrite(BACKLIGHT, brightness);
//
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
// **********Modifier keys
      if (modifier[x][y] != 0) {  // check if modifier key exists at this location in the array (a non-zero value)
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) {  // Read column to see if key is low (pressed) and was previously not pressed
            load_mod(modifier[x][y]); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable   
            send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
            old_key[x][y] = LOW; // Save state of key as "pressed"
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) {  //check if key is not pressed and was previously pressed
            clear_mod(modifier[x][y]); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
            send_mod(); // function sends all mod's over usb including the one that just released
            old_key[x][y] = HIGH; // Save state of key as "not pressed"
        }
      } 
// ***********end of modifier section
//
// ***********Normal keys and media keys in this section
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {  // check if normal or media key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key is pressed and was not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn_pressed is active low so it is not pressed and normal key needs to be sent
            load_slot(normal[x][y]); //update first available slot with normal key name
            send_normals(); // send all slots over USB including the key that just got pressed
          }
          else if (normal[x][y] == KEY_SPACE) {  // Fn - space bar turns on/off keyboard backlight
            if (brightness == 255) {
              brightness = 0;
            }
            else if (brightness == 0) {
              brightness = 255;
            }
          }
          else if (normal[x][y] == KEY_F12) {
            tp_speed = !tp_speed;
            go_0(FN_LED); // turn on FN LED
          }
          else if (media[x][y] != 0) { // Fn is pressed so send media if a key exists in the matrix
            Keyboard.press(media[x][y]); // media key is sent using keyboard press function per PJRC
            if (media[x][y] == KEY_MEDIA_MUTE) {
              mute = !mute; // invert the mute variable
              if (mute) {
                go_0(MUTE_LED);
              }
              else {
                go_1(MUTE_LED);
              }
            }    
            delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
            Keyboard.release(media[x][y]); // send media key release
          }
        }          
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed 
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          if (Fn_pressed) {  // Fn is not pressed
            clear_slot(normal[x][y]); //clear the slot that contains the normal key name
            send_normals(); // send all slots over USB including the key that was just released 
          }
        }
      } 
// **************end of normal and media key section
    }
    go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
  }
//
// **********keyboard scan complete********************************
//
//
// ***********Trackpoint Section******************************
//
  if (trackpoint_error == LOW) { // Only proceed if the trackpoint was initialized correctly and passed its self test
//
    if (tp_packet() == 0x00) { // tp_packet returned zero so data was received in mstat, mx, and my variables
      if (((0x80 & mstat) == 0x80) || ((0x40 & mstat) == 0x40))  {   // x or y overflow bits set?
      over_flow = 1; // set the overflow flag
      }   
// change the x data from 9 bit to 8 bit 2's complement
      if (!tp_speed) { // check if tp_speed variable is low
        mx = mx >> 1; // divide by 2 in order to slow tp speed down
      }
      mx = mx & 0x7f; // mask off 8th bit 
      if ((0x10 & mstat) == 0x10) {   // move the sign into 
        mx = 0x80 | mx;              // the 8th bit position
      } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of trackpoint.move function
      if (!tp_speed) { // check if tp_speed variable is low
        my = my >> 1; // divide by 2 in order to slow tp speed down
      }
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
// send the x and y data back via usb if any are non-zero
      if ((mx != 0x00) || (my != 0x00)) {
        Mouse.move(mx,my);
      }
    }
  }
//
//**********TP Buttons********************************************
//
  if (!digitalRead(TP_LEFT)) {  // the switch reads low when the button is pushed
    left_button = 1;   // set the left button variable (active high)
  }
  else {   // otherwise clear the left button variable
    left_button = 0;     
  }
  if (!digitalRead(TP_RIGHT)) {  // the switch reads low when the button is pushed
    right_button = 1;   // set the right button variable
  } 
  else {   // otherwise clear right button variable
    right_button = 0;  
  }
  if (!digitalRead(TP_MIDDLE)) {  // the switch reads low when the button is pushed
    middle_button = 1;   // set the middle button variable
  } 
  else {   // otherwise clear middle button variable
    middle_button = 0;  
  }
// Determine if any trackpoint buttons have changed since last polling cycle using xor
  button_change = (left_button ^ old_left_button) | (right_button ^ old_right_button) | (middle_button ^ old_middle_button);

// Don't send button status if there's no change since last time. 
  if (button_change){
    Mouse.set_buttons(left_button, middle_button, right_button); // send left, middle, and right button status
  }
  old_left_button = left_button; // remember button status for next polling cycle
  old_right_button = right_button;
  old_middle_button = middle_button;
//
// End of trackpoint routine
//
// *******keyboard LEDs
// Turn on or off the Caps/Num Lock LEDs based on bits from the keyboard_leds 
// variable controlled by the USB host computer
//
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_0(CAPS_LED); // turn on the Caps Lock LED
  }
  else {
    go_1(CAPS_LED); // turn off the Caps Lock LED
  }
//
  if (keyboard_leds & 1) {  // mask off all bits but D0 and test if set
    go_0(NUM_LED); // turn on the Num Lock LED
  }
  else {
    go_1(NUM_LED); // turn off the Num Lock LED
  }
//




//
// ****************End of main loop
//
}
