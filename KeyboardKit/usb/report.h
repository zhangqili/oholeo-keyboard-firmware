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

#ifdef KEYBOARD_SHARED_EP
#    define KEYBOARD_REPORT_SIZE 9
#else
#    define KEYBOARD_REPORT_SIZE 8
#endif

#define KEYBOARD_REPORT_KEYS 6

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
