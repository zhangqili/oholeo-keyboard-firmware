/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "analog.h"
#include "keyboard_conf.h"
#include "rgb.h"
#include "stdio.h"
#include "lfs.h"
#include "action.h"
#include "filter.h"
#include "mouse.h"
#include "record.h"

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

volatile bool g_debug_enable;
volatile bool g_keyboard_send_flag;

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
                g_debug_enable = !g_debug_enable;
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

void keyboard_buffer_send(void)
{
#ifdef NKRO_ENABLE
    keyboard_NKRObuffer_send(&g_keyboard_nkro_buffer);
#else
    keyboard_6KRObuffer_send(&g_keyboard_6kro_buffer);
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

void keyboard_6KRObuffer_send(Keyboard_6KROBuffer* buf)
{
    keyboard_hid_send(buf->buffer, sizeof(buf->buffer));
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

void keyboard_NKRObuffer_send(Keyboard_NKROBuffer*buf)
{
    keyboard_hid_send(buf->buffer, buf->length);
}

void keyboard_NKRObuffer_clear(Keyboard_NKROBuffer*buf)
{
    memset(buf->buffer, 0, buf->length);
}

void keyboard_init(void)
{
#ifdef NKRO_ENABLE
    static uint8_t buffer[64];
    keyboard_NKRObuffer_init(&g_keyboard_nkro_buffer, buffer, sizeof(buffer));
#endif
}

void keyboard_factory_reset(void)
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
    keyboard_save();
    //keyboard_system_reset();
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
    // mount the filesystem
    int err = lfs_mount(&g_lfs, &g_lfs_config);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(&g_lfs, &g_lfs_config);
        lfs_mount(&g_lfs, &g_lfs_config);
    }
    lfs_file_open(&g_lfs, &g_lfs_file, "config1.dat", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &g_lfs_file);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lfs_file_read(&g_lfs, &g_lfs_file, ((void *)(&g_keyboard_advanced_keys[i])) + sizeof(Key) + 4*sizeof(AnalogValue),
                      sizeof(AdvancedKey) - sizeof(Key) - 4*sizeof(AnalogValue));
    }
    lfs_file_read(&g_lfs, &g_lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_read(&g_lfs, &g_lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_read(&g_lfs, &g_lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&g_lfs, &g_lfs_file);
    printf("recovery = %d", err);
    // release any resources we were using
    lfs_unmount(&g_lfs);
    // print the boot count
}

void keyboard_save(void)
{
    // mount the filesystem
    int err = lfs_mount(&g_lfs, &g_lfs_config);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(&g_lfs, &g_lfs_config);
        lfs_mount(&g_lfs, &g_lfs_config);
    }
    // read current count
    lfs_file_open(&g_lfs, &g_lfs_file, "config1.dat", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &g_lfs_file);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lfs_file_write(&g_lfs, &g_lfs_file, ((void *)(&g_keyboard_advanced_keys[i])) + sizeof(Key) + 4*sizeof(AnalogValue),
                       sizeof(AdvancedKey) - sizeof(Key) - 4*sizeof(AnalogValue));
    }
    lfs_file_write(&g_lfs, &g_lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_write(&g_lfs, &g_lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_write(&g_lfs, &g_lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    // remember the storage is not updated until the file is closed successfully
    err = lfs_file_close(&g_lfs, &g_lfs_file);
    printf("save = %d", err);
    // release any resources we were using
    lfs_unmount(&g_lfs);
    // print the boot count
}

void keyboard_send_report(void)
{
    static uint32_t mouse_value;
    if (g_keyboard_send_report_enable 
#ifndef CONTINUOUS_POLL
        && g_keyboard_send_flag
#endif
    )
    {
#ifndef CONTINUOUS_POLL
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
        keyboard_buffer_send();
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

__WEAK void keyboard_hid_send(uint8_t *report, uint16_t len)
{
    UNUSED(report);
    UNUSED(len);
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