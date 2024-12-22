/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "mouse.h"

__attribute__((section(".noncacheable"))) __attribute__((aligned(4))) Mouse g_mouse;


void mouse_buffer_send(Mouse *buf)
{
    mouse_hid_send((uint8_t*)buf, sizeof(Mouse));
}

__WEAK void mouse_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
}