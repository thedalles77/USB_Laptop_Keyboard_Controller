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
*/

// Pico 26 Pin Matrix Decoder (Arduino, Adafruit TinyUSB)
//
// Decodes the key matrix of a laptop keyboard connected to the Raspberry Pi
// Pico 26 pin keyboard controller board (no port expander). Open
// Keyboard_pin_list_TinyUSB_Blank.txt in a text editor on the PC and put the cursor at the end of the first key
// name line. Each time a key is pressed, the two connected I/O pins are typed
// into the editor followed by a down arrow, for example:
//     Pico_GP8 + Pico_GP19
//
// Power-up check: before any keys are pressed, all 26 FPC pins are tested.
// Pins that are shorted together (grounds, LED pins, etc.) are removed from
// the scan list. Do NOT press any keys until the Pico LED turns on.
//
// Arduino IDE settings: Board = Raspberry Pi Pico (or Pico W),
//                       Tools > USB Stack = Adafruit TinyUSB
// The Serial Monitor shows which pins were removed (optional).

#include <Adafruit_TinyUSB.h>

// Define the HID report descriptor for a standard Boot Keyboard
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// Create the TinyUSB HID object instance
Adafruit_USBD_HID usb_hid;

// TinyUSB table that converts an ASCII character to {shift, keycode}
uint8_t const conv_table[128][2] = { HID_ASCII_TO_KEYCODE };

// All 26 FPC pins: Pico GPIO 0-22 and 26-28 (23-25 are internal to the Pico)
const uint8_t allPins[] = {
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
  13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26, 27, 28
};
const int NUM_PINS = sizeof(allPins);

bool activePin[NUM_PINS];          // false = removed from the scan list

// Timing
const int SETTLE_US       = 50;    // time for a driven-low pin to pull the other pin low
const int DEBOUNCE_MS     = 10;    // key must still be pressed after this delay
const int RELEASE_MS      = 50;    // all keys must be released this long
const int CHAR_DELAY_MS   = 2;     // pause between typed characters

// ---------------------------------------------------------------------
// Pin helpers
// ---------------------------------------------------------------------
// Drive one pin low. Every other pin stays an input with a pull-up.
void drivePinLow(uint8_t pin) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

void releasePin(uint8_t pin) {
  pinMode(pin, INPUT_PULLUP);
}

// Read every active pin except "skip" (-1 = none). Returns a bit mask of the
// scan list positions that are low.
uint64_t readLowPins(int skip) {
  uint64_t lowMask = 0;
  for (int i = 0; i < NUM_PINS; i++) {
    if (i == skip || !activePin[i]) continue;
    if (digitalRead(allPins[i]) == LOW) lowMask |= ((uint64_t)1 << i);
  }
  return lowMask;
}

// Drive scan list position "i" low and return the mask of other pins that follow it
uint64_t testPin(int i) {
  drivePinLow(allPins[i]);
  delayMicroseconds(SETTLE_US);
  uint64_t lowMask = readLowPins(i);
  releasePin(allPins[i]);
  return lowMask;
}

int firstBit(uint64_t mask) {
  for (int i = 0; i < NUM_PINS; i++) {
    if (mask & ((uint64_t)1 << i)) return i;
  }
  return -1;
}

// Pin label in the pin list format: Pico_GP8
String pinLabel(uint8_t pin) {
  return "Pico_GP" + String(pin);
}

// Returns true if any key is still connecting two active pins
bool anyKeyPressed() {
  for (int i = 0; i < NUM_PINS; i++) {
    if (activePin[i] && testPin(i) != 0) return true;
  }
  return false;
}

// ---------------------------------------------------------------------
// USB typing
// ---------------------------------------------------------------------
// Returns false if the PC never accepted the report
bool sendReport(uint8_t modifier, uint8_t keycode) {
  uint8_t keys[6] = {keycode, 0, 0, 0, 0, 0};
  unsigned long start = millis();
  while (!usb_hid.ready()) {
    if (millis() - start > 100) return false;  // PC not responding
    delay(1);
  }
  return usb_hid.keyboardReport(0, modifier, keys);
}

