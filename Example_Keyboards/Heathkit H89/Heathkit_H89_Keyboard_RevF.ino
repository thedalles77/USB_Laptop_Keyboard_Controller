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
// This software implements a Heathkit H89 Keyboard Controller using a Teensy directly soldered to
// a 34 pin ribbon cable connected to the pins on the keyboard. 
// 
// The project originally used a Teensy 3.2 (see PDF) but the code will also work with a Teensy 4.0 or 4.1
// No I/O translation is required because the keyboard is directly wired to the Teensy.
// 
// Revision History
// Initial Release June 16, 2020
// Rev A June 17, 2020 - Fixed row pin 18 that shows up twice in schematic
// Rev B June 17, 2020 - Fixed the zero key
// Rev C June 18, 2020 - Changed how Caps lock is reported (the mechanical switch locks down) 
// Rev D June 18, 2020 - Fixed semicolon
// Rev E June 19, 2020 - Really fixed the zero and oh keys
// Rev F 
#define MODIFIERKEY_FN 0x8f   // give Fn key a fake HID code 
#define CAPS_LED 13 // Teensy LED shows Caps-Lock
#define NUM_LED 32 // Wire a Num Lock LED with a current limit resistor to this I/O pin
#define SCRL_LED 33 // Wire a Scroll Lock LED with a current limit resistor to this I/O pin
#define CONTROL_KEY 11 // I/O 11 is ribbon pin 15 - Control key
#define SHIFT_RIGHT_KEY 17 // I/O 17 is ribbon 21 - Shift Right key
#define SHIFT_LEFT_KEY 20 // I/O 20 is ribbon 25 - Shift Left key
#define CAPS_LOCK_KEY 9 // I/O 9 is ribbon 13 - Caps Lock key
#define NUM_LOCK_KEY 15 // I/O 15 is ribbon 19 - Num Lock key but is labeled BREAK
//
const byte rows_max = 10; // sets the number of rows in the matrix
const byte cols_max = 9; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names 
// described at www.pjrc.com/teensy/td_keyboard.html 
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {KEY_9,KEY_O,KEY_L,KEY_PERIOD,0,KEYPAD_9,KEYPAD_6,KEYPAD_3,KEYPAD_ENTER},
  {KEY_8,KEY_I,KEY_K,KEY_COMMA,0,KEYPAD_8,KEYPAD_5,KEYPAD_2,KEYPAD_PERIOD},
  {KEY_7,KEY_U,KEY_J,KEY_M,0,KEYPAD_7,KEYPAD_4,KEYPAD_1,KEYPAD_0},
  {KEY_6,KEY_Y,KEY_H,KEY_N,KEY_DELETE,0,0,0,0},    
  {KEY_5,KEY_T,KEY_G,KEY_B,KEY_F5,KEY_BACKSPACE,KEY_DELETE,0,0},
  {KEY_4,KEY_R,KEY_F,KEY_V,KEY_F4,KEY_TILDE,KEY_DOWN,KEY_ENTER,0},
  {KEY_3,KEY_E,KEY_D,KEY_C,KEY_F3,KEY_EQUAL,KEY_BACKSLASH,KEY_RIGHT_BRACE,0},
  {KEY_2,KEY_W,KEY_S,KEY_X,KEY_F2,KEY_MINUS,KEY_LEFT_BRACE,KEY_QUOTE,0},
  {KEY_1,KEY_Q,KEY_A,KEY_Z,KEY_F1,KEY_0,KEY_P,KEY_SEMICOLON,KEY_SLASH},
  {KEY_ESC,KEY_TAB,KEY_SCROLL_LOCK,KEY_SPACE,0,0,0,0,0}
};

// Initialize the old_key matrix with one's. 
// 1 = key not pressed, 0 = key is pressed
boolean old_key[rows_max][cols_max] = {
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1}
};
//
// Define the Teensy 3.2 I/O numbers (translated from the ribbon pin #)
// Ribbon Cable# 14,16,18,20,22,24,26,28,30,32
// Teensy I/O  # 10,12,14,16,18,19,21,23,24,26
int Row_IO[rows_max] = {10,12,14,16,18,19,21,23,24,26}; // Teensy 3.2 I/O numbers for rows
//
// Ribbon Cable   # 02,04,06,08,10,03,05,07,09
// Teensy I/O     # 00,02,04,06,08,01,03,05,07
int Col_IO[cols_max] = {0,2,4,6,8,1,3,5,7};  // Teensy 3.2 I/O numbers for columns

