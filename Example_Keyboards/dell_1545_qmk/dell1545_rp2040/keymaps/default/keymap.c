// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
//
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
//     
    [0] = LAYOUT(
        KC_NO,   KC_INS,  KC_NO,   KC_F12,  KC_NO,   KC_NO,   KC_NO,   KC_RGHT,
		KC_NO,   KC_DEL,  KC_NO,   KC_F11,  KC_NO,   KC_NO,   KC_NO,   KC_DOWN,
		KC_UP,   KC_HOME, KC_APP,  KC_END,  KC_NO,   KC_NO,   KC_NO,   KC_LEFT,
        KC_NO,   KC_F8,   KC_F7,   KC_9,    KC_O,    KC_L,    KC_DOT,  KC_NO,
		KC_QUOT, KC_MINS, KC_LBRC, KC_0,    KC_P,    KC_SCLN, KC_NO,   KC_SLSH,
        KC_F6,   KC_EQL,  KC_RBRC, KC_8,    KC_I,    KC_K,    KC_COMM, KC_NO,
        KC_H,    KC_6,    KC_Y,    KC_7,    KC_U,    KC_J,    KC_M,    KC_N,
        KC_F5,   KC_F9,   KC_BSPC, KC_F10,  KC_NO,   KC_BSLS, KC_ENT,  KC_SPC,
		KC_G,    KC_5,    KC_T,    KC_4,    KC_R,    KC_F,    KC_V,    KC_B,
        KC_F4,   KC_F2,   KC_F3,   KC_3,    KC_E,    KC_D,    KC_C,    KC_NO,
        KC_NO,   KC_F1,   KC_CAPS, KC_2,    KC_W,    KC_S,    KC_X,    KC_NO,
        KC_ESC,  KC_GRV,  KC_TAB,  KC_1,    KC_Q,    KC_A,    KC_Z,    KC_NO,
        KC_LALT, KC_NO,   KC_NO,   KC_PSCR, KC_NO,   KC_NO,   KC_NO,   KC_RALT,
        KC_NO,   KC_NO,   KC_LSFT, KC_NO,   KC_NO,   KC_NO,   KC_RSFT, KC_NO,
        KC_NO,   KC_LCTL, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_RCTL, KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_LGUI, KC_PGUP, KC_PGDN, KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   MO(1),   KC_NO,   KC_NO
    ),
    [1] = LAYOUT(
        KC_NO,   KC_INS,  KC_NO,   KC_MNXT, KC_NO,   KC_NO,   KC_NO,   KC_RGHT,
		KC_NO,   KC_DEL,  KC_NO,   KC_F11,  KC_NO,   KC_NO,   KC_NO,   KC_DOWN,
		KC_UP,   KC_HOME, KC_APP,  KC_END,  KC_NO,   KC_NO,   KC_NO,   KC_LEFT,
        KC_NO,   KC_VOLD, KC_MUTE, KC_9,    KC_O,    KC_L,    KC_DOT,  KC_NO,
		KC_QUOT, KC_MINS, KC_LBRC, KC_0,    KC_P,    KC_SCLN, KC_NO,   KC_SLSH,
        KC_F6,   KC_EQL,  KC_RBRC, KC_8,    KC_I,    KC_K,    KC_COMM, KC_NO,
        KC_H,    KC_6,    KC_Y,    KC_7,    KC_U,    KC_J,    KC_M,    KC_N,
        KC_BRIU, KC_VOLU, KC_BSPC, KC_MPRV, KC_NO,   KC_BSLS, KC_ENT,  KC_SPC,
		KC_G,    KC_5,    KC_T,    KC_4,    KC_R,    KC_F,    KC_V,    KC_B,
        KC_BRID, KC_F2,   KC_F3,   KC_3,    KC_E,    KC_D,    KC_C,    KC_NO,
        KC_NO,   KC_F1,   KC_CAPS, KC_2,    KC_W,    KC_S,    KC_X,    KC_NO,
        KC_ESC,  KC_GRV,  KC_TAB,  KC_1,    KC_Q,    KC_A,    KC_Z,    KC_NO,
        KC_LALT, KC_NO,   KC_NO,   KC_PSCR, KC_NO,   KC_NO,   KC_NO,   KC_RALT,
        KC_NO,   KC_NO,   KC_LSFT, KC_NO,   KC_NO,   KC_NO,   KC_RSFT, KC_NO,
        KC_NO,   KC_LCTL, KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_RCTL, KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_LGUI, KC_PGUP, KC_PGDN, KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_TRNS, KC_NO,   KC_NO
    )
};



