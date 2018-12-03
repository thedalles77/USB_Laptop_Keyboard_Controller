/* Copyright 2018 Frank Adams
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
// This software controls a Lenovo ThinkPad T61 Laptop Keyboard and PS/2 Trackpoint using a Teensy LC on 
// a daughterboard with a 44 pin FPC connector. The keyboard part number is 42T3177.
// This routine uses the Teensyduino "Micro-Manager Method" to send Normal and Modifier 
// keys over USB. Only the volume control multi-media keys are supported by this routine.
// Description of Teensyduino keyboard functions is at www.pjrc.com/teensy/td_keyboard.html
// The ps/2 code uses the USB PJRC Mouse functions at www.pjrc.com/teensy/td_mouse.html 
// The ps/2 code has a watchdog timer so the code can't hang if a clock edge is missed.
// In the Arduino IDE, select Tools, Teensy LC. Also under Tools, select Keyboard+Mouse+Joystick
//
// Revision History
// Rev 1.0 - Nov 20, 2018 - Original Release
// Rev 1.1 - Dec 2, 2018 - Replaced ps/2 trackpoint code from playground arduino with my own code 
//
#define MODIFIERKEY_FN 0x8f   // give Fn key a HID code
// Trackpoint signals
#define TP_DATA 0   // ps/2 data to trackpoint (via BSS138 level translator)
#define TP_CLK 1    // ps/2 clock to trackpoint (via BSS138 level translator)
// Keyboard LEDs
#define CAPS_LED 13   // includes 715 ohm resistor for 2ma LED 
// Set the keyboard row & column size
const byte rows_max = 16; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
//
int normal[rows_max][cols_max] = {
  {KEY_TILDE,KEY_1,KEY_Q,KEY_TAB,KEY_A,KEY_ESC,KEY_Z,0},
  {KEY_F1,KEY_2,KEY_W,KEY_CAPS_LOCK,KEY_S,0,KEY_X,0},
  {KEY_F2,KEY_3,KEY_E,KEY_F3,KEY_D,KEY_F4,KEY_C,0},
  {KEY_5,KEY_4,KEY_R,KEY_T,KEY_F,KEY_G,KEY_V,KEY_B},
  {KEY_6,KEY_7,KEY_U,KEY_Y,KEY_J,KEY_H,KEY_M,KEY_N},
  {KEY_EQUAL,KEY_8,KEY_I,KEY_RIGHT_BRACE,KEY_K,KEY_F6,KEY_COMMA,0},
  {KEY_F8,KEY_9,KEY_O,KEY_F7,KEY_L,0,KEY_PERIOD,0},
  {KEY_MINUS,KEY_0,KEY_P,KEY_LEFT_BRACE,KEY_SEMICOLON,KEY_QUOTE,0,KEY_SLASH},
  {KEY_F9,KEY_F10,0,KEY_BACKSPACE,KEY_BACKSLASH,KEY_F5,KEY_ENTER,KEY_SPACE},
  {KEY_INSERT,KEY_F12,0,0,0,0,0,KEY_RIGHT},  
  {KEY_DELETE,KEY_F11,0,0,0,0,0,KEY_DOWN},
  {KEY_PAGE_UP,KEY_PAGE_DOWN,0,0,KEY_MENU,0,0,0},
  {KEY_HOME,KEY_END,0,0,0,KEY_UP,KEY_PAUSE,KEY_LEFT},
  {0,KEY_PRINTSCREEN,KEY_SCROLL_LOCK,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,MODIFIERKEY_FN,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,MODIFIERKEY_GUI,0,0,0,0,0},    
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,MODIFIERKEY_LEFT_ALT,0,MODIFIERKEY_RIGHT_ALT},
  {0,0,0,MODIFIERKEY_LEFT_SHIFT,0,0,MODIFIERKEY_RIGHT_SHIFT,0},
  {MODIFIERKEY_LEFT_CTRL,0,0,0,0,0,MODIFIERKEY_RIGHT_CTRL,0}
};
// Load the media key matrix with key names at the correct row-column location. 
// A zero indicates no media key at that location.
int media[rows_max][cols_max] = {
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
  {0,0,KEY_MEDIA_VOLUME_INC,KEY_MEDIA_VOLUME_DEC,KEY_MEDIA_MUTE,0,0,0},
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
// Define the Teensy 3.2 I/O numbers 
//
// Row FPC pin # 22,18,14,10,02,04,08,12,06,20,16,24,28,32,26,30
// Teensy I/O  # 15,20,19,18,16,08,07,06,17,09,05,10,11,12,14,26
int Row_IO[rows_max] = {15,20,19,18,16,8,7,6,17,9,5,10,11,12,14,26}; // Teensy LC I/O numbers for rows
//
// Column FPC pin # 05,13,09,07,11,03,15,17
// Teensy I/O     # 21,23,22,25,24,04,03,02
int Col_IO[cols_max] = {21,23,22,25,24,4,3,2};  // Teensy LC I/O numbers for columns
//
// Declare variables that will be used by functions
boolean trackpoint_error = LOW; // sent high when touch pad routine times out
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
// **************Functions common to keyboard and trackpoint**************************
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
// *****************Functions for Trackpoint***************************
//
// Function to send the trackpoint a byte of data (command)
//
void tp_write(char send_data)  
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_z(TP_CLK); //
  go_z(TP_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TP_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TP_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_z(TP_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_z(TP_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TP_DATA); // send a 0 to TP
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
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
    go_z(TP_DATA); // send a 1 to TP to make parity odd
  }   
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  go_z(TP_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TP_DATA)) { // Ack bit s/b low if good transfer
    trackpoint_error = HIGH; //bad ack bit so set the error flag
  }
  while ((digitalRead(TP_CLK) == LOW) || (digitalRead(TP_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
}
//
// Function to get a byte of data from the trackpoint
//
char tp_read(void)
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_z(TP_CLK); // release the clock
  go_z(TP_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
    trackpoint_error = HIGH; // No start bit so set the error flag
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
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
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    trackpoint_error = HIGH; //bad parity so set the error flag
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
    trackpoint_error = HIGH; //bad stop bit so set the error flag
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
  return rcv_data; // pass the received data back
}
//
void trackpoint_init()
{
  trackpoint_error = LOW; // start with no error
  go_z(TP_CLK); // float the clock and data to trackpoint
  go_z(TP_DATA);
  delay(1000); // wait 1 second to let power on reset release
  //  Sending reset command to trackpoint
  tp_write(0xff);
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  delayMicroseconds(100); // give the tp time to run its self diagnostic
  //  verify proper response from tp
  if (tp_read() != 0xaa) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  } 
  if (tp_read() != 0x00) { // verify correct device id
    trackpoint_error = HIGH;
  }
  //  Sending remote mode code so the trackpoint will send data only when polled
  tp_write(0xf0);  // remote mode 
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  if (trackpoint_error == HIGH) { // check for any errors from tp
    delayMicroseconds(300); // wait before trying to initialize tp one last time
    tp_write(0xff); // send tp reset code
    tp_read();  // read but don't look at response from tp
    //  Read ack byte
    tp_read();  // read but don't look at response from tp 
    tp_read();  // read but don't look at response from tp 
    //  Sending remote mode code so the trackpoint will send data only when polled
    tp_write(0xf0);  // remote mode 
    tp_read();  // read but don't look at response from tp 
    delayMicroseconds(100);
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
  slots_full = LOW;
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
//************************************Setup*******************************************
void setup() {
// ************trackpoint setup
  trackpoint_init(); // reset trackpoint, then set it's resolution and put it in remote mode 
  if (trackpoint_error) {
    trackpoint_init(); // try one more time to initialize the trackpoint
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
}
//
// *******declare and initialize trackpoint variables  
char mstat; // trackpoint status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
char mx; // trackpoint x movement = 8 data bits. The sign bit is in the status register to 
         // make a 9 bit 2's complement value. Left to right on the trackpoint gives a positive value. 
char my; // trackpoint y movement also 8 bits plus sign. trackpoint movement away from the user gives a positive value.
boolean over_flow; // set if x or y movement values are bad due to overflow
boolean left_button = 0; // on/off variable for left button = bit 0 of mstat
boolean right_button = 0; // on/off variable for right button = bit 1 of mstat
boolean middle_button = 0; // on/off variable for middle button = bit 2 of mstat
boolean old_left_button = 0; // on/off variable for left button status the previous polling cycle
boolean old_right_button = 0; // on/off variable for right button status the previous polling cycle
boolean old_middle_button = 0; // on/off variable for middle button status the previous polling cycle  
boolean button_change = 0; // Active high, shows when any trackpoint button has changed since last polling cycle
// **********declare and initialize keyboard variables
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status. 
//
//*********************************Main Loop*******************************************
//
void loop() {   
// *************Keyboard Main**************
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
          if (modifier[x][y] != MODIFIERKEY_FN) {   // Exclude Fn modifier key
            load_mod(modifier[x][y]); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable   
            send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
            old_key[x][y] = LOW; // Save state of key as "pressed"
          }
          else {
            Fn_pressed = LOW; // Fn status variable is active low
            old_key[x][y] = LOW; // old_key state is "pressed" (active low)
          }
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) {  //check if key is not pressed and was previously pressed
          if (modifier[x][y] != MODIFIERKEY_FN) { // Exclude Fn modifier key 
            clear_mod(modifier[x][y]); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
            send_mod(); // function sends all mod's over usb including the one that just released
            old_key[x][y] = HIGH; // Save state of key as "not pressed"
          }
          else {
            Fn_pressed = HIGH; // Fn is no longer active
            old_key[x][y] = HIGH; // old_key state is "not pressed"
          }
        }
      } 
// ***********end of modifier section
//
// ***********Normal keys section
      else if (normal[x][y] != 0) {  // check if normal key exists at this location in the array (a non-zero value)
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) { // check if key is pressed and was not previously pressed
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if ((normal[x][y] == KEY_SCROLL_LOCK) && (!Fn_pressed)) { // check for special case of Num Lock Key
            load_slot(KEY_NUM_LOCK); // update first available slot with Num Lock instead of Scroll Lock
            send_normals(); // send all slots over USB including the Num Lock Key that just got pressed
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
// ****************************Trackpoint Routine*********************************
//
// poll the trackpoint for new movement data
  over_flow = 0; // assume no overflow until status is received 
  trackpoint_error = LOW; // start with no error
  tp_write(0xeb);  // request data
  if (tp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  mstat = tp_read(); // save into status variable
  mx = tp_read(); // save into x variable
  my = tp_read(); // save into y variable
  if (((0x80 & mstat) == 0x80) || ((0x40 & mstat) == 0x40))  {   // x or y overflow bits set?
    over_flow = 1; // set the overflow flag
  }   
// change the x data from 9 bit to 8 bit 2's complement
  mx = mx >> 1; // convert to 7 bits of data by dividing by 2
  mx = mx & 0x7f; // don't allow sign extension
  if ((0x10 & mstat) == 0x10) {   // move the sign into 
    mx = 0x80 | mx;              // the 8th bit position
  } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
  my = my >> 1; // convert to 7 bits of data by dividing by 2
  my = my & 0x7f; // don't allow sign extension
  if ((0x20 & mstat) == 0x20) {   // move the sign into 
    my = 0x80 | my;              // the 8th bit position
  } 
  my = (~my + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx and my if over_flow or trackpoint_error is set
  if ((over_flow) || (trackpoint_error)) { 
    mx = 0x00;       // data is garbage so zero it out
    my = 0x00;
  } 
// send the x and y data back via usb if either one is non-zero
  if ((mx != 0x00) || (my != 0x00)) {
    Mouse.move(mx,my);
  }
//
// send the trackpoint left and right button status over usb if no error
  if (!trackpoint_error) {
    if ((0x01 & mstat) == 0x01) {   // if left button set 
      left_button = 1;   
    }
    else {   // clear left button
      left_button = 0;   
    }
    if ((0x02 & mstat) == 0x02) {   // if right button set 
      right_button = 1;   
    } 
    else {   // clear right button
      right_button = 0;  
    }
    if ((0x04 & mstat) == 0x04) {   // if middle button set 
      middle_button = 1;   
    } 
    else {   // clear middle button
      middle_button = 0;  
    }    
// Determine if any buttons have changed since last polling cycle
    button_change = ((left_button ^ old_left_button) | (right_button ^ old_right_button) | (middle_button ^ old_middle_button));
// Don't send button status if there's no change since last time. 
    if (button_change) {
      Mouse.set_buttons(left_button, middle_button, right_button); // send button status
    }
    old_left_button = left_button; // remember new button status for next polling cycle
    old_right_button = right_button;
    old_middle_button = middle_button;
  }
// **************************************End of trackpoint routine***********************************
//
// *******keyboard LEDs
// Turn on or off the LED for Caps Lock based on bit 1 from the keyboard_leds variable controlled by the USB host computer
//
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_1(CAPS_LED); // turn on the Caps Lock LED
  }
  else {
    go_0(CAPS_LED); // turn off the Caps Lock LED
  }
//
// ****************End of main loop
//
  delay(24); // The overall keyboard/trackpoint scanning rate was measured at 30ms
}
