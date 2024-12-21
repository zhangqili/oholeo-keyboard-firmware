/*
 * analog.h
 *
 *  Created on: 2023骞�5鏈�21鏃�
 *      Author: xq123
 */

#ifndef ANALOG_H_
#define ANALOG_H_
#include "keyboard.h"
#include "filter.h"

#define ADDRESS BCD_TO_GRAY(g_analog_active_channel)
#define BCD_TO_GRAY(x) (x^(x>>1))

#ifndef RING_BUF_LEN
#define RING_BUF_LEN 8
#endif

typedef struct
{
    uint16_t datas[RING_BUF_LEN];
    uint16_t pointer;
}RingBuf;

extern uint16_t g_ADC_Conversion_Count;
extern AnalogValue g_ADC_Averages[ADVANCED_KEY_NUM];

extern AdaptiveSchimidtFilter g_analog_filters[ADVANCED_KEY_NUM];

extern RingBuf adc_ringbuf[ADVANCED_KEY_NUM];

extern uint8_t g_analog_active_channel;

void analog_init(void);
void analog_channel_select(uint8_t x);
void analog_scan(void);
void analog_average(void);
void analog_check(void);
void analog_reset_range(void);

void ringbuf_push(RingBuf *ringbuf, AnalogValue data);
AnalogValue ringbuf_avg(RingBuf *ringbuf);

#endif /* ANALOG_H_ */
