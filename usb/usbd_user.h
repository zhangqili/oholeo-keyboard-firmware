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

#define USBD_VID           1156
#define USBD_PID           22319
#define USBD_MAX_POWER     300
#define USBD_LANGID_STRING 1033

#define CUSTOM_HID_EPIN_ADDR                 (ENDPOINT_DIR_IN | SHARED_IN_EPNUM)
#define CUSTOM_HID_EPIN_SIZE                 0x40U

#define CUSTOM_HID_EPOUT_ADDR                0x01U
#define CUSTOM_HID_EPOUT_SIZE                0x40U

#define USB_CUSTOM_HID_CONFIG_DESC_SIZ       41U
#define USB_CUSTOM_HID_DESC_SIZ              9U

#define CUSTOM_HID_REPORT_DESC_SIZE 63

#define USB_HID_CONFIG_DESC_SIZ (9+32)

#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE     64U
/*---------- -----------*/
#define USBD_CUSTOM_HID_REPORT_DESC_SIZE     (89U+54U)
/*---------- -----------*/
#define CUSTOM_HID_FS_BINTERVAL     0x1U

extern const uint8_t hid_descriptor[];
extern const uint8_t custom_hid_report_desc[USBD_CUSTOM_HID_REPORT_DESC_SIZE];

extern uint32_t g_usb_report_count;
extern uint32_t g_usb_report_count1;

extern uint32_t g_usb_keyboard_interval;

void hid_init(void);
int hid_keyboard_send(uint8_t*buffer);
int hid_mouse_send(uint8_t*buffer);
int hid_raw_send(uint8_t*buffer,int size);
void hid_keyboard_test(void);
void hid_mouse_test(void);

#ifdef __cplusplus
}
#endif

#endif
