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
// This software implements an C64 Keyboard Controller using a Teensy LC connected
// directly with keyboard
// Keyboard pins 0-9 and A-F are connected to Teensy LC pins 0-18 respectively in order.
// IMPORTANT ! DO NOT CONNECT PIN 13 AS IT DRIVES LED AND GIVES RANDOM READING !
// This routine uses the Teensyduino "Micro-Manager Method" to send Normal and Modifier 
// keys over USB.
// Description of Teensyduino keyboard functions is at www.pjrc.com/teensy/td_keyboard.html
// 
// Revision History
// Initial Release Mar 27, 2019
//
//

// comment this line to have debug code included
#define NODEBUG

struct KeyDesc {
  const char* name;
  boolean isModifier;
  int pin1;
  int pin2;
  int keyCode;
  int keyCode2;
  boolean scanned;
};
struct KeyDesc Keys[] =
{
{ "CTRL", true, 6, 20, MODIFIERKEY_CTRL, 0 },
{ "LSHIFT", true, 8, 22, MODIFIERKEY_LEFT_SHIFT, 0 },
{ "RSHIFT", true, 10, 19, MODIFIERKEY_RIGHT_SHIFT, 0 },
{ "LALT (RSTOP)", true, 8, 20, MODIFIERKEY_LEFT_ALT, 0 },
{ "RALT (RESTORE)", true, 15, 18, MODIFIERKEY_RIGHT_ALT, 0 },
{ "GUI", true, 20, 12, MODIFIERKEY_GUI, 0 },

{ "ESC", false, 4, 20, KEY_ESC, 0 },
{ "1", false, 2, 20, KEY_1, 0 },
{ "2", false, 16, 20, KEY_2, 0 },
{ "3", false, 22, 2, KEY_3, 0 },
{ "4", false, 16, 22, KEY_4, 0 },
{ "5", false, 2, 24, KEY_5, 0 },
{ "6", false, 16, 24, KEY_6, 0 },
{ "7", false, 2, 26, KEY_7, 0 },
{ "8", false, 16, 26, KEY_8, 0 },
{ "9", false, 2, 23, KEY_9, 0 },
{ "0", false, 23, 16, KEY_0, 0 },
{ "+", false, 2, 21, KEYPAD_PLUS, 0 },
{ "-", false, 21, 16, KEYPAD_MINUS, 0 },
{ "END (Pound)", false, 19, 2, KEY_END },
{ "HOME", false, 16, 19, KEY_HOME, KEY_DELETE },
{ "BACKSPACE", false, 2, 17, KEY_BACKSPACE, KEY_INSERT },
{ "Q", false, 14, 20, KEY_Q, 0 },
{ "W", false, 4, 22, KEY_W, 0 },
{ "E", false, 14, 22, KEY_E, 0 },
{ "R", false, 4, 24, KEY_R, 0 },
{ "T", false, 14, 24, KEY_T, 0 },
{ "Y", false, 4, 26, KEY_Y, 0 },
{ "U", false, 26, 14, KEY_U, 0 },
{ "I", false, 23, 4, KEY_I, 0 },
{ "O", false, 23, 14, KEY_O, 0 },
{ "P", false, 21, 4, KEY_P, 0 },
{ "TAB (@)", false, 14, 21, KEY_TAB, 0 },
{ "*", false, 4, 19, KEYPAD_ASTERIX, 0 },
{ "BACKSLASH (Pi)", false, 19, 14, KEY_BACKSLASH, 0 },
{ "A", false, 6, 22, KEY_A, 0 },
{ "S", false, 22, 12, KEY_S, 0 },
{ "D", false, 6, 24, KEY_D, 0 },
{ "F", false, 12, 24, KEY_F, 0 },
{ "G", false, 6, 26, KEY_G, 0 },
{ "H", false, 12, 26, KEY_H, 0 },
{ "J", false, 23, 6, KEY_J, 0 },
{ "K", false, 23, 12, KEY_K, 0 },
{ "L", false, 21, 6, KEY_L, 0 },
{ "'", false, 12, 21, KEY_QUOTE, 0 },
{ ";", false, 6, 19, KEY_SEMICOLON, 0 },
{ "=", false, 12, 19, KEY_EQUAL, 0 },
{ "ENTER", false, 4, 17, KEY_ENTER, 0 },
{ "Z", false, 10, 22, KEY_Z, 0 },
{ "X", false, 8, 24, KEY_X, 0 },
{ "C", false, 10, 24, KEY_C, 0 },
{ "V", false, 8, 26, KEY_V, 0 },
{ "B", false, 10, 26, KEY_B, 0 },
{ "N", false, 23, 8, KEY_N, 0 },
{ "M", false, 23, 10, KEY_M, 0 },
{ ",", false, 8, 21, KEY_COMMA, 0 },
{ ".", false, 10, 21, KEY_PERIOD, 0 },
{ "/", false, 8, 19, KEY_SLASH, 0 },
{ "UP", false, 8, 17, KEY_DOWN, KEY_UP },
{ "LEFT", false, 6, 17, KEY_RIGHT, KEY_LEFT },
{ "F1", false, 10, 17, KEY_F1, KEY_F2 },
{ "F3", false, 12, 17, KEY_F3, KEY_F4 },
{ "F5", false, 17, 14, KEY_F5, KEY_F6 },
{ "F7", false, 16, 17, KEY_F7, KEY_F8 },
{ "SPACE", false, 10, 20, KEY_SPACE, 0 },
{ "none", false, 0, 0, 0, 0 } // last key - no key
};

