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
AnalogItem AnalogItems[ADVANCED_KEY_NUM];

uint16_t Analog_Buffer[ADVANCED_KEY_NUM];

uint8_t Analog_ActiveChannel;
uint32_t Analog_Count;
bool Analog_ConvCpltFlag[4];


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
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        if(AnalogItems[i].sum/AnalogItems[i].count<Keyboard_AdvancedKeys[i].lower_bound)
        {
            lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, Keyboard_AdvancedKeys[i].upper_bound,AnalogItems[i].sum/AnalogItems[i].count);
            lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, 0.05, 0.2);
        }
        if(Keyboard_AdvancedKeys[i].mode!=LEFL_KEY_DIGITAL_MODE)
            lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+i, AnalogItems[i].sum/AnalogItems[i].count);
            //lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+i, (((float)(AnalogItems.sum))/(float)(AnalogItems.count)));
    }
}


void Analog_Recovery()
{
}

void Analog_Clean()
{

    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        AnalogItems[i].count=0;
        AnalogItems[i].sum=0;

            //lefl_advanced_key_update_raw(Keyboard_AdvancedKeys+i, (((float)(AnalogItems.sum))/(float)(AnalogItems.count)));
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (hadc==&hadc1)
    {
        Analog_ConvCpltFlag[0]=true;
        AnalogItems[0*16+Analog_ActiveChannel].sum+=Analog_Buffer[0];
        AnalogItems[0*16+Analog_ActiveChannel].count++;
        Analog_ConvCpltFlag[1]=true;
        AnalogItems[1*16+Analog_ActiveChannel].sum+=Analog_Buffer[1];
        AnalogItems[1*16+Analog_ActiveChannel].count++;
    }
    if (hadc==&hadc3)
    {
        Analog_ConvCpltFlag[2]=true;
        AnalogItems[2*16+Analog_ActiveChannel].sum+=Analog_Buffer[2];
        AnalogItems[2*16+Analog_ActiveChannel].count++;
        Analog_ConvCpltFlag[3]=true;
        AnalogItems[3*16+Analog_ActiveChannel].sum+=Analog_Buffer[3];
        AnalogItems[3*16+Analog_ActiveChannel].count++;
        //HAL_ADC_Stop_DMA(&hadc3);
    }
    if (Analog_ConvCpltFlag[0]&&Analog_ConvCpltFlag[2])
    {
        Analog_ConvCpltFlag[0]=false;
        Analog_ConvCpltFlag[1]=false;
        Analog_ConvCpltFlag[2]=false;
        Analog_ConvCpltFlag[3]=false;
        Analog_ActiveChannel++;
        if(Analog_ActiveChannel>=16)
            Analog_ActiveChannel=0;
        Analog_Channel_Select(Analog_ActiveChannel);
    }
    Analog_Count++;
}
