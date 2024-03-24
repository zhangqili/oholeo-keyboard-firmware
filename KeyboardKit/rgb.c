/*
 * rgb.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "main.h"
#include "rgb.h"
#include "string.h"
#include "math.h"

#define loop_queue_foreach(q,i) for(uint16_t (i)=(q)->front;(i)!=(q)->rear;(i)=(i+1)%(q)->len)

uint8_t g_rgb_buffer[RGB_BUFFER_LENGTH];
RGBGlobalConfig g_rgb_global_config;
RGBIndividualConfig g_rgb_configs[RGB_NUM];
ColorRGB g_rgb_colors[RGB_NUM];
RGBLoopQueue g_rgb_argument_queue;
uint32_t RGB_Tick;

static RGBLoopQueueElm RGB_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];

void rgb_init()
{
    rgb_loop_queue_init(&g_rgb_argument_queue, RGB_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    for (uint16_t i = 0; i < RGB_BUFFER_LENGTH; i++)
    {
        g_rgb_buffer[i] = NONE_PULSE;
    }
}

#define COLOR_INTERVAL(key, low, up) (uint8_t)((key) < 0 ? (low) : ((key) > 1.0 ? (up) : (key) * (up)))

void rgb_update()
{
    ColorHSV temp_hsv;
    uint16_t tempint;
    float tempf;
    memcpy(&temp_hsv, &(g_rgb_global_config.hsv), sizeof(ColorHSV));
    loop_queue_foreach(&g_rgb_argument_queue, j)
    {
        if(g_rgb_global_config.speed*(RGB_Tick-g_rgb_argument_queue.data[j].begin_time)>20.0f)
        {
            rgb_loop_queue_dequeue(&g_rgb_argument_queue);
        }
    }
    switch (g_rgb_global_config.mode)
    {
        case RGB_GLOBAL_MODE_INDIVIDUAL:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                uint8_t rgb_index = g_rgb_mapping[i];
                float r = g_keyboard_advanced_keys[i].value < g_keyboard_advanced_keys[i].upper_deadzone
                              ? 0
                              : g_keyboard_advanced_keys[i].value > 1.0
                                    ? 1.0
                                    : g_keyboard_advanced_keys[i].value;

                switch (g_rgb_configs[rgb_index].mode)
                {
                    case RGB_MODE_REACT_LINEAR:
                        g_rgb_colors[rgb_index].r = COLOR_INTERVAL(r, 0, (float)(g_rgb_configs[rgb_index].rgb.r));
                        g_rgb_colors[rgb_index].g = COLOR_INTERVAL(r, 0, (float)(g_rgb_configs[rgb_index].rgb.g));
                        g_rgb_colors[rgb_index].b = COLOR_INTERVAL(r, 0, (float)(g_rgb_configs[rgb_index].rgb.b));
                        break;
                    case RGB_MODE_REACT_TRIGGER:
                        if (g_keyboard_advanced_keys[i].key.state)
                        {
                            g_rgb_configs[rgb_index].begin_time = RGB_Tick;
                        }
                        tempf = powf(1 - g_rgb_configs[rgb_index].speed, RGB_Tick - g_rgb_configs[rgb_index].begin_time);
                        g_rgb_colors[rgb_index].r = (uint8_t)((float)(g_rgb_configs[rgb_index].rgb.r) * tempf);;
                        g_rgb_colors[rgb_index].g = (uint8_t)((float)(g_rgb_configs[rgb_index].rgb.g) * tempf);;
                        g_rgb_colors[rgb_index].b = (uint8_t)((float)(g_rgb_configs[rgb_index].rgb.b) * tempf);;
                        break;
                    case RGB_MODE_STATIC:
                        g_rgb_colors[rgb_index].r = g_rgb_configs[rgb_index].rgb.r;
                        g_rgb_colors[rgb_index].g = g_rgb_configs[rgb_index].rgb.g;
                        g_rgb_colors[rgb_index].b = g_rgb_configs[rgb_index].rgb.b;
                        break;
                    case RGB_MODE_CYCLE:
                        temp_hsv.s = g_rgb_configs[i].hsv.s;
                        temp_hsv.v = g_rgb_configs[i].hsv.v;
                        temp_hsv.h = (uint8_t)(g_rgb_configs[i].hsv.h + (RGB_Tick * g_rgb_configs[i].speed)) % 360;
                        color_set_hsv(g_rgb_colors + i, &temp_hsv);
                        break;
                    default:
                        break;
                }
                g_rgb_buffer[i] = NONE_PULSE;
            }
            break;
        case RGB_GLOBAL_MODE_WAVE:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                temp_hsv.h = ((uint16_t)(((uint32_t)(g_rgb_global_config.speed * RGB_Tick)) % 360) + (uint16_t)(
                                  g_rgb_locations[i].x * 15)) % 360;
                color_set_hsv(g_rgb_colors + i, &temp_hsv);
            }
            break;
        case RGB_GLOBAL_MODE_OFF:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                g_rgb_colors[i].r = 0;
                g_rgb_colors[i].g = 0;
                g_rgb_colors[i].b = 0;
            }
            break;
        case RGB_GLOBAL_MODE_STRING:
        case RGB_GLOBAL_MODE_FADING_STRING:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                g_rgb_colors[i].r = 0;
                g_rgb_colors[i].g = 0;
                g_rgb_colors[i].b = 0;
            }
            loop_queue_foreach(&g_rgb_argument_queue, k)
            {
                for (int8_t j = 0; j < ADVANCED_KEY_NUM; j++)
                {
                    if (g_rgb_locations[g_rgb_argument_queue.data[k].rgb_ptr].row == g_rgb_locations[j].row)
                    {
                        tempf = (1.0f - fabsf(
                                     (RGB_Tick - g_rgb_argument_queue.data[k].begin_time) * g_rgb_global_config.speed -
                                     fabsf(g_rgb_locations[g_rgb_argument_queue.data[k].rgb_ptr].x - g_rgb_locations[j].x)));
                        tempf = tempf > 0 ? tempf : 0;
                        if (g_rgb_global_config.mode == RGB_GLOBAL_MODE_FADING_STRING)
                        {
                            tempf /= (RGB_Tick - g_rgb_argument_queue.data[k].begin_time) * g_rgb_global_config.speed;
                        }
                        //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                        //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                        tempint = g_rgb_colors[j].r;
                        tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[j].rgb.r)))) >> 1;
                        g_rgb_colors[j].r = tempint < 256 ? tempint : 255;
                        //printf("%d\t",tempr);

                        tempint = g_rgb_colors[j].g;
                        tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[j].rgb.g)))) >> 1;
                        g_rgb_colors[j].g = tempint < 256 ? tempint : 255;

                        tempint = g_rgb_colors[j].b;
                        tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[j].rgb.b)))) >> 1;
                        g_rgb_colors[j].b = tempint < 256 ? tempint : 255;
                    }
                }
            }
            break;
        case RGB_GLOBAL_MODE_DIAMOND_RIPPLE:
        case RGB_GLOBAL_MODE_FADING_DIAMOND_RIPPLE:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                g_rgb_colors[i].r = 0;
                g_rgb_colors[i].g = 0;
                g_rgb_colors[i].b = 0;
            }
            loop_queue_foreach(&g_rgb_argument_queue, k) {
                for (int8_t i = 0; i < ADVANCED_KEY_NUM; i++)
                {
                    tempf = (1.0f - fabsf(
                                 (RGB_Tick - g_rgb_argument_queue.data[k].begin_time) * g_rgb_global_config.speed * 2 -
                                 fabsf(g_rgb_locations[g_rgb_argument_queue.data[k].rgb_ptr].row - g_rgb_locations[i].row) -
                                 fabsf(g_rgb_locations[g_rgb_argument_queue.data[k].rgb_ptr].x - g_rgb_locations[i].x)));
                    tempf = tempf > 0 ? tempf : 0;
                    if (g_rgb_global_config.mode == RGB_GLOBAL_MODE_FADING_DIAMOND_RIPPLE)
                    {
                        tempf /= (RGB_Tick - g_rgb_argument_queue.data[k].begin_time) * g_rgb_global_config.speed;
                    }
                    //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                    //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                    tempint = g_rgb_colors[i].r;
                    tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[i].rgb.r)))) >> 1;
                    g_rgb_colors[i].r = tempint < 256 ? tempint : 255;
                    //printf("%d\t",tempr);

                    tempint = g_rgb_colors[i].g;
                    tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[i].rgb.g)))) >> 1;
                    g_rgb_colors[i].g = tempint < 256 ? tempint : 255;

                    tempint = g_rgb_colors[i].b;
                    tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[i].rgb.b)))) >> 1;
                    g_rgb_colors[i].b = tempint < 256 ? tempint : 255;
                }
            }
            break;
        case RGB_GLOBAL_MODE_JELLY:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                g_rgb_colors[i].r = 0;
                g_rgb_colors[i].g = 0;
                g_rgb_colors[i].b = 0;
            }
            for (int8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                for (int8_t j = 0; j < ADVANCED_KEY_NUM; j++)
                {
                    tempf = (10.0f * g_keyboard_advanced_keys[i].value - fabsf(
                                 g_rgb_locations[j].row - g_rgb_locations[g_rgb_mapping[i]].row) - fabsf(
                                 g_rgb_locations[j].x - g_rgb_locations[g_rgb_mapping[i]].x));
                    tempf = tempf > 0 ? tempf > 1 ? 1 : tempf : 0;
                    //printf("%f\n",1.0 - fabsf(i+RGB_Configs[i].argument-j));
                    //printf("%.2f\t",tempf*((float)(RGB_Configs[i].rgb.r)));
                    tempint = g_rgb_colors[j].r;
                    tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[j].rgb.r)))) >> 1;
                    g_rgb_colors[j].r = tempint < 256 ? tempint : 255;
                    //printf("%d\t",tempr);

                    tempint = g_rgb_colors[j].g;
                    tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[j].rgb.g)))) >> 1;
                    g_rgb_colors[j].g = tempint < 256 ? tempint : 255;

                    tempint = g_rgb_colors[j].b;
                    tempint += ((uint8_t)(tempf * ((float)(g_rgb_configs[j].rgb.b)))) >> 1;
                    g_rgb_colors[j].b = tempint < 256 ? tempint : 255;
                }
            }
            break;
        default:
            for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
            {
                g_rgb_colors[i].r = 0;
                g_rgb_colors[i].g = 0;
                g_rgb_colors[i].b = 0;
            }
            break;
    }
    extern uint8_t LED_Report;
	if(LED_Report&0x02) {
	    g_rgb_colors[g_rgb_mapping[3]] = g_rgb_configs[g_rgb_mapping[3]].rgb;//cap lock
	}
	if(g_keyboard_advanced_keys[49].key.state) {
		g_rgb_colors[g_rgb_mapping[9]] = g_rgb_configs[g_rgb_mapping[9]].rgb;//q
	}
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        rgb_set(g_rgb_colors[i].r, g_rgb_colors[i].g, g_rgb_colors[i].b, i);
    }
}

void rgb_set(uint8_t r, uint8_t g, uint8_t b, uint16_t index)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        g_rgb_buffer[RGB_RESET_LENGTH + index * 24 + i] = (g << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        g_rgb_buffer[RGB_RESET_LENGTH + index * 24 + i + 8] = (r << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        g_rgb_buffer[RGB_RESET_LENGTH + index * 24 + i + 16] = (b << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    }
}

void rgb_flash()
{
    for (uint8_t i = 1; i < 128; i++)
    {
        for (uint8_t j = 0; j < RGB_NUM; j++)
        {
            rgb_set(i, i, i, j);
        }
        HAL_Delay(2);
    }
    for (uint8_t i = 128; i > 0; i--)
    {
        for (uint8_t j = 0; j < RGB_NUM; j++)
        {
            rgb_set(i, i, i, j);
        }
        HAL_Delay(2);
    }
    rgb_turn_off();
}

void rgb_turn_off()
{
    for (uint8_t i = 0; i < RGB_NUM; i++)
    {
        rgb_set(0, 0, 0, i);
    }
}

void rgb_recovery()
{
    ColorHSV temphsv = {273, 78, 99};
    for (uint8_t i = 0; i < RGB_NUM; i++)
    {
        g_rgb_configs[i].mode = RGB_MODE_REACT_LINEAR;
        g_rgb_configs[i].hsv = temphsv;
        g_rgb_configs[i].speed = 0.015;
        color_set_hsv(&g_rgb_configs[i].rgb, &temphsv);
    }
    ColorHSV hsv = {273, 78, 99};
    // RGB_GlobalConfig.mode=RGB_GLOBAL_MODE_WAVE;
    g_rgb_global_config.mode = RGB_GLOBAL_MODE_INDIVIDUAL;
    g_rgb_global_config.speed = 0.005;
    g_rgb_global_config.hsv = hsv;
    color_set_hsv(&g_rgb_global_config.rgb, &hsv);
}

void rgb_save()
{
}

void rgb_loop_queue_init(RGBLoopQueue* q, RGBLoopQueueElm* data, uint16_t len)
{
    q->data = data;
    q->front = 0;
    q->rear = 0;
    q->len = len;
}

RGBLoopQueueElm rgb_loop_queue_dequeue(RGBLoopQueue* q)
{
    RGBArgument a = {0, 0};
    if (q->front == q->rear)
        return a;
    q->front = (q->front + 1) % (q->len);
    return q->data[(q->front + q->len - 1) % (q->len)];
}

void rgb_loop_queue_enqueue(RGBLoopQueue* q, RGBLoopQueueElm t)
{
    if (((q->rear + 1) % (q->len)) == q->front)
        return;
    q->data[q->rear] = t;
    q->rear = (q->rear + 1) % (q->len);
}
