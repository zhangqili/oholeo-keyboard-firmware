/*
 * rgb.h
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */

#ifndef RGB_H_
#define RGB_H_

#include "lefl.h"
#include "keyboard.h"

#define RGB_NUM                 (ADVANCED_KEY_NUM+60+1)
#define ONE_PULSE               (61)
#define ZERO_PULSE              (29)
#define NONE_PULSE              (90)
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
    RGB_GLOBAL_MODE_RIPPLE,

} rgb_global_mode_t;

typedef struct __rgb_individual_config_t
{
    rgb_mode_t mode;
    lefl_color_rgb_t rgb;
    lefl_color_hsv_t hsv;
    float speed;
    float argument;
} rgb_individual_config_t;

typedef struct __rgb_global_config_t
{
    rgb_global_mode_t mode;
    lefl_color_rgb_t rgb;
    lefl_color_hsv_t hsv;
    float speed;
    float argument;
} rgb_global_config_t;


#ifdef USE_RGB
extern uint8_t RGB_Buffer[RGB_BUFFER_LENGTH];
#endif
extern lefl_color_rgb_t RGB_Colors[RGB_NUM];
extern rgb_individual_config_t RGB_Configs[RGB_NUM];
extern rgb_global_config_t RGB_GlobalConfig;
extern uint8_t RGB_TargetConfig;
extern lefl_loop_queue_t RGB_Argument_Queues[RGB_NUM];

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
