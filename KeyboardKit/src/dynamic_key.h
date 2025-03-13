/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef DYNAMIC_KEY_H_
#define DYNAMIC_KEY_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "key.h"
#include "keycode.h"
#include "advanced_key.h"

#define DYNAMIC_KEY_NUM 32
typedef enum __DynamicKeyType
{
    DYNAMIC_KEY_NONE,
    DYNAMIC_KEY_STROKE,
    DYNAMIC_KEY_MOD_TAP,
    DYNAMIC_KEY_TOGGLE_KEY,
    DYNAMIC_KEY_MUTEX,
    DYNAMIC_KEY_TYPE_NUM
} DynamicKeyType;

enum
{
    DYNAMIC_KEY_ACTION_TAP,
    DYNAMIC_KEY_ACTION_HOLD,
    DYNAMIC_KEY_ACTION_NUM,
};


typedef struct __DynamicKeyStroke4x4
{
    uint32_t type;
    Keycode key_binding[4];
    uint16_t key_control[4];
    AnalogValue press_begin_distance;
    AnalogValue press_fully_distance;
    AnalogValue release_begin_distance;
    AnalogValue release_fully_distance;
    AnalogValue value;
    uint32_t key_end_time[4];
    uint8_t key_state;
} DynamicKeyStroke4x4;

typedef struct __DynamicKeyModTap
{
    uint32_t type;
    Keycode key_binding[2];
    uint32_t duration;
    uint32_t begin_time;
    uint32_t end_time;
    uint8_t state;
} DynamicKeyModTap;

typedef struct __DynamicKeyToggleKey
{
    uint32_t type;
    Keycode key_binding;
    bool state;
} DynamicKeyToggleKey;

typedef enum __DynamicKeyMutexMode
{
    DK_MUTEX_DISTANCE_PRIORITY,
    DK_MUTEX_LAST_PRIORITY,
    DK_MUTEX_KEY1_PRIORITY,
    DK_MUTEX_KEY2_PRIORITY,
    DK_MUTEX_NEUTRAL
} DynamicKeyMutexMode;

typedef struct __DynamicKeyMutex
{
    uint32_t type;
    Keycode key_binding[2];
    uint16_t key_id[2];
    uint8_t mode;
    bool trigger_state;
} DynamicKeyMutex;

typedef union __DynamicKey
{
    uint32_t type;
    DynamicKeyStroke4x4 dks;
    DynamicKeyModTap mt;
    DynamicKeyToggleKey tk;
    DynamicKeyMutex m;
    uint32_t aligned_buffer[15];
} DynamicKey;

void dynamic_key_update(DynamicKey*dynamic_key, AdvancedKey*advanced_key, bool state);
void dynamic_key_add_buffer(DynamicKey*dynamic_key);
void dynamic_key_s_update (DynamicKey*dynamic_key, AdvancedKey*key, bool state);
void dynamic_key_mt_update(DynamicKey*dynamic_key, AdvancedKey*key, bool state);
void dynamic_key_tk_update(DynamicKey*dynamic_key, AdvancedKey*key, bool state);
void dynamic_key_m_update(DynamicKey*dynamic_key, AdvancedKey*key, bool state);

#endif