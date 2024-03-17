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
uint32_t g_ADC_Buffer[4][DMA_BUF_LEN];

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
}

void analog_average()
{

}

void analog_check()
{
    bool state;
    RGBArgument a;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
    	float_t analog_avg = ANALOG_AVERAGE(i);
        state=g_keyboard_advanced_keys[i].key.state;
        if(analog_avg<g_keyboard_advanced_keys[i].lower_bound)
        {
            advanced_key_set_range(g_keyboard_advanced_keys+i, g_keyboard_advanced_keys[i].upper_bound,analog_avg);
            advanced_key_set_deadzone(g_keyboard_advanced_keys+i, DEFAULT_UPPER_DEADZONE, g_keyboard_advanced_keys[i].lower_deadzone);
        }
        if(g_keyboard_advanced_keys[i].mode!=KEY_DIGITAL_MODE)
        advanced_key_update_raw(g_keyboard_advanced_keys+i, analog_avg);
        if(g_keyboard_advanced_keys[i].key.state&&!state)
        {
        	extern uint32_t pulse_counter;
        	pulse_counter=0;

            a.rgb_ptr = g_rgb_mapping[i];
            a.begin_time=HAL_GetTick();
            rgb_loop_queue_enqueue(&g_rgb_argument_queue, a);
        }
            //advanced_key_update_raw(Keyboard_AdvancedKeys+i, (((float)(AnalogItems.sum))/(float)(AnalogItems.count)));
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

    //  return (float_t)avg;
    return (float)ringbuf->state;
}
