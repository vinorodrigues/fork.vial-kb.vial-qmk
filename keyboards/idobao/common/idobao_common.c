// Copyright 2023 Vino Rodrigues (@vinorodrigues)
// SPDX-License-Identifier: GPL-3.0-or-later

#include QMK_KEYBOARD_H
#include "idobao_common.h"
#include "version.h"

bool macos_lock_enabled = false;
uint32_t timer_macos_lock_buffer = 0;

#ifndef MACOS_LOCK_DELAY
    #define MACOS_LOCK_DELAY (TAPPING_TERM + 1250)
#endif  // MACOS_LOCK_DELAY

#if defined(CAPS_LOCK_LED_COLOR) || defined(NUM_LOCK_LED_COLOR) || defined(SCROLL_LOCK_LED_COLOR)
    #include "color_util.h"

    #if defined(CAPS_LOCK_LED_INDEX) && !defined(CAPS_LOCK_LED_COLOR)
        #define CAPS_LOCK_LED_COLOR RGB_WHITE
    #endif

    #if defined(NUM_LOCK_LED_INDEX) && !defined(NUM_LOCK_LED_COLOR)
        #define NUM_LOCK_LED_COLOR RGB_WHITE
    #endif

    #if defined(SCROLL_LOCK_LED_INDEX) && !defined(SCROLL_LOCK_LED_COLOR)
        #define SCROLL_LOCK_LED_COLOR RGB_WHITE
    #endif
#endif  // x_LOCK_LED_COLOR

#ifdef CAPS_LOCK_LED_COLOR
RGB caps_lock_led_color;
#endif  // CAPS_LOCK_LED_COLOR

#ifdef NUM_LOCK_LED_COLOR
RGB num_lock_led_color;
#endif  // NUM_LOCK_LED_COLOR

#ifdef SCROLL_LOCK_LED_COLOR
RGB scroll_lock_led_color;
#endif  // SCROLL_LOCK_LED_COLOR


#if defined(CAPS_LOCK_LED_COLOR) || defined(NUM_LOCK_LED_COLOR) || defined(SCROLL_LOCK_LED_COLOR)

void keyboard_pre_init_kb(void) {
    keyboard_pre_init_user();

    #if defined(CAPS_LOCK_LED_COLOR) && defined(CAPS_LOCK_LED_INDEX)
    caps_lock_led_color = to_rgb(CAPS_LOCK_LED_COLOR);
    #endif  // CAPS_LOCK_LED_COLOR


    #if defined(NUM_LOCK_LED_COLOR) && defined(NUM_LOCK_LED_INDEX)
    num_lock_led_color = to_rgb(NUM_LOCK_LED_COLOR);
    #endif  // NUM_LOCK_LED_COLOR


    #if defined(SCROLL_LOCK_LED_COLOR) && defined(SCROLL_LOCK_LED_INDEX)
    scroll_lock_led_color = to_rgb(SCROLL_LOCK_LED_COLOR);
    #endif  // SCROLL_LOCK_LED_COLOR
}

#endif  // CAPS_LOCK_LED_COLOR | NUM_LOCK_LED_COLOR | SCROLL_LOCK_LED_COLOR


bool idobao_host_consumer_send(keyrecord_t *record, uint16_t data) {
    if (record->event.pressed) {
        host_consumer_send(data);
    } else {
        host_consumer_send(0);
    }
    return false;  // Skip all further processing of this key
}

bool idobao_register_code(keyrecord_t *record, uint16_t data) {
    if (record->event.pressed) {
        register_code(data);
    } else {
        unregister_code(data);
    }
    return false;  // Skip all further processing of this key
}

bool idobao_register_code_2(keyrecord_t *record, uint16_t data1, uint16_t data2) {
    if (record->event.pressed) {
        register_code(data1);
        register_code(data2);
    } else {
        unregister_code(data2);
        unregister_code(data1);
    }
    return false;  // Skip all further processing of this key
}

bool idobao_register_code_3(keyrecord_t *record, uint16_t data1, uint16_t data2, uint16_t data3) {
    if (record->event.pressed) {
        register_code(data1);
        register_code(data2);
        register_code(data3);
    } else {
        unregister_code(data3);
        unregister_code(data2);
        unregister_code(data1);
    }
    return false;  // Skip all further processing of this key
}

static bool __print_version(keyrecord_t *record) {
    if (!get_mods()) {
        if (!record->event.pressed) {
                SEND_STRING(QMK_KEYBOARD ":" QMK_KEYMAP " (v" QMK_VERSION ")");
            }
        }
    return false;
}

static bool __eeprom_clear(keyrecord_t *record) {
    if (record->event.pressed) {
        eeconfig_init_quantum();
        soft_reset_keyboard();
        wait_ms(10);  // give it time
    }
    return false;
}

