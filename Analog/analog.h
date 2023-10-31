/*
 * analog.h
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "keyboard.h"
#include "keyboard_conf.h"
#include "lefl.h"

#ifndef ANALOG_H_
#define ANALOG_H_

//#define EXTENDED_SAMPLING

typedef struct
{
     uint32_t sum;
     uint32_t count;
} AnalogItem;

extern uint32_t ADC_Buffer[4];
extern AnalogItem AnalogDatas[ADVANCED_KEY_NUM];


#ifdef EXTENDED_SAMPLING
extern AnalogItem LastAnalogDatas[ADVANCED_KEY_NUM];
#define ANALOG_AVERAGE(x) ((AnalogDatas[x].sum+LastAnalogDatas[x].sum)/(AnalogDatas[x].count+LastAnalogDatas[x].count))
#else
#define ANALOG_AVERAGE(x) ((AnalogDatas[x].sum)/(AnalogDatas[x].count))
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

#endif /* ANALOG_H_ */
