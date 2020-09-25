This code can be used on the LC and 4.0 but they need a level translator if connected to a 5 volt touchpad.
The Touchpad_3p2.ino code sets up the touchpad to be in remote mode so that the Teensy can poll it after finishing the keyboard scan. 
The Touchpad_Stream_mode.ino code sets up the touchpad to be in Stream Mode so it sends data when ever it detects finger movement.
For a complete description of how to interface a Teensy to a touchpad, read my touchpad guide at: 
https://www.hackster.io/frank-adams/laptop-touchpad-conversion-to-usb-d70519
