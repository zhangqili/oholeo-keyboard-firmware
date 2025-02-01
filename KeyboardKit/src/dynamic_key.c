/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */


#include "dynamic_key.h"
#include "command.h"
#include "keyboard.h"

//static uint8_t g_keyboard_dynamic_keys_length;

void dynamic_key_update(DynamicKey*dynamic_key,AdvancedKey*advanced_key)
{
    switch (*dynamic_key)
    {
    case DYNAMIC_KEY_STROKE:
        dynamic_key_s_update(dynamic_key, advanced_key);
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
        DynamicKeyStroke4x4*dynamic_key_s=(DynamicKeyStroke4x4*)dynamic_key;
        for (int i = 0; i < 4; i++)
        {
            if (BIT_GET(dynamic_key_s->key_state,i))
                keyboard_event_handler(MK_EVENT(dynamic_key_s->key_binding[i], KEYBOARD_EVENT_KEY_TRUE));
        }
        break;
    case DYNAMIC_KEY_MOD_TAP:
        break;
    case DYNAMIC_KEY_TOGGLE_KEY:
        break;
    case DYNAMIC_KEY_RAPPY_SNAPPY:
        {
            static uint8_t t;
            t=!t;
            if (t)
                break;
            DynamicKeyRappySnappy*dynamic_key_rs=(DynamicKeyRappySnappy*)dynamic_key;
            if (dynamic_key_rs->key1_state)
                keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_TRUE));
            if (dynamic_key_rs->key2_state)
                keyboard_event_handler(MK_EVENT(dynamic_key_rs->key2_binding, KEYBOARD_EVENT_KEY_TRUE));
        }
        break;
    default:
        break;
    }
}

