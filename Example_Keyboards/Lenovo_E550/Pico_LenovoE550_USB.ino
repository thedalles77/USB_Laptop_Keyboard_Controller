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

This code makes a Lenovo E550 laptop keyboard work as a standalone USB keyboard.
It uses a Raspberry Pi Pico with an MCP23018 port expander to interface with the 
keyboards FPC cable. The i2c bus is implemented using the Pico's built in hardware.
Holding the Fn key turns F1, F2, F3 into Mute, Volume Down, Volume Up.
The circuit board description is in step 2 of the Instructable titled 
"How to Make a USB & BLE Laptop Keyboard Controller" at this url:
https://www.instructables.com/How-to-Make-a-USB-Laptop-Keyboard-Controller
The code, circuit board files, and PDF description are at the GitHub site:
https://github.com/thedalles77/USB_Laptop_Keyboard_Controller/tree/master/Example_Keyboards/
*/

#include <Adafruit_TinyUSB.h>
# include <Wire.h>
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

// Low-Level I2C Configuration
#define I2C_SDA 2
#define I2C_SCL 3
#define MCP_ADDR 0x20

// Row and Column count
const int NUM_ROWS = 19;
const int NUM_COLS = 9;

// Pico GPIO: 0, 1, 4–22, 26–28. GPIO 2/3 are I2C, 23–25 are internal.
// Port expander: GPA0–GPA7 => 100–107, GPB0–GPB7 => 108–115.
const int rowPins[NUM_ROWS] = {10, 11, 12, 15, 17, 18, 19, 20, 21, 102, 103, 104, 105, 107, 108, 110, 111, 112, 113};
const int colPins[NUM_COLS] = {6, 7, 8, 9, 14, 100, 101, 114, 115};

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
    //  GP6                      GP7                      GP8                      GP9                      GP14                     GPA0                     GPA1                     GPB6                     GPB7
        {HID_KEY_B,               HID_KEY_4,               HID_KEY_R,               HID_KEY_G,               0,                       HID_KEY_F,               HID_KEY_5,               HID_KEY_V,               HID_KEY_T},               // GP10
        {0,                       0,                       0,                       0,                       0,                       0,                       HID_KEY_CONTROL_LEFT,    HID_KEY_CONTROL_RIGHT,   0},                       // GP11
        {HID_KEY_ARROW_RIGHT,     HID_KEY_F12,             0,                       0,                       0,                       0,                       0,                       0,                       HID_KEY_GUI_LEFT},        // GP12
        {HID_KEY_KEYPAD_0,        HID_KEY_KEYPAD_3,        HID_KEY_KEYPAD_6,        HID_KEY_KEYPAD_4,        0,                       HID_KEY_KEYPAD_2,        HID_KEY_KEYPAD_1,        HID_KEY_KEYPAD_ENTER,    HID_KEY_KEYPAD_5},        // GP15
        {HID_KEY_PAGE_DOWN,       HID_KEY_INSERT,          0,                       0,                       0,                       HID_KEY_PRINT_SCREEN,    HID_KEY_DELETE,          HID_KEY_PAGE_UP,         0},                       // GP17
        {HID_KEY_ARROW_DOWN,      HID_KEY_F11,             0,                       0,                       0,                       0,                       HID_KEY_HOME,            0,                       0},                       // GP18
        {HID_KEY_SLASH,           HID_KEY_0,               HID_KEY_P,               HID_KEY_APOSTROPHE,      0,                       HID_KEY_SEMICOLON,       HID_KEY_MINUS,           0,                       HID_KEY_BRACKET_LEFT},    // GP19
        {HID_KEY_N,               HID_KEY_7,               HID_KEY_U,               HID_KEY_H,               0,                       HID_KEY_J,               HID_KEY_6,               HID_KEY_M,               HID_KEY_Y},               // GP20
        {0,                       0,                       0,                       0,                       0,                       0,                       0,                       HID_KEY_SHIFT_RIGHT,     HID_KEY_SHIFT_LEFT},      // GP21
        {0,                       HID_KEY_2,               HID_KEY_W,               0,                       0,                       HID_KEY_S,               HID_KEY_F1,              HID_KEY_X,               HID_KEY_CAPS_LOCK},       // GPA2
        {0,                       HID_KEY_9,               HID_KEY_O,               0,                       0,                       HID_KEY_L,               HID_KEY_F8,              HID_KEY_PERIOD,          HID_KEY_F7},              // GPA3
        {HID_KEY_ALT_RIGHT,       0,                       0,                       HID_KEY_ALT_LEFT,        0,                       0,                       0,                       0,                       0},                       // GPA4
        {HID_KEY_SPACE,           HID_KEY_F10,             0,                       HID_KEY_F5,              0,                       HID_KEY_BACKSLASH,       HID_KEY_F9,              HID_KEY_ENTER,           HID_KEY_BACKSPACE},       // GPA5
        {HID_KEY_KEYPAD_ADD,      HID_KEY_KEYPAD_8,        HID_KEY_KEYPAD_MULTIPLY, HID_KEY_NUM_LOCK,        0,                       HID_KEY_KEYPAD_7,        HID_KEY_KEYPAD_SUBTRACT, HID_KEY_KEYPAD_9,        HID_KEY_KEYPAD_DIVIDE},   // GPA7
        {0,                       0,                       0,                       0,                       0,                       0,                       0,                       0,                       0},                       // GPB0
        {HID_KEY_ARROW_LEFT,      HID_KEY_END,             0,                       HID_KEY_ARROW_UP,        0,                       0,                       0,                       HID_KEY_KEYPAD_DECIMAL,  0},                       // GPB2
        {0,                       HID_KEY_8,               HID_KEY_I,               HID_KEY_F6,              0,                       HID_KEY_K,               HID_KEY_EQUAL,           HID_KEY_COMMA,           HID_KEY_BRACKET_RIGHT},   // GPB3
        {0,                       HID_KEY_3,               HID_KEY_E,               HID_KEY_F4,              0,                       HID_KEY_D,               HID_KEY_F2,              HID_KEY_C,               HID_KEY_F3},              // GPB4
        {0,                       HID_KEY_1,               HID_KEY_Q,               HID_KEY_ESCAPE,          0,                       HID_KEY_A,               HID_KEY_GRAVE,           HID_KEY_Z,               HID_KEY_TAB}              // GPB5
};