// second keycode accessible via modifier
#define SECOND_CODE MODIFIERKEY_RIGHT_SHIFT

// number of teensy pins
#define PIN_MAX 26

// PIN to IO number; zero-index is not valid and hence -1
const int PinToIO[] = { -1, 23, 0, 22, 1, 24, 2, 21, 3, 25, 4, 20, 5, 19, 6, 18, 7, 17, 8, 16, 9, 15, 10, 14, 11, 26, 12 };

const int ForbiddenInputs[] = { -1 };
const int ForbiddenOutputs[] = { -1 };

boolean is_input_allowed(int pin) {
  for(int i = 0 ; ; i++) {
    int input = ForbiddenInputs[i];
    if (input <= 0) {
      break;
    }
    if (input == pin) {
      return false;
    }
  }
  return true;
}

boolean is_output_allowed(int pin) {
  for(int i = 0 ; ; i++) {
    int output = ForbiddenOutputs[i];
    if (output <= 0) {
      break;
    }
    if (output == pin) {
      return false;
    }
  }
  return true;
}

// slots hold the normal key values to be sent over USB. 
// WARNING - this must be set between 1 and 6
#define SLOT_NORMAL_COUNT 6
int slots[SLOT_NORMAL_COUNT];

// modifiers table holds - surprise - the modifier keys states
#define SLOT_MOD_COUNT 6
int mods[SLOT_MOD_COUNT];

// general check for button pressed
boolean is_pressed(int key, int* slot, int slotCount) {
  for(int i = 0; i < slotCount ; i++) {
    if (slot[i] == key) {
      return true;
    }
  }
  return false;
}

// general slot setup function, returns true on change
boolean set_slot(int key, boolean isPressed, int* slot, int slotCount) {
  boolean currentlyPressed = is_pressed(key, slot, slotCount);
  if (currentlyPressed == isPressed) {
    return false;
  }
  for(int i = 0; i < slotCount ; i++) {
    if ((isPressed && slot[i] == 0) || (!isPressed && slot[i] == key)) {
      slot[i] = isPressed ? key : 0;
      return true;
    }
  }
  return false;
}

// check if normal key pressed
boolean is_key_pressed(int key) {
  return is_pressed(key, slots, SLOT_NORMAL_COUNT);
}

// check if mod key pressed
boolean is_mod_pressed(int key) {
  return is_pressed(key, mods, SLOT_MOD_COUNT);
}

// Function to load the key name into the first available slot
boolean load_slot(int key) {
  return set_slot(key, true, slots, SLOT_NORMAL_COUNT);
}

// Function to clear the slot that contains the key name
boolean clear_slot(int key) {
  return set_slot(key, false, slots, SLOT_NORMAL_COUNT);
}

// Function to load the modifier key name into the appropriate mod variable
boolean load_mod(int m_key) {
  return set_slot(m_key, true, mods, SLOT_MOD_COUNT);
}

