/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "stdlib.h"
#include "stdio.h"
#include "rgb.h"
#include "analog.h"
#include "advanced_key.h"

uint16_t g_ADC_Conversion_Count;
AnalogRawValue g_ADC_Averages[ANALOG_BUFFER_LENGTH];

AdaptiveSchimidtFilter g_analog_filters[ANALOG_BUFFER_LENGTH];

RingBuf adc_ringbuf[ANALOG_BUFFER_LENGTH];

uint8_t g_analog_active_channel;

__WEAK const uint16_t g_analog_map[ANALOG_BUFFER_LENGTH];

void analog_init(void)
{
}

__WEAK void analog_channel_select(uint8_t x)
{
    UNUSED(x);
}

void analog_scan(void)
{
}

__WEAK void analog_average(void)
{
    for (uint8_t i = 0; i < ANALOG_BUFFER_LENGTH; i++)
    {
        g_ADC_Averages[i] = ringbuf_avg(&adc_ringbuf[i]);
#ifdef ENABLE_FILTER
        g_ADC_Averages[i] = adaptive_schimidt_filter(&g_analog_filters[i],g_ADC_Averages[i]);
#endif
    }
}

void analog_check(void)
{
    for (uint16_t i = 0; i < ANALOG_BUFFER_LENGTH; i++)
    {
        if ((uint16_t)~g_analog_map[i])
        {
            AdvancedKey*advanced_key = &g_keyboard_advanced_keys[g_analog_map[i]];
            if (advanced_key->config.mode != KEY_DIGITAL_MODE)
            {
                advanced_key_update_raw(advanced_key, g_ADC_Averages[i]);
            }
        }
    }
}

void analog_reset_range(void)
{
    analog_average();
    for (uint16_t i = 0; i < ANALOG_BUFFER_LENGTH; i++)
    {
        if ((uint16_t)~g_analog_map[i])
        {
            advanced_key_reset_range(&g_keyboard_advanced_keys[g_analog_map[i]], g_ADC_Averages[i]);
        }
    }
}

void ringbuf_push(RingBuf* ringbuf, AnalogRawValue data)
{
    ringbuf->pointer++;
    if (ringbuf->pointer >= RING_BUF_LEN)
    {
        ringbuf->pointer = 0;
    }
    ringbuf->datas[ringbuf->pointer] = data;
}

AnalogRawValue ringbuf_avg(RingBuf* ringbuf)
{
    uint32_t avg = 0;
    for (int i = 0; i < RING_BUF_LEN; i++)
    {
        avg += ringbuf->datas[i];
    }
#ifdef OPTIMIZE_FOR_FLOAT_DIVISION
    return (AnalogValue)(avg*(1/((float)RING_BUF_LEN)));
#else
    return (AnalogValue)(avg/RING_BUF_LEN);
#endif
}
