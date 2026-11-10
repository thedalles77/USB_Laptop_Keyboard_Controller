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

This code makes a Lenovo T61 trackpoint work as a USB mouse using a Raspberry Pi
Pico (or Pico W, Pico 2, Pico 2 W) and the Adafruit TinyUSB library. The trackpoint is a
PS/2 device on GP0 (data) and GP1 (clock), read with a clock interrupt.
Arduino IDE settings: Tools > USB Stack = Adafruit TinyUSB
*/

#include <Adafruit_TinyUSB.h>

// HID report ID for the mouse report
enum {
  RID_MOUSE = 1
};

// HID report descriptor: mouse with 5 buttons, X, Y, wheel and horizontal pan
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE))
};

// Create the TinyUSB HID object instance
Adafruit_USBD_HID usb_hid;

// Hardware Pin Layout Configurations
const int PS2_DATA_PIN  = 0;
const int PS2_CLOCK_PIN = 1;

// Each PS/2 byte is 11 bits: start(0), 8 data bits (LSB first), odd parity, stop(1).
// A trackpoint movement packet is 3 bytes: status, X movement, Y movement.
// The interrupt routine below decodes complete packets and ADDS the movement to
// tpAccumX / tpAccumY. The loop sends the accumulated movement whenever USB is
// ready, so no movement is lost and the interrupt can never overrun a buffer.
volatile uint16_t bitBuffer = 0;        // bits of the byte being received
volatile int bitCount = 0;              // number of bits received for this byte
volatile unsigned long lastBitTime = 0; // micros() of the last clock edge
volatile unsigned long lastByteTime = 0;// micros() of the last complete byte

volatile uint8_t tpPacket[3];           // packet being assembled
volatile int tpIndex = 0;               // next byte position in tpPacket

volatile int32_t tpAccumX = 0;          // movement not yet sent over USB
volatile int32_t tpAccumY = 0;
volatile uint8_t tpButtons = 0;         // latest button bits: 0x01 left, 0x02 right, 0x04 middle
volatile uint8_t tpButtonsLatched = 0;  // buttons pressed since the loop last looked, so a quick
                                        // click is never missed between reports

uint8_t sentButtons = 0;                // button bits last sent over USB

const unsigned long PS2_PACKET_GAP_US = 4000;  // gap that means a new packet is starting

// Wait for the PS/2 clock to reach "level". Returns false after timeoutMs.
bool waitPS2Clock(int level, unsigned long start, unsigned long timeoutMs) {
  while (digitalRead(PS2_CLOCK_PIN) != level) {
    if (millis() - start > timeoutMs) return false;
  }
  return true;
}

// Standard PS/2 Open-Drain Writing Implementation (host to trackpoint).
// Every wait has a timeout so a missing or unresponsive trackpoint can't hang the keyboard.
// Returns true if the trackpoint clocked in the byte and sent its acknowledge bit.
bool writePS2Byte(uint8_t value) {
  const unsigned long TIMEOUT_MS = 30;
  unsigned long start = millis();
  bool ok = true;

  // Request to send: hold clock low at least 100 usec, then pull data low, then release clock
  pinMode(PS2_CLOCK_PIN, OUTPUT);
  digitalWrite(PS2_CLOCK_PIN, LOW);
  delayMicroseconds(200);

  pinMode(PS2_DATA_PIN, OUTPUT);
  digitalWrite(PS2_DATA_PIN, LOW);

  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);

  uint8_t parity = 1;

  // 8 data bits, LSB first. The trackpoint reads each bit on the rising clock edge.
  for (int i = 0; i < 8 && ok; i++) {
    ok = waitPS2Clock(LOW, start, TIMEOUT_MS);
    uint8_t bit = (value >> i) & 0x01;
    if (bit == 0) {
      pinMode(PS2_DATA_PIN, OUTPUT);
      digitalWrite(PS2_DATA_PIN, LOW);
    } else {
      pinMode(PS2_DATA_PIN, INPUT_PULLUP);
    }
    parity ^= bit;
    ok = ok && waitPS2Clock(HIGH, start, TIMEOUT_MS);
  }

  // Parity bit
  ok = ok && waitPS2Clock(LOW, start, TIMEOUT_MS);
  if (parity == 0) {
    pinMode(PS2_DATA_PIN, OUTPUT);
    digitalWrite(PS2_DATA_PIN, LOW);
  } else {
    pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  }
  ok = ok && waitPS2Clock(HIGH, start, TIMEOUT_MS);

  // Stop bit: release data
  ok = ok && waitPS2Clock(LOW, start, TIMEOUT_MS);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  ok = ok && waitPS2Clock(HIGH, start, TIMEOUT_MS);

  // Acknowledge: the trackpoint pulls data low for one clock, then releases both lines
  if (ok) {
    while (digitalRead(PS2_DATA_PIN) == HIGH && (millis() - start < TIMEOUT_MS));
    while (digitalRead(PS2_CLOCK_PIN) == HIGH && (millis() - start < TIMEOUT_MS));
    ok = (digitalRead(PS2_DATA_PIN) == LOW);
    while ((digitalRead(PS2_DATA_PIN) == LOW || digitalRead(PS2_CLOCK_PIN) == LOW) && (millis() - start < TIMEOUT_MS));
  }

  // Always leave both lines released
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  return ok;
}

