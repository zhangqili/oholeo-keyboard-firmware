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
rgb_loop_queue_t RGB_Argument_Queue;
const uint8_t RGB_Mapping[ADVANCED_KEY_NUM]={26,25,24,23,9,10,11,12,36,37,38,39,53,52,51,50,
                                             35,34,33,32,19,20,21,22,46,47,48,49,63,62,61,60,
                                             3,15,29,42,41,56,28,14,13,27,40,55,54,0,1,2,
                                             4,5,6,7,8,18,17,31,44,45,59,58,16,30,43,57};
const rgb_location_t RGB_Locations[RGB_NUM]={{0,0.625},{0,1.875},{0,3.125},{0,6.875},{0,10.5},{0,11.5},{0,12.5},{0,13.5},{0,14.5},
                                             {1,1},{1,2.5},{1,3.5},{1,4.5},{1,5.5},{1,6.5},{1,7.5},{1,8.5},{1,9.5},{1,10.5},{1,11.5},{1,12.5},{1,13.5},{1,14.5},
                                             {2,0.875},{2,2.25},{2,3.25},{2,4.25},{2,5.25},{2,6.25},{2,7.25},{2,8.25},{2,9.25},{2,10.25},{2,11.25},{2,12.25},{2,13.875},
                                             {3,0.75},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7},{3,8},{3,9},{3,10},{3,11},{3,12},{3,13},{3,14.25},
                                             {4,0.5},{4,1.5},{4,2.5},{4,3.5},{4,4.5},{4,5.5},{4,6.5},{4,7.5},{4,8.5},{4,9.5},{4,10.5},{4,11.5},{4,12.5},{4,14}};

static rgb_loop_queue_elm_t RGB_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];

