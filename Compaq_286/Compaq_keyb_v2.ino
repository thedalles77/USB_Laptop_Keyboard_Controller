/* Copyright 2019 Olga, based on work of Frank Adams
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
// This software implements old Compaq keyboard controller on Teensy 3.5 connected directly with keyboard
// IMPORTANT ! DO NOT CONNECT PIN 13 AS IT DRIVES LED AND GIVES RANDOM READING !
// This routine uses the Teensyduino "Micro-Manager Method" to send Normal and Modifier 
// keys over USB.
// Description of Teensyduino keyboard functions is at www.pjrc.com/teensy/td_keyboard.html
// 
// Revision History
// Initial Release Feb 14, 2021
//
//

// comment this line to have debug code included
#define NODEBUG

// keyboard to teensy connection definitions
#define KBD_X   32
#define KBD_FN  34
#define KBD_LSH 35
#define KBD_LCT 36
#define KBD_RSH 37
#define KBD_RCT 38
#define KBD_RAL 39
#define KBD_LAL 14
#define KBD_A   15
#define KBD_B   16
#define KBD_C   17
#define KBD_D   18
#define KBD_E   19
#define KBD_F   20
#define KBD_G   21
#define KBD_H   22
#define KBD_1   30
#define KBD_2   29
#define KBD_3   28
#define KBD_4   27
#define KBD_5   26
#define KBD_6   25
#define KBD_7   24
#define KBD_8   12
#define KBD_9   11
#define KBD_10  10
#define KBD_11   8
#define KBD_CAPS 2
#define KBD_NUM  3
#define KBD_SCR  4

#define USB_LED_NUM_LOCK 0
#define USB_LED_CAPS_LOCK 1
#define USB_LED_SCROLL_LOCK 2

struct KeyDesc {
  const char* name;
  boolean isModifier;
  int pin1;
  int pin2;
  int keyCode;
  int fnKeyCode;
  int numKeyCode;
  int scrKeyCode;
  boolean scanned;
};

struct KeySent {
  int rawKey;
  int sendKey;
};

struct KeyDesc Keys[] =
{
{ "LCTRL", true, KBD_X, KBD_LCT, MODIFIERKEY_LEFT_CTRL, 0, 0, 0 },
{ "RCTRL", true, KBD_X, KBD_RCT, MODIFIERKEY_RIGHT_CTRL, 0, 0, 0 },
{ "LSHIFT", true, KBD_X, KBD_LSH, MODIFIERKEY_LEFT_SHIFT, 0, 0, 0 },
{ "RSHIFT", true, KBD_X, KBD_RSH, MODIFIERKEY_RIGHT_SHIFT, 0, 0, 0 },
{ "LALT", true, KBD_X, KBD_LAL, MODIFIERKEY_LEFT_ALT, 0, 0, 0 },
{ "RALT", true, KBD_X, KBD_RAL, MODIFIERKEY_RIGHT_ALT, 0, 0, 0 },
{ "FN", true, KBD_X, KBD_FN, MODIFIERKEY_GUI, 0, 0, 0 },

{ "ESC", false, KBD_A, KBD_3, KEY_ESC, 0, 0, 0 },
{ "F1", false, KBD_A, KBD_2, KEY_F1, 0, 0, 0 },
{ "F2", false, KBD_A, KBD_4, KEY_F2, 0, 0, 0 },
{ "F3", false, KBD_A, KBD_1, KEY_F3, 0, 0, 0 },
{ "F4", false, KBD_A, KBD_8, KEY_F4, 0, 0, 0 },
{ "F5", false, KBD_A, KBD_11, KEY_F5, 0, 0, 0 },
{ "F6", false, KBD_A, KBD_5, KEY_F6, 0, 0, 0 },
{ "F7", false, KBD_A, KBD_7, KEY_F7, 0, 0, 0 },
{ "F8", false, KBD_A, KBD_6, KEY_F8, 0, 0, 0 },
{ "F9", false, KBD_A, KBD_10, KEY_F9, 0, 0, 0 },
{ "F10", false, KBD_A, KBD_9, KEY_F10, 0, 0, 0 },
{ "F11", false, KBD_G, KBD_9, KEY_F11, 0, 0, 0 },
{ "F12", false, KBD_G, KBD_10, KEY_F12, 0, 0, 0 },
{ "NUMLCK", false, KBD_G, KBD_6, KEY_NUM_LOCK, 0, 0, 0 },
{ "PRTSCR", false, KBD_G, KBD_7, KEY_PRINTSCREEN, 0, 0, 0 },

{ "~", false, KBD_B, KBD_3, KEY_TILDE, 0, 0, 0 },
{ "1", false, KBD_B, KBD_2, KEY_1, 0, 0, 0 },
{ "2", false, KBD_B, KBD_4, KEY_2, 0, 0, 0 },
{ "3", false, KBD_B, KBD_1, KEY_3, 0, 0, 0 },
{ "4", false, KBD_B, KBD_8, KEY_4, 0, 0, 0 },
{ "5", false, KBD_B, KBD_11, KEY_5, 0, 0, 0 },
{ "6", false, KBD_B, KBD_5, KEY_6, 0, 0, 0 },
{ "7", false, KBD_B, KBD_7, KEY_7, KEY_HOME, KEYPAD_7, 0 },
{ "8", false, KBD_B, KBD_6, KEY_8, KEY_UP, KEYPAD_8, 0 },
{ "9", false, KBD_B, KBD_10, KEY_9, KEY_PAGE_UP, KEYPAD_9, 0 },
{ "0", false, KBD_B, KBD_9, KEY_0, 0, KEYPAD_ASTERIX, 0 },
{ "-(B)", false, KBD_F, KBD_9, KEY_MINUS, 0, 0, 0 },
{ "=(Apo)", false, KBD_F, KBD_10, KEY_EQUAL, 0, 0, 0 },
{ "BCKSPC", false, KBD_F, KBD_6, KEY_BACKSPACE, 0, 0, 0 },
{ "SCRLCK", false, KBD_F, KBD_7, KEY_SCROLL_LOCK, 0, 0, 0 },

{ "TAB", false, KBD_C, KBD_3, KEY_TAB, 0, 0, 0 },
{ "Q", false, KBD_C, KBD_2, KEY_Q, 0, 0, 0 },
{ "W", false, KBD_C, KBD_4, KEY_W, 0, 0, 0 },
{ "E", false, KBD_C, KBD_1, KEY_E, 0, 0, 0 },
{ "R", false, KBD_C, KBD_8, KEY_R, 0, 0, 0 },
{ "T", false, KBD_C, KBD_11, KEY_T, 0, 0, 0 },
{ "Y", false, KBD_C, KBD_5, KEY_Y, 0, 0, 0 },
{ "U", false, KBD_C, KBD_7, KEY_U, KEY_LEFT, KEYPAD_4, 0 },
{ "I", false, KBD_C, KBD_6, KEY_I, 0, KEYPAD_5, 0 },
{ "O", false, KBD_C, KBD_10, KEY_O, KEY_RIGHT, KEYPAD_6, 0 },
{ "P", false, KBD_C, KBD_9, KEY_P, 0, KEYPAD_MINUS, 0 },
{ "[(Ue)", false, KBD_H, KBD_9, KEY_LEFT_BRACE, 0, 0, 0 },
{ "](+~)", false, KBD_H, KBD_10, KEY_RIGHT_BRACE, 0, 0, 0 },
{ "ENTER", false, KBD_H, KBD_8, KEY_ENTER, 0, KEYPAD_ENTER, 0 },
{ "PAUSE", false, KBD_H, KBD_7, KEY_PAUSE, 0, 0, 0 },

{ "CAPSLCK", false, KBD_D, KBD_2, KEY_CAPS_LOCK, 0, 0, 0 },
{ "A", false, KBD_D, KBD_4, KEY_A, 0, 0, 0 },
{ "S", false, KBD_D, KBD_1, KEY_S, 0, 0, 0 },
{ "D", false, KBD_D, KBD_8, KEY_D, 0, 0, 0 },
{ "F", false, KBD_D, KBD_11, KEY_F, 0, 0, 0 },
{ "G", false, KBD_D, KBD_5, KEY_G, 0, 0, 0 },
{ "H", false, KBD_D, KBD_3, KEY_H, 0, 0, 0 },
{ "J", false, KBD_D, KBD_7, KEY_J, KEY_END, KEYPAD_1, 0 },
{ "K", false, KBD_D, KBD_6, KEY_K, KEY_DOWN, KEYPAD_2, 0 },
{ "L", false, KBD_D, KBD_10, KEY_L, KEY_PAGE_DOWN, KEYPAD_3, 0 },
{ ";(Oe)", false, KBD_D, KBD_9, KEY_SEMICOLON, 0, KEYPAD_PLUS, 0 },
{ "'(Ae)", false, KBD_H, KBD_5, KEY_QUOTE, 0, 0, 0 },
{ "\\(#)", false, KBD_H, KBD_11, KEY_BACKSLASH, 0, 0, 0 },
{ "INS", false, KBD_H, KBD_1, KEY_INSERT, 0, 0, 0 },

{ "<>", false, KBD_E, KBD_4, KEY_MENU, 0, 0, 0 },
{ "Z", false, KBD_E, KBD_1, KEY_Z, 0, 0, 0 },
{ "X", false, KBD_E, KBD_8, KEY_X, 0, 0, 0 },
{ "C", false, KBD_E, KBD_11, KEY_C, 0, 0, 0 },
{ "V", false, KBD_E, KBD_5, KEY_V, 0, 0, 0 },
{ "B", false, KBD_E, KBD_3, KEY_B, 0, 0, 0 },
{ "N", false, KBD_E, KBD_7, KEY_N, 0, 0, 0 },
{ "M", false, KBD_E, KBD_6, KEY_M, 0, 0, 0 },
{ ",", false, KBD_E, KBD_10, KEY_COMMA, KEY_INSERT, KEYPAD_0, 0 },
{ ".", false, KBD_E, KBD_9, KEY_PERIOD, KEY_DELETE, KEYPAD_PERIOD, 0 },
{ "/", false, KBD_F, KBD_5, KEY_SLASH, 0, KEYPAD_SLASH, 0 },
{ "DEL", false, KBD_F, KBD_1, KEY_DELETE, 0, 0, 0 },

{ "SPACE", false, KBD_G, KBD_3, KEY_SPACE, 0, 0, 0 },

{ "UP", false, KBD_F, KBD_8, KEY_UP, KEY_PAGE_UP, 0, KEY_PAGE_UP },
{ "LEFT", false, KBD_G, KBD_11, KEY_LEFT, KEY_HOME, 0, KEY_HOME },
{ "DOWN", false, KBD_G, KBD_8, KEY_DOWN, KEY_PAGE_DOWN, 0, KEY_PAGE_DOWN },
{ "RIGHT", false, KBD_G, KBD_1, KEY_RIGHT, KEY_END, 0, KEY_END },

{ "none", false, 0, 0, 0, 0 } // last key - no key
};

// second keycode accessible via modifier
#define SECOND_CODE MODIFIERKEY_GUI

// number of teensy pins
#define PIN_MAX 39

// slots hold the normal key values to be sent over USB. 
// WARNING - this must be set between 1 and 6
#define SLOT_NORMAL_COUNT 6
struct KeySent slots[SLOT_NORMAL_COUNT];

// modifiers table holds - surprise - the modifier keys states
#define SLOT_MOD_COUNT 6
struct KeySent mods[SLOT_MOD_COUNT];

// general check for button pressed - uses raw key
boolean is_pressed(int rawKey, const struct KeySent* slot, int slotCount) {
  for(int i = 0; i < slotCount ; i++) {
    if (slot[i].rawKey == rawKey) {
      return true;
    }
  }
  return false;
}

// general slot setup function, returns true on change
boolean set_slot(int rawKey, int sendKey, boolean isPressed, struct KeySent* slot, int slotCount) {
  boolean currentlyPressed = is_pressed(rawKey, slot, slotCount);
  if (currentlyPressed == isPressed) {
    return false;
  }
  for(int i = 0; i < slotCount ; i++) {
    if ((isPressed && slot[i].rawKey == 0) || (!isPressed && slot[i].rawKey == rawKey)) {
      slot[i].rawKey = isPressed ? rawKey : 0;
      slot[i].sendKey = isPressed ? sendKey : 0;
      return true;
    }
  }
  return false;
}

// check if normal key pressed
boolean is_key_pressed(int rawKey) {
  return is_pressed(rawKey, slots, SLOT_NORMAL_COUNT);
}

// check if mod key pressed
boolean is_mod_pressed(int rawKey) {
  return is_pressed(rawKey, mods, SLOT_MOD_COUNT);
}

// Function to load the key name into the first available slot
boolean load_slot(int rawKey, int sendKey) {
  return set_slot(rawKey, sendKey, true, slots, SLOT_NORMAL_COUNT);
}

// Function to clear the slot that contains the key name
boolean clear_slot(int rawKey) {
  return set_slot(rawKey, 0, false, slots, SLOT_NORMAL_COUNT);
}

// Function to load the modifier key name into the appropriate mod variable
boolean load_mod(int m_key) {
  return set_slot(m_key, m_key, true, mods, SLOT_MOD_COUNT);
}

// Function to load 0 into the appropriate mod variable
boolean clear_mod(int m_key) {
  return set_slot(m_key, 0, false, mods, SLOT_MOD_COUNT);
}
//
// Function to send the modifier keys over usb
void send_mod() {
  int totalMod = 0;
  for(int i = 0; i < SLOT_MOD_COUNT ; i++) {
    totalMod |= mods[i].sendKey;
  }
  Keyboard.set_modifier(totalMod);
  Keyboard.send_now();
}
//
// Function to send the normal keys in the 6 slots over usb
void send_normals() {
  for(int i = 0 ; i < SLOT_NORMAL_COUNT ; i++) {
    int value = slots[i].sendKey;
    switch(i) {
      case 0:
        Keyboard.set_key1(value);
        break;
      case 1:
        Keyboard.set_key2(value);
        break;
      case 2:
        Keyboard.set_key3(value);
        break;
      case 3:
        Keyboard.set_key4(value);
        break;
      case 4:
        Keyboard.set_key5(value);
        break;
      case 5:
        Keyboard.set_key6(value);
        break;
       default:
         // no send here
         break;
    }
  }
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

////// DEBUG CODE

void d_start() {
#ifndef NODEBUG
  while( !Serial );
  Serial.begin(9600);
#endif
}

void d_print(const char* format, ...) {
#ifndef NODEBUG
  va_list(aptr);
  int ret;
  char buffer[200];
  va_start(aptr, format);
  ret = vsprintf(buffer, format, aptr);
  va_end(aptr);
  if (ret >= 0) {
    Serial.println(buffer);
  } else {
    Serial.println("--- error constructing debug log ---");
  }
#endif
}

////// END DEBUG CODE

int KeyCount = 0;
int readPins = 0;
int readPIN[PIN_MAX];
int writePins = 0;
int writePIN[PIN_MAX];

boolean is_in_array(int val, int* arr, int max) {
  for(int i = 0 ; i < max ; i++) {
    if (arr[i] == val) {
      return true;
    }
  }
  return false;
}

void blink() {
  go_0(KBD_CAPS);
  go_0(KBD_NUM);
  go_0(KBD_SCR);
  delay(200);
  go_1(KBD_CAPS);
  go_1(KBD_NUM);
  go_1(KBD_SCR);
}

//
//----------------------------------Setup-------------------------------------------
void setup() {
  d_start();
  d_print("Setup Start: LED");
  Keyboard.begin();
  go_1(KBD_CAPS);
  go_1(KBD_NUM);
  go_1(KBD_SCR);
  d_print("Setup LED complete, setup tables start.");
  // setup keyboard tables
  for(int i = 0 ; i < SLOT_NORMAL_COUNT ; i++) {
    slots[i].rawKey = 0;
    slots[i].sendKey = 0;
  }
  for(int i = 0 ; i < SLOT_MOD_COUNT ; i++) {
    mods[i].rawKey = 0;
    mods[i].sendKey = 0;
  }
  // initialize readers and writers
  for(int i = 0 ; i < PIN_MAX ; i++) {
    readPIN[i] = -1;
    writePIN[i] = -1;
  }
  for(int i = 0 ; ; i++) {
    struct KeyDesc& keyDesc = Keys[i];
    if (keyDesc.keyCode == 0) {
      KeyCount = i;
      break;
    }
    int pin1 = keyDesc.pin1;
    boolean pin1InRead = is_in_array(pin1, readPIN, readPins);
    boolean pin1InWrite = is_in_array(pin1, writePIN, writePins);
    int pin2 = keyDesc.pin2;
    boolean pin2InRead = is_in_array(pin2, readPIN, readPins);
    boolean pin2InWrite = is_in_array(pin2, writePIN, writePins);
    if (pin1InRead || pin2InWrite) {
      // means changed places of pins
      pin1 = keyDesc.pin2;
      pin2 = keyDesc.pin1;
      keyDesc.pin1 = pin1;
      keyDesc.pin2 = pin2;
      boolean pinInRead = pin1InRead;
      boolean pinInWrite = pin1InWrite;
      pin1InRead = pin2InRead;
      pin1InWrite = pin2InWrite;
      pin2InRead = pinInRead;
      pin2InWrite = pinInWrite;
#ifndef NODEBUG
      char text[100];
      sprintf(text, "Swapped pins for key %s, pins now are %d %d", keyDesc.name, keyDesc.pin1, keyDesc.pin2);
      d_print(text);
#endif
    }
    if ((pin1InRead && pin1InWrite) || (pin2InRead && pin2InWrite) || (pin1InRead && pin2InRead) || (pin1InWrite && pin2InWrite)) {
#ifndef NODEBUG
      char text[100];
      sprintf(text, "Error in keyboard configuration for key %s pins %d %d", keyDesc.name, keyDesc.pin1, keyDesc.pin2);
      d_print(text);
#endif
      continue;
    }
    if (!pin1InWrite) {
      writePIN[writePins++] = pin1;
    }
    if (!pin2InRead) {
      readPIN[readPins++] = pin2;
    }
  }
  for(int i = 0 ; i < writePins ; i++) {
    go_z(readPIN[i]);
  }
  for(int i = 0 ; i < writePins ; i++) {
    go_z(writePIN[i]);
  }
  d_print("Setup OK");
  blink();
}
//

//
//---------------------------------Main Loop---------------------------------------------
//
void loop() {
  // read LED state
  boolean isCapsLock = keyboard_leds & (1<<USB_LED_CAPS_LOCK);
  boolean isNumLock = keyboard_leds & (1<<USB_LED_NUM_LOCK);
  boolean isScrollLock = keyboard_leds & (1<<USB_LED_SCROLL_LOCK);
  // prepare table of scanned keys
  for(int i = 0 ; i < KeyCount ; i++) {
    Keys[i].scanned = false;
  }
  for(int j = 0 ; j < writePins ; j++) {
    // find maximum pin count in prepared table
    int pinAsOutput = writePIN[j];
    // set this pin as output, find corresponding keys and set their other pins as input will pullup
    // input on other pins
    boolean anyKey = false;
    boolean hasUnscanned = false;
    for(int i = 0 ; i < KeyCount; i++) {
      KeyDesc& keyDesc = Keys[i];
      hasUnscanned |= !keyDesc.scanned;
      if (keyDesc.pin1 == pinAsOutput && !keyDesc.scanned) {
        anyKey = true;
        go_pu(keyDesc.pin2);
      }
    }
    // if no unscanned keys found, break
    if (!hasUnscanned) {
      break;
    }
    // if no keys for this pin found, next loop
    if (!anyKey) {
      continue;
    }
    // logic LOW will set this to active
    go_0(pinAsOutput);
    // delay to settle
    delayMicroseconds(10);
    // read state
    for(int i = 0 ; i < KeyCount ; i++) {
      KeyDesc& keyDesc = Keys[i];
      if (keyDesc.pin1 == pinAsOutput && !keyDesc.scanned) {
        keyDesc.scanned = true;
        int keyCode = keyDesc.keyCode;
        int sendCode = keyCode;
        int fnKeyCode = keyDesc.fnKeyCode;
        boolean useFnCode = fnKeyCode != 0 && is_mod_pressed(SECOND_CODE);
        if (!useFnCode) {
          if (isScrollLock && keyDesc.scrKeyCode != 0) {
            sendCode = keyDesc.scrKeyCode;
          }
          if (isNumLock && keyDesc.numKeyCode != 0) {
            sendCode = keyDesc.numKeyCode;
          }
        }
        int currentValue = digitalRead(keyDesc.pin2);
        go_z(keyDesc.pin2);
        if (!currentValue) {
          // new key pressed
          if (keyDesc.isModifier) {
            if (load_mod(keyCode)) {
              d_print("Mod pressed: %s",keyDesc.name);
              send_mod();
            }
          } else {
            // if must use code2, then release keyCode and substitute keyCode with second code
            // then release SECOND_CODE, then press code2, then re-press SECOND_CODE
            if (useFnCode) {
              if (load_slot(keyCode, fnKeyCode)) {
                d_print("Fn pressed: %s",keyDesc.name);
                clear_mod(SECOND_CODE);
                send_mod();
                send_normals();
                load_mod(SECOND_CODE);
                send_mod();
              }
            } else {
              // normal way
              if (load_slot(keyCode, sendCode)) {
                d_print("Normal pressed: %s",keyDesc.name);
                send_normals();
              }
            }
          }
        } else { // key released
          if (keyDesc.isModifier) {
            if (clear_mod(keyCode)) {
              d_print("Mod released: %s",keyDesc.name);
              send_mod();
            }
          } else {
            if (clear_slot(keyCode)) {
              d_print("Released: %s",keyDesc.name);
              send_normals();
            }
          }
        }
      }
    }
    // disable pin
    go_z(pinAsOutput);
  }
//
// **********keyboard scan complete
//
// Turn on the LEDs
//
  if (isCapsLock) {
    go_0(KBD_CAPS);
  } else {
    go_1(KBD_CAPS);
  }
  if (isNumLock) {
    go_0(KBD_NUM);
  } else {
    go_1(KBD_NUM);
  }
  if (isScrollLock) {
    go_0(KBD_SCR);
  } else {
    go_1(KBD_SCR);
  }
//
  delay(25); // The overall keyboard scanning rate is about 30ms
}
