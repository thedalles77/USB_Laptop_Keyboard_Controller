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

This code makes a MacBook Pro A1286 laptop keyboard work as a standalone USB keyboard.
It uses a Raspberry Pi Pico with an MCP23018 port expander to interface with the 
keyboards FPC cable. The i2c bus is implemented using the Pico's built in hardware.
The circuit board description is in step 2 of the Instructable titled 
"How to Make a USB & BLE Laptop Keyboard Controller" at this url:
https://www.instructables.com/How-to-Make-a-USB-Laptop-Keyboard-Controller
The code, circuit board files, and PDF description are at the GitHub site:
https://github.com/thedalles77/USB_Laptop_Keyboard_Controller/tree/master/Example_Keyboards/Pico_40Pin_Keyboard
*/

#include <Adafruit_TinyUSB.h>
# include <Wire.h>
// Define the HID report descriptor for a standard Boot Keyboard
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// Create the TinyUSB HID object instance
Adafruit_USBD_HID usb_hid;

// Low-Level I2C Configuration
#define I2C_SDA 2
#define I2C_SCL 3
#define MCP_ADDR 0x20

// Row and Column count
const int NUM_ROWS = 16;
const int NUM_COLS = 9;

// Pico GPIO: 0, 1, 4–22, 26–28. GPIO 2/3 are I2C, 23–25 are internal.
// Port expander: GPA0–GPA7 => 100–107, GPB0–GPB7 => 108–115.
const int rowPins[NUM_ROWS] = {101, 20, 112,  9, 102, 10, 103, 19,
                               111, 11, 110, 18, 105, 14, 107, 15};
const int colPins[NUM_COLS] = {100,  7, 113,  8,  17, 109,  16, 106, 108};

// Base Keyboard Map. A zero indicates no key at that position.
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { HID_KEY_Q, HID_KEY_1, HID_KEY_A, HID_KEY_Z,        HID_KEY_F11,        HID_KEY_BACKSPACE, HID_KEY_F12,          HID_KEY_F1,          0 },
  { HID_KEY_W, HID_KEY_2, HID_KEY_S, HID_KEY_X,        HID_KEY_ESCAPE,     HID_KEY_TAB,       HID_KEY_GRAVE,        HID_KEY_F2,          0 },
  { HID_KEY_E, HID_KEY_3, HID_KEY_D, HID_KEY_C,        0,                  0,                 0,                    HID_KEY_F3,          0 },
  { HID_KEY_Y, HID_KEY_6, HID_KEY_H, HID_KEY_N,        HID_KEY_SLASH,      HID_KEY_SPACE,     HID_KEY_EQUAL,        HID_KEY_F6,          0 },
  { HID_KEY_R, HID_KEY_4, HID_KEY_F, HID_KEY_V,        0,                  0,                 HID_KEY_BRACKET_RIGHT,HID_KEY_F4,          0 },
  { 0,         0,         0,         0,                HID_KEY_ENTER,      0,                 0,                    HID_KEY_SHIFT_RIGHT, 0 },
  { 0,         0,         0,         HID_KEY_GUI_LEFT, HID_KEY_0,          0,                 HID_KEY_GUI_RIGHT,    0,                   0 },
  { HID_KEY_T, HID_KEY_5, HID_KEY_G, HID_KEY_B,        HID_KEY_MINUS,      0,                 HID_KEY_BRACKET_LEFT, HID_KEY_F5,          0 },
  { 0,         0,         0,         0,                HID_KEY_F10,        HID_KEY_ALT_RIGHT, 0,                    0,                   0 },
  { 0,         0,         0,         0,                HID_KEY_P,          HID_KEY_CAPS_LOCK, 0,                    0,                   0 },
  { HID_KEY_U, HID_KEY_7, HID_KEY_J, HID_KEY_M,        HID_KEY_SEMICOLON,  HID_KEY_APOSTROPHE,0,                    HID_KEY_F7,          0 },
  { HID_KEY_I, HID_KEY_8, HID_KEY_K, HID_KEY_COMMA,    0,                  HID_KEY_ARROW_LEFT,HID_KEY_ARROW_RIGHT,  HID_KEY_F8,          0 },
  { HID_KEY_O, HID_KEY_9, HID_KEY_L, HID_KEY_PERIOD,   HID_KEY_BACKSLASH,  HID_KEY_ARROW_DOWN,HID_KEY_ARROW_UP,     HID_KEY_F9,          0 },
  { 0,         0,         0,         0,                0,                  0,                 0,                    0,                   HID_KEY_SHIFT_LEFT },
  { 0,         0,         0,         0,                0,                  0,                 0,                    0,                   HID_KEY_ALT_LEFT   },
  { 0,         0,         0,         0,                0,                  0,                 0,                    0,                   HID_KEY_CONTROL_LEFT}
};

// Track key states across scans so we can detect edges
bool lastKeyState[NUM_ROWS][NUM_COLS] = {false};

// Remember the last report we transmitted so we can suppress duplicates
uint8_t lastModifier = 0;
uint8_t lastReport[6] = {0, 0, 0, 0, 0, 0};

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

void setup() {
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

  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  while (!TinyUSBDevice.mounted()) delay(1);
  delay(2000);
}

// ---------------------------------------------------------------------
// Optimized loop:
//   - Scans the full 16x9 matrix once per iteration.
//   - Tracks per-key edges to know if anything changed.
//   - Accumulates modifier bits and up to 6 normal keycodes.
//   - Sends exactly ONE HID report AFTER the complete scan, and only
//     if the aggregated report differs from the previously sent one.
// ---------------------------------------------------------------------
void loop() {
  uint8_t modifierByte      = 0;
  uint8_t keycodeReport[6]  = {0, 0, 0, 0, 0, 0};
  int     keyCount          = 0;
  bool    stateChanged      = false;
  bool    rolloverExceeded  = false;

  // 1) Full-matrix scan --------------------------------------------------
  for (int r = 0; r < NUM_ROWS; r++) {
    driveRowLow(r);
    delayMicroseconds(20);                            // settle

    // Read all 16 expander pins once for this row
    uint16_t mcpSnapshot = bb_read_mcp_pins();

    for (int c = 0; c < NUM_COLS; c++) {
      bool pressed = readCol(c, mcpSnapshot);

      // Edge detection: mark if this key transitioned
      if (pressed != lastKeyState[r][c]) {
        stateChanged        = true;
        lastKeyState[r][c]  = pressed;
      }

      // Accumulate currently-held keys into a single report
      if (pressed) {
        uint8_t key = keyMap[r][c];
        if (key != 0) {
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
    }

    releaseRow(r);
  }

  // Phantom / rollover handling: HID spec says fill all 6 slots with
  // ErrorRollOver (0x01) when more than 6 normal keys are held.
  if (rolloverExceeded) {
    for (int i = 0; i < 6; i++) keycodeReport[i] = 0x01;
  }

  // 2) One transmission per full scan -----------------------------------
  // Only send if something actually changed AND the aggregated report
  // differs from the last one we transmitted (prevents duplicate spam).
  if (stateChanged && usb_hid.ready()) {
    bool reportDiffers = (modifierByte != lastModifier);
    if (!reportDiffers) {
      for (int i = 0; i < 6; i++) {
        if (keycodeReport[i] != lastReport[i]) { reportDiffers = true; break; }
      }
    }
    if (reportDiffers) {
      usb_hid.keyboardReport(0, modifierByte, keycodeReport);
      lastModifier = modifierByte;
      for (int i = 0; i < 6; i++) lastReport[i] = keycodeReport[i];
    }
  }

  delay(8); // ~125 Hz scan rate — comfortably above debounce noise
}
