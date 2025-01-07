/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef RECORD_H_
#define RECORD_H_
#include "keyboard.h"
#include "keyboard_conf.h"

#ifndef KPS_HISTORY_LENGTH
#define KPS_HISTORY_LENGTH    65
#endif

#ifndef BIT_STREAM_LENGTH
#define BIT_STREAM_LENGTH    128
#endif

#ifndef ANALOG_HISTORY_LENGTH
#define ANALOG_HISTORY_LENGTH 129
#endif

#ifndef RECORD_MAX_KEY_NUM
#define RECORD_MAX_KEY_NUM    8
#endif

#ifndef KPS_REFRESH_RATE
#define KPS_REFRESH_RATE      144
#endif

#define BIT_DATA_LENGTH ((BIT_STREAM_LENGTH - 1) / (sizeof(size_t) * 8) + 1)

typedef uint16_t LoopArrayElement;

typedef struct __LoopArray
{
    LoopArrayElement *data;
    int16_t index;
    int16_t len;
} LoopArray;

#ifdef ENABLE_COUNTER
extern uint32_t g_key_init_counts[ADVANCED_KEY_NUM];
extern uint32_t g_key_counts[ADVANCED_KEY_NUM];
#endif

#ifdef ENABLE_KPS
extern uint16_t g_kps;
extern LoopArray g_kps_data;
#endif

#ifdef ENABLE_KPS_HISTORY
extern LoopArray g_kps_history;
extern uint16_t g_kps_max_since_last_timer;
#endif

#ifdef ENABLE_BIT_STREAM
extern size_t g_bit_stream_datas[ADVANCED_KEY_NUM][BIT_DATA_LENGTH];
#endif

#ifdef ENABLE_ANALOG_HISTORY
extern LoopArray g_analog_historys[ADVANCED_KEY_NUM];
#endif
void loop_array_push_back(LoopArray *arr,LoopArrayElement t);
uint16_t loop_array_get(LoopArray *arr,LoopArrayElement j);
LoopArrayElement loop_array_max(LoopArray *arr);
//void record_kps_register(Key*k);
//void record_bit_stream_register(Key*k);
//void record_analog_register(AdvancedKey*k);
void record_init(void);
void record_bit_stream_timer(void);
void record_analog_timer(void);
void record_kps_timer(void);
uint16_t record_get_kps(void);
void record_kps_tick(void);
void record_kps_timer(void);
void record_kps_history_timer(void);

#endif /* RECORD_H_ */
