/*
 * analog.h
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "keyboard.h"
#include "lefl.h"

#ifndef ANALOG_H_
#define ANALOG_H_

typedef struct
{
     uint32_t sum;
     uint32_t count;
} AnalogItem;

extern uint32_t ADC_Buffer[4];
extern AnalogItem AnalogItems[ADVANCED_KEY_NUM];

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
void Analog_Clean();
void Analog_Start();
void Analog_Save();

#endif /* ANALOG_H_ */
