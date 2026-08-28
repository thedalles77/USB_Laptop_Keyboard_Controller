#   Copyright 2026 Frank Adams
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#       http://www.apache.org/licenses/LICENSE-2.0
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
# This program is used to decode the key matrix of a laptop keyboard that is connected to the Raspberry Pi Pico W 
# with a MCP23018 Port Expander. The program cycles through all the possible FPC pin 
# combinations looking for a connection when a key is pressed. Open an editor with a text file that lists all 
# the keyboard keys. The program will send over USB, the two GPIO pin numbers that are connected when a key is pressed.
# Once all keys have been tested, the editor will contain a complete listing of the keyboard connections. 
# 
import time
import board
import busio
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode

# Initialize USB HID Keyboard emulation
kbd = Keyboard(usb_hid.devices)

# Initialize I2C Bus on GP2 (SDA) and GP3 (SCL)
# The MCP23018 has 4.7KΩ external pull-up resistors on SDA and SCL
i2c = busio.I2C(board.GP3, board.GP2)

# MCP23018 I2C Registers (Address is 0x20 when ADDR pin 15 is grounded)
MCP23018_ADDR = 0x20
IODIRA = 0x00   # IO Direction Register A (0=Output, 1=Input)
IODIRB = 0x01   # IO Direction Register B
GPPUA  = 0x0C   # Pull-up Resistor Configuration Register A (1=Enabled)
GPPUB  = 0x0D   # Pull-up Resistor Configuration Register B
GPIOA  = 0x12   # General Purpose IO Register A
GPIOB  = 0x13   # General Purpose IO Register B

def init_mcp23018():
    """Initializes all 16 pins of the MCP23018 as inputs with internal pull-ups enabled."""
    while not i2c.try_lock():
        pass
    try:
        # Set all 16 pins as Inputs (1 = Input)
        i2c.writeto(MCP23018_ADDR, bytes([IODIRA, 0xFF]))
        i2c.writeto(MCP23018_ADDR, bytes([IODIRB, 0xFF]))
        # Enable internal pull-up resistors (1 = Pull-up Enabled)
        i2c.writeto(MCP23018_ADDR, bytes([GPPUA, 0xFF]))
        i2c.writeto(MCP23018_ADDR, bytes([GPPUB, 0xFF]))
    finally:
        i2c.unlock()

# Call the initialization helper
init_mcp23018()

# Define the 24 remaining native Pico GPIO pins
pico_pins = [
    board.GP0, board.GP1, board.GP4, board.GP5, board.GP6, board.GP7,
    board.GP8, board.GP9, board.GP10, board.GP11, board.GP12, board.GP13,
    board.GP14, board.GP15, board.GP16, board.GP17, board.GP18, board.GP19,
    board.GP20, board.GP21, board.GP22, board.GP26, board.GP27, board.GP28
]

# Set up the native Pico digital IO instances as inputs with pull-ups
pico_gpio = []
for pin in pico_pins:
    io = digitalio.DigitalInOut(pin)
    io.direction = digitalio.Direction.INPUT
    io.pull = digitalio.Pull.UP
    pico_gpio.append(io)

def write_mcp_register(reg, value):
    """Helper function to write a single byte to an MCP23018 register."""
    while not i2c.try_lock():
        pass
    try:
        i2c.writeto(MCP23018_ADDR, bytes([reg, value]))
    finally:
        i2c.unlock()

def read_mcp_registers():
    """Helper function to read the 16-bit state of GPIOA and GPIOB using native CircuitPython."""
    while not i2c.try_lock():
        pass
    try:
        # Buffer to store 1 byte for GPIOA
        buf_a = bytearray(1)
        i2c.writeto_then_readfrom(MCP23018_ADDR, bytes([GPIOA]), buf_a)

        # Buffer to store 1 byte for GPIOB
        buf_b = bytearray(1)
        i2c.writeto_then_readfrom(MCP23018_ADDR, bytes([GPIOB]), buf_b)

        # Pull the raw numeric values out of the buffers using [0] before combining
        return buf_a[0] | (buf_b[0] << 8)
    finally:
        i2c.unlock()

