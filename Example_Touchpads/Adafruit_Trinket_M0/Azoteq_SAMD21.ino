/* Copyright 2022 Frank Adams
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
// Adafruit Trinket M0 controls an Azoteq touchpad Part No: TPS65-201A-S over I2C and sends the results 
// over USB using the Arduino mouse function. The Mouse.press and release functions are used
// to send a left button push and release based on a finger tap on the touchpad. 
// 
#include <Wire.h>
#include <Mouse.h>
#define led 13
#define rdy 3 // touchpad ready signal monitored by SAMD21, active high
#define rst_n 1 // touchpad reset driven by SAMD21, active low
#define hold_n 4 // input (active low) to hold SAMD21 out of main loop so it can be programmed 
#define tp_address 0x74 // Azoteq touchpad i2c address

char gesture0; // holds gesture events 0
char gesture1; // holds gesture events 1
char sys_info0; // holds system info 0
char sys_info1; // holds system info 1
char finger_count; // number of fingers
char xrel_high; // holds the relative x high 8 bits
char xrel_low; // holds the relative x low 8 bits
char yrel_high; // holds the relative y high 8 bits
char yrel_low; // holds the relative y low 8 bits
char xabs_high; // holds the absolute x high 8 bits
char xabs_low; // holds the absolute x low 8 bits
char yabs_high; // holds the absolute y high 8 bits
char yabs_low; // holds the absolute y low 8 bits
char tch_strength_high; // holds the touch strength high 8 bits
char tch_strength_low; // holds the touch strength low 8 bits
char tch_area; // holds the touch area/size
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
  go_0(led); // turn off the led
  go_pu(hold_n); // set the hold_n pin as an input with a pullup
  go_z(rdy); // set rdy pin as an input without a pullup
// reset the TP
  go_0(rst_n); // drive reset low (active) to TP 
  delay (1000); // wait a second
  go_1(rst_n); // drive reset high (inactive) to TP
  while (digitalRead(!hold_n)) {
    // stay in while loop until hold_n goes high to hold SAMD21 from being a mouse so it can be programmed
  }
  Mouse.begin(); // Initiate USB mouse
  Wire.begin(); // Initiate the Wire library and join the I2C bus as a master
}

void loop() {
  if (digitalRead(rdy)) { // check rdy signal to see if TP has data
    go_1(led); // turn on the led when there is activity on the TP
    Wire.beginTransmission(tp_address); //
    Wire.write(0x00); // high btye address of first register to read
    Wire.write(0x0d); // low byte address of first register to read
    Wire.endTransmission(false); // no stop bit (makes a repeated start)
    Wire.requestFrom(tp_address,16); // read 16 bytes starting at 0x000d
    if (Wire.endTransmission(true) == 0) {  // send stop bit and check if TP "acked"
      gesture0 = Wire.read(); // read the gesture 0 byte from register 0x000d
      gesture1 = Wire.read(); // read the gesture 1 byte from register 0x000e
      sys_info0 = Wire.read(); // read the system info 0 byte from register 0x000f
      sys_info1 = Wire.read(); // read the system info 1 byte from register 0x0010
      finger_count = Wire.read(); // read the finger count byte from register 0x0011
      xrel_high = Wire.read(); // read the high relative X byte from register 0x0012
      xrel_low = Wire.read(); // read the low relative X byte from register 0x0013
      yrel_high = Wire.read(); // read the high relative Y byte from register 0x0014
      yrel_low = Wire.read(); // read the low relative Y byte from register 0x0015
      xabs_high = Wire.read(); // read the high absolute X byte from register 0x0016
      xabs_low = Wire.read(); // read the low absolute X byte from register 0x0017
      yabs_high = Wire.read(); // read the high absolute Y byte from register 0x0018
      yabs_low = Wire.read(); // read the low absolute Y byte from register 0x0019
      tch_strength_high = Wire.read(); // read the high touch strength byte from register 0x001a
      tch_strength_low = Wire.read(); // read the low touch strength byte from register 0x001b
      tch_area = Wire.read(); // read the touch area/size byte from register 0x001c
//      
      // send the x and y data back via usb if either one is non-zero
      if ((xrel_low != 0x00) || (yrel_low != 0x00)) {
        Mouse.move(xrel_low,yrel_low);
        xrel_low = 0; // zero out the registers (just in case the TP doesn't)
        yrel_low = 0;
      }
      if ((gesture0 & 0x01) == 0x01) {  // test bit 0
      Mouse.click(MOUSE_LEFT);
//        left_button = 1;
      }
//      else {
//        left_button = 0;
//      }
      // Determine if the left touchpad button has changed since last polling cycle 
//      if ((left_button == 1) && (old_left_button == 0)) {
//        Mouse.press(MOUSE_LEFT); // push left button 
//      }
//      else if ((left_button == 0) && (old_left_button == 1)) {
//        Mouse.release(MOUSE_LEFT); // release left button
//      }
//      old_left_button = left_button; // remember button status for next polling cycle
    }
    else { 
      // put future no-acknoledge error handler here
    } 
    // Send the End Communication Window Command per para 8.7 of Azoteq data sheet
    Wire.beginTransmission(tp_address); //
    Wire.write(0xee); // high btye of address pointer
    Wire.write(0xee); // low byte of address pointer
    Wire.write(0x00); // data value doesn't matter
    Wire.endTransmission(true); // send stop bit
//    
    delay (30); // overall loop rate in milliseconds 
  }
  else {
    go_0(led); // turn off the led when there is no tp activity 
  }
}
