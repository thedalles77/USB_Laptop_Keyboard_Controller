#include <Adafruit_TinyUSB.h>

// =========================================================================
// 1. HID REPORT DESCRIPTOR SETUP (Fixes: desc_hid_report declared error)
// =========================================================================
uint8_t const desc_hid_report[] = { 
  TUD_HID_REPORT_DESC_KEYBOARD() 
};

// Define the physical keyboard matrix parameters
const int NUM_ROWS = 17; 
const int NUM_COLS = 8; 

const int colPins[NUM_COLS] = { 1, 2, 3, 4, 5, 6, 27, 28 };
const int rowPins[NUM_ROWS] = { 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26 };

// Map standard HID keyboard codes
const uint8_t keyMap[NUM_ROWS][NUM_COLS] = {
  { HID_KEY_ARROW_UP, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_RIGHT, HID_KEY_ENTER, HID_KEY_ESCAPE, HID_KEY_BACKSPACE, HID_KEY_TAB },
  // ... Paste your remaining 16 matrix rows exactly as they are in your repository ...
};

bool lastKeyState[NUM_ROWS][NUM_COLS] = {false};

// Bind the Adafruit USB HID instance
Adafruit_USBD_HID usb_hid;

void setup() {
  // DELAY BUFFER: Gives the hardware plenty of time to boot safely before setting up I/O pins
  delay(1000); 

  // Set up the TinyUSB HID Keyboard configuration profiles
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();

  // Initialize your keyboard rows/columns AFTER the USB engine is up
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], INPUT); 
  }
  for (int c = 0; c < NUM_COLS; c++) {
    pinMode(colPins[c], INPUT_PULLUP); 
  }

  // Force system execution to pause until the PC mounts the driver profile
  while (!TinyUSBDevice.mounted()) {
    delay(10);
  }
}

void loop() {
  for (int r = 0; r < NUM_ROWS; r++) {
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW); 

    for (int c = 0; c < NUM_COLS; c++) {
      bool currentPressed = (digitalRead(colPins[c]) == LOW);

      if (currentPressed != lastKeyState[r][c]) {
        lastKeyState[r][c] = currentPressed;
        uint8_t standardKey = keyMap[r][c];
        if (standardKey != 0) {
          if (currentPressed) {
            // keyboardPress takes TWO arguments: modifier keys (0 = none), then the keycode
            usb_hid.keyboardPress(0, standardKey); 
          } else {
            // keyboardRelease takes ONLY ONE argument: the keycode to release
            usb_hid.keyboardRelease(standardKey);
          }
        }
      }
    }
    pinMode(rowPins[r], INPUT); 
  }
  delay(8);   
}