void __do_lock_and_sleep(void) {
    send_string(SS_LCTL(SS_LGUI("q")) SS_DELAY(225) SS_TAP(X_ESC));
}

static bool __lock_and_sleep(keyrecord_t *record) {
    if (record->event.pressed) {
        __do_lock_and_sleep();
    }
    return false;
}

static bool __lock_and_sleep_delay(keyrecord_t *record) {
    if (record->event.pressed) {
        timer_macos_lock_buffer = sync_timer_read32();
        macos_lock_enabled = true;
    } else {
        macos_lock_enabled = false;
        timer_macos_lock_buffer = 0;  // arb cleanup
    }
    return false;
}

void housekeeping_task_idobao(void) {
    if (macos_lock_enabled && (sync_timer_elapsed32(timer_macos_lock_buffer) > MACOS_LOCK_DELAY)) {
        macos_lock_enabled = false;
        __do_lock_and_sleep();
    }
}

bool process_record_idobao(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        // Windows
        case KC_TASK_VIEW:               return idobao_register_code_2(record, KC_LWIN, KC_TAB);
        case KC_FILE_EXPLORER:           return idobao_register_code_2(record, KC_LWIN, KC_E);
        case KC_CORTANA:                 return idobao_register_code_2(record, KC_LWIN, KC_C);

        // macOS
        #ifdef APPLE_FN_ENABLE
        case MAGIC_TOGGLE_NKRO:
        case KC_APPLE_FN_KEY:            return idobao_register_code(record, KC_APPLE_FN);
        case MAGIC_HOST_NKRO:
        case MAGIC_UNHOST_NKRO:          return false;  // discard these keys
        #endif  // APPLE_FN_ENABLE
        case KC_MISSION_CONTROL:         return idobao_host_consumer_send(record, _AC_SHOW_ALL_WINDOWS);
        case KC_LAUNCHPAD:               return idobao_host_consumer_send(record, _AC_SHOW_ALL_APPS);
        case KC_SIRI:                    return idobao_register_code_2(record, KC_LOPT, KC_SPACE);
        case KC_SCREEN_SHOT:             return idobao_register_code_3(record, KC_LSFT, KC_LCMD, KC_4);
        case KC_LOCK_AND_SLEEP:          return __lock_and_sleep(record);
        case KC_LOCK_AND_SLEEP_DELAYED:  return __lock_and_sleep_delay(record);

        // general
        case KC_CLEAR_EEPROM:            return __eeprom_clear(record);
        case KC_VERSION:                 return __print_version(record);

        default:
            return true;  // Process all other keycodes normally
    }
}

__attribute__((weak)) bool dip_switch_update_idobao(uint8_t index, bool active) { return true; }

#ifdef RGB_MATRIX_ENABLE

extern void rgb_matrix_update_pwm_buffers(void);

#ifndef RGB_MATRIX_MAXIMUM_BRIGHTNESS
    #define INDICATOR_MAX_BRIGHTNESS 0xFF
#else
    #define INDICATOR_MAX_BRIGHTNESS RGB_MATRIX_MAXIMUM_BRIGHTNESS
#endif

#ifndef RGB_MATRIX_VAL_STEP
    #define INDICATOR_VAL_STEP 8
#else
    #define INDICATOR_VAL_STEP RGB_MATRIX_VAL_STEP
#endif

uint8_t light_brightness_get(void) {
    uint8_t value = rgb_matrix_get_val();
    if (value < INDICATOR_VAL_STEP) {
        value = INDICATOR_VAL_STEP;
    } else if (value < (INDICATOR_MAX_BRIGHTNESS - INDICATOR_VAL_STEP)) {
        value += INDICATOR_VAL_STEP; // one step more than current brightness
    } else {
        value = INDICATOR_MAX_BRIGHTNESS;
    }
    return value;
}

bool rgb_matrix_indicators_advanced_idobao(uint8_t led_min, uint8_t led_max) {
    // do nothing
    return true;
}

#if defined(CAPS_LOCK_LED_INDEX) || defined(NUM_LOCK_LED_INDEX) || defined(SCROLL_LOCK_LED_INDEX)

#if defined(CAPS_LOCK_LED_COLOR) || defined(NUM_LOCK_LED_COLOR) || defined(SCROLL_LOCK_LED_COLOR)
static void __rgb_matrix_set_color_b(int index, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t v = light_brightness_get();

    HSV hsv = rgb_to_hsv(to_rgb(r, g, b));
    RGB rgb = hsv_to_rgb(to_hsv(hsv.h, hsv.s, v));
    rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
}
#else
static void __rgb_matrix_set_color_w(int index) {
    uint8_t v = light_brightness_get();
    rgb_matrix_set_color(index, v, v, v);
}
#endif  // CAPS_LOCK_LED_COLOR | NUM_LOCK_LED_COLOR | SCROLL_LOCK_LED_COLOR

