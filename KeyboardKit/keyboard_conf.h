/*
 * keyboard_conf.h
 *
 *  Created on: Oct 29, 2023
 *      Author: xq123
 */

#ifndef KEYBOARD_CONF_H_
#define KEYBOARD_CONF_H_

/********************/
/* Keyboard General */
/********************/
#define LAYER_NUM               5
#define ADVANCED_KEY_NUM        64
#define KEY_NUM                 0
#define USE_FLOAT_VALUE
#define NKRO_ENABLE

/********************/
/* Keyboard Default */
/********************/
#define DEFAULT_ADVANCED_KEY_MODE   KEY_ANALOG_NORMAL_MODE
#define DEFAULT_TRIGGER_DISTANCE    0.08
#define DEFAULT_RELEASE_DISTANCE    0.08
#define DEFAULT_UPPER_DEADZONE      0.04
#define DEFAULT_LOWER_DEADZONE      0.2
#define DEFAULT_SCHMITT_PARAMETER   0.01

/**********/
/* Analog */
/**********/
#define RING_BUF_LEN            8

/*******/
/* RGB */
/*******/
#define ENABLE_RGB              
#define RGB_NUM                 (ADVANCED_KEY_NUM)
#define ONE_PULSE               (60)
#define ZERO_PULSE              (29)
#define NONE_PULSE              (0)
#define RGB_RESET_LENGTH        (400)
#define RGB_BUFFER_LENGTH       (RGB_RESET_LENGTH+3*8*(RGB_NUM))
#define RGB_MAX_DURATION        2000
#define FADING_DISTANCE         5.0f
#define JELLY_DISTANCE          10.0f
#define PORT_LOCATION           {1, 4.5}
#define RGB_FLASH_MAX_DURATION  1000
#define RGB_FLASH_RIPPLE_SPEED  0.03
#define RGB_DEFAULT_MODE        RGB_MODE_LINEAR
#define RGB_DEFAULT_SPEED       0.03
#define RGB_DEFAULT_COLOR_HSV   {273, 78, 99}

/**********/
/* Filter */
/**********/
//#define TOLERANCE               3
#define ENABLE_FILTER

/**********/
/* Record */
/**********/
//#define STATIC_RECORD
//#define ENABLE_ANALOG_HISTORY
//#define ENABLE_KPS
//#define ENABLE_KPS_HISTORY
//#define ENABLE_BIT_STREAM
//#define ENABLE_COUNTER

#define KPS_HISTORY_LENGTH      65
#define BIT_STREAM_LENGTH       128
#define ANALOG_HISTORY_LENGTH   129
#define RECORD_MAX_KEY_NUM      8
#define KPS_REFRESH_RATE        144

/****************/
/* User Defines */
/****************/
#define PULSE 1000
#define PULSE_LEN_MS 40

#endif /* KEYBOARD_CONF_H_ */
