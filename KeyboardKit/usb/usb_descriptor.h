/*
 * Copyright 2012,2013 Jun Wako <wakojun@gmail.com>
 * This file is based on:
 *     LUFA-120219/Demos/Device/Lowlevel/KeyboardMouse
 *     LUFA-120219/Demos/Device/Lowlevel/GenericHID
 */

/*
                         LUFA Library
         Copyright (C) Dean Camera, 2012.

    dean [at] fourwalledcubicle [dot] com
                     www.lufa-lib.org
*/

/*
    Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)
    Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)

    Permission to use, copy, modify, distribute, and sell this
    software and its documentation for any purpose is hereby granted
    without fee, provided that the above copyright notice appear in
    all copies and that both that the copyright notice and this
    permission notice and warranty disclaimer appear in supporting
    documentation, and that the name of the author not be used in
    advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.

    The author disclaim all warranties with regard to this
    software, including all implied warranties of merchantability
    and fitness.  In no event shall the author be liable for any
    special, indirect or consequential damages or any damages
    whatsoever resulting from loss of use, data or profits, whether
    in an action of contract, negligence or other tortious action,
    arising out of or in connection with the use or performance of
    this software.
*/

/** \file
 *
 *  Header file for Descriptors.c.
 */

#pragma once
#include "progmem.h"
#include <stddef.h>
#include <inttypes.h>
#include "keyboard.h"

#define ATTR_PACKED __attribute__((packed))
/** Concatenates the given input into a single token, via the C Preprocessor.
 *
 *  \param[in] x  First item to concatenate.
 *  \param[in] y  Second item to concatenate.
 *
 *  \return Concatenated version of the input.
 */
#define CONCAT(x, y) x##y

/** CConcatenates the given input into a single token after macro expansion, via the C Preprocessor.
 *
 *  \param[in] x  First item to concatenate.
 *  \param[in] y  Second item to concatenate.
 *
 *  \return Concatenated version of the expanded input.
 */
#define CONCAT_EXPANDED(x, y) CONCAT(x, y)
#define CPU_TO_LE16(x) (x)

// We don't need anything from the following files, or we have defined it already
#define __LUFA_COMMON_H__
#define __USBMODE_H__
#define __USBEVENTS_H__
#define __HIDPARSER_H__
#define __USBCONTROLLER_AVR8_H__

#define __INCLUDE_FROM_USB_DRIVER
#define __INCLUDE_FROM_HID_DRIVER
#define __INCLUDE_FROM_CDC_DRIVER
#define __INCLUDE_FROM_AUDIO_DRIVER
#define __INCLUDE_FROM_MIDI_DRIVER
#include "LUFA/Drivers/USB/Class/Common/HIDClassCommon.h"
#include "LUFA/Drivers/USB/Class/Common/HIDReportData.h"
#include "LUFA/Drivers/USB/Class/Common/CDCClassCommon.h"
#include "LUFA/Drivers/USB/Class/Common/AudioClassCommon.h"
#include "LUFA/Drivers/USB/Class/Common/MIDIClassCommon.h"
#include "LUFA/Drivers/USB/Core/USBController.h"

#define MAX_ENDPOINTS 8

/*
 * USB descriptor structure
 */
typedef struct {
    USB_Descriptor_Configuration_Header_t Config;

#ifndef KEYBOARD_SHARED_EP
    // Keyboard HID Interface
    USB_Descriptor_Interface_t Keyboard_Interface;
    USB_HID_Descriptor_HID_t   Keyboard_HID;
    USB_Descriptor_Endpoint_t  Keyboard_INEndpoint;
    USB_Descriptor_Endpoint_t  Keyboard_OUTEndpoint;
#else
    // Shared Interface
    USB_Descriptor_Interface_t Shared_Interface;
    USB_HID_Descriptor_HID_t   Shared_HID;
    USB_Descriptor_Endpoint_t  Shared_INEndpoint;
    USB_Descriptor_Endpoint_t  Shared_OUTEndpoint;
#endif

#ifdef RAW_ENABLE
    // Raw HID Interface
    USB_Descriptor_Interface_t Raw_Interface;
    USB_HID_Descriptor_HID_t   Raw_HID;
    USB_Descriptor_Endpoint_t  Raw_INEndpoint;
    USB_Descriptor_Endpoint_t  Raw_OUTEndpoint;
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
    // Mouse HID Interface
    USB_Descriptor_Interface_t Mouse_Interface;
    USB_HID_Descriptor_HID_t   Mouse_HID;
    USB_Descriptor_Endpoint_t  Mouse_INEndpoint;
#endif

#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
    // Shared Interface
    USB_Descriptor_Interface_t Shared_Interface;
    USB_HID_Descriptor_HID_t   Shared_HID;
    USB_Descriptor_Endpoint_t  Shared_INEndpoint;
#endif

#ifdef CONSOLE_ENABLE
    // Console HID Interface
    USB_Descriptor_Interface_t Console_Interface;
    USB_HID_Descriptor_HID_t   Console_HID;
    USB_Descriptor_Endpoint_t  Console_INEndpoint;
#endif

#ifdef MIDI_ENABLE
    USB_Descriptor_Interface_Association_t Audio_Interface_Association;
    // MIDI Audio Control Interface
    USB_Descriptor_Interface_t          Audio_ControlInterface;
    USB_Audio_Descriptor_Interface_AC_t Audio_ControlInterface_SPC;
    // MIDI Audio Streaming Interface
    USB_Descriptor_Interface_t                Audio_StreamInterface;
    USB_MIDI_Descriptor_AudioInterface_AS_t   Audio_StreamInterface_SPC;
    USB_MIDI_Descriptor_InputJack_t           MIDI_In_Jack_Emb;
    USB_MIDI_Descriptor_InputJack_t           MIDI_In_Jack_Ext;
    USB_MIDI_Descriptor_OutputJack_t          MIDI_Out_Jack_Emb;
    USB_MIDI_Descriptor_OutputJack_t          MIDI_Out_Jack_Ext;
    USB_Audio_Descriptor_StreamEndpoint_Std_t MIDI_In_Jack_Endpoint;
    USB_MIDI_Descriptor_Jack_Endpoint_t       MIDI_In_Jack_Endpoint_SPC;
    USB_Audio_Descriptor_StreamEndpoint_Std_t MIDI_Out_Jack_Endpoint;
    USB_MIDI_Descriptor_Jack_Endpoint_t       MIDI_Out_Jack_Endpoint_SPC;
#endif

#ifdef VIRTSER_ENABLE
    USB_Descriptor_Interface_Association_t CDC_Interface_Association;
    // CDC Control Interface
    USB_Descriptor_Interface_t            CDC_CCI_Interface;
    USB_CDC_Descriptor_FunctionalHeader_t CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t    CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t  CDC_Functional_Union;
    USB_Descriptor_Endpoint_t             CDC_NotificationEndpoint;
    // CDC Data Interface
    USB_Descriptor_Interface_t CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t  CDC_DataOutEndpoint;
    USB_Descriptor_Endpoint_t  CDC_DataInEndpoint;
#endif

#if defined(JOYSTICK_ENABLE) && !defined(JOYSTICK_SHARED_EP)
    // Joystick HID Interface
    USB_Descriptor_Interface_t Joystick_Interface;
    USB_HID_Descriptor_HID_t   Joystick_HID;
    USB_Descriptor_Endpoint_t  Joystick_INEndpoint;
#endif

#if defined(DIGITIZER_ENABLE) && !defined(DIGITIZER_SHARED_EP)
    // Digitizer HID Interface
    USB_Descriptor_Interface_t Digitizer_Interface;
    USB_HID_Descriptor_HID_t   Digitizer_HID;
    USB_Descriptor_Endpoint_t  Digitizer_INEndpoint;
#endif
} USB_Descriptor_Configuration_t;

