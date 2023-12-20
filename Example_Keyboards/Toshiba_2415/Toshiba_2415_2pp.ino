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
// This software creates a USB keyboard from a Toshiba 2415 keyboard using a Teensy 2++
//
// Revision History
// Initial Release Dec 19, 2023   (translated from the 3.2 code)
//
#define MODIFIERKEY_FN 0x8f   // give Fn key a fake HID code 
#define KEY_ARROW_LOCK KEY_F13  // give arrow lock key an unused code
// LED I/O connections
#define CAPS_LED 16
#define NUM_LED 23
#define SCRL_LED 13 // this is the LED on the Teensy
#define ARROW_LED 2
//
const byte rows_max = 16; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names 
// described at www.pjrc.com/teensy/td_keyboard.html 
// A zero indicates no normal key at that location.
unsigned int normal[rows_max][cols_max] = {
  {KEY_CAPS_LOCK,KEY_A,KEY_Z,KEY_Q,KEY_1,KEY_F1,KEY_TAB,KEY_ESC},
  {0,0,0,0,0,0,0,0},  
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_X,KEY_TILDE,0,KEY_S,KEY_2,KEY_F3,KEY_W,KEY_F2},
  {KEY_C,0,KEY_PRINTSCREEN,KEY_D,KEY_E,KEY_3,KEY_F5,KEY_F4},  
  {KEY_DELETE,0,KEY_RIGHT,KEY_PERIOD,KEY_O,KEY_9,KEY_L,KEY_F12},
  {KEY_V,0,KEY_PAUSE,0,KEY_F6,KEY_R,KEY_F,KEY_4},
  {0,0,0,0,0,0,0,0},
  {KEY_SEMICOLON,0,KEY_SLASH,KEY_LEFT_BRACE,KEY_MINUS,KEY_0,KEY_P,KEY_SPACE},  
  {0,KEY_N,KEY_LEFT,0,KEY_F9,KEY_Y,KEY_H,KEY_6},
  {0,KEY_B,0,KEY_G,KEY_T,KEY_5,KEY_F8,KEY_F7},
  {KEY_INSERT,0,KEY_DOWN,KEY_COMMA,KEY_I,KEY_8,KEY_K,KEY_F11},
  {KEY_M,0,KEY_UP,KEY_J,KEY_U,KEY_7,0,KEY_F10},  
  {KEY_QUOTE,KEY_ENTER,0,KEY_RIGHT_BRACE,0,KEY_EQUAL,KEY_BACKSPACE,0},
  {KEY_END,KEY_BACKSLASH,KEY_MENU,KEY_PAGE_DOWN,KEY_HOME,0,KEY_PAGE_UP,0}
};
// Load the numlock key matrix with key names at the correct row-column location. 
// This matrix is the same as the normal matrix except for the number pad keys
// A zero indicates no numlock key at that location.
unsigned int numlock[rows_max][cols_max] = {
  {KEY_CAPS_LOCK,KEY_A,KEY_Z,KEY_Q,KEY_1,KEY_F1,KEY_TAB,KEY_ESC},
  {0,0,0,0,0,0,0,0},  
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_X,KEY_TILDE,0,KEY_S,KEY_2,KEY_F3,KEY_W,KEY_F2},
  {KEY_C,0,KEY_PRINTSCREEN,KEY_D,KEY_E,KEY_3,KEY_F5,KEY_F4},  
  {KEY_DELETE,0,KEY_RIGHT,KEYPAD_PERIOD,KEYPAD_6,KEYPAD_9,KEYPAD_3,KEY_F12},
  {KEY_V,0,KEY_PAUSE,0,KEY_F6,KEY_R,KEY_F,KEY_4},
  {0,0,0,0,0,0,0,0},
  {KEYPAD_PLUS,0,KEYPAD_SLASH,KEY_LEFT_BRACE,KEY_MINUS,KEYPAD_ASTERIX,KEYPAD_MINUS,KEY_SPACE},  
  {0,KEY_N,KEY_LEFT,0,KEY_F9,KEY_Y,KEY_H,KEY_6},
  {0,KEY_B,0,KEY_G,KEY_T,KEY_5,KEY_F8,KEY_F7},
  {KEY_INSERT,0,KEY_DOWN,KEY_COMMA,KEYPAD_5,KEYPAD_8,KEYPAD_2,KEY_F11},
  {KEYPAD_0,0,KEY_UP,KEYPAD_1,KEYPAD_4,KEYPAD_7,0,KEY_F10},  
  {KEY_QUOTE,KEY_ENTER,0,KEY_RIGHT_BRACE,0,KEY_EQUAL,KEY_BACKSPACE,0},
  {KEY_END,KEY_BACKSLASH,KEY_MENU,KEY_PAGE_DOWN,KEY_HOME,0,KEY_PAGE_UP,0}
};
// Load the arrowlock key matrix with key names at the correct row-column location. 
// This matrix is the same as the normal matrix except for the arrow pad keys
// A zero indicates no numlock key at that location.
unsigned int arrowlock[rows_max][cols_max] = {
  {KEY_CAPS_LOCK,KEY_A,KEY_Z,KEY_Q,KEY_1,KEY_F1,KEY_TAB,KEY_ESC},
  {0,0,0,0,0,0,0,0},  
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_X,KEY_TILDE,0,KEY_S,KEY_2,KEY_F3,KEY_W,KEY_F2},
  {KEY_C,0,KEY_PRINTSCREEN,KEY_D,KEY_E,KEY_3,KEY_F5,KEY_F4},  
  {KEY_DELETE,0,KEY_RIGHT,KEY_DELETE,KEYPAD_6,KEY_PAGE_UP,KEY_PAGE_DOWN,KEY_F12},
  {KEY_V,0,KEY_PAUSE,0,KEY_F6,KEY_R,KEY_F,KEY_4},
  {0,0,0,0,0,0,0,0},
  {KEY_SEMICOLON,0,KEY_SLASH,KEY_LEFT_BRACE,KEY_MINUS,KEY_0,KEY_P,KEY_SPACE},  
  {0,KEY_N,KEY_LEFT,0,KEY_F9,KEY_Y,KEY_H,KEY_6},
  {0,KEY_B,0,KEY_G,KEY_T,KEY_5,KEY_F8,KEY_F7},
  {KEY_INSERT,0,KEY_DOWN,KEY_COMMA,0,KEY_UP,KEY_DOWN,KEY_F11},
  {KEY_INSERT,0,KEY_UP,KEY_END,KEY_LEFT,KEY_HOME,0,KEY_F10},  
  {KEY_QUOTE,KEY_ENTER,0,KEY_RIGHT_BRACE,0,KEY_EQUAL,KEY_BACKSPACE,0},
  {KEY_END,KEY_BACKSLASH,KEY_MENU,KEY_PAGE_DOWN,KEY_HOME,0,KEY_PAGE_UP,0}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// A zero indicates no modifier key at that location.
