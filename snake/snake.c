/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "snake.h"

#include <stdlib.h>
#include <storage.h>
#include <string.h>
#include "math.h"

#include "keyboard.h"
#include "rgb.h"

static const Keycode snake_layer[ADVANCED_KEY_NUM + KEY_NUM] = {
    KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,        KEY_USER | (USER_SNAKE_QUIT << 8),
    KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_USER|(USER_SNAKE_RESTART << 8),
    KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,
    KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_TRANSPARENT,KEY_USER | (USER_SNAKE_SPEED_DOWN << 8),KEY_USER | (USER_SNAKE_UP << 8),KEY_USER | (USER_SNAKE_SPEED_UP << 8),
    KEY_USER | (USER_BEEP << 8),KEY_USER | (USER_EM << 8),KEY_USER | (USER_RESET << 8),KEY_USER|(USER_SNAKE_PAUSE << 8),KEY_TRANSPARENT,KEY_TRANSPARENT,                                                                KEY_USER | (USER_SNAKE_LEFT << 8),KEY_USER | (USER_SNAKE_DOWN << 8),KEY_USER | (USER_SNAKE_RIGHT << 8),
};

static const uint8_t snake_zone_mapping[SNAKE_ZONE_HEIGHT][SNAKE_ZONE_WIDTH] =
    {
        {50,51,52,53,54,55,56,57,58,59,60,61},
        {36,37,38,39,40,41,42,43,44,45,46,47},
        {23,24,25,26,27,28,29,30,31,32,33,34},
        {9,10,11,12,13,14,15,16,17,18,19,20},
    };

#define snake_foreach(q, type, item) for (uint16_t __index = (q)->front; __index != (q)->rear; __index = (__index + 1) % (q)->len)\
for (type *item = &((q)->data[__index]); item; item = NULL)

Snake g_snake;
void snake_init(Snake *q)
{
    srand(g_keyboard_tick);
    q->len = SNAKE_MAX_LENGTH;
    q->color = (ColorRGB){0xFF,0xFF,0xFF};
    q->apple_color =  (ColorRGB){0xFF,0,0};
    q->interval = 1000;
    q->front = 0;
    q->rear = 0;
    q->running = true;
    q->failed = false;
    q->next_direction = rand()%4;
    q->next_tick = g_keyboard_tick + q->interval;
    SnakeNode t;
    t.x = rand()%SNAKE_ZONE_WIDTH;
    t.y = rand()%SNAKE_ZONE_HEIGHT;
    snake_push(q, t);
    snake_place_apple(q);
}

SnakeNode snake_pop(Snake *q)
{
    if (q->front == q->rear)
        return (SnakeNode){0,0};
    q->front = (q->front + 1) % (q->len);
    return q->data[(q->front + q->len - 1) % (q->len)];
}

void snake_push(Snake *q, SnakeNode t)
{
    if (((q->rear + 1) % (q->len)) == q->front)
        return;
    q->data[q->rear] = t;
    q->rear = (q->rear + 1) % (q->len);
}

bool snake_contains(Snake *q, SnakeNode t)
{
    snake_foreach(q, SnakeNode, item)
    {
        if (item->x == t.x && item->y == t.y)
            return true;
    }
    return false;
}

void draw_snake(Snake *snake)
{
    snake_foreach(snake, SnakeNode, item)
    {
        color_mix(&g_rgb_colors[snake_zone_mapping[item->y][item->x]], &snake->color);
    }
    ColorRGB color = snake->apple_color;
    color.r = color.r * snake->intensity;
    color.g = color.g * snake->intensity;
    color.b = color.b * snake->intensity;
    color_mix(&g_rgb_colors[snake_zone_mapping[snake->apple.y][snake->apple.x]], &color);
    uint8_t rgb_index = 0;
    switch (snake->next_direction)
    {
    case SNAKE_LEFT:
        rgb_index = 6;
        break;
    case SNAKE_UP:
        rgb_index = 21;
        break;
    case SNAKE_RIGHT:
        rgb_index = 8;
        break;
    case SNAKE_DOWN:
        rgb_index = 7;
        break;
    default:
        break;
    }
    color_mix(&g_rgb_colors[rgb_index], &snake->color);
}

void snake_launch(Snake *snake)
{
    for (int i = 0; i < 3; i++)
    {
        memcpy(g_keymap[i], snake_layer, sizeof(snake_layer));
        layer_reset(i);
    }
    snake_init(snake);

}

void snake_quit(Snake *snake)
{
    snake->running = false;
    storage_read_config(g_current_config_index);
}

void snake_pause(Snake *snake)
{
    if (snake->next_tick != 0xFFFFFFFF)
    {
        snake->next_tick = 0xFFFFFFFF;
    }
    else
    {
        snake->next_tick = g_keyboard_tick + snake->interval;
    }
}

void snake_restart(Snake *snake)
{
    snake_init(snake);
}

void snake_speed_up(Snake *snake)
{
    if (snake->interval > 200)
    {
        snake->interval -= 100;
    }
}

void snake_speed_down(Snake *snake)
{
    if (snake->interval < 5000)
    {
        snake->interval += 100;
    }
}

void snake_move(Snake *snake)
{

    if (snake->next_tick == 0xFFFFFFFF)
    {
        return;
    }
    if (snake->failed)
    {
        return;
    }
    if (g_keyboard_tick<snake->next_tick)
    {
        snake->intensity = fabs((float)((int32_t)(snake->next_tick - g_keyboard_tick) - (int32_t)snake->interval/2))/(float)(snake->interval/2);
        snake->intensity = snake->intensity < 1.0f ? snake->intensity : 1.0f;
        snake->intensity = 1.0f - snake->intensity;
        return;
    }
    snake->next_tick = g_keyboard_tick + snake->interval;
    SnakeNode t;
    SnakeNode head = snake->data[(snake->rear+snake->len-1)%snake->len];
    snake->direction = snake->next_direction;
    switch (snake->direction)
    {
    case SNAKE_LEFT:
        t.x = head.x-1;
        t.y = head.y;
        break;
    case SNAKE_UP:
        t.x = head.x;
        t.y = head.y-1;
        break;
    case SNAKE_RIGHT:
        t.x = head.x+1;
        t.y = head.y;
        break;
    case SNAKE_DOWN:
        t.x = head.x;
        t.y = head.y+1;
        break;
    default:
        return;
    }
    if (t.x < 0)
    {
        t.x = SNAKE_ZONE_WIDTH-1;
    }
    if (t.y < 0)
    {
        t.y = SNAKE_ZONE_HEIGHT-1;
    }
    if (t.x >= SNAKE_ZONE_WIDTH)
    {
        t.x = 0;
    }
    if (t.y >= SNAKE_ZONE_HEIGHT)
    {
        t.y = 0;
    }
    if (snake_contains(snake, t))
    {
        snake->failed = true;
        return;
    }
    snake_push(snake, t);
    if (t.x == snake->apple.x && t.y == snake->apple.y)
    {
        extern uint32_t pulse_counter;
        pulse_counter = PULSE_LEN_MS;
        snake_place_apple(snake);
    }
    else
    {
        snake_pop(snake);
    }
}

void snake_place_apple(Snake *snake)
{
    SnakeNode t ;
    do
    {
        t.x = rand()%SNAKE_ZONE_WIDTH;
        t.y = rand()%SNAKE_ZONE_HEIGHT;
    } while (snake_contains(snake, t));
    snake->apple = t;
}

void snake_turn(Snake *snake, uint8_t direction)
{
    if ((2+direction)%4 != snake->direction)
    {
        snake->next_direction = direction;
    }
}