/*
 * Interface indexes
 */
enum usb_interfaces {
#ifndef KEYBOARD_SHARED_EP
    KEYBOARD_INTERFACE,
#else
    SHARED_INTERFACE,
#    define KEYBOARD_INTERFACE SHARED_INTERFACE
#endif

// It is important that the Raw HID interface is at a constant
// interface number, to support Linux/OSX platforms and chrome.hid
// If Raw HID is enabled, let it be always 1.
#ifdef RAW_ENABLE
    RAW_INTERFACE,
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
    MOUSE_INTERFACE,
#endif

#if defined(SHARED_EP_ENABLE) && !defined(KEYBOARD_SHARED_EP)
    SHARED_INTERFACE,
#endif

#ifdef CONSOLE_ENABLE
    CONSOLE_INTERFACE,
#endif

#ifdef MIDI_ENABLE
    AC_INTERFACE,
    AS_INTERFACE,
#endif

#ifdef VIRTSER_ENABLE
    CCI_INTERFACE,
    CDI_INTERFACE,
#endif

#if defined(JOYSTICK_ENABLE) && !defined(JOYSTICK_SHARED_EP)
    JOYSTICK_INTERFACE,
#endif

#if defined(DIGITIZER_ENABLE) && !defined(DIGITIZER_SHARED_EP)
    DIGITIZER_INTERFACE,
#endif
    TOTAL_INTERFACES
};

#define IS_VALID_INTERFACE(i) ((i) >= 0 && (i) < TOTAL_INTERFACES)

#define NEXT_EPNUM __COUNTER__

/*
 * Endpoint numbers
 */
enum usb_endpoints {
    __unused_epnum__ = NEXT_EPNUM, // Endpoint numbering starts at 1

#ifndef KEYBOARD_SHARED_EP
    KEYBOARD_IN_EPNUM = NEXT_EPNUM,
    KEYBOARD_OUT_EPNUM = KEYBOARD_IN_EPNUM,
#else
#    define KEYBOARD_IN_EPNUM SHARED_IN_EPNUM
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
    MOUSE_IN_EPNUM = NEXT_EPNUM,
#else
#    define MOUSE_IN_EPNUM SHARED_IN_EPNUM
#endif

#ifdef RAW_ENABLE
    RAW_IN_EPNUM = NEXT_EPNUM,
#    ifdef USB_ENDPOINTS_ARE_REORDERABLE
#        define RAW_OUT_EPNUM RAW_IN_EPNUM
#    else
    RAW_OUT_EPNUM = RAW_IN_EPNUM,
#    endif
#endif

#ifdef SHARED_EP_ENABLE
    SHARED_IN_EPNUM = NEXT_EPNUM,
#ifdef KEYBOARD_SHARED_EP
    SHARED_OUT_EPNUM = SHARED_IN_EPNUM,
#endif
#endif

#ifdef CONSOLE_ENABLE
    CONSOLE_IN_EPNUM = NEXT_EPNUM,
#endif

#ifdef MIDI_ENABLE
    MIDI_STREAM_IN_EPNUM = NEXT_EPNUM,
#    ifdef USB_ENDPOINTS_ARE_REORDERABLE
#        define MIDI_STREAM_OUT_EPNUM MIDI_STREAM_IN_EPNUM
#    else
    MIDI_STREAM_OUT_EPNUM = NEXT_EPNUM,
#    endif
#endif

#ifdef VIRTSER_ENABLE
    CDC_NOTIFICATION_EPNUM = NEXT_EPNUM,
    CDC_IN_EPNUM           = NEXT_EPNUM,
#    ifdef USB_ENDPOINTS_ARE_REORDERABLE
#        define CDC_OUT_EPNUM CDC_IN_EPNUM
#    else
    CDC_OUT_EPNUM         = NEXT_EPNUM,
#    endif
#endif

#ifdef JOYSTICK_ENABLE
#    if !defined(JOYSTICK_SHARED_EP)
    JOYSTICK_IN_EPNUM = NEXT_EPNUM,
#    else
#        define JOYSTICK_IN_EPNUM SHARED_IN_EPNUM
#    endif
#endif

#ifdef DIGITIZER_ENABLE
#    if !defined(DIGITIZER_SHARED_EP)
    DIGITIZER_IN_EPNUM = NEXT_EPNUM,
#    else
#        define DIGITIZER_IN_EPNUM SHARED_IN_EPNUM
#    endif
#endif
};

#ifdef PROTOCOL_LUFA
// LUFA tells us total endpoints including control
#    define MAX_ENDPOINTS (ENDPOINT_TOTAL_ENDPOINTS - 1)
#elif defined(PROTOCOL_CHIBIOS)
// ChibiOS gives us number of available user endpoints, not control
#    define MAX_ENDPOINTS USB_MAX_ENDPOINTS
#endif

#if (NEXT_EPNUM - 1) > MAX_ENDPOINTS
#    error There are not enough available endpoints to support all functions. Please disable one or more of the following: Mouse Keys, Extra Keys, Console, NKRO, MIDI, Serial, Steno
#endif

#define KEYBOARD_EPSIZE 8
#define SHARED_EPSIZE 32
#define MOUSE_EPSIZE 16
#define RAW_EPSIZE 0x40
#define CONSOLE_EPSIZE 32
#define MIDI_STREAM_EPSIZE 64
#define CDC_NOTIFICATION_EPSIZE 8
#define CDC_EPSIZE 16
#define JOYSTICK_EPSIZE 8
#define DIGITIZER_EPSIZE 8

#ifdef EXTERN_DESCRIPTOR
/*
 * HID report descriptors
 */
#ifdef KEYBOARD_SHARED_EP
extern  const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[];
#    define SHARED_REPORT_STARTED
#else
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[];
#endif

#ifdef MOUSE_ENABLE
#    ifndef MOUSE_SHARED_EP
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport[];
#    elif !defined(SHARED_REPORT_STARTED)
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[];
#        define SHARED_REPORT_STARTED
#    endif
#endif

