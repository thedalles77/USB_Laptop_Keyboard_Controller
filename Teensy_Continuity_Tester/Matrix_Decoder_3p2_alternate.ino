/*
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
// Revision History
// Rev 1.00 - April 18, 2020 - Original Release - modified from Matrix_Decoder_T61

// This software is an alternate method to scan the FPC connector pins. 
// If you run the original Matrix_Decoder_3p2 code and find 2 interior pins are always tied together, you cannot
// exclude those pins by changing the min_pin or the max_pin values. To handle 2 interior pins that are tied together, 
// this program only scans the Teensy I/O's in the con_pin[] matrix defined below.
//
// ********Note that this program reports the Teensy I/O numbers over USB, not the FPC pin numbers*****************

// The FPC pins are connected to the Teensy I/O's as follows:
// FPC 01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34 
// I/O 23,00,22,01,21,02,20,03,19,04,18,05,17,06,24,07,25,08,33,09,26,10,27,11,28,12,32,31,30,29,16,15,14,13 

// Load the con_pin array with the Teensy 3.2 I/O numbers that correspond to the FPC pins from left to right that you want to scan. 
// Do not include the I/O numbers that correspond to the FPC pins that are shorted together.
// If you have a 34 pin cable, you need to free up I/O 13 by removing the LED on the Teensy board, then add ,13 to the array. 
int con_pin[] = {23,0,22,1,21,2,20,3,19,4,18,5,17,6,24,7,25,8,33,9,26,10,27,11,28,12,32,31,30,29,16,15,14}; // Add ,13 if no LED
//
int total = 33; // The total number of I/O's listed in con_pin[]

// load the key codes used in sending usb numbers, tab, and down arrow
int key_1 = KEY_1; 
int key_2 = KEY_2;
int key_3 = KEY_3;
int key_4 = KEY_4;
int key_5 = KEY_5;
int key_6 = KEY_6;
int key_7 = KEY_7;
int key_8 = KEY_8;
int key_9 = KEY_9;
int key_0 = KEY_0;
int key_tab = KEY_TAB;
int key_down = KEY_DOWN;
//
// Function to set a pin as an input with a pullup so it's high unless grounded by a key press
void go_z(int pin)
{
  pinMode(pin, INPUT_PULLUP);
  digitalWrite(pin, HIGH);
}

// Function to set a pin as an output and drive it to a logic low (0 volts)
void go_0(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}

// Function to send numbers over USB for display on an editor
void usb_num(int num)  // the numbers 0 thru 33 are sent over usb as 0 thru 33 
{
  switch (num) {
    case 0:    
      Keyboard.set_key1(key_0);
      Keyboard.send_now();
      break;
    case 1:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      break;
    case 2:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      break;
    case 3:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      break;
    case 4:    
      Keyboard.set_key1(key_4);
      Keyboard.send_now();
      break;
    case 5:    
      Keyboard.set_key1(key_5);
      Keyboard.send_now();
      break;
    case 6:    
      Keyboard.set_key1(key_6);
      Keyboard.send_now();
      break;
    case 7:    
      Keyboard.set_key1(key_7);
      Keyboard.send_now();
      break;
    case 8:    
      Keyboard.set_key1(key_8);
      Keyboard.send_now();
      break;
    case 9:    
      Keyboard.set_key1(key_9);
      Keyboard.send_now();
      break;
    case 10:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_0);
      Keyboard.send_now();
      break;
    case 11:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key2(key_1);
      Keyboard.send_now();
      break;
    case 12:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_2);
      Keyboard.send_now();
      break;
    case 13:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_3);
      Keyboard.send_now();
      break;
    case 14:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_4);
      Keyboard.send_now();
      break;
    case 15:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_5);
      Keyboard.send_now();
      break;
    case 16:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_6);
      Keyboard.send_now();
      break;
    case 17:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_7);
      Keyboard.send_now();
      break;
    case 18:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_8);
      Keyboard.send_now();
      break;
    case 19:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_9);
      Keyboard.send_now();
      break;
    case 20:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_0);
      Keyboard.send_now();
      break;
    case 21:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_1);
      Keyboard.send_now();
      break;
    case 22:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key2(key_2);
      Keyboard.send_now();
      break;
    case 23:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_3);
      Keyboard.send_now();
      break;
    case 24:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_4);
      Keyboard.send_now();
      break;
    case 25:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_5);
      Keyboard.send_now();
      break;
    case 26:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_6);
      Keyboard.send_now();
      break;
    case 27:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_7);
      Keyboard.send_now();
      break;  
    case 28:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_8);
      Keyboard.send_now();
      break;  
    case 29:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_9);
      Keyboard.send_now();
      break; 
    case 30:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_0);
      Keyboard.send_now();
      break; 
    case 31:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_1);
      Keyboard.send_now();
      break;  
    case 32:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_2);
      Keyboard.send_now();
      break; 
    case 33:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key2(key_3);
      Keyboard.send_now();
      break;
  }
  delay(20); 
  Keyboard.set_key1(0); // clear out the key slots
  Keyboard.set_key2(0);
  Keyboard.send_now();
  delay(20);
  Keyboard.set_key1(key_tab); // Tab over to position for next number
  Keyboard.send_now();
  delay(20); 
  Keyboard.set_key1(0); // clear out the tab from the slot
  Keyboard.send_now();
  delay(20);
}

// Function to send a down arrow over usb to position for the next key
void down_arrow(void) {
  Keyboard.set_key1(key_down); // send a down arrow
  Keyboard.send_now();
  delay(20); 
  Keyboard.set_key1(0); // release the down arrow
  Keyboard.send_now();
}

// --------------------------------------------------Setup-----------------------------------
void setup() {
  for (int k = 0; k < total; k++) {  // loop thru all row-column pins 
    go_z(con_pin[k]); // set each pin as an input with a pullup
  }
  delay(15000);  // Wait for the host to connect to the Teensy as a keyboard. If 2 pins are shorted,
              // you want the host to be ready to receive the pin numbers.
}
//
// -------------------------------------------Main Loop--------------------------------------
// 
void loop() {  
//
//
  for (int i=0; i<total-1; i++) {   // outer loop index
    go_0(con_pin[i]); // make the outer loop pin an output and send this pin low
    for (int j=i+1; j<total; j++) {   // inner loop index
      delayMicroseconds(10); // give time to let the signals settle out
      if (!digitalRead(con_pin[j])) {  // check for connection between inner and outer pins
        usb_num(con_pin[i]); // send outer loop I/O number over usb
        usb_num(con_pin[j]); // send inner loop I/O number over usb
        down_arrow(); // send a down arrow over usb
        while(!digitalRead(con_pin[j])) {  // wait until key is released 
           ;                              // if 2 pins are shorted, the code will hang here                                            
        }                                                  
      }
    }
  go_z(con_pin[i]); // return the outer loop pin to float with pullup        
  }
//
//
delay(25);  // overall keyboard scan rate is about 30 milliseconds
//
}
