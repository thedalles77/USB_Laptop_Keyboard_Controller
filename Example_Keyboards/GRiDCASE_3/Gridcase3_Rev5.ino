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
// This software creates a USB keyboard from a Gridcase3 keyboard.
//
// Revision History
// Initial Release Oct 31, 2022
// Rev 1 November 1, 2022 Fixed bad control key location in matrix
// Rev 2 November 2, 2022 Moved signal from I/O 15 to I/O 24
// Rev 3 November 3, 2022 Added Shift and Fig Shift variables. Added Media audio keys
// Rev 4 November 4, 2022 Changed to alt codes for ,=
// Rev 5 November 4, 2022 Removed alt codes for ,= and instead removed shift before sending keypress, then put back shift. Added TAB
//
#define MODIFIERKEY_FN 0x8f   // give Fn key a fake HID code
#define MODIFIERKEY_FIGSHIFT KEY_F13 // give Fig Shift key an unused code
// 
//
const byte rows_max = 12; // sets the number of rows in the matrix
const byte cols_max = 10; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names 
// described at www.pjrc.com/teensy/td_keyboard.html 
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,KEY_DOWN,KEY_RIGHT,KEY_UP,KEY_END,KEY_F11,KEY_F10,KEY_F12},  
  {KEY_ESC,0,KEY_1,KEY_Q,KEY_2,KEY_F1,KEY_3,KEY_F2,0,KEY_SPACE},  
  {KEY_EQUAL,0,KEY_TILDE,KEY_ENTER,KEY_LEFT,KEY_BACKSPACE,KEY_PRINTSCREEN,KEY_PAGE_UP,KEY_9,KEY_F9},
  {KEY_MINUS,0,KEY_SLASH,KEY_PERIOD,KEY_PAGE_DOWN,KEY_L,KEY_P,KEY_0,KEY_8,KEY_F8},
  {0,0,KEY_A,KEY_Z,KEY_K,KEY_J,KEY_I,KEY_O,KEY_7,KEY_F7},
  {0,0,KEY_N,KEY_M,KEY_H,KEY_G,KEY_Y,KEY_U,KEY_6,KEY_F6},
  {0,0,KEY_V,KEY_B,KEY_F,KEY_D,KEY_R,KEY_T,KEY_5,KEY_F5},
  {0,0,KEY_X,KEY_C,KEY_S,KEY_W,KEY_E,KEY_4,KEY_F3,KEY_F4},
  {0,KEY_DELETE,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,MODIFIERKEY_LEFT_ALT,0,0,0,0,0,0,0,0},
  {MODIFIERKEY_FN,0,MODIFIERKEY_LEFT_CTRL,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_LEFT_SHIFT,0,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_FIGSHIFT,0,0,0,0,0,0,0,0}
};
// Load the media key matrix with Fn key names at the correct row-column location. 
// A zero indicates no media key at that location. 
int media[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,KEY_MEDIA_MUTE,0,KEY_MEDIA_VOLUME_DEC,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,KEY_MEDIA_VOLUME_INC,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0,0}
};
// Initialize the old_key matrix with one's. 
// 1 = key not pressed, 0 = key is pressed
boolean old_key[rows_max][cols_max] = {
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1},
  {1,1,1,1,1,1,1,1,1,1}  
};
//
// Assign the Teensy I/O row numbers
int Row_IO[rows_max] = {10,11,12,14,24,16,17,18,19,20,21,22}; // swapped 15 for 24
//
// Assignb the column I/O numbers
int Col_IO[cols_max] = {0,1,2,3,4,5,6,7,8,9}; 
//
// Declare variables that will be used by functions
boolean slots_full = LOW; // Goes high when slots 1 thru 6 contain normal keys
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
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed".
boolean Figshift_pressed = HIGH; // Initialize Fig Shift key to HIGH = "not pressed".
boolean Shift_pressed = HIGH; // Initialize Shift key to HIGH = "not pressed".
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
          if ((modifier[x][y] != MODIFIERKEY_FN) && (modifier[x][y] != MODIFIERKEY_FIGSHIFT)) {   // Exclude Fn and figshift modifier keys  
            load_mod(modifier[x][y]); // function reads which modifier key is pressed and loads it into the appropriate mod_... variable   
            send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
            old_key[x][y] = LOW; // Save state of key as "pressed"
            if (modifier[x][y] == MODIFIERKEY_LEFT_SHIFT) {
              Shift_pressed = LOW; // shift status variable is active low
            }
          }
          else if (modifier[x][y] == MODIFIERKEY_FN)  {   // Fn is pressed
            Fn_pressed = LOW; // Fn status variable is active low
            old_key[x][y] = LOW; // old_key state is "pressed" (active low)
          }
          else { // Figshift is pressed
            Figshift_pressed = LOW; // FigShift status variable is active low
            old_key[x][y] = LOW; // old_key state is "pressed" (active low)
          }
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) {  //check if key is not pressed and was previously pressed
          if ((modifier[x][y] != MODIFIERKEY_FN) && (modifier[x][y] != MODIFIERKEY_FIGSHIFT)) { // Exclude Fn and figshift modifier keys 
            clear_mod(modifier[x][y]); // function reads which modifier key was released and loads 0 into the appropriate mod_... variable
            send_mod(); // function sends all mod's over usb including the one that just released  
            old_key[x][y] = HIGH; // Save state of key as "not pressed"
            if (modifier[x][y] == MODIFIERKEY_LEFT_SHIFT) {
              Shift_pressed = HIGH; // shift no longer active
            }
          }
          else if (modifier[x][y] == MODIFIERKEY_FN)  {    // Fn is released
            Fn_pressed = HIGH; // Fn is no longer active
            old_key[x][y] = HIGH; // old_key state is "not pressed" 
          }
          else { // Figshift is pressed
            Figshift_pressed = HIGH; // Figshift is no longer active
            old_key[x][y] = HIGH; // old_key state is "not pressed"
          }
        }
      } 
