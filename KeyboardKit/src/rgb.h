/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef RGB_H_
#define RGB_H_

#include "color.h"
#include "keyboard.h"
#include "keyboard_conf.h"

#ifndef RGB_NUM
#define RGB_NUM                 (ADVANCED_KEY_NUM)
#endif

#ifndef ONE_PULSE
#define ONE_PULSE               (7)
#endif

#ifndef ZERO_PULSE
#define ZERO_PULSE              (3)
#endif

#ifndef NONE_PULSE
#define NONE_PULSE              (0)
#endif

#ifndef RGB_RESET_LENGTH
#define RGB_RESET_LENGTH        (60)
#endif

#ifndef RGB_BUFFER_LENGTH
#define RGB_BUFFER_LENGTH       (((RGB_NUM)*(3*8))+RGB_RESET_LENGTH)
#endif

#ifndef RGB_MAX_DURATION
#define RGB_MAX_DURATION 1000
#endif

#ifndef FADING_DISTANCE
#define FADING_DISTANCE 5.0f
#endif

#ifndef JELLY_DISTANCE
#define JELLY_DISTANCE 10.0f
#endif

#ifndef PORT_LOCATION
#define PORT_LOCATION {0, 0}
#endif

#ifndef RGB_FLASH_MAX_DURATION
#define RGB_FLASH_MAX_DURATION 1000
#endif

#ifndef RGB_FLASH_RIPPLE_SPEED
#define RGB_FLASH_RIPPLE_SPEED 0.03
#endif

#ifndef RGB_DEFAULT_MODE
#define RGB_DEFAULT_MODE RGB_MODE_LINEAR
#endif

#ifndef RGB_DEFAULT_SPEED
#define RGB_DEFAULT_SPEED 0.015
#endif

#ifndef RGB_DEFAULT_COLOR_HSV
#define RGB_DEFAULT_COLOR_HSV {273, 78, 99}
#endif

#ifndef RGB_LEFT
#define RGB_LEFT 0.0f
#endif

#ifndef RGB_TOP
#define RGB_TOP 4.5f
#endif

#ifndef RGB_RIGHT
#define RGB_RIGHT 15.0f
#endif

#ifndef RGB_BOTTOM
#define RGB_BOTTOM -0.5f
#endif

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
    uint32_t begin_time;
    uint8_t rgb_ptr;
}RGBArgument;

typedef RGBArgument RGBLoopQueueElm;

typedef struct __RGBArgumentListNode
{
    RGBArgument data;
    int16_t next;
} RGBArgumentListNode;

typedef struct __RGBArgumentList
{
    RGBArgumentListNode *data;
    int16_t head;
    int16_t tail;
    int16_t len;
    int16_t free_node;
} RGBArgumentList;

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

void rgb_forward_list_init(RGBArgumentList* list, RGBArgumentListNode*data, uint16_t len);
void rgb_forward_list_erase_after(RGBArgumentList* list, RGBArgumentListNode*data);
void rgb_forward_list_push(RGBArgumentList* list, RGBArgument t);

#define ARGUMENT_BUFFER_LENGTH 64

extern uint8_t g_rgb_buffer[RGB_BUFFER_LENGTH];
extern ColorRGB g_rgb_colors[RGB_NUM];
extern RGBConfig g_rgb_configs[RGB_NUM];

extern const uint8_t g_rgb_mapping[ADVANCED_KEY_NUM];
extern const RGBLocation g_rgb_locations[RGB_NUM];
extern bool g_rgb_switch;

void rgb_init(void);
void rgb_update(void);
void rgb_update_callback(void);
void rgb_set(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void rgb_init_flash(void);
void rgb_flash(void);
void rgb_turn_off(void);
void rgb_factory_reset(void);
void rgb_save(void);
void rgb_activate(uint16_t id);

#endif /* RGB_H_ */
