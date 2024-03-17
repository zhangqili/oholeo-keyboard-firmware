/*
 * keyboard.h
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "key.h"
#include "usb_hid_keys.h"
#include "usbd_conf.h"
#include "keyboard_conf.h"


extern uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];

extern AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];


extern uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
extern const uint16_t g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

void Keyboard_Init();
void Keyboard_FactoryReset();
void Keyboard_SystemReset();
void Keyboard_Scan();
void Keyboard_SendReport();
void Keyboard_Recovery();
void Keyboard_Save();

#endif /* KEYBOARD_H_ */
