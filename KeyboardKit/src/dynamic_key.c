/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "dynamic_key.h"
#include "keyboard.h"

static uint8_t g_keyboard_dynamic_keys_length;

void dynamic_key_process(void)
{
    for (int i = 0; i < g_keyboard_dynamic_keys_length; i++)
    {
        DynamicKey* dynamic_key = g_keyboard_dynamic_keys[i];
        switch (*dynamic_key)
        {
        case DYNAMIC_KEY_STROKE:
            break;
        case DYNAMIC_KEY_MOD_TAP:    
            break;
        case DYNAMIC_KEY_TOGGLE_KEY:
            break;
        case DYNAMIC_KEY_RAPPY_SNAPPY:
            //dynamic_key_rs_execute(dynamic_key, dynamic_key);
            break;
        default:
            break;
        }
    }
}

void dynamic_key_update(DynamicKey*dynamic_key,AdvancedKey*advanced_key)
{
    switch (*dynamic_key)
    {
    case DYNAMIC_KEY_STROKE:
        break;
    case DYNAMIC_KEY_MOD_TAP:
        break;
    case DYNAMIC_KEY_TOGGLE_KEY:
        break;
    case DYNAMIC_KEY_RAPPY_SNAPPY:
        dynamic_key_rs_update(dynamic_key, advanced_key);
        break;
    default:
        break;
    }
}


void dynamic_key_add_buffer(DynamicKey*dynamic_key)
{
    switch (*dynamic_key)
    {
    case DYNAMIC_KEY_STROKE:
        break;
    case DYNAMIC_KEY_MOD_TAP:
        break;
    case DYNAMIC_KEY_TOGGLE_KEY:
        break;
    case DYNAMIC_KEY_RAPPY_SNAPPY:
        DynamicKeyRappySnappy*dynamic_key_rs=(DynamicKeyRappySnappy*)dynamic_key;
        if (dynamic_key_rs->key1_state)
            keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_TRUE));
        if (dynamic_key_rs->key2_state)
            keyboard_event_handler(MK_EVENT(dynamic_key_rs->key2_binding, KEYBOARD_EVENT_KEY_TRUE));
        break;
    default:
        break;
    }
}

void dynamic_key_rs_update(DynamicKey*dynamic_key, AdvancedKey*key)
{
    DynamicKeyRappySnappy*dynamic_key_rs=(DynamicKeyRappySnappy*)dynamic_key;
    AdvancedKey*key1 = &g_keyboard_advanced_keys[dynamic_key_rs->key1_id];
    AdvancedKey*key2 = &g_keyboard_advanced_keys[dynamic_key_rs->key2_id];
    uint8_t last_key1_state = dynamic_key_rs->key1_state;
    uint8_t last_key2_state = dynamic_key_rs->key2_state;
    if (key->key.id == dynamic_key_rs->key1_id)
    {
        if (((key1->value > key2->value)&&
        (key1->value > key1->upper_deadzone)) || 
        ((key1->value>= (ANALOG_VALUE_MAX - key1->lower_deadzone))&&
        (key2->value>= (ANALOG_VALUE_MAX - key2->lower_deadzone))))
        {
            dynamic_key_rs->key1_state = true;
        }
    }
    else if  (key->key.id == dynamic_key_rs->key2_id)
    {
        if (((key1->value < key2->value)&&
        (key2->value > key2->upper_deadzone)) || 
        ((key1->value>= (ANALOG_VALUE_MAX - key1->lower_deadzone))&&
        (key2->value>= (ANALOG_VALUE_MAX - key2->lower_deadzone))))
        {
            dynamic_key_rs->key2_state = true;
        }
    }
    if (dynamic_key_rs->key1_state && !last_key1_state)
    {
        keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_DOWN));
    }
    if (!dynamic_key_rs->key1_state && last_key1_state)
    {
        keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_UP));
    }
    if (dynamic_key_rs->key2_state && !last_key2_state)
    {
        keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_DOWN));
    }
    if (!dynamic_key_rs->key2_state && last_key2_state)
    {
        keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_UP));
    }
}