#ifdef JOYSTICK_ENABLE
#    ifndef JOYSTICK_SHARED_EP
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[];
#    elif !defined(SHARED_REPORT_STARTED)
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[];
#        define SHARED_REPORT_STARTED
#    endif
#endif

#ifdef DIGITIZER_ENABLE
#    ifndef DIGITIZER_SHARED_EP
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM DigitizerReport[];
#    elif !defined(SHARED_REPORT_STARTED)
extern  const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[];
#        define SHARED_REPORT_STARTED
#    endif
#endif

#if defined(SHARED_EP_ENABLE) && !defined(SHARED_REPORT_STARTED)
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[];
#endif

#ifdef RAW_ENABLE
extern  const USB_Descriptor_HIDReport_Datatype_t PROGMEM RawReport[];
#endif

#ifdef CONSOLE_ENABLE
extern const USB_Descriptor_HIDReport_Datatype_t PROGMEM ConsoleReport[];
#endif


/*
 * Device descriptor
 */
extern const USB_Descriptor_Device_t PROGMEM DeviceDescriptor;


/*
 * Configuration descriptors
 */
extern const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor;


/*
 * String descriptors
 */
extern const USB_Descriptor_String_t PROGMEM LanguageString;
extern const USB_Descriptor_String_t PROGMEM ManufacturerString;
extern const USB_Descriptor_String_t PROGMEM ProductString;

// clang-format on

#if defined(SERIAL_NUMBER)
// clang-format off
extern const USB_Descriptor_String_t PROGMEM SerialNumberString;
// clang-format on
#endif // defined(SERIAL_NUMBER)
#endif

#ifndef KEYBOARD_SHARED_EP
#define KEYBOARD_EPIN_ADDR  (ENDPOINT_DIR_IN | KEYBOARD_IN_EPNUM)
#define KEYBOARD_EPOUT_ADDR (ENDPOINT_DIR_OUT | KEYBOARD_OUT_EPNUM)
#endif

#ifdef RAW_ENABLE
#define RAW_EPIN_ADDR  (ENDPOINT_DIR_IN | RAW_IN_EPNUM)
#define RAW_EPOUT_ADDR (ENDPOINT_DIR_OUT | RAW_OUT_EPNUM)
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
#define MOUSE_EPIN_ADDR  (ENDPOINT_DIR_IN | MOUSE_IN_EPNUM)
#endif

#ifdef SHARED_EP_ENABLE
#define SHARED_EPIN_ADDR  (ENDPOINT_DIR_IN | SHARED_IN_EPNUM)
#ifdef KEYBOARD_SHARED_EP
#define SHARED_EPOUT_ADDR (ENDPOINT_DIR_OUT | SHARED_OUT_EPNUM)
#endif
#endif

#ifdef CONSOLE_ENABLE
#define CONSOLE_EPIN_ADDR  (ENDPOINT_DIR_IN | CONSOLE_IN_EPNUM)
#endif

#ifdef MIDI_ENABLE
#define MIDI_EPIN_ADDR  (ENDPOINT_DIR_IN | MIDI_STREAM_IN_EPNUM)
#define MIDI_EPOUT_ADDR  (ENDPOINT_DIR_OUT | MIDI_STREAM_OUT_EPNUM)
#endif

#if defined(JOYSTICK_ENABLE) && !defined(JOYSTICK_SHARED_EP)
#define JOYSTICK_EPIN_ADDR  (ENDPOINT_DIR_IN | JOYSTICK_IN_EPNUM)
#endif


#if defined(DIGITIZER_ENABLE) && !defined(DIGITIZER_SHARED_EP)
#define DIGITIZER_EPIN_ADDR  (ENDPOINT_DIR_IN | DIGITIZER_IN_EPNUM)
#endif

//usb_descripotor.c before
/*
 * Copyright 2012 Jun Wako <wakojun@gmail.com>
 * This file is based on:
 *     LUFA-120219/Demos/Device/Lowlevel/KeyboardMouse
 *     LUFA-120219/Demos/Device/Lowlevel/GenericHID
 */

/*
                         LUFA Library
         Copyright (C) Dean Camera, 2012.

    dean [at] fourwalledcubicle [dot] com
                     www.lufa-lib.org
*/

/*
    Copyright 2012  Dean Camera (dean [at] fourwalledcubicle [dot] com)
    Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)

    Permission to use, copy, modify, distribute, and sell this
    software and its documentation for any purpose is hereby granted
    without fee, provided that the above copyright notice appear in
    all copies and that both that the copyright notice and this
    permission notice and warranty disclaimer appear in supporting
    documentation, and that the name of the author not be used in
    advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.

    The author disclaim all warranties with regard to this
    software, including all implied warranties of merchantability
    and fitness.  In no event shall the author be liable for any
    special, indirect or consequential damages or any damages
    whatsoever resulting from loss of use, data or profits, whether
    in an action of contract, negligence or other tortious action,
    arising out of or in connection with the use or performance of
    this software.
*/

#include "util.h"
#include "report.h"
//#include "usb_descriptor.h"
#include "usb_descriptor_common.h"

#ifdef JOYSTICK_ENABLE
#    include "joystick.h"
#endif

#ifdef OS_DETECTION_ENABLE
#    include "os_detection.h"
#endif

#if defined(SERIAL_NUMBER) || (defined(SERIAL_NUMBER_USE_HARDWARE_ID) && SERIAL_NUMBER_USE_HARDWARE_ID == TRUE)

#    define HAS_SERIAL_NUMBER

#    if defined(SERIAL_NUMBER_USE_HARDWARE_ID) && SERIAL_NUMBER_USE_HARDWARE_ID == TRUE
#        include "hardware_id.h"
#    endif

#endif // defined(SERIAL_NUMBER) || (defined(SERIAL_NUMBER_USE_HARDWARE_ID) && SERIAL_NUMBER_USE_HARDWARE_ID == TRUE)

// clang-format off

/*
 * HID report descriptors
 */
#ifdef KEYBOARD_SHARED_EP
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[] = {
#    define SHARED_REPORT_STARTED
#else
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[] = {
#endif
    HID_RI_USAGE_PAGE(8, 0x01),        // Generic Desktop
    HID_RI_USAGE(8, 0x06),             // Keyboard
    HID_RI_COLLECTION(8, 0x01),        // Application
#ifdef KEYBOARD_SHARED_EP
        HID_RI_REPORT_ID(8, REPORT_ID_KEYBOARD),
#endif
        // Modifiers (8 bits)
        HID_RI_USAGE_PAGE(8, 0x07),    // Keyboard/Keypad
        HID_RI_USAGE_MINIMUM(8, 0xE0), // Keyboard Left Control
        HID_RI_USAGE_MAXIMUM(8, 0xE7), // Keyboard Right GUI
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        // Reserved (1 byte)
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_CONSTANT),
        // Keycodes (6 bytes)
        HID_RI_USAGE_PAGE(8, 0x07),    // Keyboard/Keypad
        HID_RI_USAGE_MINIMUM(8, 0x00),
        HID_RI_USAGE_MAXIMUM(8, 0xFF),
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_COUNT(8, 0x06),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),

        // Status LEDs (5 bits)
        HID_RI_USAGE_PAGE(8, 0x08),    // LED
        HID_RI_USAGE_MINIMUM(8, 0x01), // Num Lock
        HID_RI_USAGE_MAXIMUM(8, 0x05), // Kana
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x05),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
        // LED padding (3 bits)
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x03),
        HID_RI_OUTPUT(8, HID_IOF_CONSTANT),
    HID_RI_END_COLLECTION(0),
