/*
 * key.c
 */
#include "key.h"
#include "keyboard_def.h"

void key_update(Key* key,bool state)
{
    if ((!(key->state)) && state)
    {
        if (key->key_cb[KEY_EVENT_DOWN])
            key->key_cb[KEY_EVENT_DOWN](key);
    }
    if ((key->state) && (!state))
    {
        if (key->key_cb[KEY_EVENT_UP])
            key->key_cb[KEY_EVENT_UP](key);
    }
    key->state = state;
    key->report_state = state;
}

void key_attach(Key* key, KEY_EVENT e, key_cb_t cb)
{
    key->key_cb[e] = cb;
}

void advanced_key_update(AdvancedKey* advanced_key, float value)
{
    bool state = advanced_key->key.state;
    switch (advanced_key->mode)
    {
        case KEY_DIGITAL_MODE:
            state = value;
            break;
        case KEY_ANALOG_NORMAL_MODE:
            advanced_key->value = value;
            if(advanced_key->value-advanced_key->schmitt_parameter>advanced_key->activation_value)
            {
                state=true;
            }
            if(advanced_key->value+advanced_key->schmitt_parameter<advanced_key->activation_value)
            {
                state=false;
            }
            break;
        case KEY_ANALOG_RAPID_MODE:
            advanced_key->value = value;
            if (advanced_key->value <= advanced_key->upper_deadzone)
            {
                state = false;
                advanced_key->minimum = advanced_key->value;
                break;
            }
            if (advanced_key->value >= 1.0 - advanced_key->lower_deadzone)
            {
                state = true;
                advanced_key->maximum = advanced_key->value;
                break;
            }
            if (advanced_key->key.state)
            {
                if (advanced_key->value - advanced_key->minimum >= advanced_key->trigger_distance)
                {
                    if (advanced_key->value > advanced_key->maximum)
                        advanced_key->maximum = advanced_key->value;
                }
                if (advanced_key->maximum - advanced_key->value >= advanced_key->release_distance)
                {
                    advanced_key->minimum = advanced_key->value;
                    state = false;
                }
            }
            else
            {
                if (advanced_key->value - advanced_key->minimum >= advanced_key->trigger_distance)
                {
                    advanced_key->maximum = advanced_key->value;
                    state = true;
                }
                if (advanced_key->maximum - advanced_key->value >= advanced_key->release_distance)
                {
                    if (advanced_key->value < advanced_key->minimum)
                        advanced_key->minimum = advanced_key->value;
                }
            }
            break;
        case KEY_ANALOG_SPEED_MODE:
            if (value - advanced_key->value > advanced_key->trigger_speed)
            {
                state = true;
            }
            if (value - advanced_key->value < advanced_key->release_speed)
            {
                state = false;
            }
            advanced_key->value = value;
            break;
    }
    key_update(&(advanced_key->key), state);
}

void advanced_key_update_raw(AdvancedKey* key, float value)
{
    key->raw = value;
    if (key->mode == KEY_DIGITAL_MODE)
        advanced_key_update(key, value);
    else
        advanced_key_update(key, advanced_key_normalize(key, value));
}

void advanced_key_update_state(AdvancedKey* advanced_key, bool state)
{
    key_update(&(advanced_key->key), state);
}

__WEAK float advanced_key_normalize(AdvancedKey* key, float value)
{
    return (key->upper_bound - value) / (key->upper_bound - key->lower_bound);
}

void advanced_key_set_range(AdvancedKey* key, float upper, float lower)
{
    key->upper_bound = upper;
    key->lower_bound = lower;
    key->range = key->upper_bound - key->lower_bound;
}

void advanced_key_set_deadzone(AdvancedKey* key, float upper, float lower)
{
    /*
    key->upper_deadzone = (key->upper_bound - key->lower_bound)*upper;
    key->lower_deadzone = (key->upper_bound - key->lower_bound)*lower;
    key->range = (key->upper_bound - key->upper_deadzone) - (key->lower_bound+key->lower_deadzone);
    */
    key->upper_deadzone = upper;
    key->lower_deadzone = lower;
    key->range = (key->upper_bound - (key->upper_bound - key->lower_bound) * key->upper_deadzone) - (
                     key->lower_bound + (key->upper_bound - key->lower_bound) * key->lower_deadzone);
}
