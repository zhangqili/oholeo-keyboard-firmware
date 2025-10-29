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
    LL_DMA_SetPeriphAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)&TIM1->CCR1);
    LL_DMA_SetMemoryAddress(DMA1, LL_DMA_CHANNEL_2, (uint32_t)ws2812_buffer);
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, WS2812_BUFFER_LENGTH);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_TIM_EnableDMAReq_CC1(TIM1);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_EnableCounter(TIM1);
    //HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)ws2812_buffer, WS2812_BUFFER_LENGTH);
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
