/*
 * analog.c
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "main.h"
#include "stdlib.h"
#include "stdio.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "analog.h"
#include "rgb.h"
#include "keyboard_conf.h"
#include "math.h"

uint16_t g_ADC_Conversion_Count;
uint32_t g_ADC_Buffer[4 * DMA_BUF_LEN];
float g_ADC_Averages[ADVANCED_KEY_NUM];

AdaptiveSchimidtFilter g_analog_filters[ADVANCED_KEY_NUM];

RingBuf adc_ringbuf[ADVANCED_KEY_NUM];

#define ANALOG_AVERAGE(x) (ringbuf_avg(&adc_ringbuf[x]))

uint8_t g_analog_active_channel;

void analog_init()
{
}

void analog_channel_select(uint8_t x)
{
    x=BCD_TO_GRAY(x);
    HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, x&0x01);
    HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, x&0x02);
    HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, x&0x04);
    HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, x&0x08);
}

void analog_scan()
{
    //uint32_t ADC_sum;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        /*
        ADC_sum = 0;
        for (uint8_t j = 0; j < ANALOG_BUFFER_LENGTH; j++)
        {
            ADC_sum += g_ADC_Buffer[i + j * ADVANCED_KEY_NUM];
        }
        g_ADC_Averages[i] = ADC_sum/((float)ANALOG_BUFFER_LENGTH);
#ifdef ENABLE_FILTER
        g_ADC_Averages[i] = adaptive_schimidt_filter(g_analog_filters+i,g_ADC_Averages[i]);
#endif
        */
    }
}

void analog_average()
{
    //uint32_t ADC_sum;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        //ADC_sum = 0;
        //for (uint8_t j = 0; j < ANALOG_BUFFER_LENGTH; j++)
        //{
        //    ADC_sum += g_ADC_Buffer[i + j * ADVANCED_KEY_NUM];
        //}
        g_ADC_Averages[i] = ANALOG_AVERAGE(i);
#ifdef ENABLE_FILTER
        g_ADC_Averages[i] = adaptive_schimidt_filter(g_analog_filters+i,g_ADC_Averages[i]);
#endif
    }
}

void analog_check()
{
    bool state;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        state = g_keyboard_advanced_keys[i].key.state;
        if (g_keyboard_advanced_keys[i].mode != KEY_DIGITAL_MODE)
        {
            switch (g_keyboard_advanced_keys[i].calibration_mode)
            {
            case KEY_AUTO_CALIBRATION_POSITIVE:
                if (g_ADC_Averages[i] > g_keyboard_advanced_keys[i].lower_bound)
                    g_keyboard_advanced_keys[i].lower_bound = g_ADC_Averages[i];
                break;
            case KEY_AUTO_CALIBRATION_NEGATIVE:
                if (g_ADC_Averages[i] < g_keyboard_advanced_keys[i].lower_bound)
                    g_keyboard_advanced_keys[i].lower_bound = g_ADC_Averages[i];
            default:
                break;
            }
            advanced_key_update_raw(g_keyboard_advanced_keys + i, g_ADC_Averages[i]);
        }
        if (g_keyboard_advanced_keys[i].key.state && !state)
        {        	
            extern uint32_t pulse_counter;
        	pulse_counter=0;
#ifdef ENABLE_KPS
            record_kps_tick();
#endif
#ifdef ENABLE_COUNTER
            g_key_counts[i]++;
#endif
        }
    }
}


void ringbuf_push(RingBuf* ringbuf, uint32_t data)
{
    ringbuf->pointer++;
    if (ringbuf->pointer >= RING_BUF_LEN)ringbuf->pointer = 0;
    ringbuf->datas[ringbuf->pointer] = data;
}

float ringbuf_avg(RingBuf* ringbuf)
{
    uint32_t avg = 0;
    for (int i = 0; i < RING_BUF_LEN; i++)
        avg += ringbuf->datas[i];

    avg = ((avg >> 2) & 0x01) + (avg >> 3);
    //  avg = ringbuf->Datas[ringbuf->Pointer];
    if (avg - TOLERANCE > ringbuf->state)ringbuf->state = avg - TOLERANCE;
    if (avg + TOLERANCE < ringbuf->state)ringbuf->state = avg + TOLERANCE;

    return (float)avg;
    //return (float)ringbuf->state;
}
