// Copyright 2026 Frank Adams (@thedalles77)
// SPDX-License-Identifier: GPL-2.0-or-later

// Lenovo E550
// Matrix grid: 19 rows x 9 columns.
// Layer 0 = normal keys. Holding Fn (row GP14, column GPB0) activates layer 1.  
#include QMK_KEYBOARD_H

enum layers { _BASE, _FN };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
    //  GP6          GP7          GP8          GP9          GP14         GPA0         GPA1         GPB6         GPB7
        KC_B,        KC_4,        KC_R,        KC_G,        KC_NO,       KC_F,        KC_5,        KC_V,        KC_T,        // GP10
        KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_LCTL,     KC_RCTL,     KC_NO,       // GP11
        KC_RIGHT,    KC_F12,      KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_LGUI,     // GP12
        KC_P0,       KC_P3,       KC_P6,       KC_P4,       KC_NO,       KC_P2,       KC_P1,       KC_PENT,     KC_P5,       // GP15
        KC_PGDN,     KC_INSERT,   KC_NO,       KC_NO,       KC_NO,       KC_PSCR,     KC_DELETE,   KC_PGUP,     KC_NO,       // GP17
        KC_DOWN,     KC_F11,      KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_HOME,     KC_NO,       KC_NO,       // GP18
        KC_SLASH,    KC_0,        KC_P,        KC_QUOTE,    KC_NO,       KC_SCLN,     KC_MINUS,    KC_NO,       KC_LBRC,     // GP19
        KC_N,        KC_7,        KC_U,        KC_H,        KC_NO,       KC_J,        KC_6,        KC_M,        KC_Y,        // GP20
        KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_RSFT,     KC_LSFT,     // GP21
        KC_NO,       KC_2,        KC_W,        KC_NO,       KC_NO,       KC_S,        KC_F1,       KC_X,        KC_CAPS,     // GPA2
        KC_NO,       KC_9,        KC_O,        KC_NO,       KC_NO,       KC_L,        KC_F8,       KC_DOT,      KC_F7,       // GPA3
        KC_RALT,     KC_NO,       KC_NO,       KC_LALT,     KC_NO,       KC_NO,       KC_NO,       KC_NO,       KC_NO,       // GPA4
        KC_SPACE,    KC_F10,      KC_NO,       KC_F5,       KC_NO,       KC_BSLS,     KC_F9,       KC_ENTER,    KC_BSPC,     // GPA5
        KC_PPLS,     KC_P8,       KC_PAST,     KC_NUM_LOCK, KC_NO,       KC_P7,       KC_PMNS,     KC_P9,       KC_PSLS,     // GPA7
        KC_NO,       KC_NO,       KC_NO,       KC_NO,       MO(1),       KC_NO,       KC_NO,       KC_NO,       KC_NO,       // GPB0
        KC_LEFT,     KC_END,      KC_NO,       KC_UP,       KC_NO,       KC_NO,       KC_NO,       KC_PDOT,     KC_NO,       // GPB2
        KC_NO,       KC_8,        KC_I,        KC_F6,       KC_NO,       KC_K,        KC_EQUAL,    KC_COMMA,    KC_RBRC,     // GPB3
        KC_NO,       KC_3,        KC_E,        KC_F4,       KC_NO,       KC_D,        KC_F2,       KC_C,        KC_F3,       // GPB4
        KC_NO,       KC_1,        KC_Q,        KC_ESC,      KC_NO,       KC_A,        KC_GRAVE,    KC_Z,        KC_TAB       // GPB5
    ),
    // Fn held: media keys. _______ = fall through to the base layer key.
    //   F1 = mute. F2 = volume down, F3 = volume up
    [_FN] = LAYOUT(
    //  GP6      GP7      GP8      GP9      GP14     GPA0      GPA1     GPB6     GPB7
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP10
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP11
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP12
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP15
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP17
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP18
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP19
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP20
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP21
        _______, _______, _______, _______, _______, _______,  KC_MUTE, _______, _______,  // GPA2
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA3
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA4
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA5
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA7
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPB0
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPB2
		_______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPB3
		_______, _______, _______, _______, _______, _______,  KC_VOLD, _______, KC_VOLU,  // GPB4
		_______, _______, _______, _______, _______, _______,  _______, _______, _______   // GPB5
    )
};
