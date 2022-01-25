This code can be used on the Teensy LC and 4.0 but they need a level translator if connected to a 5 volt touchpad.
The Touchpad_3p2.ino code sets up the touchpad to be in remote mode so that the Teensy can poll it after finishing the keyboard scan. 
The Touchpad_Stream_mode.ino code sets up the touchpad to be in Stream Mode so it sends data when ever it detects finger movement.
ADB_Trackball.ino will read an Apple Data Bus trackball and send the results as USB mouse commands. 
Fujitsu_Ergotrac.ino uses the ADC in the Teensy 3.2 to read the X and Y capacitive sensor.
I2C_Touchpad.ino is an example routine to read a basic touchpad over an I2C bus and send the results as USB mouse commands. 
Read the comments at the beginning of the code to see the address and register numbers for this example touchpad.
Azoteq_TP.ino uses the Teensy I2C pins to read an Azoteq TPS65-201A-S touchpad and send the relative X/Y movement
and the finger-tap as USB mouse commands. For more information on the registers, read the Azoteq data sheet at:
https://www.azoteq.com/images/stories/pdf/iqs5xx-b000_trackpad_datasheet.pdf

For a complete description of how to interface a Teensy to a touchpad, read my touchpad guide at: 
https://www.hackster.io/frank-adams/laptop-touchpad-conversion-to-usb-d70519

The folder "Adafruit_Trinket_M0" contains touchpad routines for a PS2 touchpad and a Azoteq I2C touchpad that can be used on
an Adafruit_Trinket_M0 or any other Arduino that has hardware HID capability (like a Pro Micro).
