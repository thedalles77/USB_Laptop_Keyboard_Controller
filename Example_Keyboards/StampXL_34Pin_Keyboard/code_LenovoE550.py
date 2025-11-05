#   Copyright 2025 Frank Adams
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
#   The following GPIO connections and key matrix are for a Lenovo E550 laptop keyboard.
#   Use this KMK code as your starting point if using a different keyboard.
#   Change the name to code.py when copying this file to the Stamp XL.
#   See step 16 of my Instructable for more information.
#
import board

from kmk.kmk_keyboard import KMKKeyboard
from kmk.keys import KC
from kmk.scanners import DiodeOrientation
from kmk.extensions.media_keys import MediaKeys
from kmk.modules.layers import Layers

keyboard = KMKKeyboard()

keyboard.col_pins = (board.GP0, board.GP1, board.GP2, board.GP4, board.GP5, board.GP7, board.GP8, board.GP11, board.GP29)
keyboard.row_pins = (board.GP3, board.GP6, board.GP9, board.GP10, board.GP12, board.GP13, board.GP14, board.GP15, 
board.GP16, board.GP17, board.GP18, board.GP19, board.GP20, board.GP21, board.GP22, board.GP23, board.GP28, board.GP30, board.GP31)
keyboard.diode_orientation = DiodeOrientation.COL2ROW # Most laptop keyboards have no diodes so ROW2COL also works

keyboard.modules.append(Layers())
keyboard.extensions.append(MediaKeys())

FN = KC.TG(1)

# Add any other keys that your keyboard has using the KMK keycodes from https://docs.qmk.fm/keycodes_basic
# and the media keys from https://github.com/KMKfw/kmk_firmware/blob/main/docs/en/media_keys.md
# All key names are preceded with KC. except the FN key. KC.NO is used when there is no key at that matrix location.
# This keyboard has a keypad and those keys start with KC.P followed by the name or number from the keycodes_basic list.

