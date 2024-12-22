/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef COLOR_H_
#define COLOR_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

typedef struct __ColorRGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
} ColorRGB;

typedef ColorRGB Color;

typedef struct __ColorHSV
{
    uint16_t h;
    uint8_t s;
    uint8_t v;
} ColorHSV;

void rgb_to_hsv(ColorHSV *hsv, ColorRGB *rgb);
void hsv_to_rgb(ColorRGB *rgb, ColorHSV *hsv);
void color_get_rgb(Color *color, ColorRGB *rgb);
void color_set_rgb(Color *color, ColorRGB *rgb);
void color_get_hsv(Color *color, ColorHSV *hsv);
void color_set_hsv(Color *color, ColorHSV *hsv);
void color_mix(Color *dest, Color *source);
#endif