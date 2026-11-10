// Copyright 2026 Frank Adams (@thedalles77)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// Layer names. Higher layers take priority, so _FN is above _NUMPAD:
// holding Fn works the same whether Num Lock is on or off.
enum layers {
    _BASE = 0,
    _NUMPAD,
    _FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
// note that numlock key is sent in all layers (no scroll lock)
    [_BASE] = LAYOUT(
        KC_GRV,   KC_1,     KC_Q,     KC_TAB,   KC_A,     KC_ESC,   KC_Z,     KC_NO,
        KC_F1,    KC_2,     KC_W,     KC_CAPS,  KC_S,     KC_NO,    KC_X,     KC_NO,
        KC_F2,    KC_3,     KC_E,     KC_F3,    KC_D,     KC_F4,    KC_C,     KC_NO,
        KC_5,     KC_4,     KC_R,     KC_T,     KC_F,     KC_G,     KC_V,     KC_B,
        KC_6,     KC_7,     KC_U,     KC_Y,     KC_J,     KC_H,     KC_M,     KC_N,
        KC_EQL,   KC_8,     KC_I,     KC_RBRC,  KC_K,     KC_F6,    KC_COMM,  KC_NO,
        KC_F8,    KC_9,     KC_O,     KC_F7,    KC_L,     KC_NO,    KC_DOT,   KC_NO,
        KC_MINS,  KC_0,     KC_P,     KC_LBRC,  KC_SCLN,  KC_QUOT,  KC_NO,    KC_SLSH,
        KC_F9,    KC_F10,   KC_NO,    KC_BSPC,  KC_BSLS,  KC_F5,    KC_ENT,   KC_SPC,
        KC_INS,   KC_F12,   KC_NO,    KC_NO,    MO(_FN),  KC_NO,    KC_NO,    KC_RGHT,
        KC_DEL,   KC_F11,   KC_VOLU,  KC_VOLD,  KC_MUTE,  KC_NO,    KC_NO,    KC_DOWN,
        KC_PGUP,  KC_PGDN,  KC_LGUI,  KC_NO,    KC_APP,   KC_NO,    KC_NO,    KC_NO,
        KC_HOME,  KC_END,   KC_NO,    KC_NO,    KC_NO,    KC_UP,    KC_NO,    KC_LEFT,
        KC_NO,    KC_PSCR,  KC_NUM,   KC_NO,    KC_NO,    KC_LALT,  KC_NO,    KC_RALT,
        KC_NO,    KC_NO,    KC_NO,    KC_LSFT,  KC_NO,    KC_NO,    KC_RSFT,  KC_NO,
        KC_LCTL,  KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_NO,    KC_RCTL,  KC_NO
    ),
    // Num Lock on: the embedded keypad (7 8 9 0 / U I O P / J K L ; / M . /).
    // _______ = transparent, the key from the layer below is used.
    [_NUMPAD] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  KC_P7,    KC_P4,    _______,  KC_P1,    _______,  KC_P0,    _______,
        _______,  KC_P8,    KC_P5,    _______,  KC_P2,    _______,  _______,  _______,
        _______,  KC_P9,    KC_P6,    _______,  KC_P3,    _______,  KC_PDOT,  _______,
        _______,  KC_PSLS,  KC_PAST,  _______,  KC_PMNS,  _______,  _______,  KC_PPLS,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______
    ),
    // Fn held: F9 = eject, Home/End = brightness up/down,
    // arrows = Up: stop, Down: play/pause, Left: previous track, Right: next track.
    [_FN] = LAYOUT(
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KC_EJCT,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MNXT,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  KC_MPLY,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KC_BRIU,  KC_BRID,  _______,  _______,  _______,  KC_MSTP,  _______,  KC_MPRV,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______
    )
};

// Automatically toggle the NUMPAD layer when the host OS changes Num Lock state
bool led_update_user(led_t led_state) {
    if (led_state.num_lock) {
        layer_on(_NUMPAD);
    } else {
        layer_off(_NUMPAD);
    }
    return true;
}

// Check the state at startup to prevent the layer and host from getting out of sync
void keyboard_post_init_user(void) {
    if (host_keyboard_led_state().num_lock) {
        layer_on(_NUMPAD);
    } else {
        layer_off(_NUMPAD);
    }
}
