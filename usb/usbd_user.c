/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "usbd_user.h"
#include "command.h"
#include "keyboard.h"
#include "mouse.h"
#include "stm32f303xc.h"
#include "usb_descriptor.c"

typedef struct
{
    USB_Descriptor_Device_t device;
    USB_Descriptor_Configuration_t config;
    uint8_t strings[];
} HidDescriptor;

const HidDescriptor hid_descriptor_new =
    {
    .device = DeviceDescriptor,
    .config = ConfigurationDescriptor,
    .strings = {
    ///////////////////////////////////////
    /// string0 descriptor
    ///////////////////////////////////////
    USB_LANGID_INIT(USBD_LANGID_STRING),
    ///////////////////////////////////////
    /// string1 descriptor
    ///////////////////////////////////////
    0x28,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'd', 0x00,                  /* wcChar0 */
    'w', 0x00,                  /* wcChar1 */
    'd', 0x00,                  /* wcChar2 */
    'w', 0x00,                  /* wcChar3 */
    'd', 0x00,                  /* wcChar4 */
    'w', 0x00,                  /* wcChar5 */
    '5', 0x00,                  /* wcChar6 */
    '1', 0x00,                  /* wcChar7 */
    '6', 0x00,                  /* wcChar8 */
    '3', 0x00,                  /* wcChar9 */
    '&', 0x00,                  /* wcChar10 */
    'L', 0x00,                  /* wcChar11 */
    'z', 0x00,                  /* wcChar12 */
    'q', 0x00,                  /* wcChar13 */
    '1', 0x00,                  /* wcChar14 */
    '2', 0x00,                  /* wcChar15 */
    '3', 0x00,                  /* wcChar16 */
    '4', 0x00,                  /* wcChar17 */
    '5', 0x00,                  /* wcChar18 */
    ///////////////////////////////////////
    /// string2 descriptor
    ///////////////////////////////////////
    0x20,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    'O', 0x00,                  /* wcChar0 */
    'h', 0x00,                  /* wcChar1 */
    'o', 0x00,                  /* wcChar2 */
    'l', 0x00,                  /* wcChar3 */
    'e', 0x00,                  /* wcChar4 */
    'o', 0x00,                  /* wcChar5 */
    ' ', 0x00,                  /* wcChar6 */
    'K', 0x00,                  /* wcChar7 */
    'e', 0x00,                  /* wcChar8 */
    'y', 0x00,                  /* wcChar9 */
    'b', 0x00,                  /* wcChar10 */
    'o', 0x00,                  /* wcChar11 */
    'a', 0x00,                  /* wcChar12 */
    'r', 0x00,                  /* wcChar13 */
    'd', 0x00,                  /* wcChar14 */
    ///////////////////////////////////////
    /// string3 descriptor
    ///////////////////////////////////////
    0x16,                       /* bLength */
    USB_DESCRIPTOR_TYPE_STRING, /* bDescriptorType */
    '2', 0x00,                  /* wcChar0 */
    '0', 0x00,                  /* wcChar1 */
    '2', 0x00,                  /* wcChar2 */
    '2', 0x00,                  /* wcChar3 */
    '1', 0x00,                  /* wcChar4 */
    '2', 0x00,                  /* wcChar5 */
    '3', 0x00,                  /* wcChar6 */
    '4', 0x00,                  /* wcChar7 */
    '5', 0x00,                  /* wcChar8 */
    '6', 0x00,                  /* wcChar9 */
#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x01,
    0x00,
#endif
    0x00}
    };

enum
{
    HID_STATE_IDLE = 0,
    HID_STATE_BUSY
};

typedef struct __HIDBuffer
{
    uint8_t read_buffer[64];
    uint8_t send_buffer[64];
    uint8_t state;
} HIDBuffer;

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX HIDBuffer keyboard_buffer;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX HIDBuffer raw_buffer;
/*!< hid state ! Data can be sent only when state is idle  */

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    UNUSED(busid);
    switch (event)
    {
    case USBD_EVENT_RESET:
        keyboard_buffer.state = HID_STATE_IDLE;
        raw_buffer.state = HID_STATE_IDLE;
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
        memset(keyboard_buffer.read_buffer, 0, sizeof(keyboard_buffer.read_buffer));
        memset(raw_buffer.read_buffer, 0, sizeof(raw_buffer.read_buffer));
        usbd_ep_start_read(0, KEYBOARD_EPOUT_ADDR, keyboard_buffer.read_buffer, 64);
        usbd_ep_start_read(0, RAW_EPOUT_ADDR, raw_buffer.read_buffer, 64);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;
    default:
        break;
    }
}

