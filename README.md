# USB_Laptop_Keyboard_Controller
 This repo contains all the files that are needed to make a USB keyboard and a KVM from an old laptop.

 See the YouTube keyboard video at https://www.youtube.com/watch?v=Z1PheqSNNP8
 
 See the YouTube KVM video at https://www.youtube.com/watch?v=M5-dYo2jt14
 
 See the keyboard "Instructable" at https://www.instructables.com/id/How-to-Make-a-USB-Laptop-Keyboard-Controller/

 See the KVM "Instructable" at https://www.instructables.com/id/DIY-Portable-KVM-Crashcart-From-Recycled-Laptop-Pr/
 
 Marcel's Python matrix generator program is at:
 
 https://github.com/marcelhillesheim/USB_Laptop_Keyboard_Controller
 
 All files and folders are listed below:
 
 Instructable Laptop Keyboard Conversion to USB.pdf contains the text and pictures from the keyboard Instructable.
 
 KVM Instructable.pdf contains the text and pictures from the KVM Instuctable
 
 Keyboard_Scanner.brd  EagleCad circuit board file for Teensy LC or 3.2. 
 
 Keyboard_Scanner_LT.brd EagleCad file same as above with 2 bit Level Translator for touchpad Clock and Data
 
 Keyboard_Scanner_LT_0p5.brd EagleCad file same as above except made for a 0.5mm pitch FPC connector
     
 Blank_Key_Lists Folder
 
     Keyboard_with_number_pad.txt     Lists all keys on a keyboard with a number pad. 
     
     Keyboard_without_number_pad.txt  Lists all keys on a keyboard without a number pad. 
     
 Commodore_64 Folder
 
     Teensy LC code and key list files developed by Olga for a Commodore 64, see Readme file.  
 
 GRID 1550 Folder
 
     Teensy LC code and PDF with key matrix for a GRID 1550 with a UK keyboard layout.
     
 T Caschy Folder
 
     HP Jornada and Atari Portfolio keyboard files developed by T Caschera
     
 Teensy_Continuity_Tester Folder
 
     Matrix_Decoder_LC.ino        Teensyduino file for Teensy LC continuity tester. 
     
     Matrix_Decoder_3p2.ino       Teensyduino file for Teensy 3.2 continuity tester.  
     
 Example_Touchpads Folder
 
     Touchpad_3p2.ino             Teensyduino file for Teensy 3.2 touchpad controller
     
     Touchpad_LC.ino              Teensyduino file for Teensy LC touchpad controller
     
     Touchpad_pinout_testing.pdf    Describes how to find the clock, data, power, and ground
 
 Example_Keyboards Folder
 
     Instructions for Modifying the Teensyduino LC code.pdf     PDF file with LC instructions   
     
     Instructions for Modifying the Teensyduino 3p2 code.pdf    PDF file with 3.2 instructions 
     
     Laptop_Keyboard      A separate Folder for each laptop keyboard, containing the following:
     
          Text file with FPC pin connections. The results from running the Matrix_decoder code.
          
          PDF file with key matrix table and any additional information. 
          
          Teensyduino code that makes a simple homebrew USB keyboard routine.
          
 Each of the Laptop Keyboards listed below have been tested and documented in their own folder:
*  Dell Inspiron 1525 - Keyboard Part Number D9K01 - This includes Marcel's Python results
*  Dell Latitude 131L - Keyboard Part Number V-0511BIAS1-US
*  Dell Latitude X1 - Keyboard Part Number 0M6607
*  Dell Latitude D630 - Keyboard Part Number DP/N 0DR160 - this includes the basic and Pi KVM
*  HP Compaq Presario 2100 - Keyboard Part Number AEKT1TPU011 - This includes Marcel's Python results
*  HP Compaq Presario V4000 - Keyboard Part Number NSK-H3L01
*  HP Pavilion DV9000 - Keyboard Part number AEAT5U00110 - This includes Marcel's Python results
*  Sony Vaio PCG-K25 - Keyboard Part Number KFRMBA151B
*  Sony Vaio VPCCW - Keyboard Part Number 148754321
*  Sony Vaio VPCEA - Keyboard part number A-1765-621-A
*  Sony Vaio VPCEB4 – Keyboard part number A-1766-425-A
*  Lenovo ThinkPad T61 - Keyboard part number 42T3177 (Teensy 3p2 and Teensy LC folders)
