/* Copyright 2020 Frank Adams
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
// This software implements an IBM Thinkpad 380ED Laptop USB Keyboard/Trackpoint Controller.
// It uses a Teensy 3.2 on a custom FPC connector board that also has the amplifiers for the 
// trackpoint that are fed into the Teensy ADC.
//
// Revision History
// Initial Release May 16, 2020
// Rev A May 17, 2020 - Added Fn-F9,F10,F11,F12 adjustments for trackpoint
// Rev B May 27, 2020 - Changed Dead Zone methodology 
// Rev C May 27, 2020 - Reduced noise_zone from 50 to 15; Reduced delay from 25ms to 8ms
// Rev D June 7, 2020 - Added middle trackpoint button
//
#define MODIFIERKEY_FN 0x8f   // give Fn key a fake HID code 
#define CAPS_LED 14 // Teensy I/O's connected to thru hole pads with resistors for LEDs
#define NUM_LED 15
#define SCRL_LED 16
#define HEARTBEAT_LED 13 // onboard LED set to blink
#define TP_LEFT 20 // Trackpoint mouse buttons connected to Teensy I/O's with pullups
#define TP_RIGHT 5
#define TP_RETURN 30 // will be driven low
//
const byte rows_max = 17; // sets the number of rows in the keyboard matrix
const byte cols_max = 9; // sets the number of columns in the keyboard matrix
//
// Load the normal key matrix with the Teensyduino key names 
// described at www.pjrc.com/teensy/td_keyboard.html 
// A zero indicates no normal key at that location.
int normal[rows_max][cols_max] = {
  {KEY_ESC,KEY_TAB,KEY_TILDE,KEY_1,0,0,KEY_Z,KEY_A,KEY_Q},
  {0,KEY_CAPS_LOCK,KEY_F1,KEY_2,0,0,KEY_X,KEY_S,KEY_W},
  {0,0,0,0,0,0,0,0,0},
  {KEY_G,KEY_T,KEY_5,KEY_4,KEY_B,0,KEY_V,KEY_F,KEY_R},
  {KEY_H,KEY_Y,KEY_6,KEY_7,KEY_N,0,KEY_M,KEY_J,KEY_U},
  {KEY_F6,KEY_LEFT_BRACE,KEY_EQUAL,KEY_8,0,0,KEY_COMMA,KEY_K,KEY_I},
  {KEY_UP,0,KEY_HOME,KEY_END,KEY_LEFT,0,KEY_PAUSE,0,0},
  {0,0,KEY_PAGE_UP,KEY_PAGE_DOWN,0,0,0,0,0},
  {KEY_F4,KEY_F3,KEY_F2,KEY_3,0,0,KEY_C,KEY_D,KEY_E},
  {KEY_QUOTE,KEY_LEFT_BRACE,KEY_MINUS,KEY_0,KEY_SLASH,0,0,KEY_SEMICOLON,KEY_P},
  {KEY_F5,KEY_BACKSPACE,KEY_F9,KEY_F10,KEY_SPACE,0,KEY_ENTER,KEY_BACKSLASH,0},
  {0,0,KEY_DELETE,KEY_F11,KEY_DOWN,0,0,0,0},
  {0,0,0,KEY_PRINTSCREEN,0,0,0,0,KEY_NUM_LOCK},
  {0,0,KEY_INSERT,KEY_F12,KEY_RIGHT,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,KEY_F7,KEY_F8,KEY_9,0,0,KEY_PERIOD,KEY_L,KEY_O}
};
// Load the numlock key matrix with key names at the correct row-column location. 
// This matrix is the same as the normal matrix except for the number pad keys
// A zero indicates no numlock key at that location.
int numlock[rows_max][cols_max] = {
  {KEY_ESC,KEY_TAB,KEY_TILDE,KEY_1,0,0,KEY_Z,KEY_A,KEY_Q},
  {0,KEY_CAPS_LOCK,KEY_F1,KEY_2,0,0,KEY_X,KEY_S,KEY_W},
  {0,0,0,0,0,0,0,0,0},
  {KEY_G,KEY_T,KEY_5,KEY_4,KEY_B,0,KEY_V,KEY_F,KEY_R},
  {KEY_H,KEY_Y,KEY_6,KEYPAD_7,KEY_N,0,KEYPAD_0,KEYPAD_1,KEYPAD_4},
  {KEY_F6,KEY_LEFT_BRACE,KEY_EQUAL,KEYPAD_8,0,0,KEY_COMMA,KEYPAD_2,KEYPAD_5},
  {KEY_UP,0,KEY_HOME,KEY_END,KEY_LEFT,0,KEY_PAUSE,0,0},
  {0,0,KEY_UP,KEY_DOWN,0,0,0,0,0},
  {KEY_F4,KEY_F3,KEY_F2,KEY_3,0,0,KEY_C,KEY_D,KEY_E},
  {KEY_QUOTE,KEY_LEFT_BRACE,KEY_MINUS,KEYPAD_SLASH,KEYPAD_PLUS,0,0,KEYPAD_MINUS,KEYPAD_ASTERIX},
  {KEY_F5,KEY_BACKSPACE,KEY_F9,KEY_F10,KEY_SPACE,0,KEY_ENTER,KEY_BACKSLASH,0},
  {0,0,KEY_DELETE,KEY_F11,KEY_DOWN,0,0,0,0},
  {0,0,0,KEY_PRINTSCREEN,0,0,0,0,KEY_NUM_LOCK},
  {0,0,KEY_INSERT,KEY_F12,KEY_RIGHT,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,KEY_F7,KEY_F8,KEYPAD_9,0,0,KEYPAD_PERIOD,KEYPAD_3,KEYPAD_6}
};
// Load the modifier key matrix with key names at the correct row-column location. 
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,MODIFIERKEY_LEFT_CTRL,0,0,0,MODIFIERKEY_RIGHT_CTRL,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {MODIFIERKEY_LEFT_ALT,0,0,0,MODIFIERKEY_RIGHT_ALT,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,MODIFIERKEY_FN,0,0,0},
  {0,MODIFIERKEY_LEFT_SHIFT,0,0,0,0,MODIFIERKEY_RIGHT_SHIFT,0,0},
  {0,0,0,0,0,0,0,0,0}
};
// Load the media key matrix with Fn key names at the correct row-column location. 
// A zero indicates no media key at that location. 
int media[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0,0},
  {0,0,KEY_MEDIA_MUTE,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,KEY_MEDIA_VOLUME_INC,KEY_MEDIA_VOLUME_DEC,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},  
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,KEY_SYSTEM_WAKE_UP,0,0,0,0,KEY_SCROLL_LOCK},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}  
};
// Initialize the old_key matrix with one's (nothing pressed). 
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
// Assign the Teensy I/O row numbers
int Row_IO[rows_max] = {7,8,9,10,11,12,18,19,24,25,26,27,28,29,31,32,33}; 
//
// Assign the column I/O numbers
int Col_IO[cols_max] = {0,1,2,3,4,6,21,22,23}; 

// declare and initialize trackpoint variables
int x_read; // stores the reading from the X ADC
int y_read; // same for the Y ADC
char mx; // signed byte used by the Mouse.move function for x value. Positive value moves to the right
char my; // positive y value moves down 
int x_delta; // signed 16 bit value gives x movement amount
int y_delta; // y version. 
int x_center; //resting position of x sensor
int y_center; //resting position of y sensor
int noise_zone = 15; // dead zone around the center. Can be as low as 3 but cursor may start to drift so add some safety margin. 
                        // Increase this number if the cursor moves without touching the TP. Decrease if too much force is needed.
boolean left_button = 0; // on/off variable for left button, 1 = pushed
boolean right_button = 0; // on/off variable for right button, 1 = pushed
boolean middle_button = 0; // on/off variable for middle button, 1 = pushed
boolean old_left_button = 0; // on/off variable for left button from the previous cycle
boolean old_right_button = 0; // on/off variable for right button from the previous cycle
boolean old_middle_button = 0; // on/off variable for middle button from the previous cycle
boolean button_change = 0; // Shows when the left or right buttons have changed, 1 = change
// declare keyboard variables
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
// Trackpoint setup
  delay(1000); // delay a second to let things settle out
  analogReadRes(13); // set ADC resolution to 13 bits (16 bit ADC but low 3 bits are random noise)
  analogReadAveraging(8); // ADC will take the average of 8 reads to filter noise.
  x_center = analogRead(A11); // store the center (no movement) position for x
  y_center = analogRead(A10); // same for y
//
  go_0(TP_RETURN); // set the return pin for the trackpoint left/right buttons to always low
  go_pu(TP_RIGHT); // set right trackpoint button as input with a pullup
  go_pu(TP_LEFT); // set left trackpoint button as input with a pullup
// keyboard I/O setup  
  for (int a = 0; a < cols_max; a++) {  // loop thru all column pins 
    go_pu(Col_IO[a]); // set each column pin as an input with a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins 
    go_z(Row_IO[b]); // set each row pin as a floating output
  }  
//
  pinMode(HEARTBEAT_LED, OUTPUT); // drive the LED on the Teensy
}
//
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
extern volatile uint8_t keyboard_leds; // 8 bits sent from host to Teensy that give keyboard LED status.
char blink_count = 0; // heartbeat loop counter
boolean blinky = LOW; // heartbeat LED state
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
// ***********Normal keys, num lock keys and media keys in this section
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {  // check if normal or media key exists at this location in the array
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key pressed and not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn_pressed is active low so it is not pressed and normal key needs to be sent
            if (keyboard_leds & 1) {  // test if Num Lock is turned on
              load_slot(numlock[x][y]); //update first available slot with key name from numlock matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
            else { // Num Lock is not turned on
              load_slot(normal[x][y]); //update first available slot with key name from normal matrix
              send_normals(); // send all slots over USB including the key that just got pressed
            }
          }
          else if (media[x][y] != 0) { // Fn is pressed so send media if a key exists in the matrix
            Keyboard.press(media[x][y]); // media key is sent using keyboard press function per PJRC    
            delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
            Keyboard.release(media[x][y]); // send media key release
          }
          else if (normal[x][y] == KEY_F12) { // Fn is active and F12 is pressed - recapture TP center
            x_center = analogRead(A11); // store the center (no movement) position for x
            y_center = analogRead(A10); // same for y
          }
          else if (normal[x][y] == KEY_F10) { // Fn is active and F10 is pressed - increase noise zone value by 5
            noise_zone = noise_zone + 5;
          }
          else if (normal[x][y] == KEY_F9) { // Fn is active and F9 is pressed - decrease noise zone value by 5
            if (noise_zone >= 5) { // don't allow noise zone to be reduced below zero because it will cause cursor drift
              noise_zone = noise_zone - 5;
            }
          }
          else if (normal[x][y] == KEY_F11) { // Fn is active and F11 is pressed - Set noise zone to default value of 50
            noise_zone = 50;
          }
        }          
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed 
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          if (Fn_pressed) {  // Fn is not pressed
            if (keyboard_leds & 1) {  // test if Num Lock is turned on
              clear_slot(numlock[x][y]); //clear slot with key name from numlock matrix
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
// **********keyboard scan complete*******************************
//
// Control the 3 keyboard LEDs
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
// ******************Trackpoint Section******************
//
  mx = 0; // for each loop, start with zero and update only if movement detected
  my = 0; // same for y
// Read the ADC's
  x_read = analogRead(A11); // read the ADC tied to the X sensor
  y_read = analogRead(A10); // read the ADC tied to the Y sensor
// Check if X or Y movement is beyond the dead zone
  if ((x_read > (x_center + noise_zone)) || (x_read < (x_center - noise_zone)) || (y_read > (y_center + noise_zone)) || (y_read < (y_center - noise_zone))) {
// Only do the following if at least one reading is beyond the dead zone.
// Figure out if the X value is to the right or the left of the center resting value
    if (x_read > x_center) {  // is X ADC to the right of the resting position?
      x_delta = (x_read - x_center) / 10; // find out how far beyond center. Divide by 10 to slow it down.
      mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
    }
    else { // X ADC is equal or to the left of the resting position
      x_delta = ((x_center - x_read) / 10) * -1; // Answer should be negative so multiply by -1
      mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
    }
// Figure out if the Y value is above or below the center resting value
    if (y_read > y_center) { // is Y ADC above the resting position?
      y_delta = ((y_read - y_center) / 10) * -1; // up movement is negative for Mouse.move function
      my = byte(y_delta);  // convert signed 16 bit to signed 8 bit
    }
    else { // Y ADC is equal or below the resting position
      y_delta = ((y_center - y_read) / 10);
      my = byte(y_delta); // convert signed 16 bit to signed 8 bit
    }
  }
// send the x and y data over usb if either one is not at center position
  if ((mx != 0x00) || (my != 0x00)) {
    Mouse.move(mx,my);
  }
// ***********************read the touchpad left and right buttons***********************************
  if (!digitalRead(TP_RIGHT)) {   // check if right button is low (low = pushed) 
    right_button = 1;   // save state of button
  }
  else {   // clear right button
    right_button = 0;   // save state of button
  }
// Check left mouse button and also the Fn key
  if (!digitalRead(TP_LEFT)) {   // check if left button is low (low = pushed) 
    if (Fn_pressed) {     // Fn is not pushed
      left_button = 1;    // set the left button
    }
    else {                // Fn is pushed
      middle_button = 1;  // set the middle button
    }
  }
  else {                  // left button is not set
    if (Fn_pressed) {     // Fn is not pushed
      left_button = 0;    // clear the left button
    }
    else {                // Fn is pushed
      middle_button = 0;  // clear the middle button
    }
  }    
// Determine if the buttons have changed (using XOR) since the last cycle 
  button_change = ((left_button ^ old_left_button) | (right_button ^ old_right_button) | (middle_button ^ old_middle_button));
// Don't send button status if there's no change since last time 
  if (button_change){
    Mouse.set_buttons(left_button, middle_button, right_button); // send button status over USB
  }
  old_left_button = left_button; // remember button status for the next cycle
  old_right_button = right_button;
  old_middle_button = middle_button;
//
// Blink the LED on the Teensy to show a heartbeat
//
  if (blink_count >= 0x54) {  
    digitalWrite(HEARTBEAT_LED, blinky);
    blinky = !blinky;
    blink_count = 0;
  }
  else {
    blink_count = blink_count + 1;
  }
//
  delay(8); // The overall keyboard/trackpoint scanning rate is about 13ms
}
