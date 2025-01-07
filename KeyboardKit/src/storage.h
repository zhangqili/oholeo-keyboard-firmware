/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef STORAGE_H_
#define STORAGE_H_

#include "stdint.h"

int storage_mount(void);
void storage_unmount(void);
uint8_t storage_read_config_index(void);
void storage_save_config_index(uint8_t index);
void storage_read_config(uint8_t index);
void storage_save_config(uint8_t index);

#endif