// Copyright 2024 splitkb.com (support@splitkb.com)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "pointing_device.h"

// Define custom keycodes
enum custom_keycodes {
    SCRL_MODE = SAFE_RANGE,
};

// Layer definitions
#define BASE 0
#define NUM_SYM 1
#define MOVE_ACT 2
#define ADJUST 3
#define GAMING 4

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT_corne_hlc(
        KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                        KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,
                          KC_LCTL, MO(1),   MO(2),   KC_ENT,   KC_SPC,   KC_RGUI,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    [NUM_SYM] = LAYOUT_corne_hlc(
        KC_TRNS, KC_EXLM, KC_AT,   KC_HASH, KC_LBRC, KC_RBRC,                      KC_MINUS,KC_7,    KC_8,    KC_9,    KC_PEQL, KC_DEL,
        KC_GRV,  KC_DLR,  KC_ASTR, KC_AMPR, KC_LPRN, KC_RPRN,                      KC_PPLS, KC_4,    KC_5,    KC_6,    KC_BSLS, KC_PIPE,
        KC_TRNS, KC_TILD, KC_CIRC, KC_PERC, KC_LCBR, KC_RCBR,                      KC_DOT,  KC_1,    KC_2,    KC_3,    KC_RALT, KC_NO,
                          KC_TRNS, KC_TRNS, MO(3),   MO(3),    KC_0,    KC_TRNS,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    [MOVE_ACT] = LAYOUT_corne_hlc(
        KC_NO,   KC_NO,   LCTL(KC_RGHT), KC_VOLD, KC_VOLU, KC_MUTE,               KC_PGUP, KC_PGDN, KC_HOME, KC_END,  KC_NO,   KC_TRNS,
        KC_NO,   KC_NO,   KC_MPRV, KC_MNXT, KC_MPLY, KC_MSTP,                      KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO,   KC_PSCR,
        KC_NO,   MS_BTN3, SCRL_MODE, MS_BTN1, MS_BTN2, LCTL(KC_LEFT),              KC_NO,   KC_NO,   LCS(KC_TAB), LCTL(KC_TAB), KC_NO, KC_NO,
                          KC_LALT, KC_TRNS, TG(3),   TG(3),    MS_BTN4, MS_BTN5,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    [ADJUST] = LAYOUT_corne_hlc(
        TG(0),   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,                        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                        KC_NO,   KC_NO,   KC_NO,   KC_F11,  KC_F12,  KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   TG(4),                        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                          KC_LCTL, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_RALT,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
    [GAMING] = LAYOUT_corne_hlc(
        TG(0),   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,                        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_TRNS,                      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                          KC_NO,   KC_NO,   KC_NO,   KC_NO,    KC_NO,   KC_NO,
        KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO
    ),
};

// Track scroll mode state
static bool scroll_mode_active = false;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case SCRL_MODE:
            if (record->event.pressed) {
                // Enable scroll mode when key is pressed
                scroll_mode_active = true;
            } else {
                // Disable scroll mode when key is released
                scroll_mode_active = false;
            }
            return false;
        default:
            return true;
    }
}

// Adjust this value to control scroll speed (higher = slower)
#define SCROLL_DIVISOR 8

// Hook into pointing device to apply scroll mode
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (scroll_mode_active) {
        // Convert trackpad movement to scroll, scaled down by divisor
        // Vertical movement (y) → vertical scroll (v)
        // Horizontal movement (x) → horizontal scroll (h)
        mouse_report.v = -(mouse_report.y / SCROLL_DIVISOR);
        mouse_report.h = -(mouse_report.x / SCROLL_DIVISOR);

        // Disable cursor movement while scrolling
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}
