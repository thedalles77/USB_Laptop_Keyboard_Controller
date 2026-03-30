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
// This software controls a Lenovo ThinkPad P15/P17 Gen 1 Laptop Keyboard and Trackpoint.
// The PS/2 code for the Trackpoint uses Stream mode because Remote mode would not work.
//
// Revision History
// Rev New - Dec 13, 2025 - Original Release
// Rev A   - Mar 27, 2026 - Added middle-button scrolling using the TrackPoint
// Rev B   - Mar 29, 2026 - Nathan used ChatGPT to fix the scrolling in the Rev A code and add better comments
// Note:
// - The middle TrackPoint button is used for scrolling only.
// - It is NOT sent as a USB middle mouse button.
// - Left and right TrackPoint buttons are sent normally.

//
// ============================ Hardware Pin Definitions ============================
//

// Keyboard LEDs
#define LED_PWR   25    // Anode side of LEDs always driven high to 3.3 volts
#define CAPS_LED  31    // Active low signal to Caps Lock LED cathode
#define NUM_LED   38    // Active low signal to Num Lock LED cathode
#define MUTE_LED  27    // Active low signal to Mute LED cathode
#define FN_LED    26    // Active low signal to show when Fn key is pressed
#define BACKLIGHT 13    // Keyboard backlight PWM control input pin

// Keyboard Fn key (aka HOTKEY)
#define HOTKEY     29
#define HOTKEY_RTN 30

// TrackPoint buttons and PS/2 interface
#define TP_BUTTON_RTN 32
#define TP_LEFT       33
#define TP_RIGHT      34
#define TP_MIDDLE     35
#define TP_DATA       39
#define TP_CLK        41
#define TP_RESET      40

//
// ============================ Keyboard Matrix Size ============================
//

const byte rows_max = 18;
const byte cols_max = 8;

//
// ============================ User Tuning ============================
//
// Keyboard backlight brightness
// 0 = off, 255 = full on
int brightness = 0;

// TrackPoint speed
// 1 = normal/full speed
// 0 = slower speed
boolean tp_speed = 1;

// Scroll behavior tuning for middle-button scrolling:
//
// Hold the middle TrackPoint button, then push the TrackPoint up/down.
// A light push scrolls a little. A harder push scrolls more.
//
// SCROLL_REPEAT_MS
//   Time between scroll events while middle button is held.
//   Smaller = faster repeated scrolling
//   Larger  = slower repeated scrolling
const uint16_t SCROLL_REPEAT_MS = 40;

// SCROLL_TRIGGER_LIGHT
//   Minimum absolute Y movement needed to produce a light scroll step.
//   Increase this if scrolling triggers too easily.
//   Decrease this if scrolling feels too hard to trigger.
const int8_t SCROLL_TRIGGER_LIGHT = 1;

// SCROLL_TRIGGER_HEAVY
//   Minimum absolute Y movement needed to produce a heavy scroll step.
//   Increase this if heavy scrolling happens too easily.
//   Decrease this if you want large jumps with less force.
const int8_t SCROLL_TRIGGER_HEAVY = 35;

// SCROLL_STEP_LIGHT
//   Number of scroll lines for a light push.
const int8_t SCROLL_STEP_LIGHT = 1;

// SCROLL_STEP_HEAVY
//   Number of scroll lines for a harder push.
const int8_t SCROLL_STEP_HEAVY = 4;

//
// ============================ Key Maps ============================
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

//
// Initialize the old_key matrix with one's.
// 1 = key not pressed, 0 = key is pressed
//
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
// ============================ Keyboard Matrix Pin Map ============================
//
// Row FPC pin # 4,7,10,11,13,14,15,16,17,18,19,20,21,22,23,24,36,37
// Teensy I/O  # 1,20,4,18,17,6,16,7,15,8,14,9,10,11,12,24,36,37
//
int Row_IO[rows_max] = {1,20,4,18,17,6,16,7,15,8,14,9,10,11,12,24,36,37};

//
// Column FPC pin # 1,2,3,5,6,8,9,12
// Teensy I/O     # 23,0,22,21,2,3,19,5
//
int Col_IO[cols_max] = {23,0,22,21,2,3,19,5};