unsigned int modifier[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {MODIFIERKEY_LEFT_CTRL,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_LEFT_SHIFT,0,0,0,0,0,MODIFIERKEY_RIGHT_SHIFT},
  {0,0,MODIFIERKEY_FN,0,0,0,MODIFIERKEY_GUI,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},  
  {0,0,0,MODIFIERKEY_LEFT_ALT,MODIFIERKEY_RIGHT_ALT,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};
// Load the media key matrix with Fn key names at the correct row-column location. 
// A zero indicates no media key at that location. 
unsigned int media[rows_max][cols_max] = {
  {0,0,0,0,0,KEY_MEDIA_VOLUME_DEC,0,KEY_MEDIA_MUTE},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,KEY_MEDIA_VOLUME_INC},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,KEY_SCROLL_LOCK},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,KEY_NUM_LOCK},
  {0,0,0,0,0,0,0,KEY_ARROW_LOCK},
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
// Assign the Teensy 2++ I/O row numbers
unsigned int Row_IO[rows_max] = {2,23,4,21,28,19,9,38,10,11,40,12,41,13,42,14}; 

// Assign the column I/O numbers
unsigned int Col_IO[cols_max] = {3,22,5,20,7,18,8,39}; 
                      
// Declare variables that will be used by functions
boolean slots_full = LOW; // Goes high when slots 1 thru 6 contain normal keys
boolean touchpad_error = LOW; // sent high when touch pad routine times out
//
// slot 1 thru slot 6 hold the normal key values to be sent over USB. 
unsigned int slot1 = 0; //value of 0 means the slot is empty and can be used.  
unsigned int slot2 = 0; 
unsigned int slot3 = 0; 
unsigned int slot4 = 0; 
unsigned int slot5 = 0; 
unsigned int slot6 = 0;
//
unsigned int mod_shift_l = 0; // These variables are sent over USB as modifier keys.
unsigned int mod_shift_r = 0; // Each is either set to 0 or MODIFIER_ ... 
unsigned int mod_ctrl_l = 0;   
unsigned int mod_ctrl_r = 0;
unsigned int mod_alt_l = 0;
unsigned int mod_alt_r = 0;
unsigned int mod_gui = 0;
//
// Function to load the key name into the first available slot
void load_slot(unsigned int key) {
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
void clear_slot(unsigned int key) {
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
void load_mod(unsigned int m_key) {
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
void clear_mod(unsigned int m_key) {
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
void go_z(unsigned int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}
//
// Function to set a pin as an input with a pullup
void go_pu(unsigned int pin)
{
  pinMode(pin, INPUT_PULLUP);
  digitalWrite(pin, HIGH);
}
//
// Function to send a pin to a logic low
void go_0(unsigned int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}
//
// Function to send a pin to a logic high
void go_1(unsigned int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}
//
//----------------------------------Setup-------------------------------------------
void setup() {
  for (unsigned int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (unsigned int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as a floating output
  }
} 
//
// Keyboard variables
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard status. 
// Num lock is bit D0.
// Caps lock is bit D1.
// Scroll lock is bit D2.
// Compose is bit D3. Compose has the same HID code as the Menu key.
// Kana is bit D4. Kana switches to a Japanese key layer.
// Power is bit D5.
// Shift is bit D6.
// Do Not Disturb is bit D7.
//
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed".
boolean arrow_lock = LOW; // Wake up with arrow lock turned off
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
  for (unsigned int x = 0; x < rows_max; x++) {   // loop thru the rows
    go_0(Row_IO[x]); // Activate Row (send it low)
    delayMicroseconds(10); // give the row time to go low and settle out
    for (unsigned int y = 0; y < cols_max; y++) {   // loop thru the columns
// **********Modifier keys including the Fn special case
      if (modifier[x][y] != 0) {  // check if modifier key exists at this location in the array (a non-zero value)
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) {  // Read column to see if key is low (pressed) and was previously not pressed
          if (modifier[x][y] != MODIFIERKEY_FN) {   // Exclude Fn modifier key  
            load_mod(modifier[x][y]); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable   
            send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
            old_key[x][y] = LOW; // Save state of key as "pressed"
          }
          else {   // Fn is pressed
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
          else {    // Fn is released
            Fn_pressed = HIGH; // Fn is no longer active
            old_key[x][y] = HIGH; // old_key state is "not pressed" 
          }
        }
      } 
// ***********end of modifier section
//
// ***********Normal keys, num lock, arrow lock keys and media keys in this section
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {  // check if normal or media key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn is not pressed and normal key needs to be sent
            if (keyboard_leds & 1) {  // test if Num Lock is turned on
              load_slot(numlock[x][y]); //update first available slot with key name from numlock matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
            else if (arrow_lock) {  // test if Arrow Lock is turned on
              load_slot(arrowlock[x][y]); //update first available slot with key name from arrowlock matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
            else {
              load_slot(normal[x][y]); //update first available slot with key name from normal matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
          }
          else if (media[x][y] != 0) { // Fn is pressed so send media if a key exists in the matrix
            if (media[x][y] == KEY_ARROW_LOCK) {
              arrow_lock = !arrow_lock; // invert the arrow lock control
            }
            else {
              Keyboard.press(media[x][y]); // media key is sent using keyboard press function per PJRC    
              delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
              Keyboard.release(media[x][y]); // send media key release
            }
          }
        }          
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed 
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          if (Fn_pressed) {  // Fn is not pressed
            if (keyboard_leds & 1) {  // test if Num Lock is turned on
              clear_slot(numlock[x][y]); //clear slot with key name from numlock matrix
              send_normals(); // send all slots over USB including the key that just got released
            }
            else if (arrow_lock) {  // test if Arrow lock is turned on
              clear_slot(arrowlock[x][y]); //clear slot with key name from arrowlock matrix
              send_normals(); // send all slots over USB including the key that just got released
            }
            else {
              clear_slot(normal[x][y]); //clear slot with key name from normal matrix
              send_normals(); // send all slots over USB including the key that just got released
            }
          }
        }
      } 
// **************end of normal, num lock, and media key section 
//
    }
    go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
  }
//
// **********keyboard scan complete
//
// Control the 4 keyboard LEDs
//
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_1(CAPS_LED); // turn on the LED
  }
  else {
    go_0(CAPS_LED); // turn off the LED
  }
//
  if (keyboard_leds & 1) {  // mask off all bits but D0 and test if set
    go_1(NUM_LED); // turn on the Num Lock LED
  }
  else {
    go_0(NUM_LED); // turn off the Num Lock LED
  }
//
  if (keyboard_leds & 1<<2) {  // mask off all bits but D2 and test if set
    go_1(SCRL_LED); // turn on the Scroll Lock LED
  }
  else {
    go_0(SCRL_LED); // turn off the Scroll Lock LED
  }
//
  if (arrow_lock) {
    go_1(ARROW_LED); // turn on the Arrow Lock LED
  }
  else {
    go_0(ARROW_LED); // turn off the Arrow Lock LED
  }
//  
//******************Delay before the next scan
  delay(25); // The overall keyboard scanning rate is about 30ms
}
