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


typedef enum __RGBMode
{
    RGB_MODE_STATIC,
    RGB_MODE_CYCLE,
    RGB_MODE_REACT_LINEAR,
    RGB_MODE_REACT_TRIGGER,
} RGBMode;

typedef enum __RGBGlobalMode
{
    RGB_GLOBAL_MODE_OFF,
    RGB_GLOBAL_MODE_INDIVIDUAL,
    RGB_GLOBAL_MODE_WAVE,
    RGB_GLOBAL_MODE_STRING,
    RGB_GLOBAL_MODE_FADING_STRING,
    RGB_GLOBAL_MODE_DIAMOND_RIPPLE,
    RGB_GLOBAL_MODE_FADING_DIAMOND_RIPPLE,
    RGB_GLOBAL_MODE_JELLY,

} RGBGlobalMode;

typedef struct __RGBIndividualConfig
{
    RGBMode mode;
    ColorRGB rgb;
    ColorHSV hsv;
    float speed;
    uint32_t begin_time;
} RGBIndividualConfig;

typedef struct __RGBGlobalConfig
{
    RGBGlobalMode mode;
    ColorRGB rgb;
    ColorHSV hsv;
    float speed;
    uint32_t begin_time;
} RGBGlobalConfig;

typedef struct __RGBLocation
{
    uint8_t row;
    float x;
}RGBLocation;

typedef struct __RGBArgument
{
    uint8_t rgb_ptr;
    uint32_t begin_time;
}RGBArgument;



typedef RGBArgument RGBLoopQueueElm;

typedef struct __RGBLoopQueue
{
    RGBLoopQueueElm *data;
    int16_t front;
    int16_t rear;
    int16_t len;
} RGBLoopQueue;

void rgb_loop_queue_init(RGBLoopQueue* q, RGBLoopQueueElm*data, uint16_t len);
RGBLoopQueueElm rgb_loop_queue_dequeue(RGBLoopQueue* q);
void rgb_loop_queue_enqueue(RGBLoopQueue* q, RGBLoopQueueElm t);
extern uint32_t RGB_Tick;
//#define loop_queue_foreach(q,i) for(uint16_t (i)=(q)->front;(i)!=(q)->rear;(i)=(i+1)%(q)->len)

#define ARGUMENT_BUFFER_LENGTH 16
extern uint8_t g_rgb_buffer[RGB_BUFFER_LENGTH];
extern ColorRGB g_rgb_colors[RGB_NUM];
extern RGBIndividualConfig g_rgb_configs[RGB_NUM];
extern RGBGlobalConfig g_rgb_global_config;
extern RGBLoopQueue g_rgb_argument_queue;
extern const uint8_t g_rgb_mapping[ADVANCED_KEY_NUM];
extern const RGBLocation g_rgb_locations[RGB_NUM];

void rgb_init();
void rgb_update();
void rgb_set(uint8_t r,uint8_t g,uint8_t b,uint16_t index);
void rgb_flash();
void rgb_turn_off();
void rgb_recovery();
void rgb_save();


#endif /* RGB_H_ */