#ifndef KEYBOARD_SHARED_EP
};
#endif

#ifdef MOUSE_ENABLE
#    ifndef MOUSE_SHARED_EP
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport[] = {
#    elif !defined(SHARED_REPORT_STARTED)
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[] = {
#        define SHARED_REPORT_STARTED
#    endif
    HID_RI_USAGE_PAGE(8, 0x01),            // Generic Desktop
    HID_RI_USAGE(8, 0x02),                 // Mouse
    HID_RI_COLLECTION(8, 0x01),            // Application
#    ifdef MOUSE_SHARED_EP
        HID_RI_REPORT_ID(8, REPORT_ID_MOUSE),
#    endif
        HID_RI_USAGE(8, 0x01),             // Pointer
        HID_RI_COLLECTION(8, 0x00),        // Physical
            // Buttons (8 bits)
            HID_RI_USAGE_PAGE(8, 0x09),    // Button
            HID_RI_USAGE_MINIMUM(8, 0x01), // Button 1
            HID_RI_USAGE_MAXIMUM(8, 0x08), // Button 8
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 0x08),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

#    ifdef MOUSE_EXTENDED_REPORT
            // Boot protocol XY ignored in Report protocol
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_REPORT_SIZE(8, 0x08),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),
#    endif
            // X/Y position (2 or 4 bytes)
            HID_RI_USAGE_PAGE(8, 0x01),    // Generic Desktop
            HID_RI_USAGE(8, 0x30),         // X
            HID_RI_USAGE(8, 0x31),         // Y
#    ifndef MOUSE_EXTENDED_REPORT
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_REPORT_SIZE(8, 0x08),
#    else
            HID_RI_LOGICAL_MINIMUM(16, -32767),
            HID_RI_LOGICAL_MAXIMUM(16,  32767),
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_REPORT_SIZE(8, 0x10),
#    endif
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),

            // Vertical wheel (1 or 2 bytes)
            HID_RI_USAGE(8, 0x38),     // Wheel
#    ifndef WHEEL_EXTENDED_REPORT
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x08),
#    else
            HID_RI_LOGICAL_MINIMUM(16, -32767),
            HID_RI_LOGICAL_MAXIMUM(16,  32767),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x10),
#    endif
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),
            // Horizontal wheel (1 or 2 bytes)
            HID_RI_USAGE_PAGE(8, 0x0C),// Consumer
            HID_RI_USAGE(16, 0x0238),  // AC Pan
#    ifndef WHEEL_EXTENDED_REPORT
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x08),
#    else
            HID_RI_LOGICAL_MINIMUM(16, -32767),
            HID_RI_LOGICAL_MAXIMUM(16,  32767),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x10),
#    endif
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#    ifndef MOUSE_SHARED_EP
};
#    endif
#endif

#ifdef JOYSTICK_ENABLE
#    ifndef JOYSTICK_SHARED_EP
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM JoystickReport[] = {
#    elif !defined(SHARED_REPORT_STARTED)
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[] = {
#        define SHARED_REPORT_STARTED
#    endif
    HID_RI_USAGE_PAGE(8, 0x01),     // Generic Desktop
    HID_RI_USAGE(8, 0x04),          // Joystick
    HID_RI_COLLECTION(8, 0x01),     // Application
#    ifdef JOYSTICK_SHARED_EP
        HID_RI_REPORT_ID(8, REPORT_ID_JOYSTICK),
#    endif
        HID_RI_COLLECTION(8, 0x00), // Physical
#    if JOYSTICK_AXIS_COUNT > 0
            HID_RI_USAGE_PAGE(8, 0x01), // Generic Desktop
            HID_RI_USAGE(8, 0x30),      // X
#        if JOYSTICK_AXIS_COUNT > 1
            HID_RI_USAGE(8, 0x31),      // Y
#        endif
#        if JOYSTICK_AXIS_COUNT > 2
            HID_RI_USAGE(8, 0x32),      // Z
#        endif
#        if JOYSTICK_AXIS_COUNT > 3
            HID_RI_USAGE(8, 0x33),      // Rx
#        endif
#        if JOYSTICK_AXIS_COUNT > 4
            HID_RI_USAGE(8, 0x34),      // Ry
#        endif
#        if JOYSTICK_AXIS_COUNT > 5
            HID_RI_USAGE(8, 0x35),      // Rz
#        endif
#        if JOYSTICK_AXIS_RESOLUTION == 8
            HID_RI_LOGICAL_MINIMUM(8, -JOYSTICK_MAX_VALUE),
            HID_RI_LOGICAL_MAXIMUM(8, JOYSTICK_MAX_VALUE),
            HID_RI_REPORT_COUNT(8, JOYSTICK_AXIS_COUNT),
            HID_RI_REPORT_SIZE(8, 0x08),
#        else
            HID_RI_LOGICAL_MINIMUM(16, -JOYSTICK_MAX_VALUE),
            HID_RI_LOGICAL_MAXIMUM(16, JOYSTICK_MAX_VALUE),
            HID_RI_REPORT_COUNT(8, JOYSTICK_AXIS_COUNT),
            HID_RI_REPORT_SIZE(8, 0x10),
#        endif
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
#    endif

#    ifdef JOYSTICK_HAS_HAT
            // Hat Switch (4 bits)
            HID_RI_USAGE(8, 0x39), // Hat Switch
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x07),
            HID_RI_PHYSICAL_MINIMUM(8, 0),
            HID_RI_PHYSICAL_MAXIMUM(16, 315),
            HID_RI_UNIT(8, 0x14),  // Degree, English Rotation
            HID_RI_REPORT_COUNT(8, 1),
            HID_RI_REPORT_SIZE(8, 4),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NULLSTATE),
            // Padding (4 bits)
            HID_RI_REPORT_COUNT(8, 0x04),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),
#    endif

#    if JOYSTICK_BUTTON_COUNT > 0
            HID_RI_USAGE_PAGE(8, 0x09), // Button
            HID_RI_USAGE_MINIMUM(8, 0x01),
            HID_RI_USAGE_MAXIMUM(8, JOYSTICK_BUTTON_COUNT),
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, JOYSTICK_BUTTON_COUNT),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

