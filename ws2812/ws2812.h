/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef WS2812_H_
#define WS2812_H_

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WS2812_LED_NUM          (64)
#define ONE_PULSE               (60)
#define ZERO_PULSE              (29)
#define NONE_PULSE              (0)
#define WS2812_RESET_LENGTH     (400)
#define WS2812_BUFFER_LENGTH    (((WS2812_LED_NUM)*(3*8))+WS2812_RESET_LENGTH)

void ws2812_init(void);
void ws2812_set(uint16_t index, uint8_t r, uint8_t g, uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* WS2812_H_ */
