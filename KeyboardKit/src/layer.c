/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "layer.h"
#include "keycode.h"
#include "key.h"
#include "keyboard_def.h"
#include "keyboard.h"

uint8_t g_layer_cache[ADVANCED_KEY_NUM + KEY_NUM];
uint8_t g_current_layer;
static uint16_t layer_state;
static Keycode keymap_cache[ADVANCED_KEY_NUM + KEY_NUM];

void layer_control(Keycode keycode, uint8_t event)
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
        if (BIT_GET(layer_state,i))
        {
            return i;
        }
    }
    return 0;
}

void layer_set(uint8_t layer)
{
    BIT_SET(layer_state,layer);
    g_current_layer = layer_get();
}

void layer_reset(uint8_t layer)
{
    BIT_RESET(layer_state,layer);
    g_current_layer = layer_get();
}

void layer_toggle(uint8_t layer)
{
    BIT_TOGGLE(layer_state,layer);
    g_current_layer = layer_get();
}

void layer_cache_set(uint16_t id, uint8_t layer)
{
    g_layer_cache[id] = layer;
    int8_t layer_temp = layer;
    Keycode keycode = 0;
    while (layer_temp>=0)
    {
        keycode = g_keymap[layer_temp][id];
        if (KEYCODE(keycode) == KEY_TRANSPARENT)
        {
            layer_temp--;
        }
        else
        {
            keymap_cache[id] = keycode;
            return;
        }
    }
    keymap_cache[id] = KEY_NO_EVENT;
}

uint8_t layer_cache_get(uint16_t id)
{
    return g_layer_cache[id];
}

Keycode layer_cache_get_keycode(uint16_t id)
{
    return keymap_cache[id];
}