//
// ============================ Global State ============================
//

boolean slots_full = LOW;
boolean trackpoint_error = LOW;
boolean mute = 0;

uint8_t mstat;    // TrackPoint status byte
int8_t  mx;       // TrackPoint X movement byte
int8_t  my;       // TrackPoint Y movement byte

// Slots 1 through 6 hold normal key values to be sent over USB
int slot1 = 0;
int slot2 = 0;
int slot3 = 0;
int slot4 = 0;
int slot5 = 0;
int slot6 = 0;

// Modifier key variables sent over USB
int mod_shift_l = 0;
int mod_shift_r = 0;
int mod_ctrl_l  = 0;
int mod_ctrl_r  = 0;
int mod_alt_l   = 0;
int mod_alt_r   = 0;
int mod_gui     = 0;

// Keyboard/USB state
boolean Fn_pressed = HIGH;              // HIGH = not pressed, LOW = pressed
extern volatile uint8_t keyboard_leds;  // Host-to-device keyboard LED bits

// TrackPoint runtime variables
boolean over_flow         = 0;
boolean left_button       = 0;
boolean right_button      = 0;
boolean middle_button     = 0; // physical middle button, used only for scrolling
boolean old_left_button   = 0;
boolean old_right_button  = 0;
boolean old_middle_button = 0;
boolean button_change     = 0;

elapsedMillis scroll_timer;

//
// **************Functions common to keyboard and trackpoint**************************
//

// Function to set a pin to high impedance (acts like open drain output)
void go_z(int pin)
{
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH);
}

// Function to set a pin as an input with a pullup
void go_pu(int pin)
{
  pinMode(pin, INPUT_PULLUP);
  digitalWrite(pin, HIGH);
}

