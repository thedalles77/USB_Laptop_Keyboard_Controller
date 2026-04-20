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
#   The following GPIO connections and key matrix are for a HP Envy X360 16 inch laptop keyboard.
#   Use this KMK code as your starting point if using a different keyboard.
#   Change the name to code.py when copying this file to the WeAct Studio RP2350B Core.
#   See step 16 of my Instructable for more information.
#
import board

from kmk.kmk_keyboard import KMKKeyboard
from kmk.keys import KC
from kmk.scanners import DiodeOrientation
from kmk.extensions.media_keys import MediaKeys
from kmk.modules.layers import Layers

keyboard = KMKKeyboard()

keyboard.col_pins = (board.GP13, board.GP14, board.GP16, board.GP17, board.GP19, board.GP20, board.GP21, board.GP22)
keyboard.row_pins = (board.GP1, board.GP2, board.GP3, board.GP5, board.GP6, board.GP7, board.GP8, board.GP9, 
board.GP10, board.GP11, board.GP12, board.GP15, board.GP18, board.GP24, board.GP41)
keyboard.diode_orientation = DiodeOrientation.COL2ROW # Most laptop keyboards have no diodes so ROW2COL also works

keyboard.modules.append(Layers())
keyboard.extensions.append(MediaKeys())

FN = KC.TG(1)

# Add any other keys that your keyboard has using the KMK keycodes from https://docs.qmk.fm/keycodes_basic
# and the media keys from https://github.com/KMKfw/kmk_firmware/blob/main/docs/en/media_keys.md
# All key names are preceded with KC. except the FN key. KC.NO is used when there is no key at that matrix location.
# This keyboard has no keypad.

keyboard.keymap = [
    [#layer 0: Base Layer
    KC.DELETE,KC.PGUP,KC.LEFT,KC.NO,KC.RIGHT,KC.END,KC.PGDOWN,KC.HOME,
    KC.NO,KC.BSLASH,KC.UP,KC.BSPACE,KC.DOWN,KC.SPACE,KC.ENTER,KC.NO,
    KC.F12,KC.F9,KC.O,KC.F11,KC.N9,KC.DOT,KC.L,KC.F10,
    KC.RALT,KC.NO,KC.NO,KC.NO,KC.LALT,KC.NO,KC.NO,KC.NO,
    KC.NO,KC.NO,KC.I,KC.NO,KC.N8,KC.COMMA,KC.K,KC.RBRC,
    KC.NO,KC.NO,KC.NO,KC.LGUI,KC.NO,KC.NO,KC.NO,KC.NO,
    KC.NO,FN,KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,
    KC.N,KC.N6,KC.U,KC.H,KC.N7,KC.M,KC.J,KC.Y,
    KC.F1,KC.F2,KC.E,KC.F4,KC.N3,KC.C,KC.D,KC.F3,
    KC.B,KC.N5,KC.R,KC.G,KC.N4,KC.V,KC.F,KC.T,
    KC.CAPS,KC.GRAVE,KC.Q,KC.ESC,KC.N1,KC.Z,KC.A,KC.TAB,
    KC.F8,KC.F5,KC.W,KC.F7,KC.N2,KC.X,KC.S,KC.F6,
    KC.NO,KC.NO,KC.RSHIFT,KC.NO,KC.NO,KC.LSHIFT,KC.NO,KC.NO,
    KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,KC.RCTRL,KC.LCTRL,
    KC.EQUAL,KC.MINUS,KC.P,KC.QUOTE,KC.N0,KC.SLASH,KC.SCOLON,KC.LBRC,    
    ],

# The Fn media layer is a copy of the base layer except where a media key exists  
	[#layer 1: Fn Media Layer
    KC.DELETE,KC.PGUP,KC.LEFT,KC.NO,KC.RIGHT,KC.END,KC.PGDOWN,KC.HOME,
    KC.NO,KC.BSLASH,KC.UP,KC.BSPACE,KC.DOWN,KC.SPACE,KC.ENTER,KC.NO,
    KC.F12,KC.MPRV,KC.O,KC.MNXT,KC.N9,KC.DOT,KC.L,KC.F10,
    KC.RALT,KC.NO,KC.NO,KC.NO,KC.LALT,KC.NO,KC.NO,KC.NO,
    KC.NO,KC.NO,KC.I,KC.NO,KC.N8,KC.COMMA,KC.K,KC.RBRC,
    KC.NO,KC.NO,KC.NO,KC.LGUI,KC.NO,KC.NO,KC.NO,KC.NO,
    KC.NO,FN,KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,
    KC.N,KC.N6,KC.U,KC.H,KC.N7,KC.M,KC.J,KC.Y,
    KC.F1,KC.F2,KC.E,KC.F4,KC.N3,KC.C,KC.D,KC.BRIU,
    KC.B,KC.N5,KC.R,KC.G,KC.N4,KC.V,KC.F,KC.T,
    KC.CAPS,KC.GRAVE,KC.Q,KC.ESC,KC.N1,KC.Z,KC.A,KC.TAB,
    KC.F8,KC.MUTE,KC.W,KC.VOLU,KC.N2,KC.X,KC.S,KC.VOLD,
    KC.NO,KC.NO,KC.RSHIFT,KC.NO,KC.NO,KC.LSHIFT,KC.NO,KC.NO,
    KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,KC.NO,KC.RCTRL,KC.LCTRL,
    KC.EQUAL,KC.MINUS,KC.P,KC.QUOTE,KC.N0,KC.SLASH,KC.SCOLON,KC.LBRC,        
	],
	
]

if __name__ == '__main__':
    keyboard.go()