#        if (JOYSTICK_BUTTON_COUNT % 8) != 0
            HID_RI_REPORT_COUNT(8, 8 - (JOYSTICK_BUTTON_COUNT % 8)),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),
#        endif
#    endif
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#    ifndef JOYSTICK_SHARED_EP
};
#    endif
#endif

#ifdef DIGITIZER_ENABLE
#    ifndef DIGITIZER_SHARED_EP
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM DigitizerReport[] = {
#    elif !defined(SHARED_REPORT_STARTED)
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[] = {
#        define SHARED_REPORT_STARTED
#    endif
    HID_RI_USAGE_PAGE(8, 0x0D),            // Digitizers
    HID_RI_USAGE(8, 0x01),                 // Digitizer
    HID_RI_COLLECTION(8, 0x01),            // Application
#    ifdef DIGITIZER_SHARED_EP
        HID_RI_REPORT_ID(8, REPORT_ID_DIGITIZER),
#    endif
        HID_RI_USAGE(8, 0x20),             // Stylus
        HID_RI_COLLECTION(8, 0x00),        // Physical
            // In Range, Tip Switch & Barrel Switch (3 bits)
            HID_RI_USAGE(8, 0x32),         // In Range
            HID_RI_USAGE(8, 0x42),         // Tip Switch
            HID_RI_USAGE(8, 0x44),         // Barrel Switch
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 0x03),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
            // Padding (5 bits)
            HID_RI_REPORT_COUNT(8, 0x05),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),

            // X/Y Position (4 bytes)
            HID_RI_USAGE_PAGE(8, 0x01),    // Generic Desktop
            HID_RI_USAGE(8, 0x30),         // X
            HID_RI_USAGE(8, 0x31),         // Y
            HID_RI_LOGICAL_MAXIMUM(16, 0x7FFF),
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_REPORT_SIZE(8, 0x10),
            HID_RI_UNIT(8, 0x13),          // Inch, English Linear
            HID_RI_UNIT_EXPONENT(8, 0x0E), // -2
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#    ifndef DIGITIZER_SHARED_EP
};
#    endif
#endif

#if defined(SHARED_EP_ENABLE) && !defined(SHARED_REPORT_STARTED)
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM SharedReport[] = {
#endif

#ifdef EXTRAKEY_ENABLE
    HID_RI_USAGE_PAGE(8, 0x01),           // Generic Desktop
    HID_RI_USAGE(8, 0x80),                // System Control
    HID_RI_COLLECTION(8, 0x01),           // Application
        HID_RI_REPORT_ID(8, REPORT_ID_SYSTEM),
        HID_RI_USAGE_MINIMUM(8, 0x01),    // Pointer
        HID_RI_USAGE_MAXIMUM(16, 0x00B7), // System Display LCD Autoscale
        HID_RI_LOGICAL_MINIMUM(8, 0x01),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00B7),
        HID_RI_REPORT_COUNT(8, 1),
        HID_RI_REPORT_SIZE(8, 16),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),

    HID_RI_USAGE_PAGE(8, 0x0C),           // Consumer
    HID_RI_USAGE(8, 0x01),                // Consumer Control
    HID_RI_COLLECTION(8, 0x01),           // Application
        HID_RI_REPORT_ID(8, REPORT_ID_CONSUMER),
        HID_RI_USAGE_MINIMUM(8, 0x01),    // Consumer Control
        HID_RI_USAGE_MAXIMUM(16, 0x02A0), // AC Desktop Show All Applications
        HID_RI_LOGICAL_MINIMUM(8, 0x01),
        HID_RI_LOGICAL_MAXIMUM(16, 0x02A0),
        HID_RI_REPORT_COUNT(8, 1),
        HID_RI_REPORT_SIZE(8, 16),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),
#endif

#ifdef PROGRAMMABLE_BUTTON_ENABLE
    HID_RI_USAGE_PAGE(8, 0x0C),            // Consumer
    HID_RI_USAGE(8, 0x01),                 // Consumer Control
    HID_RI_COLLECTION(8, 0x01),            // Application
        HID_RI_REPORT_ID(8, REPORT_ID_PROGRAMMABLE_BUTTON),
        HID_RI_USAGE(8, 0x03),             // Programmable Buttons
        HID_RI_COLLECTION(8, 0x04),        // Named Array
            HID_RI_USAGE_PAGE(8, 0x09),    // Button
            HID_RI_USAGE_MINIMUM(8, 0x01), // Button 1
            HID_RI_USAGE_MAXIMUM(8, 0x20), // Button 32
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 32),
            HID_RI_REPORT_SIZE(8, 1),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
#endif

#ifdef NKRO_ENABLE
    HID_RI_USAGE_PAGE(8, 0x01),        // Generic Desktop
    HID_RI_USAGE(8, 0x06),             // Keyboard
    HID_RI_COLLECTION(8, 0x01),        // Application
        HID_RI_REPORT_ID(8, REPORT_ID_NKRO),
        // Modifiers (8 bits)
        HID_RI_USAGE_PAGE(8, 0x07),    // Keyboard/Keypad
        HID_RI_USAGE_MINIMUM(8, 0xE0), // Keyboard Left Control
        HID_RI_USAGE_MAXIMUM(8, 0xE7), // Keyboard Right GUI
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x08),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        // Keycodes
        HID_RI_USAGE_PAGE(8, 0x07),    // Keyboard/Keypad
        HID_RI_USAGE_MINIMUM(8, 0x00),
        HID_RI_USAGE_MAXIMUM(8, NKRO_REPORT_BITS * 8 - 1),
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, NKRO_REPORT_BITS * 8),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

        // Status LEDs (5 bits)
        HID_RI_USAGE_PAGE(8, 0x08),    // LED
        HID_RI_USAGE_MINIMUM(8, 0x01), // Num Lock
        HID_RI_USAGE_MAXIMUM(8, 0x05), // Kana
        HID_RI_REPORT_COUNT(8, 0x05),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
        // LED padding (3 bits)
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x03),
        HID_RI_OUTPUT(8, HID_IOF_CONSTANT),
    HID_RI_END_COLLECTION(0),
#endif

#ifdef SHARED_EP_ENABLE
};
#endif

#ifdef RAW_ENABLE
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM RawReport[] = {
    HID_RI_USAGE_PAGE(16, RAW_USAGE_PAGE), // Vendor Defined
    HID_RI_USAGE(8, RAW_USAGE_ID),         // Vendor Defined
    HID_RI_COLLECTION(8, 0x01),    // Application
        // Data to host
        HID_RI_USAGE(8, 0x62),     // Vendor Defined
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_COUNT(8, RAW_EPSIZE),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),

        // Data from host
        HID_RI_USAGE(8, 0x63),     // Vendor Defined
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_COUNT(8, RAW_EPSIZE),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE | HID_IOF_NON_VOLATILE),
    HID_RI_END_COLLECTION(0),
};
#endif

