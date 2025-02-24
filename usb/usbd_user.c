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

const uint8_t hid_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0002, 0x01),
    USB_CONFIG_DESCRIPTOR_INIT(USB_HID_CONFIG_DESC_SIZ, 0x01, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    // Keyboard
    /************** Descriptor of Joystick Mouse interface ****************/
    /* 09 */
    0x09,                          /* bLength: Interface Descriptor size */
    USB_DESCRIPTOR_TYPE_INTERFACE, /* bDescriptorType: Interface descriptor type */
    0x00,                          /* bInterfaceNumber: Number of Interface */
    0x00,                          /* bAlternateSetting: Alternate setting */
    0x02,                          /* bNumEndpoints */
    0x03,                          /* bInterfaceClass: HID */
    0x00,                          /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,                          /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,                             /* iInterface: Index of string descriptor */
    /******************** Descriptor of Joystick Mouse HID ********************/
    /* 18 */
    0x09,                    /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE_HID, /* bDescriptorType: HID */
    0x11,                    /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,                             /* bCountryCode: Hardware target country */
    0x01,                             /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,                             /* bDescriptorType */
    USBD_CUSTOM_HID_REPORT_DESC_SIZE, /* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Mouse endpoint ********************/
    /* 27 */
    0x07,                         /*bLength: Endpoint Descriptor size*/
    USB_DESCRIPTOR_TYPE_ENDPOINT, /*bDescriptorType:*/

    CUSTOM_HID_EPIN_ADDR, /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,                 /*bmAttributes: Interrupt endpoint*/
    CUSTOM_HID_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
    0x00,
    CUSTOM_HID_FS_BINTERVAL, /*bInterval: Polling Interval */
    /* 34 */

    0x07,                         /* bLength: Endpoint Descriptor size */
    USB_DESCRIPTOR_TYPE_ENDPOINT, /* bDescriptorType: */
    CUSTOM_HID_EPOUT_ADDR,        /*bEndpointAddress: Endpoint Address (OUT)*/
    0x03,                         /* bmAttributes: Interrupt endpoint */
    CUSTOM_HID_EPOUT_SIZE,        /* wMaxPacketSize: 2 Bytes max  */
    0x00,
    CUSTOM_HID_FS_BINTERVAL, /* bInterval: Polling Interval */
    // Mouse
    /* 73 */
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
    0x00};



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

const uint8_t custom_hid_report_desc[USBD_CUSTOM_HID_REPORT_DESC_SIZE] = {
    /* USER CODE BEGIN 0 */
    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x06, // USAGE (Keyboard)
    0xa1, 0x01, // COLLECTION (Application)
    0x85, 0x01, //     REPORT_ID (1)
    // bitmap of modifiers
    0x75, 0x01, //   Report Size (1),
    0x95, 0x08, //   Report Count (8),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0xE0, //   Usage Minimum (224),
    0x29, 0xE7, //   Usage Maximum (231),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0x01, //   Logical Maximum (1),
    0x81, 0x02, //   Input (Data, Variable, Absolute), ;Modifier byte
    // bitmap of keys
    0x95, 0x78, //   Report Count (120),
    0x75, 0x01, //   Report Size (1),
    0x15, 0x00, //   Logical Minimum (0),
    0x25, 0x01, //   Logical Maximum(1),
    0x05, 0x07, //   Usage Page (Key Codes),
    0x19, 0x00, //   Usage Minimum (0),
    0x29, 0x77, //   Usage Maximum (),
    0x81, 0x02, //   Input (Data, Variable, Absolute),
                // LED output report
    0x95, 0x05, //   Report Count (5),
    0x75, 0x01, //   Report Size (1),
    0x05, 0x08, //   Usage Page (LEDs),
    0x19, 0x01, //   Usage Minimum (1),
    0x29, 0x05, //   Usage Maximum (5),
    0x91, 0x02, //   Output (Data, Variable, Absolute),
    0x95, 0x01, //   Report Count (1),
    0x75, 0x03, //   Report Size (3),
    0x91, 0x03, //   Output (Constant),
    0xC0,
    //	RAW HID
    0x06, 0xC0, 0xFF, // 30
    0x0A, 0x00, 0x0C,
    0xA1, 0x01,       // Collection 0x01
    0x85, 0x02,       // REPORT_ID (2)
    0x75, 0x08,       // report size = 8 bits
    0x15, 0x00,       // logical minimum = 0
    0x26, 0xFF, 0x00, // logical maximum = 255
    0x95, 64 - 1,     // report count TX
    0x09, 0x01,       // usage
    0x81, 0x02,       // Input (array)
    0x95, 64 - 1,     // report count RX
    0x09, 0x02,       // usage
    0x91, 0x02,       // Output (array)
    0xC0,             /*     END_COLLECTION	       89 Bytes      */

    0x05, 0x01, // USAGE_PAGE (Generic Desktop)
    0x09, 0x02, // USAGE (Mouse)
    0xA1, 0x01, // COLLECTION (Application)
    0x85, 0x03, //   REPORT ID (0x01)
    0x09, 0x01, //   USAGE (Pointer)

    0xA1, 0x00, //   COLLECTION (Physical)
    0x05, 0x09, //     USAGE_PAGE (Button)
    0x19, 0x01, //     USAGE_MINIMUM (Button 1)
    0x29, 0x05, //     USAGE_MAXIMUM (Button 5)

    0x15, 0x00, //     LOGICAL_MINIMUM (0)
    0x25, 0x01, //     LOGICAL_MAXIMUM (1)
    0x95, 0x05, //     REPORT_COUNT (5)
    0x75, 0x01, //     REPORT_SIZE (1)

    0x81, 0x02, //     INPUT (Data,Var,Abs)
    0x95, 0x01, //     REPORT_COUNT (1)
    0x75, 0x03, //     REPORT_SIZE (3)
    0x81, 0x01, //     INPUT (Cnst,Var,Abs)

    0x05, 0x01, //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30, //     USAGE (X)
    0x09, 0x31, //     USAGE (Y)
    0x09, 0x38,

    0x15, 0x81, //     LOGICAL_MINIMUM (-127)
    0x25, 0x7F, //     LOGICAL_MAXIMUM (127)
    0x75, 0x08, //     REPORT_SIZE (8)
    0x95, 0x03, //     REPORT_COUNT (2)

    0x81, 0x06, //     INPUT (Data,Var,Rel)
    0xC0, 0xc0 //   END_COLLECTION
    //0xC0              /*     END_COLLECTION	             */
};

