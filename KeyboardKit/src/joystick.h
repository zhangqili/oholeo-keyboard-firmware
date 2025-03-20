/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "stdbool.h"
#include "stdint.h"
#include "keycode.h"
#include "keyboard_def.h"
#include "analog.h"

/**
 * \file
 *
 * \defgroup joystick HID Joystick
 * \{
 */

#ifndef JOYSTICK_BUTTON_COUNT
#    define JOYSTICK_BUTTON_COUNT 8
#elif JOYSTICK_BUTTON_COUNT > 32
#    error Joystick feature only supports up to 32 buttons
#endif

#ifndef JOYSTICK_AXIS_COUNT
#    define JOYSTICK_AXIS_COUNT 2
#elif JOYSTICK_AXIS_COUNT > 6
#    error Joystick feature only supports up to 6 axes
#endif

#if JOYSTICK_AXIS_COUNT == 0 && JOYSTICK_BUTTON_COUNT == 0
#    error Joystick feature requires at least one axis or button
#endif

#ifndef JOYSTICK_AXIS_RESOLUTION
#    define JOYSTICK_AXIS_RESOLUTION 8
#elif JOYSTICK_AXIS_RESOLUTION < 8 || JOYSTICK_AXIS_RESOLUTION > 16
#    error JOYSTICK_AXIS_RESOLUTION must be between 8 and 16
#endif

#define JOYSTICK_MAX_VALUE ((1L << (JOYSTICK_AXIS_RESOLUTION - 1)) - 1)

#define JOYSTICK_HAT_CENTER -1
#define JOYSTICK_HAT_NORTH 0
#define JOYSTICK_HAT_NORTHEAST 1
#define JOYSTICK_HAT_EAST 2
#define JOYSTICK_HAT_SOUTHEAST 3
#define JOYSTICK_HAT_SOUTH 4
#define JOYSTICK_HAT_SOUTHWEST 5
#define JOYSTICK_HAT_WEST 6
#define JOYSTICK_HAT_NORTHWEST 7

// configure on input_pin of the joystick_axes array entry to NO_PIN
// to prevent it from being read from the ADC. This allows outputting forged axis value.
#define JOYSTICK_AXIS_VIRTUAL \
{ NO_PIN, 0, JOYSTICK_MAX_VALUE / 2, JOYSTICK_MAX_VALUE }
#define JOYSTICK_AXIS_IN(INPUT_PIN, LOW, REST, HIGH) \
{ INPUT_PIN, LOW, REST, HIGH }

typedef struct {
    //pin_t input_pin;

    // the AVR ADC offers 10 bit precision, with significant bits on the higher part
    uint16_t min_digit;
    uint16_t mid_digit;
    uint16_t max_digit;
} joystick_config_t;

extern joystick_config_t joystick_axes[JOYSTICK_AXIS_COUNT];

#if JOYSTICK_AXIS_RESOLUTION > 8
typedef int16_t JoystickAxis;
#else
typedef int8_t JoystickAxis;
#endif

typedef struct {
#if JOYSTICK_AXIS_COUNT > 0
    JoystickAxis axes[JOYSTICK_AXIS_COUNT];
#endif

#ifdef JOYSTICK_HAS_HAT
    int8_t  hat : 4;
    uint8_t reserved : 4;
#endif

#if JOYSTICK_BUTTON_COUNT > 0
    uint8_t buttons[(JOYSTICK_BUTTON_COUNT - 1) / 8 + 1];
#endif
} __PACKED Joystick;

extern Joystick g_joystick;

#define joystick_buffer_clear(x) memset((x), 0, sizeof(Joystick))

void joystick_add_buffer(Keycode keycode);
void joystick_set_axis(Keycode keycode, AnalogValue value);
int joystick_buffer_send(Joystick *buf);
int joystick_hid_send(uint8_t *report, uint16_t len);

#endif //JOYSTICK_H
