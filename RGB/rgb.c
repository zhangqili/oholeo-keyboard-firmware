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
const uint8_t RGB_Mapping[ADVANCED_KEY_NUM]={26,25,24,23,9,10,11,12,36,37,38,39,53,52,51,50,
                                             35,34,33,32,19,20,21,22,46,47,48,49,63,62,61,60,
                                             3,15,29,42,41,56,28,14,13,27,40,55,54,0,1,2,
                                             4,5,6,7,8,18,17,31,44,45,59,58,16,30,43,57};
#define ARGUMENT_BUFFER_LENGTH 16
static lefl_loop_queue_elm_t RGB_Argument_Buffer[RGB_NUM][ARGUMENT_BUFFER_LENGTH];

#ifdef USE_RGB
void RGB_Init()
{
    for (uint8_t i = 0; i < RGB_NUM; i++) {
        lefl_loop_queue_init(RGB_Argument_Queues + i, RGB_Argument_Buffer[i], ARGUMENT_BUFFER_LENGTH);
    }
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
                uint8_t rgb_index = RGB_Mapping[i]?RGB_Mapping[i]-1:0;
                switch (RGB_Configs[rgb_index].mode)
                {
                    case RGB_MODE_REACT_LINEAR:
                        RGB_Colors[rgb_index].r=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[rgb_index].rgb.r));
                        RGB_Colors[rgb_index].g=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[rgb_index].rgb.g));
                        RGB_Colors[rgb_index].b=COLOR_INTERVAL(Keyboard_AdvancedKeys[i].value,0,(float)(RGB_Configs[rgb_index].rgb.b));
                        break;
                    case RGB_MODE_REACT_TRIGGER:
                        RGB_Configs[rgb_index].argument = Keyboard_AdvancedKeys[i].key.state?1.0:RGB_Configs[rgb_index].argument*(1.0-fabsf(RGB_Configs[rgb_index].speed));
                        RGB_Colors[rgb_index].r=(uint8_t)((float)(RGB_Configs[rgb_index].rgb.r)*RGB_Configs[rgb_index].argument);;
                        RGB_Colors[rgb_index].g=(uint8_t)((float)(RGB_Configs[rgb_index].rgb.g)*RGB_Configs[rgb_index].argument);;
                        RGB_Colors[rgb_index].b=(uint8_t)((float)(RGB_Configs[rgb_index].rgb.b)*RGB_Configs[rgb_index].argument);;
                        break;
                    case RGB_MODE_STATIC:
                        RGB_Colors[rgb_index].r=RGB_Configs[rgb_index].rgb.r;
                        RGB_Colors[rgb_index].g=RGB_Configs[rgb_index].rgb.g;
                        RGB_Colors[rgb_index].b=RGB_Configs[rgb_index].rgb.b;
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
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
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
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        RGB_Set(RGB_Colors[i].r,RGB_Colors[i].g,RGB_Colors[i].b,i);
    }
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
    for (uint8_t i=1;i<64;i++)
    {
        for (uint8_t j = 0; j < RGB_NUM; j++)
        {
            RGB_Set(i,i,i,j);
        }
      HAL_Delay(8);
    }
    for (uint8_t i=64;i>1;i--)
    {
        for (uint8_t j = 0; j < RGB_NUM; j++)
        {
            RGB_Set(i,i,i,j);
        }
      HAL_Delay(8);
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
    lefl_color_hsv_t temphsv = {0,0,100};
    for (uint8_t i=0;i<RGB_NUM;i++)
    {
        RGB_Configs[i].mode=RGB_MODE_REACT_LINEAR;
        RGB_Configs[i].hsv=temphsv;
        lefl_color_set_hsv(&RGB_Configs[i].rgb, &temphsv);

    }
    lefl_color_hsv_t hsv = {0,100,10};
    //RGB_GlobalConfig.mode=RGB_GLOBAL_MODE_WAVE;
    RGB_GlobalConfig.mode=RGB_GLOBAL_MODE_INDIVIDUAL;
    RGB_GlobalConfig.speed=0.5;
    RGB_GlobalConfig.hsv=hsv;
    lefl_color_set_hsv(&RGB_GlobalConfig.rgb, &hsv);

}

void RGB_Save()
{
}

