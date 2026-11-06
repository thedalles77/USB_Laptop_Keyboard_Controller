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

This code makes a Dell Inspiron 1545 laptop keyboard work as a standalone USB keyboard
using the 26 pin Raspberry Pi Pico keyboard controller board.
Holding the Fn key turns F7, F8, F9 into Mute, Volume Down, Volume Up.
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


const int NUM_ROWS = 17; // number of row signals
const int NUM_COLS = 8; // number of column signals

// column and row GPIO numbers attached to the column and row pins of the keyboard 
const int colPins[NUM_COLS] = {1, 2, 3, 4, 5, 6, 27, 28};
const int rowPins[NUM_ROWS] = {7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26};

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { 0,                  0,                      HID_KEY_PAGE_DOWN,  HID_KEY_GUI_LEFT,     0,                      0,                  0,                    HID_KEY_PAGE_UP },
  { 0,                  HID_KEY_CONTROL_RIGHT,  0,                  0,                    0,                      0,                  HID_KEY_CONTROL_LEFT, 0 },
  { HID_KEY_ALT_RIGHT,  0,                      0,                  HID_KEY_PRINT_SCREEN, 0,                      HID_KEY_ALT_LEFT,   0,                    0 },
  { 0,                  HID_KEY_Z,              HID_KEY_A,          HID_KEY_1,            HID_KEY_TAB,            HID_KEY_ESCAPE,     HID_KEY_GRAVE,        HID_KEY_Q },
  { 0,                  HID_KEY_C,              HID_KEY_D,          HID_KEY_3,            HID_KEY_F3,             HID_KEY_F4,         HID_KEY_F2,           HID_KEY_E },
  { HID_KEY_SPACE,      HID_KEY_ENTER,          HID_KEY_BACKSLASH,  HID_KEY_F10,          HID_KEY_BACKSPACE,      HID_KEY_F5,         HID_KEY_F9,           0 },
  { 0,                  HID_KEY_COMMA,          HID_KEY_K,          HID_KEY_8,            HID_KEY_BRACKET_RIGHT,  HID_KEY_F6,         HID_KEY_EQUAL,        HID_KEY_I },
  { 0,                  HID_KEY_PERIOD,         HID_KEY_L,          HID_KEY_9,            HID_KEY_F7,             0,                  HID_KEY_F8,           HID_KEY_O },
  { HID_KEY_ARROW_LEFT, 0,                      0,                  HID_KEY_END,          0,                      HID_KEY_ARROW_UP,   HID_KEY_HOME,         0 },
  { HID_KEY_ARROW_RIGHT,0,                      0,                  HID_KEY_F12,          0,                      0,                  HID_KEY_INSERT,       0 },
  { HID_KEY_ARROW_DOWN, 0,                      0,                  HID_KEY_F11,          0,                      0,                  HID_KEY_DELETE,       0 }, 
  { HID_KEY_SLASH,      0,                      HID_KEY_SEMICOLON,  HID_KEY_0,            HID_KEY_BRACKET_LEFT,   HID_KEY_APOSTROPHE, HID_KEY_MINUS,        HID_KEY_P },
  { HID_KEY_N,          HID_KEY_M,              HID_KEY_J,          HID_KEY_7,            HID_KEY_Y,              HID_KEY_H,          HID_KEY_6,            HID_KEY_U },
  { HID_KEY_B,          HID_KEY_V,              HID_KEY_F,          HID_KEY_4,            HID_KEY_T,              HID_KEY_G,          HID_KEY_5,            HID_KEY_R },
  { 0,                  HID_KEY_X,              HID_KEY_S,          HID_KEY_2,            HID_KEY_CAPS_LOCK,      0,                  HID_KEY_F1,           HID_KEY_W },
  { 0,                  HID_KEY_SHIFT_RIGHT,    0,                  0,                    HID_KEY_SHIFT_LEFT,     0,                  0,                    0 },
  { 0,                  0,                      0,                  0,                    0,                      0,                  0,                    0 }
};

// Fn key location in the matrix (row GP26, column GP3). It has no keycode in keyMap.
const int FN_ROW = 16;   // GP26
const int FN_COL = 2;    // GP3

