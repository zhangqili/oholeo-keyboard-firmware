/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "key.h"
#include "advanced_key.h"
#include "keyboard_conf.h"
#include "keyboard_def.h"
#include "layer.h"
#include "keycode.h"
#include "dynamic_key.h"

#define NKRO_REPORT_BITS 30

#define KEYBINDING(keycode, modifier) (((modifier) << 8) | (keycode))
#define KEYCODE(binding) ((binding) & 0xFF)
#define MODIFIER(binding) (((binding) >> 8) & 0xFF)

typedef struct
{
    uint8_t modifier;
    uint8_t reserved;
    uint8_t buffer[6];
    uint8_t keynum;
} __PACKED Keyboard_6KROBuffer;

typedef struct
{
    uint8_t modifier;
    uint8_t buffer[NKRO_REPORT_BITS];
} __PACKED Keyboard_NKROBuffer;

typedef enum
{
    KEYBOARD_EVENT_KEY_UP,
    KEYBOARD_EVENT_KEY_DOWN,
    KEYBOARD_EVENT_KEY_TRUE,
    KEYBOARD_EVENT_KEY_FALSE,
    KEYBOARD_EVENT_NUM
} KeyboardEventType;

typedef struct
{
    Keycode keycode;
    uint8_t event;
} KeyboardEvent;
#define MK_EVENT(keycode, event) ((KeyboardEvent){(keycode), (event)})

typedef enum
{
    KEYBOARD_IDLE,
    KEYBOARD_DEBUG,
    KEYBOARD_UPLOAD_CONFIG
} KEYBOARD_STATE;

enum KEYBOARD_REPORT_FLAG
{
    KEYBOARD_REPORT_FLAG = 0,
    MOUSE_REPORT_FLAG = 1,
    CONSUMER_REPORT_FLAG = 2,
    SYSTEM_REPORT_FLAG = 3,
    JOYSTICK_REPORT_FLAG = 4,
};


extern Key g_keyboard_keys[KEY_NUM];
extern AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];
extern const Keycode g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
extern Keycode g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
extern const uint8_t g_keyboard_advanced_keys_inverse_mapping[ADVANCED_KEY_NUM];

extern DynamicKey g_keyboard_dynamic_keys[DYNAMIC_KEY_NUM];

extern uint8_t g_keyboard_led_state;

extern uint32_t g_keyboard_tick;

#ifdef NKRO_ENABLE
extern bool g_keyboard_nkro_enable;
#endif

extern uint8_t g_keyboard_knob_flag;
extern volatile bool g_keyboard_send_report_enable;

extern KEYBOARD_STATE g_keyboard_state;
extern volatile uint8_t g_keyboard_send_flag;

extern uint8_t g_current_config_index;

void keyboard_event_handler(KeyboardEvent event);
void keyboard_advanced_key_event_handler(AdvancedKey*key, KeyboardEvent event);

void keyboard_add_buffer(Keycode keycode);

int keyboard_buffer_send(void);
void keyboard_buffer_clear(void);

int keyboard_6KRObuffer_add(Keyboard_6KROBuffer *buf, Keycode keycode);
int keyboard_6KRObuffer_send(Keyboard_6KROBuffer *buf);
void keyboard_6KRObuffer_clear(Keyboard_6KROBuffer *buf);

int keyboard_NKRObuffer_add(Keyboard_NKROBuffer*buf,Keycode keycode);
int keyboard_NKRObuffer_send(Keyboard_NKROBuffer*buf);
void keyboard_NKRObuffer_clear(Keyboard_NKROBuffer*buf);


void keyboard_key_update(Key *key, bool state);
void keyboard_advanced_key_update_state(AdvancedKey *key, bool state);

void keyboard_init(void);
void keyboard_reboot(void);
void keyboard_reset_to_default(void);
void keyboard_factory_reset(void);
void keyboard_jump_to_bootloader(void);
void keyboard_user_handler(uint8_t code);
void keyboard_scan(void);
void keyboard_send_report(void);
void keyboard_post_process(void);
void keyboard_recovery(void);
void keyboard_save(void);
void keyboard_set_config_index(uint8_t index);
void keyboard_task(void);
void keyboard_delay(uint32_t ms);
int keyboard_extra_hid_send(uint8_t report_id, uint16_t usage);
int keyboard_hid_send(uint8_t *report, uint16_t len);

#endif /* KEYBOARD_H_ */
