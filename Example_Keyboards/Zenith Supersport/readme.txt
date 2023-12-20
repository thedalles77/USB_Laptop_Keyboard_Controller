This project was originally based on a Teensy LC which is now obsolete.
The code as written will also work with a Teensy 4.0 on the existing LC circuit board with the following changes:
    - Add a flying lead to I/O # 24 on the backside of the Teensy 4.0 that goes to the board at the plated thru hole that was for I/O # 24 on the Teensy LC.
    - Use 150 ohm resistors for the LEDs instead of 75 ohm to stay below the 10ma current capability of the Teensy 4.0 I/O pins.
