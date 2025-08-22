/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdlib.h"
#include "analog.h"
#include "rgb.h"
#include "keyboard.h"
#include "keyboard_conf.h"
#include "math.h"
#include "lfs.h"
#include "sfud.h"
#include "keyboard_def.h"
#include "usbd_user.h"
#include "snake.h"
#include "packet.h"
#include "layer.h"
#include "qmk_midi.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define rgb_start() HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_1, (uint32_t *)g_rgb_buffer, RGB_BUFFER_LENGTH);
#define rgb_stop() HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_1);

#define DMA_BUF_LEN             10
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

sfud_flash sfud_norflash0 = {
    .name = "norflash0",
    .spi.name = "SPI1",
    .chip = {"W25Q128JV", SFUD_MF_ID_WINBOND, 0x40, 0x18, 16L * 1024L * 1024L, SFUD_WM_PAGE_256B, 4096, 0x20},
};

uint32_t pulse_counter = 0;
bool beep_switch = 0;
bool em_switch = 0;

uint32_t ADC_Buffer[4*DMA_BUF_LEN];

extern volatile uint8_t low_latency_mode;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// 重定向print start
#if defined (__ARMCC_VERSION) /* ARM Compiler */
int fputc(int ch, FILE *f)
{
  while ((USART1->ISR & USART_ISR_TXE) == 0)
      ;
  USART1->TDR = ch;
  return ch;
}
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
//_write函數在syscalls.c中， 使用__weak定义以可以直接在其他文件中定义_write函數
__attribute__((weak)) int _write(int file, char *ptr, int len)
{
  UNUSED(file);
  if (HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, 0xffff) != HAL_OK)
  {
    Error_Handler();
  }
  return HAL_OK;
}
#endif
// 重定向print end

void key_down_cb(void * k)
{
  UNUSED(k);
  pulse_counter=PULSE_LEN_MS;
}
/**
 * @brief  初始化时间戳
 * @note   使用延时函数前，必须调用本函数
 */
int DWT_Init(void)
{
  /* Disable TRC */
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
  /* Enable TRC */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

  /* Disable clock cycle counter */
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
  /* Enable  clock cycle counter */
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 0x00000001;

  /* Reset the clock cycle counter value */
  DWT->CYCCNT = 0;

  /* 3 NO OPERATION instructions */
  __ASM volatile("NOP");
  __ASM volatile("NOP");
  __ASM volatile("NOP");

  /* Check if clock cycle counter has started */
  if (DWT->CYCCNT)
  {
    return 0; /*clock cycle counter started*/
  }
  else
  {
    return 1; /*clock cycle counter not started*/
  }
}
// This Function Provides Delay In Microseconds Using DWT

void DWT_Delay_us(volatile uint32_t au32_microseconds)
{
  uint32_t au32_initial_ticks = DWT->CYCCNT;
  uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
  au32_microseconds *= au32_ticks;
  while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds - au32_ticks)
    ;
}

void (*SysMemBootJump)(void);
__IO uint32_t BootAddr = 0x1FFFD800; /* BootLoader 地址 */

void JumpToBootloader(void)
{
  uint32_t i = 0;

  /* 关闭全局中断 */
  __set_PRIMASK(1);

  /* 关闭滴答定时器，复位到默认值 */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* 设置所有时钟到默认状态，使用HSI时钟 */
  HAL_RCC_DeInit();

  /* 关闭所有中断，清除所有中断挂起标志 */
  for (i = 0; i < 8; i++)
  {
    NVIC->ICER[i] = 0xFFFFFFFF;
    NVIC->ICPR[i] = 0xFFFFFFFF;
  }

  /* 使能全局中断 */
  __set_PRIMASK(0);

  /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
  SysMemBootJump = (void (*)(void))(*((uint32_t *)(BootAddr + 4)));

  /* 设置主堆栈指针 */
  __set_MSP(*(uint32_t *)BootAddr);

  /* 跳转到系统BootLoader */
  SysMemBootJump();

  /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
  while (1)
  {
  }
}

