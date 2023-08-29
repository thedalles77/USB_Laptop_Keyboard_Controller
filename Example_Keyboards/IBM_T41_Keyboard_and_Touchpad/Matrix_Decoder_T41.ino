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
// this software will determine how a laptop keyboard matrix is wired using a Teensy 4.0 on an T41 daughterboard.
// Open an editor on a computer and load or create a file that lists every key
// on the laptop keyboard that will be tested. Connect the FPC cable of the test keyboard 
// to the Teensy/FPC board. Connect a USB cable from the Teensy to the computer.
// Wait a few seconds for the computer to see the Teensy as a keyboard. If numbers are reported on the screen
// before any keys are pressed, these I/O numbers are shorted together and must be fixed. 
// Press each key one by one on the test keyboard as listed on the editor screen. When a key 
// is pressed on the test keyboard, the program detects which Teensy I/O's  
// were connected. Those two I/O's are sent over USB (separated by a TAB) and displayed 
// on the editor. After sending the numbers, a DOWN ARROW is sent over USB to prepare for
// the next key. Once all keys on the test keyboard have been pressed, the file in
// the editor can be saved to create a row-column matrix.
//
// Revision History
// Rev 1.00 - Aug 10, 2023 - Original Release
//
// FPC 02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,20,22,24,26,28,30,32
// I/O 33,32,08,10,31,30,07,11,29,28,06,12,27,26,05,24,25,04,20,03,21,02,22,01
// Load an array with the Teensy 4.0 I/O numbers that correspond to the FPC pins
 int io_pin[] = {33,32,8,10,31,30,7,11,29,28,6,12,27,26,5,24,25,4,20,3,21,2,22,1};
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
  for (int k = 0; k < 24; k++) {  // loop thru all connected Teensy IO's 
    go_z(io_pin[k]); // set each I/O pin as an input with a pullup
  }
  delay(15000);  // Wait for the host to connect to the Teensy as a keyboard. If 2 I/O pins are shorted,
              // you want the host to be ready to receive the numbers.
}
//
// -------------------------------------------Main Loop--------------------------------------
// 
void loop() {  
//
//
  for (int i=0; i<23; i++) {   // outer loop index
    go_0(io_pin[i]); // make the outer loop I/O pin an output and send it low
    for (int j=i+1; j<24; j++) {   // inner loop index
      delayMicroseconds(10); // give time to let the signals settle out
      if (!digitalRead(io_pin[j])) {  // check for connection between inner and outer I/O pins
        usb_num(io_pin[i]); // send outer loop I/O number over usb
        usb_num(io_pin[j]); // send inner loop I/O number over usb
        down_arrow(); // send a down arrow over usb
        while(!digitalRead(io_pin[j])) {  // wait until key is released 
           ;                              // if 2 I/O pins are shorted, the code will hang here                                            
        }                                                  
      }
    }
  go_z(io_pin[i]); // return the outer loop I/O pin to float with pullup        
  }
//
//
delay(25);  // overall keyboard scan rate is about 30 milliseconds
//
}