/*
 * analog.h
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "keyboard.h"
#include "keyboard_conf.h"

#ifndef ANALOG_H_
#define ANALOG_H_


#define ADDRESS BCD_TO_GRAY(Analog_ActiveChannel)
#define BCD_TO_GRAY(x) (x^(x>>1))

#define TOLERANCE 3
#define RING_BUF_LEN 8
#define DMA_BUF_LEN 10
//#define EXTENDED_SAMPLING

typedef struct
{
     uint32_t Datas[RING_BUF_LEN];
     uint32_t Pointer;
     uint32_t state;
}RingBuf;

typedef struct
{
     uint32_t sum;
     uint32_t count;
} AnalogItem;



extern uint32_t DMA_Buffer[4][DMA_BUF_LEN];
extern float ADC_Buffer[ADVANCED_KEY_NUM];
//extern AnalogItem AnalogDatas[ADVANCED_KEY_NUM];
extern RingBuf adc_ringbuf[ADVANCED_KEY_NUM];

#ifdef EXTENDED_SAMPLING
extern AnalogItem LastAnalogDatas[ADVANCED_KEY_NUM];
#endif

extern uint16_t Analog_Buffer[ADVANCED_KEY_NUM];

extern uint8_t Analog_ActiveChannel;
extern bool Analog_ConvCpltFlag[4];

extern uint32_t Analog_Count;

void Analog_Channel_Select(uint8_t x);
void Analog_Scan();
void Analog_Init();
void Analog_Average();
void Analog_Check();
void Analog_Recovery();
void Analog_Flush();
void Analog_Start();
void Analog_Save();

void RingBuf_Push(RingBuf *ringbuf, uint32_t data);
float RingBuf_Avg(RingBuf *ringbuf);

#endif /* ANALOG_H_ */
