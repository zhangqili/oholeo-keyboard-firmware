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
        uint8_t key_index = buf[1];
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].mode = buf[2];
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].activation_value = fill_in_float(&buf[3 + 4 * 0]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].deactivation_value = fill_in_float(&buf[3 + 4 * 1]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].trigger_distance = fill_in_float(&buf[3 + 4 * 2]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].release_distance = fill_in_float(&buf[3 + 4 * 3]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].trigger_speed = fill_in_float(&buf[3 + 4 * 4]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].release_speed = fill_in_float(&buf[3 + 4 * 5]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].upper_deadzone = fill_in_float(&buf[3 + 4 * 6]);
        g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].lower_deadzone = fill_in_float(&buf[3 + 4 * 7]);
        //g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].upper_bound = fill_in_float(&buf[2 + 4 * 8]);
        //g_keyboard_advanced_keys[command_advanced_key_mapping[buf[1]]].lower_bound = fill_in_float(&buf[2 + 4 * 9]);
        break;
    case 1: // Global LED
        g_rgb_switch = buf[1];
        break;
    case 2: // LED
        for (uint8_t i = 0; i < 6; i++)
        {
            uint8_t key_index = buf[1 + 9 * i];
            if (key_index < RGB_NUM)
            {
                g_rgb_configs[g_rgb_mapping[key_index]].mode = buf[1 + 9 * i + 1];
                g_rgb_configs[g_rgb_mapping[key_index]].rgb.r = buf[1 + 9 * i + 2];
                g_rgb_configs[g_rgb_mapping[key_index]].rgb.g = buf[1 + 9 * i + 3];
                g_rgb_configs[g_rgb_mapping[key_index]].rgb.b = buf[1 + 9 * i + 4];
                rgb_to_hsv(&g_rgb_configs[key_index].hsv, &g_rgb_configs[key_index].rgb);
                memcpy(&buf[1 + 9 * i + 5], &g_rgb_configs[g_rgb_mapping[key_index]].speed, sizeof(float));
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
            memcpy(&g_keymap[layer_index][layer_page_index*LAYER_PAGE_LENGTH], buf + 3, size);
        }
        break;
    case 4: // Dynamic Key
        uint8_t dk_index = buf[1];
        if (dk_index<DYNAMIC_KEY_NUM)
        {
            memcpy(&g_keyboard_dynamic_keys[dk_index], buf + 2, sizeof(DynamicKey));
        }
        break;
    default:
        break;
    }
}

static uint16_t page_index;
void start_load_cargo(void)
{
    page_index = 0;
}

int load_cargo(void)
{
    uint8_t buf[64] = {0};
    buf[0] = 0xFF;
    // max 62 remain
    switch ((page_index >> 8) & 0xFF)
    {
    case 0: // Advanced Key
        buf[1] = 0x00;
        uint8_t key_index = page_index & 0xFF;
        buf[2] = g_keyboard_advanced_keys[key_index].key.id & 0xFF;
        buf[3] = g_keyboard_advanced_keys[key_index].mode;
        memcpy(&buf[4],&g_keyboard_advanced_keys[key_index].activation_value,sizeof(AnalogValue)*10);
        if (!hid_send(buf,63))
        {
            page_index++;
            if ((page_index & 0xFF)>=ADVANCED_KEY_NUM)
            {
                page_index = 0x0100;
            }
        }
        return 1;
        break;
    case 1: // Global LED
        buf[1] = 0x01;
        buf[2] = g_rgb_switch;
        if (!hid_send(buf,63))
        {
            page_index = 0x0200;
        }
        return 1;
        break;
    case 2: // LED
#define RGB_PAGE_NUM ((RGB_NUM + 5) / 6)
        buf[1] = 0x02;
        for (uint8_t i = 0; i < 6; i++)
        {
            uint8_t key_index = (page_index & 0xFF)*6 + i;
            if (key_index < RGB_NUM)
            {
                uint8_t rgb_index = g_rgb_mapping[g_keyboard_advanced_keys[command_advanced_key_mapping[key_index]].key.id];
                buf[2 + 9 * i + 0] = key_index;
                buf[2 + 9 * i + 1] = g_rgb_configs[rgb_index].mode;
                buf[2 + 9 * i + 2] = g_rgb_configs[rgb_index].rgb.r;
                buf[2 + 9 * i + 3] = g_rgb_configs[rgb_index].rgb.g;
                buf[2 + 9 * i + 4] = g_rgb_configs[rgb_index].rgb.b;
                memcpy(&buf[2 + 9 * i + 5], &g_rgb_configs[rgb_index].speed, sizeof(float));
            }
            else
            {
                buf[2 + 9 * i + 0] = 0xFF;
            }
            
        }
        if (!hid_send(buf,63))
        {
            page_index++;
            if ((page_index & 0xFF)*6>=RGB_NUM)
            {
                page_index = 0x0300;
            }
        }
        return 1;
        break;
#define LAYER_PAGE_LENGTH 16
#define LAYER_PAGE_EXPECTED_NUM ((ADVANCED_KEY_NUM + KEY_NUM + 15) / 16)
    case 3: // Keymap
        buf[1] = 0x03;
        uint8_t layer_index = (page_index & 0xFF) / LAYER_PAGE_EXPECTED_NUM;
        uint8_t layer_page_index = (page_index & 0xFF) % LAYER_PAGE_EXPECTED_NUM;
        buf[2] = layer_index;
        buf[3] = layer_page_index;
        uint8_t size = ((ADVANCED_KEY_NUM + KEY_NUM)-layer_page_index*LAYER_PAGE_LENGTH);
        if (size>LAYER_PAGE_LENGTH)
        {
            size = LAYER_PAGE_LENGTH;
        }
        memcpy(&buf[4], &g_keymap[layer_index][layer_page_index*LAYER_PAGE_LENGTH], size*sizeof(Keycode));
        if (!hid_send(buf,63))
        {
            page_index++;
            if ((page_index & 0xFF)>=LAYER_NUM*LAYER_PAGE_EXPECTED_NUM)
            {
                page_index = 0x0400;
            }
        }
        return 1;
        break;
    case 4: // Dynamic Key
        buf[1] = 0x04;
        uint8_t dk_index = (page_index & 0xFF);
        if (dk_index >= DYNAMIC_KEY_NUM)
        {
            page_index=0x8000;
            return 1;
        }
        if (g_keyboard_dynamic_keys[dk_index].type != DYNAMIC_KEY_NONE)
        {
            buf[2] = dk_index;
            memcpy(&buf[3],&g_keyboard_dynamic_keys[dk_index],sizeof(DynamicKey));
        }
        else
        {
            page_index=0x8000;
            return 1;
        }
        if (!hid_send(buf,63))
        {
            page_index++;
        }
        return 1;
        break;
    case 0x80: // config index
        buf[1] = 0x80;
        buf[2] = g_current_config_index;
        if (!hid_send(buf,63))
        {
            page_index = 0xFFFF;
        }
        return 1;
        break;
    default:
        return 0;
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
        keyboard_reboot();
        break;
    case 0x82: // Factory Reset
        keyboard_factory_reset();
        break;
    case 0x90:
        keyboard_set_config_index(buf[1]);
        break;
    case 0xB0: // Set Debug
        g_keyboard_state = buf[1];
        break;
    case 0xB1: // upload config
        g_keyboard_state = KEYBOARD_UPLOAD_CONFIG;
        start_load_cargo();
        break;
    case 0xFF: // Legacy
        unload_cargo(buf + 1);
        break;
    default:
        break;
    }
}

__WEAK int hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
    return 0;
}