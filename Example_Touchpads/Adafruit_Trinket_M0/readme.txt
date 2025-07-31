The file SAMD21_PS2.ino is for an Adafruit Trinket M0 to control a PS/2 touchpad and send relative movement and button pushes over USB.
The file Azoteq_SAMD21.ino is for an Adafruit Trinket M0 to control an Azoteq touchpad Part No: TPS65-201A-S over I2C and sends the results 
over USB using the Arduino mouse function. The Mouse.press and release functions are used to send a left button push and release based on a finger tap on the touchpad. 
Trinket_M0_TP.ino is for a Samsung Galaxybook Flex touchpad part number BA61-04080A. This touchpad uses I2C for communication with an INTN signal tht goes low when there is data available.