def set_pin_active_low(pin_index):
    """
    Configures a single target pin out of the 40 possibilities to drive LOW.
    Indices 0-23 are native Pico pins. Indices 24-39 are MCP23018 pins.
    """
    if pin_index < 24:
        # Drive native Pico pin LOW
        pico_gpio[pin_index].direction = digitalio.Direction.OUTPUT
        pico_gpio[pin_index].value = False
    else:
        # Convert pin index to MCP pin (0-15)
        mcp_pin = pin_index - 24
        if mcp_pin < 8:
            # Pin is on PORTA: change IODIRA to make it an output (0)
            # Leave other pins as input (1)
            reg_mask = 0xFF & ~(1 << mcp_pin)
            write_mcp_register(IODIRA, reg_mask)
            write_mcp_register(GPIOA, 0x00) # Open drain pulls down
        else:
            mcp_pin -= 8
            reg_mask = 0xFF & ~(1 << mcp_pin)
            write_mcp_register(IODIRB, reg_mask)
            write_mcp_register(GPIOB, 0x00)

def reset_pin_to_input(pin_index):
    """Resets a given pin index back to an input state with internal pull-up."""
    if pin_index < 24:
        pico_gpio[pin_index].direction = digitalio.Direction.INPUT
        pico_gpio[pin_index].pull = digitalio.Pull.UP
    else:
        # Reset entire MCP chip registers back to safe default input state
        write_mcp_register(IODIRA, 0xFF)
        write_mcp_register(IODIRB, 0xFF)

def check_all_inputs(drive_pin):
    """Scans all pins to see if any are pulled LOW by the driven pin."""
    # 1. Check native Pico inputs
    for i in range(24):
        if i == drive_pin:
            continue
        if not pico_gpio[i].value:
            return i
            
    # 2. Check MCP23018 inputs
    mcp_state = read_mcp_registers()
    for mcp_pin in range(16):
        overall_idx = mcp_pin + 24
        if overall_idx == drive_pin:
            continue
        # If a bit is 0, the pin is shorted to our driven active-low pin
        if not (mcp_state & (1 << mcp_pin)):
            return overall_idx
            
    return None

print("Starting 40-Pin Matrix Scanner...")

while True:
    # Cycle sequentially through all 40 physical connections
    for drive_pin in range(40):
        set_pin_active_low(drive_pin)
        time.sleep(0.002) # Short settle time for capacitive matrix lines
        
        detected_pin = check_all_inputs(drive_pin)
        reset_pin_to_input(drive_pin)
        
        if detected_pin is not None:
            # Determine text labels for mapping feedback
            label1 = f"Pico_GP{pico_pins[drive_pin]}" if drive_pin < 24 else f"MCP_Pin{drive_pin-24}"
            label2 = f"Pico_GP{pico_pins[detected_pin]}" if detected_pin < 24 else f"MCP_Pin{detected_pin-24}"
            
            output_string = f"{label1} + {label2}"
            
        # Key sending block 
        if key_name:
            # Check if the key name is alphanumeric without using .isalnum()
            # This handles single letters, numbers, and common uppercase names
            is_alphanumeric = False
            if len(key_name) == 1:
                c = key_name.lower()
                if ("a" <= c <= "z") or ("0" <= c <= "9"):
                    is_alphanumeric = True
            elif key_name.upper() in ["ENTER", "SPACE", "TAB", "BACKSPACE", "DELETE", "ESCAPE"]:
                is_alphanumeric = True

            if is_alphanumeric:
                try:
                    keycode = getattr(Keycode, key_name.upper())
                    keyboard.press(keycode)
                    keyboard.release(keycode)
                except AttributeError:
                    print(f"Keycode for {key_name} not found.")
            else:
                print(f"Custom key action triggered: {key_name}")            

# Send the down arrow command to prep the text editor for the next keypress
            kbd.send(Keycode.DOWN_ARROW)
            
            # Debounce delay loop to avoid flooding text fields while holding a test key down
            time.sleep(0.5)
            break 
            
    time.sleep(0.01)

