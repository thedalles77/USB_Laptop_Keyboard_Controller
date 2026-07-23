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
#   The following GPIO connections and key matrix are for a Dell 1545 laptop keyboard.
#   Use this KMK code as your starting point if using a different keyboard.
#   Change the name to code.py when copying this file to the Pi Pico.
#   See step 16 of my Instructable for more information.
#
import board
import microcontroller
from kmk.kmk_keyboard import KMKKeyboard
from kmk.keys import KC
from kmk.scanners import DiodeOrientation
from kmk.extensions.media_keys import MediaKeys
from kmk.modules.layers import Layers

keyboard = KMKKeyboard()
# jumper wires were used to connect the FPC pin number to the GPIO number on the ESP-S3 as follows:
# Note that pin numbering on the FPC adapter board was not per the silkscreen. I started with pin1 on the opposite side.
# column FPC pin# 18 19 20 21 22 23 24 25
# column GPIO #   01 04 38 39 40 41 42 02

# row FPC pin # 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17
# row GPIO #    03 46 05 06 07 08 09 10 11 12 13 14 15 16 17 18 21

keyboard.col_pins = (microcontroller.pin.GPIO1, microcontroller.pin.GPIO4, microcontroller.pin.GPIO38, microcontroller.pin.GPIO39,
                     microcontroller.pin.GPIO40, microcontroller.pin.GPIO41, microcontroller.pin.GPIO42, microcontroller.pin.GPIO2)
keyboard.row_pins = (microcontroller.pin.GPIO3, microcontroller.pin.GPIO46, microcontroller.pin.GPIO5, microcontroller.pin.GPIO6,
                     microcontroller.pin.GPIO7, microcontroller.pin.GPIO8, microcontroller.pin.GPIO9, microcontroller.pin.GPIO10,
                     microcontroller.pin.GPIO11, microcontroller.pin.GPIO12, microcontroller.pin.GPIO13, microcontroller.pin.GPIO14,
                     microcontroller.pin.GPIO15, microcontroller.pin.GPIO16, microcontroller.pin.GPIO17, microcontroller.pin.GPIO18,
                     microcontroller.pin.GPIO21)
keyboard.diode_orientation = DiodeOrientation.ROW2COL # rows are inputs because IO3 & 46 must be inputs on the ESP32-S3

keyboard.modules.append(Layers())
keyboard.extensions.append(MediaKeys())

FN = KC.TG(1)

keyboard.keymap = [
    [#layer 0: Base Layer
        KC.NO,   KC.INS,  KC.NO,   KC.F12,  KC.NO,   KC.NO,   KC.NO,   KC.RGHT,
		KC.NO,   KC.DEL,  KC.NO,   KC.F11,  KC.NO,   KC.NO,   KC.NO,   KC.DOWN,
		KC.UP,   KC.HOME, KC.APP,  KC.END,  KC.NO,   KC.NO,   KC.NO,   KC.LEFT,
        KC.NO,   KC.F8,   KC.F7,   KC.N9,   KC.O,    KC.L,    KC.DOT,  KC.NO,
		KC.QUOT, KC.MINS, KC.LBRC, KC.N0,   KC.P,    KC.SCLN, KC.NO,   KC.SLSH,
        KC.F6,   KC.EQL,  KC.RBRC, KC.N8,   KC.I,    KC.K,    KC.COMM, KC.NO,
        KC.H,    KC.N6,   KC.Y,    KC.N7,   KC.U,    KC.J,    KC.M,    KC.N,
        KC.F5,   KC.F9,   KC.BSPC, KC.F10,  KC.NO,   KC.BSLS, KC.ENT,  KC.SPC,
		KC.G,    KC.N5,   KC.T,    KC.N4,   KC.R,    KC.F,    KC.V,    KC.B,
        KC.F4,   KC.F2,   KC.F3,   KC.N3,   KC.E,    KC.D,    KC.C,    KC.NO,
        KC.NO,   KC.F1,   KC.CAPS, KC.N2,   KC.W,    KC.S,    KC.X,    KC.NO,
        KC.ESC,  KC.GRV,  KC.TAB,  KC.N1,   KC.Q,    KC.A,    KC.Z,    KC.NO,
        KC.LALT, KC.NO,   KC.NO,   KC.PSCR, KC.NO,   KC.NO,   KC.NO,   KC.RALT,
        KC.NO,   KC.NO,   KC.LSFT, KC.NO,   KC.NO,   KC.NO,   KC.RSFT, KC.NO,
        KC.NO,   KC.LCTL, KC.NO,   KC.NO,   KC.NO,   KC.NO,   KC.RCTL, KC.NO,
        KC.NO,   KC.NO,   KC.NO,   KC.LGUI, KC.PGUP, KC.PGDN, KC.NO,   KC.NO,
        KC.NO,   KC.NO,   KC.NO,   KC.NO,   KC.NO,   FN,      KC.NO,   KC.NO,        
	],
	[#layer 1: Fn Media Layer
        KC.NO,   KC.INS,  KC.NO,   KC.MNXT, KC.NO,   KC.NO,   KC.NO,   KC.RGHT,
		KC.NO,   KC.DEL,  KC.NO,   KC.F11,  KC.NO,   KC.NO,   KC.NO,   KC.DOWN,
		KC.UP,   KC.HOME, KC.APP,  KC.END,  KC.NO,   KC.NO,   KC.NO,   KC.LEFT,
        KC.NO,   KC.VOLD, KC.MUTE, KC.N9,   KC.O,    KC.L,    KC.DOT,  KC.NO,
		KC.QUOT, KC.MINS, KC.LBRC, KC.N0,   KC.P,    KC.SCLN, KC.NO,   KC.SLSH,
        KC.F6,   KC.EQL,  KC.RBRC, KC.N8,   KC.I,    KC.K,    KC.COMM, KC.NO,
        KC.H,    KC.N6,   KC.Y,    KC.N7,   KC.U,    KC.J,    KC.M,    KC.N,
        KC.BRIU, KC.VOLU, KC.BSPC, KC.MPRV, KC.NO,   KC.BSLS, KC.ENT,  KC.SPC,
		KC.G,    KC.N5,   KC.T,    KC.N4,   KC.R,    KC.F,    KC.V,    KC.B,
        KC.BRID, KC.F2,   KC.F3,   KC.N3,   KC.E,    KC.D,    KC.C,    KC.NO,
        KC.NO,   KC.F1,   KC.CAPS, KC.N2,   KC.W,    KC.S,    KC.X,    KC.NO,
        KC.ESC,  KC.GRV,  KC.TAB,  KC.N1,   KC.Q,    KC.A,    KC.Z,    KC.NO,
        KC.LALT, KC.NO,   KC.NO,   KC.PSCR, KC.NO,   KC.NO,   KC.NO,   KC.RALT,
        KC.NO,   KC.NO,   KC.LSFT, KC.NO,   KC.NO,   KC.NO,   KC.RSFT, KC.NO,
        KC.NO,   KC.LCTL, KC.NO,   KC.NO,   KC.NO,   KC.NO,   KC.RCTL, KC.NO,
        KC.NO,   KC.NO,   KC.NO,   KC.LGUI, KC.PGUP, KC.PGDN, KC.NO,   KC.NO,
        KC.NO,   KC.NO,   KC.NO,   KC.NO,   KC.NO,   FN,      KC.NO,   KC.NO,
	],
	
]
if __name__ == '__main__':
    keyboard.go()
