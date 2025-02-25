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

void usb_init(void);
int hid_keyboard_send(uint8_t *buffer, uint8_t size);
int hid_mouse_send(uint8_t*buffer);
int hid_raw_send(uint8_t*buffer,int size);
void hid_keyboard_test(void);
void hid_mouse_test(void);

#ifdef __cplusplus
}
#endif

#endif
