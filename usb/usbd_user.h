/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef __HID_KEYBOARD_H
#define __HID_KEYBOARD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_core.h"
#include "usbd_hid.h"

#define USBD_LANGID_STRING 1033

typedef struct __USBBuffer
{
    uint8_t read_buffer[64];
    uint8_t send_buffer[64];
    uint8_t state;
} USBBuffer;

void usb_init(void);
int hid_keyboard_send(uint8_t *buffer, uint8_t size);
int hid_mouse_send(uint8_t*buffer);
int hid_raw_send(uint8_t*buffer,int size);
int hid_extra_send(uint8_t report_id, uint16_t usage);
int hid_joystick_send(uint8_t *buffer, int size);
int usb_midi_send(uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif
