/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "command.h"
#include "keyboard.h"
#include "keyboard_def.h"
#include "rgb.h"
#include "string.h"

__WEAK const uint8_t command_advanced_key_mapping[ADVANCED_KEY_NUM];
// static const uint8_t command_rgb_mapping[] = {0, 1, 2, 3};

// static inline float fill_in_float(uint8_t *buf) __attribute__((optimize("O0")));
static inline float fill_in_float(uint8_t *buf)
{
    float value;
    // uint8_t *p = &value;
    // p[0] = buf[0];
    // p[1] = buf[1];
    // p[2] = buf[2];
    // p[3] = buf[3];
    memcpy(&value, buf, 4);
    return value;
}

void unload_cargo(uint8_t *buf)
{
    // max 62 remain
    switch (buf[0])
    {
    case 0: // Advanced Key
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].mode = buf[2];
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].activation_value = fill_in_float(&buf[3 + 4 * 0]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].deactivation_value = fill_in_float(&buf[3 + 4 * 1]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].trigger_distance = fill_in_float(&buf[3 + 4 * 2]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].release_distance = fill_in_float(&buf[3 + 4 * 3]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].trigger_speed = fill_in_float(&buf[3 + 4 * 4]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].release_speed = fill_in_float(&buf[3 + 4 * 5]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].upper_deadzone = fill_in_float(&buf[3 + 4 * 6]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].lower_deadzone = fill_in_float(&buf[3 + 4 * 7]);
        //g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].upper_bound = fill_in_float(&buf[2 + 4 * 8]);
        //g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].lower_bound = fill_in_float(&buf[2 + 4 * 9]);
        break;
    case 1: // Global LED
        g_rgb_switch = buf[1];
        break;
    case 2: // LED
        for (uint8_t i = 0; i < 7; i++)
        {
            if (buf[1]+i<RGB_NUM)
            {
                g_rgb_configs[g_rgb_mapping[buf[1]+i]].mode  = buf[2 + 8 * i + 0];
                g_rgb_configs[g_rgb_mapping[buf[1]+i]].rgb.r = buf[2 + 8 * i + 1];
                g_rgb_configs[g_rgb_mapping[buf[1]+i]].rgb.g = buf[2 + 8 * i + 2];
                g_rgb_configs[g_rgb_mapping[buf[1]+i]].rgb.b = buf[2 + 8 * i + 3];
                g_rgb_configs[g_rgb_mapping[buf[1]+i]].speed = fill_in_float(&buf[2 + 8 * i + 4]);
                rgb_to_hsv(&g_rgb_configs[g_rgb_mapping[buf[1]+i]].hsv, &g_rgb_configs[g_rgb_mapping[buf[1]+i]].rgb);
            }
            
        }
        break;
#define LAYER_PAGE_LENGTH 16
#define LAYER_PAGE_EXPECTED_NUM ((ADVANCED_KEY_NUM + KEY_NUM + 15) / 16)
    case 3: // Keymap
        uint8_t layer_index = buf[1];
        uint8_t layer_page_index = buf[2];
        if (layer_index < LAYER_NUM && layer_page_index < LAYER_PAGE_EXPECTED_NUM)
        {
            uint8_t size = ((ADVANCED_KEY_NUM + KEY_NUM)-layer_page_index*LAYER_PAGE_LENGTH)*sizeof(uint16_t);
            size = size > LAYER_PAGE_LENGTH*sizeof(uint16_t) ? LAYER_PAGE_LENGTH*sizeof(uint16_t) : size;
            memcpy(&g_keymap[layer_index][layer_page_index*16], buf + 3, size);
        }
        break;
    default:
        break;
    }
}
void command_prase(uint8_t *buf, uint8_t len)
{
    UNUSED(len);
    switch (buf[0])
    {
    case 0x80: // Save
        keyboard_save();
        break;
    case 0x81: // System Reset
        keyboard_system_reset();
        break;
    case 0x82: // Factory Reset
        keyboard_factory_reset();
        break;
    case 0xB0: // Set Debug
        g_debug_enable = buf[1];
        break;
    case 0xFF: // Legacy
        unload_cargo(buf + 1);
        break;
    default:
        break;
    }
}
