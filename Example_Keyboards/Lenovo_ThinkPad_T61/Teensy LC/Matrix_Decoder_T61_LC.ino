//
// This software is in the public domain.
// It will determine how a laptop keyboard matrix is wired using a Teensy LC on an FPC T61 daughterboard.
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
// Rev 1.00 - Nov 20, 2018 - Original Release
//
// FPC 02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,20,22,24,26,28,30,32
// I/O 16,04,08,21,17,25,07,22,18,24,06,23,19,03,05,02,20,09,15,10,14,11,26,12
// Load an array with the Teensy LC I/O numbers that correspond to the FPC pins
 int con_pin[] = {16,4,8,21,17,25,7,22,18,24,6,23,19,3,5,2,20,9,15,10,14,11,26,12};
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
  for (int k = 0; k < 24; k++) {  // loop thru all connector pins 
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
  for (int i=0; i<23; i++) {   // outer loop index
    go_0(con_pin[i]); // make the outer loop pin an output and send this pin low
    for (int j=i+1; j<24; j++) {   // inner loop index
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