// Trackpoint Hardware Interrupt Service Routine (ISR) - runs on each falling clock edge
void clkInterruptSR() {
  unsigned long now = micros();

  // A long gap means any partly received byte is stale (lost clock edge): start over
  if ((now - lastBitTime) > 3000) {
    bitCount = 0;
    bitBuffer = 0;
  }
  lastBitTime = now;

  bitBuffer |= ((uint16_t)digitalRead(PS2_DATA_PIN) << bitCount);
  bitCount++;
  if (bitCount < 11) return;

  // A complete 11-bit frame has arrived
  uint16_t frame = bitBuffer;
  bitCount = 0;
  bitBuffer = 0;

  // Check start bit = 0, stop bit = 1 and odd parity over the 8 data bits + parity bit.
  // A bad frame means a bit was lost or corrupted: drop the partial packet and resync.
  bool startBitOK = ((frame & 0x001) == 0);
  bool stopBitOK  = ((frame & 0x400) != 0);
  bool parityOK   = (__builtin_popcount((frame >> 1) & 0x1FF) & 1) == 1;
  if (!startBitOK || !stopBitOK || !parityOK) {
    tpIndex = 0;
    return;
  }
  uint8_t data = (frame >> 1) & 0xFF;

  // Bytes of one packet arrive back to back. A gap means a new packet is starting.
  if ((now - lastByteTime) > PS2_PACKET_GAP_US) tpIndex = 0;
  lastByteTime = now;

  // The first byte of every packet has bit 3 set. Skip bytes until one does (resync).
  if (tpIndex == 0 && (data & 0x08) == 0) return;

  tpPacket[tpIndex++] = data;
  if (tpIndex < 3) return;
  tpIndex = 0;

  // Complete packet: status, X, Y
  uint8_t status = tpPacket[0];
  tpButtonsLatched |= (status & 0x07) & ~tpButtons;  // remember new presses
  tpButtons = status & 0x07;
  if (status & 0xC0) return;  // X or Y overflow: movement is not valid, keep the buttons only

  int dx = tpPacket[1];
  int dy = tpPacket[2];
  if (status & 0x10) dx -= 256;  // 9-bit two's complement sign bits
  if (status & 0x20) dy -= 256;
  tpAccumX += dx;
  tpAccumY += dy;
}

// Reset the trackpoint and turn on streaming. The interrupt must not be attached.
bool initTrackpoint() {
  bool ok = writePS2Byte(0xFF); // Reset (replies FA, then AA 00 when its self test is done)
  delay(600);
  ok = writePS2Byte(0xF4) && ok; // Enable Streaming
  delay(20);
  return ok;
}

// If the trackpoint resets itself (power glitch, cable bump), it sends AA 00 and then
// stops streaming. With the packet logic above, those two bytes sit in tpPacket waiting
// for a third byte that never comes. Detect that and turn streaming back on.
void checkTrackpointReset() {
  noInterrupts();
  bool resetSeen = (tpIndex == 2 && tpPacket[0] == 0xAA && tpPacket[1] == 0x00 &&
                    (micros() - lastByteTime) > 50000);
  interrupts();
  if (!resetSeen) return;

  detachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN));
  writePS2Byte(0xF4);           // Enable Streaming
  noInterrupts();
  tpIndex = 0;
  bitCount = 0;
  bitBuffer = 0;
  interrupts();
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
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
  usb_hid.setStringDescriptor("T61 USB Trackpoint");
  usb_hid.begin();

  // If the PC already enumerated the Pico without the mouse, reconnect so it sees it
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // Set initial line constraints to pullup states
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);

  // Wait until the PC has finished connecting the mouse
  while (!TinyUSBDevice.mounted()) delay(1);

  // --- Wake Trackpoint --- (each PS/2 write has a timeout, so this can't hang)
  initTrackpoint();

  // Attach the Trackpoint clock interrupt
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
}

void loop() {
  // Take the movement and buttons collected by the interrupt
  noInterrupts();
  int32_t moveX = tpAccumX;
  int32_t moveY = tpAccumY;
  uint8_t latched = tpButtonsLatched;
  // Buttons held now, plus any pressed and already released since the last report.
  // A latched click is sent as a press now and a release in the next report.
  uint8_t buttons = (tpButtons | latched) & 0x03;   // left and right buttons
  interrupts();

  // A USB mouse report holds -127 to +127. Send that much now and keep the rest for next time.
  if (moveX > 127) moveX = 127;
  if (moveX < -127) moveX = -127;
  if (moveY > 127) moveY = 127;
  if (moveY < -127) moveY = -127;

  // Send one report with the buttons and the movement (PS/2 Y is up-positive, the PC is down-positive).
  // Only remove what was sent once USB has accepted the report, so nothing is lost.
  if ((buttons != sentButtons || moveX != 0 || moveY != 0) && waitUsbReady()) {
    if (usb_hid.mouseReport(RID_MOUSE, buttons, (int8_t)moveX, (int8_t)(-moveY), 0, 0)) {
      noInterrupts();
      tpAccumX -= moveX;
      tpAccumY -= moveY;
      tpButtonsLatched &= ~latched;
      interrupts();
      sentButtons = buttons;
    }
  }

  checkTrackpointReset();

  delay(2); // short pause between checks
}
