// This decoder software is for a Gridcase 3 computer keyboard with a 10 pin and a 12 pin connector. There may be diodes in series 
// with the switches in this keyboard so the keys are scanned in both directions. 
// Before trying to download this code into the Teensy 4.1, select Tools, Board, Teensyduino, Teensy 4.1
// Next select Tools, USB Type, Keyboard
// Load this software with File, Open and select the filename. 
// Connect the Teensy 4.1 to the PC with a USB cable. Your PC will load the necessary drivers. 
// Compile and burn the code into the Teensy by selecting the right arrow (below edit)
// It will take several seconds the first time to compile and you will have to push the button on the Teensy
// but eventually it will report no errors and show it finished programming the Teensy. Unplug the USB Cable temporarily.
// Open a text editor like Wordpad with a list of all the keys and place the cursor to the right of the first key in the list.
// Power up the Teensy (by attaching the USB cable), the code will delay 20 seconds before it scans for any key presses.
// If it then reports two numbers, those pins are always shorted together and must be excluded. 
// Press each key as listed on the text editor and 2 pin numbers should be listed, then a down arrow to get ready for the next key.
// Save the finished key list text file for transfer into a matrix.
//
// Load an array with the Teensy I/O numbers that correspond to keyboard pins 1 thru 10 from connector 1 and 1 thru 12 from connector 2.
int con_pin[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22}; // I/O 13 is skipped because it's for the LED
//
// Define maximum and minimum pin numbers that will be tested. 
//
int max_pin = 22; // the total pins to can on the keyboard  
int min_pin = 1; // the first pin to be tested (a value of 1 is the first in the list from above which is I/O 0)
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
  Keyboard.set_key1(0); // clear the down arrow
  Keyboard.send_now();
}

// --------------------------------------------------Setup-----------------------------------
void setup() {
  for (int k = 0; k < max_pin; k++) {  // loop thru all connector pins 
    go_z(con_pin[k]); // set each pin as an input with a pullup
  }
  delay(15000);  // Wait for the host to connect to the Teensy as a keyboard. If 2 pins are shorted
              // you want the host to be ready to receive the pin numbers.
}
//
int blink_count = 0; // loop counter
boolean blinky = LOW; // Blink LED state
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
           ;     // if 2 pins are shorted, the code will hang here waiting for their release                                          
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
// Blink the LED to show the Teensy is alive
//
  if (blink_count == 0x0a) {  
    pinMode(13, OUTPUT);
    digitalWrite(13, blinky);
    blinky = !blinky;
    blink_count = 0;
  }
  else {
    blink_count++;
  }
delay(25);  // overall keyboard scan rate is about 30 milliseconds
//
}
