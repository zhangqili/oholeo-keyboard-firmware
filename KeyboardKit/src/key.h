/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KEY_H_
#define KEY_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

typedef enum
{
    KEY_EVENT_UP,
    KEY_EVENT_DOWN,
    KEY_EVENT_NUM
} KEY_EVENT;
typedef void (*key_cb_t)(void *);
typedef struct __Key
{
    uint16_t id;
    bool state;
    bool report_state;
    key_cb_t key_cb[KEY_EVENT_NUM];
} Key;
void key_update(Key *key, bool state);
void key_attach(Key *key, KEY_EVENT e, key_cb_t cb);

#endif