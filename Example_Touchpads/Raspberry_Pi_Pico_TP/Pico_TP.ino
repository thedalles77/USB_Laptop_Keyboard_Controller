/* Copyright 2024 Frank Adams
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
// This code allows a Raspberry Pi Pico to control a PS/2 Touchpad (herein refered to as TP)
// The tp is set up in remote mode so it can be polled at a regular rate.
// The PS/2 relative movement and button pushes are sent as a USB mouse to a host computer .
// I initially used: #include USBMouse.h and USBMouse Mouse but I want to use keyboard functions in future code.
// This link talks about using #include USBMouseKeyboard.h and USBMouseKeyboard MouseKeyboard when you want both to work.
// https://github.com/arduino/ArduinoCore-mbed/issues/412
// 
// The Pico's built in watchdog timer is coded based on the example at:
// https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#example155
// 
// You must cycle power (unplug and plug in the USB cable) after programming the Pico to clear the watchdog timer variables.
// 
#include "PluggableUSBHID.h"
#include "USBMouseKeyboard.h"    
USBMouseKeyboard MouseKeyboard;

#include "hardware/watchdog.h"
//
#define TP_DATA 1 // connect the tp data to this Pico I/O pin
#define TP_CLK 3 // connect the tp clock to this Pico I/O pin
#define TP_ERROR 14 // // test point for monitoring if a watchdog timeout has occured
//
// Function to set a pin to high impedance (acts like open drain output)
void go_z(int pin)
{
  pinMode(pin, INPUT); // 
}
//
// Function to set a pin as an input with a pullup
void go_pu(int pin)
{
  pinMode(pin, INPUT_PULLUP);
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
  char odd_parity = 0; // clear parity bit count
// Enable the bus by floating the clock and data
  go_z(TP_CLK); //
  go_z(TP_DATA); //
  delayMicroseconds(180); // wait before requesting the bus. 180 measures 250us.
  go_0(TP_CLK); //   Send the Clock line low to request to transmit data
  delayMicroseconds(75); // wait for at least 100 microseconds per bus spec.  75 measures over 100us.
// Request to Send
  go_0(TP_DATA); //  Send the Data line low, then let clock go high to request to send  
  go_z(TP_CLK); //   
// watch for clock to go high
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// watch for clock to go low    
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
// send the 8 bits of data, lsb first by looping 8 times
  for (int j=0; j<8; j++) {
    if (send_data & 1) {  //check if lsb is set
      go_z(TP_DATA); // send a 1 to TP
      odd_parity = odd_parity + 1; // keep running total of 1's sent
    }
    else {
      go_0(TP_DATA); // send a 0 to TP
    }
// watch for clock to go high
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    }
// watch for clock to go low
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    }  
    send_data = send_data >> 1; // shift data right by 1 to prepare for next loop
  }
// finished looping on data bits
// send the parity bit based on running count of 1's (only need the lsb to tell if it's odd or even)
  if (odd_parity & 1) {  //check if lsb of parity is set
    go_0(TP_DATA); // already odd so send a 0 to TP
  }
  else {
    go_z(TP_DATA); // send a 1 to TP to make parity odd
  }   
//  watch for clock to go high
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
//  watch for clock to go low
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
// stop bit
  go_z(TP_DATA); //  Release the Data line so it goes high as the stop bit
//  watch for clock to go high
while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
}
//  watch for clock to go low
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
//  Ack bit
  if (digitalRead(TP_DATA)) { // future error handler can go here if Ack bit is high (bad) 
  }
//  watch for clock to go high
  while (digitalRead(TP_CLK) == LOW) { // loop if clock is low 
  }
// Write to touchpad is complete - inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
}
//
// Function to get a byte of data from the touchpad
//
char tp_read(void)
{
  char rcv_data = 0; // initialize to zero
  char mask = 1; // shift a 1 across the 8 bits to select where to load the data
  char rcv_parity = 0; // count the ones received
  go_z(TP_CLK); // release the clock
  go_z(TP_DATA); // release the data
//  watch for clock to go low
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA)) { // Start bit s/b low from tp
  // start bit not correct - put future error handler here
  }  
//  watch for clock to go high
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// receive the 8 bits of data, lsb first by looping 8 times
  for (int k=0; k<8; k++) {  
//  watch for clock to go low
    while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
    }
    if (digitalRead(TP_DATA)) { // check if data is high
      rcv_data = rcv_data | mask; // set the appropriate bit in the rcv data
      rcv_parity++; // increment the parity bit counter
    }
    mask = mask << 1; //shift the mask left 1 bit
//  watch for clock to go high
    while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
    }
  }
// finished looping on data bits
// receive the parity bit next
// watch for clock to go low
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
// read the data and if high, add it to the running parity bit counter
  if (digitalRead(TP_DATA))  { // check if parity bit is high
    rcv_parity++; // increment the parity bit counter
  }
  rcv_parity = rcv_parity & 1; // only need the lsb to tell if the count is odd
  if (rcv_parity == 0) { // check for bad (even) parity
  // future parity error handler goes here
  } 
// watch for clock to go high
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// stop bit
// watch for clock to go low
  while (digitalRead(TP_CLK) == HIGH) { // loop until the clock goes low
  }
  if (digitalRead(TP_DATA) == LOW) { // check if stop bit is bad (low)
  // future error handler here
  }
// watch for clock to go high
  while (digitalRead(TP_CLK) == LOW) { // loop until the clock goes high
  }
// Inhibit the bus so the tp only talks when we're listening
  go_0(TP_CLK);
  return rcv_data; // pass the received data back
}
//
void touchpad_init()
{
  go_z(TP_CLK); // float the clock and data to touchpad
  go_z(TP_DATA);
  
  delay(1000); // wait 1 second so tp can run its power up self diagnostic  
  watchdog_update(); // reloads watchdog timer with initial value
  //  Send reset command to touchpad
  tp_write(0xff);
                   
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler here                
  }
                      
  delay(1000); // wait 1 second so tp can run its self diagnostic                    
  watchdog_update(); // reloads watchdog timer with initial value                     
  //  verify proper response (0xaa00) from tp
  if (tp_read() == 0xaa) { // high byte - verify basic assurance test passed
  // future error handler here    
  } 
        
  if (tp_read() == 0x00) { // low byte - verify basic assurance test passed 
  // future error handler here    
  }
         
  //  Send touchpad disable code so the other registers can be safely setup
  
  tp_write(0xf5);  // tp disable 
             
  if (tp_read() == 0xfa) { // verify correct ack byte
  // future error handle here           
  }
                                                   
  // Load Mode Byte with 0x40 using the following special sequence from paragraph 3.5.2 of Synaptics spec. 510-000080
  // /https://content.instructables.com/FYM/6QKB/IB8QCVZX/FYM6QKBIB8QCVZX.pdf
  // Mode Byte = 0x40 is Relative Mode, 80 packets per second.
  // The special sequence involves setting the resolution four times followed by setting the sample rate.
// #1 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }  
  tp_write(0x01);  // to 1 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }
// #2 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }
// #3 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }
// #4 set resolution
  tp_write(0xe8);  // set resolution 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }  
  tp_write(0x00);  // to zero 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }
// Set sample rate
  tp_write(0xf3);  // set sample rate 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }  
  tp_write(0x14);  // to 14 hex 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  } 
  // this concludes the special sequence to load the Mode byte with 0x40
  //
  // set the resolution (for real)
  tp_write(0xe8); //  Sending resolution command
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  } 
  tp_write(0x03); // value of 0x03 = 8 counts/mm resolution (default is 4 counts/mm)
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  } 
  // set the sample rate 
  tp_write(0xf3); //  Sending sample rate command
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  } 
  tp_write(0x28); // 0x28 = 40 samples per second, the default value used for Synaptics TP
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }              
  //  Sending remote mode code so the touchpad will send data only when polled (instead of stream mode)
  tp_write(0xf0);  // remote mode 
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler
  }
//  Sending touchpad enable code 
  tp_write(0xf4);  // f4 = tp enable 
  if (tp_read() != 0xfa) { // verify correct ack byte
  //  future error handler here
  }
//
}
// ************************************Begin Routine*********************************************************
void setup()
{
  pinMode(TP_ERROR, OUTPUT); //
  watchdog_enable(1500, 1); // start the watchdog timer with 1.5 second value
  touchpad_init(); // reset touchpad, then set it's resolution and put it in remote mode
// For debug, drive an output pin high if the watchdog timer ever causes a reboot
  if (watchdog_caused_reboot()) {
    digitalWrite(TP_ERROR, HIGH);
  } 
  else {
    digitalWrite(TP_ERROR, LOW);
  }
//
}
// declare and initialize variables  
  char mstat; // touchpad status reg = Y overflow, X overflow, Y sign bit, X sign bit, Always 1, Middle Btn, Right Btn, Left Btn
  char mx; // touchpad x movement = 8 data bits. The sign bit is in the status register to 
           // make a 9 bit 2's complement value. Left to right on the touchpad gives a positive value, (MouseKeyboard.move is same) 
  char my; // touchpad y movement also 8 bits plus sign. Movement bottom to top gives a positive value, (MouseKeyboard.move is opposite)
  int16_t mx16; //need 16 bit x and y movement values for MouseKeyboard.move 
  int16_t my16;
  boolean over_flow; // set if x or y movement values are bad due to overflow
  boolean left_button = 0; // on/off variable for left button = bit 0 of mstat
  boolean right_button = 0; // on/off variable for right button = bit 1 of mstat
  boolean old_left_button = 0; // on/off variable for left button status from the previous polling cycle
  boolean old_right_button = 0; // on/off variable for right button status from the previous polling cycle
  boolean older_left_button = 0; // on/off variable for left button status from two previous polling cycles back
  boolean older_right_button = 0; // on/off variable for right button status from two previous polling cycles back
// 
// ************************************Main Loop***************************************************************
void loop() {
// poll the touchpad for new movement data
  over_flow = 0; // assume no overflow until status is received 
  tp_write(0xeb);  // request data
  if (tp_read() != 0xfa) { // verify correct ack byte
  // future error handler here        
  }
  mstat = tp_read(); // save into status variable
  mx = tp_read(); // save into 8 bit x variable
  my = tp_read(); // save into 8 bit y variable
  if (((0x80 & mstat) == 0x80) || ((0x40 & mstat) == 0x40))  {   // x or y overflow bits set?
     over_flow = 1; // set the overflow flag
  }   
// change the x data from 9 bit to 16 bit 2's complement
  mx16 = 0x00ff & mx; // move the low 8 bits into the 16 bit variable
  if ((0x10 & mstat) == 0x10) {   // is x sign bit set? 
    mx16 = 0xff00 + mx16; // make the high 8 bits 1's (including the sign bit)             
  } 
// change the y data from 9 bit to 16 bit 2's complement and then take the 2's complement 
// because y movement on ps/2 format is opposite of MouseKeyboard.move function
  my16 = 0xff & my; // move the low 8 bits into the 16 bit variable
  if ((0x20 & mstat) == 0x20) {   // is y sign bit set? 
    my16 = 0xff00 + my16; // make the high 8 bits 1's (including the sign bit) 
  } 
  my16 = (~my16 + 0x0001); // change the sign of y data by taking the 2's complement (invert and add 1)

// zero out mx and my if over_flow is set
  if (over_flow) {
    mx16 = 0x0000;       // data is garbage so zero it out 
    my16 = 0x0000;
  } 
// Send the x and y data back via usb if either one is non-zero.
// This avoids constantly sending USB data when there is no movement.
  if ((mx16 != 0x0000) || (my16 != 0x0000)) {
    MouseKeyboard.move(mx16,my16); 
  }
//
// Send the touchpad left and right button status over usb if a change has occured (push or release).
// Two reads in a row must give the same result in order to give a press or release (noise filter) 
  if ((0x01 & mstat) == 0x01) {   // if left button set 
    left_button = 1;   
  }
  else {   // clear left button
    left_button = 0;   
  }
  if (left_button && old_left_button && !older_left_button) { // true if left button was pushed
    MouseKeyboard.press(MOUSE_LEFT);
  }
  else if (!left_button && !old_left_button && older_left_button) { // true if left button was released
    MouseKeyboard.release(MOUSE_LEFT);
  }
  if ((0x02 & mstat) == 0x02) {   // if right button set 
    right_button = 1;   
  } 
  else {   // clear right button
    right_button = 0;  
  }
  if (right_button && old_right_button && !older_right_button) { // true if right button was pushed
    MouseKeyboard.press(MOUSE_RIGHT);
  }
  else if (!right_button && !old_right_button && older_right_button) { // true if right button was just released
    MouseKeyboard.release(MOUSE_RIGHT);
  }
//
  older_left_button = old_left_button; // save button status for next polling cycle
  older_right_button = old_right_button;
  old_left_button = left_button; 
  old_right_button = right_button;
//
// **************************************End of touchpad routine***********************************
// 
  watchdog_update(); // reloads watchdog timer with initial value
//
  delay(25);  // wait before repeating next polling cycle
}
