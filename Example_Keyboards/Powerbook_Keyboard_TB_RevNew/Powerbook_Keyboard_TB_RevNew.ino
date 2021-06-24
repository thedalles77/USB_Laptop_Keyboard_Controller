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
// This software implements a Powerbook 140-180 Laptop Keyboard Controller using a  
// Teensy 3.2 on a daughterboard with 16 and 14 pin connectors. 
// The TB section of code is from this post:
// https://imgur.com/a/BQjt9le
// 
// Revision History
// Initial Release June 23, 2021
// 
#define CAPS_LED 13 // Teensy LED shows Caps-Lock
#define ADB_DATA_PIN 5 // TB bidirectional data pin
//
const byte rows_max = 16; // sets the number of rows in the matrix
const byte cols_max = 8; // sets the number of columns in the matrix
//
// Load the normal key matrix with the Teensyduino key names described at www.pjrc.com/teensy/td_keyboard.html
// A zero indicates no normal key at that location.
//
int normal[rows_max][cols_max] = {
  {0,KEY_7,KEY_8,KEY_RIGHT_BRACE,KEY_BACKSLASH,KEY_9,KEY_QUOTE,KEY_6},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,KEY_ESC},
  {0,KEY_1,KEY_2,KEY_I,KEY_H,KEY_3,KEY_BACKSPACE,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,KEY_TAB,KEY_A,0,KEY_LEFT,0},
  {KEY_Z,0,0,KEY_Q,KEY_S,0,0,0},
  {KEY_X,0,0,KEY_W,KEY_DOWN,0,KEY_UP,0},
  {KEY_C,0,0,KEY_E,0,0,KEY_D,0},
  {KEY_V,0,0,KEY_R,KEY_F,0,KEY_RIGHT,0},
  {KEY_N,KEY_TILDE,KEY_G,KEY_K,KEY_J,KEY_BACKSLASH,KEY_L,KEY_4},
  {KEY_M,KEY_T,KEY_Y,KEY_P,KEY_COMMA,KEY_LEFT_BRACE,KEY_SEMICOLON,KEY_5},
  {0,KEY_MINUS,KEY_0,0,KEY_PERIOD,KEY_O,KEY_ENTER,KEY_EQUAL},
  {KEY_SPACE,0,0,KEY_DELETE,KEY_B,0,KEY_U,0}
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
// initialize old special keys to "not pressed"
boolean old_CAPS_LOCK = 1;
boolean old_CTRL = 1;
boolean old_ALT = 1;
boolean old_SHIFT = 1;
//
// Define the Teensy 3.2 I/O numbers (translated from the FPC pin #)
// Row FPC pin # 01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16
// Teensy I/O  # 12,14,15,16,17,18,19,20,21,22,23,04,03,02,01,00
int Row_IO[rows_max] = {12,14,15,16,17,18,19,20,21,22,23,4,3,2,1,0}; // Teensy 3.2 I/O numbers for rows
//
// Column FPC pin # 18,19,20,21,22,23,24,25 // FPC 17 is tied to ground
// Teensy I/O     # 24,25,26,27,28,29,30,31 // Teensy I/O#6 is not used
int Col_IO[cols_max] = {24,25,26,27,28,29,30,31};  // Teensy 3.2 I/O numbers for columns 

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
// TB Constants
int dat = 0;
int ADB_TIMEOUT = 10000;
// TB Functions
static void send_0bit() {
 digitalWrite(ADB_DATA_PIN, LOW);
 delayMicroseconds(65);
 digitalWrite(ADB_DATA_PIN, HIGH);
 delayMicroseconds(35);
}

static void send_1bit() {
 digitalWrite(ADB_DATA_PIN, LOW);
 delayMicroseconds(35);
 digitalWrite(ADB_DATA_PIN, HIGH);
 delayMicroseconds(65);
}

static void attn() {
  //delay(1);
  digitalWrite(ADB_DATA_PIN, LOW);
  delayMicroseconds(800-35);
  send_0bit();
}