void usb_dc_low_level_init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  /**USB GPIO Configuration
  PA11     ------> USB_DM
  PA12     ------> USB_DP
  */
  GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_USB;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /* Peripheral clock enable */
  __HAL_RCC_USB_CLK_ENABLE();
  /* USB interrupt Init */
  HAL_NVIC_SetPriority(USB_LP_CAN_RX0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USB_LP_CAN_RX0_IRQn);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_ADC4_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  //!!!
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);

  DWT_Init();
  sfud_device_init(&sfud_norflash0);

  HAL_GPIO_WritePin(INHIBIT_GPIO_Port, INHIBIT_Pin, GPIO_PIN_RESET);
  keyboard_init();
  for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
  {
    key_attach(&g_keyboard_advanced_keys[i].key,KEY_EVENT_DOWN,key_down_cb);
  }

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED);
  rgb_start();

  HAL_ADC_Start_DMA(&hadc1, ADC_Buffer + DMA_BUF_LEN*0, DMA_BUF_LEN);
  HAL_ADC_Start_DMA(&hadc2, ADC_Buffer + DMA_BUF_LEN*1, DMA_BUF_LEN);
  HAL_ADC_Start_DMA(&hadc3, ADC_Buffer + DMA_BUF_LEN*2, DMA_BUF_LEN);
  HAL_ADC_Start_DMA(&hadc4, ADC_Buffer + DMA_BUF_LEN*3, DMA_BUF_LEN);

  HAL_TIM_Base_Start_IT(&htim2);

  rgb_init_flash();

  analog_reset_range();

  if (g_keyboard_advanced_keys[0].raw < 1400 || g_keyboard_advanced_keys[0].raw > (4096 - 1400))
  {
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        rgb_set(i, 100, 0, 0);
    }
    HAL_Delay(2);
    JumpToBootloader();
  }
  if (g_keyboard_advanced_keys[13].raw < 1400 || g_keyboard_advanced_keys[13].raw > (4096 - 1400))
  {
    keyboard_reset_to_default();
    keyboard_save();
    keyboard_reboot();
  }
  if (g_keyboard_advanced_keys[60].raw < 1400 || g_keyboard_advanced_keys[60].raw > (4096 - 1400))
  {
    keyboard_factory_reset();
    keyboard_reboot();
  }
  usb_init();
  g_keyboard_config.nkro = true;
  HAL_TIM_Base_Start_IT(&htim7);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (low_latency_mode == 1)
    {
      low_latency_mode = 2;
      rgb_turn_off();
    }
    
    if (low_latency_mode)
    {
      keyboard_task();
      if(g_keyboard_led_state&BIT(1))
      {
        rgb_set(g_rgb_mapping[28],  0xff, 0xff, 0xff);
      }
      else
      {
        rgb_set(g_rgb_mapping[28],  0, 0, 0);
      }
      if(g_keyboard_led_state&BIT(2))
      {
        rgb_set(g_rgb_mapping[26],  0xff, 0xff, 0xff);
      }
      else
      {
        rgb_set(g_rgb_mapping[26],  0, 0, 0);
      }
      if (g_current_layer == 2)
      {
        rgb_set(g_rgb_mapping[0], 0xff, 0, 0);
        rgb_set(g_rgb_mapping[1], 0, 0, 0);
        rgb_set(g_rgb_mapping[2], 0, 0, 0);
        rgb_set(g_rgb_mapping[3], 0, 0, 0);
        rgb_set(g_rgb_mapping[4], 0, 0, 0);
        rgb_set(g_rgb_mapping[g_current_config_index+1], 0xff, 0xff, 0xff);
        rgb_set(g_rgb_mapping[37], 0xff, 0xff, 0xff);
        if (g_keyboard_config.nkro)
          rgb_set(g_rgb_mapping[19], 0xff, 0xff, 0xff);
        else
          rgb_set(g_rgb_mapping[19], 0, 0, 0);
        if (g_keyboard_config.debug)
          rgb_set(g_rgb_mapping[31], 0xff, 0xff, 0xff);
        else
          rgb_set(g_rgb_mapping[31], 0, 0, 0);
        if (beep_switch)
          rgb_set(g_rgb_mapping[46], 0xff, 0xff, 0xff);
        else
          rgb_set(g_rgb_mapping[46], 0, 0, 0);
        if (em_switch)
          rgb_set(g_rgb_mapping[45], 0xff, 0xff, 0xff);
        else
          rgb_set(g_rgb_mapping[45], 0, 0, 0);
        if (g_keyboard_config.winlock)
          rgb_set(g_rgb_mapping[56], 0xff, 0xff, 0xff);
        else
          rgb_set(g_rgb_mapping[56], 0, 0, 0);
      }
      else
      {
        rgb_set(g_rgb_mapping[0], 0, 0, 0);
        rgb_set(g_rgb_mapping[1], 0, 0, 0);
        rgb_set(g_rgb_mapping[2], 0, 0, 0);
        rgb_set(g_rgb_mapping[3], 0, 0, 0);
        rgb_set(g_rgb_mapping[4], 0, 0, 0);
        rgb_set(g_rgb_mapping[19], 0, 0, 0);
        rgb_set(g_rgb_mapping[31], 0, 0, 0);
        rgb_set(g_rgb_mapping[46], 0, 0, 0);
        rgb_set(g_rgb_mapping[45], 0, 0, 0);
        rgb_set(g_rgb_mapping[37], 0, 0, 0);
        rgb_set(g_rgb_mapping[56], 0, 0, 0);
      }
    }
    else
    {
      rgb_update();
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_TIM1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM7)
  {
    if (!low_latency_mode)
    {
      keyboard_task();
      if (pulse_counter)
      {
        pulse_counter--;
        if (beep_switch)
        {
          HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
        }
        if (em_switch)
        {
          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
        }
      }
      else
      {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
        HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
      }
    }
  }
  if (htim->Instance == TIM2)
  {
    uint32_t adc1 = 0;
    uint32_t adc2 = 0;
    uint32_t adc3 = 0;
    uint32_t adc4 = 0;

    for (int i = 0; i < DMA_BUF_LEN; i++)
    {
      adc1 += ADC_Buffer[DMA_BUF_LEN * 0 + i];
      adc2 += ADC_Buffer[DMA_BUF_LEN * 1 + i];
      adc3 += ADC_Buffer[DMA_BUF_LEN * 2 + i];
      adc4 += ADC_Buffer[DMA_BUF_LEN * 3 + i];
    }

    ringbuf_push(&g_adc_ringbufs[0 * 16 + BCD_TO_GRAY(g_analog_active_channel)], (float)adc1 / (float)DMA_BUF_LEN);
    ringbuf_push(&g_adc_ringbufs[1 * 16 + BCD_TO_GRAY(g_analog_active_channel)], (float)adc2 / (float)DMA_BUF_LEN);
    ringbuf_push(&g_adc_ringbufs[2 * 16 + BCD_TO_GRAY(g_analog_active_channel)], (float)adc3 / (float)DMA_BUF_LEN);
    ringbuf_push(&g_adc_ringbufs[3 * 16 + BCD_TO_GRAY(g_analog_active_channel)], (float)adc4 / (float)DMA_BUF_LEN);

    if (htim->Instance->CNT < 700)
    {
      g_analog_active_channel++;
      if (g_analog_active_channel >= 16)
      {
        g_analog_active_channel = 0;
      }
      analog_channel_select(g_analog_active_channel);
    }
  }
}