// ***********end of modifier section
//
// ***********Figure Shift in this section
      else if ((!Figshift_pressed) && (normal[x][y] != 0)) { // Figshift is active and normal array has character at this location
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) { // check if key pressed and not previously pressed
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (normal [x][y] == KEY_A) { // Figshift A
            Keyboard.press('('); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('('); // send key release
          }
          else if (normal [x][y] == KEY_B) { // Figshift B
//  *******Windows PC Alt Code sequence to give a ¥ is Alt 157*******************************
            Keyboard.press(MODIFIERKEY_RIGHT_ALT); // Push and hold right Alt
            delay(5); 
            Keyboard.press(KEYPAD_1); // Push 1
            delay(5); 
            Keyboard.release(KEYPAD_1); // Release 1
            delay(5); 
            Keyboard.press(KEYPAD_5); // Push 5
            delay(5); 
            Keyboard.release(KEYPAD_5); // Release 5
            delay(5);
            Keyboard.press(KEYPAD_7); // Push 7
            delay(5); 
            Keyboard.release(KEYPAD_7); // Release 7
            delay(5);
            Keyboard.release(MODIFIERKEY_RIGHT_ALT); // Release right Alt
            delay(5);
          }
// No Figshift C
          else if (normal [x][y] == KEY_D) { // Figshift D
            Keyboard.press('!'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('!'); // send key release
          }
          else if (normal [x][y] == KEY_E) { // Figshift E
            Keyboard.press('}'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('}'); // send key release
          }
          else if (normal [x][y] == KEY_F) { // Figshift F
            Keyboard.press(':'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release(':'); // send key release
          }
          else if (normal [x][y] == KEY_G) { // Figshift G
            Keyboard.press(';'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release(';'); // send key release
          }
          else if (normal [x][y] == KEY_H) { // Figshift H
//  *******Windows PC Alt Code sequence to give a ' is Alt 39*******couldn't use literal (''')
            Keyboard.press(MODIFIERKEY_RIGHT_ALT); // Push and hold right Alt
            delay(5); 
            Keyboard.press(KEYPAD_3); // Push 3
            delay(5); 
            Keyboard.release(KEYPAD_3); // Release 3
            delay(5); 
            Keyboard.press(KEYPAD_9); // Push 9
            delay(5); 
            Keyboard.release(KEYPAD_9); // Release 9
            delay(5);
            Keyboard.release(MODIFIERKEY_RIGHT_ALT); // Release right Alt
            delay(5);
          }
          else if (normal [x][y] == KEY_I) { // Figshift I
            Keyboard.press('>'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('>'); // send key release
          }
          else if (normal [x][y] == KEY_J) { // Figshift J
            Keyboard.press('"'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('"'); // send key release
          }
          else if (normal [x][y] == KEY_K) { // Figshift K
            Keyboard.press('@'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('@'); // send key release
          }
          else if (normal [x][y] == KEY_L) { // Figshift L
            Keyboard.press('&'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('&'); // send key release
          }
          else if (normal [x][y] == KEY_M) { // Figshift M
            Keyboard.press('$'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('$'); // send key release
          }
          else if (normal [x][y] == KEY_N) { // Figshift N
//  *******Windows PC Alt Code sequence to give a £ is Alt 156*******************************
            Keyboard.press(MODIFIERKEY_RIGHT_ALT); // Push and hold right Alt
            delay(5); 
            Keyboard.press(KEYPAD_1); // Push 1
            delay(5); 
            Keyboard.release(KEYPAD_1); // Release 1
            delay(5); 
            Keyboard.press(KEYPAD_5); // Push 5
            delay(5); 
            Keyboard.release(KEYPAD_5); // Release 5
            delay(5);
            Keyboard.press(KEYPAD_6); // Push 6
            delay(5); 
            Keyboard.release(KEYPAD_6); // Release 6
            delay(5);
            Keyboard.release(MODIFIERKEY_RIGHT_ALT); // Release right Alt
            delay(5);
          }
          else if (normal [x][y] == KEY_O) { // Figshift O
            Keyboard.press('x'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('x'); // send key release
          }
          else if (normal [x][y] == KEY_P) { // Figshift P
//  *******Windows PC Alt Code sequence to give a ÷ is Alt 246*******************************
            Keyboard.press(MODIFIERKEY_RIGHT_ALT); // Push and hold right Alt
            delay(5); 
            Keyboard.press(KEYPAD_2); // Push 2
            delay(5); 
            Keyboard.release(KEYPAD_2); // Release 2
            delay(5); 
            Keyboard.press(KEYPAD_4); // Push 4
            delay(5); 
            Keyboard.release(KEYPAD_4); // Release 4
            delay(5);
            Keyboard.press(KEYPAD_6); // Push 6
            delay(5); 
            Keyboard.release(KEYPAD_6); // Release 6
            delay(5);
            Keyboard.release(MODIFIERKEY_RIGHT_ALT); // Release right Alt
            delay(5);
          }
          else if (normal [x][y] == KEY_Q) { // Figshift Q
            Keyboard.press('?'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('?'); // send key release
          }
          else if (normal [x][y] == KEY_R) { // Figshift R
            Keyboard.press('*'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('*'); // send key release
          }
          else if (normal [x][y] == KEY_S) { // Figshift S
            Keyboard.press(')'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release(')'); // send key release
          }
          else if (normal [x][y] == KEY_T) { // Figshift T
            Keyboard.press('^'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('^'); // send key release
          }
          else if (normal [x][y] == KEY_U) { // Figshift U
            Keyboard.press('<'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('<'); // send key release
          }
// No Figshift V
          else if (normal [x][y] == KEY_W) { // Figshift W
            Keyboard.press('{'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('{'); // send key release
          }
          else if (normal [x][y] == KEY_X) { // Figshift X
            Keyboard.press(']'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release(']'); // send key release
          }
          else if (normal [x][y] == KEY_Y) { // Figshift Y
            Keyboard.press('#'); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('#'); // send key release
          }
          else if (normal [x][y] == KEY_Z) { // Figshift Z
            Keyboard.press('['); // literal is sent with Keyboard.press function
            delay(5); // 5msec delay to make sure it's sent
            Keyboard.release('['); // send key release
          }
        }
      }
// End of Figshift keys
// ***********Normal keys and media keys in this section
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {  // check if normal or media key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn is not pressed and normal key needs to be sent
// Pick out the special cases first
            if ((normal [x][y] == KEY_EQUAL) && (Shift_pressed)) { // plus key without a shift (Send a shifted EQUAL)
              Keyboard.press(MODIFIERKEY_RIGHT_SHIFT); // send a shifted equal to make a plus
              Keyboard.press(KEY_EQUAL);
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_EQUAL);
              Keyboard.release(MODIFIERKEY_RIGHT_SHIFT);
            }
            else if ((normal [x][y] == KEY_EQUAL) && (!Shift_pressed)) { // plus key with a shift = TAB
// temporarily remove the shift
              clear_mod(MODIFIERKEY_LEFT_SHIFT); // function loads 0 into the mod_... variable
              send_mod(); // function sends all mod's over usb including the one that just released  
              delay(5); // 5msec delay
              Keyboard.press(KEY_TAB); // send a tab
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_TAB);
              delay(5); // 5msec delay
// put back the shift
              load_mod(MODIFIERKEY_LEFT_SHIFT); // function loads shift into the mod_... variable   
              send_mod(); // function sends the state of all modifier keys over usb including the shift
            }
            else if ((normal [x][y] == KEY_MINUS) && (!Shift_pressed)) { // MINUS key with a shift
              Keyboard.press(MODIFIERKEY_RIGHT_SHIFT); // send a shifted 5 to make a Percent
              Keyboard.press(KEY_5);
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_5);
              Keyboard.release(MODIFIERKEY_RIGHT_SHIFT);
            }
            else if ((normal [x][y] == KEY_SLASH) && (!Shift_pressed)) { // Slash key with a shift to make an underscore
              Keyboard.press(MODIFIERKEY_RIGHT_SHIFT); // send a shifted MINUS to make a underscore
              Keyboard.press(KEY_MINUS);
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_MINUS);
              Keyboard.release(MODIFIERKEY_RIGHT_SHIFT);
            }
            else if ((normal [x][y] == KEY_PERIOD) && (!Shift_pressed)) { // Period key with a shift to make a comma (but to send a comma, there can be no shift)
// temporarily remove the shift
              clear_mod(MODIFIERKEY_LEFT_SHIFT); // function loads 0 into the mod_... variable
              send_mod(); // function sends all mod's over usb including the one that just released  
              delay(5); // 5msec delay
              Keyboard.press(KEY_COMMA); // send a comma
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_COMMA);
              delay(5); // 5msec delay
// put back the shift
              load_mod(MODIFIERKEY_LEFT_SHIFT); // function loads shift into the mod_... variable   
              send_mod(); // function sends the state of all modifier keys over usb including the shift
            }
            else if ((normal [x][y] == KEY_TILDE) && (Shift_pressed)) { // tilde is a place holder for VS. No shift = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_TILDE) && (!Shift_pressed)) { // VS (tilde) with a shift to mame = (but to send a =, there can be no shift)
// temporarily remove the shift
              clear_mod(MODIFIERKEY_LEFT_SHIFT); // function loads 0 into the mod_... variable
              send_mod(); // function sends all mod's over usb including the one that just released  
              delay(5); // 5msec delay
              Keyboard.press(KEY_EQUAL); // send a equal
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_EQUAL);
// put back the shift
              load_mod(MODIFIERKEY_LEFT_SHIFT); // function loads shift into the mod_... variable   
              send_mod(); // function sends the state of all modifier keys over usb including the shift
            }
            else if ((normal [x][y] == KEY_BACKSPACE) && (!Shift_pressed)) { // Backspace with a shift for home (but to send a home, there can be no shift)
// temporarily remove the shift
              clear_mod(MODIFIERKEY_LEFT_SHIFT); // function loads 0 into the mod_... variable
              send_mod(); // function sends all mod's over usb including the one that just released  
              delay(5); // 5msec delay
              Keyboard.press(KEY_HOME); // send a home
              delay(5); // 5msec delay to make sure it's sent
              Keyboard.release(KEY_HOME);
// put back the shift
              load_mod(MODIFIERKEY_LEFT_SHIFT); // function loads shift into the mod_... variable   
              send_mod(); // function sends the state of all modifier keys over usb including the shift
            }
            else if ((normal [x][y] == KEY_1) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_2) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_3) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_4) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_5) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_6) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_7) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_8) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_9) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_0) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
// special cases are done            
            else { // send all the others normally
              load_slot(normal[x][y]); //update first available slot with key name from normal matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
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
// special cases don't need to send a clear slot
            if (normal [x][y] == KEY_EQUAL)  { // plus key with or without a shift = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_MINUS) && (!Shift_pressed)) { // MINUS key with a shift
              delay(1);
            }
            else if ((normal [x][y] == KEY_SLASH) && (!Shift_pressed)) { // Slash key with a shift to make an underscore
              delay(1);
            }
            else if ((normal [x][y] == KEY_PERIOD) && (!Shift_pressed)) { // Period key with a shift
              delay(1);
            }
            else if (normal [x][y] == KEY_TILDE) { // VS (tilde) 
              delay(1);
            }
            else if ((normal [x][y] == KEY_BACKSPACE) && (!Shift_pressed)) { // Backspace with a shift
              delay(1);
            }
            else if ((normal [x][y] == KEY_1) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_2) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_3) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_4) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_5) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_6) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_7) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_8) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_9) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else if ((normal [x][y] == KEY_0) && (!Shift_pressed)) { // shifted numbers = DO NOTHING
              delay(1); 
            }
            else {
              clear_slot(normal[x][y]); //clear slot with key name from normal matrix
              send_normals(); // send all slots over USB including the key that just got released
            }
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
//******************Delay before the next scan
  delay(25); // The overall keyboard scanning rate is about 30ms
}
