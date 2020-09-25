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
// This software interfaces the Teensy 3.2 with a PS/2 touchpad from a laptop.
// The ps/2 code uses the USB PJRC Mouse functions at www.pjrc.com/teensy/td_mouse.html 
// The ps/2 code has a watchdog timer so the code can't hang if a clock edge is missed.
// In the Arduino IDE, select Tools, Teensy 3.2. Also under Tools, select Keyboard+Mouse+Joystick
//
// Revision History
// Rev 1.0 - Nov 23, 2018 - Original Release
// Rev 1.1 - Dec 2, 2018 - Replaced ps/2 trackpoint code from playground arduino with my own code 
// Rev 1.2 - Feb 2, 2019 - Changed the error routine and added an error LED
// Rev 1.3 - Feb 14, 2019 - Added pinouts for different touchpads
// Rev 1.4 - July 31, 2020 - Added enable command for Elantech touchpads and increased the delay after reset
//
// This code has been tested on the following touchpads:
//
// Dell D630 Touchpad. I used the wires from the touchpad connector as follows:
// Pin 2 = 5V
// Pin 1 = Gnd
// Pin 7 = Clock
// Pin 6 = Data
// This touchpad has resistive pullups so no additional pullups were required. 
//
// HP Pavilion DV9000 Touchpad part number 920-000702-04 Rev A
// The test points on the touchpad were wired as follows:
// T22 = 3.3V  (The touchpad also works with 5V)
// T23 = Gnd   (I soldered to the ground plane)
// T10 = Clock 
// T11 = Data 
// This touchpad has active pullups so no additional pullups were required.
// 
// Dell 1545 touchpad. I soldered wires to the 4 pin connector as follows:
// Pin 1 = 5V
// Pin 4 = Gnd
// Pin 3 = Clock
// Pin 2 = Data
// This touchpad has active pullups so no additional pullups were required. 
//
// The touchpad ps/2 data and clock lines are connected to the following Teensy I/O pins (modify to match your jumper wires)
#define TP_DATA 14
#define TP_CLK 15
// Teensy LED will be lit if the touchpad fails to respond properly during initialization
#define ERROR_LED 13
//
// Declare variable that will be used by functions
boolean touchpad_error = LOW; // sent high when touch pad routine times out
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
// *****************Functions for Touchpad***************************
//
// Function to send the touchpad a byte of data (command)
//
void tp_write(char send_data)  
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_pu(TP_CLK); //
  go_pu(TP_DATA); //
  delayMicroseconds(250); // wait before requesting the bus
  go_0(TP_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(100); // wait for 100 microseconds per bus spec
  go_0(TP_DATA); //  Send the Data line low (the start bit)
  delayMicroseconds(1); //
  go_pu(TP_CLK); //   Release the Clock line so it is pulled high
  delayMicroseconds(1); // give some time to let the clock line go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
// send the 8 bits of send_data 
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_pu(TP_DATA); // send a 1 to TP
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
    go_pu(TP_DATA); // send a 1 to TP to make parity odd
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
  go_pu(TP_DATA); //  Release the Data line so it goes high as the stop bit
  delayMicroseconds(80); // testing shows delay at least 40us 
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  delayMicroseconds(1); // wait to let the data settle
  if (digitalRead(TP_DATA)) { // Ack bit s/b low if good transfer
  }
  while ((digitalRead(TP_CLK) == LOW) || (digitalRead(TP_DATA) == LOW)) { // loop if clock or data are low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
}
//
// Function to get a byte of data from the touchpad
//
char tp_read(void)
{
  unsigned int timeout = 200; // breakout of loop if over this value in msec
  elapsedMillis watchdog; // zero the watchdog timer clock
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_pu(TP_CLK); // release the clock
  go_pu(TP_DATA); // release the data
  delayMicroseconds(5); // delay to let clock go high
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    if (watchdog >= timeout) { //check for infinite loop
      break; // break out of infinite loop
    }
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put error handler here if desired
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
  if (rcv_parity == 0) { // check for bad (even) parity
  // bad parity - pass to future error handler
  } 
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
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
  // send bad stop bit to future error handler
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
void touchpad_init()
{
  touchpad_error = LOW; // start with no error
  go_pu(TP_CLK); // float the clock and data to touchpad
  go_pu(TP_DATA);
  //  Sending reset command to touchpad
  tp_write(0xff);
  if (tp_read() != 0xfa) { // verify correct ack byte
    touchpad_error = HIGH;
  }
  delay(1000); // wait 1000ms so tp can run its self diagnostic
  //  verify proper response from tp
  if (tp_read() != 0xaa) { // verify basic assurance test passed
    touchpad_error = HIGH;
  } 
  if (tp_read() != 0x00) { // verify basic assurance test passed
    touchpad_error = HIGH;
  }
  //  Send touchpad disable code 
  tp_write(0xf5);  // tp disable 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }
  // Load Mode Byte with 00 using the following special sequence from page 38.
  // Send set resolution to 0 four times followed by a set sample rate to 0x14
// #1 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }  
  tp_write(0x01);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }
// #2 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }
// #3 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }
// #4 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }
// Set sample rate
  tp_write(0xf3);  // set sample rate 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }  
  tp_write(0x14);  // to 14 hex 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  } 
  // set the resolution 
  tp_write(0xe8); //  Sending resolution command
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  } 
  tp_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  } 
  // set the sample rate 
  tp_write(0xf3); //  Sending sample rate command
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  } 
  tp_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (tp_read() != 0xfa) { // verify correct ack byte
//    init_error = HIGH;
  }  
  //  Sending remote mode code so the touchpad will send data only when polled
  tp_write(0xf0);  // remote mode 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    touchpad_error = HIGH;
  } 
  //  Sending touchpad enable code (needed for Elan touchpads)
  tp_write(0xf4);  // tp enable 
  if (tp_read() != 0xfa) { // verify correct ack byte
//    touchpad_error = HIGH;
  }
}
// ************************************Begin Routine*********************************************************
void setup()
{
  touchpad_init(); // reset touchpad, then set it's resolution and put it in remote mode 
  pinMode(ERROR_LED, OUTPUT); // define teensy I/O 13 as an output
}

