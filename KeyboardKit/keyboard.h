/*
 * keyboard.h
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "key.h"
#include "advanced_key.h"
#include "usb_hid_keys.h"
#include "keyboard_conf.h"
#include "keyboard_def.h"

#define KEYBINDING(keycode,modifier) (((modifier)<<8)|(keycode))
#define KEY_KEYCODE(binding) ((binding)&0xFF)
#define KEY_MODIFIER(binding) (((binding)>>8)&0xFF)

typedef struct
{
    uint8_t buffer[8];
    uint8_t keynum;
}Keyboard_6KROBuffer;

extern Key g_keyboard_keys[KEY_NUM];
extern AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];
extern uint8_t g_keyboard_current_layer;
extern const uint16_t g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
extern uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

extern uint8_t g_keyboard_report_buffer[HID_BUFFER_LENGTH];

int keyboard_6KRObuffer_add(Keyboard_6KROBuffer*buf,uint16_t key);
void keyboard_6KRObuffer_send(Keyboard_6KROBuffer*buf);
void keyboard_6KRObuffer_clear(Keyboard_6KROBuffer*buf);
void keyboard_init();
void keyboard_factory_reset();
void keyboard_system_reset();
void keyboard_scan();
void keyboard_send_report();
void keyboard_post_process();
void keyboard_recovery();
void keyboard_save();
void keyboard_task();
void keyboard_delay();
void keyboard_hid_send(uint8_t *report, uint16_t len);

#endif /* KEYBOARD_H_ */
