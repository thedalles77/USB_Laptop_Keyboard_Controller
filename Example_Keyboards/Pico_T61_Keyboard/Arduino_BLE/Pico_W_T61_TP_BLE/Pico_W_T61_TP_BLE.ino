/*
    Copyright 2026 Frank Adams

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://apache.org

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <MouseBLE.h>

// Hardware Pin Layout Configurations
const int PS2_DATA_PIN  = 0;
const int PS2_CLOCK_PIN = 1;

// Volatile tracking variables altered within the Interrupt context
volatile uint16_t bitBuffer = 0;
volatile int bitCount = 0;
volatile bool packetReady = false;

// 3-Byte Payload storage array
uint8_t ps2Packet[3];
int packetIndex = 0;
unsigned long lastBitTime = 0;

// Optimized Open-Drain Interfacing Protocol: Switch Mode rather than driving HIGH
void writePS2Byte(uint8_t value) {
  // Inhibit communications line by forcing clock low
  pinMode(PS2_CLOCK_PIN, OUTPUT);
  digitalWrite(PS2_CLOCK_PIN, LOW);
  delayMicroseconds(200); 

  // Pull Data Low to request Host-To-Device session
  pinMode(PS2_DATA_PIN, OUTPUT);
  digitalWrite(PS2_DATA_PIN, LOW);
  
  // Release clock to let the trackpoint handle line timings
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP); 

  uint8_t parity = 1; 
  
  // Cycle bits out sequentially on falling device edges
  for (int i = 0; i < 8; i++) {
    while (digitalRead(PS2_CLOCK_PIN) == HIGH); 
    uint8_t bit = (value >> i) & 0x01;
    
    if (bit == 0) {
      pinMode(PS2_DATA_PIN, OUTPUT);
      digitalWrite(PS2_DATA_PIN, LOW);
    } else {
      pinMode(PS2_DATA_PIN, INPUT_PULLUP); 
    }
    parity ^= bit;
    while (digitalRead(PS2_CLOCK_PIN) == LOW); 
  }

  // Handle Parity line constraint
  while (digitalRead(PS2_CLOCK_PIN) == HIGH);
  if (parity == 0) {
    pinMode(PS2_DATA_PIN, OUTPUT);
    digitalWrite(PS2_DATA_PIN, LOW);
  } else {
    pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  }
  while (digitalRead(PS2_CLOCK_PIN) == LOW);

  // Send Stop Bit configuration framing 
  while (digitalRead(PS2_CLOCK_PIN) == HIGH);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);
  while (digitalRead(PS2_CLOCK_PIN) == LOW);

  // Allow Line Release verification handshakes
  unsigned long timeout = millis();
  while (digitalRead(PS2_DATA_PIN) == HIGH && (millis() - timeout < 250)); 
  while (digitalRead(PS2_CLOCK_PIN) == HIGH && (millis() - timeout < 250)); 
  while ((digitalRead(PS2_DATA_PIN) == LOW || digitalRead(PS2_CLOCK_PIN) == LOW) && (millis() - timeout < 250)); 
}

// Highly optimized Hardware Interrupt Service Routine (ISR)
// Triggers instantly whenever the Trackpoint drops the Clock line LOW
void clkInterruptSR() {
  unsigned long now = micros();
  
  // If lines lay silent for more than 3ms, resynchronize framing boundaries
  if ((now - lastBitTime) > 3000) {
    bitCount = 0;
    bitBuffer = 0;
  }
  lastBitTime = now;

  // Sample data pin directly and pack it into the bit-shifting vector
  uint8_t bitVal = digitalRead(PS2_DATA_PIN);
  bitBuffer |= (bitVal << bitCount);
  bitCount++;

  // Flag full isolation once 11 bits are grouped (1 Start + 8 Data + 1 Parity + 1 Stop)
  if (bitCount >= 11) {
    uint8_t completedByte = (bitBuffer >> 1) & 0xFF; // Discard start framing bit
    
    ps2Packet[packetIndex] = completedByte;
    packetIndex++;
    
    if (packetIndex >= 3) {
      packetReady = true; // Signals main loop to route payload
    }
    
    bitCount = 0;
    bitBuffer = 0;
  }
}

void setup() {
  // Set initial line constraints to pullup states to protect translating IC configurations
  pinMode(PS2_CLOCK_PIN, INPUT_PULLUP);
  pinMode(PS2_DATA_PIN, INPUT_PULLUP);

  // Launch Bluetooth framework operations
  MouseBLE.begin("T61 BLE Trackpoint", "Lenovo Mod");
  delay(5000); 

  // Initialize and awake tracking engine registers inside device
  writePS2Byte(0xFF); // Reset Target command string
  delay(600);         // Extended margin window matching hardware boot cycles
  writePS2Byte(0xF4); // Enable standard data streaming
  delay(200);

  // Attach the optimized interrupt tracking structure to the physical Clock GPIO line
  attachInterrupt(digitalPinToInterrupt(PS2_CLOCK_PIN), clkInterruptSR, FALLING);
}

void loop() {
  // Process coordinate transfers safely inside non-interrupt execution thread contexts
  if (packetReady) {
    uint8_t statusByte = ps2Packet[0];
    int16_t movementX  = ps2Packet[1];
    int16_t movementY  = ps2Packet[2];

    // Enforce basic protocol sanity (Bit 3 of first byte must always report 1)
    if ((statusByte & 0x08) == 0) {
      // Structure frame was corrupt or out of line alignment; dump index counters to clear path
      noInterrupts();
      packetIndex = 0;
      packetReady = false;
      interrupts();
      return;
    }

    // Apply Sign Extensions for negative Two's Complement conversions
    if (statusByte & 0x10) movementX |= 0xFF00; 
    if (statusByte & 0x20) movementY |= 0xFF00; 

    // Read click state logic elements
    bool leftClick  = (statusByte & 0x01);
    bool rightClick = (statusByte & 0x02);

    // Update Left Click State machine vectors
    if (leftClick) {
      if (!MouseBLE.isPressed(MOUSE_LEFT)) MouseBLE.press(MOUSE_LEFT);
    } else {
      if (MouseBLE.isPressed(MOUSE_LEFT)) MouseBLE.release(MOUSE_LEFT);
    }

    // Update Right Click State machine vectors
    if (rightClick) {
      if (!MouseBLE.isPressed(MOUSE_RIGHT)) MouseBLE.press(MOUSE_RIGHT);
    } else {
      if (MouseBLE.isPressed(MOUSE_RIGHT)) MouseBLE.release(MOUSE_RIGHT);
    }

    // Send updates out cleanly over wireless Bluetooth links
    if (movementX != 0 || movementY != 0) {
      MouseBLE.move(movementX, -movementY, 0);
    }

    // Complete transaction parsing cycle; release tracking latch hooks
    noInterrupts();
    packetIndex = 0;
    packetReady = false;
    interrupts();
  }
}
