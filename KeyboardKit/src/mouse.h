/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef MOUSE_H_
#define MOUSE_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __Mouse {
    uint8_t buttons;
    int8_t x;
    int8_t y;
    int8_t v;
    int8_t h;
} Mouse;

//void mouse_buffer_clear(Mouse*mouse);
void mouse_event_handler(KeyboardEvent event);
void mouse_buffer_clear(void);
void mouse_add_buffer(uint16_t keycode);
int mouse_buffer_send(void);
int mouse_hid_send(uint8_t *report, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif /* MOUSE_H_ */
