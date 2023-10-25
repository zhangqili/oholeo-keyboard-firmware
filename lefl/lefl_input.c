/*
 * lefl_input.c
 */
#include "lefl.h"

void lefl_key_update(lefl_key_t* key,bool state)
{
    if((!(key->state))&&state)
    {
        if(key->key_cb[KEY_EVENT_DOWN])
            key->key_cb[KEY_EVENT_DOWN](key);
    }
    if((key->state)&&(!state))
    {
        if(key->key_cb[KEY_EVENT_UP])
            key->key_cb[KEY_EVENT_UP](key);
    }
    key->state=state;
}

void lefl_key_attach(lefl_key_t* key, KEY_EVENT e,lefl_key_cb_t cb)
{
    key->key_cb[e]=cb;
}

void lefl_advanced_key_update(lefl_advanced_key_t* advanced_key,float value)
{
    bool state = advanced_key->key.state;
    switch(advanced_key->mode)
    {
    case LEFL_KEY_DIGITAL_MODE:
        state=value;
        break;
    case LEFL_KEY_ANALOG_NORMAL_MODE:
        advanced_key->value=value;
        if(advanced_key->value-advanced_key->schmitt_parameter>advanced_key->trigger_distance)
        {
            state=true;
        }
        if(advanced_key->value+advanced_key->schmitt_parameter<advanced_key->trigger_distance)
        {
            state=false;
        }
        break;
    case LEFL_KEY_ANALOG_RAPID_MODE:
        advanced_key->value=value;
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
        if (advanced_key->value <= 0.0)
        {
            state = false;
            advanced_key->minimum = 0.0;
        }
        if (advanced_key->value >= 1.0)
        {
            state = true;
            advanced_key->maximum = 1.0;
        }
        break;
    case LEFL_KEY_ANALOG_SPEED_MODE:
        if(value-advanced_key->value > advanced_key->trigger_speed)
        {
            state=true;
        }
        if(value-advanced_key->value < advanced_key->release_speed)
        {
            state=false;
        }
        advanced_key->value=value;
        break;
    }
    lefl_key_update(&(advanced_key->key), state);
}

void lefl_advanced_key_update_raw(lefl_advanced_key_t* key, float value)
{
    key->raw=value;
    if(key->mode==LEFL_KEY_DIGITAL_MODE)
        lefl_advanced_key_update(key,value);
    else
        lefl_advanced_key_update(key,lefl_advanced_key_normalize(key, value));
}

void lefl_advanced_key_update_state(lefl_advanced_key_t* advanced_key, bool state)
{
    lefl_key_update(&(advanced_key->key), state);
}

float lefl_advanced_key_normalize(lefl_advanced_key_t* key, float value)
{
    float x;
    x=(key->upper_bound-(key->upper_bound - key->lower_bound)*key->upper_deadzone -value)/(key->range);
    return x;
}

void lefl_advanced_key_set_range(lefl_advanced_key_t* key,float upper,float lower)
{
    key->upper_bound=upper;
    key->lower_bound=lower;
    key->range = key->upper_bound - key->lower_bound;
}

void lefl_advanced_key_set_deadzone(lefl_advanced_key_t* key,float upper,float lower)
{
    /*
    key->upper_deadzone = (key->upper_bound - key->lower_bound)*upper;
    key->lower_deadzone = (key->upper_bound - key->lower_bound)*lower;
    key->range = (key->upper_bound - key->upper_deadzone) - (key->lower_bound+key->lower_deadzone);
    */
    key->upper_deadzone = upper;
    key->lower_deadzone = lower;
    key->range = (key->upper_bound - (key->upper_bound - key->lower_bound)*key->upper_deadzone) - (key->lower_bound+(key->upper_bound - key->lower_bound)*key->lower_deadzone);
}