#ifdef CONSOLE_ENABLE
static const USB_Descriptor_HIDReport_Datatype_t PROGMEM ConsoleReport[] = {
    HID_RI_USAGE_PAGE(16, 0xFF31), // Vendor Defined (PJRC Teensy compatible)
    HID_RI_USAGE(8, 0x74),         // Vendor Defined (PJRC Teensy compatible)
    HID_RI_COLLECTION(8, 0x01),    // Application
        // Data to host
        HID_RI_USAGE(8, 0x75),     // Vendor Defined
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x00FF),
        HID_RI_REPORT_COUNT(8, CONSOLE_EPSIZE),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
    HID_RI_END_COLLECTION(0),
};
#endif

/*
 * Device descriptor
 */
static const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
    .Header = {
        .Size                   = sizeof(USB_Descriptor_Device_t),
        .Type                   = DTYPE_Device
    },
    .USBSpecification           = VERSION_BCD(2, 0, 0),

#if VIRTSER_ENABLE
    .Class                      = USB_CSCP_IADDeviceClass,
    .SubClass                   = USB_CSCP_IADDeviceSubclass,
    .Protocol                   = USB_CSCP_IADDeviceProtocol,
#else
    .Class                      = USB_CSCP_NoDeviceClass,
    .SubClass                   = USB_CSCP_NoDeviceSubclass,
    .Protocol                   = USB_CSCP_NoDeviceProtocol,
#endif

    .Endpoint0Size              = FIXED_CONTROL_ENDPOINT_SIZE,
    // Specified in config.h
    .VendorID                   = VENDOR_ID,
    .ProductID                  = PRODUCT_ID,
    .ReleaseNumber              = DEVICE_VER,
    .ManufacturerStrIndex       = 0x01,
    .ProductStrIndex            = 0x02,
#ifdef HAS_SERIAL_NUMBER
    .SerialNumStrIndex          = 0x03,
#else // HAS_SERIAL_NUMBER
    .SerialNumStrIndex          = 0x00,
#endif // HAS_SERIAL_NUMBER
    .NumberOfConfigurations     = FIXED_NUM_CONFIGURATIONS
};

#ifndef USB_MAX_POWER_CONSUMPTION
#    define USB_MAX_POWER_CONSUMPTION 500
#endif

#ifndef USB_POLLING_INTERVAL_MS
#    define USB_POLLING_INTERVAL_MS 1
#endif

/*
 * Configuration descriptors
 */
static const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
    .Config = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Configuration_Header_t),
            .Type               = DTYPE_Configuration
        },
        .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
        .TotalInterfaces        = TOTAL_INTERFACES,
        .ConfigurationNumber    = 1,
        .ConfigurationStrIndex  = NO_DESCRIPTOR,
        .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_REMOTEWAKEUP),
        .MaxPowerConsumption    = USB_CONFIG_POWER_MA(USB_MAX_POWER_CONSUMPTION)
    },
#ifndef KEYBOARD_SHARED_EP
    /*
     * Keyboard
     */
    .Keyboard_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = KEYBOARD_INTERFACE,
        .AlternateSetting       = 0x00,
        .TotalEndpoints         = 2,
        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_BootSubclass,
        .Protocol               = HID_CSCP_KeyboardBootProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Keyboard_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(KeyboardReport)
    },
    .Keyboard_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = KEYBOARD_EPIN_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = KEYBOARD_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
    .Keyboard_OUTEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = KEYBOARD_EPOUT_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = KEYBOARD_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
#endif

#ifdef RAW_ENABLE
    /*
     * Raw HID
     */
    .Raw_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = RAW_INTERFACE,
        .AlternateSetting       = 0x00,
        .TotalEndpoints         = 2,
        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Raw_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(RawReport)
    },
    .Raw_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = RAW_EPIN_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = RAW_EPSIZE,
        .PollingIntervalMS      = 0x01
    },
    .Raw_OUTEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = RAW_EPOUT_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = RAW_EPSIZE,
        .PollingIntervalMS      = 0x01
    },
#endif

#if defined(MOUSE_ENABLE) && !defined(MOUSE_SHARED_EP)
    /*
     * Mouse
     */
    .Mouse_Interface  = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = MOUSE_INTERFACE,
        .AlternateSetting       = 0x00,
        .TotalEndpoints         = 1,
        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_BootSubclass,
        .Protocol               = HID_CSCP_MouseBootProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Mouse_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(MouseReport)
    },
    .Mouse_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = MOUSE_EPIN_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = MOUSE_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
#endif

#ifdef SHARED_EP_ENABLE
    /*
     * Shared
     */
    .Shared_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = SHARED_INTERFACE,
        .AlternateSetting       = 0x00,
#    ifdef KEYBOARD_SHARED_EP
        .TotalEndpoints         = 2,
#    else
        .TotalEndpoints         = 1,
#    endif
        .Class                  = HID_CSCP_HIDClass,
#    ifdef KEYBOARD_SHARED_EP
        .SubClass               = HID_CSCP_BootSubclass,
        .Protocol               = HID_CSCP_KeyboardBootProtocol,
#    else
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,
#    endif
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Shared_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(SharedReport)
    },
    .Shared_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = SHARED_EPIN_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = SHARED_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
#ifdef KEYBOARD_SHARED_EP
    .Shared_OUTEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = SHARED_EPOUT_ADDR,
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = SHARED_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
#endif
#endif

#ifdef CONSOLE_ENABLE
    /*
     * Console
     */
    .Console_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = CONSOLE_INTERFACE,
        .AlternateSetting       = 0x00,
        .TotalEndpoints         = 1,
        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Console_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(ConsoleReport)
    },
    .Console_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = (ENDPOINT_DIR_IN | CONSOLE_IN_EPNUM),
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = CONSOLE_EPSIZE,
        .PollingIntervalMS      = 0x01
    },
#endif

