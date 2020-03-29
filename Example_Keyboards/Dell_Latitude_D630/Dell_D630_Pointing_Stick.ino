/* Copyright 2020 Frank Adams
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
// This software interfaces the Teensy 3.2 with a laptop pointing stick (aka trackpoint) that is not PS/2.
// Raw trackpoints use a Wheatstone bridge configuration with 4 resistive strain gauges.  
// The connections from the Teensy are 3.3 volts, ground, x voltage to A0, and y voltage to A1.
// In the Arduino IDE, select Tools, Teensy 3.2. Also under Tools, select Keyboard+Mouse+Joystick
//
// Revision History
// Rev 1.0 - March 28, 2020 - Original Release
//
// declare and initialize variables  
  int x_read; // stores the reading from the X ADC
  int y_read; // same for the Y ADC
  char mx; // signed byte used by the Mouse.move function for x value. Positive value moves to the right
  char my; // positive y value moves down 
  int x_delta; // signed 16 bit value gives x movement amount
  int y_delta; // y version. 
  int x_center; //resting position of x sensor
  int y_center; //resting position of y sensor
  int noise_zone = 175; // dead zone around the center. Increase this number if 
                        // the cursor moves without touching the TP. Decrease if
                        // too much force is needed to move the cursor.
  
void setup()
{
  delay(1000); // delay a second to let things settle out
  analogReadRes(16); // set ADC resolution to 16 bits (13 usable)
  analogReadAveraging(16); // ADC will take the average of 16 reads 
  x_center = analogRead(A0); // store the center (no movement) position for x
  y_center = analogRead(A1); // same for y. 
// When the keyboard code is added, detect when Fn - F8 is pushed to indicate that
// x_center and y_center should be read again from the ADC. This would be because
// the operator notices the cursor is moving without being pushed.
}
//
// ************************************Main Loop***************************************************************
void loop() {
  mx = 0; // for each loop, start with zero and update only if movement detected
  my = 0; // same for y

  x_read = analogRead(A0); // read the ADC tied to the X sensor
  if (x_read > (x_center + noise_zone)) {  // is ADC to the right of the noise zone beyond the resting position?
    x_delta = (x_read - (x_center + noise_zone)) / 10; // find out how far beyond noise zone. Divide by 10 to slow it down.
    mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
  }
  else if (x_read < (x_center - noise_zone)) { // is ADC to the left of the noise zone beyond the resting position?
        x_delta = (((x_center - noise_zone) - x_read) / 10) * -1; // Answer should be negative so multiply by -1
    mx = byte(x_delta);
  }

  y_read = analogRead(A1); // read the ADC tied to the Y sensor
  if (y_read > (y_center + noise_zone)) {
    y_delta = ((y_read - (y_center + noise_zone)) / 10) * -1; // up movement is negative for Mouse.move function
    my = byte(y_delta);  
  }
  else if (y_read < (y_center - noise_zone)) {
    y_delta = (((y_center - noise_zone) - y_read) / 10);
    my = byte(y_delta);
  }

// send the x and y data over usb if either one is not at center position
  if ((mx != 0x00) || (my != 0x00)) {
    Mouse.move(mx,my);
  }
//
// **************************************End of trackpoint routine***********************************
// 
  delay(30);  // wait 30ms before repeating main loop
}
