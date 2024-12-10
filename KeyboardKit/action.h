#ifndef ACTION_H_
#define ACTION_H_

#include "stdint.h"
#include "stddef.h"
#include "stdbool.h"
#include "key.h"
#include "advanced_key.h"

typedef enum __Action
{
    ACTION_DYNAMIC_KEYSTROKE,
    ACTION_MOD_TAP,
    ACTION_TOGGLE_KEY,
    ACTION_RAPPY_SNAPPY,
    ACTION_NUM
} Action;
void action_execute(Key *key, Action *action);

typedef struct __ActionDynamicKeyStroke4x4
{
    Action type;
    uint16_t key_binding[4];
    AnalogValue distance[4];
} ActionDynamicKeyStroke4x4;

typedef struct __ActionModTap
{
    Action type;
    uint16_t times;
} ActionModTap;

typedef struct __ActionToggleKey
{
    Action type;
    bool state;
} ActionToggleKey;

typedef struct __attribute__((packed))  __ActionRappySnappy
{
    Action type;
    uint16_t key1_id;
    uint16_t key2_id;
} ActionRappySnappy;

void action_dks_execute(Key*key, Action*action);
void action_mt_execute(Key*key, Action*action);
void action_tk_execute(Key*key, Action*action);
void action_rs_execute(Key*key, Action*action);

#endif