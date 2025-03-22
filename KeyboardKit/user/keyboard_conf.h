/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KEYBOARD_CONF_H_
#define KEYBOARD_CONF_H_

/********************/
/* Keyboard General */
/********************/
#define LAYER_NUM               5
#define ADVANCED_KEY_NUM        64
#define KEY_NUM                 0
//#define ENABLE_FIXED_POINT_EXPERIMENTAL
//#define CONTINOUS_POLL
#define OPTIMIZE_FOR_FLOAT_DIVISION
//#define CONTINOUS_DEBUG
#define DEBUG_INTERVAL 1

/********************/
/* Keyboard Default */
/********************/
#define DEFAULT_ADVANCED_KEY_MODE   KEY_ANALOG_NORMAL_MODE
#define DEFAULT_CALIBRATION_MODE    KEY_AUTO_CALIBRATION_UNDEFINED
#define DEFAULT_TRIGGER_DISTANCE    0.08
#define DEFAULT_RELEASE_DISTANCE    0.08
#define DEFAULT_UPPER_DEADZONE      0.00
#define DEFAULT_LOWER_DEADZONE      0.2
#define DEFAULT_ACTIVATION_VALUE    0.5
#define DEFAULT_DEACTIVATION_VALUE  0.49
#define DEFAULT_ESTIMATED_RANGE     500

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
#define RGB_LEFT                0.0f
#define RGB_TOP                 4.5f
#define RGB_RIGHT               15.0f
#define RGB_BOTTOM              -0.5f
#define RGB_USE_LIST_EXPERIMENTAL

/************/
/* Joystick */
/************/
#define JOYSTICK_BUTTON_COUNT 32
#define JOYSTICK_AXIS_COUNT 6

/********/
/* MIDI */
/********/
#define MIDI_REF_VELOCITY 0.01

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

/*******/
/* USB */
/*******/
#define USB_POLLING_INTERVAL_MS 1
#define FIXED_CONTROL_ENDPOINT_SIZE 0x40
#define VENDOR_ID 0xFEED
#define PRODUCT_ID 22319
#define DEVICE_VER 2
#define FIXED_NUM_CONFIGURATIONS 1

#define MANUFACTURER "dwdwdw5163&Lzq12345"
#define PRODUCT "Oholeo Keyboard"
#define SERIAL_NUMBER "2022123456"

#define RAW_ENABLE
//#define KEYBOARD_SHARED_EP
#define SHARED_EP_ENABLE
#define NKRO_ENABLE
#define MOUSE_ENABLE
#define MOUSE_SHARED_EP
#define EXTRAKEY_ENABLE
#define JOYSTICK_ENABLE
#define JOYSTICK_SHARED_EP
//#define DIGITIZER_ENABLE
//#define DIGITIZER_SHARED_EP
//#define PROGRAMMABLE_BUTTON_ENABLE
#define MIDI_ENABLE

/****************/
/* User Defines */
/****************/
#define PULSE 1000
#define PULSE_LEN_MS 40

enum USER_KEYCODE {
    USER_BEEP = 0,
    USER_EM = 1,
    USER_SNAKE_LAUNCH = 0x10,
    USER_SNAKE_QUIT = 0x11,
    USER_SNAKE_PAUSE = 0x12,
    USER_SNAKE_SPEED_UP = 0x13,
    USER_SNAKE_SPEED_DOWN = 0x14,
    USER_SNAKE_RESTART = 0x15,
    USER_SNAKE_LEFT = 0x18,
    USER_SNAKE_UP = 0x19,
    USER_SNAKE_RIGHT = 0x1A,
    USER_SNAKE_DOWN = 0x1B,

    USER_RESET = 0xFF,
};

#endif /* KEYBOARD_CONF_H_ */