// Fn key location in the matrix (row GPB0, column GP14). It has no keycode in keyMap.
const int FN_ROW = 14;   // GPB0
const int FN_COL = 4;    // GP14

// Media keys sent when Fn is held: matrix position of the F key and the consumer code to send
struct FnMediaKey {
  int row;
  int col;
  uint16_t consumerCode;
};
const FnMediaKey fnMediaKeys[] = {
  { 9, 6, HID_USAGE_CONSUMER_MUTE},              // Fn + F1 (row GPA2, column GPA1)
  {17, 6, HID_USAGE_CONSUMER_VOLUME_DECREMENT},  // Fn + F2 (row GPB4, column GPA1)
  {17, 8, HID_USAGE_CONSUMER_VOLUME_INCREMENT}   // Fn + F3 (row GPB4, column GPB7)
};
const int NUM_FN_MEDIA_KEYS = sizeof(fnMediaKeys) / sizeof(fnMediaKeys[0]);

// Current state of every key from the latest scan (true = pressed)
bool keyState[NUM_ROWS][NUM_COLS] = {false};

// Remember the last reports we transmitted so we can suppress duplicates
uint8_t lastModifier = 0;
uint8_t lastReport[6] = {0, 0, 0, 0, 0, 0};
uint16_t lastConsumer = 0;

// i2c write function
bool bb_write_mcp_16bit(uint8_t reg, uint16_t value) {
  Wire1.beginTransmission(MCP_ADDR);
  Wire1.write(reg);
  Wire1.write(value & 0xFF);         // Lower byte (GPA)
  Wire1.write((value >> 8) & 0xFF);  // Upper byte (GPB)
  return (Wire1.endTransmission() == 0);
}

