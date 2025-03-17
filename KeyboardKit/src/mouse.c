/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "mouse.h"

__attribute__((section(".noncacheable"))) __attribute__((aligned(4))) Mouse g_mouse;

void mouse_add_buffer(Keycode keycode)
{
    switch (keycode)
    {
    case MOUSE_LBUTTON:
        g_mouse.buttons |= 0x01;
        break;
    case MOUSE_RBUTTON:
        g_mouse.buttons |= 0x02;
        break;
    case MOUSE_MBUTTON:
        g_mouse.buttons |= 0x04;
        break;
    case MOUSE_FORWARD:
        g_mouse.buttons |= 0x08;
        break;
    case MOUSE_BACK:
        g_mouse.buttons |= 0x10;
        break;
    case MOUSE_WHEEL_UP:
        g_mouse.v = 1;
        break;
    case MOUSE_WHEEL_DOWN:
        g_mouse.v = -1;
        break;
    default:
        break;
    }
}

int mouse_buffer_send(Mouse *buf)
{
    return mouse_hid_send((uint8_t*)buf, sizeof(Mouse));
}

__WEAK int mouse_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
    return 0;
}