// Declare variables that will be used by functions
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
//
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
}
//
// Function to send the modifier keys over usb
void send_mod() {
  Keyboard.set_modifier(mod_shift_l | mod_shift_r | mod_ctrl_l);
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
//----------------------------------Setup-------------------------------------------
void setup() {
  for (int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as a floating output
  }  
//
  go_pu(CONTROL_KEY); // set I/O 11 as an input with a pullup (this is ribbon pin 15 - Control key
  go_pu(SHIFT_RIGHT_KEY); // set I/O 17 as an input with a pullup (this is ribbon pin 21 - Shift Right key
  go_pu(SHIFT_LEFT_KEY); // set I/O 20 as an input with a pullup (this is ribbon pin 25 - Shift Left key
  go_pu(CAPS_LOCK_KEY);  // set I/O 9 as an input with a pullup (this is ribbon pin 13 - Caps Lock key
  go_pu(NUM_LOCK_KEY);   // set I/O 15 as an input with a pullup (this is ribbon pin 19 - Num Lock key but labeled BREAK
}
//
extern volatile uint8_t keyboard_leds; // 8 bits sent from Pi to Teensy that give keyboard LED status. Caps lock is bit D1.
//
boolean Cntrl_pressed = HIGH; // Initialize Control key to HIGH = "not pressed"
boolean ShiftR_pressed = HIGH; // Initialize Shift right key to HIGH = "not pressed"
boolean ShiftL_pressed = HIGH; // Initialize Shift left key to HIGH = "not pressed"
boolean NumLck_pressed = HIGH; // Initialize Num Lock key to HIGH = "not pressed"
boolean CapsLck_pressed = HIGH; // Initialize Caps Lock key to HIGH = "not pressed"
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
// Modifier Keys - Control and Shift right and left are not part of the matrix
  // **********Control Key*********
  if (!digitalRead(CONTROL_KEY) && (Cntrl_pressed)) { //check if control key was just pressed
    Cntrl_pressed = LOW; // save state as pressed
    load_mod(MODIFIERKEY_LEFT_CTRL); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable
    send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
  }
  else if (digitalRead(CONTROL_KEY) && (!Cntrl_pressed)) { //check if control key is not pressed and was previously pressed
    Cntrl_pressed = HIGH; // save state as not pressed
    clear_mod(MODIFIERKEY_LEFT_CTRL); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
    send_mod(); // function sends all mod's over usb including the one that just released
  }
  // **********Shift Right Key*********
  if (!digitalRead(SHIFT_RIGHT_KEY) && (ShiftR_pressed)) { //check if shift right key was just pressed
    ShiftR_pressed = LOW; // save state as pressed
    load_mod(MODIFIERKEY_RIGHT_SHIFT); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable
    send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
  }
  else if (digitalRead(SHIFT_RIGHT_KEY) && (!ShiftR_pressed)) { //check if shift right key is not pressed and was previously pressed
    ShiftR_pressed = HIGH; // save state as not pressed
    clear_mod(MODIFIERKEY_RIGHT_SHIFT); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
    send_mod(); // function sends all mod's over usb including the one that just released
  }    
  // **********Shift Left Key*********
  if (!digitalRead(SHIFT_LEFT_KEY) && (ShiftL_pressed)) { //check if shift left key was just pressed
    ShiftL_pressed = LOW; // save state as pressed
    load_mod(MODIFIERKEY_LEFT_SHIFT); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable
    send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
  }
  else if (digitalRead(SHIFT_LEFT_KEY) && (!ShiftL_pressed)) { //check if shift left key is not pressed and was previously pressed
    ShiftL_pressed = HIGH; // save state as not pressed
    clear_mod(MODIFIERKEY_LEFT_SHIFT); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
    send_mod(); // function sends all mod's over usb including the one that just released
  }   
// ***********end of modifier key section
//
// Special keys
//
// *******Num Lock is implemented using the Break Key which is not part of the matrix*******
  if (!digitalRead(NUM_LOCK_KEY) && (NumLck_pressed)) { //check if Num lock key was just pressed
    NumLck_pressed = LOW; // save state as pressed
    load_slot(KEY_NUM_LOCK); // //update first available slot with num lock    
    send_normals(); // send all slots over USB including the num lock key that was just pressed
  }
  else if (digitalRead(NUM_LOCK_KEY) && (!NumLck_pressed)) { //check if num lock key is not pressed and was previously pressed
    NumLck_pressed = HIGH; // save state as not pressed
    clear_slot(KEY_NUM_LOCK); //clear the slot that contains the num lock
    send_normals(); // send all slots over USB including the num lock that was just released
  } 
// *******Caps Lock is a mechanical key switch that will lock in the down position*******
  if (!digitalRead(CAPS_LOCK_KEY) && (CapsLck_pressed)) { //capture when it's first pushed and locked down
    CapsLck_pressed = LOW; // save state as pressed
    Keyboard.press(KEY_CAPS_LOCK); // send caps lock over usb    
    delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
    Keyboard.release(KEY_CAPS_LOCK); // send a caps lock release over usb (even though it's stuck in the down position)
  }
  else if (digitalRead(CAPS_LOCK_KEY) && (!CapsLck_pressed)) { //capture when it's first pushed to release the locked down state
    CapsLck_pressed = HIGH; // save state as not pressed
    Keyboard.press(KEY_CAPS_LOCK); // send caps lock over usb     
    delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
    Keyboard.release(KEY_CAPS_LOCK); // send a caps lock release over usb
  }
//
// ***********Normal keys in this section  
  for (int x = 0; x < rows_max; x++) {   // loop thru the rows
    go_0(Row_IO[x]); // Activate Row (send it low)
    delayMicroseconds(10); // give the row time to go low and settle out
    for (int y = 0; y < cols_max; y++) {   // loop thru the columns
      if (normal[x][y] != 0) {  // check if normal key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          load_slot(normal[x][y]); //update first available slot with normal key name
          send_normals(); // send all slots over USB including the key that just got pressed
        }          
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed 
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          clear_slot(normal[x][y]); //clear the slot that contains the normal key name
          send_normals(); // send all slots over USB including the key that was just released 
        }
      } 
// **************end of normal key section 
//
    }
    go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
  }
//
// **********keyboard scan complete
//
// Turn on or off the LEDs for Num Lock, Caps Lock, and Scroll Lock based on bit 0, 1, and 2 from the keyboard_leds 
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
//
  if (keyboard_leds & 1<<2) {  // mask off all bits but D2 and test if set
    go_1(SCRL_LED); // turn on the Scroll Lock LED
  }
  else {
    go_0(SCRL_LED); // turn off the Scroll Lock LED
  }
//

  delay(25); // The overall keyboard scanning rate is about 30ms
}
