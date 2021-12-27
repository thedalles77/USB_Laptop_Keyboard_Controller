/* Copyright 2021 Frank Adams
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
// This software implements a Logitech K120 Keyboard Controller and a Fujitsu Ergotrac touch sensor
// using a Teensy 4.1. The Teensy keyboard signals are wired to a pcb with pads. The flexible sheets 
// with traces in the keyboard are pressed against the pcb pads to make the connection. No FPC connector is used.
// The Ergotrac is sent a logic level pulse after the keyboard scan and two ADC channels read the voltage
// to sense the up/down and left/right movement
//
// Revision History
// Initial Release Dec 26, 2021
//
// keyboard values
#define MODIFIERKEY_FN 0x8f   // give Fn key a HID code
#define CAPS_LED 22 // Teensy LED shows Caps-Lock
#define NUM_LED 23
// X/Y outputs from ErgoTrac feed Teensy ADC channels
#define ErgoTrac_X A17
#define ErgoTrac_Y A16
// Reference signal from Teensy in to ErgoTrac
#define ErgoTrac_Ref 39
//
//
const byte rows_max = 18; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
//
int normal[rows_max][cols_max] = {
  {0,0,0,0,0,0,0,0},
  {KEY_ESC,KEY_TILDE,KEY_TAB,KEY_Q,KEY_A,KEY_F,KEY_Z,KEY_CAPS_LOCK},
  {KEY_F8,KEY_F9,KEY_8,KEY_9,KEY_0,KEY_O,KEY_L,KEY_SLASH},
  {KEY_PRINTSCREEN,KEY_SCROLL_LOCK,KEY_PAUSE,KEY_PAGE_UP,KEY_NUM_LOCK,KEY_HOME,KEY_PAGE_DOWN,KEY_INSERT},
  {KEY_F10,KEY_F11,KEY_F12,KEY_MINUS,KEY_EQUAL,KEY_I,KEY_LEFT,0},  
  {KEY_F6,KEY_F7,KEY_U,KEY_H,KEY_7,KEY_J,KEY_N,KEY_PERIOD},
  {KEY_F4,KEY_4,KEY_R,KEY_5,KEY_F,KEY_V,KEY_M,KEY_MENU}, 
  {KEY_F5,KEY_T,KEY_6,KEY_Y,KEY_G,KEY_B,KEY_COMMA,0}, 
  {0,0,0,0,0,0,0,0}, //
  {KEYPAD_SLASH,KEYPAD_8,KEY_END,KEYPAD_ASTERIX,KEYPAD_7,KEYPAD_9,KEYPAD_5,KEYPAD_6}, //
  {0,0,0,0,0,0,0,0}, //
  {0,KEY_BACKSPACE,KEY_P,KEY_LEFT_BRACE,KEY_SEMICOLON,KEY_QUOTE,KEY_K,KEY_DOWN}, 
  {KEY_RIGHT_BRACE,KEY_BACKSLASH,KEY_DELETE,KEY_ENTER,KEY_UP,KEY_RIGHT,KEYPAD_0,KEY_SPACE}, 
  {KEYPAD_4,KEYPAD_1,KEYPAD_2,KEYPAD_3,KEYPAD_MINUS,KEYPAD_PLUS,0,KEYPAD_ENTER}, 
  {KEY_F2,KEY_F3,KEY_3,KEY_E,KEY_D,KEY_C,0,0}, 
  {KEY_F1,KEY_1,KEY_2,KEY_W,KEY_9,KEY_X,KEY_S,KEYPAD_PERIOD}, 
  {0,0,0,0,0,0,0,0}, //
  {0,0,0,0,0,0,0,0}  //
};
// Load the modifier key matrix with key names at the correct row-column location.
// A zero indicates no modifier key at that location.
int modifier[rows_max][cols_max] = {
  {0,0,MODIFIERKEY_CTRL,0,MODIFIERKEY_RIGHT_CTRL,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,MODIFIERKEY_SHIFT,0,0,0,MODIFIERKEY_RIGHT_SHIFT},   
  {0,0,0,0,0,0,0,0},
  {0,MODIFIERKEY_GUI,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {MODIFIERKEY_ALT,0,0,0,0,MODIFIERKEY_RIGHT_ALT,0,0},
  {0,0,0,0,0,0,MODIFIERKEY_FN,0} // actually the MODIFIERKEY_RIGHT_GUI key
};
// Load the media key matrix with Fn key names at the correct row-column location.
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
  {0,0,0,0,0,0,0,0},   
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {KEY_MEDIA_VOLUME_DEC,KEY_MEDIA_VOLUME_INC,0,0,0,0,0,0},
  {KEY_MEDIA_MUTE,0,0,0,0,0,0,0},
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
// Define the Teensy 4.1 I/O numbers (translated from the FPC pin #)
int Row_IO[rows_max] = {0,1,10,11,12,24,25,26,27,28,29,30,31,32,36,35,34,33}; // Teensy 4.0 I/O numbers for rows
//
int Col_IO[cols_max] = {2,3,4,5,6,7,8,9};  // Teensy 4.1 I/O numbers for columns

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
// declare and initialize ErgoTrac variables
int x_read; // stores the reading from the X ADC
int y_read; // same for the Y ADC
char mx; // signed byte used by the Mouse.move function for x value. Positive value moves to the right
char my; // positive y value moves down 
int x_delta; // signed 16 bit value gives x movement amount
int y_delta; // y version. 
int x_center; //resting position of x sensor
int y_center; //resting position of y sensor
int noise_zone = 50; // dead zone around the center. 
char slowdown_left = 75; // factor to slow the curser movement down going left
char slowdown_right = 60; // factor to slow the curser movement down going right
char slowdown_up = 70; // factor to slow the curser movement down going up
char slowdown_down = 75; // factor to slow the curser movement down going down
boolean left_button = 0; // on/off variable for left button, 1 = pushed
boolean right_button = 0; // on/off variable for right button, 1 = pushed
boolean old_left_button = 0; // on/off variable for left button from the previous cycle
boolean old_right_button = 0; // on/off variable for right button from the previous cycle
boolean button_change = 0; // Shows when the left or right buttons have changed, 1 = change
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
// Function to set a pin to high impedance (acts like open drain output)
void go_z(int pin)
{
  pinMode(pin, INPUT);
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
// Function to re-capture the at-rest X & Y ADC values
void recenter(void) {
  digitalWrite(ErgoTrac_Ref, LOW); // resting state of reference
  delay(25); // wait before sending calibration pulse
  digitalWrite(ErgoTrac_Ref, HIGH); // Send calibration pulse high
  delayMicroseconds(400); // wait before reading ADC to allow for rise time
  x_center = analogRead(ErgoTrac_X); // store the center (no movement) value for x
  y_center = analogRead(ErgoTrac_Y); // same for y
  delayMicroseconds(50); // wait before releasing calibration pulse
  digitalWrite(ErgoTrac_Ref, LOW); // Send calibration pulse back to resting state
}
// ************************************Begin Routine*********************************************************
void setup()
{
  delay(2000); // wait to let things settle out
  analogReadRes(12); // ADC has 10 usable bits but set resolution to the full 12 bits  
  analogReadAveraging(8); // ADC will take the average of multiple reads to filter noise.
  pinMode(ErgoTrac_Ref, OUTPUT); // define Teensy I/O that drives the refernce pulses to the ErgoTrac
  recenter(); // capture the at-rest X and Y position
    for (int a = 0; a < cols_max; a++) {  // loop thru all column pins
    go_z(Col_IO[a]); // set each column pin as an input without a pullup
  }
//
  for (int b = 0; b < rows_max; b++) {  // loop thru all row pins
    go_z(Row_IO[b]); // set each row pin as a floating output
  } 
  delay(25); // wait 25ms before entering main loop
}
boolean Fn_pressed = HIGH; // Initialize Fn key to HIGH = "not pressed"
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status. Caps lock is bit D1.

//
// ************************************Main Loop***************************************************************
void loop() {
// ******************************************Keyboard**********************************************************
//
  elapsedMillis loop_timer; // zero the loop timer. Timer is used at the end of the loop to determine when to repeat
// Scan keyboard matrix with an outer loop that drives each row low and an inner loop that reads every column (with pull ups).
// The routine looks at each key's present state (by reading the column input pin) and also the previous state from the last scan
// that was 30msec ago. The status of a key that was just pressed or just released is sent over USB and the state is saved in the old_key matrix.
// The keyboard keys will read as logic low if they are pressed (negative logic).
// The old_key matrix also uses negative logic (low=pressed).
//
  for (int x = 0; x < rows_max; x++) {   // loop thru the rows
    go_0(Row_IO[x]); // Activate Row (send it low)
    delayMicroseconds(50); // give the row time to go low and settle out
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
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) { // check if key is pressed and was not previously pressed and slots not full
          old_key[x][y] = LOW; // Save state of key as "pressed"
          if (Fn_pressed) {  // Fn_pressed high means it's not pressed
// **** Special case for Scroll Lock and Pause which are used by Ergotrac as left and right mouse buttons 
            if(normal[x][y] == KEY_SCROLL_LOCK) {
              left_button = 1;    // set the left button
            }
            else if(normal[x][y] == KEY_PAUSE) {
              right_button = 1;    // set the right button
            } 
// All the other normal keys are sent
            else {
              load_slot(normal[x][y]); //update first available slot with normal key name
              send_normals(); // send all slots over USB including the key that just got pressed 
            }
          }
          else if (media[x][y] != 0) { // Fn is pressed so send media if a key exists in the matrix
            Keyboard.press(media[x][y]); // media key is sent using keyboard press function per PJRC   
            //delay(5); // delay 5 milliseconds before releasing to make sure it gets sent over USB
            delayMicroseconds(300);
            Keyboard.release(media[x][y]); // send media key release
          }

          else if (normal[x][y] == KEY_F11) { // Fn is active and F11 is pressed - recapture TP center
            recenter(); // recapture the at-rest X and Y position
          }
          else if (normal[x][y] == KEY_F10) { // Fn is active and F10 is pressed - increase noise zone value by 5
            noise_zone = noise_zone + 5;
          }
          else if (normal[x][y] == KEY_F9) { // Fn is active and F9 is pressed - decrease noise zone value by 5
            noise_zone = noise_zone - 5;
          }
          else if (normal[x][y] == KEY_F8) { // Fn is active and F8 is pressed 
            slowdown_left = slowdown_left - 5; // decrease all slowdown values by 5
            slowdown_right = slowdown_right - 5;
            slowdown_up = slowdown_up - 5;
            slowdown_down = slowdown_down - 5;
          } 
          else if (normal[x][y] == KEY_F7) { // Fn is active and F7 is pressed 
            slowdown_left = slowdown_left + 5; // increase all slowdown values by 5
            slowdown_right = slowdown_right + 5;
            slowdown_up = slowdown_up + 5;
            slowdown_down = slowdown_down + 5;
          }
          else if (normal[x][y] == KEY_F6) { // Fn is active and F6 is pressed 
            noise_zone = 50; // Set noise zone and slowdowns to default values
            slowdown_left = 75; // movement left needs more slowdown for some reason
            slowdown_right = 60;
            slowdown_up = 70;
            slowdown_down = 75;
          }
          else if (normal[x][y] == KEY_F5) { // Fn is active and F5 is pressed 
            if (noise_zone >= 1000) { // toggle back to normal or huge (off) value
              noise_zone = 50; // back to start up value
            }
            else {
              noise_zone = 5000; // huge noise zone won't allow cursor movement
            }
          }

        
        }         
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) { //check if key is not pressed, but was previously pressed
          old_key[x][y] = HIGH; // Save state of key as "not pressed"
          if (Fn_pressed) {  // Fn is not pressed

// **** Special case for releazse of Scroll Lock or Pause (left and right mouse buttons) 
            if(normal[x][y] == KEY_SCROLL_LOCK) {
              left_button = 0;    // clear the left button
            }
            else if(normal[x][y] == KEY_PAUSE) {
              right_button = 0;    // clear the right button
            } 
// All the other normal keys are released
            else {
            clear_slot(normal[x][y]); //clear the slot that contains the normal key name
            send_normals(); // send all slots over USB including the key that was just released
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
// ****************************************LEDs********************
// Turn on the LEDs for Caps and Num Lock based on the keyboard_leds variable controlled by the USB host computer
// CAPS LOCK
  if (keyboard_leds & 1<<1) {  // mask off all bits but D1 and test if set
    go_1(CAPS_LED); // turn on the LED
  }
  else {
    go_0(CAPS_LED); // turn off the LED
  }
// NUM LOCK
  if (keyboard_leds & 1) {  // mask off all bits but D0 and test if set
    go_1(NUM_LED); // turn on the LED
  }
  else {
    go_0(NUM_LED); // turn off the LED
  }
//
// ********************Ergotrac*************************************************************************************
//
  digitalWrite(ErgoTrac_Ref, HIGH); // send calibration reference pulse
//
  mx = 0; // for each loop, start with zero and update only if movement detected
  my = 0; // same for y
// Read the ADC's
  x_read = analogRead(ErgoTrac_X); // read the ADC tied to the X sensor
  y_read = analogRead(ErgoTrac_Y); // read the ADC tied to the Y sensor
// Check if X or Y movement is beyond the dead zone
  if ((x_read > (x_center + noise_zone)) || (x_read < (x_center - noise_zone)) || (y_read > (y_center + noise_zone)) || (y_read < (y_center - noise_zone))) {
// Only do the following if at least one reading is beyond the dead zone.
// Figure out if the X value is to the right or the left of the center resting value
    if (x_read > x_center) {  // is X ADC to the right of the resting position?
      x_delta = (x_read - x_center) / slowdown_right; // find out how far beyond center. Divide it to slow it down.
      mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
    }
    else { // X ADC is equal or to the left of the resting position
      x_delta = ((x_center - x_read) / slowdown_left) * -1; // Answer should be negative so multiply by -1
      mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
    }
// Figure out if the Y value is above or below the center resting value
    if (y_read > y_center) { // is Y ADC above the resting position?
      y_delta = ((y_read - y_center) / slowdown_down) * -1; // up movement is negative for Mouse.move function
      my = byte(y_delta);  // convert signed 16 bit to signed 8 bit
    }
    else { // Y ADC is equal or below the resting position
      y_delta = (y_center - y_read) / slowdown_up;
      my = byte(y_delta); // convert signed 16 bit to signed 8 bit
    }
  }
// send the x and y data over usb if either one is not at center position
  if ((mx != 0x00) || (my != 0x00)) {
    Mouse.move(mx,my);
  }
  digitalWrite(ErgoTrac_Ref, LOW); // release calibration reference

// Mouse buttons (Scroll Lock and Pause keys)
//  
// Determine if the buttons have changed (using XOR) since the last cycle 
  button_change = ((left_button ^ old_left_button) | (right_button ^ old_right_button));
// Don't send button status if there's no change since last time 
  if (button_change){
    Mouse.set_buttons(left_button,0, right_button); // send button status over USB
  }
  old_left_button = left_button; // remember button status for the next cycle
  old_right_button = right_button;
//
  delay(20);  // wait before repeating
}