// i2c read function
uint16_t bb_read_mcp_pins() {
  Wire1.beginTransmission(MCP_ADDR);
  Wire1.write(0x12); // GPIOA register address
  if (Wire1.endTransmission() != 0) return 0xFFFF;

  // Request 2 bytes sequentially (GPIOA then GPIOB)
  if (Wire1.requestFrom(MCP_ADDR, 2) != 2) return 0xFFFF;

  uint8_t gpioa = Wire1.read();
  uint8_t gpiob = Wire1.read();
  
  return ((uint16_t)gpiob << 8) | gpioa;
}

// Helpers -------------------------------------------------------------

// Drive a single row LOW (leaving all other rows floating with pullups)
static inline void driveRowLow(int r) {
  int pin = rowPins[r];
  if (pin <= 30) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  } else {
    uint8_t pinNum = pin - 100;                       // 0..15
    uint16_t mask  = (uint16_t)~(1u << pinNum);       // 0 = output/low on selected pin
    bb_write_mcp_16bit(0x00, mask);                   // IODIR: only this pin is output
    bb_write_mcp_16bit(0x14, mask);                   // OLAT : drive it LOW
  }
}

// Release a row back to a floating input with pull-up
static inline void releaseRow(int r) {
  int pin = rowPins[r];
  if (pin <= 30) {
    pinMode(pin, INPUT_PULLUP);
  } else {
    bb_write_mcp_16bit(0x00, 0xFFFF);                 // All expander pins = input
    bb_write_mcp_16bit(0x0C, 0xFFFF);                 // All expander pull-ups enabled
  }
}

// Read one column bit (true == pressed / low)
static inline bool readCol(int c, uint16_t mcpSnapshot) {
  int pin = colPins[c];
  if (pin <= 30) {
    return (digitalRead(pin) == LOW);
  } else {
    uint8_t pinNum = pin - 100;
    return ((mcpSnapshot & (1u << pinNum)) == 0);
  }
}

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
  usb_hid.setStringDescriptor("Lenovo E550 Keyboard");
  usb_hid.begin();

  // If the PC already enumerated the Pico without the keyboard, reconnect so it sees it
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // Initialize hardware I2C1 on pins 2 and 3
  Wire1.setSDA(I2C_SDA);
  Wire1.setSCL(I2C_SCL);
  Wire1.begin();
  Wire1.setClock(400000); // Set i2c bus to 400kHz mode

  for (int i = 0; i < NUM_ROWS; i++)
    if (rowPins[i] < 30) pinMode(rowPins[i], INPUT_PULLUP);
  for (int i = 0; i < NUM_COLS; i++)
    if (colPins[i] < 30) pinMode(colPins[i], INPUT_PULLUP);

  bb_write_mcp_16bit(0x00, 0xFFFF); // IODIR : all expander pins = input
  bb_write_mcp_16bit(0x0C, 0xFFFF); // GPPU  : all expander pull-ups on

  while (!TinyUSBDevice.mounted()) delay(1);
  delay(2000);
}

// ---------------------------------------------------------------------
// Loop:
//   1) Scans the full 19x9 matrix into keyState[][].
//   2) Checks the Fn key. With Fn held, F1/F2/F3 send mute / volume down /
//      volume up as a consumer (media) report instead of F1/F2/F3.
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
    driveRowLow(r);
    delayMicroseconds(20);                            // settle

    // Read all 16 expander pins once for this row
    uint16_t mcpSnapshot = bb_read_mcp_pins();

    for (int c = 0; c < NUM_COLS; c++) {
      keyState[r][c] = readCol(c, mcpSnapshot);
    }

    releaseRow(r);
  }

  // 2) Fn layer ------------------------------------------------------------
  bool fnHeld = keyState[FN_ROW][FN_COL];
  bool usedByFn[NUM_ROWS][NUM_COLS] = {{false}};
  if (fnHeld) {
    for (int i = 0; i < NUM_FN_MEDIA_KEYS; i++) {
      const FnMediaKey &m = fnMediaKeys[i];
      if (keyState[m.row][m.col]) {
        usedByFn[m.row][m.col] = true;                // don't also send F1/F2/F3
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
