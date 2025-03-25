/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "mouse.h"
#include "string.h"

static Mouse mouse;

void mouse_event_handler(KeyboardEvent event)
{
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_UP:
    case KEYBOARD_EVENT_KEY_DOWN:
        BIT_SET(g_keyboard_send_flags, MOUSE_REPORT_FLAG);
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        mouse_add_buffer(MODIFIER(event.keycode));
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
    }
}


void mouse_buffer_clear(void)
{
    memset(&mouse, 0, sizeof(Mouse));
}

void mouse_add_buffer(Keycode keycode)
{
    switch (keycode)
    {
    case MOUSE_LBUTTON:
        mouse.buttons |= 0x01;
        break;
    case MOUSE_RBUTTON:
        mouse.buttons |= 0x02;
        break;
    case MOUSE_MBUTTON:
        mouse.buttons |= 0x04;
        break;
    case MOUSE_FORWARD:
        mouse.buttons |= 0x08;
        break;
    case MOUSE_BACK:
        mouse.buttons |= 0x10;
        break;
    case MOUSE_WHEEL_UP:
        mouse.v = 1;
        break;
    case MOUSE_WHEEL_DOWN:
        mouse.v = -1;
        break;
    default:
        break;
    }
}

int mouse_buffer_send(void)
{
    static uint32_t mouse_value;
    int ret = 0;
    if ((*(uint32_t*)&mouse)!=mouse_value)
    {
        ret = mouse_hid_send((uint8_t*)&mouse, sizeof(Mouse));
        if (!ret)
        {
            mouse_value = *(uint32_t*)&mouse;
        }
    }
    return ret;
}

__WEAK int mouse_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
    return 0;
}