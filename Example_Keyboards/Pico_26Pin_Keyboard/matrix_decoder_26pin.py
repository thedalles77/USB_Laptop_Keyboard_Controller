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
# This program is used to decode the key matrix of a laptop keyboard that is connected to the basic
# 26 pin Raspberry Pi Pico board (no port expander). The program cycles through all the possible FPC pin
# combinations looking for a connection when a key is pressed. Open an editor with a text file that lists all
# the keyboard keys. The program will send over USB, the two GPIO pin numbers that are connected when a key is pressed.
# Once all keys have been tested, the editor will contain a complete listing of the keyboard connections.
#
import time
import board
import digitalio
import usb_hid
from adafruit_hid.keyboard import Keyboard
from adafruit_hid.keycode import Keycode
from adafruit_hid.keyboard_layout_us import KeyboardLayoutUS

# Initialize USB HID Keyboard emulation
kbd = Keyboard(usb_hid.devices)
layout = KeyboardLayoutUS(kbd)

# The 26 Pico GPIO numbers connected to the keyboard FPC connector.
# GP23, GP24 and GP25 are used inside the Pico and are not available.
pico_gp_numbers = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
                   13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 26, 27, 28]
NUM_PINS = len(pico_gp_numbers)

# Set up all the Pico pins as inputs with pull-ups
pico_gpio = []
for gp in pico_gp_numbers:
    io = digitalio.DigitalInOut(getattr(board, "GP%d" % gp))
    io.direction = digitalio.Direction.INPUT
    io.pull = digitalio.Pull.UP
    pico_gpio.append(io)

def pin_label(pin_index):
    """Text label for a pin, for example Pico_GP8"""
    return "Pico_GP%d" % pico_gp_numbers[pin_index]

def set_pin_active_low(pin_index):
    """Drives one of the 26 pins LOW. All other pins stay inputs with pull-ups."""
    pico_gpio[pin_index].direction = digitalio.Direction.OUTPUT
    pico_gpio[pin_index].value = False

def reset_pin_to_input(pin_index):
    """Resets a given pin back to an input state with internal pull-up."""
    pico_gpio[pin_index].direction = digitalio.Direction.INPUT
    pico_gpio[pin_index].pull = digitalio.Pull.UP

def check_all_inputs(drive_pin):
    """Scans all pins to see if any are pulled LOW by the driven pin."""
    for i in range(NUM_PINS):
        if i == drive_pin:
            continue
        if not pico_gpio[i].value:  # low = connected to the driven pin
            return i
    return None

def pin_still_connected(drive_pin, detected_pin):
    """Returns True if the two pins are still connected (key still pressed)."""
    set_pin_active_low(drive_pin)
    time.sleep(0.002)
    connected = not pico_gpio[detected_pin].value
    reset_pin_to_input(drive_pin)
    return connected

print("Starting 26-Pin Matrix Scanner...")

# --- BEGIN BOOT CALIBRATION BLOCK ---
ignored_shorts = set()
print("\n--- INITIALIZING BOOT CALIBRATION ---")
print("DO NOT PRESS ANY KEYS! Detecting static grounds and LED paths...")

for calibration_run in range(3):
    for drive_pin in range(NUM_PINS):
        set_pin_active_low(drive_pin)
        time.sleep(0.002)
        detected_pin = check_all_inputs(drive_pin)
        reset_pin_to_input(drive_pin)
        if detected_pin is not None:
            pair = (min(drive_pin, detected_pin), max(drive_pin, detected_pin))
            if pair not in ignored_shorts:
                print(f"Ignoring {pin_label(pair[0])} + {pin_label(pair[1])}")
            ignored_shorts.add(pair)
    time.sleep(0.1)

print(f"Calibration complete. Found {len(ignored_shorts)} static paths to ignore.")
print("--- SCANNING STARTED (You can now press keys) ---\n")
# --- END BOOT CALIBRATION BLOCK ---

while True:
    # Cycle sequentially through all 26 pins
    for drive_pin in range(NUM_PINS):
        set_pin_active_low(drive_pin)
        time.sleep(0.002) # Short settle time for capacitive matrix lines

        detected_pin = check_all_inputs(drive_pin)
        reset_pin_to_input(drive_pin)

        if detected_pin is not None:
            pair = (min(drive_pin, detected_pin), max(drive_pin, detected_pin))
            if pair in ignored_shorts:
                continue

            output_string = f"{pin_label(drive_pin)} + {pin_label(detected_pin)}"

            # Print to the Thonny Serial console for immediate logging
            print(f"Detected matrix short: {output_string}")

            try:
                # Automatically type the pin connection into your open text document
                layout.write(output_string)

                # Tap the down arrow to prepare the text editor for the next row
                kbd.send(Keycode.DOWN_ARROW)
            except Exception as e:
                print(f"USB HID string transmission failed: {e}")

            # Wait until the key is released so it is only sent once,
            # then wait a little longer to ignore contact bounce
            while pin_still_connected(drive_pin, detected_pin):
                time.sleep(0.01)
            time.sleep(0.05)
            break

    time.sleep(0.01)
