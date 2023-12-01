/* Copyright 2023 Frank Adams
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
// This software controls a Lenovo ThinkPad T61 Laptop PS/2 Trackpoint
// using a Teensy 4.0 on a daughterboard with a 40 pin connector. 
// The ps/2 code uses the USB PJRC Mouse functions at www.pjrc.com/teensy/td_mouse.html 
// In the Arduino IDE, select Tools, Teensy 4.0. Also under Tools, select Keyboard+Mouse+Joystick
//
// Revision History
// Rev New - Nov 30 , 2023 - Original Release
//
// Trackpoint signals
#define TRACK_DATA 1   // ps/2 data to trackpoint (level translated)
#define TRACK_CLK 0    // ps/2 clock to trackpoint (level translated)
#define BLINK_LED 13
//
// Declare variables that will be used by functions
boolean trackpoint_error = LOW; // sent high when track point routine times out or gives bad ack. Also to turn off.
//
// **************Functions for trackpoint**************************
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
// Function to send the trackpoint a byte of data (command)
//
void trackp_write(char send_data)  
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_z(TRACK_CLK); //
  go_z(TRACK_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TRACK_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TRACK_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_z(TRACK_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_z(TRACK_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TRACK_DATA); // send a 0 to TP
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }  
    send_data = send_data >> 1; // shift data right by 1 to prepare for next loop
  }
// send the parity bit
  if (odd_parity & 1) {  //check if lsb of parity is set
    go_0(TRACK_DATA); // already odd so send a 0 to TP
  }
  else {
    go_z(TRACK_DATA); // send a 1 to TP to make parity odd
  }   
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  go_z(TRACK_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TRACK_DATA)) { // Ack bit s/b low if good transfer
    trackpoint_error = HIGH; //bad ack bit so set the error flag
  }
  while ((digitalRead(TRACK_CLK) == LOW) || (digitalRead(TRACK_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TRACK_CLK);
}
//
// Function to get a byte of data from the trackpoint
//
char trackp_read(void)
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_z(TRACK_CLK); // release the clock
  go_z(TRACK_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TRACK_DATA)) { // Start bit s/b low from tp
    trackpoint_error = HIGH; // No start bit so set the error flag
  }  
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  for (int k=0; k<8; k++) {  
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
    if (digitalRead(TRACK_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1;
    delayMicroseconds(1); // delay to let the clock settle out
    while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
      if (watchdog >= timeout) { //check for infinite loop
        trackpoint_error = HIGH; // set error flag       
        break; // break out of infinite loop
      }
    }
  }
// receive parity
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TRACK_DATA))  { // check if received parity is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // mask off all bits except the lsb
  if (rcv_parity == 0) { // check for bad (even) parity
    trackpoint_error = HIGH; //bad parity so set the error flag
  } 
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// stop bit
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TRACK_DATA) == LOW) { // check if stop bit is bad (low)
    trackpoint_error = HIGH; //bad stop bit so set the error flag
  }
  delayMicroseconds(1); // delay to let the clock settle out
  while (digitalRead(TRACK_CLK) == LOW) { // loop until the clock goes high
    if (watchdog >= timeout) { //check for infinite loop
      trackpoint_error = HIGH; // set error flag       
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TRACK_CLK);
  return rcv_data; // pass the received data back
}
//
void trackpoint_init()
{
  trackpoint_error = LOW; // start with no error
  go_z(TRACK_CLK); // float the clock and data to trackpoint
  go_z(TRACK_DATA);
  //  Sending reset command to trackpoint
  trackp_write(0xff);
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  delay(1000); // wait 1000ms so tp can run its self diagnostic
  //  verify proper response from touchpad
  if (trackp_read() != 0xaa) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  } 
  if (trackp_read() != 0x00) { // verify basic assurance test passed
    trackpoint_error = HIGH;
  }
  //  Send touchpad disable code 
  trackp_write(0xf5);  // tp disable 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
  // Load Mode Byte with 00 using the following special sequence from page 38.
  // Send set resolution to 0 four times followed by a set sample rate to 0x14
// #1 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x01);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #2 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x00);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #3 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x00);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// #4 set resolution
  trackp_write(0xe8);  // set resolution 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x00);  // to zero 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
// Set sample rate
  trackp_write(0xf3);  // set sample rate 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  trackp_write(0x14);  // to 14 hex 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  // set the resolution 
  trackp_write(0xe8); //  Sending resolution command
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  trackp_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  // set the sample rate 
  trackp_write(0xf3); //  Sending sample rate command
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  trackp_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }  
  //  Sending remote mode code so the touchpad will send data only when polled
  trackp_write(0xf0);  // remote mode 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  } 
  //  Sending touchpad enable code (needed for Elan touchpads)
  trackp_write(0xf4);  // tp enable 
  if (trackp_read() != 0xfa) { // verify correct ack byte
    trackpoint_error = HIGH;
  }
}
//
//************************************Setup*******************************************
void setup() {
// ************trackpoint setup
  trackpoint_init(); // reset trackpoint, then set it's resolution and put it in remote mode 
  if (trackpoint_error) {
    delay(500); // wait 1/2 sec
    trackpoint_init(); // try one more time to initialize the trackpoint
  }  
//
  pinMode(BLINK_LED, OUTPUT); // I/O 13 drives the LED on the Teensy as a heartbeat
}
//
// *******declare and initialize trackpoint variables  
char mstat_track; // trackpoint status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
char mx_track; // trackpoint x movement = 8 data bits. The sign bit is in the status register to 
// make a 9 bit 2's complement value. Left to right on the trackpoint gives a positive value. 
char my_track; // trackpoint y movement also 8 bits plus sign. trackpoint movement away from the user gives a positive value.
boolean over_flow_track; // set if x or y movement values are bad due to overflow
boolean left_button_track = 0; // on/off variable for left button = bit 0 of mstat_track
boolean right_button_track = 0; // on/off variable for right button = bit 1 of mstat_track
boolean middle_button_track = 0; // on/off variable for middle button = bit 2 of mstat_track
boolean old_left_button_track = 0; // on/off variable for left button status the previous polling cycle
boolean old_right_button_track = 0; // on/off variable for right button status the previous polling cycle
boolean old_middle_button_track = 0; // on/off variable for middle button status the previous polling cycle  
boolean button_change_track = 0; // Active high, shows when any trackpoint button has changed since last polling cycle
//
char blink_count = 0; // Blink loop counter
boolean blinky = LOW; // Blink LED state
//
//*********************************Main Loop********************************************
//
void loop() {   
//
// ****************************Read the Trackpoint registers*********************************
//
  if (!trackpoint_error) { // check if trackpoint is working and enabled
    // poll the trackpoint for new movement data
    over_flow_track = 0; // assume no overflow until status is received 
    trackp_write(0xeb);  // request data
    if (trackp_read() != 0xfa) { // verify correct ack byte  
      trackpoint_error = HIGH;
    }
    mstat_track = trackp_read(); // save into status variable
    mx_track = trackp_read(); // save into x variable
    my_track = trackp_read(); // save into y variable
    if (((0x80 & mstat_track) == 0x80) || ((0x40 & mstat_track) == 0x40))  {   // x or y overflow bits set?
      over_flow_track = 1; // set the overflow flag
    }   
// change the x data from 9 bit to 8 bit 2's complement
    mx_track = mx_track >> 1; // convert to 7 bits of data by dividing by 2
    mx_track = mx_track & 0x7f; // don't allow sign extension
    if ((0x10 & mstat_track) == 0x10) {   // move the sign into the 8th bit position
      mx_track = 0x80 | mx_track;        
    } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
    my_track = my_track >> 1; // convert to 7 bits of data by dividing by 2
    my_track = my_track & 0x7f; // don't allow sign extension
    if ((0x20 & mstat_track) == 0x20) {   // move the sign into 
      my_track = 0x80 | my_track;              // the 8th bit position
    } 
    my_track = (~my_track + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx_track and my_track if over_flow_track or trackpoint_error is set
    if ((over_flow_track) || (trackpoint_error)) { 
      mx_track = 0x00;       // data is garbage so zero it out
      my_track = 0x00;
    }
  } 
//

// send the x and y trackpoint data over USB if x or y is non-zero
  if ((mx_track != 0x00) || (my_track != 0x00)) {
    Mouse.move(mx_track,my_track);
  }
//
// trackpoint button status 
//  
  if ((0x01 & mstat_track) == 0x01) {   // if left button set 
    left_button_track = 1;   
  }
  else {   // clear left button
    left_button_track = 0;   
  }
  if ((0x02 & mstat_track) == 0x02) {   // if right button set 
    right_button_track = 1;   
  } 
  else {   // clear right button
    right_button_track = 0;  
  }
  if ((0x04 & mstat_track) == 0x04) {   // if middle button set 
    middle_button_track = 1;   
  } 
  else {   // clear middle button
    middle_button_track = 0;  
  }    
  // Determine if any trackpoint buttons have changed since last polling cycle
  button_change_track = ((left_button_track ^ old_left_button_track) | (right_button_track ^ old_right_button_track) | (middle_button_track ^ old_middle_button_track));
// Send trackpoint button status if there was a change since last time. 
  if (button_change_track) {
    Mouse.set_buttons(left_button_track, middle_button_track, right_button_track); // send button status
    old_left_button_track = left_button_track; // remember new button status for next polling cycle
    old_right_button_track = right_button_track;
    old_middle_button_track = middle_button_track;
  }
//
// **************************************End of trackpoint routine***********************************
//
// Blink LED on Teensy to show a heart beat
//
  if (blink_count == 0x17) {  
    digitalWrite(BLINK_LED, blinky);
    blinky = !blinky;
    blink_count = 0;
  }
  else {
    blink_count = blink_count + 1;
  }
//
// ****************End of main loop
//
  delay(28); // The overall trackpoint scanning rate is approximately 30ms
}
