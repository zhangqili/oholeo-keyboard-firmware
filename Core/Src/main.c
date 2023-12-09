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

uint8_t USB_Recive_Buffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]; //USB接收缓存
uint8_t USB_Received_Count;//USB接收数据计数

uint32_t usb_adc_send_idx = 0;
uint32_t err_cnt=0;
enum state_t {
	NORMAL,
	DEBUG,
};

enum state_t global_state = NORMAL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
* @brief  初始化时间戳
* @param  �??????????????????????????????????????????
* @retval �??????????????????????????????????????????
* @note   使用延时函数前，必须调用本函�??????????????????????????????????????????
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

void (*SysMemBootJump)(void); /* 声明�??????????????????????个函数指�?????????????????????? */
__IO uint32_t BootAddr = 0x1FFFD800; /*   的系�?????????????????????? BootLoader 地址 */

void JumpToBootloader(void) {
  uint32_t i=0;

  /* 关闭全局中断 */
  __set_PRIMASK(1);

  /* 关闭滴答定时器，复位到默认�?? */
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;

  /* 设置�??????????????????????有时钟到默认状�?�， 使用 HSI 时钟 */
  HAL_RCC_DeInit();

  /* 关闭�??????????????????????有中断，清除�??????????????????????有中断挂起标�?????????????????????? */
  for (i = 0; i < 8; i++)
  {
      NVIC->ICER[i]=0xFFFFFFFF;
      NVIC->ICPR[i]=0xFFFFFFFF;
  }

  /* 使能全局中断 */
  __set_PRIMASK(0);

  /* 跳转到系�?????????????????????? BootLoader，首地址�?????????????????????? MSP，地�??????????????????????+4 是复位中断服务程序地�?????????????????????? */
  SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

  /* 设置主堆栈指�?????????????????????? */
  __set_MSP(*(uint32_t *)BootAddr);

  /* 跳转到系�?????????????????????? BootLoader */
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
//  lefl_bit_array_init(&Keyboard_KeyArray, (size_t*)(Keyboard_ReportBuffer+2), 168);
//  lefl_bit_array_init(&Keyboard_KeyArray, (size_t*)(&Keyboard_ReportBuffer[1]), 128);

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


//	HAL_ADC_Start_IT(&hadc1);
//	HAL_ADC_Start_IT(&hadc2);
//	HAL_ADC_Start_IT(&hadc3);
//	HAL_ADC_Start_IT(&hadc4);
//	HAL_ADC_Start(&hadc1);
//	HAL_ADC_Start(&hadc2);
//	HAL_ADC_Start(&hadc3);
//	HAL_ADC_Start(&hadc4);

  HAL_ADC_Start(&hadc2);
  HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t*)&DMA_Buffer[0], DMA_BUF_LEN);
  HAL_ADC_Start(&hadc4);
  HAL_ADCEx_MultiModeStart_DMA(&hadc3, (uint32_t*)&DMA_Buffer[DMA_BUF_LEN*2], DMA_BUF_LEN);

  //Analog_Start();
  /* Analog Calibration BEGIN */
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);


  HAL_Delay(500);
  if(RingBuf_Avg(&adc_ringbuf[49])<1400)
  {
	  JumpToBootloader();
  }
  for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
  {
      lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, RingBuf_Avg(&adc_ringbuf[i]), 1200);
