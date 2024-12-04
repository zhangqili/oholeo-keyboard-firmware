/*
 * rgb.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "rgb.h"
#include "string.h"
#include "math.h"

#define loop_queue_foreach(q, i) for (uint16_t(i) = (q)->front; (i) != (q)->rear; (i) = (i + 1) % (q)->len)

#define MANHATTAN_DISTANCE(m, n) (fabsf((m)->x - (n)->x) + fabsf((m)->y - (n)->y))

__WEAK const uint8_t g_rgb_mapping[ADVANCED_KEY_NUM];
__WEAK const RGBLocation g_rgb_locations[RGB_NUM];

uint8_t g_rgb_buffer[RGB_BUFFER_LENGTH];
RGBConfig g_rgb_configs[RGB_NUM];
ColorRGB g_rgb_colors[RGB_NUM];
RGBLoopQueue g_rgb_argument_queue;
bool g_rgb_switch = true;
uint32_t RGB_Tick;

static RGBLoopQueueElm RGB_Argument_Buffer[ARGUMENT_BUFFER_LENGTH];

void rgb_init()
{
    rgb_loop_queue_init(&g_rgb_argument_queue, RGB_Argument_Buffer, ARGUMENT_BUFFER_LENGTH);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        key_attach(&g_keyboard_advanced_keys[i].key, KEY_EVENT_DOWN, rgb_activate);
    }
    for (uint16_t i = 0; i < RGB_BUFFER_LENGTH; i++)
    {
        g_rgb_buffer[i] = NONE_PULSE;
    }
}

#define COLOR_INTERVAL(key, low, up) (uint8_t)((key) < 0 ? (low) : ((key) > 1.0 ? (up) : (key) * (up)))

void rgb_update()
{
    if (!g_rgb_switch)
    {
        rgb_turn_off();
        return;
    }
    ColorHSV temp_hsv;
    ColorRGB temp_rgb;
    float intensity;
    memset(g_rgb_colors, 0, sizeof(g_rgb_colors));
    loop_queue_foreach(&g_rgb_argument_queue, i)
    {
        RGBConfig *config = g_rgb_configs + g_rgb_argument_queue.data[i].rgb_ptr;
        RGBLocation *location = (RGBLocation *)&g_rgb_locations[g_rgb_argument_queue.data[i].rgb_ptr];
        uint32_t duration = RGB_Tick - g_rgb_argument_queue.data[i].begin_time;
        float distance = duration * config->speed;
        if (duration > RGB_MAX_DURATION)
        {
            rgb_loop_queue_pop(&g_rgb_argument_queue);
        }
        switch (config->mode)
        {
        case RGB_MODE_FIXED:
            break;
        case RGB_MODE_STATIC:
            break;
        case RGB_MODE_CYCLE:
            break;
        case RGB_MODE_LINEAR:
            break;
        case RGB_MODE_TRIGGER:
            break;
        case RGB_MODE_STRING:
        case RGB_MODE_FADING_STRING:
        case RGB_MODE_DIAMOND_RIPPLE:
        case RGB_MODE_FADING_DIAMOND_RIPPLE:
            for (int8_t j = 0; j < RGB_NUM; j++)
            {
                switch (config->mode)
                {
                case RGB_MODE_STRING:
                    intensity = (1.0f - fabsf(distance - fabsf(location->x - g_rgb_locations[j].x)));
                    intensity = intensity > 0 ? intensity : 0;
                    break;
                case RGB_MODE_FADING_STRING:
                    intensity = (distance - fabsf(location->x - g_rgb_locations[j].x));
                    if (intensity > 0)
                    {
                        intensity = FADING_DISTANCE - intensity > 0 ? FADING_DISTANCE - intensity : 0;
                        intensity /= FADING_DISTANCE;
                        break;
                    }
                    else
                    {
                        intensity = 1.0f + intensity > 0 ? 1.0f + intensity : 0;
                    }
                    break;
                case RGB_MODE_DIAMOND_RIPPLE:
                    intensity = (1.0f - fabsf(distance - MANHATTAN_DISTANCE(location, g_rgb_locations + j)));
                    intensity = intensity > 0 ? intensity : 0;
                    break;
                case RGB_MODE_FADING_DIAMOND_RIPPLE:
                    intensity = (distance - MANHATTAN_DISTANCE(location, g_rgb_locations + j));
                    if (intensity > 0)
                    {
                        intensity = FADING_DISTANCE - intensity > 0 ? FADING_DISTANCE - intensity : 0;
                        intensity /= FADING_DISTANCE;
                        break;
                    }
                    else
                    {
                        intensity = 1.0f + intensity > 0 ? 1.0f + intensity : 0;
                    }
                    break;
                default:
                    intensity = 0;
                    break;
                }
                temp_rgb.r = ((uint8_t)(intensity * ((float)(g_rgb_configs[j].rgb.r)))) >> 1;
                temp_rgb.g = ((uint8_t)(intensity * ((float)(g_rgb_configs[j].rgb.g)))) >> 1;
                temp_rgb.b = ((uint8_t)(intensity * ((float)(g_rgb_configs[j].rgb.b)))) >> 1;
                color_mix(&g_rgb_colors[j], &temp_rgb);
            }
            break;
        case RGB_MODE_JELLY:
            break;
        default:
            break;
        }
    }
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        uint8_t rgb_index = g_rgb_mapping[g_keyboard_advanced_keys[i].key.id];
        intensity = g_keyboard_advanced_keys[i].value < g_keyboard_advanced_keys[i].upper_deadzone
                        ? 0
                    : g_keyboard_advanced_keys[i].value > 1.0
                        ? 1.0
                        : g_keyboard_advanced_keys[i].value;
        switch (g_rgb_configs[rgb_index].mode)
        {
        case RGB_MODE_LINEAR:
            temp_rgb.r = COLOR_INTERVAL(intensity, 0, (float)(g_rgb_configs[rgb_index].rgb.r));
            temp_rgb.g = COLOR_INTERVAL(intensity, 0, (float)(g_rgb_configs[rgb_index].rgb.g));
            temp_rgb.b = COLOR_INTERVAL(intensity, 0, (float)(g_rgb_configs[rgb_index].rgb.b));
            color_mix(&g_rgb_colors[rgb_index], &temp_rgb);
            break;
        case RGB_MODE_TRIGGER:
            if (g_keyboard_advanced_keys[i].key.state)
            {
                g_rgb_configs[rgb_index].begin_time = RGB_Tick;
            }
            intensity = powf(1 - g_rgb_configs[rgb_index].speed, RGB_Tick - g_rgb_configs[rgb_index].begin_time);
            temp_rgb.r = (uint8_t)((float)(g_rgb_configs[rgb_index].rgb.r) * intensity);
            temp_rgb.g = (uint8_t)((float)(g_rgb_configs[rgb_index].rgb.g) * intensity);
            temp_rgb.b = (uint8_t)((float)(g_rgb_configs[rgb_index].rgb.b) * intensity);
            color_mix(&g_rgb_colors[rgb_index], &temp_rgb);
            break;
        case RGB_MODE_FIXED:
            color_set_rgb(&g_rgb_colors[rgb_index], &g_rgb_configs[rgb_index].rgb);
            break;
        case RGB_MODE_STATIC:
            color_mix(&g_rgb_colors[rgb_index], &g_rgb_configs[rgb_index].rgb);
            break;
        case RGB_MODE_CYCLE:
            temp_hsv.s = g_rgb_configs[i].hsv.s;
            temp_hsv.v = g_rgb_configs[i].hsv.v;
            temp_hsv.h = (uint16_t)(g_rgb_configs[i].hsv.h + (RGB_Tick % (uint16_t)(360 / g_rgb_configs[i].speed)) * g_rgb_configs[i].speed) % 360;
            color_set_hsv(&temp_rgb, &temp_hsv);
            color_mix(&g_rgb_colors[rgb_index], &temp_rgb);
            break;
        case RGB_MODE_JELLY:
            for (int8_t j = 0; j < ADVANCED_KEY_NUM; j++)
            {
                intensity = (JELLY_DISTANCE * g_keyboard_advanced_keys[i].value - MANHATTAN_DISTANCE(&g_rgb_locations[j], &g_rgb_locations[rgb_index]));
                intensity = intensity > 0 ? intensity > 1 ? 1 : intensity : 0;

                temp_rgb.r = ((uint8_t)(intensity * ((float)(g_rgb_configs[j].rgb.r)))) >> 1;
                temp_rgb.g = ((uint8_t)(intensity * ((float)(g_rgb_configs[j].rgb.g)))) >> 1;
                temp_rgb.b = ((uint8_t)(intensity * ((float)(g_rgb_configs[j].rgb.b)))) >> 1;
                color_mix(&g_rgb_colors[j], &temp_rgb);
            }
            break;
        default:
            break;
        }
    }
    extern uint8_t LED_Report;
	if(LED_Report&0x02)
    {
	    g_rgb_colors[g_rgb_mapping[28]].r = 0xff;
	    g_rgb_colors[g_rgb_mapping[28]].g = 0xff;
	    g_rgb_colors[g_rgb_mapping[28]].b = 0xff;//cap lock
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

void rgb_init_flash()
{
    float intensity;
    ColorRGB temp_rgb;
    uint32_t begin_time = RGB_Tick;
    RGBLocation location = PORT_LOCATION;
    bool animation_playing = false;
    while (RGB_Tick - begin_time < RGB_FLASH_MAX_DURATION)
    {
        float distance = (RGB_Tick - begin_time) * RGB_FLASH_RIPPLE_SPEED;
        memset(g_rgb_colors, 0, sizeof(g_rgb_colors));
        animation_playing = false;
        for (int8_t i = 0; i < RGB_NUM; i++)
        {
            //rgb_flash();
            intensity = (distance - sqrtf((location.x - g_rgb_locations[i].x) * (location.x - g_rgb_locations[i].x) +
                                          (location.y - g_rgb_locations[i].y) * (location.y - g_rgb_locations[i].y)));
                                          
            //intensity = (distance - MANHATTAN_DISTANCE(&location, g_rgb_locations+i));
            if (intensity > 0)
            {
                intensity = 10 - intensity > 0 ? 10 - intensity : 0;
                intensity /= 10;
            }
            else
            {
                intensity = 1.0f + intensity > 0 ? 1.0f + intensity : 0;
            }
            if (intensity > 0 || distance < 4)
            {
                animation_playing = true;
            }
            
            temp_rgb.r = ((uint8_t)(intensity * 255));
            temp_rgb.g = ((uint8_t)(intensity * 255));
            temp_rgb.b = ((uint8_t)(intensity * 255));
            color_mix(&g_rgb_colors[i], &temp_rgb);
        }
        if (!animation_playing)
        {
            break;
        }
        for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            rgb_set(g_rgb_colors[i].r, g_rgb_colors[i].g, g_rgb_colors[i].b, i);
        }
        //keyboard_delay(1);
    }
    rgb_turn_off();
}

void rgb_flash()
{
    float intensity;
    ColorRGB temp_rgb;
    uint32_t begin_time = RGB_Tick;
    while (RGB_Tick - begin_time < RGB_FLASH_MAX_DURATION)
    {
        float distance = (RGB_Tick - begin_time);
        memset(g_rgb_colors, 0, sizeof(g_rgb_colors));
        intensity = (RGB_FLASH_MAX_DURATION/2 - fabs(distance - (RGB_FLASH_MAX_DURATION/2)))/((float)(RGB_FLASH_MAX_DURATION/2));
        for (int8_t i = 0; i < RGB_NUM; i++)
        {
            temp_rgb.r = ((uint8_t)(intensity * 255));
            temp_rgb.g = ((uint8_t)(intensity * 255));
            temp_rgb.b = ((uint8_t)(intensity * 255));
            color_mix(&g_rgb_colors[i], &temp_rgb);
        }
        for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            rgb_set(g_rgb_colors[i].r, g_rgb_colors[i].g, g_rgb_colors[i].b, i);
        }
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

void rgb_factory_reset()
{
    ColorHSV temphsv = RGB_DEFAULT_COLOR_HSV;
    for (uint8_t i = 0; i < RGB_NUM; i++)
    {
        g_rgb_configs[i].mode = RGB_DEFAULT_MODE;
        g_rgb_configs[i].hsv = temphsv;
        g_rgb_configs[i].speed = RGB_DEFAULT_SPEED;
        color_set_hsv(&g_rgb_configs[i].rgb, &temphsv);
    }
}

void rgb_enqueue()
{
}

void rgb_save()
{
}

void rgb_activate(void *key)
{
    RGBArgument a;
    a.rgb_ptr = g_rgb_mapping[((Key *)key)->id];
    a.begin_time = RGB_Tick;
    switch (g_rgb_configs[g_rgb_mapping[((Key *)key)->id]].mode)
    {
    case RGB_MODE_STRING:
    case RGB_MODE_FADING_STRING:
    case RGB_MODE_DIAMOND_RIPPLE:
    case RGB_MODE_FADING_DIAMOND_RIPPLE:
        rgb_loop_queue_push(&g_rgb_argument_queue, a);
        break;
    default:
        break;
    }
}

void rgb_loop_queue_init(RGBLoopQueue *q, RGBLoopQueueElm *data, uint16_t len)
{
    q->data = data;
    q->front = 0;
    q->rear = 0;
    q->len = len;
}

RGBLoopQueueElm rgb_loop_queue_pop(RGBLoopQueue *q)
{
    RGBArgument a = {0, 0};
    if (q->front == q->rear)
        return a;
    q->front = (q->front + 1) % (q->len);
    return q->data[(q->front + q->len - 1) % (q->len)];
}

void rgb_loop_queue_push(RGBLoopQueue *q, RGBLoopQueueElm t)
{
    if (((q->rear + 1) % (q->len)) == q->front)
        return;
    q->data[q->rear] = t;
    q->rear = (q->rear + 1) % (q->len);
}