void rgb_matrix_indicators_idobao(void) {
    // this code executes when the matrix is enabled

    #ifdef CAPS_LOCK_LED_INDEX
    if (host_keyboard_led_state().caps_lock) {
        #ifdef CAPS_LOCK_LED_COLOR
        __rgb_matrix_set_color_b(CAPS_LOCK_LED_INDEX, caps_lock_led_color.r, caps_lock_led_color.g, caps_lock_led_color.b);
        #else
        __rgb_matrix_set_color_w(CAPS_LOCK_LED_INDEX);
        #endif  // CAPS_LOCK_LED_COLOR
    }
    #endif  // CAPS_LOCK_LED_INDEX

    #ifdef NUM_LOCK_LED_INDEX
    if (host_keyboard_led_state().num_lock) {
        #ifdef NUM_LOCK_LED_COLOR
        __rgb_matrix_set_color_b(NUM_LOCK_LED_INDEX, num_lock_led_color.r, num_lock_led_color.g, num_lock_led_color.b);
        #else
        __rgb_matrix_set_color_w(NUM_LOCK_LED_INDEX);
        #endif  // NUM_LOCK_LED_COLOR
    }
    #endif  // NUM_LOCK_LED_INDEX

    #ifdef SCROLL_LOCK_LED_INDEX
    if (host_keyboard_led_state().scroll_lock) {
        #ifdef SCROLL_LOCK_LED_COLOR
        __rgb_matrix_set_color_b(SCROLL_LOCK_LED_INDEX, scroll_lock_led_color.r, scroll_lock_led_color.g, scroll_lock_led_color.b);
        #else
        __rgb_matrix_set_color_w(SCROLL_LOCK_LED_INDEX);
        #endif  // SCROLL_LOCK_LED_COLOR
    }
    #endif  // SCROLL_LOCK_LED_INDEX
}

bool led_update_idobao(led_t led_state) {
    // this code executes regardless of matrix enabled state
    bool res = led_update_user(led_state);

    if (rgb_matrix_is_enabled()
        #if defined(ENABLE_RGB_MATRIX_RAINDROPS)
        && (rgb_matrix_get_mode() != RGB_MATRIX_RAINDROPS)
        #endif
        #if defined(ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS)
        && (rgb_matrix_get_mode() != RGB_MATRIX_JELLYBEAN_RAINDROPS)
        #endif
        #if defined(ENABLE_RGB_MATRIX_PIXEL_RAIN)
        && (rgb_matrix_get_mode() != RGB_MATRIX_PIXEL_RAIN)
        #endif
        ) {
        return res;
    }

    // this code executes when the matrix is NOT enabled
    if (res) {
        #ifdef CAPS_LOCK_LED_INDEX
        if (led_state.caps_lock) {
            #ifdef CAPS_LOCK_LED_COLOR
            __rgb_matrix_set_color_b(CAPS_LOCK_LED_INDEX, caps_lock_led_color.r, caps_lock_led_color.g, caps_lock_led_color.b);
            #else
            __rgb_matrix_set_color_w(CAPS_LOCK_LED_INDEX);
            #endif  // CAPS_LOCK_LED_COLOR
        } else {
            rgb_matrix_set_color(CAPS_LOCK_LED_INDEX, HSV_OFF);
        }
        #endif  // CAPS_LOCK_LED_INDEX

        #ifdef NUM_LOCK_LED_INDEX
        if (led_state.num_lock) {
            #ifdef NUM_LOCK_LED_COLOR
            __rgb_matrix_set_color_b(NUM_LOCK_LED_INDEX, num_lock_led_color.r, num_lock_led_color.g, num_lock_led_color.b);
            #else
            __rgb_matrix_set_color_w(NUM_LOCK_LED_INDEX);
            #endif  // NUM_LOCK_LED_COLOR
        } else {
            rgb_matrix_set_color(NUM_LOCK_LED_INDEX, HSV_OFF);
        }
        #endif  // NUM_LOCK_LED_INDEX

        #ifdef SCROLL_LOCK_LED_INDEX
        if (led_state.scroll_lock) {
            #ifdef SCROLL_LOCK_LED_COLOR
            __rgb_matrix_set_color_b(SCROLL_LOCK_LED_INDEX, scroll_lock_led_color.r, scroll_lock_led_color.g, scroll_lock_led_color.b);
            #else
            __rgb_matrix_set_color_w(SCROLL_LOCK_LED_INDEX);
            #endif  // SCROLL_LOCK_LED_COLOR
        } else {
            rgb_matrix_set_color(SCROLL_LOCK_LED_INDEX, HSV_OFF);
        }
        #endif  // SCROLL_LOCK_LED_INDEX

        rgb_matrix_update_pwm_buffers();
    }

    return res;
}

#endif  // CAPS_LOCK_LED_INDEX | NUM_LOCK_LED_INDEX | SCROLL_LOCK_LED_INDEX
#endif  // RGB_MATRIX_ENABLE
