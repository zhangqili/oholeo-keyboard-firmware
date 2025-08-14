/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "usbd_user.h"
#include "command.h"
#include "stm32f303xc.h"
#include "usb_descriptor.h"
#include "lamp_array.h"

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    UNUSED(speed);

    return (const uint8_t *)&DeviceDescriptor;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    UNUSED(speed);

    return (const uint8_t *)&ConfigurationDescriptor;
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    UNUSED(speed);

#ifdef CONFIG_USB_HS
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    static const uint8_t device_quality_descriptor[] = {0x0a,
        USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
        0x00,
        0x02,
        0x00,
        0x00,
        0x00,
        0x40,
        0x01,
        0x00};
    return device_quality_descriptor; 
#else
    return NULL;
#endif
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    UNUSED(speed);

    return NULL;
}

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    MANUFACTURER,                    /* Manufacturer */
    PRODUCT,           /* Product */
    SERIAL_NUMBER,                 /* Serial Number */
};

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(string_descriptors) / sizeof(char *))) {
        return NULL;
    }
    return string_descriptors[index];
}

const struct usb_descriptor usb_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

void usbd_hid_get_report(uint8_t busid, uint8_t intf, uint8_t report_id, uint8_t report_type, uint8_t **data, uint32_t *len)
{
    UNUSED(busid);
    UNUSED(intf);
    UNUSED(report_id);
    UNUSED(report_type);
    UNUSED(data);
    UNUSED(len);
    switch (intf)
    {
#ifdef LIGHTING_ENABLE
    case SHARED_INTERFACE:
        switch (report_id)
        {
        case REPORT_ID_LIGHTING_LAMP_ARRAY_ATTRIBUTES:
            {
                *len = lamp_array_get_lamp_array_attributes_report(*data);
            }
            break;
        case REPORT_ID_LIGHTING_LAMP_ATTRIBUTES_RESPONSE:
            {
                *len = lamp_array_get_lamp_attributes_report(*data);
            }
            break;
        default:
            (*data[0]) = 0;
            *len = 1;
            break;
        }
        break;
#endif
    default:
        (*data[0]) = 0;
        *len = 1;
        break;
    }
}

void usbd_hid_set_report(uint8_t busid, uint8_t intf, uint8_t report_id, uint8_t report_type, uint8_t *report, uint32_t report_len)
{
    UNUSED(busid);
    UNUSED(report_type);
    if (intf == KEYBOARD_INTERFACE
#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
    || intf == SHARED_INTERFACE
#endif
    )
    {
        if (report_len == 2)
        {
            if (report_id == REPORT_ID_KEYBOARD || report_id == REPORT_ID_NKRO) {
                g_keyboard_led_state = report[1];
            }
        }
        else if (report_len == 1)
        {
            g_keyboard_led_state = report[0];
        }
    }
#ifdef LIGHTING_ENABLE
    if (intf == SHARED_INTERFACE)
    {
        switch (report_id) {
            case REPORT_ID_LIGHTING_LAMP_ATTRIBUTES_REQUEST: {
                lamp_array_set_lamp_attributes_id(report);
                break;
            }
            case REPORT_ID_LIGHTING_LAMP_MULTI_UPDATE: {
                lamp_array_set_multiple_lamps(report);
                break;   
            }
            case REPORT_ID_LIGHTING_LAMP_RANGE_UPDATE: {
                lamp_array_set_lamp_range(report);
                break;   
            }
            case REPORT_ID_LIGHTING_LAMP_ARRAY_CONTROL: {
                lamp_array_set_autonomous_mode(report);
                break;
            }
            default: {
                break;
            }
        }
    }
#endif
}

/* Store example melody as an array of note values */
    
#ifndef KEYBOARD_SHARED_EP
static bool keyboard_state;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t keyboard_buffer[KEYBOARD_EPSIZE];

static void usbd_hid_keyboard_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    keyboard_state = USB_STATE_IDLE;
}

static struct usbd_interface keyboard_intf;
static struct usbd_endpoint keyboard_in_ep = {
    .ep_cb = usbd_hid_keyboard_in_callback,
    .ep_addr = KEYBOARD_EPIN_ADDR};
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
static struct usbd_interface mouse_intf;
static struct usbd_endpoint mouse_in_ep = {
    .ep_cb = usbd_hid_mouse_in_callback,
    .ep_addr = MOUSE_EPIN_ADDR};
#endif

#ifdef SHARED_EP_ENABLE
static bool shared_state;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t shared_buffer[SHARED_EPSIZE];

static void usbd_hid_shared_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    shared_state = USB_STATE_IDLE;
}