#ifdef MIDI_ENABLE
    /*
     * MIDI
     */
    .Audio_Interface_Association = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_Association_t),
            .Type               = DTYPE_InterfaceAssociation
        },
        .FirstInterfaceIndex    = AC_INTERFACE,
        .TotalInterfaces        = 2,
        .Class                  = AUDIO_CSCP_AudioClass,
        .SubClass               = AUDIO_CSCP_ControlSubclass,
        .Protocol               = AUDIO_CSCP_ControlProtocol,
        .IADStrIndex            = NO_DESCRIPTOR,
    },
    .Audio_ControlInterface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = AC_INTERFACE,
        .AlternateSetting       = 0,
        .TotalEndpoints         = 0,
        .Class                  = AUDIO_CSCP_AudioClass,
        .SubClass               = AUDIO_CSCP_ControlSubclass,
        .Protocol               = AUDIO_CSCP_ControlProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Audio_ControlInterface_SPC = {
        .Header = {
            .Size               = sizeof(USB_Audio_Descriptor_Interface_AC_t),
            .Type               = AUDIO_DTYPE_CSInterface
        },
        .Subtype                = AUDIO_DSUBTYPE_CSInterface_Header,
        .ACSpecification        = VERSION_BCD(1, 0, 0),
        .TotalLength            = sizeof(USB_Audio_Descriptor_Interface_AC_t),
        .InCollection           = 1,
        .InterfaceNumber        = AS_INTERFACE,
    },
    .Audio_StreamInterface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = AS_INTERFACE,
        .AlternateSetting       = 0,
        .TotalEndpoints         = 2,
        .Class                  = AUDIO_CSCP_AudioClass,
        .SubClass               = AUDIO_CSCP_MIDIStreamingSubclass,
        .Protocol               = AUDIO_CSCP_StreamingProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Audio_StreamInterface_SPC = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_AudioInterface_AS_t),
            .Type               = AUDIO_DTYPE_CSInterface
        },
        .Subtype                = AUDIO_DSUBTYPE_CSInterface_General,
        .AudioSpecification     = VERSION_BCD(1, 0, 0),
        .TotalLength            = offsetof(USB_Descriptor_Configuration_t, MIDI_Out_Jack_Endpoint_SPC) + sizeof(USB_MIDI_Descriptor_Jack_Endpoint_t) - offsetof(USB_Descriptor_Configuration_t, Audio_StreamInterface_SPC)
    },
    .MIDI_In_Jack_Emb = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_InputJack_t),
            .Type               = AUDIO_DTYPE_CSInterface
        },
        .Subtype                = AUDIO_DSUBTYPE_CSInterface_InputTerminal,
        .JackType               = MIDI_JACKTYPE_Embedded,
        .JackID = 0x01,
        .JackStrIndex           = NO_DESCRIPTOR
    },
    .MIDI_In_Jack_Ext = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_InputJack_t),
            .Type               = AUDIO_DTYPE_CSInterface
        },
        .Subtype                = AUDIO_DSUBTYPE_CSInterface_InputTerminal,
        .JackType               = MIDI_JACKTYPE_External,
        .JackID                 = 0x02,
        .JackStrIndex           = NO_DESCRIPTOR
    },
    .MIDI_Out_Jack_Emb = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_OutputJack_t),
            .Type               = AUDIO_DTYPE_CSInterface
        },
        .Subtype                = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,
        .JackType               = MIDI_JACKTYPE_Embedded,
        .JackID                 = 0x03,
        .NumberOfPins           = 1,
        .SourceJackID           = {0x02},
        .SourcePinID            = {0x01},
        .JackStrIndex           = NO_DESCRIPTOR
    },
    .MIDI_Out_Jack_Ext = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_OutputJack_t),
            .Type               = AUDIO_DTYPE_CSInterface
        },
        .Subtype                = AUDIO_DSUBTYPE_CSInterface_OutputTerminal,
        .JackType               = MIDI_JACKTYPE_External,
        .JackID                 = 0x04,
        .NumberOfPins           = 1,
        .SourceJackID           = {0x01},
        .SourcePinID            = {0x01},
        .JackStrIndex           = NO_DESCRIPTOR
    },
    .MIDI_In_Jack_Endpoint = {
        .Endpoint = {
            .Header = {
                .Size           = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t),
                .Type           = DTYPE_Endpoint
            },
            .EndpointAddress    = (ENDPOINT_DIR_OUT | MIDI_STREAM_OUT_EPNUM),
            .Attributes         = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize       = MIDI_STREAM_EPSIZE,
            .PollingIntervalMS  = 0x05
        },
        .Refresh                = 0,
        .SyncEndpointNumber     = 0
    },
    .MIDI_In_Jack_Endpoint_SPC = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_Jack_Endpoint_t),
            .Type               = AUDIO_DTYPE_CSEndpoint
        },
        .Subtype                = AUDIO_DSUBTYPE_CSEndpoint_General,
        .TotalEmbeddedJacks     = 0x01,
        .AssociatedJackID       = {0x01}
    },
    .MIDI_Out_Jack_Endpoint = {
        .Endpoint = {
            .Header = {
                .Size           = sizeof(USB_Audio_Descriptor_StreamEndpoint_Std_t),
                .Type           = DTYPE_Endpoint
            },
            .EndpointAddress    = (ENDPOINT_DIR_IN | MIDI_STREAM_IN_EPNUM),
            .Attributes         = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize       = MIDI_STREAM_EPSIZE,
            .PollingIntervalMS  = 0x05
        },
        .Refresh                = 0,
        .SyncEndpointNumber     = 0
    },
    .MIDI_Out_Jack_Endpoint_SPC = {
        .Header = {
            .Size               = sizeof(USB_MIDI_Descriptor_Jack_Endpoint_t),
            .Type               = AUDIO_DTYPE_CSEndpoint
        },
        .Subtype                = AUDIO_DSUBTYPE_CSEndpoint_General,
        .TotalEmbeddedJacks     = 0x01,
        .AssociatedJackID       = {0x03}
    },
#endif

#ifdef VIRTSER_ENABLE
    /*
     * Virtual Serial
     */
    .CDC_Interface_Association = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_Association_t),
            .Type               = DTYPE_InterfaceAssociation
        },
        .FirstInterfaceIndex    = CCI_INTERFACE,
        .TotalInterfaces        = 2,
        .Class                  = CDC_CSCP_CDCClass,
        .SubClass               = CDC_CSCP_ACMSubclass,
        .Protocol               = CDC_CSCP_ATCommandProtocol,
        .IADStrIndex            = NO_DESCRIPTOR,
    },
    .CDC_CCI_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = CCI_INTERFACE,
        .AlternateSetting       = 0,
        .TotalEndpoints         = 1,
        .Class                  = CDC_CSCP_CDCClass,
        .SubClass               = CDC_CSCP_ACMSubclass,
        .Protocol               = CDC_CSCP_ATCommandProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .CDC_Functional_Header = {
        .Header = {
            .Size               = sizeof(USB_CDC_Descriptor_FunctionalHeader_t),
            .Type               = CDC_DTYPE_CSInterface
        },
        .Subtype                = 0x00,
        .CDCSpecification       = VERSION_BCD(1, 1, 0),
    },
    .CDC_Functional_ACM = {
        .Header = {
            .Size               = sizeof(USB_CDC_Descriptor_FunctionalACM_t),
            .Type               = CDC_DTYPE_CSInterface
        },
        .Subtype                = 0x02,
        .Capabilities           = 0x02,
    },
    .CDC_Functional_Union = {
        .Header = {
            .Size               = sizeof(USB_CDC_Descriptor_FunctionalUnion_t),
            .Type               = CDC_DTYPE_CSInterface
        },
        .Subtype                = 0x06,
        .MasterInterfaceNumber  = CCI_INTERFACE,
        .SlaveInterfaceNumber   = CDI_INTERFACE,
    },
    .CDC_NotificationEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = (ENDPOINT_DIR_IN | CDC_NOTIFICATION_EPNUM),
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
        .PollingIntervalMS      = 0xFF
    },
    .CDC_DCI_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = CDI_INTERFACE,
        .AlternateSetting       = 0,
        .TotalEndpoints         = 2,
        .Class                  = CDC_CSCP_CDCDataClass,
        .SubClass               = CDC_CSCP_NoDataSubclass,
        .Protocol               = CDC_CSCP_NoDataProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .CDC_DataOutEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = (ENDPOINT_DIR_OUT | CDC_OUT_EPNUM),
        .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = CDC_EPSIZE,
        .PollingIntervalMS      = 0x05
    },
    .CDC_DataInEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = (ENDPOINT_DIR_IN | CDC_IN_EPNUM),
        .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = CDC_EPSIZE,
        .PollingIntervalMS      = 0x05
    },
