/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef SNAKE_H
#define SNAKE_H

#include "stdint.h"
#include "stdbool.h"
#include "color.h"

#define SNAKE_ZONE_WIDTH 12
#define SNAKE_ZONE_HEIGHT 4
#define SNAKE_MAX_LENGTH (SNAKE_ZONE_WIDTH*SNAKE_ZONE_HEIGHT)

typedef struct __SnakeNode{
    int8_t x;
    int8_t y;
} SnakeNode;

enum {
    SNAKE_LEFT = 0,
    SNAKE_UP = 1,
    SNAKE_RIGHT = 2,
    SNAKE_DOWN = 3,
};

typedef struct __Snake
{
    uint8_t direction;
    uint8_t next_direction;
    ColorRGB color;
    SnakeNode apple;
    ColorRGB apple_color;
    bool failed;
    bool running;
    uint16_t interval;
    uint32_t next_tick;
    float intensity;
    SnakeNode data[SNAKE_MAX_LENGTH];
    int16_t front;
    int16_t rear;
    int16_t len;
} Snake;

extern Snake g_snake;

void snake_init(Snake *q);
SnakeNode snake_pop(Snake *q);
void snake_push(Snake *q, SnakeNode t);
bool snake_contains(Snake *q, SnakeNode t);

void draw_snake(Snake *snake);
void snake_launch(Snake *snake);
void snake_quit(Snake *snake);
void snake_pause(Snake *snake);
void snake_restart(Snake *snake);
void snake_speed_up(Snake *snake);
void snake_speed_down(Snake *snake);
void snake_move(Snake *snake);
void snake_place_apple(Snake *snake);
void snake_turn(Snake *snake, uint8_t direction);

#endif //SNAKE_H