static struct usbd_interface shared_intf;
static struct usbd_endpoint shared_in_ep = {
    .ep_cb = usbd_hid_shared_in_callback,
    .ep_addr = SHARED_EPIN_ADDR};
#endif

#ifdef RAW_ENABLE
static bool raw_state;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t raw_in_buffer[RAW_EPSIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t raw_out_buffer[RAW_EPSIZE];

static void usbd_hid_raw_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    raw_state = USB_STATE_IDLE;
}

static void usbd_hid_raw_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    usbd_ep_start_read(0, RAW_EPOUT_ADDR, raw_out_buffer, 64);
    command_parse(raw_out_buffer, sizeof(raw_out_buffer));
}

static struct usbd_interface raw_intf;

static struct usbd_endpoint raw_in_ep = {
    .ep_cb = usbd_hid_raw_in_callback,
    .ep_addr = RAW_EPIN_ADDR};
static struct usbd_endpoint raw_out_ep = {
    .ep_cb = usbd_hid_raw_out_callback,
    .ep_addr = RAW_EPOUT_ADDR};
#endif

#ifdef CONSOLE_ENABLE

#endif

#ifdef MIDI_ENABLE
static bool midi_state;
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t midi_in_buffer[4];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t midi_out_buffer[4];

static void usbd_midi_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;
}

static void usbd_midi_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    (void)busid;
    (void)ep;
    (void)nbytes;

    midi_state = USB_STATE_IDLE;
}

static struct usbd_interface midi_in_intf;
static struct usbd_interface midi_out_intf;

static struct usbd_endpoint midi_out_ep = {
    .ep_addr = MIDI_EPOUT_ADDR,
    .ep_cb = usbd_midi_bulk_out
};
static struct usbd_endpoint midi_in_ep = {
    .ep_addr = MIDI_EPIN_ADDR,
    .ep_cb = usbd_midi_bulk_in
};
#endif

#ifdef VIRTSER_ENABLE

#endif

#if defined(JOYSTICK_ENABLE) && !defined(JOYSTICK_SHARED_EP)
static struct usbd_interface joystick_intf;
static struct usbd_endpoint joystick_in_ep = {
    .ep_cb = usbd_hid_joystick_in_callback,
    .ep_addr = JOYSTICK_EPIN_ADDR};
#endif

#if defined(DIGITIZER_ENABLE) && !defined(DIGITIZER_SHARED_EP)
static struct usbd_interface digitizer_intf;
static struct usbd_endpoint digitizer_in_ep = {
    .ep_cb = usbd_hid_digitizer_in_callback,
    .ep_addr = DIGITIZER_EPIN_ADDR};
#endif

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    UNUSED(busid);
    switch (event)
    {
    case USBD_EVENT_RESET:
        keyboard_state = USB_STATE_IDLE;
        shared_state = USB_STATE_IDLE;
        raw_state = USB_STATE_IDLE;
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        g_keyboard_is_suspend = usb_device_is_suspend(0);
        break;
    case USBD_EVENT_CONFIGURED:
        memset(raw_out_buffer, 0, sizeof(raw_out_buffer));
        usbd_ep_start_read(0, RAW_EPOUT_ADDR, raw_out_buffer, RAW_EPSIZE);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;
    default:
        break;
    }
}

void usb_init(void)
{
    usbd_desc_register(0, &usb_descriptor);
    
#ifndef KEYBOARD_SHARED_EP
    usbd_add_interface(0, usbd_hid_init_intf(0, &keyboard_intf, KeyboardReport, sizeof(KeyboardReport)));
    usbd_add_endpoint(0, &keyboard_in_ep);
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
    usbd_add_interface(0, usbd_hid_init_intf(0, &mouse_intf, MouseReport, sizeof(MouseReport)));
    usbd_add_endpoint(0, &mouse_in_ep);
#endif

#ifdef RAW_ENABLE
    usbd_add_interface(0, usbd_hid_init_intf(0, &raw_intf, RawReport, sizeof(RawReport)));
    usbd_add_endpoint(0, &raw_in_ep);
    usbd_add_endpoint(0, &raw_out_ep);
#endif

#ifdef SHARED_EP_ENABLE
    usbd_add_interface(0, usbd_hid_init_intf(0, &shared_intf, SharedReport, sizeof(SharedReport)));
    usbd_add_endpoint(0, &shared_in_ep);
#endif

#ifdef CONSOLE_ENABLE

#endif

#ifdef MIDI_ENABLE
    usbd_add_interface(0, &midi_out_intf);
    usbd_add_interface(0, &midi_in_intf);
    usbd_add_endpoint(0, &midi_out_ep);
    usbd_add_endpoint(0, &midi_in_ep);
#endif

#ifdef VIRTSER_ENABLE

#endif

#if defined(JOYSTICK_ENABLE) && !defined(JOYSTICK_SHARED_EP)
    usbd_add_interface(0, usbd_hid_init_intf(0, &joystick_intf, JoystickReport, sizeof(JoystickReport)));
    usbd_add_endpoint(0, &joystick_in_ep);
#endif

#if defined(DIGITIZER_ENABLE) && !defined(DIGITIZER_SHARED_EP) 
    usbd_add_interface(0, usbd_hid_init_intf(0, &digitizer_intf, DigitizerReport, sizeof(DigitizerReport)));
    usbd_add_endpoint(0, &digitizer_in_ep);
#endif

    usbd_initialize(0, USB_BASE, usbd_event_handler);
}

