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
// This software controls a Lenovo ThinkPad T61 Laptop Keyboard only. Use this routine first
// and after getting the keyboard to work, try adding the PS/2 Trackpoint and PS/2 Touchpad.
//
// Revision History
// Rev New - Nov 30 , 2023 - Original Release
//
// Keyboard LEDs
#define CAPS_LED 2   // Wire these 3 I/O's to the anode side of LED's 
#define NUM_LED 3    // Wire the cathode side thru a dropping resistor
#define BLINK_LED 13  // The LED on the Teensy is programmed to blink 
// Keyboard Fn key (aka HOTKEY)
#define HOTKEY 18       // Fn key 
// Set the keyboard row & column size
const byte rows_max = 17; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {0,KEY_SCROLL_LOCK,KEY_PRINTSCREEN,0,0,0,0,0},
  {KEY_HOME,0,KEY_END,KEY_LEFT,KEY_PAUSE,0,0,KEY_UP},
  {0,0,0,KEY_PAGE_DOWN,KEY_PAGE_UP,0,0,0},
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
  {KEY_6,KEY_U,KEY_7,KEY_N,KEY_M,KEY_J,KEY_Y,KEY_H},
  {KEY_PAGE_UP,0,KEY_PAGE_DOWN,KEY_PAGE_DOWN,KEY_PAGE_UP,KEY_MENU,0,0}
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
  {KEY_6,KEYPAD_4,KEYPAD_7,KEY_N,KEYPAD_0,KEYPAD_1,KEY_Y,KEY_H},
  {KEY_PAGE_UP,0,KEY_PAGE_DOWN,KEY_PAGE_DOWN,KEY_PAGE_UP,KEY_MENU,0,0}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// Access IBM key is used as the Windows GUI key.
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,0,0,MODIFIERKEY_RIGHT_ALT,0,0,0,MODIFIERKEY_LEFT_ALT},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
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
  {0,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_GUI,0,0,0,0,0,0}
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
  {1,1,1,1,1,1,1,1}
};
//
// Define the Teensy 4.0 I/O numbers 
//
// Row FPC pin # 32,28,24,20,16,12,08,04,22,26,30,18,14,10,06,02,24
// Teensy I/O  # 10,30,08,24,29,27,16,09,26,28,32,31,17,25,33,15,14
int Row_IO[rows_max] = {10,30,8,24,29,27,16,9,26,28,32,31,17,25,33,15,14}; // Teensy 4.0 I/O numbers for rows
//
// Column FPC pin # 05,09,13,17,15,11,07,03
// Teensy I/O     # 07,06,20,21,04,05,19,08
int Col_IO[cols_max] = {7,6,4,23,22,5,19,8};  // Teensy 4.0 I/O numbers for columns
//
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
  delay(24); // The overall keyboard scanning rate is approximately 30ms
}