void rgb_update_callback()
{
  if (g_snake.running)
  {
    snake_move(&g_snake);
    draw_snake(&g_snake);
    return;
  }
	if(g_keyboard_led_state&BIT(1))
  {
	  g_rgb_colors[g_rgb_mapping[28]].r = 0xff;
	  g_rgb_colors[g_rgb_mapping[28]].g = 0xff;
	  g_rgb_colors[g_rgb_mapping[28]].b = 0xff;//cap lock
	}
	if(g_keyboard_led_state&BIT(2))
  {
	  g_rgb_colors[g_rgb_mapping[26]].r = 0xff;
	  g_rgb_colors[g_rgb_mapping[26]].g = 0xff;
	  g_rgb_colors[g_rgb_mapping[26]].b = 0xff;//cap lock
	}
  if (g_current_layer == 2)
  {
	  g_rgb_colors[g_rgb_mapping[0]].r = 0xff;
	  g_rgb_colors[g_rgb_mapping[0]].g = 0;
	  g_rgb_colors[g_rgb_mapping[0]].b = 0;
	  g_rgb_colors[g_rgb_mapping[1]].r = 0;
	  g_rgb_colors[g_rgb_mapping[1]].g = 0;
	  g_rgb_colors[g_rgb_mapping[1]].b = 0;
	  g_rgb_colors[g_rgb_mapping[2]].r = 0;
	  g_rgb_colors[g_rgb_mapping[2]].g = 0;
	  g_rgb_colors[g_rgb_mapping[2]].b = 0;
	  g_rgb_colors[g_rgb_mapping[3]].r = 0;
	  g_rgb_colors[g_rgb_mapping[3]].g = 0;
	  g_rgb_colors[g_rgb_mapping[3]].b = 0;
	  g_rgb_colors[g_rgb_mapping[4]].r = 0;
	  g_rgb_colors[g_rgb_mapping[4]].g = 0;
	  g_rgb_colors[g_rgb_mapping[4]].b = 0;
	  g_rgb_colors[g_rgb_mapping[g_current_config_index+1]].r = 0xff;
	  g_rgb_colors[g_rgb_mapping[g_current_config_index+1]].g = 0xff;
	  g_rgb_colors[g_rgb_mapping[g_current_config_index+1]].b = 0xff;
    if (g_keyboard_config.nkro)
    {
      g_rgb_colors[g_rgb_mapping[19]].r = 0xff;
      g_rgb_colors[g_rgb_mapping[19]].g = 0xff;
      g_rgb_colors[g_rgb_mapping[19]].b = 0xff;
    }
    if (g_keyboard_config.debug)
    {
      g_rgb_colors[g_rgb_mapping[31]].r = 0xff;
      g_rgb_colors[g_rgb_mapping[31]].g = 0xff;
      g_rgb_colors[g_rgb_mapping[31]].b = 0xff;
    }
    if (beep_switch)
    {
      g_rgb_colors[g_rgb_mapping[46]].r = 0xff;
      g_rgb_colors[g_rgb_mapping[46]].g = 0xff;
      g_rgb_colors[g_rgb_mapping[46]].b = 0xff;
    }
    if (em_switch)
    {
      g_rgb_colors[g_rgb_mapping[45]].r = 0xff;
      g_rgb_colors[g_rgb_mapping[45]].g = 0xff;
      g_rgb_colors[g_rgb_mapping[45]].b = 0xff;
    }
    if (low_latency_mode)
    {
      g_rgb_colors[g_rgb_mapping[37]].r = 0xff;
      g_rgb_colors[g_rgb_mapping[37]].g = 0xff;
      g_rgb_colors[g_rgb_mapping[37]].b = 0xff;
    }
    if (g_keyboard_config.winlock)
    {
      g_rgb_colors[g_rgb_mapping[56]].r = 0xff;
      g_rgb_colors[g_rgb_mapping[56]].g = 0xff;
      g_rgb_colors[g_rgb_mapping[56]].b = 0xff;
    }
  }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
  UNUSED(hadc);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
  UNUSED(hadc);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