int usb_send_shared_ep(uint8_t *buffer, uint8_t size)
{
    if (shared_state == USB_STATE_BUSY)
    {
        return 1;
    }
    memcpy(shared_buffer, buffer, size);
    int ret = usbd_ep_start_write(0, SHARED_EPIN_ADDR, shared_buffer, SHARED_EPSIZE);
    if (ret < 0)
    {
        return 1;
    }
    shared_state = USB_STATE_BUSY;
    return 0;
}

int usb_send_keyboard(uint8_t *buffer, uint8_t size)
{
    UNUSED(size);
    if (keyboard_state == USB_STATE_BUSY)
    {
        return 1;
    }
    memcpy(keyboard_buffer, buffer, KEYBOARD_EPSIZE);
    int ret = usbd_ep_start_write(0, KEYBOARD_EPIN_ADDR, keyboard_buffer, KEYBOARD_EPSIZE);
    if (ret < 0)
    {
        return 1;
    }
    keyboard_state = USB_STATE_BUSY;
    return 0;
}

int usb_send_raw(uint8_t *buffer, uint8_t size)
{
    UNUSED(size);
    if (raw_state == USB_STATE_BUSY)
    {
        return 1;
    }
    else
    {
    }
    if (size > 0 && size <= 64)
    {
        memset(raw_in_buffer, 0, 64);
        memcpy(raw_in_buffer, buffer, size);
    }
    else
    {
        return 1;
    }
    int ret = usbd_ep_start_write(0, RAW_EPIN_ADDR, raw_in_buffer, 64);
    if (ret < 0)
    {
        return 1;
    }
    raw_state = USB_STATE_BUSY;
    return 0;

}

int usb_send_midi(uint8_t *buffer, uint8_t size)
{
    if (midi_state == USB_STATE_BUSY)
    {
        return 1;
    }
    else
    {
    }
    memcpy(midi_in_buffer, buffer, size);
    int ret = usbd_ep_start_write(0, MIDI_EPIN_ADDR, midi_in_buffer, 4);
    if (ret < 0)
    {
        return 1;
    }
    midi_state = USB_STATE_BUSY;
    return 0;
}

//		0x05, 0x01,   //Usage Page (Generic Desktop)
//		0x09, 0x05,   //Usage (Game Pad)
//		0xA1, 0x01,   //Collection (Application)
//		0x85, 0x03,   //REPORT_ID (3)
//		0x05, 0x01,   //Usage Page (Generic Desktop)
//		0x09, 0x01,   //Usage (Pointer)
//		0xA0,         //Collection (Physical)
//		0x14,         //Logical Minimum (0)
//		0x25, 0xFF,   //Logical Maximum (255)
//		0x75, 0x08,   //Report Size (8)
//		0x95, 0x04,   //Report Count (4)
//		0x09, 0x34,   //Usage (Ry)
//		0x09, 0x33,   //Usage (Rx)
//		0x09, 0x31,   //Usage (Y)
//		0x09, 0x30,   //Usage (X)
//		0x81, 0x02,   //Input (Data, Variable, Absolute)
//		0x14,         //Logical Minimum (0)
//		0x25, 0x01,   //Logical Maximum (1)
//		0x75, 0x01,   //Report Size (1)
//		0x95, 0x02,   //Report Count (2)
//		0x05, 0x09,   //Usage Page (Button)
//		0x19, 0x01,   //Usage Minimum (Button 1)
//		0x29, 0x02,   //Usage Maximum (Button 2)
//		0x81, 0x02,   //Input (Data, Variable, Absolute)
//		0x75, 0x06,   //Report Size (6)
//		0x95, 0x01,   //Report Count (1)
//		0x81, 0x03,   //Input (Constant, Variable, Absolute)
//		0xC0,         //END Collection
//		0xC0,         //END Collection 142
