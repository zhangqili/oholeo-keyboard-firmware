/*
 * analog.c
 *
 *  Created on: 2023年5月21日
 *      Author: xq123
 */
#include "main.h"
#include "usart.h"
#include "stdlib.h"
#include "stdio.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "analog.h"
#include "rgb.h"
#include "lefl.h"

uint32_t ADC_Buffer[4];
#define EXTENDED_SAMPLING
AnalogItem AnalogDatas[ADVANCED_KEY_NUM];
#ifdef EXTENDED_SAMPLING
AnalogItem LastAnalogDatas[ADVANCED_KEY_NUM];
#define ANALOG_AVERAGE(x) ((AnalogDatas[x].sum+LastAnalogDatas[x].sum)/(AnalogDatas[x].count+LastAnalogDatas[x].count))
#else
#define ANALOG_AVERAGE(x) ((AnalogDatas[x].sum)/(AnalogDatas[x].count))
#endif

uint16_t Analog_Buffer[ADVANCED_KEY_NUM];

uint8_t Analog_ActiveChannel;
uint32_t Analog_Count;
bool Analog_ConvCpltFlag[4];
#define BCD_TO_GRAY(x) (x^(x>>1))

void Analog_Init()
{
}


void Analog_Start()
{
    Analog_ActiveChannel=0;
    /*
    HAL_ADC_Start_DMA(&hadc1, (ADC_Buffer+0), 1);
    HAL_ADC_Start_DMA(&hadc2, (ADC_Buffer+1), 1);
    HAL_ADC_Start_DMA(&hadc3, (ADC_Buffer+2), 1);
    HAL_ADC_Start_DMA(&hadc4, (ADC_Buffer+3), 1);
    */
    //HAL_ADC_Start_IT(&hadc1);
    //HAL_ADC_Start_IT(&hadc2);
    Analog_Scan();
}

void Analog_Channel_Select(uint8_t x)
{
    x=BCD_TO_GRAY(x);
    HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, x&0x01);
    HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, x&0x02);
    HAL_GPIO_WritePin(C_GPIO_Port, C_Pin, x&0x04);
    HAL_GPIO_WritePin(D_GPIO_Port, D_Pin, x&0x08);
}

void Analog_Scan()
{
    Analog_Channel_Select(Analog_ActiveChannel);
    //HAL_ADC_Start(&hadc2);
    //HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)(Analog_Buffer), 1);
    //HAL_ADC_Start_IT(&hadc3);
    HAL_ADC_Start_IT(&hadc4);
}

void Analog_Average()
{

}

void Analog_Check()
{
    bool state;
    rgb_argument_t a;
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        state=Keyboard_AdvancedKeys[i].key.state;
        if(ANALOG_AVERAGE(i)<Keyboard_AdvancedKeys[i].lower_bound)
        {
            lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, Keyboard_AdvancedKeys[i].upper_bound,ANALOG_AVERAGE(i));
            lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, 0.05, 0.2);
        }
        if(Keyboard_AdvancedKeys[i].mode!=LEFL_KEY_DIGITAL_MODE)
        lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+i, ANALOG_AVERAGE(i));
                if(Keyboard_AdvancedKeys[i].key.state&&!state)
        {
            a.rgb_ptr = RGB_Mapping[i];
            a.argument=0.0;
            rgb_loop_queue_enqueue(&RGB_Argument_Queue, a);
        }
            //lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+i, (((float)(AnalogItems.sum))/(float)(AnalogItems.count)));
    }
}


void Analog_Recovery()
{

}

void Analog_Flush()
{
#ifdef EXTENDED_SAMPLING
    memcpy(LastAnalogDatas,AnalogDatas,sizeof(AnalogDatas));
#endif
    memset(AnalogDatas,0,sizeof(AnalogDatas));
}

#define ADDRESS BCD_TO_GRAY(Analog_ActiveChannel)

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc==&hadc1)
    {
        Analog_ConvCpltFlag[0]=true;
        Analog_ConvCpltFlag[1]=true;
    }
    if (hadc==&hadc3)
    {
        Analog_ConvCpltFlag[2]=true;
        Analog_ConvCpltFlag[3]=true;
        //HAL_ADC_Stop_DMA(&hadc3);
    }
    if (Analog_ConvCpltFlag[0]&&Analog_ConvCpltFlag[2])
    {
        Analog_ConvCpltFlag[0]=false;
        Analog_ConvCpltFlag[1]=false;
        Analog_ConvCpltFlag[2]=false;
        Analog_ConvCpltFlag[3]=false;
        AnalogDatas[0*16+ADDRESS].sum+=HAL_ADC_GetValue(&hadc1);
        AnalogDatas[0*16+ADDRESS].count++;
        AnalogDatas[1*16+ADDRESS].sum+=HAL_ADC_GetValue(&hadc2);
        AnalogDatas[1*16+ADDRESS].count++;
        AnalogDatas[2*16+ADDRESS].sum+=HAL_ADC_GetValue(&hadc3);
        AnalogDatas[2*16+ADDRESS].count++;
        AnalogDatas[3*16+ADDRESS].sum+=HAL_ADC_GetValue(&hadc4);
        AnalogDatas[3*16+ADDRESS].count++;
        Analog_ActiveChannel++;
        if(Analog_ActiveChannel>=16)
            Analog_ActiveChannel=0;
        Analog_Channel_Select(Analog_ActiveChannel);
    }
    Analog_Count++;
}