#define DKS_PRESS_DURATION 5
#define DKS_PRESS_BEGIN 0
#define DKS_PRESS_FULLY 4
#define DKS_RELEASE_BEGIN 8
#define DKS_RELEASE_FULLY 12
void dynamic_key_s_update(DynamicKey*dynamic_key, AdvancedKey*key)
{
    DynamicKeyStroke4x4*dynamic_key_s=(DynamicKeyStroke4x4*)dynamic_key;
    AnalogValue last_value = dynamic_key_s->value;
    AnalogValue current_value = key->value;
    uint8_t last_key_state = dynamic_key_s->key_state;
    if (current_value > last_value)
    {
        if (current_value >= dynamic_key_s->press_begin_distance &&
            last_value < dynamic_key_s->press_begin_distance)
        {
            for (int i = 0; i < 4; i++)
            {
                if (BIT_GET(dynamic_key_s->key_control[i], DKS_PRESS_BEGIN))
                {
                    if (BIT_GET(dynamic_key_s->key_control[i],DKS_PRESS_BEGIN+1))
                    {
                        dynamic_key_s->key_end_time[i] = 0xFFFFFFFF;
                    }
                    else
                    {
                        dynamic_key_s->key_end_time[i] = g_keyboard_tick + DKS_PRESS_DURATION;
                    }
                    BIT_SET(dynamic_key_s->key_state, i);
                }
                else
                {
                    BIT_RESET(dynamic_key_s->key_state, i);
                }
            }
        }
        if (current_value >= dynamic_key_s->press_fully_distance &&
            last_value < dynamic_key_s->press_fully_distance)
        {
            for (int i = 0; i < 4; i++)
            {
                if (BIT_GET(dynamic_key_s->key_control[i], DKS_PRESS_FULLY))
                {
                    if (BIT_GET(dynamic_key_s->key_control[i],DKS_PRESS_FULLY+1))
                    {
                        dynamic_key_s->key_end_time[i] = 0xFFFFFFFF;
                    }
                    else
                    {
                        dynamic_key_s->key_end_time[i] = g_keyboard_tick + DKS_PRESS_DURATION;
                    }
                    BIT_SET(dynamic_key_s->key_state, i);
                }
                else
                {
                    BIT_RESET(dynamic_key_s->key_state, i);
                }
            }
        }
    }
    if (current_value < last_value)
    {
        if (current_value <= dynamic_key_s->release_begin_distance &&
            last_value > dynamic_key_s->release_begin_distance)
        {
            for (int i = 0; i < 4; i++)
            {
                if (BIT_GET(dynamic_key_s->key_control[i], DKS_RELEASE_BEGIN))
                {
                    if (BIT_GET(dynamic_key_s->key_control[i],DKS_RELEASE_BEGIN+1))
                    {
                        dynamic_key_s->key_end_time[i] = 0xFFFFFFFF;
                    }
                    else
                    {
                        dynamic_key_s->key_end_time[i] = g_keyboard_tick + DKS_PRESS_DURATION;
                    }
                    BIT_SET(dynamic_key_s->key_state, i);
                }
                else
                {
                    BIT_RESET(dynamic_key_s->key_state, i);
                }
            }
        }
        if (current_value <= dynamic_key_s->release_fully_distance &&
            last_value > dynamic_key_s->release_fully_distance)
        {
            for (int i = 0; i < 4; i++)
            {
                if (BIT_GET(dynamic_key_s->key_control[i], DKS_RELEASE_FULLY))
                {
                    if (BIT_GET(dynamic_key_s->key_control[i],DKS_RELEASE_FULLY+1))
                    {
                        dynamic_key_s->key_end_time[i] = 0xFFFFFFFF;
                    }
                    else
                    {
                        dynamic_key_s->key_end_time[i] = g_keyboard_tick + DKS_PRESS_DURATION;
                    }
                    BIT_SET(dynamic_key_s->key_state, i);
                }
                else
                {
                    BIT_RESET(dynamic_key_s->key_state, i);
                }
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (g_keyboard_tick > dynamic_key_s->key_end_time[i])
        {
            BIT_RESET(dynamic_key_s->key_state, i);
        }
        if (BIT_GET(dynamic_key_s->key_state, i) && !BIT_GET(last_key_state, i))
        {
            keyboard_event_handler(MK_EVENT(dynamic_key_s->key_binding[i], KEYBOARD_EVENT_KEY_DOWN));
        }
        if (!BIT_GET(dynamic_key_s->key_state, i) && BIT_GET(last_key_state, i))
        {
            keyboard_event_handler(MK_EVENT(dynamic_key_s->key_binding[i], KEYBOARD_EVENT_KEY_DOWN));
        }
    }
    dynamic_key_s->value = current_value;
}

void dynamic_key_rs_update(DynamicKey*dynamic_key, AdvancedKey*key)
{
    DynamicKeyRappySnappy*dynamic_key_rs=(DynamicKeyRappySnappy*)dynamic_key;
    AdvancedKey*key1 = &g_keyboard_advanced_keys[command_advanced_key_mapping[dynamic_key_rs->key1_id]];
    AdvancedKey*key2 = &g_keyboard_advanced_keys[command_advanced_key_mapping[dynamic_key_rs->key2_id]];
    if (key->key.id == dynamic_key_rs->key1_id)
    {
        uint8_t last_key1_state = dynamic_key_rs->key1_state;
        if ((key1->value > key2->value) ||
        ((key1->value>= (ANALOG_VALUE_MAX - key1->lower_deadzone))&&
        (key2->value>= (ANALOG_VALUE_MAX - key2->lower_deadzone))))
        {
            dynamic_key_rs->key1_state = true;
        }
        else if (key1->value != key2->value || (key1->value < key1->upper_deadzone))
        {
            dynamic_key_rs->key1_state = false;
        }
        if (dynamic_key_rs->key1_state && !last_key1_state)
        {
            keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_DOWN));
        }
        if (!dynamic_key_rs->key1_state && last_key1_state)
        {
            keyboard_event_handler(MK_EVENT(dynamic_key_rs->key1_binding, KEYBOARD_EVENT_KEY_UP));
        }
    }
    if  (key->key.id == dynamic_key_rs->key2_id)
    {
        uint8_t last_key2_state = dynamic_key_rs->key2_state;
        if ((key1->value < key2->value) ||
        ((key1->value>= (ANALOG_VALUE_MAX - key1->lower_deadzone))&&
        (key2->value>= (ANALOG_VALUE_MAX - key2->lower_deadzone))))
        {
            dynamic_key_rs->key2_state = true;
        }
        else if (key1->value != key2->value || (key2->value > key2->upper_deadzone))
        {
            dynamic_key_rs->key2_state = false;
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
}