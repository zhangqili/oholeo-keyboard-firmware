/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "joystick.h"
#include "keyboard_def.h"
#include "string.h"

static Joystick joystick;

void joystick_event_handler(KeyboardEvent event)
{
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_UP:
    case KEYBOARD_EVENT_KEY_DOWN:
        BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        joystick_add_buffer(MODIFIER(event.keycode));
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
    }
}

void joystick_buffer_clear(void)
{
    memset(&joystick, 0, sizeof(Joystick));
}

void joystick_add_buffer(Keycode keycode)
{
    if (keycode >= JOYSTICK_BUTTON_COUNT) return;

    joystick.buttons[keycode / 8] |= 1 << (keycode % 8);
    //joystick.dirty = true;
}

void joystick_set_axis(Keycode keycode, AnalogValue value)
{
    if (!(keycode & 0xE0)) return;
    uint8_t axis = (keycode & 0x1F);
    if (axis >= JOYSTICK_AXIS_COUNT) return;
    JoystickAxis analog_value = (value - ANALOG_VALUE_MIN) / (float)ANALOG_VALUE_RANGE * JOYSTICK_MAX_VALUE;
    switch ((keycode >> 5) & 0x03)
    {
    case 0x01:
        joystick.axes[axis] += analog_value;
        break;
    case 0x02:
        joystick.axes[axis] -= analog_value;
        break;
    case 0x03:
        joystick.axes[axis] += ((keycode & 0x80) ? -(analog_value*2 - JOYSTICK_MAX_VALUE) : (analog_value*2 - JOYSTICK_MAX_VALUE));
        break;
    default:
        break;
    }

}
int joystick_buffer_send(void)
{
    return joystick_hid_send((uint8_t*)&joystick, sizeof(Joystick));
}

__WEAK int joystick_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
    return 0;
}