keyboard.keymap = [
    [#layer 0: Base Layer
    KC.LSHIFT,		KC.NO,		KC.RSHIFT,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
    KC.TAB,			KC.NO,		KC.Z,		KC.A,		KC.N1,		KC.Q,		KC.GRAVE,	KC.ESC,		KC.NO,
    KC.Y,			KC.N,		KC.M,		KC.J,		KC.N7,		KC.U,		KC.N6,		KC.H,		KC.NO,
    KC.F3,			KC.NO,		KC.C,		KC.D,		KC.N3,		KC.E,		KC.F2,		KC.F4,		KC.NO,
    KC.CAPS,		KC.NO,		KC.X,		KC.S,		KC.N2,		KC.W,		KC.F1,		KC.NO,		KC.NO,
    KC.T,			KC.B,		KC.V,		KC.F,		KC.N4,		KC.R,		KC.N5,		KC.G,		KC.NO,
    KC.F7,			KC.NO,		KC.DOT,		KC.L,		KC.N9,		KC.O,		KC.F8,		KC.NO,		KC.NO,
    KC.LBRC,		KC.SLASH,	KC.NO,		KC.SCOLON,	KC.N0,		KC.P,		KC.MINUS,	KC.QUOTE,	KC.NO,
    KC.RBRC,		KC.NO,		KC.COMMA,	KC.K,		KC.N8,		KC.I,		KC.EQUAL,	KC.F6,		KC.NO,
    KC.NO,			KC.NO,		KC.RCTRL,	KC.NO,		KC.NO,		KC.NO,		KC.LCTRL,	KC.NO,		KC.NO,
    KC.NO,			KC.RALT,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.LALT,	KC.NO,
    KC.LGUI,		KC.RIGHT,	KC.NO,		KC.NO,		KC.F12,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
    KC.NO,			KC.LEFT,	KC.PDOT,	KC.NO,		KC.END,		KC.NO,		KC.NO,		KC.UP,		KC.NO,
    KC.NO,			KC.DOWN,	KC.NO,		KC.NO,		KC.F11,		KC.NO,		KC.HOME,	KC.NO,		KC.NO,
    KC.BSPACE,		KC.SPACE,	KC.ENTER,	KC.BSLASH,	KC.F10,		KC.NO,		KC.F9,		KC.F5,		KC.NO,
    KC.NO,			KC.PGDOWN,	KC.PGUP,	KC.PSCREEN,	KC.INSERT,	KC.NO,		KC.DELETE,	KC.NO,		KC.NO,
    KC.NO,			KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		FN,
    KC.PSLS,		KC.PPLS,	KC.P9,		KC.P7,		KC.P8,		KC.PAST,	KC.PMNS,	KC.NUM_LOCK, KC.NO,
    KC.P5,			KC.P0,		KC.PENT,	KC.P2,		KC.P3,		KC.P6,		KC.P1,		KC.P4,		KC.NO,
    ],

# The Fn media layer is a copy of the base layer except where a media key exists (like mute at Fn-F1)  
	[#layer 1: Fn Media Layer
    KC.LSHIFT,		KC.NO,		KC.RSHIFT,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
    KC.TAB,			KC.NO,		KC.Z,		KC.A,		KC.N1,		KC.Q,		KC.GRAVE,	KC.ESC,		KC.NO,
    KC.Y,			KC.N,		KC.M,		KC.J,		KC.N7,		KC.U,		KC.N6,		KC.H,		KC.NO,
    KC.VOLU,		KC.NO,		KC.C,		KC.D,		KC.N3,		KC.E,		KC.VOLD,	KC.F4,		KC.NO,
    KC.CAPS,		KC.NO,		KC.X,		KC.S,		KC.N2,		KC.W,		KC.MUTE,	KC.NO,		KC.NO,
    KC.T,			KC.B,		KC.V,		KC.F,		KC.N4,		KC.R,		KC.N5,		KC.G,		KC.NO,
    KC.F7,			KC.NO,		KC.DOT,		KC.L,		KC.N9,		KC.O,		KC.F8,		KC.NO,		KC.NO,
    KC.LBRC,		KC.SLASH,	KC.NO,		KC.SCOLON,	KC.N0,		KC.P,		KC.MINUS,	KC.QUOTE,	KC.NO,
    KC.RBRC,		KC.NO,		KC.COMMA,	KC.K,		KC.N8,		KC.I,		KC.EQUAL,	KC.BRIU,	KC.NO,
    KC.NO,			KC.NO,		KC.RCTRL,	KC.NO,		KC.NO,		KC.NO,		KC.LCTRL,	KC.NO,		KC.NO,
    KC.NO,			KC.RALT,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.LALT,	KC.NO,
    KC.LGUI,		KC.RIGHT,	KC.NO,		KC.NO,		KC.F12,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
    KC.NO,			KC.LEFT,	KC.PDOT,	KC.NO,		KC.END,		KC.NO,		KC.NO,		KC.UP,		KC.NO,
    KC.NO,			KC.DOWN,	KC.NO,		KC.NO,		KC.F11,		KC.NO,		KC.HOME,	KC.NO,		KC.NO,
    KC.BSPACE,		KC.SPACE,	KC.ENTER,	KC.BSLASH,	KC.F10,		KC.NO,		KC.F9,		KC.BRID,	KC.NO,
    KC.NO,			KC.PGDOWN,	KC.PGUP,	KC.PSCREEN,	KC.INSERT,	KC.NO,		KC.DELETE,	KC.NO,		KC.NO,
    KC.NO,			KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		FN,
    KC.PSLS,		KC.PPLS,	KC.P9,		KC.P7,		KC.P8,		KC.PAST,	KC.PMNS,	KC.NUM_LOCK, KC.NO,
    KC.P5,			KC.P0,		KC.PENT,	KC.P2,		KC.P3,		KC.P6,		KC.P1,		KC.P4,		KC.NO,
	],
	
]

if __name__ == '__main__':
    keyboard.go()