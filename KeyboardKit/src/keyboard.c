/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "analog.h"
#include "keyboard_conf.h"
#include "rgb.h"
#include "dynamic_key.h"
#include "mouse.h"
#include "record.h"
#include "storage.h"
#include "joystick.h"
#include "report.h"
#include "process_midi.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

__WEAK const Keycode g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
__WEAK AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];
__WEAK Key g_keyboard_keys[KEY_NUM];

uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

DynamicKey g_keyboard_dynamic_keys[DYNAMIC_KEY_NUM];

uint8_t g_keyboard_led_state;

uint32_t g_keyboard_tick;

uint8_t g_keyboard_knob_flag;
volatile bool g_keyboard_send_report_enable = true;

KEYBOARD_STATE g_keyboard_state;
volatile uint8_t g_keyboard_send_flags;

uint8_t g_current_config_index;

#ifdef NKRO_ENABLE
bool g_keyboard_nkro_enable;
static Keyboard_NKROBuffer keyboard_nkro_buffer;
#endif
static Keyboard_6KROBuffer keyboard_6kro_buffer;
static uint16_t keyboard_consumer_buffer;
static uint16_t keyboard_system_buffer;

KeyboardEvent keyboard_make_event(Key*key, uint8_t event)
{
    if (event == KEYBOARD_EVENT_KEY_DOWN)
        layer_cache_set(key->id, g_current_layer);
    Keycode keycode = layer_cache_get_keycode(key->id);
    return MK_EVENT(keycode, event);
}

void keyboard_event_handler(KeyboardEvent event)
{
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_UP:
        if (KEYCODE(event.keycode) <= KEY_EXSEL)
        {
            BIT_SET(g_keyboard_send_flags, KEYBOARD_REPORT_FLAG);
        }
        else
        {
            switch (KEYCODE(event.keycode))
            {
            case MOUSE_COLLECTION:
                BIT_SET(g_keyboard_send_flags, MOUSE_REPORT_FLAG);
                break;
            case CONSUMER_COLLECTION:
                keyboard_consumer_buffer = 0;
                BIT_SET(g_keyboard_send_flags, CONSUMER_REPORT_FLAG);
                break;
            case SYSTEM_COLLECTION:
                keyboard_system_buffer = 0;
                BIT_SET(g_keyboard_send_flags, SYSTEM_REPORT_FLAG);
                break;
            case JOYSTICK_COLLECTION:
                BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
                break;
            case LAYER_CONTROL:
                layer_control(event.keycode,event.event);
                break;
            default:
                break;
            }
        }
        break;
    case KEYBOARD_EVENT_KEY_DOWN:
        if (KEYCODE(event.keycode) <= KEY_EXSEL)
        {
            BIT_SET(g_keyboard_send_flags, KEYBOARD_REPORT_FLAG);
        }
        else
        {
            switch (KEYCODE(event.keycode))
            {
            case MOUSE_COLLECTION:
                BIT_SET(g_keyboard_send_flags, MOUSE_REPORT_FLAG);
                break;
            case CONSUMER_COLLECTION:
                keyboard_consumer_buffer = CONSUMER_KEYCODE_TO_RAWCODE(MODIFIER(event.keycode));
                BIT_SET(g_keyboard_send_flags, CONSUMER_REPORT_FLAG);
                break;
            case SYSTEM_COLLECTION:
                keyboard_system_buffer = MODIFIER(event.keycode);
                BIT_SET(g_keyboard_send_flags, SYSTEM_REPORT_FLAG);
                break;
            case JOYSTICK_COLLECTION:
                BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
                break;
            case LAYER_CONTROL:
                layer_control(event.keycode,event.event);
                break;
            case KEYBOARD_OPERATION:
                switch (MODIFIER(event.keycode))
                {
                case KEYBOARD_REBOOT:
                    keyboard_reboot();
                    break;
                case KEYBOARD_FACTORY_RESET:
                    keyboard_factory_reset();
                    break;
                case KEYBOARD_SAVE:
                    keyboard_save();
                    break;
                case KEYBOARD_BOOTLOADER:
                    keyboard_jump_to_bootloader();
                    break;
                case KEYBOARD_TOGGLE_DEBUG:
                    g_keyboard_state = (g_keyboard_state != KEYBOARD_DEBUG);
                    break;
                case KEYBOARD_RESET_TO_DEFAULT:
                    keyboard_reset_to_default();
                    break;
                case KEYBOARD_TOGGLE_NKRO:
                    g_keyboard_nkro_enable = !g_keyboard_nkro_enable;
                    break;
                case KEYBOARD_CONFIG0:
                case KEYBOARD_CONFIG1:
                case KEYBOARD_CONFIG2:
                case KEYBOARD_CONFIG3:
                    keyboard_set_config_index((event.keycode >> 8) & 0x0F);
                    break;
                default:
                    break;
                }
                break;
            case KEY_USER:
                keyboard_user_handler(MODIFIER(event.keycode));
                break;
            default:
                break;
            }
        }
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        keyboard_add_buffer(event.keycode);
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
    }
}

