/*
Copyright 2011,2012 Jun Wako <wakojun@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "util.h"

#ifdef JOYSTICK_ENABLE
#    include "joystick.h"
#endif

// clang-format off

/* HID report IDs */
enum hid_report_ids { 
    REPORT_ID_ALL = 0,
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE,
    REPORT_ID_SYSTEM,
    REPORT_ID_CONSUMER,
    REPORT_ID_PROGRAMMABLE_BUTTON,
    REPORT_ID_NKRO,
    REPORT_ID_JOYSTICK,
    REPORT_ID_DIGITIZER,
    REPORT_ID_COUNT = REPORT_ID_DIGITIZER
};

#define IS_VALID_REPORT_ID(id) ((id) >= REPORT_ID_ALL && (id) <= REPORT_ID_COUNT)

// clang-format on

#define NKRO_REPORT_BITS 30

#ifdef KEYBOARD_SHARED_EP
#    define KEYBOARD_REPORT_SIZE 9
#else
#    define KEYBOARD_REPORT_SIZE 8
#endif

#define KEYBOARD_REPORT_KEYS 6

#ifdef __cplusplus
extern "C" {
#endif

/*
 * keyboard report is 8-byte array retains state of 8 modifiers and 6 keys.
 *
 * byte |0       |1       |2       |3       |4       |5       |6       |7
 * -----+--------+--------+--------+--------+--------+--------+--------+--------
 * desc |mods    |reserved|keys[0] |keys[1] |keys[2] |keys[3] |keys[4] |keys[5]
 *
 * It is exended to 16 bytes to retain 120keys+8mods when NKRO mode.
 *
 * byte |0       |1       |2       |3       |4       |5       |6       |7        ... |15
 * -----+--------+--------+--------+--------+--------+--------+--------+--------     +--------
 * desc |mods    |bits[0] |bits[1] |bits[2] |bits[3] |bits[4] |bits[5] |bits[6]  ... |bit[14]
 *
 * mods retains state of 8 modifiers.
 *
 *  bit |0       |1       |2       |3       |4       |5       |6       |7
 * -----+--------+--------+--------+--------+--------+--------+--------+--------
 * desc |Lcontrol|Lshift  |Lalt    |Lgui    |Rcontrol|Rshift  |Ralt    |Rgui
 *
 */
typedef struct {
#ifdef KEYBOARD_SHARED_EP
    uint8_t report_id;
#endif
    uint8_t mods;
    uint8_t reserved;
    uint8_t keys[KEYBOARD_REPORT_KEYS];
} PACKED report_keyboard_t;

typedef struct {
    uint8_t report_id;
    uint8_t mods;
    uint8_t bits[NKRO_REPORT_BITS];
} PACKED report_nkro_t;

typedef struct {
    uint8_t  report_id;
    uint16_t usage;
} PACKED report_extra_t;

typedef struct {
    uint8_t  report_id;
    uint32_t usage;
} PACKED report_programmable_button_t;

#ifdef MOUSE_EXTENDED_REPORT
typedef int16_t mouse_xy_report_t;
#else
typedef int8_t mouse_xy_report_t;
#endif

#ifdef WHEEL_EXTENDED_REPORT
typedef int16_t mouse_hv_report_t;
#else
typedef int8_t mouse_hv_report_t;
#endif

typedef struct {
#ifdef MOUSE_SHARED_EP
    uint8_t report_id;
#endif
    uint8_t buttons;
#ifdef MOUSE_EXTENDED_REPORT
    int8_t boot_x;
    int8_t boot_y;
#endif
    mouse_xy_report_t x;
    mouse_xy_report_t y;
    mouse_hv_report_t v;
    mouse_hv_report_t h;
} PACKED report_mouse_t;

typedef struct {
#ifdef DIGITIZER_SHARED_EP
    uint8_t report_id;
#endif
    bool     in_range : 1;
    bool     tip : 1;
    bool     barrel : 1;
    uint8_t  reserved : 5;
    uint16_t x;
    uint16_t y;
} PACKED report_digitizer_t;

#if JOYSTICK_AXIS_RESOLUTION > 8
typedef int16_t joystick_axis_t;
#else
typedef int8_t joystick_axis_t;
#endif

typedef struct {
#ifdef JOYSTICK_SHARED_EP
    uint8_t report_id;
#endif
#if JOYSTICK_AXIS_COUNT > 0
    joystick_axis_t axes[JOYSTICK_AXIS_COUNT];
#endif

#ifdef JOYSTICK_HAS_HAT
    int8_t  hat : 4;
    uint8_t reserved : 4;
#endif

#if JOYSTICK_BUTTON_COUNT > 0
    uint8_t buttons[(JOYSTICK_BUTTON_COUNT - 1) / 8 + 1];
#endif
} PACKED report_joystick_t;

#ifdef __cplusplus
}
#endif
