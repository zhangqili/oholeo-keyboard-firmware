/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "joystick.h"
#include "keyboard_def.h"

Joystick g_joystick;

void joystick_add_buffer(Keycode keycode)
{
    if (keycode >= JOYSTICK_BUTTON_COUNT) return;

    g_joystick.buttons[keycode / 8] |= 1 << (keycode % 8);
    //g_joystick.dirty = true;
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
        g_joystick.axes[axis] += analog_value;
        break;
    case 0x02:
        g_joystick.axes[axis] -= analog_value;
        break;
    case 0x03:
        g_joystick.axes[axis] += ((keycode & 0x80) ? -(analog_value*2 - JOYSTICK_MAX_VALUE) : (analog_value*2 - JOYSTICK_MAX_VALUE));
        break;
    default:
        break;
    }

}
int joystick_buffer_send(Joystick *buf)
{
    return joystick_hid_send((uint8_t*)buf, sizeof(Joystick));
}

__WEAK int joystick_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
    return 0;
}