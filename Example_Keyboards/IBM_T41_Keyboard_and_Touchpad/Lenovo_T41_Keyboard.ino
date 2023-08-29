/* Copyright 2023 Frank Adams
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
// This software controls a IBM ThinkPad T41 Laptop Keyboard and PS/2 Trackpoint using a Teensy 4.0 on 
// a daughterboard with a 40 pin connector. 
// This routine uses the Teensyduino "Micro-Manager Method" to send Normal and Modifier 
// keys over USB. Only the volume control multi-media keys are supported by this routine.
// Description of Teensyduino keyboard functions is at www.pjrc.com/teensy/td_keyboard.html
// The ps/2 code uses the USB PJRC Mouse functions at www.pjrc.com/teensy/td_mouse.html 
// In the Arduino IDE, select Tools, Teensy 4.0. Also under Tools, select Keyboard+Mouse+Joystick
//
// Revision History
// Rev New - August 17, 2023 - Original Release
//
// Trackpoint signals
#define TRACK_DATA 23   // ps/2 data to trackpoint (level translated)
#define TRACK_CLK 0    // ps/2 clock to trackpoint (level translated)
// Touchpad signals
#define TOUCH_DATA 17  // ps/2 data to touchpad (level translated)
#define TOUCH_CLK 16   // ps/2 clock to touchpad (level translated)
// Keyboard LEDs
#define CAPS_LED 18   // Wire these 3 I/O's to the anode side of LED's 
#define NUM_LED 19    // Wire the cathode side thru a dropping resistor
#define BLINK_LED 13  // The LED on the Teensy is programmed to blink 
// Keyboard Fn key (aka HOTKEY)
#define HOTKEY 9       // Fn key 
// Set the keyboard row & column size
const byte rows_max = 16; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {0,KEY_SCROLL_LOCK,KEY_PRINTSCREEN,0,0,0,0,0},
  {KEY_HOME,0,KEY_END,KEY_LEFT,KEY_PAUSE,0,0,KEY_UP},
  {KEY_PAGE_UP,0,KEY_PAGE_DOWN,KEY_PAGE_DOWN,KEY_PAGE_UP,0,0,0},
  {KEY_INSERT,0,KEY_F12,KEY_RIGHT,0,0,0,0},
  {KEY_DELETE,0,KEY_F11,KEY_DOWN,0,0,0,0},
  {KEY_MINUS,KEY_P,KEY_0,KEY_SLASH,0,KEY_SEMICOLON,KEY_LEFT_BRACE,KEY_QUOTE},
  {KEY_F8,KEY_O,KEY_9,0,KEY_PERIOD,KEY_L,KEY_F7,0},
  {KEY_EQUAL,KEY_I,KEY_8,0,KEY_COMMA,KEY_K,KEY_RIGHT_BRACE,KEY_F6},
  {KEY_TILDE,KEY_Q,KEY_1,0,KEY_Z,KEY_A,KEY_TAB,KEY_ESC},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_F1,KEY_W,KEY_2,0,KEY_X,KEY_S,KEY_CAPS_LOCK,0},
  {KEY_F2,KEY_E,KEY_3,0,KEY_C,KEY_D,KEY_F3,KEY_F4},
  {KEY_5,KEY_R,KEY_4,KEY_B,KEY_V,KEY_F,KEY_T,KEY_G},
  {KEY_F9,0,KEY_F10,KEY_SPACE,KEY_ENTER,KEY_BACKSLASH,KEY_BACKSPACE,KEY_F5},
  {KEY_6,KEY_U,KEY_7,KEY_N,KEY_M,KEY_J,KEY_Y,KEY_H}
};
// Load the num lock key matrix which is the same as the normal matrix except for the number pad keys
// A zero indicates no key at that location.
int numlock[rows_max][cols_max] = {
  {0,KEY_SCROLL_LOCK,KEY_PRINTSCREEN,0,0,0,0,0},
  {KEY_HOME,0,KEY_END,KEY_LEFT,KEY_PAUSE,0,0,KEY_UP},
  {KEY_PAGE_UP,0,KEY_PAGE_DOWN,KEY_PAGE_DOWN,KEY_PAGE_UP,0,0,0},
  {KEY_INSERT,0,KEY_F12,KEY_RIGHT,0,0,0,0},
  {KEY_DELETE,0,KEY_F11,KEY_DOWN,0,0,0,0},
  {KEY_MINUS,KEYPAD_ASTERIX,KEYPAD_SLASH,KEYPAD_PLUS,0,KEYPAD_MINUS,KEY_LEFT_BRACE,KEY_QUOTE},
  {KEY_F8,KEYPAD_6,KEYPAD_9,0,KEYPAD_PERIOD,KEYPAD_3,KEY_F7,0},
  {KEY_EQUAL,KEYPAD_5,KEYPAD_8,0,KEY_COMMA,KEYPAD_2,KEY_RIGHT_BRACE,KEY_F6},
  {KEY_TILDE,KEY_Q,KEY_1,0,KEY_Z,KEY_A,KEY_TAB,KEY_ESC},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_F1,KEY_W,KEY_2,0,KEY_X,KEY_S,KEY_CAPS_LOCK,0},
  {KEY_F2,KEY_E,KEY_3,0,KEY_C,KEY_D,KEY_F3,KEY_F4},
  {KEY_5,KEY_R,KEY_4,KEY_B,KEY_V,KEY_F,KEY_T,KEY_G},
  {KEY_F9,0,KEY_F10,KEY_SPACE,KEY_ENTER,KEY_BACKSLASH,KEY_BACKSPACE,KEY_F5},
  {KEY_6,KEYPAD_4,KEYPAD_7,KEY_N,KEYPAD_0,KEYPAD_1,KEY_Y,KEY_H}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// Access IBM key is used as the Windows GUI key.
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,0,0,MODIFIERKEY_RIGHT_ALT,0,0,0,MODIFIERKEY_LEFT_ALT},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,MODIFIERKEY_GUI},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,MODIFIERKEY_RIGHT_SHIFT,0,MODIFIERKEY_LEFT_SHIFT,0},
  {MODIFIERKEY_LEFT_CTRL,0,0,0,MODIFIERKEY_RIGHT_CTRL,0,0,0},
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
  {0,0,0,0,0,0,0,0},
  {0,KEY_MEDIA_VOLUME_INC,0,0,0,KEY_MEDIA_MUTE,KEY_MEDIA_VOLUME_DEC,0},
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
  {1,1,1,1,1,1,1,1}
};
//
// Define the Teensy 4.0 I/O numbers 
//
// Row FPC pin # 32,28,24,20,16,12,08,04,22,26,30,18,14,10,06,02
// Teensy I/O  # 01,02,03,04,05,06,07,08,20,21,22,25,27,29,31,33
int Row_IO[rows_max] = {1,2,3,4,5,6,7,8,20,21,22,25,27,29,31,33}; // Teensy 4.0 I/O numbers for rows
//
// Column FPC pin # 05,09,13,17,15,11,07,03
// Teensy I/O     # 10,11,12,24,26,28,30,32
int Col_IO[cols_max] = {10,11,12,24,26,28,30,32};  // Teensy 4.0 I/O numbers for columns
//
// Declare variables that will be used by functions
boolean touchpad_error = LOW; // sent high when touch pad routine times out or gives bad ack. Also used to turn off.
boolean trackpoint_error = LOW; // sent high when track point routine times out or gives bad ack. Also to turn off.
boolean slots_full = LOW; // Goes high when slots 1 thru 6 contain normal keys
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
// **************Functions common to keyboard, trackpoint, and touchpad**************************
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
// *****************Functions for Trackpoint***************************
//
// Function to send the trackpoint a byte of data (command)
//
void trackp_write(char send_data)  
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_z(TRACK_CLK); //
  go_z(TRACK_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TRACK_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TRACK_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_z(TRACK_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_z(TRACK_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TRACK_DATA); // send a 0 to TP
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }  
    send_data = send_data >> 1; // shift data right by 1 to prepare for next loop
  }
// send the parity bit
  if (odd_parity & 1) {  //check if lsb of parity is set
    go_0(TRACK_DATA); // already odd so send a 0 to TP
  }
  else {
    go_z(TRACK_DATA); // send a 1 to TP to make parity odd
  }   
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  go_z(TRACK_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TRACK_DATA)) { // Ack bit s/b low if good transfer
    trackpoint_error = HIGH; //bad ack bit so set the error flag
  }
  while ((digitalRead(TRACK_CLK) == LOW) || (digitalRead(TRACK_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TRACK_CLK);
}
//
// Function to get a byte of data from the trackpoint
//
char trackp_read(void)
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_z(TRACK_CLK); // release the clock
  go_z(TRACK_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TRACK_DATA)) { // Start bit s/b low from tp
    trackpoint_error = HIGH; // No start bit so set the error flag
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
    if (digitalRead(TRACK_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TRACK_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    trackpoint_error = HIGH; //bad parity so set the error flag
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TRACK_DATA) == LOW) { // check if stop bit is bad (low)
    trackpoint_error = HIGH; //bad stop bit so set the error flag
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TRACK_CLK);
  return rcv_data; // pass the received data back
}
//
void trackpoint_init()
{
  trackpoint_error = LOW; // start with no error
  go_z(TRACK_CLK); // float the clock and data to trackpoint
  go_z(TRACK_DATA);
  //  Sending reset command to trackpoint
  trackp_write(0xff);
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  delay(1000); // wait 1000ms so tp can run its self diagnostic
  //  verify proper response from touchpad
  if (trackp_read() != 0xaa) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  } 
  if (trackp_read() != 0x00) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  }
  //  Send touchpad disable code 
  trackp_write(0xf5);  // tp disable 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  // Load Mode Byte with 00 using the following special sequence from page 38.
  // Send set resolution to 0 four times followed by a set sample rate to 0x14
// #1 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x01);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #2 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x00);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #3 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x00);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #4 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x00);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// Set sample rate
  trackp_write(0xf3);  // set sample rate 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x14);  // to 14 hex 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  // set the resolution 
  trackp_write(0xe8); //  Sending resolution command
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  trackp_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  // set the sample rate 
  trackp_write(0xf3); //  Sending sample rate command
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  trackp_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  //  Sending remote mode code so the touchpad will send data only when polled
  trackp_write(0xf0);  // remote mode 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  //  Sending touchpad enable code (needed for Elan touchpads)
  trackp_write(0xf4);  // tp enable 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
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
// *****************Functions for Touchpad***************************
//
// Function to send the touchpad a byte of data (command)
//
void touch_write(char send_data)  
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_pu(TOUCH_CLK); //
  go_pu(TOUCH_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TOUCH_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TOUCH_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_pu(TOUCH_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_pu(TOUCH_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TOUCH_DATA); // send a 0 to TP
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TOUCH_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH; // flag the touchpad as bad
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH; // flag the touchpad as bad
        break; // break out of infinite loop
      }
    }  
    send_data = send_data >> 1; // shift data right by 1 to prepare for next loop
  }
// send the parity bit
  if (odd_parity & 1) {  //check if lsb of parity is set
    go_0(TOUCH_DATA); // already odd so send a 0 to TP
  }
  else {
    go_pu(TOUCH_DATA); // send a 1 to TP to make parity odd
  }   
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  go_pu(TOUCH_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TOUCH_DATA)) { // Ack bit s/b low if good transfer
  }
  while ((digitalRead(TOUCH_CLK) == LOW) || (digitalRead(TOUCH_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the touchpad only talks when we're listening
  go_0(TOUCH_CLK);
}
//
// Function to get a byte of data from the touchpad
//
char touch_read(void)
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_touch_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_pu(TOUCH_CLK); // release the clock
  go_pu(TOUCH_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TOUCH_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH; // flag the touchpad as bad
        break; // break out of infinite loop
      }
    }
    if (digitalRead(TOUCH_DATA)) { // check if data is high
      rcv_touch_data = rcv_touch_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TOUCH_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        touchpad_error = HIGH; // flag the touchpad as bad
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TOUCH_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
  // bad parity - pass to future error handler
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TOUCH_DATA) == LOW) { // check if stop bit is bad (low)
  // send bad stop bit to future error handler
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TOUCH_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      touchpad_error = HIGH; // flag the touchpad as bad
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the touchpad only talks when we're listening
  go_0(TOUCH_CLK);
  return rcv_touch_data; // pass the received data back
}
//
void touchpad_init()
{
  touchpad_error = LOW; // start with no error
  go_pu(TOUCH_CLK); // float the clock and data to touchpad
  go_pu(TOUCH_DATA);
  //  Sending reset command to touchpad
  touch_write(0xff);
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
  delay(1000); // wait 1000ms so tp can run its self diagnostic
  //  verify proper response from touchpad
  if (touch_read() != 0xaa) { // verify basic assurance test passed
    touchpad_error = HIGH;
  } 
  if (touch_read() != 0x00) { // verify basic assurance test passed
    touchpad_error = HIGH;
  }
  //  Send touchpad disable code 
  touch_write(0xf5);  // tp disable 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
  // Load Mode Byte with 00 using the following special sequence from page 38.
  // Send set resolution to 0 four times followed by a set sample rate to 0x14
// #1 set resolution
  touch_write(0xe8);  // set resolution 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  touch_write(0x01);  // to zero 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// #2 set resolution
  touch_write(0xe8);  // set resolution 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  touch_write(0x00);  // to zero 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// #3 set resolution
  touch_write(0xe8);  // set resolution 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  touch_write(0x00);  // to zero 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// #4 set resolution
  touch_write(0xe8);  // set resolution 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  touch_write(0x00);  // to zero 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
// Set sample rate
  touch_write(0xf3);  // set sample rate 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  touch_write(0x14);  // to 14 hex 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  // set the resolution 
  touch_write(0xe8); //  Sending resolution command
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  touch_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  // set the sample rate 
  touch_write(0xf3); //  Sending sample rate command
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  touch_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }  
  //  Sending remote mode code so the touchpad will send data only when polled
  touch_write(0xf0);  // remote mode 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  } 
  //  Sending touchpad enable code (needed for Elan touchpads)
  touch_write(0xf4);  // tp enable 
  if (touch_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
}
//************************************Setup*******************************************
void setup() {
// ************trackpoint setup
  trackpoint_init(); // reset trackpoint, then set it's resolution and put it in remote mode 
  if (trackpoint_error) {
    delay(500); // wait 1/2 sec
    trackpoint_init(); // try one more time to initialize the trackpoint
  }  
// ************touchpad setup
  touchpad_init(); // reset touchpad, then set it's resolution and put it in remote mode
  if (touchpad_error) {
    delay(500); // wait 1/2 sec
    touchpad_init(); // try one more time to initialize the touchpad
  }
// ************keyboard setup
  for (int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as a floating output
  }  
//
// Turn off Num lock if it's on at power up. This avoids unwanted numbers showing up instead of letters.
  extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status.
  delay(1000); // wait to let the Teensy receive keyboard_leds byte from host
  if (keyboard_leds & 1) {  // test if bit d0 is set, this is the num lock bit
    Keyboard.press(KEY_NUM_LOCK); // Press the num lock key to de-select it
    delay(5); // delay 5 milliseconds before releasing the key
    Keyboard.release(KEY_NUM_LOCK); // Release the num lock key
  }
//  
  go_pu(HOTKEY);    // Pull up the Fn "Hotkey" input so it can be read. If key pressed, it will be low.
//
  pinMode(BLINK_LED, OUTPUT); // I/O 13 drives the LED on the Teensy as a heartbeat
}
//
// *******declare and initialize trackpoint variables  
char mstat_track; // trackpoint status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
char mx_track; // trackpoint x movement = 8 data bits. The sign bit is in the status register to 
         // make a 9 bit 2's complement value. Left to right on the trackpoint gives a positive value. 
char my_track; // trackpoint y movement also 8 bits plus sign. trackpoint movement away from the user gives a positive value.
boolean over_flow_track; // set if x or y movement values are bad due to overflow
boolean left_button_track = 0; // on/off variable for left button = bit 0 of mstat_track
boolean right_button_track = 0; // on/off variable for right button = bit 1 of mstat_track
boolean middle_button_track = 0; // on/off variable for middle button = bit 2 of mstat_track
boolean old_left_button_track = 0; // on/off variable for left button status the previous polling cycle
boolean old_right_button_track = 0; // on/off variable for right button status the previous polling cycle
boolean old_middle_button_track = 0; // on/off variable for middle button status the previous polling cycle  
boolean button_change_track = 0; // Active high, shows when any trackpoint button has changed since last polling cycle
//
// ********declare and initialize touchpad variables  
  char mstat_touch; // touchpad status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
  char mx_touch; // touchpad x movement = 8 data bits. The sign bit is in the status register to 
           // make a 9 bit 2's complement value. Left to right on the touchpad gives a positive value. 
  char my_touch; // touchpad y movement also 8 bits plus sign. Touchpad movement away from the user gives a positive value.
  boolean over_flow_touch; // set if x or y movement values are bad due to overflow
  boolean left_button_touch = 0; // on/off variable for left button = bit 0 of mstat_touch
  boolean right_button_touch = 0; // on/off variable for right button = bit 1 of mstat_touch
  boolean old_left_button_touch = 0; // on/off variable for left button status the previous polling cycle
  boolean old_right_button_touch = 0; // on/off variable for right button status the previous polling cycle
  boolean button_change_touch = 0; // Active high, shows when a touchpad left or right button has changed since last polling cycle
//
// **********declare and initialize keyboard variables
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status. 
char blink_count = 0; // Blink loop counter
boolean blinky = LOW; // Blink LED state
//
//*********************************Main Loop********************************************
//
void loop() {   
// *************Keyboard Main**************
// Read the Fn key (aka Hotkey) which is not part of the key matrix
  if (!digitalRead(HOTKEY)) {
    Fn_pressed = LOW; // Fn key is pressed (active low)
  }
  else  {
    Fn_pressed = HIGH; // Fn key is not pressed
  }
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
// ***********Normal keys section
      else if (normal[x][y] != 0) {  // check if normal key exists at this location in the array (a non-zero value)
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if ((normal[x][y] == KEY_SCROLL_LOCK) && (!Fn_pressed)) { // check for special case of Num Lock Key
            load_slot(KEY_NUM_LOCK); // update first available slot with Num Lock instead of Scroll Lock
            send_normals(); // send all slots over USB including the Num Lock Key that just got pressed
          }
          else if ((normal[x][y] == KEY_F1) && (!Fn_pressed)) { // check for special case of Fn & F1 to toggle trackpoint on/off
            trackpoint_error = !trackpoint_error; // invert the state of the trackpoint error flag to turn it on or off
            while (!digitalRead(Col_IO[y])) {  // loop until F1 key is released
            delay(1); //
            }
          }
          else if ((normal[x][y] == KEY_F2) && (!Fn_pressed)) { // check for special case of Fn & F2 to toggle touchpad on/off
            touchpad_error = !touchpad_error; // invert the state of the touchpad error flag to turn it on or off
            while (!digitalRead(Col_IO[y])) {  // loop until F2 key is released
            delay(1); //
            }
          }
          else if (keyboard_leds & 1) {  // test if Num Lock is turned on
            load_slot(numlock[x][y]); //update first available slot with key name from numlock matrix
            send_normals(); // send all slots over USB including the key that just got pressed
            }
          else {
            load_slot(normal[x][y]); //update first available slot with normal key name
            send_normals(); // send all slots over USB including the key that just got pressed 
          }
        }          
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          if ((normal[x][y] == KEY_SCROLL_LOCK) && (!Fn_pressed)) { // check for special case of Num Lock Key
            clear_slot(KEY_NUM_LOCK); // clear the slot that contains Num Lock
            send_normals(); // send all slots over USB including the Num Lock key
          }
          else if (keyboard_leds & 1) {  // test if Num Lock is turned on
            clear_slot(numlock[x][y]); //clear slot with key name from numlock matrix
            send_normals(); // send all slots over USB including the key that just got released
          }
          else {
            clear_slot(normal[x][y]); //clear the slot that contains the normal key name
            send_normals(); // send all slots over USB including the key that was just released 
          }
        }
      } 
// **************end of normal section
//
// *************Volume key section. Note PJRC states that volume up, down, & mute should be sent with Keyboard.press function. 
      else if (media[x][y] != 0) {  // check if any volume control key exists at this location in the array (a non-zero value)
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) { // check if key is pressed and was not previously pressed
          old_key[x][y] = LOW; // Save state of key as "pressed" 
          Keyboard.press(media[x][y]); // send volume key press
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed
          old_key[x][y] = HIGH; // Save state of key as "not pressed" 
          Keyboard.release(media[x][y]); // send volume key release 
        }
      }
// ***************end of volume section
    }
    go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
  }
//
// **********keyboard scan complete
//
// ****************************Read the Trackpoint registers*********************************
//
  if (!trackpoint_error) { // check if trackpoint is working and enabled
    // poll the trackpoint for new movement data
    over_flow_track = 0; // assume no overflow until status is received 
    trackp_write(0xeb);  // request data
    if (trackp_read() != 0xfa) { // verify correct ack byte  
      trackpoint_error = HIGH;
    }
    mstat_track = trackp_read(); // save into status variable
    mx_track = trackp_read(); // save into x variable
    my_track = trackp_read(); // save into y variable
    if (((0x80 & mstat_track) == 0x80) || ((0x40 & mstat_track) == 0x40))  {   // x or y overflow bits set?
      over_flow_track = 1; // set the overflow flag
    }   
// change the x data from 9 bit to 8 bit 2's complement
    mx_track = mx_track >> 1; // convert to 7 bits of data by dividing by 2
    mx_track = mx_track & 0x7f; // don't allow sign extension
    if ((0x10 & mstat_track) == 0x10) {   // move the sign into the 8th bit position
      mx_track = 0x80 | mx_track;        
    } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
    my_track = my_track >> 1; // convert to 7 bits of data by dividing by 2
    my_track = my_track & 0x7f; // don't allow sign extension
    if ((0x20 & mstat_track) == 0x20) {   // move the sign into 
      my_track = 0x80 | my_track;              // the 8th bit position
    } 
    my_track = (~my_track + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx_track and my_track if over_flow_track or trackpoint_error is set
    if ((over_flow_track) || (trackpoint_error)) { 
      mx_track = 0x00;       // data is garbage so zero it out
      my_track = 0x00;
    }
  } 
//
// ***********************Read the Touchpad registers
//
  if (touchpad_error == LOW) { // check if touchpad is working and enabled
// poll the touchpad for new movement data
    over_flow_touch = 0; // assume no overflow until status is received 
    touch_write(0xeb);  // request data
    if (touch_read() != 0xfa) { // verify correct ack byte
      touchpad_error = HIGH;
    }
    mstat_touch = touch_read(); // save into status variable
    mx_touch = touch_read(); // save into x variable
    my_touch = touch_read(); // save into y variable
    if (((0x80 & mstat_touch) == 0x80) || ((0x40 & mstat_touch) == 0x40))  {   // x or y overflow bits set?
      over_flow_touch = 1; // set the overflow flag
    }   
// change the x data from 9 bit to 8 bit 2's complement
    mx_touch = mx_touch & 0x7f; // mask off 8th bit
    if ((0x10 & mstat_touch) == 0x10) {   // move the sign into the 8th bit position
      mx_touch = 0x80 | mx_touch;          
    } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
    my_touch = my_touch & 0x7f; // mask off 8th bit
    if ((0x20 & mstat_touch) == 0x20) {   // move the sign into 
      my_touch = 0x80 | my_touch;              // the 8th bit position
    } 
    my_touch = (~my_touch + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx_touch and my_touch if over_flow_touch or touchpad_error is set
    if ((over_flow_touch) || (touchpad_error)) { 
      mx_touch = 0x00;       // data is garbage so zero it out
      my_touch = 0x00;
    } 
  }
// send the x and y trackpoint data over USB if x or y is non-zero
  if ((mx_track != 0x00) || (my_track != 0x00)) {
    Mouse.move(mx_track,my_track);
  }
// if no trackpoint data, send the x and y touchpad data over USB if x or y is non-zero
  else if ((mx_touch != 0x00) || (my_touch != 0x00)) {
    Mouse.move(mx_touch,my_touch);
  }
//
// trackpoint button status 
//  
  if ((0x01 & mstat_track) == 0x01) {   // if left button set 
    left_button_track = 1;   
  }
  else {   // clear left button
    left_button_track = 0;   
  }
  if ((0x02 & mstat_track) == 0x02) {   // if right button set 
    right_button_track = 1;   
  } 
  else {   // clear right button
    right_button_track = 0;  
  }
  if ((0x04 & mstat_track) == 0x04) {   // if middle button set 
    middle_button_track = 1;   
  } 
  else {   // clear middle button
    middle_button_track = 0;  
  }    
  // Determine if any trackpoint buttons have changed since last polling cycle
  button_change_track = ((left_button_track ^ old_left_button_track) | (right_button_track ^ old_right_button_track) | (middle_button_track ^ old_middle_button_track));
// touchpad button status
  if ((0x01 & mstat_touch) == 0x01) {   // if left button set 
    left_button_touch = 1;   
  }
  else {   // clear left button
    left_button_touch = 0;   
  }
  if ((0x02 & mstat_touch) == 0x02) {   // if right button set 
    right_button_touch = 1;   
  } 
  else {   // clear right button
    right_button_touch = 0;  
  }
  // Determine if any touchpad buttons have changed since last polling cycle
  button_change_touch = (left_button_touch ^ old_left_button_touch) | (right_button_touch ^ old_right_button_touch);
//
// Send trackpoint button status if there was a change since last time. 
  if (button_change_track) {
    Mouse.set_buttons(left_button_track, middle_button_track, right_button_track); // send button status
    old_left_button_track = left_button_track; // remember new button status for next polling cycle
    old_right_button_track = right_button_track;
    old_middle_button_track = middle_button_track;
  }
// if no button change from trackpoint, check if touchpad buttons have changed
  else if (button_change_touch){
    Mouse.set_buttons(left_button_touch, 0, right_button_touch); // send button status
    old_left_button_touch = left_button_touch; // remember new button status for next polling cycle
    old_right_button_touch = right_button_touch;
  } 
//
// **************************************End of trackpoint/touchpad routine***********************************
//  
// *******keyboard LEDs
// Turn on or off the LEDs for Num Lock and Caps Lock based on bit 0 and 1 from the keyboard_leds 
// variable controlled by the USB host computer
//
  if (keyboard_leds & 1) {  // mask off all bits but D0 and test if set
    go_1(NUM_LED); // turn on the Num Lock LED
  }
  else {
    go_0(NUM_LED); // turn off the Num Lock LED
  }
//
//
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_1(CAPS_LED); // turn on the Caps Lock LED
  }
  else {
    go_0(CAPS_LED); // turn off the Caps Lock LED
  }
//
// Blink LED on Teensy to show a heart beat
//
  if (blink_count == 0x17) {  
    digitalWrite(BLINK_LED, blinky);
    blinky = !blinky;
    blink_count = 0;
  }
  else {
    blink_count = blink_count + 1;
  }
//
// ****************End of main loop
//
  delay(24); // The overall keyboard/trackpoint scanning rate is approximately 30ms
}
