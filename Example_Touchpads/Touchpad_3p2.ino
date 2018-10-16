// This software is in the public domain
// It interfaces the Teensy 3.2 with a PS/2 touchpad found in a laptop.
// The PS/2 code was originally from https://playground.arduino.cc/uploads/ComponentLib/mouse.txt
// but the interface to the PC was changed from RS232 serial to USB using the PJRC Mouse functions. 
// A watchdog timer was also added to the "while loops" so the code doesn't hang if the touchpad hiccups.
//
// This code has been tested on the touchpad from an HP Pavilion DV9000
// Touchpad part number 920-000702-04 Rev A
// The test points on the touchpad were wired to a Teensy 3.2 as follows:
// T22 = 5V wired to the Teensy Vin pin
// T23 = Gnd wired to the Teensy Ground pin   It's hard to solder to T23 so I soldered to a bypass cap gnd pad.1 
// T10 = Clock wired to the Teensy I/O 14 pin
// T11 = Data wired to the Teensy I/O 15 pin
//
// Clock and Data measure open to the 5 volt pin, indicating no pull up resistors but,
// Clock and Data both measure 5 volts when the touchpad is powered, indicating active pullups are in 
// the touchpad blob top chip.
// The ps/2 signals are at 5 volts from the touchpad to the Teensy which is 5 volt tolerant.
// The ps/2 signals are at 3.3 volts from the Teensy to the touchpad which is enough to be a logic high.
//
// Revision History
// Initial Release Oct 16, 2018
//
// The touchpad ps/2 data and clock lines are connected to the following Teensy I/O pins
#define MDATA 15
#define MCLK 14
//
// Declare variable that will be used by functions
boolean touchpad_error = LOW; // sent high when touch pad routine times out
//
// Function to float a pin and let the pull-up or Touchpad determine the logic level
void go_z(int pin)  
{
  pinMode(pin, INPUT); // make the pin an input so it floats
  digitalWrite(pin, HIGH);
}
// function to drive a pin to a logic low
void go_0(int pin)
{
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
}
// Function to send the Touchpad a command
void touchpad_write(char data)
{
  char i;
  char parity = 1;
 // put pins in output mode 
  go_z(MDATA);
  go_z(MCLK);
  elapsedMillis watchdog; // set watchdog to zero
  delayMicroseconds(300);
  go_0(MCLK);
  delayMicroseconds(300);
  go_0(MDATA);
  delayMicroseconds(10);
  // start bit 
  go_z(MCLK);
  // wait for touchpad to take control of clock)
  while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag
      break;
    }
  }
  // clock is low, and we are clear to send data 
  for (i=0; i < 8; i++) {
    if (data & 0x01) {
      go_z(MDATA);
    } 
    else {
      go_0(MDATA);
    }
    // wait for clock cycle 
    while (digitalRead(MCLK) == LOW) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }      
    while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
    parity = parity ^ (data & 0x01);
    data = data >> 1;
  }  
  // parity 
  if (parity) {
    go_z(MDATA);
  } 
  else {
    go_0(MDATA);
  }
 // wait for clock cycle
  while (digitalRead(MCLK) == LOW) {
  if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }  
  while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }  
  // stop bit 
  go_z(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  // wait for touchpad to switch modes 
  while ((digitalRead(MCLK) == LOW) || (digitalRead(MDATA) == LOW)) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  // put a hold on the incoming data. 
  go_0(MCLK);
}

//
// Function to get a byte of data from the touchpad
//
char touchpad_read(void)
{
  char data = 0x00;
  int i;
  char bity = 0x01;
  // start the clock 
  elapsedMillis watchdog; // set watchdog to zero
  go_z(MCLK);
  go_z(MDATA);
  delayMicroseconds(50);
  while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  delayMicroseconds(5);  // wait for clock ring to settle 
  while (digitalRead(MCLK) == LOW) {  // eat start bit 
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  for (i=0; i < 8; i++) {
    while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
    if (digitalRead(MDATA) == HIGH) {
      data = data | bity;
    }
    while (digitalRead(MCLK) == LOW) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
    bity = bity << 1;
  }
  // ignore parity bit  
  while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  while (digitalRead(MCLK) == LOW) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  // eat stop bit 
  while (digitalRead(MCLK) == HIGH) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  while (digitalRead(MCLK) == LOW) {
    if (watchdog >= 200) { //check for infinite loop
      touchpad_error = HIGH; // set error flag       
      break;
    }
  }
  // put a hold on the incoming data. 
  go_0(MCLK);
  return data;
}

void touchpad_init()
{
  touchpad_error = LOW; // start with no error
  go_z(MCLK); // float the clock and data to touchpad
  go_z(MDATA);
  //  Sending reset to touchpad
  touchpad_write(0xff);
  touchpad_read();  // ack byte
  //  Read ack byte
  touchpad_read();  // blank 
  touchpad_read();  // blank 
  // Default resolution is 4 counts/mm which is too small
  //  Sending resolution command
  touchpad_write(0xe8);
  touchpad_read();  // ack byte
  touchpad_write(0x03); // value of 03 gives 8 counts/mm resolution
  touchpad_read();  // ack byte
  //  Sending remote mode code so the touchpad will send data only when polled
  touchpad_write(0xf0);  // remote mode 
  touchpad_read();  // Read ack byte 
  delayMicroseconds(100);
}
// ************************************Begin Routine*********************************************************
void setup()
{
  touchpad_init(); // reset touchpad, then set it's resolution and put it in remote mode 
  if (touchpad_error) {
    touchpad_init(); // try one more time to initialize the touchpad
  }
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
// poll the touchpad for new movement data
  over_flow = 0; // assume no overflow until status is received 
  touchpad_error = LOW; // start with no error
  touchpad_write(0xeb);  // request data
  touchpad_read();      // ignore ack
  mstat = touchpad_read(); // save into status variable
  mx = touchpad_read(); // save into x variable
  my = touchpad_read(); // save into y variable
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
  if (!touchpad_error) {
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
//
// **************************************End of touchpad routine***********************************
// 
  delay(30);  // wait 30ms before repeating next polling cycle
}
