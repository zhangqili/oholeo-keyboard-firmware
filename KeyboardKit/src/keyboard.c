/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "analog.h"
#include "keyboard_conf.h"
#include "rgb.h"
#include "action.h"
#include "filter.h"
#include "mouse.h"
#include "record.h"
#include "storage.h"

#include "stdio.h"
#include "string.h"

__WEAK const uint16_t g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
__WEAK AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];
__WEAK Key g_keyboard_keys[KEY_NUM];

Action *g_keyboard_actions[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

#ifdef NKRO_ENABLE
Keyboard_NKROBuffer g_keyboard_nkro_buffer;
#else
Keyboard_6KROBuffer g_keyboard_6kro_buffer;
#endif

uint8_t g_keyboard_knob_flag;
volatile bool g_keyboard_send_report_enable = true;

KEYBOARD_STATE g_keyboard_state;
volatile bool g_keyboard_send_flag;

uint8_t g_current_config_index;

uint16_t keyboard_get_keycode(uint8_t id)
{
    int8_t layer = layer_cache_get(id);
    uint16_t keycode = 0;
    while (layer>=0)
    {
        keycode = g_keymap[layer][id];
        if ((keycode & 0xFF ) == KEY_TRANSPARENT)
        {
            layer--;
        }
        else
        {
            return keycode;
        }
    }
    return KEY_NO_EVENT;
}

void keyboard_event_handler(KeyboardEvent event)
{
    uint16_t keycode = 0;
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_UP:
        g_keyboard_send_flag = true;
        //layer_cache_set(event.id, g_current_layer);
        keycode = keyboard_get_keycode(event.id);
        switch (keycode & 0xFF)
        {
        case LAYER_CONTROL:
            layer_control(keycode,event.event);
            break;
        default:
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_DOWN:
        g_keyboard_send_flag = true;
        layer_cache_set(event.id, g_current_layer);
        keycode = keyboard_get_keycode(event.id);
        switch (keycode & 0xFF)
        {
        case LAYER_CONTROL:
            layer_control(keycode,event.event);     
            break;
        case KEY_SYSTEM:
            switch ((keycode >> 8) & 0xFF)
            {
            case SYSTEM_RESET:
                keyboard_system_reset();
                break;
            case SYSTEM_FACTORY_RESET:
                keyboard_factory_reset();
                break;
            case SYSTEM_SAVE:
                keyboard_save();
                break;
            case SYSTEM_BOOTLOADER:
                keyboard_jump_to_bootloader();
                break;
            case SYSTEM_DEBUG:
                g_keyboard_state = (g_keyboard_state != KEYBOARD_DEBUG);
                break;
            case SYSTEM_RESET_TO_DEFAULT:
                keyboard_reset_to_default();
                break;
            case SYSTEM_CONFIG0:
            case SYSTEM_CONFIG1:
            case SYSTEM_CONFIG2:
            case SYSTEM_CONFIG3:
                keyboard_set_config_index((keycode >> 8) & 0x0F);
                break;
            default:
                break;
            }       
            break;
        case KEY_USER:
            keyboard_user_handler((keycode >> 8) & 0xFF);
            break;
        default:
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        keycode = keyboard_get_keycode(event.id);
        keyboard_add_buffer(keycode);
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
    }
}

void keyboard_add_buffer(uint16_t keycode)
{
    if ((keycode & 0xFF) <= KEY_EXSEL)
    {
#ifdef NKRO_ENABLE
        keyboard_NKRObuffer_add(&g_keyboard_nkro_buffer, keycode);
#else
        keyboard_6KRObuffer_add(&g_keyboard_6kro_buffer, keycode);
#endif
    }
    else
    {
        switch (keycode & 0xFF)
        {
        case MOUSE_COLLECTION:
            mouse_add_buffer(keycode >> 8);
            break;
        default:
            break;
        }
    }
}

int keyboard_buffer_send(void)
{
#ifdef NKRO_ENABLE
    return keyboard_NKRObuffer_send(&g_keyboard_nkro_buffer);
#else
    return keyboard_6KRObuffer_send(&g_keyboard_6kro_buffer);
#endif
}

void keyboard_buffer_clear(void)
{
#ifdef NKRO_ENABLE
    keyboard_NKRObuffer_clear(&g_keyboard_nkro_buffer);
#else
    keyboard_6KRObuffer_clear(&g_keyboard_6kro_buffer);
#endif
}

int keyboard_6KRObuffer_add(Keyboard_6KROBuffer *buf, uint16_t key)
{
    buf->buffer[0] |= KEY_MODIFIER(key);
    if (KEY_KEYCODE(key) != KEY_NO_EVENT && buf->keynum < 6)
    {
        buf->buffer[2 + buf->keynum] = KEY_KEYCODE(key);
        buf->keynum++;
        return 0;
    }
    else
    {
        return 1;
    }
}

int keyboard_6KRObuffer_send(Keyboard_6KROBuffer* buf)
{
    return keyboard_hid_send(buf->buffer, sizeof(buf->buffer));
}

void keyboard_6KRObuffer_clear(Keyboard_6KROBuffer* buf)
{
    memset(buf, 0, sizeof(Keyboard_6KROBuffer));
}

void keyboard_NKRObuffer_init(Keyboard_NKROBuffer*buf,uint8_t* data_buf,uint8_t length)
{
    buf->buffer = data_buf;
    buf->length = length;
}

int keyboard_NKRObuffer_add(Keyboard_NKROBuffer*buf,uint16_t key)
{
    uint8_t index = KEY_KEYCODE(key)/8+1;
    if (index<buf->length)
    {
        buf->buffer[index] |= (1 << (KEY_KEYCODE(key)%8));
    }
    buf->buffer[0] |= KEY_MODIFIER(key);
    return 0;
}

int keyboard_NKRObuffer_send(Keyboard_NKROBuffer*buf)
{
    return keyboard_hid_send(buf->buffer, buf->length);
}

void keyboard_NKRObuffer_clear(Keyboard_NKROBuffer*buf)
{
    memset(buf->buffer, 0, buf->length);
}

void keyboard_init(void)
{
    storage_mount();
    g_current_config_index = storage_read_config_index();
#ifdef NKRO_ENABLE
    static uint8_t buffer[64];
    keyboard_NKRObuffer_init(&g_keyboard_nkro_buffer, buffer, sizeof(buffer));
#endif
}

void keyboard_reset_to_default(void)
{
    memcpy(g_keymap, g_default_keymap, sizeof(g_keymap));
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        g_keyboard_advanced_keys[i].mode = DEFAULT_ADVANCED_KEY_MODE;
        g_keyboard_advanced_keys[i].trigger_distance = DEFAULT_TRIGGER_DISTANCE;
        g_keyboard_advanced_keys[i].release_distance = DEFAULT_RELEASE_DISTANCE;
        g_keyboard_advanced_keys[i].activation_value = DEFAULT_ACTIVATION_VALUE;
        g_keyboard_advanced_keys[i].deactivation_value = DEFAULT_DEACTIVATION_VALUE;
        g_keyboard_advanced_keys[i].calibration_mode = DEFAULT_CALIBRATION_MODE;
        advanced_key_set_deadzone(g_keyboard_advanced_keys + i, DEFAULT_UPPER_DEADZONE, DEFAULT_LOWER_DEADZONE);
    }
    rgb_factory_reset();
}

void keyboard_factory_reset(void)
{
    keyboard_reset_to_default();
    for (int i = 0; i < STORAGE_CONFIG_FILE_NUM; i++)
    {
        storage_save_config(i);
    }
}

__WEAK void keyboard_system_reset(void)
{
}

__WEAK void keyboard_jump_to_bootloader(void)
{
}

__WEAK void keyboard_user_handler(uint8_t code)
{
    UNUSED(code);
}


__WEAK void keyboard_scan(void)
{

}

void keyboard_recovery(void)
{
    storage_read_config(g_current_config_index);
}

void keyboard_save(void)
{
    storage_save_config(g_current_config_index);
}

void keyboard_set_config_index(uint8_t index)
{
    if (index < STORAGE_CONFIG_FILE_NUM)
    {
        g_current_config_index = index;
    }
    storage_save_config_index(g_current_config_index);
    keyboard_recovery();
}

void keyboard_send_report(void)
{
    static uint32_t mouse_value;
    if (g_keyboard_send_report_enable 
#ifndef CONTINOUS_POLL
        && g_keyboard_send_flag
#endif
    )
    {
#ifndef CONTINOUS_POLL
        g_keyboard_send_flag = false;
#endif
        keyboard_buffer_clear();
        mouse_buffer_clear(&g_mouse);

        for (int i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            keyboard_event_handler(MK_EVENT(g_keyboard_advanced_keys[i].key.id, g_keyboard_advanced_keys[i].key.state ? KEYBOARD_EVENT_KEY_TRUE : KEYBOARD_EVENT_KEY_FALSE));
        }
        for (int i = 0; i < KEY_NUM; i++)
        {        
            keyboard_event_handler(MK_EVENT(g_keyboard_keys[i].id, g_keyboard_keys[i].state ? KEYBOARD_EVENT_KEY_TRUE : KEYBOARD_EVENT_KEY_FALSE));
        }
        if (keyboard_buffer_send())
        {
            g_keyboard_send_flag = true;
        }
        if ((*(uint32_t*)&g_mouse)!=mouse_value)
        {
            mouse_buffer_send(&g_mouse);
        }
        mouse_value = *(uint32_t*)&g_mouse;
    }
}

__WEAK void keyboard_task(void)
{
    keyboard_scan();
    analog_average();
    analog_check();
    keyboard_post_process();
    keyboard_send_report();
}

__WEAK int keyboard_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
    return 0;
}
__WEAK void keyboard_delay(uint32_t ms)
{
    UNUSED(ms);
}
__WEAK void keyboard_post_process(void)
{
}

void keyboard_key_update(Key *key, bool state)
{
    if (!key->state && state)
    {
        keyboard_event_handler(MK_EVENT(key->id, KEYBOARD_EVENT_KEY_DOWN));
    }
    if (key->state && !state)
    {
        keyboard_event_handler(MK_EVENT(key->id, KEYBOARD_EVENT_KEY_UP));
    }
    key_update(key, state);
}

void keyboard_advanced_key_update_state(AdvancedKey *key, bool state)
{
    if (!key->key.state && state)
    {
        keyboard_event_handler(MK_EVENT(key->key.id, KEYBOARD_EVENT_KEY_DOWN));
#ifdef ENABLE_RGB
        rgb_activate(key->key.id);
#endif
#ifdef ENABLE_KPS
        record_kps_tick();
#endif
#ifdef ENABLE_COUNTER
        g_key_counts[key->key.id]++;
#endif
    }
    if (key->key.state && !state)
    {
        keyboard_event_handler(MK_EVENT(key->key.id, KEYBOARD_EVENT_KEY_UP));
    }
    advanced_key_update_state(key, state);
}