// Function to load 0 into the appropriate mod variable
boolean clear_mod(int m_key) {
  return set_slot(m_key, false, mods, SLOT_MOD_COUNT);
}
//
// Function to send the modifier keys over usb
void send_mod() {
  int totalMod = 0;
  for(int i = 0; i < SLOT_MOD_COUNT ; i++) {
    totalMod |= mods[i];
  }
  Keyboard.set_modifier(totalMod);
  Keyboard.send_now();
}
//
// Function to send the normal keys in the 6 slots over usb
void send_normals() {
  for(int i = 0 ; i < SLOT_NORMAL_COUNT ; i++) {
    int value = slots[i];
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
// find highest count less than given if given
int FindMaxPin(int* PinCount, int lessThan) {
  int maxPin = -1;
  int currentMax = -1;
  for(int i = 1 ; i <= PIN_MAX ; i++) {
    int value = PinCount[i];
    if (value > currentMax && (lessThan < 0 || value <= lessThan)) {
      maxPin = i;
      currentMax = value;
    }
  }
  return maxPin;
}

////// DEBUG CODE

void d_start() {
#ifndef NODEBUG
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

int KeyCount;
int PinOrderFromMax[PIN_MAX];
//
//----------------------------------Setup-------------------------------------------
void setup() {
  d_start();
  d_print("Setup Start");
  // setup keyboard tables
  for(int i = 0 ; i < SLOT_NORMAL_COUNT ; i++) {
    slots[i] = 0;
  }
  for(int i = 0 ; i < SLOT_MOD_COUNT ; i++) {
    mods[i] = 0;
  }
  int PinCount[PIN_MAX + 1];
  // calculate pin count given
  for(int i = 1 ; i <= PIN_MAX ; i++) {
    PinCount[i] = 0;
  }
  for(int i = 0 ; ; i++) {
    const struct KeyDesc& keyDesc = Keys[i];
    if (keyDesc.keyCode == 0) {
      KeyCount = i;
      break;
    }
    PinCount[keyDesc.pin1]++;
    PinCount[keyDesc.pin2]++;
  }
  int maxPin = -1;
  for(int i = 0 ; i < PIN_MAX ; i++) {
    maxPin = FindMaxPin(PinCount, maxPin > 0 ? PinCount[maxPin] : -1);
    if (maxPin > 0) {
      PinOrderFromMax[i] = maxPin;
      PinCount[maxPin] = -1;
    } else {
      PinOrderFromMax[i] = -1;
    }
  }
  for(int i = 1 ; i <= PIN_MAX ; i++) {
    go_pu(PinToIO[i]);
  }
  d_print("Setup OK");
}
//

//
//---------------------------------Main Loop---------------------------------------------
//
void loop() {
  // prepare table of scanned keys
  for(int i = 0 ; i < KeyCount ; i++) {
    Keys[i].scanned = false;
  }
  for(int i = 0 ; i < PIN_MAX ; i++) {
    // find maximum pin count in prepared table
    int pinAsOutput = PinOrderFromMax[i];
    if (pinAsOutput <= 0 || !is_output_allowed(pinAsOutput)) {
      continue;
    }
    // set this pin as output, find corresponding keys and set their other pins as input will pullup
    // input on other pins
    boolean anyKey = false;
    boolean hasUnscanned = false;
    for(int i = 0 ; i < KeyCount; i++) {
      KeyDesc& keyDesc = Keys[i];
      hasUnscanned |= !keyDesc.scanned;
      int pinAsInput = -1;
      if (keyDesc.pin1 == pinAsOutput) {
        pinAsInput = keyDesc.pin2;
      } else
      if (keyDesc.pin2 == pinAsOutput) {
        pinAsInput = keyDesc.pin1;
      }
      if (pinAsInput > 0 && is_input_allowed(pinAsInput) && !keyDesc.scanned) {
        anyKey = true;
        go_pu(PinToIO[pinAsInput]);
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
    go_0(PinToIO[pinAsOutput]);
    // delay to settle
    delayMicroseconds(10);
    // read state
    for(int i = 0 ; i < KeyCount ; i++) {
      KeyDesc& keyDesc = Keys[i];
      int pinAsInput = -1;
      if (keyDesc.pin1 == pinAsOutput) {
        pinAsInput = keyDesc.pin2;
      } else
      if (keyDesc.pin2 == pinAsOutput) {
        pinAsInput = keyDesc.pin1;
      }
      if (pinAsInput > 0 && !keyDesc.scanned) {
        keyDesc.scanned = true;
        int keyCode = keyDesc.keyCode;
        int keyCode2 = keyDesc.keyCode2;
        boolean useCode2 = keyCode2 != 0 && is_mod_pressed(SECOND_CODE);
        int currentValue = digitalRead(PinToIO[pinAsInput]);
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
            if (useCode2) {
              if (load_slot(keyCode2)) {
                d_print("Code 2 pressed: %s",keyDesc.name);
                clear_mod(SECOND_CODE);
                send_mod();
                send_normals();
                load_mod(SECOND_CODE);
                send_mod();
              }
            } else {
              // normal way
              if (load_slot(keyCode)) {
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
            if (useCode2) {
              if (clear_slot(keyCode2)) {
                d_print("Code 2 released: %s",keyDesc.name);
                send_normals();
              }
            } else {
              // normal way
              if (clear_slot(keyCode)) {
                d_print("Normal released: %s",keyDesc.name);
                send_normals();
              }
            }
          }
        }
      }
    }
    // disable pin
    go_z(PinToIO[pinAsOutput]);
  }
//
// **********keyboard scan complete
//
// Turn on the LED on the Teensy for Caps Lock based on bit 1 in the keyboard_leds variable controlled by the USB host computer
//
  if (is_mod_pressed(MODIFIERKEY_LEFT_SHIFT) || is_mod_pressed(MODIFIERKEY_RIGHT_SHIFT)) {  // mask off all bits but D1 and test if set
    go_1(LED_BUILTIN); // turn on the LED
  } else {
    go_0(LED_BUILTIN); // turn off the LED
  }
//
  delay(25); // The overall keyboard scanning rate is about 30ms
}
