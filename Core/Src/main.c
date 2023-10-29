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
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "stdlib.h"
#include "analog.h"
#include "rgb.h"
#include "keyboard.h"
#include "keyboard_conf.h"
#include "lefl.h"
#include "usbd_custom_hid_if.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
bool RGB_Update_Flag=false;
char uart_buf[64];
uint32_t debug;
uint32_t max32;
uint32_t min32=0xFFFFFFFF;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
* @brief  初始化时间戳
* @param  �???????????????????????????????????????
* @retval �???????????????????????????????????????
* @note   使用延时函数前，必须调用本函�???????????????????????????????????????
*/
int DWT_Init(void)
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
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
  while ((DWT->CYCCNT - au32_initial_ticks) < au32_microseconds-au32_ticks);
}

void (*SysMemBootJump)(void); /* 声明�???????????????????个函数指�??????????????????? */
__IO uint32_t BootAddr = 0x1FFFD800; /*   的系�??????????????????? BootLoader 地址 */

void JumpToBootloader(void) {
  uint32_t i=0;

  /* 关闭全局中断 */
  __set_PRIMASK(1);

  /* 关闭滴答定时器，复位到默认�?? */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* 设置�???????????????????有时钟到默认状�?�， 使用 HSI 时钟 */
  HAL_RCC_DeInit();

  /* 关闭�???????????????????有中断，清除�???????????????????有中断挂起标�??????????????????? */
  for (i = 0; i < 8; i++)
  {
      NVIC->ICER[i]=0xFFFFFFFF;
      NVIC->ICPR[i]=0xFFFFFFFF;
  }

  /* 使能全局中断 */
  __set_PRIMASK(0);

  /* 跳转到系�??????????????????? BootLoader，首地址�??????????????????? MSP，地�???????????????????+4 是复位中断服务程序地�??????????????????? */
  SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

  /* 设置主堆栈指�??????????????????? */
  __set_MSP(*(uint32_t *)BootAddr);

  /* 跳转到系�??????????????????? BootLoader */
  SysMemBootJump();

  /* 跳转成功的话，不会执行到这里，用户可以在这里添加代码 */
  while (1)
  {

  }
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
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  DWT_Init();
  lefl_bit_array_init(&Keyboard_KeyArray, (size_t*)(Keyboard_ReportBuffer+2), 168);

  HAL_GPIO_WritePin(INHIBIT_GPIO_Port, INHIBIT_Pin, GPIO_PIN_RESET);
  Keyboard_Init();
  RGB_Init();
  Analog_Recovery();
  RGB_Recovery();

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED);
  RGB_Start();
  //HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);

  //Analog_Init();
  RGB_Flash();
  RGB_TurnOff();

  //Analog_Start();
  /* Analog Calibration BEGIN */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  HAL_ADC_Start(&hadc2);
  HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)(Analog_Buffer+0), 1);
  HAL_ADC_Start(&hadc4);
  HAL_ADCEx_MultiModeStart_DMA(&hadc3, (uint32_t*)(Analog_Buffer+2), 1);
  HAL_Delay(1000);
  if(AnalogDatas[49].sum/AnalogDatas[49].count<16384)
  {
      JumpToBootloader();
  }
  for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
  {
      lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, AnalogDatas[i].sum/AnalogDatas[i].count, 7200);
      lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, DEFAULT_UPPER_DEADZONE, DEFAULT_LOWER_DEADZONE);
  }
  Analog_Flush();
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start_IT(&htim6);
  //sprintf(uart_buf,"%f\n",Keyboard_AdvancedKeys[0].upper_bound);
  //HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, 64, 0xFF);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      HAL_Delay(1);
      //sprintf(uart_buf,"/%d=%d\n",AnalogItems[0].count,AnalogItems[0].sum/AnalogItems[0].count);
      //sprintf(uart_buf,"%ld\t%d\t%d\t%d\t%d\n",Analog_Count,Analog_Buffer[0],Analog_Buffer[1],Analog_Buffer[2],Analog_Buffer[3]);
      //sprintf(uart_buf,"%ld\t%ld\t%ld\t%ld\t%ld\n",Analog_Count,AnalogItems[0].count,AnalogItems[16*1].count,AnalogItems[16*2].count,AnalogItems[16*3].count);
      //sprintf(uart_buf,"%ld\t%ld\t%ld\t%ld\t%ld\n",Analog_Count,Analog_Buffer[0],Analog_Buffer[1],Analog_Buffer[2],Analog_Buffer[3]);
      //sprintf(uart_buf,"%ld\t%ld\t%ld\t%ld\t%ld\n",Analog_Count,AnalogItems[0].sum/AnalogItems[0].count,AnalogItems[16*1].sum/AnalogItems[16*1].count,AnalogItems[16*2].sum/AnalogItems[16*2].count,AnalogItems[16*3].sum/AnalogItems[16*3].count);
      //sprintf(uart_buf,"max:%ld\tmin:%ld\n",max32,min32);
      //sprintf(uart_buf,"%f\n",Keyboard_AdvancedKeys[5].value);
      //sprintf(uart_buf,"%d\n",(uint8_t)((Keyboard_AdvancedKeys[5].value)<0?(0):((Keyboard_AdvancedKeys[5].value)>1.0?((float)(RGB_Configs[10].rgb.r)):(Keyboard_AdvancedKeys[5].value)*((float)(RGB_Configs[10].rgb.r)))));
      //HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, 64, 0xFF);
      if(RGB_Update_Flag)
      {
          RGB_Update_Flag=false;
          RGB_Update();
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
                              |RCC_PERIPHCLK_TIM1|RCC_PERIPHCLK_ADC12
                              |RCC_PERIPHCLK_ADC34;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.Adc34ClockSelection = RCC_ADC34PLLCLK_DIV1;
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
  if (htim->Instance==TIM7)
  {

    //Keyboard_Scan();
#ifdef PAUSE_TIM_WHEN_CALCULATING
    HAL_TIM_Base_Stop_IT(&htim2);
#endif
    //HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
    Analog_Check();
    Keyboard_SendReport();
    //debug = AnalogItems[33].sum/AnalogItems[33].count;
    //sprintf(uart_buf,"%ld/%ld/%ld/%ld\n",AnalogItems[0].sum/AnalogItems[0].count,AnalogItems[1].sum/AnalogItems[1].count,AnalogItems[2].sum/AnalogItems[2].count,AnalogItems[3].sum/AnalogItems[3].count);
    /*
    sprintf(uart_buf,"====\n0:%ld\t%ld\n5:%ld\t%ld\n6:%ld\t%ld\n7:%ld\t%ld\n",AnalogDatas[0].count,AnalogDatas[0].sum/AnalogDatas[0].count/16,
            AnalogDatas[5].count,AnalogDatas[5].sum/AnalogDatas[5].count/16,
            AnalogDatas[6].count,AnalogDatas[6].sum/AnalogDatas[6].count/16,
            AnalogDatas[7].count,AnalogDatas[7].sum/AnalogDatas[7].count/16);
    //debug=AnalogItems[4].count;
    if(AnalogDatas[1].count<min32)
        min32 = AnalogDatas[1].count;
    if(AnalogDatas[1].count>max32)
        max32 = AnalogDatas[1].count;
    */
    Analog_Flush();
#ifdef PAUSE_TIM_WHEN_CALCULATING
    HAL_TIM_Base_Start_IT(&htim2);
#endif
    //HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

  }
  if (htim->Instance==TIM6)
  {
      RGB_Update_Flag=true;
  }
  if (htim->Instance==TIM2)
  {
      Analog_ActiveChannel++;
      if(Analog_ActiveChannel>=16)
          Analog_ActiveChannel=0;
      Analog_Channel_Select(Analog_ActiveChannel);
  }
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
