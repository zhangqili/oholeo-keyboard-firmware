/*
 * analog.h
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#ifndef ANALOG_H_
#define ANALOG_H_
#include "keyboard.h"
#include "filter.h"

#define Analog_Clean() (ADC_Conversion_Count=0)
#define ADDRESS BCD_TO_GRAY(g_analog_active_channel)
#define BCD_TO_GRAY(x) (x^(x>>1))

#define RING_BUF_LEN 8
#define DMA_BUF_LEN 10
#define ANALOG_BUFFER_LENGTH (4 * DMA_BUF_LEN)

typedef struct
{
     uint32_t datas[RING_BUF_LEN];
     uint32_t pointer;
}RingBuf;

extern uint16_t g_ADC_Conversion_Count;
extern uint32_t g_ADC_Buffer[ANALOG_BUFFER_LENGTH];
extern float g_ADC_Averages[ADVANCED_KEY_NUM];

extern AdaptiveSchimidtFilter g_analog_filters[ADVANCED_KEY_NUM];

extern RingBuf adc_ringbuf[ADVANCED_KEY_NUM];

extern uint8_t g_analog_active_channel;

void analog_init();
void analog_channel_select(uint8_t x);
void analog_scan();
void analog_average();
void analog_check();
void analog_reset_range();

void ringbuf_push(RingBuf *ringbuf, uint32_t data);
float ringbuf_avg(RingBuf *ringbuf);

#endif /* ANALOG_H_ */
