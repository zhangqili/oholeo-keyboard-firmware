#ifndef ADVANCED_KEY_H_
#define ADVANCED_KEY_H_

#include "key.h"
#include "keyboard_conf.h"

#ifdef USE_FLOAT_VALUE
typedef float KeyValueType;
#else
typedef uint16_t KeyValueType;
#endif

typedef enum
{
    KEY_DIGITAL_MODE,
    KEY_ANALOG_NORMAL_MODE,
    KEY_ANALOG_RAPID_MODE,
    KEY_ANALOG_SPEED_MODE
} KeyMode;
typedef enum
{
    KEY_NO_CALIBRATION,
    KEY_AUTO_CALIBRATION_POSITIVE,
    KEY_AUTO_CALIBRATION_NEGATIVE,
    KEY_AUTO_CALIBRATION_UNDEFINED,
} CalibrationMode;

typedef struct __AdvancedKey
{
    Key key;
    uint8_t mode;
    uint8_t calibration_mode;
    KeyValueType value;
    KeyValueType raw;
    
    KeyValueType activation_value;
    KeyValueType phantom_lower_deadzone;

    KeyValueType trigger_distance;
    KeyValueType release_distance;
    KeyValueType schmitt_parameter;
    KeyValueType maximum;
    KeyValueType minimum;
    KeyValueType trigger_speed;
    KeyValueType release_speed;
    KeyValueType upper_deadzone;
    KeyValueType lower_deadzone;
    KeyValueType upper_bound;
    KeyValueType lower_bound;
} AdvancedKey;

void advanced_key_init(AdvancedKey *key);
void advanced_key_update(AdvancedKey *key, KeyValueType value);
void advanced_key_update_raw(AdvancedKey *key, KeyValueType value);
void advanced_key_update_state(AdvancedKey *key, bool state);
KeyValueType advanced_key_normalize(AdvancedKey *key, KeyValueType value);
void advanced_key_set_range(AdvancedKey *key, KeyValueType upper, KeyValueType lower);
void advanced_key_reset_range(AdvancedKey* key, KeyValueType value);
void advanced_key_set_deadzone(AdvancedKey *key, KeyValueType upper, KeyValueType lower);

#endif