void keyboard_advanced_key_event_handler(AdvancedKey*key, KeyboardEvent event)
{
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_DOWN:
        layer_cache_set(key->key.id, g_current_layer);
        keyboard_event_handler(event);
#ifdef ENABLE_RGB
        rgb_activate(key->key.id);
#endif
#ifdef ENABLE_KPS
        record_kps_tick();
#endif
#ifdef ENABLE_COUNTER
        g_key_counts[key->key.id]++;
#endif
        break;
    case KEYBOARD_EVENT_KEY_UP:
        keyboard_event_handler(event);
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        switch (KEYCODE(event.keycode))
        {
        case JOYSTICK_COLLECTION:
            if (MODIFIER(event.keycode) & 0xE0)
            {
                BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
                joystick_set_axis(MODIFIER(event.keycode), key->value);
                break;
            }
            // fall through
        default:
            keyboard_add_buffer(event.keycode);
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
    }
}

void keyboard_add_buffer(uint16_t keycode)
{
    if (KEYCODE(keycode) <= KEY_EXSEL)
    {
#ifdef NKRO_ENABLE
        if (g_keyboard_nkro_enable)
        {
            keyboard_NKRObuffer_add(&keyboard_nkro_buffer, keycode);
        }
        else
#endif
        {
            keyboard_6KRObuffer_add(&keyboard_6kro_buffer, keycode);
        }
    }
    else
    {
        switch (KEYCODE(keycode))
        {
        case MOUSE_COLLECTION:
            mouse_add_buffer(MODIFIER(keycode));
            break;
        case DYNAMIC_KEY:
            dynamic_key_add_buffer(&g_keyboard_dynamic_keys[MODIFIER(keycode)]);
            break;
        case CONSUMER_COLLECTION:
            if (!keyboard_consumer_buffer)
            {
                keyboard_consumer_buffer = CONSUMER_KEYCODE_TO_RAWCODE(MODIFIER(keycode));
            }
            break;
        case SYSTEM_COLLECTION:
            if (!keyboard_system_buffer)
            {
                keyboard_system_buffer = MODIFIER(keycode);
            }
            break;
        case JOYSTICK_COLLECTION:
            joystick_add_buffer(MODIFIER(keycode));
            break;
        default:
            break;
        }
    }
}

int keyboard_buffer_send(void)
{
#ifdef NKRO_ENABLE
    if (g_keyboard_nkro_enable)
    {
        return keyboard_NKRObuffer_send(&keyboard_nkro_buffer);
    }
#endif
    return keyboard_6KRObuffer_send(&keyboard_6kro_buffer);
}

void keyboard_buffer_clear(void)
{
#ifdef NKRO_ENABLE
    if (g_keyboard_nkro_enable)
    {
        keyboard_NKRObuffer_clear(&keyboard_nkro_buffer);
    }
#endif
    keyboard_6KRObuffer_clear(&keyboard_6kro_buffer);
}