#endif

#if defined(JOYSTICK_ENABLE) && !defined(JOYSTICK_SHARED_EP)
    /*
     * Joystick
     */
    .Joystick_Interface = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = JOYSTICK_INTERFACE,
        .AlternateSetting       = 0x00,
        .TotalEndpoints         = 1,
        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Joystick_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(JoystickReport)
    },
    .Joystick_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = (ENDPOINT_DIR_IN | JOYSTICK_IN_EPNUM),
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = JOYSTICK_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
#endif

#if defined(DIGITIZER_ENABLE) && !defined(DIGITIZER_SHARED_EP)
    /*
     * Digitizer
     */
    .Digitizer_Interface  = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Interface_t),
            .Type               = DTYPE_Interface
        },
        .InterfaceNumber        = DIGITIZER_INTERFACE,
        .AlternateSetting       = 0x00,
        .TotalEndpoints         = 1,
        .Class                  = HID_CSCP_HIDClass,
        .SubClass               = HID_CSCP_NonBootSubclass,
        .Protocol               = HID_CSCP_NonBootProtocol,
        .InterfaceStrIndex      = NO_DESCRIPTOR
    },
    .Digitizer_HID = {
        .Header = {
            .Size               = sizeof(USB_HID_Descriptor_HID_t),
            .Type               = HID_DTYPE_HID
        },
        .HIDSpec                = VERSION_BCD(1, 1, 1),
        .CountryCode            = 0x00,
        .TotalReportDescriptors = 1,
        .HIDReportType          = HID_DTYPE_Report,
        .HIDReportLength        = sizeof(DigitizerReport)
    },
    .Digitizer_INEndpoint = {
        .Header = {
            .Size               = sizeof(USB_Descriptor_Endpoint_t),
            .Type               = DTYPE_Endpoint
        },
        .EndpointAddress        = (ENDPOINT_DIR_IN | DIGITIZER_IN_EPNUM),
        .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
        .EndpointSize           = DIGITIZER_EPSIZE,
        .PollingIntervalMS      = USB_POLLING_INTERVAL_MS
    },
#endif
};

/*
 * String descriptors
 */

#define USB_DESCRIPTOR_SIZE_LITERAL_U16STRING(str) \
    (sizeof(USB_Descriptor_Header_t) + sizeof(str) - sizeof(wchar_t)) // include header, don't count null terminator

static const USB_Descriptor_String_t PROGMEM LanguageString = {
    .Header = {
        .Size                   = sizeof(USB_Descriptor_Header_t) + sizeof(uint16_t),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = {LANGUAGE_ID_ENG}
};

static const USB_Descriptor_String_t PROGMEM ManufacturerString = {
    .Header = {
        .Size                   = USB_DESCRIPTOR_SIZE_LITERAL_U16STRING(USBSTR(MANUFACTURER)),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = USBSTR(MANUFACTURER)
};

static const USB_Descriptor_String_t PROGMEM ProductString = {
    .Header = {
        .Size                   = USB_DESCRIPTOR_SIZE_LITERAL_U16STRING(USBSTR(PRODUCT)),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = USBSTR(PRODUCT)
};

// clang-format on

#if defined(SERIAL_NUMBER)
// clang-format off
static const USB_Descriptor_String_t PROGMEM SerialNumberString = {
    .Header = {
        .Size                   = USB_DESCRIPTOR_SIZE_LITERAL_U16STRING(USBSTR(SERIAL_NUMBER)),
        .Type                   = DTYPE_String
    },
    .UnicodeString              = USBSTR(SERIAL_NUMBER)
};
// clang-format on

#else // defined(SERIAL_NUMBER)

#    if defined(SERIAL_NUMBER_USE_HARDWARE_ID) && SERIAL_NUMBER_USE_HARDWARE_ID == TRUE

#        if defined(__AVR__)
#            error Dynamically setting the serial number on AVR is unsupported as LUFA requires the string to be in PROGMEM.
#        endif // defined(__AVR__)

#        ifndef SERIAL_NUMBER_LENGTH
#            define SERIAL_NUMBER_LENGTH (sizeof(hardware_id_t) * 2)
#        endif

#        define SERIAL_NUMBER_DESCRIPTOR_SIZE                                            \
            (sizeof(USB_Descriptor_Header_t)                     /* Descriptor header */ \
             + (((SERIAL_NUMBER_LENGTH) + 1) * sizeof(wchar_t))) /* Length of serial number, with potential extra character as we're converting 2 nibbles at a time */

uint8_t SerialNumberString[SERIAL_NUMBER_DESCRIPTOR_SIZE] = {0};

void set_serial_number_descriptor(void) {
    static bool is_set = false;
    if (is_set) {
        return;
    }
    is_set = true;

    static const char        hex_str[] = "0123456789ABCDEF";
    hardware_id_t            id        = get_hardware_id();
    USB_Descriptor_String_t* desc      = (USB_Descriptor_String_t*)SerialNumberString;

    // Copy across nibbles from the hardware ID as unicode hex characters
    int      length = MIN(sizeof(id) * 2, SERIAL_NUMBER_LENGTH);
    uint8_t* p      = (uint8_t*)&id;
    for (int i = 0; i < length; i += 2) {
        desc->UnicodeString[i + 0] = hex_str[p[i / 2] >> 4];
        desc->UnicodeString[i + 1] = hex_str[p[i / 2] & 0xF];
    }

    desc->Header.Size = sizeof(USB_Descriptor_Header_t) + (length * sizeof(wchar_t)); // includes header, don't count null terminator
    desc->Header.Type = DTYPE_String;
}

#    endif // defined(SERIAL_NUMBER_USE_HARDWARE_ID) && SERIAL_NUMBER_USE_HARDWARE_ID == TRUE

#endif // defined(SERIAL_NUMBER)

