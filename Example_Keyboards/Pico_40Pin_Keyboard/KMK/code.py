# Copyright 2026 Frank Adams
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#     http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# KMK code.py for a MacBook Pro A1286 keyboard on the Pi Pico + MCP23018 port expander board.
# Holding Fn turns F10, F11, F12 into Mute, Volume Down, Volume Up.
#
# Files on the CIRCUITPY drive:
#   code.py                      (this file)
#   pico_mcp23018_scanner.py     (custom matrix scanner for the port expander board)
#   kmk/                         (the KMK firmware folder)
#
# Pin numbers: 0 - 28 = Pico GPIO, 100 - 107 = MCP23018 GPA0 - GPA7, 108 - 115 = GPB0 - GPB7

import board
import busio

from kmk.extensions.media_keys import MediaKeys
from kmk.keys import KC
from kmk.kmk_keyboard import KMKKeyboard
from kmk.modules.layers import Layers

from pico_mcp23018_scanner import PicoMCP23018MatrixScanner

# MCP23018 port expander on the Pico's I2C1: GP2 = SDA, GP3 = SCL (4.7K pull-ups on the board)
i2c = busio.I2C(scl=board.GP3, sda=board.GP2, frequency=400_000)

# Same row and column order as the QMK and Arduino code for this keyboard
ROW_PINS = (101, 20, 112, 9, 102, 10, 103, 19, 111, 11, 110, 18, 105, 14, 107, 15)
COL_PINS = (100, 7, 113, 8, 17, 109, 16, 106, 108)

keyboard = KMKKeyboard()
keyboard.matrix = PicoMCP23018MatrixScanner(row_pins=ROW_PINS, col_pins=COL_PINS, i2c=i2c)
keyboard.modules.append(Layers())
keyboard.extensions.append(MediaKeys())

FN = KC.MO(1)
XXXXXXX = KC.NO
_______ = KC.TRNS

keyboard.keymap = [
    # Layer 0: base layer (16 rows x 9 columns)
    [
      # GPA0     GP7      GPB5     GP8      GP17     GPB1     GP16     GPA6     GPB0
        KC.Q,    KC.N1,   KC.A,    KC.Z,    KC.F11,  KC.BSPC, KC.F12,  KC.F1,   XXXXXXX,   # GPA1
        KC.W,    KC.N2,   KC.S,    KC.X,    KC.ESC,  KC.TAB,  KC.GRV,  KC.F2,   XXXXXXX,   # GP20
        KC.E,    KC.N3,   KC.D,    KC.C,    XXXXXXX, XXXXXXX, XXXXXXX, KC.F3,   XXXXXXX,   # GPB4
        KC.Y,    KC.N6,   KC.H,    KC.N,    KC.SLSH, KC.SPC,  KC.EQL,  KC.F6,   XXXXXXX,   # GP9
        KC.R,    KC.N4,   KC.F,    KC.V,    XXXXXXX, XXXXXXX, KC.RBRC, KC.F4,   XXXXXXX,   # GPA2
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC.ENT,  XXXXXXX, XXXXXXX, KC.RSFT, XXXXXXX,   # GP10
        XXXXXXX, XXXXXXX, XXXXXXX, KC.LGUI, KC.N0,   XXXXXXX, KC.RGUI, XXXXXXX, XXXXXXX,   # GPA3
        KC.T,    KC.N5,   KC.G,    KC.B,    KC.MINS, FN,      KC.LBRC, KC.F5,   XXXXXXX,   # GP19
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC.F10,  KC.RALT, XXXXXXX, XXXXXXX, XXXXXXX,   # GPB3
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC.P,    KC.CAPS, XXXXXXX, XXXXXXX, XXXXXXX,   # GP11
        KC.U,    KC.N7,   KC.J,    KC.M,    KC.SCLN, KC.QUOT, XXXXXXX, KC.F7,   XXXXXXX,   # GPB2
        KC.I,    KC.N8,   KC.K,    KC.COMM, XXXXXXX, KC.LEFT, KC.RGHT, KC.F8,   XXXXXXX,   # GP18
        KC.O,    KC.N9,   KC.L,    KC.DOT,  KC.BSLS, KC.DOWN, KC.UP,   KC.F9,   XXXXXXX,   # GPA5
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC.LSFT,   # GP14
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC.LALT,   # GPA7
        XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC.LCTL,   # GP15
    ],
    # Layer 1: Fn held. F10 = Mute, F11 = Volume Down, F12 = Volume Up
    [
      # GPA0     GP7      GPB5     GP8      GP17     GPB1     GP16     GPA6     GPB0
        _______, _______, _______, _______, KC.VOLD, _______, KC.VOLU, _______, _______,   # GPA1
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP20
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GPB4
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP9
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GPA2
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP10
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GPA3
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP19
        _______, _______, _______, _______, KC.MUTE, _______, _______, _______, _______,   # GPB3
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP11
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GPB2
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP18
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GPA5
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP14
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GPA7
        _______, _______, _______, _______, _______, _______, _______, _______, _______,   # GP15
    ],
]

if __name__ == '__main__':
    keyboard.go()
