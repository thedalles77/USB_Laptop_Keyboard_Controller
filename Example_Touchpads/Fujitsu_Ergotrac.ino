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
// This software interfaces the Teensy 3.2 with a Fujitsu Ergotrac touch sensor from a Lifebook 755x.
// Connections: Teensy I/O 9 to pin 1 - This drives the reference pulse to the touch sensor
//              Teensy ADC 10 to pin 2 - ADC reads this pulse voltage for the X movement
//              Teensy ADC 11 to pin 3 - ADC reads this pulse voltage for the Y movement
//              Teensy Ground to pin 4 
// 
// Revision History
// Rev New Nov 2, 2021 - Original Release
//
// X/Y outputs from ErgoTrac feed Teensy ADC channels
#define ErgoTrac_X A10
#define ErgoTrac_Y A11
// Reference signal from Teensy into ErgoTrac
#define ErgoTrac_Ref 9
// left and right mouse buttons
#define TP_LEFT 8
#define TP_RIGHT 7
//
// declare and initialize ErgoTrac variables
int x_read; // stores the reading from the X ADC
int y_read; // same for the Y ADC
char mx; // signed byte used by the Mouse.move function for x value. Positive value moves to the right
char my; // positive y value moves down 
int x_delta; // signed 16 bit value gives x movement amount
int y_delta; // y version. 
int x_center; //resting position of x sensor
int y_center; //resting position of y sensor
int noise_zone = 150; // dead zone around the center. 
char slowdown_left = 75; // factor to slow the curser movement down going left
char slowdown_right = 60; // factor to slow the curser movement down going right
char slowdown_up = 70; // factor to slow the curser movement down going up
char slowdown_down = 75; // factor to slow the curser movement down going down
boolean left_button = 0; // on/off variable for left button, 1 = pushed
boolean right_button = 0; // on/off variable for right button, 1 = pushed
boolean old_left_button = 0; // on/off variable for left button from the previous cycle
boolean old_right_button = 0; // on/off variable for right button from the previous cycle
boolean button_change = 0; // Shows when the left or right buttons have changed, 1 = change

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
// Function to re-capture the at-rest X & Y ADC values
void recenter(void) {
  digitalWrite(ErgoTrac_Ref, LOW); // resting state of reference
  delay(25); // wait before sending calibration pulse
  digitalWrite(ErgoTrac_Ref, HIGH); // Send calibration pulse high
  delayMicroseconds(400); // wait before reading ADC to allow for rise time
  x_center = analogRead(ErgoTrac_X); // store the center (no movement) value for x
  y_center = analogRead(ErgoTrac_Y); // same for y
  delayMicroseconds(50); // wait before releasing calibration pulse
  digitalWrite(ErgoTrac_Ref, LOW); // Send calibration pulse back to resting state
}
// ************************************Begin Routine*********************************************************
void setup()
{
  go_pu(TP_RIGHT); // set right button as input with a pullup
  go_pu(TP_LEFT); // set left button as input with a pullup
  delay(2000); // wait to let things settle out
  analogReadRes(13); // set ADC resolution to 13 bits (16 bit ADC but low 3 bits are random noise)
  analogReadAveraging(8); // ADC will take the average of multiple reads to filter noise.
  pinMode(ErgoTrac_Ref, OUTPUT); // define Teensy I/O that drives the refernce pulses to the ErgoTrac
  recenter(); // capture the at-rest X and Y position
  delay(25); // wait before entering main loop
}
//
// ************************************Main Loop***************************************************************
void loop() {
  digitalWrite(ErgoTrac_Ref, HIGH); // send calibration reference pulse

  mx = 0; // for each loop, start with zero and update only if movement detected
  my = 0; // same for y
// Read the ADC's
  x_read = analogRead(ErgoTrac_X); // read the ADC tied to the X sensor
  y_read = analogRead(ErgoTrac_Y); // read the ADC tied to the Y sensor
// Check if X or Y movement is beyond the dead zone
  if ((x_read > (x_center + noise_zone)) || (x_read < (x_center - noise_zone)) || (y_read > (y_center + noise_zone)) || (y_read < (y_center - noise_zone))) {
// Only do the following if at least one reading is beyond the dead zone.
// Figure out if the X value is to the right or the left of the center resting value
    if (x_read > x_center) {  // is X ADC to the right of the resting position?
      x_delta = (x_read - x_center) / slowdown_right; // find out how far beyond center. Divide it to slow it down.
      mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
    }
    else { // X ADC is equal or to the left of the resting position
      x_delta = ((x_center - x_read) / slowdown_left) * -1; // Answer should be negative so multiply by -1
      mx = byte(x_delta); // convert signed 16 bit to signed 8 bit
    }
// Figure out if the Y value is above or below the center resting value
    if (y_read > y_center) { // is Y ADC above the resting position?
      y_delta = ((y_read - y_center) / slowdown_down) * -1; // up movement is negative for Mouse.move function
      my = byte(y_delta);  // convert signed 16 bit to signed 8 bit
    }
    else { // Y ADC is equal or below the resting position
      y_delta = (y_center - y_read) / slowdown_up;
      my = byte(y_delta); // convert signed 16 bit to signed 8 bit
    }
  }
// send the x and y data over usb if either one is not at center position
  if ((mx != 0x00) || (my != 0x00)) {
    Mouse.move(mx,my);
  }
  digitalWrite(ErgoTrac_Ref, LOW); // release calibration reference
//
// Read the right button
//
  if (!digitalRead(TP_RIGHT)) {   // check if right button is low (low = pushed) 
    right_button = 1;   // set the right button
  }
  else {   // clear right button
    right_button = 0;   // clear the right button
  }
// Read the left button 
  if (!digitalRead(TP_LEFT)) {   // check if left button is low (low = pushed) 
    left_button = 1;    // set the left button
  }
  else {   // clear left button
    left_button = 0;    // clear the left button
  }
//    
// Determine if the buttons have changed (using XOR) since the last cycle 
  button_change = ((left_button ^ old_left_button) | (right_button ^ old_right_button));
// Don't send button status if there's no change since last time 
  if (button_change){
    Mouse.set_buttons(left_button,0, right_button); // send button status over USB
  }
  old_left_button = left_button; // remember button status for the next cycle
  old_right_button = right_button;

  delay(25);  // wait before repeating
}
