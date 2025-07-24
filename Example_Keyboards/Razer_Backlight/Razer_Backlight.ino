/* Copyright 2025 Frank Adams
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
// Razor keyboard backlight controller using an Adafruit Trinket M0
//
// The 8 pin 0.5mm FPC cable from the keyboard backlight has the following pinout:
// pins 1 & 2 = Ground
// pin 3 = Backlight enable when high. I have it wired to 3.3 volts
// pin 4 = I2C Clock that connects to the Trinket M0 pin 2. Also has a 2K pull up resistor to 3.3 volts.
// pin 5 = I2C Data that connects to the Trinket M0 pin 0   Also has a 2K pull up resistor to 3.3 volts.
// pin 6 = 3.3 volts tied to Trinket M0 3.3 volt regulator. At full brightness it only draws 240ma on the USB 5v bus
// 
#define red_led 0x3c // backlight red i2c address 
#define blue_led 0x3d // backlight blue i2c address is 3d
#define green_led 0x3f // backlight green i2c address is 3f
#include <Wire.h> // Needed for I2C bus
// The following is needed to turn off the VERY BRIGHT RGB LED on the Trinket M0
#include <Adafruit_DotStar.h>
#include <SPI.h>         
#define NUMPIXELS 1 // Number of LEDs in strip
#define DATAPIN   7
#define CLOCKPIN   8
Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BGR);
//
// store the backlight brightness values in three 8 bit unsigned integers 
uint8_t red_value = 50; // 50 is the initial brightness value given to all 3 colors
uint8_t blue_value = 50;
uint8_t green_value = 50;
//
String command; // used to hold the backlight command sent over 9600 baud serial bus.
// Commands Are: u=all up by 5, d=all down by 5, ur=up red by 5, ub=up blue by 5, ug=up green by 5, 
// dr=down red by 5, db=down blue by 5, dg=down green by 5, mid=set all to the initial middle value, 
// off=set all to zero, max=set all to maximum brightness of 255
int pot_pin = A3;  // analog input pin A3 tied to wiper arm of a 10K potentiometer. Ends of pot go to 3.3V and ground
int mode_switch_pin = 1; // switch input with a pull up that enables the potentiometer when low (on) or the Com port when high (off)
int push_button_pin = 4; // input with pull up that displays a colorfull backlight display when pressed (low)
//
unsigned int brightness10 = 0; // ADC 10 bit value from potentiometer controls brightness of backlight leds
uint8_t brightness8 = 0; // 10 bit converted to 8 bit brightness (drop 2 lsb's by dividing by 4)
//
// ********************Common Functions**********************************************
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
//*********************************Setup*****************************************************
//
void setup() {
  delay (1000); // wait 1 second to let the power stabilize
// the next 2 lines are for turning off the VERY BRIGHT RGB LED on the Trinket M0
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off 
//  
  Serial.begin(9600); // start the serial bus monitor so commands can be given at 9600 baud
  pinMode(pot_pin, INPUT); // Set the analog pin as an input to the ADC
  go_pu(mode_switch_pin); // switch input needs pull up so it will go to zero when tied to ground.
  go_pu(push_button_pin); // push button switch input needs pull up so it will go to zero when pushed.
//  
  Wire.begin(); // Initiate the Wire library and join the I2C bus as a master
// I'm using the default (slow) i2c clock speed just to be safe
//
// The following values are sent to the 3 backlight IC's to get them set up for operation.
//
// write to the red led address at 3c, data value 4f and 00
  Wire.beginTransmission(red_led); // back light address  
  Wire.write(0x4f);    
  Wire.write(0x00);    
  Wire.endTransmission(true); // send stop bit
  delay (5); // delay
// write to address 3c, data value 2a, 01 15 times, 00 13 times
  Wire.beginTransmission(red_led);
  Wire.write(0x2a);
  for (int k = 0; k < 15; k++) {  // send it multiple times 
    Wire.write(0x01);  
  }
  for (int k = 0; k < 13; k++) {  // send it multiple times 
    Wire.write(0x00);  
  }
  Wire.endTransmission(true); // send stop bit
// write to address 3c, data value 05, 00 28 times
  Wire.beginTransmission(red_led);
  Wire.write(0x05);
  for (int k = 0; k < 28; k++) {  // send it multiple times 
    Wire.write(0x00);  
  }
  Wire.endTransmission(true); // send stop bit
// write to address 3c, data value 25, 00
  Wire.beginTransmission(red_led);
  Wire.write(0x25);    
  Wire.write(0x00);    
  Wire.endTransmission(true); // send stop bit
// write to address 3c, data value 48, 01
  Wire.beginTransmission(red_led);
  Wire.write(0x48);    
  Wire.write(0x01);    
  Wire.endTransmission(true); // send stop bit
// write to address 3c, data value 00, 01
  Wire.beginTransmission(red_led);
  Wire.write(0x00);    
  Wire.write(0x01);    
  Wire.endTransmission(true); // send stop bit

  delay (5);

// write to the blue led address at 3d, data value 4f and 00
  Wire.beginTransmission(blue_led); // back light address  
  Wire.write(0x4f);    
  Wire.write(0x00);    
  Wire.endTransmission(true); // send stop bit
  delay (5); // delay
// write to address 3d, data value 2a, 01 15 times, 00 13 times
  Wire.beginTransmission(blue_led);
  Wire.write(0x2a);
  for (int k = 0; k < 15; k++) {  // send it multiple times 
    Wire.write(0x01);  
  }
  for (int k = 0; k < 13; k++) {  // send it multiple times 
    Wire.write(0x00);  
  }
  Wire.endTransmission(true); // send stop bit
// write to address 3d, data value 05, 00 28 times
  Wire.beginTransmission(blue_led);
  Wire.write(0x05);
  for (int k = 0; k < 28; k++) {  // send it multiple times 
    Wire.write(0x00);  
  }
  Wire.endTransmission(true); // send stop bit
// write to address 3d, data value 25, 00
  Wire.beginTransmission(blue_led);
  Wire.write(0x25);    
  Wire.write(0x00);    
  Wire.endTransmission(true); // send stop bit
// write to address 3d, data value 48, 01
  Wire.beginTransmission(blue_led);
  Wire.write(0x48);    
  Wire.write(0x01);    
  Wire.endTransmission(true); // send stop bit
// write to address 3d, data value 00, 01
  Wire.beginTransmission(blue_led);
  Wire.write(0x00);    
  Wire.write(0x01);    
  Wire.endTransmission(true); // send stop bit

  delay (5);

// write to the green led address at 3f, data value 4f and 00
  Wire.beginTransmission(green_led); // back light address  
  Wire.write(0x4f);    
  Wire.write(0x00);    
  Wire.endTransmission(true); // send stop bit
  delay (5); // delay
// write to address 3f, data value 2a, 01 15 times, 00 13 times
  Wire.beginTransmission(green_led);
  Wire.write(0x2a);
  for (int k = 0; k < 15; k++) {  // send it multiple times 
    Wire.write(0x01);  
  }
  for (int k = 0; k < 13; k++) {  // send it multiple times 
    Wire.write(0x00);  
  }
  Wire.endTransmission(true); // send stop bit
// write to address 3f, data value 05, 00 28 times
  Wire.beginTransmission(green_led);
  Wire.write(0x05);
  for (int k = 0; k < 28; k++) {  // send it multiple times 
    Wire.write(0x00);  
  }
  Wire.endTransmission(true); // send stop bit
// write to address 3f, data value 25, 00
  Wire.beginTransmission(green_led);
  Wire.write(0x25);    
  Wire.write(0x00);    
  Wire.endTransmission(true); // send stop bit
// write to address 3f, data value 48, 01
  Wire.beginTransmission(green_led);
  Wire.write(0x48);    
  Wire.write(0x01);    
  Wire.endTransmission(true); // send stop bit
// write to address 3f, data value 00, 01
  Wire.beginTransmission(green_led);
  Wire.write(0x00);    
  Wire.write(0x01);    
  Wire.endTransmission(true); // send stop bit

  delay (5);

// send initial brightness settings

// Red initial value
  Wire.beginTransmission(red_led); // back light address  
  Wire.write(0x05); // write 05  
  for (int k = 0; k < 25; k++) {  // send value 25 times 
    Wire.write(red_value); // write value, 25 times   
  } 
  Wire.endTransmission(true); // send stop bit
//
// Blue initial value
  Wire.beginTransmission(blue_led); // back light address  
  Wire.write(0x05); // write 05  
  for (int i = 0; i < 25; i++) {  // send it 25 times 
    Wire.write(blue_value); // write value, 25 times   
  } 
  Wire.endTransmission(true); // send stop bit
//
// Green initial value
  Wire.beginTransmission(green_led); // back light address  
  Wire.write(0x05); // write 05  
  for (int j = 0; j < 15; j++) {  // send it 15 times 
    Wire.write(green_value); // write value, 15 times   
  } 
  for (int g = 0; g < 13; g++) {  // send it 13 times 
    Wire.write(0x00); // write zero's 13 times. this value makes no difference 
  }
  Wire.endTransmission(true); // send stop bit
//
  delay (2); 
// short bursts that activate the new values 
  Wire.beginTransmission(red_led); // back light address  
  Wire.write(0x25); // write 25  
  Wire.write(0x00); // write 00
  Wire.endTransmission(true); // send stop bit
  //
  Wire.beginTransmission(blue_led); // back light address  
  Wire.write(0x25); // write 25  
  Wire.write(0x00); // write 00
  Wire.endTransmission(true); // send stop bit
  //
  
  Wire.beginTransmission(green_led); // back light address  
  Wire.write(0x25); // write 25  
  Wire.write(0x00); // write 00   
  Wire.endTransmission(true); // send stop bit
  // 
}
// *********************************************Main Loop*****************************************
//
void loop() {
// ***********************************Pulsing Light Display**********************************************
  if (!digitalRead(push_button_pin)) { // low causes pulsing light display 
    // Red block
    Wire.beginTransmission(red_led); // red back light address  
    Wire.write(0x05); // write 05  
    for (int k = 0; k < 25; k++) {  // send value 25 times 
      red_value = red_value + 3;
      Wire.write(red_value); // write value, 25 times
    }
    Wire.endTransmission(true); // send stop bit
// Blue block
    Wire.beginTransmission(blue_led); // blue back light address  
    Wire.write(0x05); // write 05  
    for (int i = 0; i < 25; i++) {  // send it 25 times 
      blue_value = blue_value - 1;
      Wire.write(blue_value); // write value, 25 times   
    } 
    Wire.endTransmission(true); // send stop bit
// Green block
    Wire.beginTransmission(green_led); // green back light address  
    Wire.write(0x05); // write 05  
    for (int j = 0; j < 15; j++) {  // send it 15 times 
      green_value = green_value + 2;
      Wire.write(green_value); // write value, 15 times   
    } 
    for (int g = 0; g < 13; g++) {  // send it 13 times 
      Wire.write(0x00); // write zero's 13 times. this value makes no difference 
    }
    Wire.endTransmission(true); // send stop bit
//
  delay (2);
// short bursts that activate the new values
  Wire.beginTransmission(red_led); // red back light address  
  Wire.write(0x25); // write 25  
  Wire.write(0x00); // write 00
  Wire.endTransmission(true); // send stop bit
//
  Wire.beginTransmission(blue_led); // blue back light address  
  Wire.write(0x25); // write 25  
  Wire.write(0x00); // write 00
  Wire.endTransmission(true); // send stop bit
//
  Wire.beginTransmission(green_led); // green back light address  
  Wire.write(0x25); // write 25  
  Wire.write(0x00); // write 00   
  Wire.endTransmission(true); // send stop bit

  delay (100);
//    
  }
// ***********************************Mode Switch Enables Potentiometer**************************************************************
  else if (!digitalRead(mode_switch_pin)) { //  read the potentiometer as a brightness control when switch is low
//  read the ADC and convert to 8 bits used as brightness by all 3 LED colors
    brightness10 = analogRead(pot_pin); // Read the 10 bit analog value from the pot and store it in the brightness 16 bit integer
    brightness10 = brightness10 >> 2; // shift right 2 bits to throw away the 2 lsbs. Now the low byte contains the brightness value
    brightness8 = (uint8_t)brightness10; // convert 16 bit value to 8 bit value with a type cast
    red_value = brightness8; //store the potentiometer value as the brightness for all 3 variables
    blue_value = brightness8; 
    green_value = brightness8;   
// Red 3c block
    Wire.beginTransmission(red_led); // red back light address  
    Wire.write(0x05); // write 05  
    for (int k = 0; k < 25; k++) {  // send value 25 times 
      Wire.write(red_value); // write value, 25 times
    } 
    Wire.endTransmission(true); // send stop bit
// Blue 3d block
    Wire.beginTransmission(blue_led); // blue back light address  
    Wire.write(0x05); // write 05  
    for (int i = 0; i < 25; i++) {  // send it 25 times 
      Wire.write(blue_value); // write value, 25 times   
    } 
    Wire.endTransmission(true); // send stop bit
// Green block
    Wire.beginTransmission(green_led); // green back light address  
    Wire.write(0x05); // write 05  
    for (int j = 0; j < 15; j++) {  // send it 15 times 
      Wire.write(green_value); // write value, 15 times   
    } 
    for (int g = 0; g < 13; g++) {  // send it 13 times 
      Wire.write(0x00); // write zero's 13 times. this value makes no difference 
    }
    Wire.endTransmission(true); // send stop bit
//
    delay (2);
// short commands that activate the new values  
    Wire.beginTransmission(red_led); // red back light address  
    Wire.write(0x25); // write 25  
    Wire.write(0x00); // write 00
    Wire.endTransmission(true); // send stop bit
//
    Wire.beginTransmission(blue_led); // blue back light address  
    Wire.write(0x25); // write 25  
    Wire.write(0x00); // write 00
    Wire.endTransmission(true); // send stop bit
//
    Wire.beginTransmission(green_led); // green back light address  
    Wire.write(0x25); // write 25  
    Wire.write(0x00); // write 00   
    Wire.endTransmission(true); // send stop bit
// 
  }
//**********************************Command over Serial Bus*****************************************************
  else if (Serial.available() > 0) { // check if anything received over serial com port
    command = Serial.readStringUntil('\n');
    command.trim(); // Remove leading/trailing whitespace
    if (command.equals("u")) {
      if (red_value <= 250) {
        red_value = red_value + 5;
      }
      if (blue_value <= 250) {
        blue_value = blue_value + 5;
      }
      if (green_value <= 250) {
        green_value = green_value + 5;
      }
    }
    else if (command.equals("d")) {
      if (red_value >= 5) {
        red_value = red_value - 5;
      }
      if (blue_value >= 5) {
        blue_value = blue_value - 5;
      }
      if (green_value >= 5) {
        green_value = green_value - 5;
      }
    }
    else if (command.equals("ur")) {
      if (red_value <= 250) {
        red_value = red_value + 5;
      }
    }
    else if (command.equals("ub")) {
      if (blue_value <= 250) {
        blue_value = blue_value + 5;
      }
    }
    else if (command.equals("ug")) {
      if (green_value <= 250) {
        green_value = green_value + 5;
      }
    }
    else if (command.equals("dr")) {
      if (red_value >= 5) {
        red_value = red_value - 5;
      }
    }
    else if (command.equals("db")) {
      if (blue_value >= 5) {
        blue_value = blue_value - 5;
      }
    }
    else if (command.equals("dg")) {
      if (green_value >= 5) {
        green_value = green_value - 5;
      }
    }
    else if (command.equals("mid")) {
      red_value = 50;
      blue_value = 50;
      green_value = 50;
    }
    else if (command.equals("off")) {
      red_value = 0;
      blue_value = 0;
      green_value = 0;
    }
        else if (command.equals("max")) {
      red_value = 255;
      blue_value = 255;
      green_value = 255;
    }
    else {
      Serial.println("Invalid command - Commands are u, d, ur, ub, ug, dr, db, dg, mid, off, max");
    }
// send brightness values over i2c to the backlight driver ic's
    //
// Red 3c block
    Wire.beginTransmission(red_led); // red back light address  
    Wire.write(0x05); // write 05  
    for (int k = 0; k < 25; k++) {  // send value 25 times 
      Wire.write(red_value); // write value, 25 times   
    } 
    Wire.endTransmission(true); // send stop bit
// Blue 3d block
    Wire.beginTransmission(blue_led); // blue back light address  
    Wire.write(0x05); // write 05  
    for (int i = 0; i < 25; i++) {  // send it 25 times 
      Wire.write(blue_value); // write value, 25 times   
    } 
    Wire.endTransmission(true); // send stop bit
// Green block
    Wire.beginTransmission(green_led); // green back light address  
    Wire.write(0x05); // write 05  
    for (int j = 0; j < 15; j++) {  // send it 15 times 
      Wire.write(green_value); // write value, 15 times   
    } 
    for (int g = 0; g < 13; g++) {  // send it 13 times 
      Wire.write(0x00); // write zero's 13 times. this value makes no difference 
    }
    Wire.endTransmission(true); // send stop bit
//
    delay (2);
// short commands that activate the new values  
    Wire.beginTransmission(red_led); // red back light address  
    Wire.write(0x25); // write 25  
    Wire.write(0x00); // write 00
    Wire.endTransmission(true); // send stop bit
//
    Wire.beginTransmission(blue_led); // blue back light address  
    Wire.write(0x25); // write 25  
    Wire.write(0x00); // write 00
    Wire.endTransmission(true); // send stop bit
//
    Wire.beginTransmission(green_led); // green back light address  
    Wire.write(0x25); // write 25  
    Wire.write(0x00); // write 00   
    Wire.endTransmission(true); // send stop bit
//         
  } 
//  
delay (5); //
}