// Function to send a pin to a logic low
void go_0(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

// Function to send a pin to a logic high
void go_1(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

//
// *****************Functions for Keyboard*****************************
//

// Function to load the key name into the first available slot
void load_slot(int key)
{
  if (!slot1) {
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

  if (!slot1 || !slot2 || !slot3 || !slot4 || !slot5 || !slot6) {
    slots_full = LOW;
  }
  else {
    slots_full = HIGH;
  }
}

// Function to clear the slot that contains the key name
void clear_slot(int key)
{
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

  if (!slot1 || !slot2 || !slot3 || !slot4 || !slot5 || !slot6) {
    slots_full = LOW;
  }
  else {
    slots_full = HIGH;
  }
}

// Function to load the modifier key name into the appropriate mod variable
void load_mod(int m_key)
{
  if (m_key == MODIFIERKEY_LEFT_SHIFT) {
    mod_shift_l = m_key;
  }
  else if (m_key == MODIFIERKEY_RIGHT_SHIFT) {
    mod_shift_r = m_key;
  }
  else if (m_key == MODIFIERKEY_LEFT_CTRL) {
    mod_ctrl_l = m_key;
  }
  else if (m_key == MODIFIERKEY_RIGHT_CTRL) {
    mod_ctrl_r = m_key;
  }
  else if (m_key == MODIFIERKEY_LEFT_ALT) {
    mod_alt_l = m_key;
  }
  else if (m_key == MODIFIERKEY_RIGHT_ALT) {
    mod_alt_r = m_key;
  }
  else if (m_key == MODIFIERKEY_GUI) {
    mod_gui = m_key;
  }
}

// Function to clear the appropriate mod variable
void clear_mod(int m_key)
{
  if (m_key == MODIFIERKEY_LEFT_SHIFT) {
    mod_shift_l = 0;
  }
  else if (m_key == MODIFIERKEY_RIGHT_SHIFT) {
    mod_shift_r = 0;
  }
  else if (m_key == MODIFIERKEY_LEFT_CTRL) {
    mod_ctrl_l = 0;
  }
  else if (m_key == MODIFIERKEY_RIGHT_CTRL) {
    mod_ctrl_r = 0;
  }
  else if (m_key == MODIFIERKEY_LEFT_ALT) {
    mod_alt_l = 0;
  }
  else if (m_key == MODIFIERKEY_RIGHT_ALT) {
    mod_alt_r = 0;
  }
  else if (m_key == MODIFIERKEY_GUI) {
    mod_gui = 0;
  }
}

// Function to send modifier keys over USB
void send_mod()
{
  Keyboard.set_modifier(mod_shift_l | mod_shift_r | mod_ctrl_l | mod_ctrl_r | mod_alt_l | mod_alt_r | mod_gui);
  Keyboard.send_now();
}

// Function to send the normal keys in the 6 slots over USB
void send_normals()
{
  Keyboard.set_key1(slot1);
  Keyboard.set_key2(slot2);
  Keyboard.set_key3(slot3);
  Keyboard.set_key4(slot4);
  Keyboard.set_key5(slot5);
  Keyboard.set_key6(slot6);
  Keyboard.send_now();
}

//
// *****************Functions for TrackPoint***************************
//

// Convert TrackPoint Y movement into a wheel amount.
// Positive dy = push up on the stick after conversion, which scrolls upward.
// Negative dy = push down, which scrolls downward.
int8_t scroll_amount_from_dy(int8_t dy)
{
  if (dy >= SCROLL_TRIGGER_HEAVY) return -SCROLL_STEP_HEAVY;
  if (dy >= SCROLL_TRIGGER_LIGHT) return -SCROLL_STEP_LIGHT;
  if (dy <= -SCROLL_TRIGGER_HEAVY) return  SCROLL_STEP_HEAVY;
  if (dy <= -SCROLL_TRIGGER_LIGHT) return  SCROLL_STEP_LIGHT;
  return 0;
}

// ********Function to send the trackpoint a byte of data (command)*********
void tp_write(char send_data)
{
  unsigned int timeout = 200;
  elapsedMillis watchdog;
  char odd_parity = 0;

  // Enable the PS/2 bus by floating the clock and data
  go_pu(TP_CLK);
  go_pu(TP_DATA);
  delayMicroseconds(250);

  go_0(TP_CLK);          // request to transmit
  delayMicroseconds(100);
  go_0(TP_DATA);         // start bit
  delayMicroseconds(1);
  go_pu(TP_CLK);         // release clock
  delayMicroseconds(1);

  while (digitalRead(TP_CLK) == HIGH) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  // send 8 data bits, LSB first
  for (int j = 0; j < 8; j++) {
    if (send_data & 1) {
      go_pu(TP_DATA);
      odd_parity = odd_parity + 1;
    }
    else {
      go_0(TP_DATA);
    }

    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == LOW) {
      if (watchdog >= timeout) {
        trackpoint_error = HIGH;
        break;
      }
    }

    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == HIGH) {
      if (watchdog >= timeout) {
        trackpoint_error = HIGH;
        break;
      }
    }

    send_data = send_data >> 1;
  }

  // parity bit
  if (odd_parity & 1) {
    go_0(TP_DATA);
  }
  else {
    go_pu(TP_DATA);
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  go_pu(TP_DATA);        // stop bit
  delayMicroseconds(80);

  while (digitalRead(TP_CLK) == HIGH) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  delayMicroseconds(1);
  if (digitalRead(TP_DATA)) {
    // Ack should be low; keep behavior same as original
  }

  while ((digitalRead(TP_CLK) == LOW) || (digitalRead(TP_DATA) == LOW)) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  // Inhibit the bus so the TrackPoint only talks when the Teensy is listening
  go_0(TP_CLK);
}

// *********Function to get a byte of data from the trackpoint**********
char tp_read(void)
{
  unsigned int timeout = 200;
  elapsedMillis watchdog;
  char rcv_data = 0;
  char mask = 1;
  char rcv_parity = 0;

  go_pu(TP_CLK);
  go_pu(TP_DATA);
  delayMicroseconds(5);

  while (digitalRead(TP_CLK) == HIGH) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  if (digitalRead(TP_DATA)) {
    // Start bit should be low
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  for (int k = 0; k < 8; k++) {
    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == HIGH) {
      if (watchdog >= timeout) {
        trackpoint_error = HIGH;
        break;
      }
    }

    if (digitalRead(TP_DATA)) {
      rcv_data = rcv_data | mask;
      rcv_parity++;
    }

    mask = mask << 1;

    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == LOW) {
      if (watchdog >= timeout) {
        trackpoint_error = HIGH;
        break;
      }
    }
  }

  // receive parity
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  if (digitalRead(TP_DATA)) {
    rcv_parity++;
  }

  rcv_parity = rcv_parity & 1;
  if (rcv_parity == 0) {
    trackpoint_error = HIGH;
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  // stop bit
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  if (digitalRead(TP_DATA) == LOW) {
    // Stop bit should be high
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
    if (watchdog >= timeout) {
      trackpoint_error = HIGH;
      break;
    }
  }

  // Inhibit the bus so the TrackPoint only talks when the Teensy is listening
  go_0(TP_CLK);
  return rcv_data;
}

// ********Function to decode a 3 byte packet of data from the trackpoint (status, X Delta, Y Delta)********
char tp_packet(void)
{
  // ******************************Receive Byte 0 = Status***********************
  char rcv_data = 0;
  char mask = 1;
  char rcv_parity = 0;

  unsigned int time_leftover = 24;
  elapsedMillis tp_timer;

  go_pu(TP_CLK);
  go_pu(TP_DATA);
  delayMicroseconds(5);

  while (digitalRead(TP_CLK) == HIGH) {
    if (tp_timer >= time_leftover) {
      go_0(TP_CLK);
      return 1; // no TrackPoint data available
    }
  }

  if (digitalRead(TP_DATA)) {
    // Start bit should be low
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  for (int k = 0; k < 8; k++) {
    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == HIGH) {
    }

    if (digitalRead(TP_DATA)) {
      rcv_data = rcv_data | mask;
      rcv_parity++;
    }

    mask = mask << 1;

    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == LOW) {
    }
  }

  // receive parity
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA)) {
    rcv_parity++;
  }

  rcv_parity = rcv_parity & 1;
  if (rcv_parity == 0) {
    // parity error ignored here to preserve original behavior
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  // stop bit
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA) == LOW) {
    // stop bit should be high
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  mstat = (uint8_t)rcv_data;

  // ******************************Receive Byte 1 = Delta X**************************
  rcv_data = 0;
  mask = 1;
  rcv_parity = 0;

  delayMicroseconds(5);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA)) {
    // Start bit should be low
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  for (int k = 0; k < 8; k++) {
    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == HIGH) {
    }

    if (digitalRead(TP_DATA)) {
      rcv_data = rcv_data | mask;
      rcv_parity++;
    }

    mask = mask << 1;

    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == LOW) {
    }
  }

  // receive parity
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA)) {
    rcv_parity++;
  }

  rcv_parity = rcv_parity & 1;
  if (rcv_parity == 0) {
    // parity error ignored here to preserve original behavior
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  // stop bit
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA) == LOW) {
    // stop bit should be high
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  mx = (int8_t)rcv_data;

  // ******************************Receive Byte 2 = Delta Y**************************
  rcv_data = 0;
  mask = 1;
  rcv_parity = 0;

  delayMicroseconds(5);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA)) {
    // Start bit should be low
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  for (int k = 0; k < 8; k++) {
    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == HIGH) {
    }

    if (digitalRead(TP_DATA)) {
      rcv_data = rcv_data | mask;
      rcv_parity++;
    }

    mask = mask << 1;

    delayMicroseconds(1);
    while (digitalRead(TP_CLK) == LOW) {
    }
  }

  // receive parity
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA)) {
    rcv_parity++;
  }

  rcv_parity = rcv_parity & 1;
  if (rcv_parity == 0) {
    // parity error ignored here to preserve original behavior
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  // stop bit
  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == HIGH) {
  }

  if (digitalRead(TP_DATA) == LOW) {
    // stop bit should be high
  }

  delayMicroseconds(1);
  while (digitalRead(TP_CLK) == LOW) {
  }

  my = (int8_t)rcv_data;

  // Inhibit the bus so the TrackPoint only talks when we're listening
  go_0(TP_CLK);

  return 0;
}

