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

#define RGB_NUM                 (ADVANCED_KEY_NUM)
#define ONE_PULSE               (60)
#define ZERO_PULSE              (29)
#define NONE_PULSE              (0)
#define RGB_RESET_LENGTH        (400)
#define RGB_BUFFER_LENGTH       (RGB_RESET_LENGTH+3*8*(RGB_NUM))

#define RGB_MAX_DURATION 2000
#define FADING_DISTANCE 5.0f
#define JELLY_DISTANCE 10.0f
#define PORT_LOCATION {1, 4.5}
#define RGB_FLASH_MAX_DURATION 1000
#define RGB_FLASH_RIPPLE_SPEED 0.03

#define RGB_DEFAULT_MODE RGB_MODE_LINEAR
#define RGB_DEFAULT_SPEED 0.03
#define RGB_DEFAULT_COLOR_HSV {273, 78, 99}

typedef enum __RGBMode
{
    RGB_MODE_FIXED,
    RGB_MODE_STATIC,
    RGB_MODE_CYCLE,
    RGB_MODE_LINEAR,
    RGB_MODE_TRIGGER,
    RGB_MODE_STRING,
    RGB_MODE_FADING_STRING,
    RGB_MODE_DIAMOND_RIPPLE,
    RGB_MODE_FADING_DIAMOND_RIPPLE,
    RGB_MODE_JELLY,
} RGBMode;

typedef struct __RGBConfig
{
    RGBMode mode;
    ColorRGB rgb;
    ColorHSV hsv;
    float speed;
    uint32_t begin_time;
} RGBConfig;

typedef struct __RGBLocation
{
    float x;
    float y;
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
RGBLoopQueueElm rgb_loop_queue_pop(RGBLoopQueue* q);
void rgb_loop_queue_push(RGBLoopQueue* q, RGBLoopQueueElm t);
extern uint32_t RGB_Tick;
//#define loop_queue_foreach(q,i) for(uint16_t (i)=(q)->front;(i)!=(q)->rear;(i)=(i+1)%(q)->len)

#define ARGUMENT_BUFFER_LENGTH 64
extern uint8_t g_rgb_buffer[RGB_BUFFER_LENGTH];
extern ColorRGB g_rgb_colors[RGB_NUM];
extern RGBConfig g_rgb_configs[RGB_NUM];
extern RGBLoopQueue g_rgb_argument_queue;
extern const uint8_t g_rgb_mapping[ADVANCED_KEY_NUM];
extern const RGBLocation g_rgb_locations[RGB_NUM];
extern bool g_rgb_switch;

void rgb_init();
void rgb_update();
void rgb_set(uint8_t r,uint8_t g,uint8_t b,uint16_t index);
void rgb_init_flash();
void rgb_flash();
void rgb_turn_off();
void rgb_factory_reset();
void rgb_save();
void rgb_activate(void * key);

#endif /* RGB_H_ */
