from enum import Enum
import os

"""
    Copyright 2019 Marcel Hillesheim
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
"""

"""
author: Marcel Hillesheim with modifications by Frank Adams to work with KMK

input:
Put the key list text file in the same folder as this Python program (so it's easy to find).
output:
Matrix column and row I/O's
and
Main key matrix

Description:
https://www.hackster.io/frank-adams/usb-laptop-keyboard-controller-solder-party-rp2350-stamp-xl-4fa642

Little tool to help creating a key matrix for a usb-keyboards using KMK.

Lenovo keyboards use a single row pin and a single column pin for the FN key which causes this program
to not place the FN key in the matrix. Manually place FN in the matrix (without the KC. in front)
It's not pretty but it's functional :d
"""

# 2350
con_pin_2350 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 26, 27,
              28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42]
kmk_devices = [('2350', con_pin_2350)]

separator = "-----------------------------------------------------\n"


class KeyType(Enum):
    KEY = 1
    Func = 2
    MODIFIER = 3
    # for all-one matrix
    ONE = 4


class Key:
    is_assigned = False

    def __init__(self, label, modifier_value, pin1, pin2):
        self.label = label
        self.pin1 = pin1
        self.pin2 = pin2
        self.type = KeyType.KEY
        # generate key type from label
        for key_type in KeyType:
            if label.startswith(key_type.name):
                self.type = key_type
        # check if user set the key type
        for key_type in KeyType:
            if modifier_value == key_type.name:
                self.type = key_type

def generate_matrix(path, con_pin):
    file = open(path)
    content = file.readlines()

    keys = []

    # result list for input pins
    input_pins = []
    # result list for output pins
    output_pins = []

    content = [x.strip() for x in content]

    # read in file and store values line by line into key objects
    for line in content:
        line = line.split()
        if len(line) >= 3:
            keys.append(Key(line[0], line[1], int(line[-1]), int(line[-2])))
    # initialize matrix creator by finding common pins of control key
    temporary = [keys[0].pin1, keys[1].pin1, keys[0].pin2, keys[1].pin2]
    # determine if there is a common pin
    for pin in temporary:
        if temporary.count(pin) == 2:
            output_pins.append(pin)

    # if no common pin found ask user for initial input
    if not output_pins:
        output_pins.append(int(input("No common pin found for the Control keys. Please enter an output (row) pin for a Control key: ")))

    print("initial output pin: {}".format(output_pins[0]))
    # iterate until no new output pins or input pins are found
    found = True
    while found:
        found = False
        for key in keys:
            # if not already assigned
            if not key.is_assigned:
                # set partner pin to the opposite array e.g. pin1 output pin -> pin2 input pin
                if key.pin1 in output_pins:
                    input_pins.append(key.pin2)
                    key.is_assigned = True
                    found = True
                elif key.pin1 in input_pins:
                    output_pins.append(key.pin2)
                    key.is_assigned = True
                    found = True
                elif key.pin2 in output_pins:
                    input_pins.append(key.pin1)
                    key.is_assigned = True
                    found = True
                elif key.pin2 in input_pins:
                    output_pins.append(key.pin1)
                    key.is_assigned = True
                    found = True

    input_pins = list(set(input_pins))
    output_pins = list(set(output_pins))
    input_pins.sort()
    output_pins.sort()
    # Output results
    print(separator + "Results:\n" + separator)
    # print GPIO pins
    print("GPIO PINS:")
    print("\n" + str(len(input_pins)) + " input column GPIO's:")
    print(input_pins)
    print("\n" + str(len(output_pins)) + " output row GPIO's:")
    print(output_pins)
    print(separator)

    # create the different matrices for every key type
    for key_type in KeyType:
        matrix = separator + key_type.name + "\n" + separator + "{\n"
        # rows
        for output_pin in output_pins:
            key_row = ""
            # columns
            for input_pin in input_pins:
                # default key value
                key_label = "KC.NO"
                if key_type == KeyType.ONE:
                    key_label = "1"
                # search for key that matches with row and column pin
                for key in keys:
                    if (((key.pin1 == input_pin) | (key.pin2 == input_pin)) & (
                            (key.pin1 == output_pin) | (key.pin2 == output_pin))):
                        if key.type == key_type:
                            key_label = key.label
                key_row = key_row + key_label + ","
            matrix = matrix + "{" + key_row[:-1] + "},\n"
        matrix = matrix[:-1] + "\n}"
        print(matrix)
    print(separator + "Finished")
    
if __name__ == "__main__":
    default_file_names = ["Keyboard_pin_list_KMK.txt"]
    file_suggestions = []

    for file_name in os.listdir():
        if file_name.endswith('.txt'):
            if file_name in default_file_names:
                file_suggestions.insert(0, file_name)
            else:
                file_suggestions.append(file_name)
    max_length = max(len(file_name) for file_name in file_suggestions)
    print('{:^6s} {:^{max_length}s}'.format('index', 'file name', max_length=max_length))
    for i, suggestion in enumerate(file_suggestions):
        print('{:^6s} {:^{max_length}s}'.format(str(i + 1), suggestion, max_length=max_length))
    while True:
        user_input = input(
            separator + 'Enter the index number of the *.txt file you want.\nOR: enter your own filepath:\n')
        if os.path.exists(user_input):
            path = user_input
            break
        elif user_input.isdigit():
            path = file_suggestions[int(user_input) - 1]
            break
        else:
            print('Couldn\'t handle input. Please try again.')

    print(separator)
    print('{:^6s} {:^12s}'.format('index', 'device'))
    for i, device in enumerate(kmk_devices):
        print('{:^6s} {:^12s}'.format(str(i + 1), device[0]))

    while True:
        user_input = input(
            separator + 'Please enter the index number of your device:\n')
        # TODO custom pin layout
        if user_input.isdigit():
            con_pin = kmk_devices[int(user_input) - 1][1]
            break
        else:
            print('Couldn\'t handle input. Please try again.')
    generate_matrix(path, con_pin)