//
// ************************************TP Initialization Setup**********************************************
//
void trackpoint_init()
{
  trackpoint_error = LOW;
  go_pu(TP_CLK);
  go_pu(TP_DATA);

  // pulse reset high, then low
  go_0(TP_RESET);
  delay(1000);
  go_1(TP_RESET);
  delay(1000);
  go_0(TP_RESET);
  delay(1000);

  // Send reset command
  tp_write(0xff);
  if (tp_read() != 0xfa) {
    trackpoint_error = HIGH;
  }

  delay(1000); // wait for self-test

  if (tp_read() != 0xaa) {
    trackpoint_error = HIGH;
  }

  if (tp_read() != 0x00) {
    trackpoint_error = HIGH;
  }

  // Disable so mode byte can be loaded
  tp_write(0xf5);
  if (tp_read() != 0xfa) {
    trackpoint_error = HIGH;
  }

  // Load Mode Byte = 00 using Synaptics special sequence
  tp_write(0xe8);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x00);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  tp_write(0xe8);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x00);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  tp_write(0xe8);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x00);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  tp_write(0xe8);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x00);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  tp_write(0xf3);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x14);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  // Set real resolution
  tp_write(0xe8);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x03);  // 8 counts/mm
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  // Set real sample rate
  tp_write(0xf3);
  if (tp_read() != 0xfa) trackpoint_error = HIGH;
  tp_write(0x28);  // 40 samples/second
  if (tp_read() != 0xfa) trackpoint_error = HIGH;

  // Stream mode
  tp_write(0xea);
  if (tp_read() != 0xfa) {
    trackpoint_error = HIGH;
  }

  // Enable TrackPoint
  tp_write(0xf4);
  if (tp_read() != 0xfa) {
    trackpoint_error = HIGH;
  }
}

