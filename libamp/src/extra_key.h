/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef EXTRA_KEY_H
#define EXTRA_KEY_H
#include "keyboard.h"

#ifdef __cplusplus
extern "C" {
#endif

void extra_key_event_handler(KeyboardEvent event);
int consumer_key_buffer_send(void);
int system_key_buffer_send(void);
int extra_key_hid_send(uint8_t report_id, uint16_t usage);

#ifdef __cplusplus
}
#endif

#endif //EXTRA_KEY_H
