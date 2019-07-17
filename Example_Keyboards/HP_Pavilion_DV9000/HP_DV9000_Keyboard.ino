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
// This software implements an HP Pavilion DV9000 Laptop Keyboard Controller using a Teensy LC on 
// a daughterboard with a 26 pin FPC connector. The keyboard part number is AEAT5U00110.
// This routine uses the Teensyduino "Micro-Manager Method" to send Normal and Modifier 
// keys over USB. Multi-media keys are sent with keyboard press and release functions.
// Description of Teensyduino keyboard functions is at www.pjrc.com/teensy/td_keyboard.html
// 
// Revision History
// Initial Release Nov 18, 2018
// Rev 1 July 16, 2019 - check if slots are full when detecting a key press
//
#define MODIFIERKEY_FN 0x8f   // give Fn key a fake HID code 
#define CAPS_LED 13 // Teensy LED shows Caps-Lock
//
const byte rows_max = 18; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names 
// described at www.pjrc.com/teensy/td_keyboard.html 
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {KEYPAD_7,KEYPAD_PLUS,KEYPAD_9,KEYPAD_2,KEYPAD_3,KEYPAD_1,KEYPAD_ENTER,KEYPAD_8},
  {KEYPAD_SLASH,KEYPAD_6,KEYPAD_MINUS,KEYPAD_5,KEYPAD_PERIOD,KEYPAD_4,KEYPAD_0,KEYPAD_ASTERIX},
  {KEY_DELETE,KEY_PAGE_UP,KEY_HOME,KEY_END,KEY_PAGE_DOWN,KEY_RIGHT,KEY_LEFT,KEY_INSERT},
  {KEY_EQUAL,KEY_MINUS,KEY_LEFT_BRACE,KEY_SLASH,KEY_SEMICOLON,KEY_0,KEY_P,KEY_QUOTE},
  {KEY_F12,KEY_F9,KEY_F10,KEY_PERIOD,KEY_L,KEY_9,KEY_O,KEY_F11},
  {0,KEY_BACKSLASH,KEY_NUM_LOCK,KEY_SPACE,KEY_ENTER,KEY_DOWN,KEY_UP,KEY_BACKSPACE},
  {0,0,0,0,0,0,0,0},
  {0,KEY_MENU,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,KEY_RIGHT_BRACE,KEY_COMMA,KEY_K,KEY_8,KEY_I,0},
  {KEY_N,KEY_6,KEY_Y,KEY_M,KEY_J,KEY_7,KEY_U,KEY_H},
  {0,0,0,0,0,0,0,0},    
  {KEY_B,KEY_5,KEY_T,KEY_V,KEY_F,KEY_4,KEY_R,KEY_G},
  {KEY_F1,KEY_F2,KEY_F3,KEY_C,KEY_D,KEY_3,KEY_E,KEY_F4},
  {KEY_CAPS_LOCK,KEY_TILDE,KEY_TAB,KEY_Z,KEY_A,KEY_1,KEY_Q,KEY_ESC},
  {KEY_F8,KEY_F5,KEY_F6,KEY_X,KEY_S,KEY_2,KEY_W,KEY_F7},
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
  {MODIFIERKEY_RIGHT_ALT,0,0,0,0,MODIFIERKEY_LEFT_ALT,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,MODIFIERKEY_GUI},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_FN,0,0,0,0,0,0},    
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,MODIFIERKEY_LEFT_CTRL,0,MODIFIERKEY_RIGHT_CTRL,0,0,0},
  {0,0,0,MODIFIERKEY_LEFT_SHIFT,0,0,MODIFIERKEY_RIGHT_SHIFT,0}
};
// Load the media key matrix with Fn key names at the correct row-column location. 
// A zero indicates no media key at that location.
int media[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,KEY_PAUSE,KEY_PRINTSCREEN,0,0,0,0,KEY_SCROLL_LOCK},
  {0,0,0,0,0,0,0,0},
  {KEY_MEDIA_NEXT_TRACK,KEY_MEDIA_PLAY_PAUSE,KEY_MEDIA_STOP,0,0,0,0,KEY_MEDIA_PREV_TRACK},
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
  {0,KEY_SYSTEM_SLEEP,0,0,0,0,0,0},
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
// Define the Teensy LC I/O numbers (translated from the FPC pin #)
// Row FPC pin # 01,02,03,04,05,06,07,08,09,10,11,12,13,14,16,17,20,23
// Teensy I/O  # 23,00,22,01,24,02,21,03,25,04,20,05,19,06,07,17,09,14
int Row_IO[rows_max] = {23,0,22,1,24,2,21,3,25,4,20,5,19,6,7,17,9,14}; // Teensy LC I/O numbers for rows
//
// Column FPC pin # 15,18,19,21,22,24,25,26
// Teensy I/O     # 18,08,16,15,10,11,26,12
int Col_IO[cols_max] = {18,8,16,15,10,11,26,12};                       // Teensy LC I/O numbers for columns

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
}
//
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
extern volatile uint8_t keyboard_leds; // 8 bits sent from Pi to Teensy that give keyboard LED status. Caps lock is bit D1.
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
// **********Modifier keys including the Fn special case
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
// ***********Normal keys and media keys in this section
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {  // check if normal or media key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn_pressed is active low so it is not pressed and normal key needs to be sent
            load_slot(normal[x][y]); //update first available slot with normal key name
            send_normals(); // send all slots over USB including the key that just got pressed
          }
          else if (media[x][y] != 0) { // Fn is pressed so send media if a key exists in the matrix
            Keyboard.press(media[x][y]); // media key is sent using keyboard press function per PJRC    
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
//
    }
    go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
  }
//
// **********keyboard scan complete
//
// Turn on the LED on the Teensy for Caps Lock based on bit 1 in the keyboard_leds variable controlled by the USB host computer
//
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_1(CAPS_LED); // turn on the LED
  }
  else {
    go_0(CAPS_LED); // turn off the LED
  }
//
  delay(25); // The overall keyboard scanning rate is about 30ms
}
