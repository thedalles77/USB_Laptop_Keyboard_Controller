# USB_Laptop_Keyboard_Controller
 This repo contains all the files that are needed to make a USB keyboard and a KVM from an old laptop.

 See the YouTube keyboard video at https://www.youtube.com/watch?v=Z1PheqSNNP8
 
 See the YouTube KVM video at https://www.youtube.com/watch?v=M5-dYo2jt14
 
 See the IBM Thinkpad 380ED Keyboard and Trackpoint Conversion to USB video at https://www.youtube.com/watch?v=WGSg1ky4UUs
 
 See the keyboard "Instructable" at https://www.instructables.com/id/How-to-Make-a-USB-Laptop-Keyboard-Controller/

 See the KVM "Instructable" at https://www.instructables.com/id/DIY-Portable-KVM-Crashcart-From-Recycled-Laptop-Pr/
 
 See the IBM Thinkpad 380ED to USB project description at https://hackaday.io/project/171439-ibm-thinkpad-380ed-keyboardtrackpoint-to-usb
 
 Marcel's Python matrix generator program has been modified for the Teensy 3.2 and 4.0. His repo is forked here:
 
 https://github.com/thedalles77/USB_Laptop_Keyboard_Controller-1
 
 All files and folders in my repo are listed below:
 
 Keyboard_Scanner_Eagle_Layouts Folder
 
     Keyboard_Scanner.brd  EagleCad circuit board file for Teensy LC or 3.2. 
 
     Keyboard_Scanner_LT.brd EagleCad file same as above with 2 bit Level Translator for touchpad Clock and Data
     
     Keyboard_Scanner_LT2.brd  EagleCad file same as above with improved hole spacing for right angle header on 3.2 backside signals
 
     Keyboard_Scanner_LT_0p5.brd EagleCad file same as Keyboard_Scanner except made for a 0.5mm pitch FPC connector
 
     Keyboard_Scanner_Dual.brd EagleCad file is for keyboards with dual FPC cables
 
     Keyboard_Scanner_4p0.brd  EagleCad file for Teensy LC or 4.0
     
     Keyboard_Scanner_LC_thruhole.brd  EagleCad file for Teensy LC with 26 pin thru hole FPC connector
     
     Keyboard_Scanner_2pp.brd  EagleCad file for Teensy ++2.0 and up to 36 pins at 1mm, 0.8mm, or 0.5mm pitch
 
     Keyboard_Scanner_.....2020-02-26.zip  Zipped gerber files for PJRC fabrication
     
     Note that the Lenovo Thinkpad T61 Eagle files are in the Example_Keyboards Folder
     
 Blank_Key_Lists Folder
 
     Keyboard_with_number_pad.txt     Lists all keys on a keyboard with a number pad. 
     
     Keyboard_without_number_pad.txt  Lists all keys on a keyboard without a number pad. 
     
 Commodore_64 Folder
 
     Teensy LC code and key list files developed by Olga for a Commodore 64, see Readme file.  
 
 GRID 1550 Folder
 
     Teensy LC code for a GRID 1550 UK keyboard and roller-shaft mouse. PDF describes project.

IBM T43 Folder

     Teensy 3.2 code and KiCad layout for an IBM Thinkpad T43 keyboard developed by Nat. PDF describes project.
     
 T Caschy Folder
 
     HP Jornada and Atari Portfolio keyboard files developed by T Caschera
     
 Teensy_Continuity_Tester Folder
 
     Matrix_Decoder_LC.ino        Teensyduino file for Teensy LC continuity tester. 
     
     Matrix_Decoder_3p2.ino       Teensyduino file for Teensy 3.2 continuity tester.  
     
     Matrix_Decoder_4p0.ino       Teensyduino file for Teensy 4.0 continuity tester.
     
     Matrix_Decoder_2pp.ino       Teensyduino file for Teensy ++2.0 continuity tester.
     
     Matrix_Decoder_LC_alternate.ino   Alternate code allows exclusion of grounded pins. 
     
     Matrix_Decoder_3p2_alternate.ino   Alternate code allows exclusion of grounded pins.  
     
     Matrix_Decoder_4p0_alternate.ino   Alternate code allows exclusion of grounded pins.
     
     Matrix_Decoder_French Folder has above files but sends a shift key along with the number key.
     
 Example_Touchpads Folder
 
     Touchpad_3p2.ino             Teensyduino file for Teensy 3.2 touchpad controller.
     
     Touchpad_LC.ino              Teensyduino file for Teensy LC touchpad controller.
     
     Touchpad_pinout_testing.pdf    Describes how to find the clock, data, power, and ground.
 
 Example_Keyboards Folder
 
     Instructions for Modifying the Teensyduino LC code.pdf     PDF file with LC instructions.   
     
     Instructions for Modifying the Teensyduino 3p2 code.pdf    PDF file with 3.2 instructions.
     
     Instructions for Modifying the Teensyduino 4p0 code.pdf    PDF file with 4.0 instructions.
     
     Laptop_Keyboard      A separate Folder for each laptop keyboard, containing the following:
     
          Text file with FPC pin connections. The results from running the Matrix_decoder code.
          
          PDF file with key matrix table and any additional information. 
          
          Teensyduino code that makes a simple homebrew USB keyboard routine.
          
