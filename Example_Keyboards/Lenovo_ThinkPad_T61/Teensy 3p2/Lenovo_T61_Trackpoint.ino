/* Copyright 2019 Frank Adams
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
// This software controls a Lenovo ThinkPad T61 Laptop Trackpoint using a Teensy 3.2 on 
// a daughterboard with a 44 pin FPC connector. The keyboard part number is 42T3177.
// This code uses the USB PJRC Mouse functions at www.pjrc.com/teensy/td_mouse.html 
// The ps/2 code has a watchdog timer so the code can't hang if a clock edge is missed.
// In the Arduino IDE, select Tools, Teensy 3.2. Also under Tools, select Keyboard+Mouse+Joystick
//
// Revision History
// Rev 1.0 - May 26, 2019 - Original Release
//
// Trackpoint signals
#define TP_DATA 18   // ps/2 data to trackpoint
#define TP_CLK 19    // ps/2 clock to trackpoint
#define TP_RESET 0   // active high trackpoint reset at power up
//
// **************Functions for Trackpoint**************************
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
void tp_write(char send_data)  
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_z(TP_CLK); //
  go_z(TP_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TP_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TP_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_z(TP_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_z(TP_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TP_DATA); // send a 0 to TP
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        break; // break out of infinite loop
      }
    }  
    send_data = send_data >> 1; // shift data right by 1 to prepare for next loop
  }
// send the parity bit
  if (odd_parity & 1) {  //check if lsb of parity is set
    go_0(TP_DATA); // already odd so send a 0 to TP
  }
  else {
    go_z(TP_DATA); // send a 1 to TP to make parity odd
  }   
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  go_z(TP_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  while ((digitalRead(TP_CLK) == LOW) || (digitalRead(TP_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
}
//
// Function to get a byte of data from the trackpoint
//
char tp_read(void)
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_z(TP_CLK); // release the clock
  go_z(TP_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        break; // break out of infinite loop
      }
    }
    if (digitalRead(TP_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
  return rcv_data; // pass the received data back
}
//
void trackpoint_init()
{
  go_z(TP_CLK); // float the clock and data to trackpoint
  go_z(TP_DATA);
  // Trackpoint Reset signal is active high. Start it off low to let power stabilize
  go_0(TP_RESET); // drive low
  delay(1000); // wait 1 second
  go_1(TP_RESET); // drive High to activate Reset signal to trackpoint
  delay(1000); // wait 1 second to give it a good long reset
  go_0(TP_RESET); // drive Reset back to the inactive (low) state
  delay(1000); // wait 1 second before proceeding so trackpoint is ready
  //  Sending reset command to trackpoint
  tp_write(0xff);
  if (tp_read() != 0xfa) { // verify correct ack byte
    // add future error handler
  }
  delayMicroseconds(100); // give the tp time to run its self diagnostic
  //  verify proper response from tp
  if (tp_read() != 0xaa) { // verify basic assurance test passed
    // add future error handler
  } 
  if (tp_read() != 0x00) { // verify correct device id
    // add future error handler
  }
  //  Sending remote mode code so the trackpoint will send data only when polled
  tp_write(0xf0);  // remote mode 
  if (tp_read() != 0xfa) { // verify correct ack byte
    // add future error handler
  } 
}
//
//************************************Setup*******************************************
void setup() {
// ************trackpoint setup
  trackpoint_init(); // reset trackpoint, then set it's resolution and put it in remote mode 
  }
//
// *******declare and initialize trackpoint variables  
char mstat; // trackpoint status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
char mx; // trackpoint x movement = 8 data bits. The sign bit is in the status register to 
         // make a 9 bit 2's complement value. Left to right on the trackpoint gives a positive value. 
char my; // trackpoint y movement also 8 bits plus sign. trackpoint movement away from the user gives a positive value.
boolean over_flow; // set if x or y movement values are bad due to overflow
boolean left_button = 0; // on/off variable for left button = bit 0 of mstat
boolean right_button = 0; // on/off variable for right button = bit 1 of mstat
boolean middle_button = 0; // on/off variable for middle button = bit 2 of mstat
boolean old_left_button = 0; // on/off variable for left button status the previous polling cycle
boolean old_right_button = 0; // on/off variable for right button status the previous polling cycle
boolean old_middle_button = 0; // on/off variable for middle button status the previous polling cycle  
boolean button_change = 0; // Active high, shows when any trackpoint button has changed since last polling cycle
//
//*********************************Main Loop*******************************************
//
void loop() {   
// poll the trackpoint for new movement data
  over_flow = 0; // assume no overflow until status is received 
  tp_write(0xeb);  // request data
  if (tp_read() != 0xfa) { // verify correct ack byte
     // add future error handler here
  }
  mstat = tp_read(); // save into status variable
  mx = tp_read(); // save into x variable
  my = tp_read(); // save into y variable
  if (((0x80 & mstat) == 0x80) || ((0x40 & mstat) == 0x40))  {   // x or y overflow bits set?
    over_flow = 1; // set the overflow flag
  }   
// change the x data from 9 bit to 8 bit 2's complement
  mx = mx >> 1; // convert to 7 bits of data by dividing by 2
  mx = mx & 0x7f; // don't allow sign extension
  if ((0x10 & mstat) == 0x10) {   // move the sign into 
    mx = 0x80 | mx;              // the 8th bit position
  } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
  my = my >> 1; // convert to 7 bits of data by dividing by 2
  my = my & 0x7f; // don't allow sign extension
  if ((0x20 & mstat) == 0x20) {   // move the sign into 
    my = 0x80 | my;              // the 8th bit position
  } 
  my = (~my + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx and my if over_flow is set
  if (over_flow) { 
    mx = 0x00;       // data is garbage so zero it out
    my = 0x00;
  } 
// send the x and y data back via usb if either one is non-zero
  if ((mx != 0x00) || (my != 0x00)) {
    Mouse.move(mx,my);
  }
//
// send the trackpoint left and right button status over usb if no error
    if ((0x01 & mstat) == 0x01) {   // if left button set 
      left_button = 1;   
    }
    else {   // clear left button
      left_button = 0;   
    }
    if ((0x02 & mstat) == 0x02) {   // if right button set 
      right_button = 1;   
    } 
    else {   // clear right button
      right_button = 0;  
    }
    if ((0x04 & mstat) == 0x04) {   // if middle button set 
      middle_button = 1;   
    } 
    else {   // clear middle button
      middle_button = 0;  
    }    
// Determine if any buttons have changed since last polling cycle
    button_change = ((left_button ^ old_left_button) | (right_button ^ old_right_button) | (middle_button ^ old_middle_button));
// Don't send button status if there's no change since last time. 
    if (button_change) {
      Mouse.set_buttons(left_button, middle_button, right_button); // send button status
    }
    old_left_button = left_button; // remember new button status for next polling cycle
    old_right_button = right_button;
    old_middle_button = middle_button;

// **************************************End of trackpoint routine***********************************
  delay(24); // The overall keyboard/trackpoint scanning rate is 30ms
}
