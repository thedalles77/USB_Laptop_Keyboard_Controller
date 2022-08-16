The driver works perfect for this keyboard.

I had a problem with spanish layout (105 keys) where key for '<' and '>' symbols (between left shift and 'Z' key) wasn't recognized by driver.

After studing Teensy library keylayout.h file I could find the solution:

That extra key is KEY_NON_US_BS (instead of KEY_NON_US_100). Using this constant in matrix, spanish layout works great!

Very important:
When using **Marcel's Python matrix generator program** you must press keys like you were using english layout keyboard and then in Windows, selecting your language layout (spanish for me) everything works OK (locale symbols shows ok, i.e. 'ñ', '¿', '¡').


[![](https://github.com/NitemareReal/USB_Laptop_Keyboard_Controller/blob/master/Example_Keyboards/Lenovo_G550/IMG_20220803_155138292_2.jpg)](https://github.com/NitemareReal/USB_Laptop_Keyboard_Controller/blob/master/Example_Keyboards/Lenovo_G550/IMG_20220803_155138292_2.jpg)

[![](https://github.com/NitemareReal/USB_Laptop_Keyboard_Controller/blob/master/Example_Keyboards/Lenovo_G550/IMG_20220803_155037375_2.jpg)](https://github.com/NitemareReal/USB_Laptop_Keyboard_Controller/blob/master/Example_Keyboards/Lenovo_G550/IMG_20220803_155037375_2.jpg)

[![](https://github.com/NitemareReal/USB_Laptop_Keyboard_Controller/blob/master/Example_Keyboards/Lenovo_G550/IMG_20220803_155308305_HDR_2.jpg)](https://github.com/NitemareReal/USB_Laptop_Keyboard_Controller/blob/master/Example_Keyboards/Lenovo_G550/IMG_20220803_155308305_HDR_2.jpg)
