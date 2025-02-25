/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef MOUSE_H_
#define MOUSE_H_

#include "stdint.h"

typedef struct __Mouse {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t v;
    int8_t h;
} Mouse;


#define mouse_buffer_clear(x) memset((x), 0, sizeof(Mouse))
//void mouse_buffer_clear(Mouse*mouse);

void mouse_add_buffer(uint16_t keycode);
int mouse_buffer_send(Mouse *buf);
int mouse_hid_send(uint8_t *report, uint16_t len);

extern Mouse g_mouse;

#endif /* MOUSE_H_ */
