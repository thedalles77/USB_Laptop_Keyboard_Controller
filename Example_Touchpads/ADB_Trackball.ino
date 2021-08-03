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
// This software implements an Apple ADB interface for a trackball. It uses a  
// Teensy 3.2 and is based on the code from this Imgur post https://imgur.com/a/BQjt9le
// 
// Revision History
// Initial Release Aug 3, 2021
// 
#define ADB_DATA_PIN 5 // TB bidirectional data pin
//
// TB Constants
int ADB_TIMEOUT = 10000;
// TB Functions
static void send_0bit() {
 digitalWrite(ADB_DATA_PIN, LOW);
 delayMicroseconds(65);
 digitalWrite(ADB_DATA_PIN, HIGH);
 delayMicroseconds(35);
}

static void send_1bit() {
 digitalWrite(ADB_DATA_PIN, LOW);
 delayMicroseconds(35);
 digitalWrite(ADB_DATA_PIN, HIGH);
 delayMicroseconds(65);
}

static void attn() {
  //delay(1);
  digitalWrite(ADB_DATA_PIN, LOW);
  delayMicroseconds(800-35);
  send_0bit();
}

void send_talk_command() {
 //attn
 pinMode(ADB_DATA_PIN, OUTPUT);
 attn();
 //address (3) 0011
 //0
  send_0bit();
  send_0bit();
  send_1bit();
  send_1bit();
 //talk 11
  send_1bit();
  send_1bit();
 //register 00
  send_0bit();
  send_0bit();
 //stop bit
  send_0bit();
 //start to stop time
 delayMicroseconds(140);

}
int receive_data_packet() {
 pinMode(ADB_DATA_PIN, INPUT);
 int data_time = 0;
 bool command_stop = false;
 int adb_data = 0;
 //begin reading ADB datapin
 
 while(digitalRead(ADB_DATA_PIN)!=LOW && data_time < ADB_TIMEOUT){
  //wait for beginning of start bit
  data_time+=1;
  delayMicroseconds(1);
 }

  while(digitalRead(ADB_DATA_PIN)!=HIGH && data_time < ADB_TIMEOUT){
  //wait for end of start bit
  data_time+=1;
  delayMicroseconds(1);
 }

 while(digitalRead(ADB_DATA_PIN)!=LOW && data_time < ADB_TIMEOUT){
  //wait for beginning of first data bit
  data_time+=1;
  delayMicroseconds(1);
 }

 //data packet
 data_time = 0;
 while(!command_stop && data_time < ADB_TIMEOUT) {
  int low = 0;
  int high = 0;
  bool bit_stop = false;
  while(!bit_stop && data_time < ADB_TIMEOUT) {
    
    int adb_stream = digitalRead(ADB_DATA_PIN);
    if(adb_stream == LOW) low+=1;
    if(adb_stream == HIGH) high+=1;
    if(adb_stream == LOW && high > 0) bit_stop = true;
    if(high > 240){
      bit_stop = true;
      break;
    }
    data_time+=1;
    delayMicroseconds(1);
  }
  if(high > 240){
    command_stop = true;
  }
  //append the new data bit to adb_data
  if(high > low && !command_stop) adb_data = (adb_data << 1) + 1;
  if(low > high && !command_stop) adb_data = adb_data << 1;
  
 }
 //stop bit 
 return adb_data;
}

void parse_data(int data) {
      if(data !=0) {
       int mousex = data & 0b0000000001111111;
       int mousey = (data >> 8) & 0b01111111;
       //the mouse value is
        //7 bit two's complement
       if((mousey & 0b1000000) != 0) {
          mousey =  ((~mousey + 1) & 0b01111111) * -1;
       }
       if((mousex & 0b1000000) != 0) {
          mousex =  ((~mousex + 1) & 0b01111111) * -1;
       }
       Mouse.move(mousex, mousey);
       Mouse.move(mousex, mousey);

      int mouseClick = data & 0b1000000000000000;
      
      if(mouseClick == 0) {
        Mouse.set_buttons(1,0,0);
      }
      else if(mouseClick != 0) {
        Mouse.set_buttons(0,0,0);
      }
     }
}

//
//----------------------------------Setup-------------------------------------------
void setup() {
  
}
//
//---------------------------------Main Loop---------------------------------------------
//
void loop() {   
   send_talk_command();
   parse_data(receive_data_packet());
//
  delay(20); // 
}
