/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "keyboard.h"
#include "analog.h"
#include "keyboard_conf.h"
#include "mouse.h"
#include "layer.h"
#include "record.h"
#include "report.h"
#include "process_midi.h"

#include "stdio.h"
#include "string.h"
#include "math.h"

#ifdef RGB_ENABLE
#include "rgb.h"
#endif
#ifdef STORAGE_ENABLE
#include "storage.h"
#endif
#ifdef DYNAMICKEY_ENABLE
#include "dynamic_key.h"
#endif
#ifdef EXTRAKEY_ENABLE
#include "extra_key.h"
#endif
#ifdef JOYSTICK_ENABLE
#include "joystick.h"
#endif
#ifdef MIDI_ENABLE
#include "qmk_midi.h"
#endif

__WEAK const Keycode g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
__WEAK AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];
__WEAK Key g_keyboard_keys[KEY_NUM];

uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

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

KeyboardEvent keyboard_make_event(Key*key, uint8_t event)
{
    if (event == KEYBOARD_EVENT_KEY_DOWN)
        layer_cache_set(key->id, g_current_layer);
    return MK_EVENT(layer_cache_get_keycode(key->id), event);
}

void keyboard_event_handler(KeyboardEvent event)
{
    switch (KEYCODE(event.keycode))
    {
#ifdef MOUSE_ENABLE
    case MOUSE_COLLECTION:
        mouse_event_handler(event);
        break;
#endif
#ifdef EXTRAKEY_ENABLE
    case CONSUMER_COLLECTION:
    case SYSTEM_COLLECTION:
        extra_key_event_handler(event);
        break;
#endif
#ifdef JOYSTICK_ENABLE
    case JOYSTICK_COLLECTION:
        joystick_event_handler(event);
        break;
#endif
#ifdef DYNAMICKEY_ENABLE
    case DYNAMIC_KEY:
        if (event.event == KEYBOARD_EVENT_KEY_TRUE)
        {
            dynamic_key_add_buffer(&g_keyboard_dynamic_keys[MODIFIER(event.keycode)]);
        }
        break;
#endif
    case LAYER_CONTROL:
        layer_control(event);
        break;
    case KEYBOARD_OPERATION:
        keyboard_operation_event_handler(event);
        break;
    case KEY_USER:
        keyboard_user_event_handler(event);
        break;
        
    default:
        switch (event.event)
        {
        case KEYBOARD_EVENT_KEY_UP:
        case KEYBOARD_EVENT_KEY_DOWN:
            BIT_SET(g_keyboard_send_flags, KEYBOARD_REPORT_FLAG);
            break;
        case KEYBOARD_EVENT_KEY_TRUE:
            if (KEYCODE(event.keycode) <= KEY_EXSEL)
            {
#ifdef NKRO_ENABLE
                if (g_keyboard_nkro_enable)
                {
                    keyboard_NKRObuffer_add(&keyboard_nkro_buffer, event.keycode);
                }
                else
#endif
                {
                    keyboard_6KRObuffer_add(&keyboard_6kro_buffer, event.keycode);
                }
            }
            break;
        case KEYBOARD_EVENT_KEY_FALSE:
            break;
        default:
            break;
        }
        break;
    }
}

