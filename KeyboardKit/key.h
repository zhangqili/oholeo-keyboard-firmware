#ifndef KEY_H_
#define KEY_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"

typedef enum
{
    KEY_EVENT_UP,
    KEY_EVENT_DOWN,
    EVENT_NUM
} KEY_EVENT;
typedef void (*key_cb_t)(void *);
typedef struct __Key
{
    uint16_t id;
    bool state;
    key_cb_t key_cb[EVENT_NUM];
} Key;
void key_update(Key *key, bool state);
void key_attach(Key *key, KEY_EVENT e, key_cb_t cb);

typedef enum
{
    KEY_DIGITAL_MODE,
    KEY_ANALOG_NORMAL_MODE,
    KEY_ANALOG_RAPID_MODE,
    KEY_ANALOG_SPEED_MODE
} KeyMode;

typedef struct __AdvancedKey
{
    Key key;
    KeyMode mode;
    float value;
    float raw;

    float activation_value;
    float phantom_lower_deadzone;

    float trigger_distance;
    float release_distance;
    float schmitt_parameter;
    float maximum;
    float minimum;
    float trigger_speed;
    float release_speed;
    float upper_deadzone;
    float lower_deadzone;
    float range;
    float upper_bound;
    float lower_bound;
} AdvancedKey;

void advanced_key_init(AdvancedKey *key);
void advanced_key_update(AdvancedKey *key, float value);
void advanced_key_update_raw(AdvancedKey *key, float value);
void advanced_key_update_state(AdvancedKey *key, bool state);
float advanced_key_normalize(AdvancedKey *key, float value);
void advanced_key_set_range(AdvancedKey *key, float upper, float lower);
void advanced_key_set_deadzone(AdvancedKey *key, float upper, float lower);
#endif