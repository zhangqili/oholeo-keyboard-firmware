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
#include "usbd_custom_hid_if.h"
#include "math.h"
#include "lfs.h"
#include "sfud.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define rgb_start() HAL_TIM_PWM_Start_DMA(&htim1,TIM_CHANNEL_1,(uint32_t*)g_rgb_buffer,RGB_BUFFER_LENGTH);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile bool RGB_Update_Flag=false;
char uart_buf[64];
uint32_t debug;
uint32_t max32;
uint32_t min32=0xFFFFFFFF;

uint8_t USB_Recive_Buffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]; //USB接收缓存
uint8_t USB_Received_Count;//USB接收数据计数

uint32_t usb_adc_send_idx = 0;
uint32_t err_cnt=0;

sfud_flash sfud_norflash0 = {
        .name = "norflash0",
        .spi.name = "SPI1",
        .chip = {"W25Q128JV", SFUD_MF_ID_WINBOND, 0x40, 0x18, 16L*1024L*1024L, SFUD_WM_PAGE_256B, 4096, 0x20},
};



enum state_t {
	NORMAL,
	ADC,
	JOYSTICK,
	REQUEST_PROFILE,
	REQUEST_SAVE,
	FACTORYRESET,
};
int32_t state_counter = 0;
enum state_t global_state = NORMAL;


uint8_t LED_Report = 0;

uint32_t test_cnt = 0;

uint32_t pulse_counter=PULSE_LEN_MS;
uint8_t beep_switch=0;
uint8_t em_switch=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// 重定向print start
// 重定向printf start
//_write函數在syscalls.c中， 使用__weak定义以可以直接在其他文件中定义_write函數
__attribute__((weak)) int _write(int file, char *ptr, int len)
{
     if(HAL_UART_Transmit(&huart1,ptr,len,0xffff) != HAL_OK)
     {
         Error_Handler();
     }
     return HAL_OK;
}
// 重定向print end

/**
* @brief  初始化时间戳
* @note   使用延时函数前，必须调用本函数
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

void (*SysMemBootJump)(void);
__IO uint32_t BootAddr = 0x1FFFD800; /* BootLoader 地址 */

void JumpToBootloader(void) {
  uint32_t i=0;

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
      NVIC->ICER[i]=0xFFFFFFFF;
      NVIC->ICPR[i]=0xFFFFFFFF;
  }

  /* 使能全局中断 */
  __set_PRIMASK(0);

  /* 跳转到系统BootLoader，首地址是MSP，地址+4是复位中断服务程序地址 */
  SysMemBootJump = (void (*)(void)) (*((uint32_t *) (BootAddr + 4)));

  /* 设置主堆栈指针 */
  __set_MSP(*(uint32_t *)BootAddr);

  /* 跳转到系统BootLoader */
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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  //!!!
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);

  DWT_Init();
  sfud_device_init(&sfud_norflash0);

  HAL_GPIO_WritePin(INHIBIT_GPIO_Port, INHIBIT_Pin, GPIO_PIN_RESET);
  rgb_init();

  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(&hadc4, ADC_SINGLE_ENDED);
  rgb_start();

  rgb_flash();
  rgb_turn_off();

  HAL_ADC_Start_DMA(&hadc1, g_ADC_Buffer[0], DMA_BUF_LEN);
  HAL_ADC_Start_DMA(&hadc2, g_ADC_Buffer[1], DMA_BUF_LEN);
  HAL_ADC_Start_DMA(&hadc3, g_ADC_Buffer[2], DMA_BUF_LEN);
  HAL_ADC_Start_DMA(&hadc4, g_ADC_Buffer[3], DMA_BUF_LEN);

  HAL_TIM_Base_Start_IT(&htim2);

  HAL_Delay(500);
  if(ringbuf_avg(&adc_ringbuf[49])<1400)
  {
	  JumpToBootloader();
  }
  keyboard_recovery();
  for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
  {
      advanced_key_set_range(g_keyboard_advanced_keys+i, ringbuf_avg(&adc_ringbuf[i]), 1200);
      key_attach(&g_keyboard_advanced_keys[i].key, KEY_EVENT_UP, NULL);
      key_attach(&g_keyboard_advanced_keys[i].key, KEY_EVENT_DOWN, NULL);
//	  advanced_key_set_range(Keyboard_AdvancedKeys+i,(ADC_Buffer[i]), 1200);

      advanced_key_set_deadzone(g_keyboard_advanced_keys+i, DEFAULT_UPPER_DEADZONE, g_keyboard_advanced_keys[i].lower_deadzone);
      g_rgb_configs[i].speed=0.01;
  }
  g_rgb_global_config.speed = 0.01;
  HAL_TIM_Base_Start_IT(&htim7);
  //sprintf(uart_buf,"%f\n",Keyboard_AdvancedKeys[0].upper_bound);
  //HAL_UART_Transmit(&huart1, (uint8_t*)uart_buf, 64, 0xFF);
  memset(USB_Recive_Buffer, 0, sizeof(USB_Recive_Buffer));
