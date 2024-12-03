/*
 * keyboard_conf.h
 *
 *  Created on: Oct 29, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_CONF_H_
#define KEYBOARD_CONF_H_

#include "usbd_conf.h"

//#define EXTENDED_SAMPLING

#define ENABLE_FILTER

//#define STATIC_RECORD

//#define ENABLE_ANALOG_HISTORY

//#define ENABLE_KPS

//#define ENABLE_KPS_HISTORY

//#define ENABLE_BIT_STREAM

//#define ENABLE_COUNTER
//#define PAUSE_TIM_WHEN_CALCULATING

#define TIM2_COUNTER_PERIOD 2400*2-1

#define ADC12_BEGIN_TIME 800*2

#define ADC34_BEGIN_TIME 1600*2
#define DEFAULT_ADVANCED_KEY_MODE KEY_ANALOG_NORMAL_MODE
//#define DEFAULT_ADVANCED_KEY_MODE KEY_ANALOG_NORMAL_MODE

#define DEFAULT_TRIGGER_DISTANCE 0.08

#define DEFAULT_RELEASE_DISTANCE 0.08

#define DEFAULT_UPPER_DEADZONE 0.04

#define DEFAULT_LOWER_DEADZONE 0.2

#define DEFAULT_SCHMITT_PARAMETER 0.01

#define USE_FLOAT_VALUE

#define PULSE 1000
#define PULSE_LEN_MS 40

#define LAYER_NUM               5
#define ADVANCED_KEY_NUM        64
#define KEY_NUM                 0

#define HID_BUFFER_LENGTH       USBD_CUSTOMHID_OUTREPORT_BUF_SIZE

#define GPIO_READ(x)          GPIO_ReadInputDataBit(x##_GPIO_Port,x##_Pin)
#define K1_READ               !GPIO_READ(K1)
#define K2_READ               !GPIO_READ(K2)
#define K3_READ               !GPIO_READ(K3)
#define K4_READ               !GPIO_READ(K4)
#define FN_K1_READ            !GPIO_READ(FN_K1)
#define FN_K2_READ            !GPIO_READ(FN_K2)
#define FN_K3_READ            !GPIO_READ(FN_K3)
#define FN_K4_READ            !GPIO_READ(FN_K4)
#define FN_K5_READ            !GPIO_READ(FN_K5)
#define FN_K6_READ            GPIO_READ(FN_K6)
#define KNOB_READ             GPIO_READ(KNOB)


#define KEY_FN_K1               g_keyboard_keys[0]
#define KEY_FN_K2               g_keyboard_keys[1]
#define KEY_FN_K3               g_keyboard_keys[2]
#define KEY_FN_K4               g_keyboard_keys[3]
#define KEY_FN_K5               g_keyboard_keys[4]
#define KEY_FN_K6               g_keyboard_keys[5]
#define KEY_KNOB                g_keyboard_keys[6]
#define KEY_KNOB_CLOCKWISE      g_keyboard_keys[7]
#define KEY_KNOB_ANTICLOCKWISE  g_keyboard_keys[8]

#define KEY_FN_K1_INDEX               0
#define KEY_FN_K2_INDEX               1
#define KEY_FN_K3_INDEX               2
#define KEY_FN_K4_INDEX               3
#define KEY_FN_K5_INDEX               4
#define KEY_FN_K6_INDEX               5
#define KEY_KNOB_INDEX                6
#define KEY_KNOB_CLOCKWISE_INDEX      7
#define KEY_KNOB_ANTICLOCKWISE_INDEX  8


#endif /* KEYBOARD_CONF_H_ */
