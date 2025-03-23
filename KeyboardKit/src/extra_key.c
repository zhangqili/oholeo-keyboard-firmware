/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "extra_key.h"
#include "report.h"

static uint16_t consumer_buffer;
static uint16_t system_buffer;

void extra_key_event_handler(KeyboardEvent event)
{
    switch (event.event)
    {
    case KEYBOARD_EVENT_KEY_UP:
        switch (KEYCODE(event.keycode))
        {
        case CONSUMER_COLLECTION:
            consumer_buffer = 0;
            BIT_SET(g_keyboard_send_flags, CONSUMER_REPORT_FLAG);
            break;
        case SYSTEM_COLLECTION:
            system_buffer = 0;
            BIT_SET(g_keyboard_send_flags, SYSTEM_REPORT_FLAG);
            break;
        default:
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_DOWN:
        switch (KEYCODE(event.keycode))
        {
        case CONSUMER_COLLECTION:
            consumer_buffer = CONSUMER_KEYCODE_TO_RAWCODE(MODIFIER(event.keycode));
            BIT_SET(g_keyboard_send_flags, CONSUMER_REPORT_FLAG);
            break;
        case SYSTEM_COLLECTION:
            system_buffer = MODIFIER(event.keycode);
            BIT_SET(g_keyboard_send_flags, SYSTEM_REPORT_FLAG);
            break;
        default:
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_TRUE:
        switch (KEYCODE(event.keycode))
        {
        case CONSUMER_COLLECTION:
            if (!consumer_buffer)
            {
                consumer_buffer = CONSUMER_KEYCODE_TO_RAWCODE(MODIFIER(event.keycode));
            }
                break;
        case SYSTEM_COLLECTION:
            if (!system_buffer)
            {
                system_buffer = MODIFIER(event.keycode);
            }
        default:
            break;
        }
        break;
    case KEYBOARD_EVENT_KEY_FALSE:
        break;
    default:
        break;
    }
}

int consumer_key_buffer_send(void)
{
    return extra_key_hid_send(REPORT_ID_CONSUMER, consumer_buffer);
}

int system_key_buffer_send(void)
{
    return extra_key_hid_send(REPORT_ID_SYSTEM, system_buffer);
}

__WEAK int extra_key_hid_send(uint8_t report_id, uint16_t usage)
{
    UNUSED(report_id);
    UNUSED(usage);
    return 0;
}