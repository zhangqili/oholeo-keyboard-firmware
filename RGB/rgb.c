/*
 * rgb.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "rgb.h"

#ifdef USE_RGB
uint8_t RGB_Buffer[RGB_BUFFER_LENGTH];
#endif
rgb_global_config_t RGB_GlobalConfig;
rgb_individual_config_t RGB_Configs[RGB_NUM];
lefl_color_rgb_t RGB_Colors[RGB_NUM];
uint8_t RGB_TargetConfig;
lefl_loop_queue_t RGB_Argument_Queues[RGB_NUM];
#define ARGUMENT_BUFFER_LENGTH 16
static lefl_loop_queue_elm_t RGB0_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];
static lefl_loop_queue_elm_t RGB1_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];
static lefl_loop_queue_elm_t RGB2_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];
static lefl_loop_queue_elm_t RGB3_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];
static lefl_loop_queue_elm_t RGB4_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];

#ifdef USE_RGB
void RGB_Init()
{
    lefl_loop_queue_init(RGB_Argument_Queues + 0, RGB0_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    lefl_loop_queue_init(RGB_Argument_Queues + 1, RGB1_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    lefl_loop_queue_init(RGB_Argument_Queues + 2, RGB2_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    lefl_loop_queue_init(RGB_Argument_Queues + 3, RGB3_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    lefl_loop_queue_init(RGB_Argument_Queues + 4, RGB4_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    for (uint16_t i = 0; i < 400; i++) {
        RGB_Buffer[i] = NONE_PULSE;
    }
}

#define COLOR_INTERVAL(key,low,up) (uint8_t)((key)<0?(low):((key)>1.0?(up):(key)*(up)))
void RGB_Update()
{
    lefl_color_hsv_t temp_hsv;
    uint16_t tempint;
    float tempf;
    memcpy(&temp_hsv,&(RGB_GlobalConfig.hsv),sizeof(lefl_color_hsv_t));
    switch (RGB_GlobalConfig.mode)
    {
        case RGB_GLOBAL_MODE_INDIVIDUAL:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                switch (RGB_Configs[i].mode)
                {
                    case RGB_MODE_REACT_LINEAR:
                        RGB_Colors[i].r=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[i].rgb.r));
                        RGB_Colors[i].g=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[i].rgb.g));
                        RGB_Colors[i].b=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[i].rgb.b));
                        break;
                    case RGB_MODE_REACT_TRIGGER:
                        RGB_Configs[i].argument = Keyboard_AdvancedKeys[i].key.state?1.0:RGB_Configs[i].argument*(1.0-fabsf(RGB_Configs[i].speed));
                        RGB_Colors[i].r=(uint8_t)((float)(RGB_Configs[i].rgb.r)*RGB_Configs[i].argument);;
                        RGB_Colors[i].g=(uint8_t)((float)(RGB_Configs[i].rgb.g)*RGB_Configs[i].argument);;
                        RGB_Colors[i].b=(uint8_t)((float)(RGB_Configs[i].rgb.b)*RGB_Configs[i].argument);;
                        break;
                    case RGB_MODE_STATIC:
                        RGB_Colors[i].r=RGB_Configs[i].rgb.r;
                        RGB_Colors[i].g=RGB_Configs[i].rgb.g;
                        RGB_Colors[i].b=RGB_Configs[i].rgb.b;
                        break;
                    case RGB_MODE_CYCLE:
                        RGB_Configs[i].argument+=RGB_Configs[i].speed;
                        if(RGB_Configs[i].argument>359.9f)
                        {
                            RGB_Configs[i].argument=0;
                        }
                        if(RGB_Configs[i].argument<0)
                        {
                            RGB_Configs[i].argument=360;
                        }
                        temp_hsv.s = RGB_Configs[i].hsv.s;
                        temp_hsv.v = RGB_Configs[i].hsv.v;
                        temp_hsv.h = (RGB_Configs[i].hsv.h+(uint16_t)(RGB_Configs[i].argument))%360;
                        lefl_color_set_hsv(RGB_Colors + i, &temp_hsv);
                        break;
                    default:
                        break;
                }
                RGB_Buffer[i] = NONE_PULSE;
            }
            break;
        case RGB_GLOBAL_MODE_WAVE:
            RGB_GlobalConfig.argument+=RGB_GlobalConfig.speed;
            if(RGB_GlobalConfig.argument>359.9f)
            {
                RGB_GlobalConfig.argument=0;
            }
            if(RGB_GlobalConfig.argument<0)
            {
                RGB_GlobalConfig.argument=360;
            }
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM+60; i++)
            {
                temp_hsv.h = ((uint16_t)RGB_GlobalConfig.argument + i*15)%360;
                lefl_color_set_hsv(RGB_Colors+i, &temp_hsv);
            }
            break;
        case RGB_GLOBAL_MODE_OFF:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                RGB_Colors[i].r=0;
                RGB_Colors[i].g=0;
                RGB_Colors[i].b=0;
            }
            break;
        case RGB_GLOBAL_MODE_RIPPLE:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                RGB_Colors[i].r=0;
                RGB_Colors[i].g=0;
                RGB_Colors[i].b=0;
            }
            for (int8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                lefl_loop_queue_foreach(RGB_Argument_Queues+i,j)
                {
                    RGB_Argument_Queues[i].data[j] += RGB_GlobalConfig.speed;
                    if(RGB_Argument_Queues[i].data[j]>6.0f)
                    {
                        lefl_loop_queue_dequeue(RGB_Argument_Queues+i);
                    }
                }
                for (int8_t j = 0; j < ADVANCED_KEY_NUM; j++)
                {
                    lefl_loop_queue_foreach(RGB_Argument_Queues+i,k)
                    {
                        tempf = (1.0f-fabsf(i+RGB_Argument_Queues[i].data[k]-j));
                        tempf = tempf > 0 ? tempf : 0;
                        //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                        //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                        tempint = RGB_Colors[j].r;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.r))))>>1;
                        RGB_Colors[j].r=tempint<256?tempint:255;
                        //printf("%d\t",tempr);

                        tempint = RGB_Colors[j].g;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.g))))>>1;
                        RGB_Colors[j].g=tempint<256?tempint:255;

                        tempint = RGB_Colors[j].b;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.b))))>>1;
                        RGB_Colors[j].b=tempint<256?tempint:255;

                        tempf = (1.0f-fabsf(i-RGB_Argument_Queues[i].data[k]-j));
                        tempf = tempf > 0 ? tempf : 0;

                        tempint = RGB_Colors[j].r;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.r))))>>1;
                        RGB_Colors[j].r=tempint<256?tempint:255;

                        tempint = RGB_Colors[j].g;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.g))))>>1;
                        RGB_Colors[j].g=tempint<256?tempint:255;

                        tempint = RGB_Colors[j].b;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.b))))>>1;
                        RGB_Colors[j].b=tempint<256?tempint:255;

                    }
                }
            }
            break;
        default:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                RGB_Colors[i].r=0;
                RGB_Colors[i].g=0;
                RGB_Colors[i].b=0;
            }
            break;
    }
    RGB_Set(RGB_Colors[0].r,RGB_Colors[0].g,RGB_Colors[0].b,0);
    RGB_Set(RGB_Colors[1].r,RGB_Colors[1].g,RGB_Colors[1].b,1);
    RGB_Set(RGB_Colors[2].r,RGB_Colors[2].g,RGB_Colors[2].b,2);
    RGB_Set(RGB_Colors[3].r,RGB_Colors[3].g,RGB_Colors[3].b,3);
}

void RGB_Set(uint8_t r,uint8_t g,uint8_t b,uint16_t index)
{
  for (uint8_t i = 0;i < 8;i++)
  {
    RGB_Buffer[RGB_RESET_LENGTH+index*24+i]      = (g << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
    RGB_Buffer[RGB_RESET_LENGTH+index*24+i + 8]  = (r << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
    RGB_Buffer[RGB_RESET_LENGTH+index*24+i + 16] = (b << i) & (0x80)?ONE_PULSE:ZERO_PULSE;
  }
}

void RGB_Flash()
{
    for (uint8_t i=1;i<127;i++)
    {
      RGB_Set(i,i,i,0);
      RGB_Set(i,i,i,1);
      RGB_Set(i,i,i,2);
      RGB_Set(i,i,i,3);
      HAL_Delay(2);
    }
    for (uint8_t i=128;i>3;i--)
    {
      RGB_Set(i,i,i,0);
      RGB_Set(i,i,i,1);
      RGB_Set(i,i,i,2);
      RGB_Set(i,i,i,3);
      HAL_Delay(2);
    }
    RGB_TurnOff();
}

void RGB_TurnOff()
{
	for (uint8_t i=0;i<RGB_NUM;i++)
    {
        RGB_Set(0,0,0,i);
    }
}
#endif


void RGB_Recovery()
{
}

void RGB_Save()
{
}