void send_talk_command() {
 //attn
 pinMode(ADB_DATA_PIN, OUTPUT);
 attn();
 //address (3) 0011
 //0
  send_0bit();
  send_0bit();
  send_1bit();
  send_1bit();
 //talk 11
  send_1bit();
  send_1bit();
 //register 00
  send_0bit();
  send_0bit();
 //stop bit
  send_0bit();
 //start to stop time
 delayMicroseconds(140);

}
int receive_data_packet() {
 pinMode(ADB_DATA_PIN, INPUT);
 int data_time = 0;
 bool command_stop = false;
 int adb_data = 0;
 //begin reading ADB datapin
 
 while(digitalRead(ADB_DATA_PIN)!=LOW && data_time < ADB_TIMEOUT){
  //wait for beginning of start bit
  data_time+=1;
  delayMicroseconds(1);
 }

  while(digitalRead(ADB_DATA_PIN)!=HIGH && data_time < ADB_TIMEOUT){
  //wait for end of start bit
  data_time+=1;
  delayMicroseconds(1);
 }

 while(digitalRead(ADB_DATA_PIN)!=LOW && data_time < ADB_TIMEOUT){
  //wait for beginning of first data bit
  data_time+=1;
  delayMicroseconds(1);
 }

 //data packet
 data_time = 0;
 while(!command_stop && data_time < ADB_TIMEOUT) {
  int low = 0;
  int high = 0;
  bool bit_stop = false;
  while(!bit_stop && data_time < ADB_TIMEOUT) {
    
    int adb_stream = digitalRead(ADB_DATA_PIN);
    if(adb_stream == LOW) low+=1;
    if(adb_stream == HIGH) high+=1;
    if(adb_stream == LOW && high > 0) bit_stop = true;
    if(high > 240){
      bit_stop = true;
      break;
    }
    data_time+=1;
    delayMicroseconds(1);
  }
  if(high > 240){
    command_stop = true;
  }
  //append the new data bit to adb_data
  if(high > low && !command_stop) adb_data = (adb_data << 1) + 1;
  if(low > high && !command_stop) adb_data = adb_data << 1;
  
 }
 //stop bit 
 return adb_data;
}
/*data bits:
 * [c][x][x][x][x][x][x][x][c2][y][y][y][y][y][y][y]
*/
void parse_data(int data) {
      if(data !=0) {
       int mousex = data & 0b0000000001111111;
       int mousey = (data >> 8) & 0b01111111;
       //the mouse value is
        //7 bit two's complement
       if((mousey & 0b1000000) != 0) {
          mousey =  ((~mousey + 1) & 0b01111111) * -1;
       }
       if((mousex & 0b1000000) != 0) {
          mousex =  ((~mousex + 1) & 0b01111111) * -1;
       }
       Mouse.move(mousex, mousey);
       Mouse.move(mousex, mousey);

      int mouseClick = data & 0b1000000000000000;
      
      if(mouseClick == 0) {
        Mouse.set_buttons(1,0,0);
      }
      else if(mouseClick != 0) {
        Mouse.set_buttons(0,0,0);
      }
     }
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
extern volatile uint8_t keyboard_leds; // 8 bits sent from Host to Teensy that give keyboard LED status. Caps lock is bit D1.
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
//
// ***********Normal keys in this section
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
    }
    go_z(Row_IO[x]); // De-activate Row (send it to hi-z)
  }
// **********Normal keyboard scan complete
//
// **********Modifier and Caps Lock keys have their own column that is tied to ground. The rows for these keys are 
// turned into inputs with pullups to see if a key is pressed, then returned to floating outputs for the next loop. 
//
// Control Key
  go_pu(Row_IO[1]); // Read MODIFIERKEY_CTRL on Row 1. The column is always grounded
  if (!digitalRead(Row_IO[1]) && (old_CTRL)) {  // check if key is pressed and was not pressed last time 
    mod_ctrl_l = MODIFIERKEY_CTRL; 
    send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
    old_CTRL = LOW; // Save state of key as "pressed"
  }
  else if (digitalRead(Row_IO[1]) && (!old_CTRL)) {  //check if key is not pressed and was previously pressed
    mod_ctrl_l = 0;
    send_mod(); // function sends all mod's over usb including the one that just released
    old_CTRL = HIGH; // old_key state is "not pressed"
  }
  go_z(Row_IO[1]); // return this row pin to a floating output for the next loop
// Alt Key
  go_pu(Row_IO[2]); // Read MODIFIERKEY_ALT on Row 2. The column is always grounded
  if (!digitalRead(Row_IO[2]) && (old_ALT)) { // check if key is pressed and was not pressed last time  
    mod_alt_l = MODIFIERKEY_ALT; 
    send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
    old_ALT = LOW; // Save state of key as "pressed"
  }
  else if (digitalRead(Row_IO[2]) && (!old_ALT)) {  //check if key is not pressed and was previously pressed
    mod_alt_l = 0;
    send_mod(); // function sends all mod's over usb including the one that just released
    old_ALT = HIGH; // old_key state is "not pressed"
  }
  go_z(Row_IO[2]); // return this row pin to a floating output for the next loop
// Shift Key
  go_pu(Row_IO[3]); // Read MODIFIERKEY_SHIFT on Row 3. The column is always grounded
  if (!digitalRead(Row_IO[3]) && (old_SHIFT)) {  // check if key is pressed and was not pressed last time 
    mod_shift_l = MODIFIERKEY_SHIFT; 
    send_mod(); // function sends the state of all modifier keys over usb including the one that just got pressed
    old_SHIFT = LOW; // Save state of key as "pressed"
  }
  else if (digitalRead(Row_IO[3]) && (!old_SHIFT)) {  //check if key is not pressed and was previously pressed
    mod_shift_l = 0;
    send_mod(); // function sends all mod's over usb including the one that just released
    old_SHIFT = HIGH; // old_key state is "not pressed"
  }
  go_z(Row_IO[3]); // return this row pin to a floating output for the next loop
// Caps Lock Key
  go_pu(Row_IO[5]); // Read KEY_CAPS_LOCK on Row 5. The column is always grounded
  if ((!digitalRead(Row_IO[5])) && (old_CAPS_LOCK) && (!slots_full)) { // check if key is pressed and was not pressed last time and slots are not full
    load_slot(KEY_CAPS_LOCK); //update first available slot with key name
    send_normals(); // send all slots over USB including the key that just got pressed
    old_CAPS_LOCK = LOW; // Save state of key as "pressed"
  }
  else if (digitalRead(Row_IO[5]) && (!old_CAPS_LOCK)) {  //check if key is not pressed and was previously pressed
    clear_slot(KEY_CAPS_LOCK); //clear the slot that contains the normal key name
    send_normals(); // send all slots over USB including the key that was just released 
    old_CAPS_LOCK = HIGH; // old_key state is "not pressed"
  }
  go_z(Row_IO[5]); // return this row pin to a floating output for the next loop
// ***********end of modifier and Caps Lock section
//
// ************TB***********************************
   send_talk_command();
   dat = receive_data_packet();
   parse_data(dat);
// ***********end of TB******************************
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
  delay(15); // The overall keyboard scanning rate is about 30ms
}
