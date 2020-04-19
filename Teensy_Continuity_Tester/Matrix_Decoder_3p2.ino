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
// It will determine how a laptop keyboard matrix is wired using a Teensy 3.2 on an FPC daughterboard.
// Open an editor on a computer and load or create a file that lists every key
// on the laptop keyboard that will be tested. Connect the FPC cable of the test keyboard 
// to the Teensy/FPC board. Connect a USB cable from the Teensy to the computer.
// Wait a few seconds for the computer to see the Teensy as a keyboard. If numbers are reported on the screen
// before any keys are pressed, these pin numbers are shorted together and must be fixed. 
// Press each key one by one on the test keyboard as listed on the editor screen. When a key 
// is pressed on the test keyboard, the program detects which two pins on the FPC connector 
// were connected. Those two pin numbers are sent over USB (separated by a TAB) and displayed 
// on the editor. After sending the numbers, a DOWN ARROW is sent over USB to prepare for
// the next key. Once all keys on the test keyboard have been pressed, the file in
// the editor can be saved to create a row-column matrix.
//
// If your keyboard has diodes, you must pay attention to the order of the two pins that are reported by the Teensy. The code performs 
// a bottom up test first, followed by a top down test so that one of the two tests will forward bias the diode. 
// The first pin reported over USB is the cathode side and the second pin is the anode side. The diode direction must be taken into 
// account when programming the TMK or Teensyduino keyboard routine.   
//
// Revision History
// Rev 1.00 - Nov 18, 2018 - Original Release
// Rev 1.1 - April 19, 2020 - Use min_pin in bottom up loop
//
// Load an array with the Teensy 3.2 I/O numbers that correspond to FPC pins 1 thru 34.
 int con_pin[] = {23, 0, 22, 1, 21, 2, 20, 3, 19, 4, 18, 5, 17, 6, 24, 7, 25, 8, 33, 9, 26, 10, 27, 11, 28, 12, 32, 31, 30, 29, 16, 15, 14, 13};
//
// Define maximum and minimum pin numbers that will be tested.
// max_pin is usually set to the FPC connector size. min_pin is usually set to 1. The routine will start testing at pin 1 and go up to the max pin size.
// The max and min pin values can be adjusted to exclude testing the FPC traces at the edges if they are reported as shorted. An example would be if pin 1
// and pin 34 are both grounds. They will be reported as tied together but they are not needed by the key matrix. In this case, set the 
// min_pin to 2 and the max_pin to 33.
//  
int max_pin = 33; // the keyboard FPC connector pin count. If set to 34, unsolder the LED or the code won't work
int min_pin = 1; // the first pin to be tested on the FPC connector (usually pin 1)
//
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
void usb_num(int num)  // the numbers 0 thru 33 are sent over usb as 1 thru 34 
{
  switch (num) {
    case 0:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      break;
    case 1:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      break;
    case 2:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      break;
    case 3:    
      Keyboard.set_key1(key_4);
      Keyboard.send_now();
      break;
    case 4:    
      Keyboard.set_key1(key_5);
      Keyboard.send_now();
      break;
    case 5:    
      Keyboard.set_key1(key_6);
      Keyboard.send_now();
      break;
    case 6:    
      Keyboard.set_key1(key_7);
      Keyboard.send_now();
      break;
    case 7:    
      Keyboard.set_key1(key_8);
      Keyboard.send_now();
      break;
    case 8:    
      Keyboard.set_key1(key_9);
      Keyboard.send_now();
      break;
    case 9:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_0);
      Keyboard.send_now();
      break;
    case 10:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key2(key_1);
      Keyboard.send_now();
      break;
    case 11:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_2);
      Keyboard.send_now();
      break;
    case 12:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_3);
      Keyboard.send_now();
      break;
    case 13:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_4);
      Keyboard.send_now();
      break;
    case 14:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_5);
      Keyboard.send_now();
      break;
    case 15:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_6);
      Keyboard.send_now();
      break;
    case 16:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_7);
      Keyboard.send_now();
      break;
    case 17:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_8);
      Keyboard.send_now();
      break;
    case 18:    
      Keyboard.set_key1(key_1);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_9);
      Keyboard.send_now();
      break;
    case 19:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_0);
      Keyboard.send_now();
      break;
    case 20:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_1);
      Keyboard.send_now();
      break;
    case 21:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key2(key_2);
      Keyboard.send_now();
      break;
    case 22:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_3);
      Keyboard.send_now();
      break;
    case 23:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_4);
      Keyboard.send_now();
      break;
    case 24:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_5);
      Keyboard.send_now();
      break;
    case 25:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_6);
      Keyboard.send_now();
      break;
    case 26:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_7);
      Keyboard.send_now();
      break;  
    case 27:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_8);
      Keyboard.send_now();
      break;  
    case 28:    
      Keyboard.set_key1(key_2);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_9);
      Keyboard.send_now();
      break; 
    case 29:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_0);
      Keyboard.send_now();
      break; 
    case 30:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_1);
      Keyboard.send_now();
      break;  
    case 31:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_2);
      Keyboard.send_now();
      break; 
    case 32:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key1(0);
      Keyboard.send_now();
      delay(50);
      Keyboard.set_key2(key_3);
      Keyboard.send_now();
      break;
    case 33:    
      Keyboard.set_key1(key_3);
      Keyboard.send_now();
      delay(20);
      Keyboard.set_key2(key_4);
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
  for (int k = 0; k < max_pin; k++) {  // loop thru all connector pins 
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
// ***********Bottom up Test************ 
//
  for (int i=min_pin-1; i<max_pin-1; i++) {   // outer loop pin (min_pin-1 is typically = 0)
    go_0(con_pin[i]); // make the outer loop pin an output and send this pin low
    for (int j=i+1; j<max_pin; j++) {   // inner loop pin
      delayMicroseconds(10); // give time to let the signals settle out
      if (!digitalRead(con_pin[j])) {  // check for connection between inner and outer pins
        usb_num(i); // send outer loop pin number over usb
        usb_num(j); // send inner loop pin number over usb
        down_arrow(); // send a down arrow over usb
        while(!digitalRead(con_pin[j])) {  // wait until key is released 
           ;                              // if 2 pins are shorted, the code will hang here                                            
        }                                                  
      }
    }
  go_z(con_pin[i]); // return the outer loop pin to float with pullup        
  }
//
// *********Top down Test***********
//
  for (int p=max_pin-1; p>min_pin-1; p--) {   // outer loop pin
    go_0(con_pin[p]); // make the outer loop pin an output and send this pin low
    for (int r=p-1; r>=min_pin-1; r--) {   // inner loop pin
      delayMicroseconds(10); // give time to let the signals settle out
      if (!digitalRead(con_pin[r])) {  // check for connection between inner and outer pins
        usb_num(p); // send outer loop pin number over usb
        usb_num(r); // send inner loop pin number over usb
        down_arrow(); // send a down arrow over usb
        while(!digitalRead(con_pin[r])) {  // wait until key is released 
           ;                                               
        }                                                  
      }
    }
  go_z(con_pin[p]); // return the outer loop pin to float with pullup
  }
//
delay(25);  // overall keyboard scan rate is about 30 milliseconds
//
}
