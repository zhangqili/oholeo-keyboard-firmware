/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "advanced_key.h"
#include "keyboard.h"
#include "keyboard_def.h"

void advanced_key_update(AdvancedKey* advanced_key, AnalogValue value)
{
    advanced_key->difference = value - advanced_key->value;
    bool state = advanced_key->key.state;
    switch (advanced_key->config.mode)
    {
        case KEY_DIGITAL_MODE:
            state = (bool)value;
            break;
        case KEY_ANALOG_NORMAL_MODE:
            advanced_key->value = value;
            if((advanced_key->value - ANALOG_VALUE_MIN) > advanced_key->config.activation_value)
            {
                state = true;
            }
            if((advanced_key->value - ANALOG_VALUE_MIN) < advanced_key->config.deactivation_value)
            {
                state = false;
            }
            break;
        case KEY_ANALOG_RAPID_MODE:
            advanced_key->value = value;
            if ((advanced_key->value - ANALOG_VALUE_MIN) <= advanced_key->config.upper_deadzone)
            {
                state = false;
                advanced_key->extremum = advanced_key->value;
                break;
            }
            if (advanced_key->value >= ANALOG_VALUE_MAX - advanced_key->config.lower_deadzone)
            {
                state = true;
                advanced_key->extremum = advanced_key->value;
                break;
            }
            if (advanced_key->key.state && advanced_key->extremum - advanced_key->value >= advanced_key->config.release_distance)
            {
                state =false;
                advanced_key->extremum = advanced_key->value;
            }
            if (!advanced_key->key.state && advanced_key->value - advanced_key->extremum >= advanced_key->config.trigger_distance)
            {
                state = true;
                advanced_key->extremum = advanced_key->value;
            }
            if ((advanced_key->key.state && advanced_key->value > advanced_key->extremum) ||
                (!advanced_key->key.state && advanced_key->value < advanced_key->extremum))
            {
                advanced_key->extremum = advanced_key->value;
            }
            break;
        case KEY_ANALOG_SPEED_MODE:
            if (advanced_key->difference > advanced_key->config.trigger_speed)
            {
                state = true;
            }
            if (advanced_key->difference < advanced_key->config.release_speed)
            {
                state = false;
            }
            advanced_key->value = value;
            break;
        default:
            break;
    }
    keyboard_advanced_key_update_state(advanced_key, state);
}

void advanced_key_update_raw(AdvancedKey* advanced_key, AnalogRawValue value)
{
    advanced_key->raw = value;
    switch (advanced_key->config.calibration_mode)
    {
    case KEY_AUTO_CALIBRATION_POSITIVE:
        if (value > advanced_key->config.lower_bound)
            advanced_key_set_range(advanced_key, advanced_key->config.upper_bound, value);
        break;
    case KEY_AUTO_CALIBRATION_NEGATIVE:
        if (value < advanced_key->config.lower_bound)
            advanced_key_set_range(advanced_key, advanced_key->config.upper_bound, value);
        break;
    case KEY_AUTO_CALIBRATION_UNDEFINED:
        if (value - advanced_key->config.upper_bound > DEFAULT_ESTIMATED_RANGE)
        {
            advanced_key->config.calibration_mode = KEY_AUTO_CALIBRATION_POSITIVE;
            advanced_key_set_range(advanced_key, advanced_key->config.upper_bound, value);
            break;
        }
        if (advanced_key->config.upper_bound - value > DEFAULT_ESTIMATED_RANGE)
        {
            advanced_key->config.calibration_mode = KEY_AUTO_CALIBRATION_NEGATIVE;
            advanced_key_set_range(advanced_key, advanced_key->config.upper_bound, value);
            break;
        }
        advanced_key_update(advanced_key, ANALOG_VALUE_MIN);
        return;
    default:
        break;
    }
    if (advanced_key->config.mode == KEY_DIGITAL_MODE)
        advanced_key_update(advanced_key, value);
    else
        advanced_key_update(advanced_key, advanced_key_normalize(advanced_key, value));
}

void advanced_key_update_state(AdvancedKey* advanced_key, bool state)
{
    key_update(&(advanced_key->key), state);
}

__WEAK AnalogValue advanced_key_normalize(AdvancedKey* advanced_key, AnalogRawValue value)
{
#ifdef OPTIMIZE_FOR_FLOAT_DIVISION
    return  ANALOG_VALUE_MIN + ANALOG_VALUE_RANGE * (advanced_key->config.upper_bound - value) * advanced_key->range_reciprocal;
#else
    return  ANALOG_VALUE_MIN + ANALOG_VALUE_RANGE * (advanced_key->config.upper_bound - value) / (advanced_key->config.upper_bound - advanced_key->config.lower_bound);
#endif
}

void advanced_key_set_range(AdvancedKey* advanced_key, AnalogRawValue upper, AnalogRawValue lower)
{
    advanced_key->config.upper_bound = upper;
    advanced_key->config.lower_bound = lower;
#ifdef OPTIMIZE_FOR_FLOAT_DIVISION
    advanced_key->range_reciprocal = 1.0f/(advanced_key->config.upper_bound - advanced_key->config.lower_bound);
#endif
}

void advanced_key_reset_range(AdvancedKey* advanced_key, AnalogRawValue value)
{
    switch (advanced_key->config.calibration_mode)
    {
    case KEY_AUTO_CALIBRATION_POSITIVE:
        advanced_key_set_range(advanced_key, value, value+DEFAULT_ESTIMATED_RANGE);
        break;
    case KEY_AUTO_CALIBRATION_NEGATIVE:
        advanced_key_set_range(advanced_key, value, value-DEFAULT_ESTIMATED_RANGE);
        break;
    default:
        advanced_key_set_range(advanced_key, value, value-DEFAULT_ESTIMATED_RANGE);
        break;
    }
}

void advanced_key_set_deadzone(AdvancedKey* advanced_key, AnalogValue upper, AnalogValue lower)
{
    advanced_key->config.upper_deadzone = upper;
    advanced_key->config.lower_deadzone = lower;
}