//
//************************************Setup*******************************************
//
void setup()
{
  // keyboard setup
  for (int a = 0; a < cols_max; a++) {
    go_pu(Col_IO[a]);
  }

  for (int b = 0; b < rows_max; b++) {
    go_z(Row_IO[b]);
  }

  trackpoint_init();
  if (trackpoint_error) {
    delay(500);
    trackpoint_init();
  }

  go_pu(HOTKEY);
  go_0(HOTKEY_RTN);

  pinMode(BACKLIGHT, OUTPUT);
  analogWriteFrequency(BACKLIGHT, 400);
  analogWrite(BACKLIGHT, brightness);

  go_1(LED_PWR);   // power LED anodes
  go_1(CAPS_LED);  // LEDs off at startup (active low)
  go_1(NUM_LED);
  go_1(MUTE_LED);
  go_1(FN_LED);

  go_0(TP_BUTTON_RTN);
  go_pu(TP_LEFT);
  go_pu(TP_RIGHT);
  go_pu(TP_MIDDLE);

  scroll_timer = SCROLL_REPEAT_MS; // allow immediate first scroll event
}

//
//*********************************Main Loop********************************************
//
void loop()
{
  // *************Keyboard Main**************

  // Read the Fn key (not part of the key matrix)
  if (!digitalRead(HOTKEY)) {
    Fn_pressed = LOW;   // Fn pressed (active low)
    go_0(FN_LED);       // turn on Fn LED
  }
  else {
    Fn_pressed = HIGH;  // Fn not pressed
    go_1(FN_LED);       // turn off Fn LED
  }

  // Set backlight brightness
  analogWrite(BACKLIGHT, brightness);

  // Scan keyboard matrix
  for (int x = 0; x < rows_max; x++) {
    go_0(Row_IO[x]);               // activate row
    delayMicroseconds(10);

    for (int y = 0; y < cols_max; y++) {

      // **********Modifier keys
      if (modifier[x][y] != 0) {
        if (!digitalRead(Col_IO[y]) && (old_key[x][y])) {
          load_mod(modifier[x][y]);
          send_mod();
          old_key[x][y] = LOW;
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) {
          clear_mod(modifier[x][y]);
          send_mod();
          old_key[x][y] = HIGH;
        }
      }

      // ***********Normal keys and media keys
      else if ((normal[x][y] != 0) || (media[x][y] != 0)) {
        if (!digitalRead(Col_IO[y]) && (old_key[x][y]) && (!slots_full)) {
          old_key[x][y] = LOW;

          if (Fn_pressed) {
            load_slot(normal[x][y]);
            send_normals();
          }
          else if (normal[x][y] == KEY_SPACE) { // Fn + Space toggles backlight
            if (brightness == 255) {
              brightness = 0;
            }
            else if (brightness == 0) {
              brightness = 255;
            }
          }
          else if (normal[x][y] == KEY_F12) { // Fn + F12 toggles TrackPoint speed
            tp_speed = !tp_speed;
            go_0(FN_LED);
          }
          else if (media[x][y] != 0) {
            Keyboard.press(media[x][y]);

            if (media[x][y] == KEY_MEDIA_MUTE) {
              mute = !mute;
              if (mute) {
                go_0(MUTE_LED);
              }
              else {
                go_1(MUTE_LED);
              }
            }

            delay(5);
            Keyboard.release(media[x][y]);
          }
        }
        else if (digitalRead(Col_IO[y]) && (!old_key[x][y])) {
          old_key[x][y] = HIGH;
          if (Fn_pressed) {
            clear_slot(normal[x][y]);
            send_normals();
          }
        }
      }
    }

    go_z(Row_IO[x]); // deactivate row
  }

  // ***********TrackPoint Section******************************

  // Read physical middle button state once per loop.
  // It is used ONLY for scrolling, not sent as a USB middle button.
  middle_button = !digitalRead(TP_MIDDLE);

  // Allow immediate scroll event on first middle-button press
  if (middle_button && !old_middle_button) {
    scroll_timer = SCROLL_REPEAT_MS;
  }

  if (trackpoint_error == LOW) {
    if (tp_packet() == 0x00) {
      over_flow = 0;
      if ((mstat & 0x80) || (mstat & 0x40)) {
        over_flow = 1;
      }

      int8_t dx = mx;
      int8_t dy = my;

      // Convert X from 9-bit PS/2 format to signed 8-bit
      if (!tp_speed) {
        dx = dx >> 1;
      }
      dx = (int8_t)((uint8_t)dx & 0x7F);
      if (mstat & 0x10) {
        dx = (int8_t)(0x80 | ((uint8_t)dx & 0x7F));
      }

      // Convert Y from 9-bit PS/2 format to signed 8-bit, then invert sign
      if (!tp_speed) {
        dy = dy >> 1;
      }
      dy = (int8_t)((uint8_t)dy & 0x7F);
      if (mstat & 0x20) {
        dy = (int8_t)(0x80 | ((uint8_t)dy & 0x7F));
      }
      dy = (int8_t)(~dy + 1);

      // Zero out bad movement if overflow is set
      if (over_flow) {
        dx = 0;
        dy = 0;
      }

      // Middle button held = scroll mode only
      if (middle_button) {
        int8_t wheel = scroll_amount_from_dy(dy);
        if ((wheel != 0) && (scroll_timer >= SCROLL_REPEAT_MS)) {
          Mouse.scroll(wheel);
          scroll_timer = 0;
        }
      }
      else {
        // Normal pointer motion when middle button is not held
        if ((dx != 0) || (dy != 0)) {
          Mouse.move(dx, dy);
        }
      }
    }
  }

  // **********TrackPoint Buttons********************************************
  // Left and right buttons work normally.
  // Middle button is NOT sent to the host; it is reserved for scrolling only.

  if (!digitalRead(TP_LEFT)) {
    left_button = 1;
  }
  else {
    left_button = 0;
  }

  if (!digitalRead(TP_RIGHT)) {
    right_button = 1;
  }
  else {
    right_button = 0;
  }

  // Determine if left or right button changed
  button_change = ((left_button ^ old_left_button) | (right_button ^ old_right_button));

  if (button_change) {
    Mouse.set_buttons(left_button, 0, right_button);
  }

  old_left_button = left_button;
  old_right_button = right_button;
  old_middle_button = middle_button;

  // **********Keyboard LEDs********************************************
  // keyboard_leds bit 1 = Caps Lock, bit 0 = Num Lock

  if (keyboard_leds & (1 << 1)) {
    go_0(CAPS_LED);
  }
  else {
    go_1(CAPS_LED);
  }

  if (keyboard_leds & 1) {
    go_0(NUM_LED);
  }
  else {
    go_1(NUM_LED);
  }
}