// declare and initialize variables  
  char mstat; // touchpad status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
  char mx; // touchpad x movement = 8 data bits. The sign bit is in the status register to 
           // make a 9 bit 2's complement value. Left to right on the touchpad gives a positive value. 
  char my; // touchpad y movement also 8 bits plus sign. Touchpad movement away from the user gives a positive value.
  boolean over_flow; // set if x or y movement values are bad due to overflow
  boolean left_button = 0; // on/off variable for left button = bit 0 of mstat
  boolean right_button = 0; // on/off variable for right button = bit 1 of mstat
  boolean old_left_button = 0; // on/off variable for left button status the previous polling cycle
  boolean old_right_button = 0; // on/off variable for right button status the previous polling cycle
  boolean button_change = 0; // Active high, shows when a touchpad left or right button has changed since last polling cycle
  
// ************************************Main Loop***************************************************************
void loop() {
  if (touchpad_error == LOW) { // check if touchpad is present
    digitalWrite(ERROR_LED, LOW); // turn off LED on Teensy to show touchpad initialized OK
// poll the touchpad for new movement data
    over_flow = 0; // assume no overflow until status is received 
    tp_write(0xeb);  // request data
    if (tp_read() != 0xfa) { // verify correct ack byte
    // bad ack - pass to future error handler
    }
    mstat = tp_read(); // save into status variable
    mx = tp_read(); // save into x variable
    my = tp_read(); // save into y variable
    if (((0x80 & mstat) == 0x80) || ((0x40 & mstat) == 0x40))  {   // x or y overflow bits set?
      over_flow = 1; // set the overflow flag
    }   
// change the x data from 9 bit to 8 bit 2's complement
    mx = mx & 0x7f; // mask off 8th bit
    if ((0x10 & mstat) == 0x10) {   // move the sign into 
      mx = 0x80 | mx;              // the 8th bit position
    } 
// change the y data from 9 bit to 8 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of touchpad.move function
    my = my & 0x7f; // mask off 8th bit
    if ((0x20 & mstat) == 0x20) {   // move the sign into 
      my = 0x80 | my;              // the 8th bit position
    } 
    my = (~my + 0x01); // change the sign of y data by taking the 2's complement (invert and add 1)
// zero out mx and my if over_flow or touchpad_error is set
    if ((over_flow) || (touchpad_error)) { 
      mx = 0x00;       // data is garbage so zero it out
      my = 0x00;
    } 
// send the x and y data back via usb if either one is non-zero
    if ((mx != 0x00) || (my != 0x00)) {
      Mouse.move(mx,my);
    }
//
// send the touchpad left and right button status over usb if no error
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
// Determine if the left or right touch pad buttons have changed since last polling cycle
    button_change = (left_button ^ old_left_button) | (right_button ^ old_right_button);
// Don't send button status if there's no change since last time. 
    if (button_change){
      Mouse.set_buttons(left_button, 0, right_button); // send button status
    }
    old_left_button = left_button; // remember new button status for next polling cycle
    old_right_button = right_button;
    }
  else  {
    digitalWrite(ERROR_LED, HIGH);
  }
//
// **************************************End of touchpad routine***********************************
// 
  delay(30);  // wait 30ms before repeating next polling cycle
}
