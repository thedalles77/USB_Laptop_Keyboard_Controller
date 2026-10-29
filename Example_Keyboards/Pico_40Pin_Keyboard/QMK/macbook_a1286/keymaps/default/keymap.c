// Copyright 2026 Frank Adams (@thedalles77)
// SPDX-License-Identifier: GPL-2.0-or-later

// MacBook Pro A1286
// Matrix grid: 16 rows x 9 columns, same order as the key matrix table in the PDF.
// Layer 0 = normal keys. Holding Fn (row GP19, column GPB1) activates layer 1.
#include QMK_KEYBOARD_H

enum layers { _BASE, _FN };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
    //  GPA0     GP7      GPB5     GP8      GP17     GPB1      GP16     GPA6     GPB0
        KC_Q,    KC_1,    KC_A,    KC_Z,    KC_F11,  KC_BSPC,  KC_F12,  KC_F1,   KC_NO,    // GPA1
        KC_W,    KC_2,    KC_S,    KC_X,    KC_ESC,  KC_TAB,   KC_GRV,  KC_F2,   KC_NO,    // GP20
        KC_E,    KC_3,    KC_D,    KC_C,    KC_NO,   KC_NO,    KC_NO,   KC_F3,   KC_NO,    // GPB4
        KC_Y,    KC_6,    KC_H,    KC_N,    KC_SLSH, KC_SPC,   KC_EQL,  KC_F6,   KC_NO,    // GP9
        KC_R,    KC_4,    KC_F,    KC_V,    KC_NO,   KC_NO,    KC_RBRC, KC_F4,   KC_NO,    // GPA2
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_ENT,  KC_NO,    KC_NO,   KC_RSFT, KC_NO,    // GP10
        KC_NO,   KC_NO,   KC_NO,   KC_LGUI, KC_0,    KC_NO,    KC_RGUI, KC_NO,   KC_NO,    // GPA3
        KC_T,    KC_5,    KC_G,    KC_B,    KC_MINS, MO(_FN),  KC_LBRC, KC_F5,   KC_NO,    // GP19
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_F10,  KC_RALT,  KC_NO,   KC_NO,   KC_NO,    // GPB3
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_P,    KC_CAPS,  KC_NO,   KC_NO,   KC_NO,    // GP11
        KC_U,    KC_7,    KC_J,    KC_M,    KC_SCLN, KC_QUOT,  KC_NO,   KC_F7,   KC_NO,    // GPB2
        KC_I,    KC_8,    KC_K,    KC_COMM, KC_NO,   KC_LEFT,  KC_RGHT, KC_F8,   KC_NO,    // GP18
        KC_O,    KC_9,    KC_L,    KC_DOT,  KC_BSLS, KC_DOWN,  KC_UP,   KC_F9,   KC_NO,    // GPA5
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,   KC_NO,   KC_LSFT,  // GP14
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,   KC_NO,   KC_LALT,  // GPA7
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,   KC_NO,   KC_LCTL   // GP15
    ),

    // Fn held: media keys. _______ = fall through to the base layer key.
    //   F5 / F6   = keyboard backlight down / up      F10       = mute
    //   F7 / F8 / F9 = previous track, play/pause, next track
    //   F11 / F12 = volume down / up
    [_FN] = LAYOUT(
    //  GPA0     GP7      GPB5     GP8      GP17     GPB1      GP16     GPA6     GPB0
        _______, _______, _______, _______, KC_VOLD, _______,  KC_VOLU, _______, _______,  // GPA1  F11, F12
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP20
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPB4
        _______, _______, _______, _______, _______, _______,  _______, BL_UP,   _______,  // GP9   F6
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA2
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP10
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA3
        _______, _______, _______, _______, _______, _______,  _______, BL_DOWN, _______,  // GP19  F5 (Fn at GPB1)
        _______, _______, _______, _______, KC_MUTE, _______,  _______, _______, _______,  // GPB3  F10
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP11
        _______, _______, _______, _______, _______, _______,  _______, KC_MPRV, _______,  // GPB2  F7
        _______, _______, _______, _______, _______, _______,  _______, KC_MPLY, _______,  // GP18  F8
        _______, _______, _______, _______, _______, _______,  _______, KC_MNXT, _______,  // GPA5  F9
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GP14
        _______, _______, _______, _______, _______, _______,  _______, _______, _______,  // GPA7
        _______, _______, _______, _______, _______, _______,  _______, _______, _______   // GP15
    )
};