// Media keys sent when Fn is held: matrix position of the F key and the consumer code to send
struct FnMediaKey {
  int row;
  int col;
  uint16_t consumerCode;
};
const FnMediaKey fnMediaKeys[] = {
  {7, 4, HID_USAGE_CONSUMER_MUTE},              // Fn + F7 (row GP14, column GP5)
  {7, 6, HID_USAGE_CONSUMER_VOLUME_DECREMENT},  // Fn + F8 (row GP14, column GP27)
  {5, 6, HID_USAGE_CONSUMER_VOLUME_INCREMENT}   // Fn + F9 (row GP12, column GP27)
};
const int NUM_FN_MEDIA_KEYS = sizeof(fnMediaKeys) / sizeof(fnMediaKeys[0]);

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
  usb_hid.setStringDescriptor("Dell 1545 Keyboard");
  usb_hid.begin();

  // If the PC already enumerated the Pico without the keyboard, reconnect so it sees it
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT); // Make the row gpio's float by making them inputs w/o pullups to start.
  }
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); // Make the column gpio's inputs with pullups
  }

// Wait until the Pico is plugged in and recognized by the PC
  while ( !TinyUSBDevice.mounted() ) {
    delay(1);
  }
  delay(2000); // Short safety buffer for the host OS to finish enumeration
}

// ---------------------------------------------------------------------
// Loop:
//   1) Scans the full 17x8 matrix into keyState[][].
//   2) Checks the Fn key. With Fn held, F7/F8/F9 send mute / volume down /
//      volume up as a consumer (media) report instead of F7/F8/F9.
//   3) Builds the keyboard report: modifier bits and up to 6 normal keycodes.
//   4) Sends each report only when it differs from the one last sent.
// ---------------------------------------------------------------------
void loop() {
  uint8_t  modifierByte      = 0;
  uint8_t  keycodeReport[6]  = {0, 0, 0, 0, 0, 0};
  int      keyCount          = 0;
  bool     rolloverExceeded  = false;
  uint16_t consumerCode      = 0;

  // 1) Full-matrix scan --------------------------------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);
    delayMicroseconds(20);                            // settle

    for (int c = 0; c < NUM_COLS; c++) {
      keyState[r][c] = (digitalRead(colPins[c]) == LOW);
    }

    pinMode(rowPins[r], INPUT);                       // Let row pin float again
  }

  // 2) Fn layer ------------------------------------------------------------
  bool fnHeld = keyState[FN_ROW][FN_COL];
  bool usedByFn[NUM_ROWS][NUM_COLS] = {{false}};
  if (fnHeld) {
    for (int i = 0; i < NUM_FN_MEDIA_KEYS; i++) {
      const FnMediaKey &m = fnMediaKeys[i];
      if (keyState[m.row][m.col]) {
        usedByFn[m.row][m.col] = true;                // don't also send F7/F8/F9
        if (consumerCode == 0) consumerCode = m.consumerCode;
      }
    }
  }

  // 3) Build the keyboard report from all held keys ------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    for (int c = 0; c < NUM_COLS; c++) {
      if (!keyState[r][c] || usedByFn[r][c]) continue;
      uint8_t key = keyMap[r][c];
      if (key == 0) continue;                         // empty position (includes Fn)
      if (key >= HID_KEY_CONTROL_LEFT && key <= HID_KEY_GUI_RIGHT) {
        // Modifier: fold into the modifier byte
        modifierByte |= (uint8_t)(1u << (key - HID_KEY_CONTROL_LEFT));
      } else {
        // Normal key: append if we still have room (6KRO)
        if (keyCount < 6) {
          keycodeReport[keyCount++] = key;
        } else {
          rolloverExceeded = true;
        }
      }
    }
  }

  // Phantom / rollover handling: HID spec says fill all 6 slots with
  // ErrorRollOver (0x01) when more than 6 normal keys are held.
  if (rolloverExceeded) {
    for (int i = 0; i < 6; i++) keycodeReport[i] = 0x01;
  }

  // 4) Send reports that changed ---------------------------------------------
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

  delay(8); // ~125 Hz scan rate — comfortably above debounce noise
}
