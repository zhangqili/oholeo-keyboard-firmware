/*
 * analog.c
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "stdlib.h"
#include "stdio.h"
#include "rgb.h"
#include "analog.h"
#include "record.h"
#include "advanced_key.h"

uint16_t g_ADC_Conversion_Count;
AnalogValue g_ADC_Averages[ADVANCED_KEY_NUM];

AdaptiveSchimidtFilter g_analog_filters[ADVANCED_KEY_NUM];

RingBuf adc_ringbuf[ADVANCED_KEY_NUM];

uint8_t g_analog_active_channel;

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
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        g_ADC_Averages[i] = ringbuf_avg(&adc_ringbuf[i]);
#ifdef ENABLE_FILTER
        g_ADC_Averages[i] = adaptive_schimidt_filter(g_analog_filters+i,g_ADC_Averages[i]);
#endif
    }
}

void analog_check(void)
{
    bool state;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        state = g_keyboard_advanced_keys[i].key.state;
        if (g_keyboard_advanced_keys[i].mode != KEY_DIGITAL_MODE)
        {
            advanced_key_update_raw(g_keyboard_advanced_keys + i, g_ADC_Averages[i]);
        }
        if (g_keyboard_advanced_keys[i].key.state && !state)
        {
            
#ifdef ENABLE_RGB
            rgb_activate(&g_keyboard_advanced_keys[i].key);
#endif
#ifdef ENABLE_KPS
            record_kps_tick();
#endif
#ifdef ENABLE_COUNTER
            g_key_counts[i]++;
#endif
        }
    }
}

void analog_reset_range(void)
{
    analog_average();
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        advanced_key_reset_range(g_keyboard_advanced_keys + i, g_ADC_Averages[i]);
    }
}

void ringbuf_push(RingBuf* ringbuf, AnalogValue data)
{
    ringbuf->pointer++;
    if (ringbuf->pointer >= RING_BUF_LEN)
    {
        ringbuf->pointer = 0;
    }
    ringbuf->datas[ringbuf->pointer] = data;
}

AnalogValue ringbuf_avg(RingBuf* ringbuf)
{
    uint32_t avg = 0;
    for (int i = 0; i < RING_BUF_LEN; i++)
    {
        avg += ringbuf->datas[i];
    }

    avg /= RING_BUF_LEN;
    //avg = ((avg >> 2) & 0x01) + (avg >> 3);

    return (AnalogValue)avg;
}
