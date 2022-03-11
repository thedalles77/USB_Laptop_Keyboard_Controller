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
// This software reads the 6 logic signals from a Grid 1550 laptop "mouse" and converts it to USB.
// The Grid 1550 laptop has a shaft in place of a touchpad. The shaft is rolled forward or backwards
// which moves the cursor up or down. Likewise the entire shaft can be slid left or right to move the cursor left or right.
// There are two buttons on the left of the shaft and two buttons on the right of the shaft.
// The two buttons closest to the shaft act like normal left mouse buttons. 
// The two buttons furthest from the shaft are right mouse buttons. 
// Pins 3 and 4 are active for up/down movement. Pins 5 and 6 are active for left/right movement. 
// The pin pairs use a rotational encoding scheme that is described in the following wiki page:
// https://en.wikipedia.org/wiki/Rotary_encoder
//
// I/O pins 0 & 1 are used to enable/disable the mouse and to adjust its speed
// I/O 1    I/O 0     Result
// High     High      Count by 1
// High     Low       Count by 2
// Low      High      Count by 3
// Low      Low       Mouse disabled
// 
// Revision History
// Rev 1.0 - Nov 3, 2019 - Original Release
// Rev 1.1 - Nov 4, 2019 - Fixed swapped mouse buttons
// Rev 1.2 - Nov 4, 2019 - Added speed controls
//
#include <TimerOne.h> // used for making a 24msec timer
//
#define SPEED_PIN0 0 // Input with pullup - Can be driven by the Teensy controlling the keyboard
#define SPEED_PIN1 1 // Input with pullup - Can be driven by the Teensy controlling the keyboard
// Connect the Teensy I/O's 3 thru 8 to the Grid mouse connector pins 3 thru 8
#define MOUSE_PIN3 3 // Left/Right Encoder signal A
#define MOUSE_PIN4 4 // Left/Right Encoder signal B
#define MOUSE_PIN5 5 // Up/Down Encoder signal A
#define MOUSE_PIN6 6 // Up/Down Encoder signal B
#define MOUSE_PIN7 7 // Right Mouse Button
#define MOUSE_PIN8 8 // Left Mouse Button
//
// Declare and initialize variables  
  boolean left_button = 0; // on/off variable for left button, 1 = pushed
  boolean right_button = 0; // on/off variable for right button, 1 = pushed
  boolean old_left_button = 0; // on/off variable for left button from the previous cycle
  boolean old_right_button = 0; // on/off variable for right button from the previous cycle
  boolean button_change = 0; // Shows when the left or right buttons have changed, 1 = change
  volatile int8_t x_count = 0; // left/right movement. 8 bit signed. Volatile because used in ISR
  volatile int8_t y_count = 0; // up or down movement. 8 bit signed. Volatile because used in ISR
  boolean x_A; // holds the state of pin 3
  boolean x_A_Last; // holds the previous state of pin 3
  boolean y_A; // holds the state of pin 5
  boolean y_A_Last; // holds the previous state of pin 5
  boolean mouse_on = 1; // high when mouse is turned on, low when turned off
  boolean speed0 = 1; // result of reading pin 0
  boolean speed1 = 1; // result of reading pin 1
  int8_t count_by = 1; // value to add or substract from x & y counters (1, 2, or 3 depending on SPEED_PIN0 & 1)
