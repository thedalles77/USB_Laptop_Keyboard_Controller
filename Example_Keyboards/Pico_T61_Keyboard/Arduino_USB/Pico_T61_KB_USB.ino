/*
    Copyright 2026 Frank Adams

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

This code makes a Lenovo T61 laptop keyboard work as a USB keyboard using a
Raspberry Pi Pico (or Pico W, Pico 2, Pico 2 W) and the Adafruit TinyUSB library.
The mute, volume up and volume down keys are sent as USB media (consumer control) keys.
Arduino IDE settings: Tools > USB Stack = Adafruit TinyUSB
*/

#include <Adafruit_TinyUSB.h>

// HID report IDs: one report for normal keys, one for media (consumer control) keys
enum {
  RID_KEYBOARD = 1,
  RID_CONSUMER_CONTROL
};

// HID report descriptor: keyboard + consumer control (mute, volume)
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL))
};

// Create the TinyUSB HID object instance
Adafruit_USBD_HID usb_hid;

const int NUM_ROWS = 16; // number of row signals
const int NUM_COLS = 8; // number of column signals

// column and row GPIO numbers attached to the column and row pins of the keyboard
const int colPins[NUM_COLS] = {11, 22, 10, 21, 9, 20, 8, 7};
const int rowPins[NUM_ROWS] = {27, 26, 17, 14, 12, 19, 18, 15, 13, 6, 16, 5, 28, 2, 4, 3};

// Placeholder tokens for the media keys. They are sent as consumer control codes, not keyboard keys.
#define TOKEN_MUTE_KEY 0xFE
#define TOKEN_VOL_UP   0xFD
#define TOKEN_VOL_DOWN 0xFC

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  //  GP11                  GP22                  GP10              GP21                   GP9                GP20                GP8                    GP7
  { HID_KEY_GRAVE,        HID_KEY_1,            HID_KEY_Q,        HID_KEY_TAB,           HID_KEY_A,         HID_KEY_ESCAPE,     HID_KEY_Z,             0                    },  // GP27
  { HID_KEY_F1,           HID_KEY_2,            HID_KEY_W,        HID_KEY_CAPS_LOCK,     HID_KEY_S,         0,                  HID_KEY_X,             0                    },  // GP26
  { HID_KEY_F2,           HID_KEY_3,            HID_KEY_E,        HID_KEY_F3,            HID_KEY_D,         HID_KEY_F4,         HID_KEY_C,             0                    },  // GP17
  { HID_KEY_5,            HID_KEY_4,            HID_KEY_R,        HID_KEY_T,             HID_KEY_F,         HID_KEY_G,          HID_KEY_V,             HID_KEY_B            },  // GP14
  { HID_KEY_6,            HID_KEY_7,            HID_KEY_U,        HID_KEY_Y,             HID_KEY_J,         HID_KEY_H,          HID_KEY_M,             HID_KEY_N            },  // GP12
  { HID_KEY_EQUAL,        HID_KEY_8,            HID_KEY_I,        HID_KEY_BRACKET_RIGHT, HID_KEY_K,         HID_KEY_F6,         HID_KEY_COMMA,         0                    },  // GP19
  { HID_KEY_F8,           HID_KEY_9,            HID_KEY_O,        HID_KEY_F7,            HID_KEY_L,         0,                  HID_KEY_PERIOD,        0                    },  // GP18
  { HID_KEY_MINUS,        HID_KEY_0,            HID_KEY_P,        HID_KEY_BRACKET_LEFT,  HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, 0,                     HID_KEY_SLASH        },  // GP15
  { HID_KEY_F9,           HID_KEY_F10,          0,                HID_KEY_BACKSPACE,     HID_KEY_BACKSLASH, HID_KEY_F5,         HID_KEY_ENTER,         HID_KEY_SPACE        },  // GP13
  { HID_KEY_INSERT,       HID_KEY_F12,          0,                0,                     0,                 0,                  0,                     HID_KEY_ARROW_RIGHT  },  // GP6
  { HID_KEY_DELETE,       HID_KEY_F11,          TOKEN_VOL_UP,     TOKEN_VOL_DOWN,        TOKEN_MUTE_KEY,    0,                  0,                     HID_KEY_ARROW_DOWN   },  // GP16
  { HID_KEY_PAGE_UP,      HID_KEY_PAGE_DOWN,    HID_KEY_GUI_LEFT, 0,                     0,                 0,                  0,                     0                    },  // GP5
  { HID_KEY_HOME,         HID_KEY_END,          0,                0,                     0,                 HID_KEY_ARROW_UP,   0,                     HID_KEY_ARROW_LEFT   },  // GP28
  { 0,                    HID_KEY_PRINT_SCREEN, 0,                0,                     0,                 HID_KEY_ALT_LEFT,   0,                     HID_KEY_ALT_RIGHT    },  // GP2
  { 0,                    0,                    0,                HID_KEY_SHIFT_LEFT,    0,                 0,                  HID_KEY_SHIFT_RIGHT,   0                    },  // GP4
  { HID_KEY_CONTROL_LEFT, 0,                    0,                0,                     0,                 0,                  HID_KEY_CONTROL_RIGHT, 0                    }   // GP3
};

