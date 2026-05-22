/* Copyright 2026 Frank Adams
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
// This software allows a Lenovo ThinkPad P15 Laptop Keyboard to be connected to the Thinkpad motherboard 
// or to a Teensy 4.1 that acts as a USB controller. The software controls 27 analog switches in order to 
// disconnect the motherboard from the keyboard. 
// The keyboard backlight is not used. The trackpoint is controlled by a separate circuit but the Teensy 
// reads the left, middle, and right buttons and sends them over USB regardless of switch positions.
//
// Revision History
// Rev New - May 22, 2026 - Original Release
//
// Keyboard LEDs used to indicate the switch position. If these LED indicators are not wanted, change the 26 & 25 to 40 & 41 (unused I/O's)
#define MIC_LED 26    // Active low signal turns on Mic LED to indicate Teensy sending keyboard info over USB
#define MUTE_LED 25   // Active low signal turns on Mute LED to indicate MB is connected to the keyboard
// Keyboard Fn key (aka HOTKEY)
#define HOTKEY 27       // Fn key
// TP buttons
#define TP_LEFT 36 // "mouse" buttons for the trackpoint
#define TP_RIGHT 35
#define TP_MIDDLE 34 
// ADG714 control signals. With Sync low, serial data is shifted in on the falling edge of the clock. Taking sync high updates the switches.
#define SYNC_N 32 // Sync signal to all 4 adg714's. When low, the input shift register, SCLK and DIN buffers are enabled.
#define SCLK 31 // Serial Clock to all 4 adg714's. Falling clock edge shifts data in. 
#define DIN 30 // Serial Data to all 4 adg714's (they're not daisy chained). 1 = switch closed.
// Debug
#define MICRO_SW 33 // Spare input pin that could be programmed to toggle keyboard control if the Fn key method is a problem
#define TEENSY_LED 13 // LED on the Teensy used for debug
// Set the keyboard row & column size
const byte rows_max = 18; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
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
  {0,KEY_RIGHT,0,0,KEY_F12,KEYPAD_PERIOD,KEY_INSERT,0},
  {KEY_EQUAL,KEY_LEFT,0,0,KEY_END,KEY_LEFT_BRACE,KEY_HOME,KEY_UP},
  {0,KEY_DOWN,0,0,KEY_F11,0,KEY_DELETE,0},
  {KEY_BACKSPACE,KEY_SPACE,KEY_ENTER,KEY_BACKSLASH,KEY_F10,0,KEY_F9,KEY_F5},
  {KEY_RIGHT_BRACE,0,0,0,KEY_PAGE_DOWN,KEY_BACKSPACE,KEY_PAGE_UP,0},
  {KEYPAD_SLASH,KEYPAD_PLUS,KEYPAD_9,KEYPAD_7,KEYPAD_8,KEYPAD_ASTERIX,KEYPAD_MINUS,KEY_NUM_LOCK},
  {KEYPAD_5,KEYPAD_0,KEYPAD_ENTER,KEYPAD_2,KEYPAD_3,KEYPAD_6,KEYPAD_1,KEYPAD_4}  
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
// Row FPC pin # 4,7, 10,11,13,14,15,16,17,18,19,20,21,22,23,24,36,37
// Teensy I/O  # 1,20,4, 18,17,6, 16,7, 15,8, 14,9, 10,11,12,24,28,29
int Row_IO[rows_max] = {1,20,4,18,17,6,16,7,15,8,14,9,10,11,12,24,28,29}; // Teensy 4.1 I/O numbers for rows
//
// Column FPC pin # 1, 2,3, 5, 6,8,9, 12
// Teensy I/O     # 23,0,22,21,2,3,19,5
int Col_IO[cols_max] = {23,0,22,21,2,3,19,5};  // Teensy 4.1 I/O numbers for columns
//
// Define the time to hold down the fn key that causes the switches to change state
//
int fn_trip = 100; // fn_trip x 30msec = seconds fn key must be held down to cause the switches to toggle. 133 gives 4 seconds, 100 gives 3 seconds
int fn_counter = 0; // will be incremented every 30ms if the fn key is held down. When the counter reaches the trip value, the switches all toggle
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
// **************Functions**************************
//
// Function to set a pin to high impedance by making it an input (acts like an open drain output )
void go_z(int pin)
{
  pinMode(pin, INPUT);
}
//
// Function to set a pin as an input with a pullup
void go_pu(int pin)
{
  pinMode(pin, INPUT_PULLUP);
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
// Function to open or close all ADG714 switches
void sw_cntrl(boolean data_in)
{ 
  if (data_in) {
    go_1(DIN); // drive data high into the adg714 shift registers to close all switches
  }
  else  {
    go_0(DIN); // drive data low into the adg714 shift registers to open all switches
  }
  delay(1); // delay before dropping sync_n
// drop SYNC_N to enable shifting in new data
  go_0(SYNC_N); 
  delay(1); // delay before dropping SCLK
// toggle SCLK 8 times to shift in the data.
  for (int j=0; j<8; j++) {
    go_0(SCLK);
    delay(1); // delay 1/2 of the SCLK duty cycle
    go_1(SCLK);
    delay(1); // delay 1/2 of the SCLK duty cycle
  }
  go_1(SYNC_N); // rising edge transfers adg714 shift register contents to register that controls switches 
}
//
// Function to setup the Teensy I/O pins so they can scan the keyboard. Turn on mic LED
void teensy_scan() {
  for (int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as floating
  } 
//
  go_pu(HOTKEY);    // Pull up the Fn "Hotkey" input so it can be read. If key pressed, it will be low.
//
  go_1(MUTE_LED);   // mute led off
  go_0(MIC_LED);    // mic led on showing that Teensy is scanning the keyboard
}
//
// Function to setup Teensy I/O's and turn on mute LED (testing showed it works best to keep Teensy pull ups enabled)
void mb_scan() {
  for (int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as pulled up input
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as floating
  } 
//
  go_pu(HOTKEY);    // Pull up the Fn "Hotkey" Teensy input. 
//
  go_0(MUTE_LED);   // mute led turned on showing that MB is scanning the keyboard
  go_1(MIC_LED);    // mic led turned off
}
//
//************************************Setup*******************************************
void setup() {
// At power up, the Teensy I/O's are setup to scan the row/column and fn pins of the keyboard
  teensy_scan(); // rows are floating, columns are pulled up, Mic LED is turned on
// TP Button Inputs
  go_pu(TP_LEFT); // Set the TP buttons as inputs with pullups
  go_pu(TP_RIGHT);
  go_pu(TP_MIDDLE);
// Initialize adg714 control signals
  go_1(SYNC_N); // resting state high
  go_1(SCLK); // resting state high
  go_0(DIN); // resting state low
// send serial signals that initialize the adg714 switches
  sw_cntrl(LOW); // shift in 0's to open all adg714 switches so MB is not able to see the keyboard
}
// initialize the MB/Teensy control variable to "not motherboard"
boolean MB = LOW; // Motherboard tied to keyboard when high, Teensy reads keyboard when low
// initialize keyboard variables
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
// initialize TP button variables
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
//
// *************Keyboard Main**************
// Read the Fn key (aka Hotkey) which is not part of the key matrix
  if (!digitalRead(HOTKEY)) { // active low fn key press
    fn_counter = fn_counter + 1; // increment counter
    if (fn_counter >= fn_trip) {  // check if counter has reached the trip point
      fn_counter = 0; // clear counter
      MB = !MB; // Toggle the motherboard/Teensy control signal
      sw_cntrl(MB); // set the adg714 switches accordingly
      if (MB) {
        mb_scan(); // configure teensy I/O's for motherboard scan and turn on correct LED
      }
      else {
        teensy_scan(); // configure I/O's for Teensy scan and turn on correct LED
      }
    }
    else {
      Fn_pressed = LOW; // Fn key is pressed (but not long enough to toggle the switches)
    }
  }
  else  {
    Fn_pressed = HIGH; // Fn key is not pressed
    fn_counter = 0; // reset counter to zero
  }
//
// If MB is low, scan keyboard matrix with an outer loop that drives each row low and an inner loop that reads every column (with pull ups).
// The routine looks at each key's present state (by reading the column input pin) and also the previous state from the last scan
// that was 30msec ago. The status of a key that was just pressed or just released is sent over USB and the state is saved in the old_key matrix. 
// The keyboard keys will read as logic low if they are pressed (negative logic).
// The old_key matrix also uses negative logic (low=pressed). 
//
  if (!MB) { // proceed with Teensy keyboard scan when the MB is not switched to the keyboard
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
      }
      go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
    }
  }
//
// **********keyboard section finished********************************
//
// ***********Trackpoint Buttons - always scanned and reported over USB, regardless of MB value******************************
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
    Mouse.set_buttons(left_button, middle_button, right_button); // send left, middle, and right button status over usb
  }
  old_left_button = left_button; // remember button status for next polling cycle
  old_right_button = right_button;
  old_middle_button = middle_button;
//
// End of trackpoint button routine
//
// ****************End of main loop*****
//
delay(28); // This delay value added to the delay from the above code makes the overall keyboard scanning rate about 30ms
}