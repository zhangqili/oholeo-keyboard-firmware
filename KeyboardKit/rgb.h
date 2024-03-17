/*
 * rgb.h
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */

#ifndef RGB_H_
#define RGB_H_

#include "keyboard.h"
#include "color.h"

#define RGB_NUM                 (ADVANCED_KEY_NUM+1)
#define ONE_PULSE               (60)
#define ZERO_PULSE              (29)
#define NONE_PULSE              (0)
#define RGB_RESET_LENGTH        (400)
#define RGB_BUFFER_LENGTH       (RGB_RESET_LENGTH+3*8*(RGB_NUM))

#define RGB_Start() HAL_TIM_PWM_Start_DMA(&htim1,TIM_CHANNEL_1,(uint32_t*)RGB_Buffer,RGB_BUFFER_LENGTH);

#define USE_RGB
//#define RGB_Start() HAL_TIMEx_PWMN_Start_DMA(&htim8,TIM_CHANNEL_3,RGB_buffer,RGB_BUFFER_LENGTH);

typedef enum __rgb_mode_t
{
    RGB_MODE_STATIC,
    RGB_MODE_CYCLE,
    RGB_MODE_REACT_LINEAR,
    RGB_MODE_REACT_TRIGGER,
} rgb_mode_t;

typedef enum __rgb_global_mode_t
{
    RGB_GLOBAL_MODE_OFF,
    RGB_GLOBAL_MODE_INDIVIDUAL,
    RGB_GLOBAL_MODE_WAVE,
    RGB_GLOBAL_MODE_STRING,
    RGB_GLOBAL_MODE_FADING_STRING,
    RGB_GLOBAL_MODE_DIAMOND_RIPPLE,
    RGB_GLOBAL_MODE_FADING_DIAMOND_RIPPLE,
    RGB_GLOBAL_MODE_JELLY,

} rgb_global_mode_t;

typedef struct __rgb_individual_config_t
{
    rgb_mode_t mode;
    ColorRGB rgb;
    ColorHSV hsv;
    float speed;
    uint32_t begin_time;
} rgb_individual_config_t;

typedef struct __rgb_global_config_t
{
    rgb_global_mode_t mode;
    ColorRGB rgb;
    ColorHSV hsv;
    float speed;
    uint32_t begin_time;
} rgb_global_config_t;

typedef struct __rgb_location_t
{
    uint8_t row;
    float x;
}rgb_location_t;

typedef struct __rgb_argument_t
{
    uint8_t rgb_ptr;
    uint32_t begin_time;
}rgb_argument_t;



typedef rgb_argument_t rgb_loop_queue_elm_t;

typedef struct __rgb_loop_queue_t
{
    rgb_loop_queue_elm_t *data;
    int16_t front;
    int16_t rear;
    int16_t len;
} rgb_loop_queue_t;

void rgb_loop_queue_init(rgb_loop_queue_t* q, rgb_loop_queue_elm_t*data, uint16_t len);
rgb_loop_queue_elm_t rgb_loop_queue_dequeue(rgb_loop_queue_t* q);
void rgb_loop_queue_enqueue(rgb_loop_queue_t* q, rgb_loop_queue_elm_t t);
//#define lefl_loop_queue_foreach(q,i) for(uint16_t (i)=(q)->front;(i)!=(q)->rear;(i)=(i+1)%(q)->len)

#define ARGUMENT_BUFFER_LENGTH 64
#ifdef USE_RGB
extern uint8_t RGB_Buffer[RGB_BUFFER_LENGTH];
#endif
extern ColorRGB RGB_Colors[RGB_NUM];
extern rgb_individual_config_t RGB_Configs[RGB_NUM];
extern rgb_global_config_t RGB_GlobalConfig;
extern uint8_t RGB_TargetConfig;
extern uint32_t RGB_Tick;
extern rgb_loop_queue_t RGB_Argument_Queue;
extern const uint8_t RGB_Mapping[ADVANCED_KEY_NUM];

#ifdef USE_RGB
void RGB_Init();
void RGB_Update();
void RGB_Set(uint8_t r,uint8_t g,uint8_t b,uint16_t index);
void RGB_Flash();
void RGB_TurnOff();
#endif
void RGB_Recovery();
void RGB_Save();

#endif /* RGB_H_ */