int keyboard_6KRObuffer_add(Keyboard_6KROBuffer *buf, Keycode keycode)
{
    buf->modifier |= MODIFIER(keycode);
    if (KEYCODE(keycode) != KEY_NO_EVENT && buf->keynum < 6)
    {
        buf->buffer[buf->keynum] = KEYCODE(keycode);
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
    return keyboard_hid_send((uint8_t*)buf, sizeof(buf->buffer));
}

void keyboard_6KRObuffer_clear(Keyboard_6KROBuffer* buf)
{
    memset(buf, 0, sizeof(Keyboard_6KROBuffer));
}

int keyboard_NKRObuffer_add(Keyboard_NKROBuffer*buf,Keycode keycode)
{
    if (KEYCODE(keycode) > NKRO_REPORT_BITS*8 )
    {
        return 1;
    }
    uint8_t index = KEYCODE(keycode)/8;
    buf->buffer[index] |= (1 << (KEYCODE(keycode)%8));
    buf->modifier |= MODIFIER(keycode);
    return 0;
}

int keyboard_NKRObuffer_send(Keyboard_NKROBuffer*buf)
{
    return keyboard_hid_send((uint8_t*)buf, sizeof(Keyboard_NKROBuffer));
}

void keyboard_NKRObuffer_clear(Keyboard_NKROBuffer*buf)
{
    memset(buf, 0, sizeof(Keyboard_NKROBuffer));
}

void keyboard_init(void)
{
    storage_mount();
    g_current_config_index = storage_read_config_index();
}

__WEAK void keyboard_reset_to_default(void)
{
    memcpy(g_keymap, g_default_keymap, sizeof(g_keymap));
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        g_keyboard_advanced_keys[i].config.mode = DEFAULT_ADVANCED_KEY_MODE;
        g_keyboard_advanced_keys[i].config.trigger_distance = DEFAULT_TRIGGER_DISTANCE * ANALOG_VALUE_RANGE;
        g_keyboard_advanced_keys[i].config.release_distance = DEFAULT_RELEASE_DISTANCE * ANALOG_VALUE_RANGE;
        g_keyboard_advanced_keys[i].config.activation_value = DEFAULT_ACTIVATION_VALUE * ANALOG_VALUE_RANGE;
        g_keyboard_advanced_keys[i].config.deactivation_value = DEFAULT_DEACTIVATION_VALUE * ANALOG_VALUE_RANGE;
        g_keyboard_advanced_keys[i].config.calibration_mode = DEFAULT_CALIBRATION_MODE;
        advanced_key_set_deadzone(g_keyboard_advanced_keys + i, 
            DEFAULT_UPPER_DEADZONE * ANALOG_VALUE_RANGE, 
            DEFAULT_LOWER_DEADZONE * ANALOG_VALUE_RANGE);
    }
    rgb_factory_reset();
    memset(g_keyboard_dynamic_keys, 0, sizeof(g_keyboard_dynamic_keys));
}

void keyboard_factory_reset(void)
{
    keyboard_reset_to_default();
    for (int i = 0; i < STORAGE_CONFIG_FILE_NUM; i++)
    {
        storage_save_config(i);
    }
}