//
// USB Interrupt Service Routine (ISR) Activates every 24msec when timer1 ticks  
void sendUSB(void) { 
// send the x and y data back to the host via usb if either counter is non-zero. 
// if the mouse is turned off, the counters can't increment so they will both be zero.
    if ((x_count != 0x00) || (y_count != 0x00)) {
      Mouse.move(x_count,y_count);
      x_count = 0; // clear the x and y counters
      y_count = 0;
    }
// read the touchpad left and right buttons
    if (!digitalRead(MOUSE_PIN7)) {   // check if right button is low (low = pushed) 
      right_button = 1;   // save state of button
    }
    else {   // clear right button
      right_button = 0;   // save state of button
    }
    if (!digitalRead(MOUSE_PIN8)) {   // check if left button is low (low = pushed) 
      left_button = 1;  // save state of button 
    } 
    else {   // clear left button
      left_button = 0;  // save state of button
    }
// Determine if the left or right mouse buttons have changed (using XOR) since the last cycle 
    button_change = (left_button ^ old_left_button) | (right_button ^ old_right_button);
// Don't send button status if there's no change since last time or if the mouse is turned off. 
    if (button_change && mouse_on){
      Mouse.set_buttons(left_button, 0, right_button); // send button status over USB
    }
    old_left_button = left_button; // remember button status for the next cycle
    old_right_button = right_button;
}
// *****************Setup the timer and input pins***********************************************
void setup() {
  Timer1.initialize(24000); // 24msec timer
  Timer1.attachInterrupt(sendUSB); // sendUSB function will run every 24 milliseconds
  pinMode(SPEED_PIN0, INPUT_PULLUP); // both speed controls are inputs with pullups
  pinMode(SPEED_PIN1, INPUT_PULLUP); // if left floating, both will be 1's to give count_by 1
  pinMode(MOUSE_PIN3, INPUT); // teensy I/O 3 is an input for left/right signal A
  pinMode(MOUSE_PIN4, INPUT); // teensy I/O 4 is an input for left/right signal B
  pinMode(MOUSE_PIN5, INPUT); // teensy I/O 5 is an input for up/down signal A
  pinMode(MOUSE_PIN6, INPUT); // teensy I/O 6 is an input for up/down signal B
  pinMode(MOUSE_PIN7, INPUT); // teensy I/O 7 is an input for right mouse button
  pinMode(MOUSE_PIN8, INPUT); // teensy I/O 8 is an input for left mouse button
  x_A_Last = digitalRead(MOUSE_PIN3); // save logic state of pin 3
  y_A_Last = digitalRead(MOUSE_PIN5); // save logic state of pin 5
}
// *********Main Loop constantly watches for activity on pins 3 and 5 plus status on pins 0 & 1********* 
void loop() {
// adjust the x counter if pin 3 changes (i.e. has an up or down edge)
   x_A = digitalRead(MOUSE_PIN3); // Read the current state of pin 3
   if (x_A != x_A_Last){     // If pin 3 is different than the previous pin 3, an edge has occured.
     noInterrupts(); // disable the timer interrupt while modifying the x counter 
     if (digitalRead(MOUSE_PIN4) != x_A) { // see if pin 4 equals pin 3
       x_count = x_count + count_by; // pin 3 is ahead of pin 4 so move the cursor to the right by increasing the counter
     } 
     else {
       x_count = x_count - count_by; // pin 3 is behind pin 4 so move the cursor to the left by decreasing the counter
     }
     interrupts(); // enable the timer interrupt
   } 
   x_A_Last = x_A; // save the state of pin 3 for the next loop
// adjust the y counter if pin 5 has an up or down edge
   y_A = digitalRead(MOUSE_PIN5); // Reads the current state of pin 5
   if (y_A != y_A_Last){     // If pin 5 is different than the previous pin 5, an edge has occured.
     noInterrupts(); // disable the timer interrupt while modifying the y counter
     if (digitalRead(MOUSE_PIN6) != y_A) { // see if pin 6 equals pin 5
       y_count = y_count + count_by; // pin 5 is ahead of pin 6 so move the cursor down by increasing the counter
     } 
     else {
       y_count = y_count - count_by; // pin 5 is behind pin 6 so move the cursor up by decreasing the counter
     }
     interrupts(); // enable the timer interrupt
   } 
   y_A_Last = y_A; // save the state of pin 5 for the next loop   
// Read speed pins 0 & 1 to adjust count and on/off control
   speed0 = digitalRead(SPEED_PIN0);
   speed1 = digitalRead(SPEED_PIN1);
   if (speed0 && speed1) { // this is the high & high case
     count_by = 1; // slow speed movement
     mouse_on = 1; // turn mouse on
   }
   else if (!speed0 && speed1) { // this is the low & high case
     count_by = 2; // middle speed movement
     mouse_on = 1; // turn mouse on
   }
   else if (speed0 && !speed1){ // this is the high & low case
     count_by = 3; // high speed movement
     mouse_on = 1; // turn mouse on
   }
   else { // this is the low & low case
     count_by = 0; // no movement
     mouse_on = 0; // turn mouse off    
   }
}
