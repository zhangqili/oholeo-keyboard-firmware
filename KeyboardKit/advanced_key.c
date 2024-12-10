#include "advanced_key.h"
#include "keyboard_def.h"

void advanced_key_update(AdvancedKey* advanced_key, AnalogValue value)
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

void advanced_key_update_raw(AdvancedKey* key, AnalogValue value)
{
    key->raw = value;
    switch (key->calibration_mode)
    {
    case KEY_AUTO_CALIBRATION_POSITIVE:
        if (value > key->lower_bound)
            key->lower_bound = value;
        break;
    case KEY_AUTO_CALIBRATION_NEGATIVE:
        if (value < key->lower_bound)
            key->lower_bound = value;
        break;
    case KEY_AUTO_CALIBRATION_UNDEFINED:
        if (value - key->upper_bound > 500)
        {
            key->calibration_mode = KEY_AUTO_CALIBRATION_POSITIVE;
            key->lower_bound = value;
            break;
        }
        if (key->upper_bound - value > 500)
        {
            key->calibration_mode = KEY_AUTO_CALIBRATION_NEGATIVE;
            key->lower_bound = value;
            break;
        }
        return;
    default:
        break;
    }
    if (key->mode == KEY_DIGITAL_MODE)
        advanced_key_update(key, value);
    else
        advanced_key_update(key, advanced_key_normalize(key, value));
}

void advanced_key_update_state(AdvancedKey* advanced_key, bool state)
{
    key_update(&(advanced_key->key), state);
}

__WEAK AnalogValue advanced_key_normalize(AdvancedKey* key, AnalogValue value)
{
    return (key->upper_bound - value) / (key->upper_bound - key->lower_bound);
}

void advanced_key_set_range(AdvancedKey* key, AnalogValue upper, AnalogValue lower)
{
    key->upper_bound = upper;
    key->lower_bound = lower;
    //key->range = key->upper_bound - key->lower_bound;
}

void advanced_key_reset_range(AdvancedKey* key, AnalogValue value)
{
    switch (key->calibration_mode)
    {
    case KEY_AUTO_CALIBRATION_POSITIVE:
        advanced_key_set_range(key, value, value+500);
        break;
    case KEY_AUTO_CALIBRATION_NEGATIVE:
        advanced_key_set_range(key, value, value-500);
        break;
    default:
        advanced_key_set_range(key, value, value-500);
        break;
    }
}

void advanced_key_set_deadzone(AdvancedKey* key, AnalogValue upper, AnalogValue lower)
{
    /*
    key->upper_deadzone = (key->upper_bound - key->lower_bound)*upper;
    key->lower_deadzone = (key->upper_bound - key->lower_bound)*lower;
    key->range = (key->upper_bound - key->upper_deadzone) - (key->lower_bound+key->lower_deadzone);
    */
    key->upper_deadzone = upper;
    key->lower_deadzone = lower;
    //key->range = (key->upper_bound - (key->upper_bound - key->lower_bound) * key->upper_deadzone) - (
    //                 key->lower_bound + (key->upper_bound - key->lower_bound) * key->lower_deadzone);
}

