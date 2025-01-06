/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "layer.h"
#include "keycode.h"
#include "key.h"

uint8_t g_layer_cache[ADVANCED_KEY_NUM + KEY_NUM];
uint8_t g_current_layer;
static uint16_t layer_state;

void layer_control(uint16_t keycode, uint8_t event)
{
    uint8_t layer = (keycode >> 8 & 0x0F);
    if (event == KEY_EVENT_DOWN)
    {
        switch ((keycode >> 12) & 0x0F)
        {
        case LAYER_MOMENTARY:
            layer_toggle(layer);
            break;
        case LAYER_TURN_ON:
            layer_set(layer);
            break;
        case LAYER_TURN_OFF:
            layer_reset(layer);
            break;
        case LAYER_TOGGLE:
            layer_toggle(layer);
            break;
        default:
            break;
        }
    }
    else if (event == KEY_EVENT_UP)
    {
        switch ((keycode >> 12) & 0x0F)
        {
        case LAYER_MOMENTARY:
            layer_toggle(layer);
            break;
        default:
            break;
        }
    }
    
    
}

uint8_t layer_get(void)
{
    for (int i = 15; i > 0; i--)
    {
        if (layer_state & (1 << i))
        {
            return i;
        }
    }
    return 0;
}

void layer_set(uint8_t layer)
{
    layer_state |= 1 << layer;
    g_current_layer = layer_get();
}

void layer_reset(uint8_t layer)
{
    layer_state &= (~(1 << layer));
    g_current_layer = layer_get();
}

void layer_toggle(uint8_t layer)
{
    layer_state ^= 1 << layer;
    g_current_layer = layer_get();
}

void layer_cache_set(uint16_t id, uint8_t layer)
{
    g_layer_cache[id] = layer;
}

uint8_t layer_cache_get(uint16_t id)
{
    return g_layer_cache[id];
}