//  Keyboard_Save();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  if(global_state == REQUEST_SAVE){
		  keyboard_save();
		  global_state = NORMAL;
	  }
	  if(global_state == FACTORYRESET){
		  keyboard_factory_reset();
		   //request profile
		   state_counter = 96;// send twice for safety
		   global_state = REQUEST_PROFILE;
	  }

      rgb_update();
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
	    analog_check();

    	if(pulse_counter<PULSE_LEN_MS) {
    		pulse_counter++;
    		if(beep_switch) {
				HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    		}
    		if(em_switch) {
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
    		}
    	} else {
    		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
    		HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);

    	}

	    if(g_keyboard_advanced_keys[49].key.state&g_keyboard_advanced_keys[0].key.state){
	    	global_state=ADC;
	    }
		if(g_keyboard_advanced_keys[49].key.state&g_keyboard_advanced_keys[33].key.state){
			global_state=NORMAL;
			beep_switch=0;
			em_switch=0;
		}
		if(g_keyboard_advanced_keys[49].key.state&g_keyboard_advanced_keys[61].key.state){
			global_state=JOYSTICK;
		}
		if(g_keyboard_advanced_keys[49].key.state&g_keyboard_advanced_keys[39].key.state){
			beep_switch=1;
		}
		if(g_keyboard_advanced_keys[49].key.state&g_keyboard_advanced_keys[60].key.state){
			em_switch=1;
		}


	    switch(global_state) {
	    case NORMAL:
	    	keyboard_send_report();
	    	break;
	    case ADC:
	    	g_keyboard_report_buffer[0] = 2;
	    	g_keyboard_report_buffer[1] = usb_adc_send_idx;
		    for(int i=0;i<16; i++) {
//		    	Keyboard_ReportBuffer[i+2] = i%2?(uint32_t)RingBuf_Avg(&adc_ringbuf[i/2 + usb_adc_send_idx*8]):((uint32_t)RingBuf_Avg(&adc_ringbuf[i/2 + usb_adc_send_idx*8]))>>8;
		    	g_keyboard_report_buffer[i+2] = i%2?(uint32_t)g_keyboard_advanced_keys[i/2 + usb_adc_send_idx*8].raw:((uint32_t)g_keyboard_advanced_keys[i/2 + usb_adc_send_idx*8].raw)>>8;

		    }

		    usb_adc_send_idx++;
		    if(usb_adc_send_idx>=8)usb_adc_send_idx=0;
		    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,g_keyboard_report_buffer,17+1);
			break;
	    case JOYSTICK:
	    	int8_t Ry,Rx,y,x;
	    	y = (g_keyboard_advanced_keys[1].upper_bound-g_keyboard_advanced_keys[1].raw)/(g_keyboard_advanced_keys[1].upper_bound-g_keyboard_advanced_keys[1].lower_bound)*127.0
	    			- (g_keyboard_advanced_keys[10].upper_bound-g_keyboard_advanced_keys[10].raw)/(g_keyboard_advanced_keys[10].upper_bound-g_keyboard_advanced_keys[10].lower_bound)*127.0;
	    	x = (g_keyboard_advanced_keys[0].upper_bound-g_keyboard_advanced_keys[0].raw)/(g_keyboard_advanced_keys[0].upper_bound-g_keyboard_advanced_keys[0].lower_bound)*127.0
	    			- (g_keyboard_advanced_keys[2].upper_bound-g_keyboard_advanced_keys[2].raw)/(g_keyboard_advanced_keys[2].upper_bound-g_keyboard_advanced_keys[2].lower_bound)*127.0;

	    	g_keyboard_report_buffer[0] = 3;
	    	//Ry,Rx,y,x
	    	g_keyboard_report_buffer[1] = 128;
	    	g_keyboard_report_buffer[2] = 0;
	    	g_keyboard_report_buffer[3] = y-128;
	    	g_keyboard_report_buffer[4] = x-128;
	    	g_keyboard_report_buffer[5] = 0;
		    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,g_keyboard_report_buffer,5+1);

	    	break;

	    case REQUEST_PROFILE:
	    	state_counter--;
	    	if(state_counter<=0)global_state=NORMAL;

	    	if(state_counter/32 == 0) {
				//sent performance data
				g_keyboard_report_buffer[0] = 2;
				g_keyboard_report_buffer[1] = (state_counter%16) + 16;
				for(int i=0;i<4;i++) {
					uint32_t index = i + (state_counter%16)*4;
					g_keyboard_report_buffer[2 + i*4+0] = (uint8_t)(roundf(g_keyboard_advanced_keys[index].activation_value*100.0)) |
							((g_keyboard_advanced_keys[index].mode - 1) << 7);
					g_keyboard_report_buffer[2 + i*4+1] = (uint8_t)(roundf(g_keyboard_advanced_keys[index].trigger_distance*100.0));
					g_keyboard_report_buffer[2 + i*4+2] = (uint8_t)(roundf(g_keyboard_advanced_keys[index].release_distance*100.0));
					g_keyboard_report_buffer[2 + i*4+3] = (uint8_t)(roundf(g_keyboard_advanced_keys[index].phantom_lower_deadzone*100.0));
				}
	    	}
	    	if(state_counter/32 == 1) {
		    	//sent rgb data
		    	g_keyboard_report_buffer[0] = 2;
		    	g_keyboard_report_buffer[1] = (state_counter%16) + 32;
		    	for(int i=0;i<4;i++) {
		    		uint32_t index = i + (state_counter%16)*4;
		    		g_keyboard_report_buffer[2 + i*4+0] = (uint8_t)(g_rgb_global_config.mode<<4) | g_rgb_configs[g_rgb_mapping[index]].mode;
		    		g_keyboard_report_buffer[2 + i*4+1] = g_rgb_configs[g_rgb_mapping[index]].rgb.r;
		    		g_keyboard_report_buffer[2 + i*4+2] = g_rgb_configs[g_rgb_mapping[index]].rgb.g;
		    		g_keyboard_report_buffer[2 + i*4+3] = g_rgb_configs[g_rgb_mapping[index]].rgb.b;
		    	}
	    	}
	    	if(state_counter/32 == 2) {
		    	//sent keymap data
		    	g_keyboard_report_buffer[0] = 2;
		    	g_keyboard_report_buffer[1] = (state_counter%16) + 48;
		    	for(int i=0;i<4;i++) {
		    		uint32_t index = i + (state_counter%16)*4;
		    		g_keyboard_report_buffer[2 + i*4+0] = g_keymap[0][g_keyboard_advanced_keys[index].key.id] >>8;
		    		g_keyboard_report_buffer[2 + i*4+1] = g_keymap[0][g_keyboard_advanced_keys[index].key.id]&0xff;
		    		g_keyboard_report_buffer[2 + i*4+2] = g_keymap[1][g_keyboard_advanced_keys[index].key.id]>>8;
		    		g_keyboard_report_buffer[2 + i*4+3] = g_keymap[1][g_keyboard_advanced_keys[index].key.id]&0xff;
		    	}
	    	}
		    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,g_keyboard_report_buffer,17+1);
	    	break;

	    default:
	    	break;
	    }



  }
  if (htim->Instance==TIM6)
  {
      RGB_Update_Flag=true;
  }
  if (htim->Instance==TIM2) {
	  test_cnt++;
//	  if(test_cnt%2==0) {
		uint32_t adc1=0;
		uint32_t adc2=0;
		uint32_t adc3=0;
		uint32_t adc4=0;

		for(int i=0;i<DMA_BUF_LEN;i++) {
		 adc1+=g_ADC_Buffer[0][i]&0xfff;
		 adc2+=g_ADC_Buffer[1][i]&0xfff;
		 adc3+=g_ADC_Buffer[2][i]&0xfff;
		 adc4+=g_ADC_Buffer[3][i]&0xfff;
		}

		ringbuf_push(&adc_ringbuf[0*16+ADDRESS] , (float_t)adc1/(float_t)DMA_BUF_LEN);
		ringbuf_push(&adc_ringbuf[1*16+ADDRESS] , (float_t)adc2/(float_t)DMA_BUF_LEN);
		ringbuf_push(&adc_ringbuf[2*16+ADDRESS] , (float_t)adc3/(float_t)DMA_BUF_LEN);
		ringbuf_push(&adc_ringbuf[3*16+ADDRESS] , (float_t)adc4/(float_t)DMA_BUF_LEN);

		if (htim->Instance->CNT < 700) {
			g_analog_active_channel++;
			if(g_analog_active_channel>=16)g_analog_active_channel=0;
			analog_channel_select(g_analog_active_channel);
		}
//	  }
  }
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

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