void keyboard_operation_event_handler(KeyboardEvent event)
{
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_UP:
        break;
    case KEYBOARD_EVENT_KEY_DOWN:
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
        case KEYBOARD_DEBUG_TOGGLE:
            g_keyboard_state = (g_keyboard_state != KEYBOARD_STATE_DEBUG);
            break;
        case KEYBOARD_RESET_TO_DEFAULT:
            keyboard_reset_to_default();
            break;
        case KEYBOARD_NKRO_TOGGLE:
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
    case KEYBOARD_EVENT_KEY_TRUE:
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
#ifdef RGB_ENABLE
        rgb_activate(key->key.id);
#endif
#ifdef KPS_ENABLE
        record_kps_tick();
#endif
#ifdef COUNTER_ENABLE
        g_key_counts[key->key.id]++;
#endif
        break;
    case KEYBOARD_EVENT_KEY_UP:
        keyboard_event_handler(event);
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        switch (KEYCODE(event.keycode))
        {
#ifdef JOYSTICK_ENABLE
        case JOYSTICK_COLLECTION:
            if (MODIFIER(event.keycode) & 0xE0)
            {
                BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
                joystick_set_axis(MODIFIER(event.keycode), key->value);
                break;
            }
#endif
            // fall through
        default:
            keyboard_event_handler(event);
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
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
#ifdef STORAGE_ENABLE
    storage_mount();
    g_current_config_index = storage_read_config_index();
#endif
#ifdef RGB_ENABLE
    rgb_init();
#endif
#ifdef MIDI_ENABLE
    setup_midi();
#endif
    keyboard_recovery();
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
#ifdef RGB_ENABLE
    rgb_factory_reset();
#endif
#ifdef DYNAMICKEY_ENABLE
    memset(g_keyboard_dynamic_keys, 0, sizeof(g_keyboard_dynamic_keys));
#endif
}

void keyboard_factory_reset(void)
{
    keyboard_reset_to_default();
#ifdef STORAGE_ENABLE
    for (int i = 0; i < STORAGE_CONFIG_FILE_NUM; i++)
    {
        storage_save_config(i);
    }
#endif
}

__WEAK void keyboard_reboot(void)
{
}

__WEAK void keyboard_jump_to_bootloader(void)
{
}

__WEAK void keyboard_user_event_handler(KeyboardEvent event)
{
    UNUSED(event);
}


__WEAK void keyboard_scan(void)
{

}

void keyboard_recovery(void)
{
#ifdef STORAGE_ENABLE
    storage_read_config(g_current_config_index);
#else
    keyboard_reset_to_default();
#endif
}

void keyboard_save(void)
{
#ifdef STORAGE_ENABLE
    storage_save_config(g_current_config_index);
#endif
}

void keyboard_set_config_index(uint8_t index)
{
#ifdef STORAGE_ENABLE
    if (index < STORAGE_CONFIG_FILE_NUM)
    {
        g_current_config_index = index;
    }
    storage_save_config_index(g_current_config_index);
#else
    UNUSED(index);
#endif

    keyboard_recovery();
}

void keyboard_send_report(void)
{
    keyboard_buffer_clear();
#ifdef MOUSE_ENABLE
    mouse_buffer_clear();
#endif
#ifdef JOYSTICK_ENABLE
    joystick_buffer_clear();
#endif

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
#ifdef MOUSE_ENABLE
        if (BIT_GET(g_keyboard_send_flags,MOUSE_REPORT_FLAG))
        {
            if (!mouse_buffer_send())
            {
                BIT_RESET(g_keyboard_send_flags,MOUSE_REPORT_FLAG);
            }
        }
#endif
#ifdef EXTRAKEY_ENABLE
        if (BIT_GET(g_keyboard_send_flags,CONSUMER_REPORT_FLAG))
        {
            if (!consumer_key_buffer_send())
            {
                BIT_RESET(g_keyboard_send_flags,CONSUMER_REPORT_FLAG);
            }
        }
        if (BIT_GET(g_keyboard_send_flags,SYSTEM_REPORT_FLAG))
        {
            if (!system_key_buffer_send())
            {
                BIT_RESET(g_keyboard_send_flags,SYSTEM_REPORT_FLAG);
            }
        }
#endif
        if (BIT_GET(g_keyboard_send_flags,KEYBOARD_REPORT_FLAG))
        {
            if (!keyboard_buffer_send())
            {
                BIT_RESET(g_keyboard_send_flags,KEYBOARD_REPORT_FLAG);
            }
        }
#ifdef JOYSTICK_ENABLE
        if (BIT_GET(g_keyboard_send_flags,JOYSTICK_REPORT_FLAG))
        {
            if (!joystick_buffer_send())
            {
                BIT_RESET(g_keyboard_send_flags,JOYSTICK_REPORT_FLAG);
            }
        }
#endif
    }
}

__WEAK void keyboard_task(void)
{
    keyboard_scan();
    analog_average();
    analog_check();
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
#ifdef DYNAMICKEY_ENABLE
    case DYNAMIC_KEY:
        const uint8_t dynamic_key_index = (keycode>>8)&0xFF;
        dynamic_key_update(&g_keyboard_dynamic_keys[dynamic_key_index], key, state);
        break;
#endif
#ifdef MIDI_ENABLE
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
            midi_event_handler(event, velocity);
            keyboard_advanced_key_event_handler(key,event);
        }
        if (key->key.state && !state)
        {
            KeyboardEvent event = keyboard_make_event(&key->key, KEYBOARD_EVENT_KEY_UP);
            midi_event_handler(event, velocity);
            keyboard_advanced_key_event_handler(key,event);
        }
        advanced_key_update_state(key, state);
        key->key.report_state = state;
        break;
#endif
#ifdef JOYSTICK_ENABLE
    case JOYSTICK_COLLECTION:
        if (MODIFIER(keycode) & 0xE0)
        {
            BIT_SET(g_keyboard_send_flags, JOYSTICK_REPORT_FLAG);
            key->key.report_state = true;
            break;
        }
#endif
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
