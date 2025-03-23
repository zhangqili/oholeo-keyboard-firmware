/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
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

#ifndef ANALOG_BUFFER_LENGTH
#define ANALOG_BUFFER_LENGTH ADVANCED_KEY_NUM
#endif

#ifndef ANALOG_CHANNEL_MAX
#define ANALOG_CHANNEL_MAX 16
#endif


#define ANALOG_NO_MAP    0xFFFF

typedef struct
{
    uint16_t datas[RING_BUF_LEN];
    uint16_t pointer;
}RingBuf;

extern uint16_t g_ADC_Conversion_Count;
extern AnalogRawValue g_ADC_Averages[ANALOG_BUFFER_LENGTH];

extern AdaptiveSchimidtFilter g_analog_filters[ANALOG_BUFFER_LENGTH];

extern RingBuf adc_ringbuf[ANALOG_BUFFER_LENGTH];

extern uint8_t g_analog_active_channel;

extern const uint16_t g_analog_map[ANALOG_BUFFER_LENGTH];

void analog_init(void);
void analog_channel_select(uint8_t x);
void analog_scan(void);
void analog_average(void);
void analog_check(void);
void analog_reset_range(void);

void ringbuf_push(RingBuf *ringbuf, AnalogRawValue data);
AnalogRawValue ringbuf_avg(RingBuf *ringbuf);

#endif /* ANALOG_H_ */