//	  lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i,(ADC_Buffer[i]), 1200);

      lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, DEFAULT_UPPER_DEADZONE, DEFAULT_LOWER_DEADZONE);
  }
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start_IT(&htim6);
  //sprintf(uart_buf,"%f\n",Keyboard_AdvancedKeys[0].upper_bound);
  //HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, 64, 0xFF);
  memset(USB_Recive_Buffer, 0, sizeof(USB_Recive_Buffer));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		if(USB_Recive_Buffer[1]==1) {
//			USB_Recive_Buffer[0]=0;
//			global_state = DEBUG;
//		}






      HAL_Delay(1);


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
  if (htim->Instance==TIM7)
  {
	    Analog_Check();

	    if(Keyboard_AdvancedKeys[49].key.state&Keyboard_AdvancedKeys[0].key.state){
	    	global_state=DEBUG;
	    }
		if(Keyboard_AdvancedKeys[49].key.state&Keyboard_AdvancedKeys[33].key.state){
			global_state=NORMAL;
		}

	    switch(global_state) {
	    case NORMAL:
	    	Keyboard_SendReport();
	    	break;
	    case DEBUG:
	    	Keyboard_ReportBuffer[0] = 2;
	    	Keyboard_ReportBuffer[1] = usb_adc_send_idx;
		    for(int i=0;i<16; i++) {
//		    	Keyboard_ReportBuffer[i+2] = i%2?(uint32_t)RingBuf_Avg(&adc_ringbuf[i/2 + usb_adc_send_idx*8]):((uint32_t)RingBuf_Avg(&adc_ringbuf[i/2 + usb_adc_send_idx*8]))>>8;
		    	Keyboard_ReportBuffer[i+2] = i%2?(uint32_t)Keyboard_AdvancedKeys[i/2 + usb_adc_send_idx*8].raw:((uint32_t)Keyboard_AdvancedKeys[i/2 + usb_adc_send_idx*8].raw)>>8;

		    }

		    usb_adc_send_idx++;
		    if(usb_adc_send_idx>=8)usb_adc_send_idx=0;
		    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,17+1);
			break;
	    }


  }
  if (htim->Instance==TIM6)
  {
      RGB_Update_Flag=true;
  }
  if (htim->Instance==TIM2) {
//		AnalogDatas[0*16+ADDRESS].sum+=(ADC_Buffer[0]&0x0fff)<<4;
//		AnalogDatas[0*16+ADDRESS].count++;
//		AnalogDatas[1*16+ADDRESS].sum+=((ADC_Buffer[0]>>16)&0x0fff)<<4;
//		AnalogDatas[1*16+ADDRESS].count++;
//
//		AnalogDatas[2*16+ADDRESS].sum+=(ADC_Buffer[2]&0x0fff)<<4;
//		AnalogDatas[2*16+ADDRESS].count++;
//		AnalogDatas[3*16+ADDRESS].sum+=((ADC_Buffer[2]>>16)&0x0fff)<<4;
//		AnalogDatas[3*16+ADDRESS].count++;
//	  HAL_ADCEx_MultiModeStop_DMA(&hadc1);
//	  HAL_ADCEx_MultiModeStop_DMA(&hadc3);
//	  HAL_ADC_Stop(&hadc2);
//	  HAL_ADC_Stop(&hadc4);
//		HAL_ADC_MspDeInit(&hadc1);
//		HAL_ADC_MspDeInit(&hadc2);
//		HAL_ADC_MspDeInit(&hadc3);
//		HAL_ADC_MspDeInit(&hadc4);

//		RingBuf_Push(&adc_ringbuf[0*16+ADDRESS], ADC_Buffer[0]&0x0fff);
//		RingBuf_Push(&adc_ringbuf[1*16+ADDRESS], (ADC_Buffer[0]>>16)&0x0fff);
//		RingBuf_Push(&adc_ringbuf[2*16+ADDRESS], ADC_Buffer[2]&0x0fff);
//		RingBuf_Push(&adc_ringbuf[3*16+ADDRESS], (ADC_Buffer[2]>>16)&0x0fff);




		uint32_t adc1=0;
		uint32_t adc2=0;
		uint32_t adc3=0;
		uint32_t adc4=0;

		for(int i=0;i<DMA_BUF_LEN;i++) {
		 adc1+=DMA_Buffer[i*2];
		 adc2+=DMA_Buffer[i*2+1];
		 adc3+=DMA_Buffer[(i+DMA_BUF_LEN)*2];
		 adc4+=DMA_Buffer[(i+DMA_BUF_LEN)*2+1];
		}

//		ADC_Buffer[0*16+ADDRESS] = (float_t)adc1/DMA_BUF_LEN;
//		ADC_Buffer[1*16+ADDRESS] = (float_t)adc2/DMA_BUF_LEN;
//		ADC_Buffer[2*16+ADDRESS] = (float_t)adc3/DMA_BUF_LEN;
//		ADC_Buffer[3*16+ADDRESS] = (float_t)adc4/DMA_BUF_LEN;

		RingBuf_Push(&adc_ringbuf[0*16+ADDRESS] , (float_t)adc1/DMA_BUF_LEN);
		RingBuf_Push(&adc_ringbuf[1*16+ADDRESS] , (float_t)adc2/DMA_BUF_LEN);
		RingBuf_Push(&adc_ringbuf[2*16+ADDRESS] , (float_t)adc3/DMA_BUF_LEN);
		RingBuf_Push(&adc_ringbuf[3*16+ADDRESS] , (float_t)adc4/DMA_BUF_LEN);

	if (htim->Instance->CNT < 600) {
		Analog_Count++;
		Analog_ActiveChannel++;
		if(Analog_ActiveChannel>=16)Analog_ActiveChannel=0;
		Analog_Channel_Select(Analog_ActiveChannel);
	}
//	  if (htim->Instance->CNT < 200) {
//			RingBuf_Push(&adc_ringbuf[0*16+ADDRESS], ADC_Buffer[0]&0x0fff);
//			RingBuf_Push(&adc_ringbuf[1*16+ADDRESS], (ADC_Buffer[0]>>16)&0x0fff);
//			RingBuf_Push(&adc_ringbuf[2*16+ADDRESS], ADC_Buffer[2]&0x0fff);
//			RingBuf_Push(&adc_ringbuf[3*16+ADDRESS], (ADC_Buffer[2]>>16)&0x0fff);
//	  }
//	  RingBuf_Push(&adc_ringbuf[0*16+ADDRESS], HAL_ADC_GetValue(&hadc1));
//	  RingBuf_Push(&adc_ringbuf[1*16+ADDRESS], HAL_ADC_GetValue(&hadc2));
//	  RingBuf_Push(&adc_ringbuf[2*16+ADDRESS], HAL_ADC_GetValue(&hadc3));
//	  RingBuf_Push(&adc_ringbuf[3*16+ADDRESS], HAL_ADC_GetValue(&hadc4));

//	HAL_ADC_MspInit(&hadc1);
//	HAL_ADC_MspInit(&hadc2);
//	HAL_ADC_MspInit(&hadc3);
//	HAL_ADC_MspInit(&hadc4);
//
////	    MX_ADC1_Init();
////	    MX_ADC2_Init();n
////	    MX_ADC3_Init();
////	    MX_ADC4_Init();
//		HAL_ADC_Start(&hadc2);
//		HAL_ADC_Start(&hadc4);
//		HAL_ADCEx_MultiModeStart_DMA(&hadc1, &DMA_Buffer[0], RING_BUF_LEN);
//		HAL_ADCEx_MultiModeStart_DMA(&hadc3, &DMA_Buffer[RING_BUF_LEN], RING_BUF_LEN);

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
