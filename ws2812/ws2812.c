/*
 * Copyright (c) 2025 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: MIT
 */
#include "ws2812.h"
#include "tim.h"

static uint8_t ws2812_buffer[WS2812_BUFFER_LENGTH];

void ws2812_init(void)
{
    for (uint16_t i = 0; i < WS2812_BUFFER_LENGTH; i++)
    {
        ws2812_buffer[i] = NONE_PULSE;
    }
    HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)ws2812_buffer, WS2812_BUFFER_LENGTH);
}

void ws2812_set(uint16_t index, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint8_t i = 0; i < 8; i++)
    {
        ws2812_buffer[WS2812_RESET_LENGTH + index * 24 + i] = (g << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        ws2812_buffer[WS2812_RESET_LENGTH + index * 24 + i + 8] = (r << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
        ws2812_buffer[WS2812_RESET_LENGTH + index * 24 + i + 16] = (b << i) & (0x80) ? ONE_PULSE : ZERO_PULSE;
    }
}
