/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef COMMAND_H_
#define COMMAND_H_

#include"stdint.h"
#include"keyboard.h"

extern const uint8_t command_advanced_key_mapping[ADVANCED_KEY_NUM];

void command_prase(uint8_t *buf,uint8_t len);
void start_load_cargo(void);
int load_cargo(void);
int hid_send(uint8_t *report, uint16_t len);

#endif