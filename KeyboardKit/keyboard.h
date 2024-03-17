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

#define ADVANCED_KEY_NUM        64
#define KEY_NUM                 8

extern uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];

extern AdvancedKey Keyboard_AdvancedKeys[ADVANCED_KEY_NUM];


extern int16_t keymap[5][64];
extern const int16_t default_keymap[5][64];

void Keyboard_Init();
void Keyboard_FactoryReset();
void Keyboard_SystemReset();
void Keyboard_Scan();
void Keyboard_SendReport();
void Keyboard_Recovery();
void Keyboard_Save();

#endif /* KEYBOARD_H_ */
