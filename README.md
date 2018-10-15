# USB_Laptop_Keyboard_Controller
* This repo contains all the files that are needed to make a USB keyboard from an old laptop keyboard.

* See the "Instructable" at this address

* See the YouTube video at this address

* A PDF version of the Instructable with additional informatuion is in the directory with this README file.

* The directory tree is given below:

* Eagle_Board Folder
*     Keyboard_Scanner  EagleCad circuit board file for Teensy LC & 3.2. Can be fab'ed by OSHPark or DirtyPCBs (among others).
* Blank_Key_Lists Folder
*     Keyboard_with_number_pad     Lists all keys on a keyboard with a number pad. Used when running the Matrix_Decoder code.
*     Keyboard_without_number_pad  Lists all keys on a keyboard without a number pad. Used when running the Matrix_Decoder code.
* Teensy_Continuity_Tester Folder
*     Matrix_Decoder_LC        Teensyduino file for Teensy LC continuity tester. Code sends 2 pins numbers when key is pressed. 
*     Matrix_Decoder_3p2       Teensyduino file for Teensy 3.2 continuity tester. Code sends 2 pins numbers when key is pressed. 
* Example_Keyboards Folder
*     Instructions for Modifying the Teensyduino LC code     Word file with LC instructions and translation table.
*     Instructions for Modifying the Teensyduino 3p2 code    Word file with 3.2 instructions and translation table.
*     Laptop_Keyboard      A separate Folder for each laptop keyboard, containing the following:
*          Text file with FPC pin connections. The results from running the Matrix_decoder code.
*          Word file with key matrix table and any additional information. Use this table if using the TMK routine.
*          Teensyduino file to make a USB keyboard. Matrix table already incorporated into a simple homebrew routine.
          
* Each of the Laptop Keyboards listed below have been tested and documented in their own folder:
*  Dell Inspiron 1525 - Keyboard Part Number D9K01
*  Dell Latitude 131L - Keyboard Part Number V-0511BIAS1-US
*  Dell Latitude X1 - Keyboard Part Number 0M6607
*  Dell Latitude D630 - Keyboard Part Number DP/N 0DR160
*  HP Compaq Presario 2100 - Keyboard Part Number AEKT1TPU011
*  HP Compaq Presario V4000 - Keyboard Part Number NSK-H3L01
*  HP Pavilion DV9000 - Keyboard Part number AEAT5U00110
*  Sony Vaio PCG-K25 - Keyboard Part Number KFRMBA151B
*  Sony Vaio VPCCW - Keyboard Part Number 148754321
*  Sony Vaio VPCEA - Keyboard part number A-1765-621-A
*  Sony Vaio VPCEB4 – Keyboard part number A-1766-425-A
