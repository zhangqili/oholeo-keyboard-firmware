/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef EVENT_H
#define EVENT_H
#include "stdint.h"
#include "keycode.h"

typedef enum
{
    KEYBOARD_EVENT_KEY_UP,
    KEYBOARD_EVENT_KEY_DOWN,
    KEYBOARD_EVENT_KEY_TRUE,
    KEYBOARD_EVENT_KEY_FALSE,
    KEYBOARD_EVENT_NUM
} KeyboardEventType;

typedef struct
{
    Keycode keycode;
    uint8_t event;
} KeyboardEvent;
#define MK_EVENT(keycode, event) ((KeyboardEvent){(keycode), (event)})

#endif //EVENT_H