static void usbd_hid_keyboard_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    keyboard_buffer.state = HID_STATE_IDLE;
}

static void usbd_hid_keyboard_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual out len:%ld\r\n", nbytes);
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    usbd_ep_start_read(0, KEYBOARD_EPOUT_ADDR, keyboard_buffer.read_buffer, 64);
    g_keyboard_led_state = keyboard_buffer.read_buffer[0];
}



static void usbd_hid_raw_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    raw_buffer.state = HID_STATE_IDLE;
}

static void usbd_hid_raw_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual out len:%ld\r\n", nbytes);
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    usbd_ep_start_read(0, RAW_EPOUT_ADDR, raw_buffer.read_buffer, 64);
    command_prase(raw_buffer.read_buffer, sizeof(raw_buffer.read_buffer));
}

static struct usbd_endpoint keyboard_in_ep = {
    .ep_cb = usbd_hid_keyboard_in_callback,
    .ep_addr = KEYBOARD_EPIN_ADDR};

static struct usbd_endpoint keyboard_out_ep = {
    .ep_cb = usbd_hid_keyboard_out_callback,
    .ep_addr = KEYBOARD_EPOUT_ADDR};

static struct usbd_endpoint raw_in_ep = {
    .ep_cb = usbd_hid_raw_in_callback,
    .ep_addr = RAW_EPIN_ADDR};

static struct usbd_endpoint raw_out_ep = {
    .ep_cb = usbd_hid_raw_out_callback,
    .ep_addr = RAW_EPOUT_ADDR};

struct usbd_interface intf0; // Custom
struct usbd_interface intf1; // Custom

void hid_init(void)
{
    usbd_desc_register(0, (uint8_t*)&hid_descriptor_new);
    usbd_add_interface(0, usbd_hid_init_intf(0, &intf0, KeyboardReport, sizeof(KeyboardReport)));
    usbd_add_endpoint(0, &keyboard_in_ep);
    usbd_add_endpoint(0, &keyboard_out_ep);
    usbd_add_interface(0, usbd_hid_init_intf(0, &intf1, RawReport, sizeof(RawReport)));
    usbd_add_endpoint(0, &raw_in_ep);
    usbd_add_endpoint(0, &raw_out_ep);
    usbd_initialize(0, USB_BASE, usbd_event_handler);
}

int hid_keyboard_send(uint8_t *buffer)
{
    if (keyboard_buffer.state == HID_STATE_BUSY)
    {
        return 1;
    }
    else
    {
    }
    memcpy(keyboard_buffer.send_buffer, buffer, 8);
    int ret = usbd_ep_start_write(0, KEYBOARD_EPIN_ADDR, keyboard_buffer.send_buffer, 8);
    if (ret < 0)
    {
        return 1;
    }
    keyboard_buffer.state = HID_STATE_BUSY;
    return 0;
}

int hid_mouse_send(uint8_t *buffer)
{
    //int ret = usbd_ep_start_write(0, KEYBOARD_EPIN_ADDR, send_buffer, 64);
    int ret = 0;
    if (ret < 0) {
        return 1;
    }
    return 0;
}

void hid_keyboard_test()
{
}

int hid_raw_send(uint8_t *buffer, int size)
{
    if (raw_buffer.state == HID_STATE_BUSY)
    {
        return 1;
    }
    else
    {
    }
    if (size > 0 && size <= 64)
    {
        memset(raw_buffer.send_buffer, 0, 64);
        memcpy(raw_buffer.send_buffer, buffer, size);
    }
    else
    {
        return 1;
    }
    int ret = usbd_ep_start_write(0, RAW_EPIN_ADDR, raw_buffer.send_buffer, 64);
    if (ret < 0)
    {
        return 1;
    }
    raw_buffer.state = HID_STATE_BUSY;
    return 0;
}
