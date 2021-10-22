/* Copyright 2021 Frank Adams
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
// This software implements an I2C Touchpad Controller using a Teensy 3.2 
// It assumes the TP is at I2C address 0x24
// Register 0 function unknown - not used
// Register 1 is the button register. 0x00=no buttons, 0x03=left button, 0x04=right button
// Register 2 function unknown - not used
// Register 3 is the X delta movement 8 bit 2's complement value. Left to right gives positive values
// Register 4 is the Y delta movement 8 bit 2's complement value. Top to bottom gives positive values
// Register 5 function unknown - not used
// Register 6 is the touch sensor. 0x00=no touch, 0x0f=touch - The code below does not use this register
//
// Release Date: August 3, 2021
//
#include <Wire.h> // used for i2c bus to touchpad
//
// define the pins on the Teensy used for the touchpad I2C bus
#define TP_DATA 14 // Touchpad i2c data signal 
#define TP_CLK 15  // Touchpad i2c clock signal 

//
//----------------------------------Setup before main loop-------------------------------------------
//
void setup() {
Wire.begin(); // i2c for touchpad
}
// declare and initialize touchpad variables  
char buttonreg; // touchpad left and right buttons read from register #1. Left button is bit 0 and 1, right button is bit 2
char xreg; // touchpad x movement from register #3. 2's complement, left to right movement gives positive value. 
char yreg; // touchpad y movement from register #4. 2's complement, top to bottom movement gives positive value.
boolean left_button = 0; // Active high, on/off variable for left button 
boolean right_button = 0; // Active high, on/off variable for right button 
boolean old_left_button = 0; // Active high, on/off variable for left button status from the previous polling cycle
boolean old_right_button = 0; // Active high, on/off variable for right button status from the previous polling cycle
boolean button_change = 0; // Active high, shows when a touchpad left or right button has changed since the last polling cycle
//
//---------------------------------Main Loop---------------------------------------------
//
void loop() {   
// poll the touchpad for new movement or button data 
//
  Wire.beginTransmission(0x24); // Load the TP register pointer 
  Wire.write(0x01);             // so it points to register 1
  Wire.endTransmission();       // 
  Wire.requestFrom(0x24, 4); // Read 4 bytes from the TP starting at register 1
    
  buttonreg = Wire.read(); // read the button status bits in register 1
  Wire.read(); // read and throw away byte from unused register 2
  xreg = Wire.read(); // read the X value from register 3
  yreg = Wire.read(); // read the Y value from register 4    
// send the x and y data back via usb if either one is non-zero
  if ((xreg != 0x00) || (yreg != 0x00)) {
    Mouse.move(xreg,yreg);
  }
//
// check the left and right bits in the button register from the touchpad
  if (buttonreg == 0x03) {   // the left button is pushed 
    left_button = 1;   // set the left button variable
  }
  else {   // otherwise clear the left button variable
    left_button = 0;   
  }
  if (buttonreg == 0x04) { // the right button is pushed
    right_button = 1;   // set the right button variable
  } 
  else {   // otherwise clear right button variable
    right_button = 0;  
  }
// Determine if the left or right touchpad buttons have changed since last polling cycle using xor
  button_change = (left_button ^ old_left_button) | (right_button ^ old_right_button);
// Don't send button status if there's no change since last time. 
  if (button_change){
    Mouse.set_buttons(left_button, 0, right_button); // send left, middle, and right button status
  }
  old_left_button = left_button; // remember button status for next polling cycle
  old_right_button = right_button;
//
  delay(25); // This is the polling rate in msec
// 
}
