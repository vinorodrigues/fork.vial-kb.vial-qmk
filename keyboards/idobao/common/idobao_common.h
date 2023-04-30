// Copyright 2022 Vino Rodrigues (@vinorodrigues)
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "stdint.h"
#include "quantum_keycodes.h"
#include "action.h"

#ifndef VIA_ENABLE
    #define USER00 (SAFE_RANGE)
#endif

enum keycron_custom_keycodes {
    // Windows
    KC_TASK_VIEW = USER00,
    KC_FILE_EXPLORER,
    KC_CORTANA,

    // macOS
    KC_APPLE_FN_KEY,
    KC_MISSION_CONTROL,
    KC_LAUNCHPAD,
    KC_SIRI,
    KC_SCREEN_SHOT,
    KC_LOCK_AND_SLEEP,
    KC_LOCK_AND_SLEEP_DELAYED,

    // General
    KC_CLEAR_EEPROM,
    KC_VERSION
};

enum macos_consumer_usages {
    _AC_SHOW_ALL_WINDOWS = 0x29F,  // mapped to Q1_MCON
    _AC_SHOW_ALL_APPS    = 0x2A0   // mapped to Q1_LPAD
};

#define KC_TASK KC_TASK_VIEW
#define KC_FLXP KC_FILE_EXPLORER
#define KC_CRTA KC_CORTANA

#define KC_APPL KC_APPLE_FN_KEY
#define KC_MCTL KC_MISSION_CONTROL
#define KC_LPAD KC_LAUNCHPAD
#define KC_SNAP KC_SCREEN_SHOT
#define KC_LOCK KC_LOCK_AND_SLEEP
#define KC_LOKD KC_LOCK_AND_SLEEP_DELAYED

#define KC_EECL KC_CLEAR_EEPROM
#define KC_VERS KC_VERSION

void keyboard_pre_init_kb(void);

bool idobao_host_consumer_send(keyrecord_t *record, uint16_t data);
bool idobao_register_code(keyrecord_t *record, uint16_t data);
bool idobao_register_code_2(keyrecord_t *record, uint16_t data1, uint16_t data2);
bool idobao_register_code_3(keyrecord_t *record, uint16_t data1, uint16_t data2, uint16_t data3);

void housekeeping_task_idobao(void);
bool process_record_idobao(uint16_t keycode, keyrecord_t *record);

bool dip_switch_update_idobao(uint8_t index, bool active);

#ifdef RGB_MATRIX_ENABLE

uint8_t light_brightness_get(void);
bool rgb_matrix_indicators_advanced_idobao(uint8_t led_min, uint8_t led_max);

#if defined(CAPS_LOCK_LED_INDEX) || defined(NUM_LOCK_LED_INDEX) || defined(SCROLL_LOCK_LED_INDEX)

void rgb_matrix_indicators_idobao(void);
bool led_update_idobao(led_t led_state);

#endif  // CAPS_LOCK_LED_INDEX | NUM_LOCK_LED_INDEX | SCROLL_LOCK_LED_INDEX
#endif  // RGB_MATRIX_ENABLE