#ifdef ENABLE_MOUSE
/*!< hid mouse report descriptor */
#endif
/*!< custom hid report descriptor */
enum
{
    HID_STATE_IDLE = 0,
    HID_STATE_BUSY
};

/*!< hid state ! Data can be sent only when state is idle  */
static volatile uint8_t custom_state;

uint32_t g_usb_report_count;
uint32_t g_usb_report_count1;

USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t read_buffer[CUSTOM_HID_EPOUT_SIZE];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t send_buffer[CUSTOM_HID_EPIN_SIZE];
static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    UNUSED(busid);
    switch (event)
    {
    case USBD_EVENT_RESET:
        custom_state = HID_STATE_IDLE;
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
        usbd_ep_start_read(0, CUSTOM_HID_EPOUT_ADDR, read_buffer, CUSTOM_HID_EPOUT_SIZE);
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;
    default:
        break;
    }
}

static void usbd_hid_custom_in_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    custom_state = HID_STATE_IDLE;
}

static void usbd_hid_custom_out_callback(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // USB_LOG_RAW("actual out len:%ld\r\n", nbytes);
    UNUSED(busid);
    UNUSED(ep);
    UNUSED(nbytes);
    usbd_ep_start_read(0, CUSTOM_HID_EPOUT_ADDR, read_buffer, 64);
    if (read_buffer[0] == 1)
    {
        g_keyboard_led_state = read_buffer[1];
    }
    if (read_buffer[0] == 2)
    {
        command_prase(read_buffer + 1, sizeof(read_buffer) - 1);
    }
}

static struct usbd_endpoint custom_in_ep = {
    .ep_cb = usbd_hid_custom_in_callback,
    .ep_addr = CUSTOM_HID_EPIN_ADDR};

static struct usbd_endpoint custom_out_ep = {
    .ep_cb = usbd_hid_custom_out_callback,
    .ep_addr = CUSTOM_HID_EPOUT_ADDR};

struct usbd_interface intf0; // Custom

void hid_init(void)
{
    usbd_desc_register(0, (uint8_t*)&hid_descriptor_new);
    usbd_add_interface(0, usbd_hid_init_intf(0, &intf0, SharedReport, sizeof(SharedReport)));
    usbd_add_endpoint(0, &custom_in_ep);
    usbd_add_endpoint(0, &custom_out_ep);

    usbd_initialize(0, USB_BASE, usbd_event_handler);
}

int hid_keyboard_send(uint8_t *buffer)
{
    memcpy(send_buffer + 1, buffer, 17);
    send_buffer[0] = REPORT_ID_NKRO;
    if (custom_state == HID_STATE_BUSY)
    {
        return 1;
    }
    else
    {
    }
    int ret = usbd_ep_start_write(0, CUSTOM_HID_EPIN_ADDR, send_buffer, 64);
    if (ret < 0)
    {
        return 1;
    }
    custom_state = HID_STATE_BUSY;
    return 0;
}

int hid_mouse_send(uint8_t *buffer)
{
    memcpy(send_buffer + 1, buffer, 4);
    send_buffer[0] = REPORT_ID_MOUSE;
    if (custom_state == HID_STATE_BUSY) {
        return 1;
    }
    int ret = usbd_ep_start_write(0, CUSTOM_HID_EPIN_ADDR, send_buffer, 64);
    if (ret < 0) {
        return 1;
    }
    custom_state = HID_STATE_BUSY;
    return 0;
}

void hid_keyboard_test()
{
    memset(send_buffer, 0, 64);
    // memcpy(send_buffer+1, buffer, 17);
    send_buffer[0] = 1;
    send_buffer[4] = 1;
    if (custom_state == HID_STATE_BUSY)
    {
        return;
    }
    else
    {
    }
    int ret = usbd_ep_start_write(0, CUSTOM_HID_EPIN_ADDR, send_buffer, 64);
    if (ret < 0)
    {
        return;
    }
    custom_state = HID_STATE_BUSY;
}

int hid_raw_send(uint8_t *buffer, int size)
{
    if (custom_state == HID_STATE_BUSY)
    {
        return 1;
    }
    else
    {
    }
    if (size > 0 && size <= 63)
    {
        memcpy(send_buffer + 1, buffer, size);
    }
    else
    {
        return 1;
    }

    send_buffer[0] = 2;
    int ret = usbd_ep_start_write(0, CUSTOM_HID_EPIN_ADDR, send_buffer, 64);
    if (ret < 0)
    {
        return 1;
    }
    custom_state = HID_STATE_BUSY;
    return 0;
}
