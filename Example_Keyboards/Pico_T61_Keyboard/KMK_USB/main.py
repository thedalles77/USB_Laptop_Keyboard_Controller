#   Copyright 2024 Frank Adams
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
import board

from kmk.kmk_keyboard import KMKKeyboard
from kmk.keys import KC
from kmk.scanners import DiodeOrientation
from kmk.extensions.media_keys import MediaKeys
from kmk.modules.layers import Layers

keyboard = KMKKeyboard()


keyboard.col_pins = (board.GP11, board.GP22, board.GP10, board.GP21, board.GP9, board.GP20, board.GP8, board.GP7)
keyboard.row_pins = (board.GP27, board.GP26, board.GP17, board.GP14, board.GP12, board.GP19, board.GP18, board.GP15, 
					board.GP13, board.GP6, board.GP16, board.GP5, board.GP28, board.GP2, board.GP4, board.GP3)
keyboard.diode_orientation = DiodeOrientation.COL2ROW

keyboard.modules.append(Layers())
keyboard.extensions.append(MediaKeys())

FN = KC.TG(1)

keyboard.keymap = [
    [#layer 0: Base Layer
	 KC.GRAVE,		KC.N1,		KC.Q,		KC.TAB,		KC.A,		KC.ESC,		KC.Z,		KC.NO,
	 KC.F1,			KC.N2,		KC.W,		KC.CAPS,	KC.S,		KC.NO,		KC.X,		KC.NO,
	 KC.F2,			KC.N3,		KC.E,		KC.F3,		KC.D,		KC.F4,		KC.C,		KC.NO,
	 KC.N5,			KC.N4,		KC.R,		KC.T,		KC.F,		KC.G,		KC.V,		KC.B,
	 KC.N6,			KC.N7,		KC.U,		KC.Y,		KC.J,		KC.H,		KC.M,		KC.N,
	 KC.EQUAL,		KC.N8,		KC.I,		KC.RBRC,	KC.K,		KC.F6,		KC.COMMA,	KC.NO,
	 KC.F8,			KC.N9,		KC.O,		KC.F7,		KC.L,		KC.NO,		KC.DOT,		KC.NO,
	 KC.MINUS,		KC.N0,		KC.P,		KC.LBRC,	KC.SCOLON,	KC.QUOTE,	KC.NO,		KC.SLASH,
	 KC.F9,			KC.F10,		KC.NO,		KC.BSPACE,	KC.BSLASH,	KC.F5,		KC.ENTER,	KC.SPACE,
	 KC.INSERT,		KC.F12,		KC.NO,		KC.NO,		FN,		KC.NO,		KC.NO,		KC.RIGHT,
	 KC.DELETE,		KC.F11,		KC.VOLU,	KC.VOLD,	KC.MUTE,	KC.NO,		KC.NO,		KC.DOWN,
	 KC.PGUP,		KC.PGDOWN,	KC.LGUI,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
	 KC.HOME,		KC.END,		KC.NO,		KC.NO,		KC.NO,		KC.UP,		KC.PAUSE,	KC.LEFT,
	 KC.NO,			KC.PSCREEN,	KC.SLCK,	KC.NO,		KC.NO,		KC.LALT,	KC.NO,		KC.RALT,
	 KC.NO,			KC.NO,		KC.NO,		KC.LSHIFT,	KC.NO,		KC.NO,		KC.RSHIFT,	KC.NO,
	 KC.LCTRL,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.RCTRL,	KC.NO,
	],
	[#layer 1: Fn Media Layer
	 KC.GRAVE,		KC.N1,		KC.Q,		KC.TAB,		KC.A,		KC.ESC,		KC.Z,		KC.NO,
	 KC.F1,			KC.N2,		KC.W,		KC.CAPS,	KC.S,		KC.NO,		KC.X,		KC.NO,
	 KC.F2,			KC.N3,		KC.E,		KC.F3,		KC.D,		KC.F4,		KC.C,		KC.NO,
	 KC.N5,			KC.N4,		KC.R,		KC.T,		KC.F,		KC.G,		KC.V,		KC.B,
	 KC.N6,			KC.N7,		KC.U,		KC.Y,		KC.J,		KC.H,		KC.M,		KC.N,
	 KC.EQUAL,		KC.N8,		KC.I,		KC.RBRC,	KC.K,		KC.F6,		KC.COMMA,	KC.NO,
	 KC.F8,			KC.N9,		KC.O,		KC.F7,		KC.L,		KC.NO,		KC.DOT,		KC.NO,
	 KC.MINUS,		KC.N0,		KC.P,		KC.LBRC,	KC.SCOLON,	KC.QUOTE,	KC.NO,		KC.SLASH,
	 KC.F9,			KC.F10,		KC.NO,		KC.BSPACE,	KC.BSLASH,	KC.F5,		KC.ENTER,	KC.SPACE,
	 KC.INSERT,		KC.F12,		KC.NO,		KC.NO,		FN,		KC.NO,		KC.NO,		KC.MNXT,
	 KC.DELETE,		KC.F11,		KC.VOLU,	KC.VOLD,	KC.MUTE,	KC.NO,		KC.NO,		KC.MPLY,
	 KC.PGUP,		KC.PGDOWN,	KC.LGUI,	KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,
	 KC.BRIU,		KC.BRID,	KC.NO,		KC.NO,		KC.NO,		KC.MSTP,	KC.PAUSE,	KC.MPRV,
	 KC.NO,			KC.PSCREEN,	KC.NLCK,	KC.NO,		KC.NO,		KC.LALT,	KC.NO,		KC.RALT,
	 KC.NO,			KC.NO,		KC.NO,		KC.LSHIFT,	KC.NO,		KC.NO,		KC.RSHIFT,	KC.NO,
	 KC.LCTRL,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.NO,		KC.RCTRL,	KC.NO,
	],
	
]

if __name__ == '__main__':
    keyboard.go()
