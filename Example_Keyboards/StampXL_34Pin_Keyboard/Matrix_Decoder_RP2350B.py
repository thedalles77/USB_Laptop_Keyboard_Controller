# This program is used to decode the key matrix of a laptop keyboard. Use an FPC connector to connect all the 
# laptop keyboard pins to the GPIO pins of a RP2350 Stamp XL which uses a RP2350B chip and has 48 GPIO.
# The program cycles thru all the possible pin 
# combinations looking for a connection when a key is pressed. Open an editor with a text file that lists all 
# the keyboard keys. The program will send over USB, the two GPIO pin numbers that are connected when a key is pressed.
# Once all keys have been tested, the editor will contain a complete listing of the keyboard connections. 
# 
import time
import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS
from adafruit_hid.keycode import Keycode

kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)

# List the Pi Pico GP I/O pins in the array that are connected to the keyboard FPC cable.
# If the program finds that 2 pins are always connected (probably grounds or LEDs), remove them from the I_O list and
# comment them in the Send_GP function. See GP23, GP24, and GP25 in the Send_GP function for an example.

I_O = [board.GP0, board.GP1, board.GP2, board.GP3, board.GP4, board.GP5, board.GP6, board.GP7, board.GP8,  
board.GP9, board.GP10, board.GP11, board.GP12, board.GP13, board.GP14, board.GP15, board.GP16, board.GP17, 
board.GP18, board.GP19, board.GP20, board.GP21, board.GP22, board.GP23, board.GP24, board.GP25, board.GP26,
board.GP27, board.GP28, board.GP29, board.GP30, board.GP31, board.GP32, board.GP33]


# This function converts the board.GP number from the array into a text number and sends it over USB.
# It will work with up to 48 GPIO pins (for the RP2350B)
def Send_GP(GP):
    if GP == board.GP0:
        layout.write('0')
    elif GP == board.GP1:
        layout.write('1')
    elif GP == board.GP2:
        layout.write('2')
    elif GP == board.GP3:
        layout.write('3')
    elif GP == board.GP4:
        layout.write('4')
    elif GP == board.GP5:
        layout.write('5')
    elif GP == board.GP6:
        layout.write('6')
    elif GP == board.GP7:
        layout.write('7')
    elif GP == board.GP8:
        layout.write('8')
    elif GP == board.GP9:
        layout.write('9')
    elif GP == board.GP10:
        layout.write('10')
    elif GP == board.GP11:
        layout.write('11')
    elif GP == board.GP12:
        layout.write('12')
    elif GP == board.GP13:
        layout.write('13')
    elif GP == board.GP14:
        layout.write('14')
    elif GP == board.GP15:
        layout.write('15')
    elif GP == board.GP16:
        layout.write('16')
    elif GP == board.GP17:
        layout.write('17')
    elif GP == board.GP18:
        layout.write('18')
    elif GP == board.GP19:
        layout.write('19')
    elif GP == board.GP20:
        layout.write('20')
    elif GP == board.GP21:
        layout.write('21')
    elif GP == board.GP22:
        layout.write('22')
    elif GP == board.GP23:
        layout.write('23')
    elif GP == board.GP24:
        layout.write('24')
    elif GP == board.GP25:
        layout.write('25')
    elif GP == board.GP26:
        layout.write('26')
    elif GP == board.GP27:
        layout.write('27')
    elif GP == board.GP28:
        layout.write('28')
    elif GP == board.GP29:
        layout.write('29')
    elif GP == board.GP30:
        layout.write('30')
    elif GP == board.GP31:
        layout.write('31')
    elif GP == board.GP32:
        layout.write('32')
    elif GP == board.GP33:
        layout.write('33')
    elif GP == board.GP34:
        layout.write('34')
    elif GP == board.GP35:
        layout.write('35')
    elif GP == board.GP36:
        layout.write('36')
    elif GP == board.GP37:
        layout.write('37')
    elif GP == board.GP38:
        layout.write('38')
    elif GP == board.GP39:
        layout.write('39')
    elif GP == board.GP40:
        layout.write('40')
    elif GP == board.GP41:
        layout.write('41')
    elif GP == board.GP42:
        layout.write('42')
    elif GP == board.GP43:
        layout.write('43')
    elif GP == board.GP44:
        layout.write('44')
    elif GP == board.GP45:
        layout.write('45')
    elif GP == board.GP46:
        layout.write('46')
    elif GP == board.GP47:
        layout.write('47')
    else: 
        layout.write('not defined')    

while True:
    # outer loop drives a gpio pin low
    for i in range(0, len(I_O)-1): # drive each gpio pin low up to the second from the last
        row = digitalio.DigitalInOut(I_O[i])
        row.direction = digitalio.Direction.OUTPUT
        row.value = False
        # inner loop reads all gpio pins that are greater than the outer loop index number
        for j in range(i+1, len(I_O)): # read up to the last gpio pin in the array
            column = digitalio.DigitalInOut(I_O[j])
            column.direction = digitalio.Direction.INPUT
            column.pull = digitalio.Pull.UP
            if column.value == False: # key is pushed if low
                Send_GP(I_O[j])  # display inner loop gpio pin number 
                kbd.send(Keycode.TAB, Keycode.TAB)  # move over 2 tabs for next number
                Send_GP(I_O[i])  # display outer loop gpio pin number
                kbd.send(Keycode.DOWN_ARROW)  # move down to next line
                while column.value == False:  # loop until key is released to proceed
                    pass # Do nothing
            column.deinit()	 # release the column pin as an input w/ pullup
        # 
        row.deinit() # release the row pin as an output
     
    time.sleep(0.001) # small delay before repeating main loop
