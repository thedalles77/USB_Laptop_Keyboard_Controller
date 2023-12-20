This software implements an Alienware 17 Laptop Keyboard Controller (laptop model number p18e001) using a Teensy LC on 
a daughterboard with a 26 pin FPC connector. The FPC cable has been trimmed down from its original pin count 
(see PDF picture) and is inserted into FPC connector pins 2 thru 26 (pin 2 is not active). 

Now that the Teensy LC is obsolete, this keyboard controller should be implemented with a Teensy 4.0 or 4.1
and an FPC connector sized so the cable does not need to be trimmed.

Load the Matrix_Decoder code into the Teensy and make a key list as described in the Instructable.
Then use the LC code as your starting point but adjust for the new Teensy.
