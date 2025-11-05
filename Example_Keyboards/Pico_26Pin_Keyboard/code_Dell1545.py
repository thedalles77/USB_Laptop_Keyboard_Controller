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
#   The following GPIO connections and key matrix are for a Dell 1545 laptop keyboard.
#   Use this KMK code as your starting point if using a different keyboard.
#   Change the name to code.py when copying this file to the Pi Pico.
#   See step 16 of my Instructable for more information.
#
import board

from kmk.kmk_keyboard import KMKKeyboard
from kmk.keys import KC
from kmk.scanners import DiodeOrientation
from kmk.extensions.media_keys import MediaKeys
from kmk.modules.layers import Layers

keyboard = KMKKeyboard()


keyboard.col_pins = (board.GP1, board.GP2, board.GP3, board.GP4, board.GP5, board.GP6, board.GP27, board.GP28)
keyboard.row_pins = (board.GP7, board.GP8, board.GP9, board.GP10, board.GP11, board.GP12, board.GP13, board.GP14, 
board.GP15, board.GP16, board.GP17, board.GP18, board.GP19, board.GP20, board.GP21, board.GP22, board.GP26)
keyboard.diode_orientation = DiodeOrientation.COL2ROW # Most laptop keyboards have no diodes so ROW2COL also works

keyboard.modules.append(Layers())
keyboard.extensions.append(MediaKeys())

FN = KC.TG(1)

keyboard.keymap = [
    [#layer 0: Base Layer
	 KC.NO,			KC.NO,		KC.PGDOWN,	KC.LGUI,	KC.NO,		KC.NO,		KC.NO,		KC.PGUP,
	 KC.NO,			KC.RCTRL,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.LCTRL,	KC.NO,
	 KC.RALT,		KC.NO,		KC.NO,		KC.PSCREEN,	KC.NO,		KC.LALT,	KC.NO,		KC.EJCT,
	 KC.NO,			KC.Z,		KC.A,		KC.N1,		KC.TAB,		KC.ESC,		KC.GRAVE,	KC.Q,
	 KC.NO,			KC.C,		KC.D,		KC.N3,		KC.F3,		KC.F4,		KC.F2,		KC.E,
	 KC.SPACE,		KC.ENTER,	KC.BSLASH,	KC.F10,		KC.BSPACE,	KC.F5,		KC.F9,		KC.NO,
	 KC.NO,			KC.COMMA,	KC.K,		KC.N8,		KC.RBRC,	KC.F6,		KC.EQUAL,	KC.I,
	 KC.NO,			KC.DOT,		KC.L,		KC.N9,		KC.F7,		KC.NO,		KC.F8,		KC.O,
	 KC.LEFT,		KC.NO,		KC.NO,		KC.END,		KC.NO,		KC.UP,		KC.HOME,	KC.NO,
	 KC.RIGHT,		KC.NO,		KC.NO,		KC.F12,		KC.NO,		KC.NO,		KC.INSERT,	KC.NO,
	 KC.DOWN,		KC.NO,		KC.NO,		KC.F11,		KC.NO,		KC.NO,		KC.DELETE,	KC.NO,
	 KC.SLASH,		KC.NO,		KC.SCOLON,	KC.N0,		KC.LBRC,	KC.QUOTE,	KC.MINUS,	KC.P,
	 KC.N,			KC.M,		KC.J,		KC.N7,		KC.Y,		KC.H,		KC.N6,		KC.U,
	 KC.B,			KC.V,		KC.F,		KC.N4,		KC.T,		KC.G,		KC.N5,		KC.R,
	 KC.NO,			KC.X,		KC.S,		KC.N2,		KC.CAPS,	KC.NO,		KC.F1,		KC.W,
	 KC.NO,			KC.RSHIFT,	KC.NO,		KC.NO,		KC.LSHIFT,	KC.NO,		KC.NO,		KC.NO,
     KC.NO,			KC.NO,		FN,			KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
	],
	[#layer 1: Fn Media Layer
	 KC.NO,			KC.NO,		KC.PGDOWN,	KC.LGUI,	KC.NO,		KC.NO,		KC.NO,		KC.PGUP,
	 KC.NO,			KC.RCTRL,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.LCTRL,	KC.NO,
	 KC.RALT,		KC.NO,		KC.NO,		KC.PSCREEN,	KC.NO,		KC.LALT,	KC.NO,		KC.EJCT,
	 KC.NO,			KC.Z,		KC.A,		KC.N1,		KC.TAB,		KC.ESC,		KC.GRAVE,	KC.Q,
	 KC.NO,			KC.C,		KC.D,		KC.N3,		KC.F3,		KC.BRID,	KC.F2,		KC.E,
	 KC.SPACE,		KC.ENTER,	KC.BSLASH,	KC.MPRV,	KC.BSPACE,	KC.BRIU,	KC.VOLU,	KC.NO,
	 KC.NO,			KC.COMMA,	KC.K,		KC.N8,		KC.RBRC,	KC.F6,		KC.EQUAL,	KC.I,
	 KC.NO,			KC.DOT,		KC.L,		KC.N9,		KC.MUTE,	KC.NO,		KC.VOLD,	KC.O,
	 KC.LEFT,		KC.NO,		KC.NO,		KC.END,		KC.NO,		KC.UP,		KC.HOME,	KC.NO,
	 KC.RIGHT,		KC.NO,		KC.NO,		KC.MNXT,	KC.NO,		KC.NO,		KC.INSERT,	KC.NO,
	 KC.DOWN,		KC.NO,		KC.NO,		KC.MPLY,	KC.NO,		KC.NO,		KC.DELETE,	KC.NO,
	 KC.SLASH,		KC.NO,		KC.SCOLON,	KC.N0,		KC.LBRC,	KC.QUOTE,	KC.MINUS,	KC.P,
	 KC.N,			KC.M,		KC.J,		KC.N7,		KC.Y,		KC.H,		KC.N6,		KC.U,
	 KC.B,			KC.V,		KC.F,		KC.N4,		KC.T,		KC.G,		KC.N5,		KC.R,
	 KC.NO,			KC.X,		KC.S,		KC.N2,		KC.CAPS,	KC.NO,		KC.F1,		KC.W,
	 KC.NO,			KC.RSHIFT,	KC.NO,		KC.NO,		KC.LSHIFT,	KC.NO,		KC.NO,		KC.NO,
     KC.NO,			KC.NO,		FN,			KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
	],
	
]

if __name__ == '__main__':
    keyboard.go()