// Current state of every key from the latest scan (true = pressed)
bool keyState[NUM_ROWS][NUM_COLS] = {false};

// Remember the last reports we transmitted so we can suppress duplicates
uint8_t lastModifier = 0;
uint8_t lastReport[6] = {0, 0, 0, 0, 0, 0};
uint16_t lastConsumer = 0;

// Wait (up to 20 msec) for USB to be ready for the next report
static bool waitUsbReady() {
  unsigned long start = millis();
  while (!usb_hid.ready()) {
    if (millis() - start > 20) return false;
    delay(1);
  }
  return true;
}

void setup() {
  // Start the USB HID device first. The Pico core starts USB before setup()
  // runs, so the HID interface must be added as early as possible.
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("T61 USB Keyboard");
  usb_hid.begin();

  // If the PC already enumerated the Pico without the keyboard, reconnect so it sees it
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the col gpio's inputs with pullups
  }
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT_PULLUP); // Make the row gpio's inputs with pullups
  }

  // Wait until the PC has finished connecting the keyboard
  while (!TinyUSBDevice.mounted()) delay(1);
}

// ---------------------------------------------------------------------
// Loop:
//   1) Scans the full 16x8 matrix into keyState[][].
//   2) Builds the keyboard report: modifier bits and up to 6 normal keycodes.
//      Mute / volume keys become a consumer (media) code instead.
//   3) Sends each report only when it differs from the one last sent.
// ---------------------------------------------------------------------
void loop() {
  uint8_t  modifierByte      = 0;
  uint8_t  keycodeReport[6]  = {0, 0, 0, 0, 0, 0};
  int      keyCount          = 0;
  bool     rolloverExceeded  = false;
  uint16_t consumerCode      = 0;

  // 1) Full-matrix scan --------------------------------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);                      // drive this row low
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(20);                            // settle (weak 50K pullups)

    for (int c = 0; c < NUM_COLS; c++) {
      keyState[r][c] = (digitalRead(colPins[c]) == LOW);
    }

    pinMode(rowPins[r], INPUT_PULLUP);                // release the row
  }

  // 2) Build the reports from all held keys --------------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      if (!keyState[r][c]) continue;
      uint8_t key = keyMap[r][c];
      if (key == 0) continue;                         // empty position

      if (key == TOKEN_MUTE_KEY || key == TOKEN_VOL_UP || key == TOKEN_VOL_DOWN) {
        if (consumerCode == 0) {                      // one media key at a time
          if (key == TOKEN_MUTE_KEY)    consumerCode = HID_USAGE_CONSUMER_MUTE;
          else if (key == TOKEN_VOL_UP) consumerCode = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
          else                          consumerCode = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
        }
      } else if (key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT) {
        // Modifier: fold into the modifier byte
        modifierByte |= (uint8_t)(1u << (key - HID_KEY_CONTROL_LEFT));
      } else if (keyCount < 6) {
        keycodeReport[keyCount++] = key;              // Normal key (6KRO)
      } else {
        rolloverExceeded = true;
      }
    }
  }

  // HID spec: fill all 6 slots with ErrorRollOver (0x01) when more than 6 normal keys are held.
  if (rolloverExceeded) {
    for (int i = 0; i < 6; i++) keycodeReport[i] = 0x01;
  }

  // 3) Send reports that changed ---------------------------------------------
  // A report is only marked as sent if USB accepted it, so a change is never lost.
  bool reportDiffers = (modifierByte != lastModifier);
  for (int i = 0; i < 6 && !reportDiffers; i++) {
    if (keycodeReport[i] != lastReport[i]) reportDiffers = true;
  }
  if (reportDiffers && waitUsbReady()) {
    if (usb_hid.keyboardReport(RID_KEYBOARD, modifierByte, keycodeReport)) {
      lastModifier = modifierByte;
      for (int i = 0; i < 6; i++) lastReport[i] = keycodeReport[i];
    }
  }

  // Media key press (code) or release (0)
  if (consumerCode != lastConsumer && waitUsbReady()) {
    if (usb_hid.sendReport16(RID_CONSUMER_CONTROL, consumerCode)) {
      lastConsumer = consumerCode;
    }
  }

  delay(8); // ~125 Hz scan rate - slow enough to eliminate key bounce
}
