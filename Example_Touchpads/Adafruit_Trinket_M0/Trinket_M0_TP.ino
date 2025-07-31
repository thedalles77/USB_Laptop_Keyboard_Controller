/* Copyright 2025 Frank Adams
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
// Adafruit Trinket M0 controls a Samsung Galaxybook Flex NP950QCG touchpad over I2C and sends the results 
// over USB using the Arduino Mouse functions. The X and Y delta values plus a left button push and release are sent. 
// There is a single mouse button on the bottom in the middle plus a finger tap on the touchpad will also give a left button press
// The default firmware in the touchpad doesn't have any right mouse button press so it is not available.
// The touchpad probably has other modes of operation that include right mouse button but they need to be loaded
// on power up. Because I don't have a users manual that defines how to set up other modes, I'm left with the default mode at power up. 
// 
// The following 6 lines are needed to turn off the VERY BRIGHT RGB LED on the Trinket M0
#include <Adafruit_DotStar.h>
#include <SPI.h>         
#define NUMPIXELS 1 // Number of LEDs in strip
#define DATAPIN   7
#define CLOCKPIN   8
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
// Include the Arduino Wire and Mouse libraries for I2C and USB HID
#include <Wire.h>
#include <Mouse.h>
//
#define led 13 // use the on-board LED to show when the touchpad detects activity
#define Int_N 1 // touchpad Interrupt signal monitored by Trinket, active low
#define tp_address 0x40 // touchpad I2C address
//
char reg0; // These are the first 8 registers that are read from the touchpad
char reg1; // 
char reg2; // 
char reg3; // 
char reg4; // 
char reg5; // 
char reg6; // 
char reg7; // 

boolean left_button = 0; // Active high, on/off variable for left button 
boolean old_left_button = 0; // Active high, on/off variable for left button status from the previous polling cycle
//
// ********************Common Functions**********************************************
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
//
void setup() {
  delay (1000); // let power stabilize
// the next 2 lines are for turning off the VERY BRIGHT RGB LED on the Trinket M0
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off 
//
  go_0(led); // turn off the on-board led
  go_z(Int_N); // set the Int_N pin as an input without a pullup
  Mouse.begin(); // Initiate USB mouse
  Wire.begin(); // Initiate the Wire library and join the I2C bus as a master
  //
}
//
void loop() {
  if (!digitalRead(Int_N)) { // check interrupt signal to see if low. If low, the TP has data to read
    go_1(led); // turn on the on-board led when there is activity on the TP
    Wire.beginTransmission(tp_address); // write to I2C address 0x40 and should get an "ack"
    Wire.write(0x00); // high btye address of first register to read (value doesn't make a difference for this touchpad)
    Wire.write(0x00); // low byte address of first register to read (value doesn't make a difference for this touchpad)
    Wire.endTransmission(true); // with stop bit. Other touchpads want this to be false (repeated start)
    Wire.requestFrom(tp_address,8); // read 8 bytes (other touchpads will read from the high/low address just loaded)
    if (Wire.endTransmission(true) == 0) {  // send stop bit and check if TP "acked"
      reg0 = Wire.read(); // value always 0x08
      reg1 = Wire.read(); // value always 0x00 
      reg2 = Wire.read(); // value always 0x08
      reg3 = Wire.read(); // 0x01 when button pressed or finger tap. 0x00 when button or finger released
      reg4 = Wire.read(); // X Delta high byte (this value is used for mouse X movement)
      reg5 = Wire.read(); // X Delta low byte (low 8 bits not used since Mouse.move only needs an 8 bit value
      reg6 = Wire.read(); // Y Delta high byte (same as X)
      reg7 = Wire.read(); // Y Delta low byte 
// send the x and y delta (relative) movement data back via USB if either one is non-zero
      if ((reg4 != 0x00) || (reg6 != 0x00)) {
        Mouse.move(reg4,reg6);
      }
// mouse button
      if ((reg3 & 0x01) == 0x01) {  // test bit 0 for left mouse button
        left_button = 1;
      }
      else {
        left_button = 0;
        }
// Determine if the left touchpad button has changed since last polling cycle 
      if ((left_button == 1) && (old_left_button == 0)) { // button was just pushed
        Mouse.press(MOUSE_LEFT); // send left button press
      }
      else if ((left_button == 0) && (old_left_button == 1)) { // button was just released
        Mouse.release(MOUSE_LEFT); // send a release of the left button
      }
      old_left_button = left_button; // remember button status for next polling cycle
    }    
    else { 
      // put future no-acknoledge error handler here
    } 
//    
  }
  else {
    go_0(led); // turn off the on-board led when there is no touchpad activity 
  }

}