__WEAK void keyboard_reboot(void)
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
    keyboard_buffer_clear();
    mouse_buffer_clear(&g_mouse);
    joystick_buffer_clear(&g_joystick);

    if (g_keyboard_send_report_enable 
#ifndef CONTINOUS_POLL
        && g_keyboard_send_flags
#endif
    )
    {
        for (int i = 0; i < ADVANCED_KEY_NUM; i++)
        {
            keyboard_advanced_key_event_handler(&g_keyboard_advanced_keys[i], 
                keyboard_make_event(&g_keyboard_advanced_keys[i].key, g_keyboard_advanced_keys[i].key.report_state ? KEYBOARD_EVENT_KEY_TRUE : KEYBOARD_EVENT_KEY_FALSE));
        }
        for (int i = 0; i < KEY_NUM; i++)
        {        
            keyboard_event_handler(keyboard_make_event(&g_keyboard_keys[i], g_keyboard_keys[i].report_state ? KEYBOARD_EVENT_KEY_TRUE : KEYBOARD_EVENT_KEY_FALSE));
        }
#ifdef CONTINOUS_POLL
        BIT_SET(g_keyboard_send_flags,KEYBOARD_REPORT_FLAG);
#endif
        if (BIT_GET(g_keyboard_send_flags,MOUSE_REPORT_FLAG))
        {
            if ((*(uint32_t*)&g_mouse)!=mouse_value)
            {
                if (!mouse_buffer_send(&g_mouse))
                {
                    BIT_RESET(g_keyboard_send_flags,MOUSE_REPORT_FLAG);
                }
                mouse_value = *(uint32_t*)&g_mouse;
            }
        }
        if (BIT_GET(g_keyboard_send_flags,CONSUMER_REPORT_FLAG))
        {
            if (!keyboard_extra_hid_send(REPORT_ID_CONSUMER, keyboard_consumer_buffer))
            {
                BIT_RESET(g_keyboard_send_flags,CONSUMER_REPORT_FLAG);
            }
        }
        if (BIT_GET(g_keyboard_send_flags,SYSTEM_REPORT_FLAG))
        {
            if (!keyboard_extra_hid_send(REPORT_ID_SYSTEM, keyboard_system_buffer))
            {
                BIT_RESET(g_keyboard_send_flags,SYSTEM_REPORT_FLAG);
            }
        }
        if (BIT_GET(g_keyboard_send_flags,KEYBOARD_REPORT_FLAG))
        {
            if (!keyboard_buffer_send())
            {
                BIT_RESET(g_keyboard_send_flags,KEYBOARD_REPORT_FLAG);
            }
        }
        if (BIT_GET(g_keyboard_send_flags,JOYSTICK_REPORT_FLAG))
        {
            if (!joystick_buffer_send(&g_joystick))
            {
                BIT_RESET(g_keyboard_send_flags,JOYSTICK_REPORT_FLAG);
            }
        }
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

__WEAK int keyboard_extra_hid_send(uint8_t report_id, uint16_t usage)
{
    UNUSED(report_id);
    UNUSED(usage);
    return 0;
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
        keyboard_event_handler(keyboard_make_event(key, KEYBOARD_EVENT_KEY_DOWN));
    }
    if (key->state && !state)
    {
        keyboard_event_handler(keyboard_make_event(key, KEYBOARD_EVENT_KEY_UP));
    }
    key_update(key, state);
    key->report_state = state;
}

void keyboard_advanced_key_update_state(AdvancedKey *key, bool state)
{
    const Keycode keycode = layer_cache_get_keycode(key->key.id);
    switch (KEYCODE(keycode))
    {
    case DYNAMIC_KEY:
        const uint8_t dynamic_key_index = (keycode>>8)&0xFF;
        dynamic_key_update(&g_keyboard_dynamic_keys[dynamic_key_index], key, state);
        break;
    case MIDI_COLLECTION:
    case MIDI_NOTE:
        float intensity = fabs(key->difference/(float)MIDI_REF_VELOCITY);
        if (intensity > 1.0f)
        {
            intensity = 1.0f;
        }
        uint8_t velocity = intensity*127;
        if (!key->key.state && state)
        {
            KeyboardEvent event = keyboard_make_event(&key->key, KEYBOARD_EVENT_KEY_DOWN);
            midi_event_handler(event, true, velocity);
            keyboard_advanced_key_event_handler(key,event);
        }
        if (key->key.state && !state)
        {
            KeyboardEvent event = keyboard_make_event(&key->key, KEYBOARD_EVENT_KEY_UP);
            midi_event_handler(event, true, velocity);
            keyboard_advanced_key_event_handler(key,event);
        }
        advanced_key_update_state(key, state);
        key->key.report_state = state;
        break;
    case JOYSTICK_COLLECTION:
        if (MODIFIER(keycode) & 0xE0)
        {
            BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
            key->key.report_state = true;
            break;
        }
        // fall through
    default:
        if (!key->key.state && state)
        {
            KeyboardEvent event = keyboard_make_event(&key->key, KEYBOARD_EVENT_KEY_DOWN);
            keyboard_advanced_key_event_handler(key,event);
        }
        if (key->key.state && !state)
        {
            KeyboardEvent event = keyboard_make_event(&key->key, KEYBOARD_EVENT_KEY_UP);
            keyboard_advanced_key_event_handler(key,event);
        }
        advanced_key_update_state(key, state);
        key->key.report_state = state;
        break;
    }
}