#ifdef USE_RGB
void RGB_Init()
{
    rgb_loop_queue_init(&RGB_Argument_Queue, RGB_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
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
    lefl_loop_queue_foreach(&RGB_Argument_Queue,j)
    {
        RGB_Argument_Queue.data[j].argument += RGB_Configs[RGB_Argument_Queue.data[j].rgb_ptr].speed;
        if(RGB_Argument_Queue.data[j].argument>20.0f)
        {
            rgb_loop_queue_dequeue(&RGB_Argument_Queue);
        }
    }
    switch (RGB_GlobalConfig.mode)
    {
        case RGB_GLOBAL_MODE_INDIVIDUAL:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                uint8_t rgb_index = RGB_Mapping[i];
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
                temp_hsv.h = ((uint16_t)RGB_GlobalConfig.argument + (uint16_t)(RGB_Locations[i].x*15))%360;
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
        case RGB_GLOBAL_MODE_STRING:
        case RGB_GLOBAL_MODE_FADING_STRING:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                RGB_Colors[i].r=0;
                RGB_Colors[i].g=0;
                RGB_Colors[i].b=0;
            }
            lefl_loop_queue_foreach(&RGB_Argument_Queue,k)
            {
                for(int8_t j = 0; j < ADVANCED_KEY_NUM; j++)
                {
                    if(RGB_Locations[RGB_Argument_Queue.data[k].rgb_ptr].row==RGB_Locations[j].row)
                    {
                        tempf = (1.0f-fabsf(RGB_Argument_Queue.data[k].argument-fabsf(RGB_Locations[RGB_Argument_Queue.data[k].rgb_ptr].x-RGB_Locations[j].x)));
                        tempf = tempf > 0 ? tempf : 0;
                        if(RGB_GlobalConfig.mode==RGB_GLOBAL_MODE_FADING_STRING)
                        {
                            tempf/=RGB_Argument_Queue.data[k].argument;
                        }
                        //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                        //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                        tempint = RGB_Colors[j].r;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[j].rgb.r))))>>1;
                        RGB_Colors[j].r=tempint<256?tempint:255;
                        //printf("%d\t",tempr);

                        tempint = RGB_Colors[j].g;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[j].rgb.g))))>>1;
                        RGB_Colors[j].g=tempint<256?tempint:255;

                        tempint = RGB_Colors[j].b;
                        tempint += ((uint8_t)(tempf*((float)(RGB_Configs[j].rgb.b))))>>1;
                        RGB_Colors[j].b=tempint<256?tempint:255;

                    }
                }
            }

            break;
        case RGB_GLOBAL_MODE_DIAMOND_RIPPLE:
        case RGB_GLOBAL_MODE_FADING_DIAMOND_RIPPLE:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                RGB_Colors[i].r=0;
                RGB_Colors[i].g=0;
                RGB_Colors[i].b=0;
            }
            lefl_loop_queue_foreach(&RGB_Argument_Queue,k)
            {
                for (int8_t i = 0; i < ADVANCED_KEY_NUM; i++)
                {
                    tempf = (1.0f-fabsf(RGB_Argument_Queue.data[k].argument*2-fabsf(RGB_Locations[RGB_Argument_Queue.data[k].rgb_ptr].row-RGB_Locations[i].row)-fabsf(RGB_Locations[RGB_Argument_Queue.data[k].rgb_ptr].x-RGB_Locations[i].x)));
                    tempf = tempf > 0 ? tempf : 0;
                    if(RGB_GlobalConfig.mode==RGB_GLOBAL_MODE_FADING_DIAMOND_RIPPLE)
                    {
                        tempf/=RGB_Argument_Queue.data[k].argument;
                    }
                    //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                    //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                    tempint = RGB_Colors[i].r;
                    tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.r))))>>1;
                    RGB_Colors[i].r=tempint<256?tempint:255;
                    //printf("%d\t",tempr);

                    tempint = RGB_Colors[i].g;
                    tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.g))))>>1;
                    RGB_Colors[i].g=tempint<256?tempint:255;

                    tempint = RGB_Colors[i].b;
                    tempint += ((uint8_t)(tempf*((float)(RGB_Configs[i].rgb.b))))>>1;
                    RGB_Colors[i].b=tempint<256?tempint:255;
                }
            }


            break;
        case RGB_GLOBAL_MODE_JELLY:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                RGB_Colors[i].r=0;
                RGB_Colors[i].g=0;
                RGB_Colors[i].b=0;
            }
            for (int8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                for (int8_t j = 0; j < ADVANCED_KEY_NUM; j++)
                {
                    tempf = (10.0f*Keyboard_AdvancedKeys[i].value-fabsf(RGB_Locations[j].row-RGB_Locations[RGB_Mapping[i]].row)-fabsf(RGB_Locations[j].x-RGB_Locations[RGB_Mapping[i]].x));
                    tempf = tempf > 0 ? tempf > 1 ? 1: tempf : 0;
                    //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                    //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                    tempint = RGB_Colors[j].r;
                    tempint += ((uint8_t)(tempf*((float)(RGB_Configs[j].rgb.r))))>>1;
                    RGB_Colors[j].r=tempint<256?tempint:255;
                    //printf("%d\t",tempr);

                    tempint = RGB_Colors[j].g;
                    tempint += ((uint8_t)(tempf*((float)(RGB_Configs[j].rgb.g))))>>1;
                    RGB_Colors[j].g=tempint<256?tempint:255;

                    tempint = RGB_Colors[j].b;
                    tempint += ((uint8_t)(tempf*((float)(RGB_Configs[j].rgb.b))))>>1;
                    RGB_Colors[j].b=tempint<256?tempint:255;
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
    lefl_color_hsv_t temphsv = {273,78,99};
    for (uint8_t i=0;i<RGB_NUM;i++)
    {
        RGB_Configs[i].mode=RGB_MODE_REACT_LINEAR;
        RGB_Configs[i].hsv=temphsv;
        RGB_Configs[i].speed=0.15;
        lefl_color_set_hsv(&RGB_Configs[i].rgb, &temphsv);

    }
    lefl_color_hsv_t hsv = {273,78,99};
    //RGB_GlobalConfig.mode=RGB_GLOBAL_MODE_WAVE;
    RGB_GlobalConfig.mode=RGB_GLOBAL_MODE_INDIVIDUAL;
    RGB_GlobalConfig.speed=0.5;
    RGB_GlobalConfig.hsv=hsv;
    lefl_color_set_hsv(&RGB_GlobalConfig.rgb, &hsv);

}

void RGB_Save()
{
}


void rgb_loop_queue_init(rgb_loop_queue_t* q, rgb_loop_queue_elm_t*data, uint16_t len)
{
    q->data = data;
    q->front = 0;
    q->rear = 0;
    q->len = len;
}

rgb_loop_queue_elm_t rgb_loop_queue_dequeue(rgb_loop_queue_t* q)
{
    rgb_argument_t a = {0,0};
    if(q->front==q->rear)
        return a;
    q->front = (q->front+1)%(q->len);
    return q->data[(q->front+q->len-1)%(q->len)];
}

void rgb_loop_queue_enqueue(rgb_loop_queue_t* q, rgb_loop_queue_elm_t t)
{
    if(((q->rear+1)%(q->len))==q->front)
        return;
    q->data[q->rear]=t;
    q->rear = (q->rear+1)%(q->len);
}
