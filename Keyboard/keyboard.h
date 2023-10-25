/*
 * keyboard.h
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "lefl.h"
#include "usb_hid_keys.h"
#include "usbd_conf.h"

#define ADVANCED_KEY_NUM        64
#define KEY_NUM                 8

extern uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
extern lefl_bit_array_t Keyboard_KeyArray;
extern uint8_t Keyboard_EC11_Flag;
extern uint8_t Keyboard_Wheel_Flag;
extern bool Keyboard_SHIFT_Flag;
extern bool Keyboard_ALPHA_Flag;

extern uint16_t Keyboard_Advanced_SHIFT_IDs[ADVANCED_KEY_NUM];
extern uint16_t Keyboard_SHIFT_IDs[KEY_NUM];
extern uint16_t Keyboard_Advanced_ALPHA_IDs[ADVANCED_KEY_NUM];
extern uint16_t Keyboard_ALPHA_IDs[KEY_NUM];
extern uint8_t Keyboard_TargetAdvancedKey;

extern lefl_key_t Keyboard_Keys[KEY_NUM];
extern lefl_advanced_key_t Keyboard_AdvancedKeys[ADVANCED_KEY_NUM];

#define KEY_SHIFT               Keyboard_Keys[0]
#define KEY_ALPHA               Keyboard_Keys[1]
#define KEY_KNOB                Keyboard_Keys[2]
#define KEY_WHEEL               Keyboard_Keys[3]
#define KEY_KNOB_CLOCKWISE      Keyboard_Keys[4]
#define KEY_KNOB_ANTICLOCKWISE  Keyboard_Keys[5]
#define KEY_WHEEL_CLOCKWISE     Keyboard_Keys[6]
#define KEY_WHEEL_ANTICLOCKWISE Keyboard_Keys[7]


#define KEY_SHIFT_INDEX               0
#define KEY_ALPHA_INDEX               1
#define KEY_KNOB_INDEX                2
#define KEY_WHEEL_INDEX               3
#define KEY_KNOB_CLOCKWISE_INDEX      4
#define KEY_KNOB_ANTICLOCKWISE_INDEX  5
#define KEY_WHEEL_CLOCKWISE_INDEX     6
#define KEY_WHEEL_ANTICLOCKWISE_INDEX 7

void Keyboard_Init();
void Keyboard_ID_Recovery();
void Keyboard_Scan();
void Keyboard_SendReport();

#endif /* KEYBOARD_H_ */