bool tapKey(uint8_t modifier, uint8_t keycode) {
  bool ok = sendReport(modifier, keycode);  // press
  delay(CHAR_DELAY_MS);
  ok = sendReport(0, 0) && ok;              // release
  delay(CHAR_DELAY_MS);
  return ok;
}

bool typeText(const String &text) {
  for (unsigned int n = 0; n < text.length(); n++) {
    uint8_t ch = (uint8_t)text[n];
    if (ch >= 128) continue;
    uint8_t modifier = conv_table[ch][0] ? KEYBOARD_MODIFIER_LEFTSHIFT : 0;
    if (!tapKey(modifier, conv_table[ch][1])) {
      Serial.println("USB keyboard not responding - text not sent");
      return false;
    }
  }
  return true;
}

// ---------------------------------------------------------------------
// Power-up check: remove pins that are shorted without a key pressed
// ---------------------------------------------------------------------
void removeShortedPins() {
  for (int i = 0; i < NUM_PINS; i++) activePin[i] = true;

  // 1) Pins that read low with nothing driven (tied to ground somewhere)
  uint64_t stuckLow = readLowPins(-1);
  for (int i = 0; i < NUM_PINS; i++) {
    if (stuckLow & ((uint64_t)1 << i)) {
      activePin[i] = false;
      Serial.println("Removed " + pinLabel(allPins[i]) + " (low with nothing driven)");
    }
  }

  // 2) Pins that are shorted to each other. Repeat until a full pass is clean.
  bool foundShort = true;
  while (foundShort) {
    foundShort = false;
    for (int i = 0; i < NUM_PINS; i++) {
      if (!activePin[i]) continue;
      uint64_t lowMask = testPin(i);
      if (lowMask == 0) continue;
      foundShort = true;
      activePin[i] = false;
      String msg = "Removed " + pinLabel(allPins[i]);
      for (int j = 0; j < NUM_PINS; j++) {
        if (lowMask & ((uint64_t)1 << j)) {
          activePin[j] = false;
          msg += " + " + pinLabel(allPins[j]);
        }
      }
      Serial.println(msg + " (shorted together)");
    }
  }

  int count = 0;
  for (int i = 0; i < NUM_PINS; i++) if (activePin[i]) count++;
  Serial.println(String(count) + " pins in the scan list. Ready - press keys one at a time.");
}

// ---------------------------------------------------------------------
void setup() {
  // Start the USB keyboard FIRST. The Pico core starts USB before setup()
  // runs, so the HID keyboard must be added as early as possible.
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("Pico Matrix Decoder");
  usb_hid.begin();

  // If the PC already enumerated the Pico (as a Serial port only), disconnect
  // and reconnect so the PC sees the keyboard too.
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);   // LED off until the pin check is done

  // All FPC pins: inputs with pull-ups
  for (int i = 0; i < NUM_PINS; i++) {
    pinMode(allPins[i], INPUT_PULLUP);
  }

  // Wait for the PC to finish connecting the keyboard
  while (!TinyUSBDevice.mounted()) delay(1);
  delay(2000);

  removeShortedPins();
  digitalWrite(LED_BUILTIN, HIGH);  // ready for key presses
}

void loop() {
  for (int i = 0; i < NUM_PINS; i++) {
    if (!activePin[i]) continue;

    int j = firstBit(testPin(i));
    if (j < 0) continue;

    // Debounce: the same two pins must still be connected
    delay(DEBOUNCE_MS);
    if ((testPin(i) & ((uint64_t)1 << j)) == 0) continue;

    // Type the two pins, then move the editor cursor down one line
    if (typeText(pinLabel(allPins[i]) + " + " + pinLabel(allPins[j]))) {
      tapKey(0, HID_KEY_ARROW_DOWN);
    }
    Serial.println(pinLabel(allPins[i]) + " + " + pinLabel(allPins[j]));

    // Wait until the key is released (and stays released) before scanning again
    unsigned long releasedAt = millis();
    while (millis() - releasedAt < RELEASE_MS) {
      if (anyKeyPressed()) releasedAt = millis();
      delay(1);
    }
    return;  // start the next scan from the first pin
  }
}
