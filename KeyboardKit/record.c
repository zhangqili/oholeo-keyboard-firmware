/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "record.h"

#ifdef ENABLE_ANALOG_HISTORY
LoopArray g_analog_historys[ADVANCED_KEY_NUM];
static LoopArrayElement analog_datas[ADVANCED_KEY_NUM][ANALOG_HISTORY_LENGTH];
#endif

#ifdef ENABLE_KPS
LoopArray g_kps_data;
uint16_t g_kps;
static LoopArrayElement kps_buf;
static LoopArrayElement kps_data[KPS_REFRESH_RATE];
#endif

#ifdef ENABLE_KPS_HISTORY
LoopArray g_kps_history;
uint16_t g_kps_max;
static LoopArrayElement kps_history_data[KPS_HISTORY_LENGTH];
#endif

#ifdef ENABLE_BIT_STREAM
size_t g_bit_stream_datas[ADVANCED_KEY_NUM][BIT_DATA_LENGTH];
#endif

#ifdef ENABLE_COUNTER
uint32_t g_key_init_counts[ADVANCED_KEY_NUM];
uint32_t g_key_counts[ADVANCED_KEY_NUM];
#endif
// static Key *kps_queue[RECORD_MAX_KEY_NUM];
// static Key *bit_queue[RECORD_MAX_KEY_NUM];
// static AdvancedKey *analog_queue[RECORD_MAX_KEY_NUM];
void loop_array_init(LoopArray *arr,LoopArrayElement *data,uint16_t len)
{
    arr->data = data;
    arr->len = len;
    arr->index = 0;
}

void loop_array_push_back(LoopArray *arr, LoopArrayElement t)
{
    arr->index++;
    if (arr->index >= arr->len)
    {
        arr->index = 0;
    }
    arr->data[arr->index] = t;
}

LoopArrayElement loop_array_get(LoopArray *arr, LoopArrayElement j)
{
    if (j < arr->len)
    {
        return arr->data[arr->index - j >= 0 ? arr->index - j : arr->index - j + arr->len];
    }
    else
    {
        return 0;
    }
}

LoopArrayElement loop_array_max(LoopArray *arr)
{
    LoopArrayElement max = 0;
    for(uint8_t i=0;i<arr->len;i++)
    {
        if(arr->data[i]>max)
            max=arr->data[i];
    }
    return max;
}

void record_init()
{
    for (int i = 0; i < ADVANCED_KEY_NUM; i++)
    {
#ifdef ENABLE_ANALOG_HISTORY
        loop_array_init(&g_analog_historys[i],analog_datas[i],ANALOG_HISTORY_LENGTH);
#endif
#ifdef ENABLE_KPS
        loop_array_init(&g_kps_data,kps_data,KPS_REFRESH_RATE);
#endif
#ifdef ENABLE_KPS_HISTORY
        loop_array_init(&g_kps_history,kps_history_data,KPS_HISTORY_LENGTH);
#endif
#ifdef ENABLE_BIT_STREAM
#endif
    }
    
}

/*
void record_kps_register(Key*k)
{
    for (uint8_t i = 0; i < RECORD_MAX_KEY_NUM; i++)
    {
        if(kps_queue[i]!=NULL)
        {
            kps_queue[i]=k;
            return;
        }
    }
}

void record_bit_stream_register(Key*k)
{
    for (uint8_t i = 0; i < RECORD_MAX_KEY_NUM; i++)
    {
        if(bit_queue[i]!=NULL)
        {
            bit_queue[i]=k;
            return;
        }
    }
}
void record_analog_register(AdvancedKey*k)
{
    for (uint8_t i = 0; i < RECORD_MAX_KEY_NUM; i++)
    {
        if(analog_queue[i]!=NULL)
        {
            analog_queue[i]=k;
            return;
        }
    }
}
*/

#ifdef ENABLE_BIT_STREAM
void record_bit_stream_timer()
{
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        for (int16_t j = BIT_DATA_LENGTH - 1; j > 0; j--)
        {
            g_bit_stream_datas[i][j] <<= 1;
            g_bit_stream_datas[i][j] |= (g_bit_stream_datas[i][j - 1] >> ((sizeof(size_t) * 8) - 1));
        }
        g_bit_stream_datas[i][0] <<= 1;
        g_bit_stream_datas[i][0] |= g_keyboard_advanced_keys[i].key.state;
    }
}
#endif

#ifdef ENABLE_ANALOG_HISTORY
void record_analog_timer()
{
    for (int i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        loop_array_push_back(g_analog_historys+i, (uint16_t)g_keyboard_advanced_keys[i].raw);
    }
    
}
#endif

#ifdef ENABLE_KPS_HISTORY
void record_kps_history_timer()
{
    loop_array_push_back(&g_kps_history,g_kps_max);
    g_kps_max=0;
}

void record_kps_timer()
{
    loop_array_push_back(&g_kps_data,kps_buf);
    g_kps = record_get_kps();
    if(g_kps_max < g_kps)
    {
        g_kps_max = g_kps;
    }
    kps_buf=0;
}
#endif

#ifdef ENABLE_KPS
void record_kps_tick()
{
    kps_buf++;
}

uint16_t record_get_kps()
{
    uint16_t kps = 0;
    for (uint8_t i = 0; i < KPS_REFRESH_RATE; i++)
    {
        kps += g_kps_data.data[i];
    }
    return kps;
}
#endif