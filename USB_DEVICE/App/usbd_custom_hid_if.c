/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
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
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "keyboard.h"
#include "rgb.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern uint8_t LED_Report;
uint32_t api_lut[64] = {15, 14, 13, 12, 44, 43, 37, 63, 59, 58, 31, 30, 29, 28, 8, 9, 10, 11, 42, 36, 35, 62, 56, 57, 24, 25, 26, 27, 3, 2, 1, 0, 41, 38, 34, 61, 55, 19, 18, 17, 16, 4, 5, 6, 7, 40, 39, 33, 60, 54, 53, 20, 21, 22, 23, 45, 46, 47, 32, 48, 49, 50, 51, 52};
/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
#define LSB(_x) ((_x) & 0xFF)
#define MSB(_x) ((_x) >> 8)

#define RAWHID_USAGE_PAGE	0xFFC0
#define RAWHID_USAGE		0x0C00
#define RAWHID_TX_SIZE 64
#define RAWHID_RX_SIZE 64
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
        0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
        0x09, 0x06,                    // USAGE (Keyboard)
        0xa1, 0x01,                    // COLLECTION (Application)
		0x85, 0x01,                    //     REPORT_ID (1)
		// bitmap of modifiers
		0x75, 0x01,         //   Report Size (1),
		0x95, 0x08,         //   Report Count (8),
		0x05, 0x07,       //   Usage Page (Key Codes),
		0x19, 0xE0,       //   Usage Minimum (224),
		0x29, 0xE7,       //   Usage Maximum (231),
		0x15, 0x00,       //   Logical Minimum (0),
		0x25, 0x01,       //   Logical Maximum (1),
		0x81, 0x02,       //   Input (Data, Variable, Absolute), ;Modifier byte
		// bitmap of keys
		0x95, 0x78,       //   Report Count (120),
		0x75, 0x01,       //   Report Size (1),
		0x15, 0x00,       //   Logical Minimum (0),
		0x25, 0x01,       //   Logical Maximum(1),
		0x05, 0x07,       //   Usage Page (Key Codes),
		0x19, 0x00,       //   Usage Minimum (0),
		0x29, 0x77,       //   Usage Maximum (),
		0x81, 0x02,       //   Input (Data, Variable, Absolute),
        // LED output report
        0x95, 0x05,       			   //   Report Count (5),
        0x75, 0x01,       			   //   Report Size (1),
        0x05, 0x08,      		 	   //   Usage Page (LEDs),
        0x19, 0x01,       			   //   Usage Minimum (1),
        0x29, 0x05,       			   //   Usage Maximum (5),
        0x91, 0x02,       			   //   Output (Data, Variable, Absolute),
        0x95, 0x01,       			   //   Report Count (1),
        0x75, 0x03,       			   //   Report Size (3),
        0x91, 0x03,       			   //   Output (Constant),
		0xC0  ,
			//	RAW HID
		0x06, 0xC0, 0xFF,	// 30
		0x0A, 0x00, 0x0C,
		0xA1, 0x01,				// Collection 0x01
		0x85, 0x02,            	// REPORT_ID (2)
		0x75, 0x08,				// report size = 8 bits
		0x15, 0x00,				// logical minimum = 0
		0x26, 0xFF, 0x00,		// logical maximum = 255
		0x95, 17,				// report count TX
		0x09, 0x01,				// usage
		0x81, 0x02,				// Input (array)
		0x95, 17,				// report count RX
		0x09, 0x02,				// usage
		0x91, 0x02,				// Output (array)
		0xC0,    /*     END_COLLECTION	       89 Bytes      */
		// joystick
//		0x05, 0x01,   //Usage Page (Generic Desktop)
//		0x09, 0x05,   //Usage (Game Pad)
//		0xA1, 0x01,   //Collection (Application)
//		0x85, 0x03,   //REPORT_ID (3)
//		0x05, 0x01,   //Usage Page (Generic Desktop)
//		0x09, 0x01,   //Usage (Pointer)
//		0xA0,         //Collection (Physical)
//		0x14,         //Logical Minimum (0)
//		0x25, 0xFF,   //Logical Maximum (255)
//		0x75, 0x08,   //Report Size (8)
//		0x95, 0x04,   //Report Count (4)
//		0x09, 0x34,   //Usage (Ry)
//		0x09, 0x33,   //Usage (Rx)
//		0x09, 0x31,   //Usage (Y)
//		0x09, 0x30,   //Usage (X)
//		0x81, 0x02,   //Input (Data, Variable, Absolute)
//		0x14,         //Logical Minimum (0)
//		0x25, 0x01,   //Logical Maximum (1)
//		0x75, 0x01,   //Report Size (1)
//		0x95, 0x02,   //Report Count (2)
//		0x05, 0x09,   //Usage Page (Button)
//		0x19, 0x01,   //Usage Minimum (Button 1)
//		0x29, 0x02,   //Usage Maximum (Button 2)
//		0x81, 0x02,   //Input (Data, Variable, Absolute)
//		0x75, 0x06,   //Report Size (6)
//		0x95, 0x01,   //Report Count (1)
//		0x81, 0x03,   //Input (Constant, Variable, Absolute)
//		0xC0,         //END Collection
//		0xC0,         //END Collection 142
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */
extern uint8_t USB_Recive_Buffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE]; //USB接收缓存
extern uint8_t USB_Received_Count;//USB接收数据计数
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
	//num cap scroll compose kana
   /*查看接收数据长度*/
   USB_Received_Count = USBD_GetRxCount( &hUsbDeviceFS,CUSTOM_HID_EPOUT_ADDR );
   //printf("USB_Received_Count = %d \r\n",USB_Received_Count);

   USBD_CUSTOM_HID_HandleTypeDef   *hhid; //定义一个指向USBD_CUSTOM_HID_HandleTypeDef结构体的指针
   hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;//得到USB接收数据的储存地址

   memcpy(USB_Recive_Buffer, hhid->Report_buf, sizeof(USB_Recive_Buffer));
   if(USB_Recive_Buffer[0]==1) {
	   LED_Report = USB_Recive_Buffer[1];
   }
   if(USB_Recive_Buffer[0]==2) {
	   uint8_t page_num = USB_Recive_Buffer[1];
	   enum state_t {
			NORMAL,
			ADC,
			JOYSTICK,
			REQUEST_PROFILE,
			REQUEST_SAVE,
		};
	   extern enum state_t global_state;
	   extern int32_t state_counter;
	   if(page_num == 255) {
		   //request profile
		   state_counter = 96;// send twice for safety
		   global_state = REQUEST_PROFILE;
	   }
	   if(page_num == 254) {
		   //request save
		   global_state = REQUEST_SAVE;
	   }
	   if(page_num<16) {  ///performance
		   for(int i=0;i<4;i++) {
			   if(USB_Recive_Buffer[2+i*4+0]&0x80){
				   Keyboard_AdvancedKeys[api_lut[page_num*4+i]].mode = LEFL_KEY_ANALOG_RAPID_MODE;
				   Keyboard_AdvancedKeys[api_lut[page_num*4+i]].trigger_distance = (float_t)USB_Recive_Buffer[2+i*4+1]/100.0;
				   Keyboard_AdvancedKeys[api_lut[page_num*4+i]].release_distance = (float_t)USB_Recive_Buffer[2+i*4+2]/100.0;
				   lefl_advanced_key_set_deadzone(&Keyboard_AdvancedKeys[api_lut[page_num*4+i]], 0.02, (float_t)USB_Recive_Buffer[2+i*4+3]/100.0);
	//			   Keyboard_AdvancedKeys[api_lut[page_num*4+i]].lower_deadzone = (float_t)USB_Recive_Buffer[2+i*4+3]/100.0;
			   } else {
				   Keyboard_AdvancedKeys[api_lut[page_num*4+i]].mode = LEFL_KEY_ANALOG_NORMAL_MODE;
				   Keyboard_AdvancedKeys[api_lut[page_num*4+i]].trigger_distance = (float_t)USB_Recive_Buffer[2+i*4+0]/100.0;
				   lefl_advanced_key_set_deadzone(&Keyboard_AdvancedKeys[api_lut[page_num*4+i]], 0.02, 0.0);
			   }
		   }
	   } else if(page_num<32) {  ///rgb
		   page_num %= 16; // needed
		   for(int i=0;i<4;i++) {
			   RGB_GlobalConfig.mode = USB_Recive_Buffer[2+i*4+0]>>4;
			   RGB_GlobalConfig.rgb = *(lefl_color_rgb_t *)&USB_Recive_Buffer[2+i*4+1];
			   lefl_rgb_to_hsv(&RGB_GlobalConfig.hsv, &RGB_GlobalConfig.rgb);
			   RGB_Configs[RGB_Mapping[api_lut[page_num*4+i]]].mode = USB_Recive_Buffer[2+i*4+0]&0x0f;
			   RGB_Configs[RGB_Mapping[api_lut[page_num*4+i]]].rgb = *(lefl_color_rgb_t *)&USB_Recive_Buffer[2+i*4+1];
			   lefl_rgb_to_hsv(&RGB_Configs[RGB_Mapping[api_lut[page_num*4+i]]].hsv, &RGB_Configs[RGB_Mapping[api_lut[page_num*4+i]]].rgb);

		   }
	   } else if(page_num<48) {  ///keycode
		   page_num %= 16; // needed
		   for(int i=0;i<4;i++) {
			   keymap[0][Keyboard_AdvancedKeys[api_lut[page_num*4+i]].key.id] = USB_Recive_Buffer[2+i*4+0] << 8 | USB_Recive_Buffer[2+i*4+1];
		   }
	   }
   }
//   if(USB_Recive_Buffer[0]==2)USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,USB_Recive_Buffer,33);
//   for(i=0;i<USB_Received_Count;i++)
//   {
//       USB_Recive_Buffer[i]=hhid->Report_buf[i];  //把接收到的数据送到自定义的缓存区保存（Report_buf[i]为USB的接收缓存区）
//   }
  return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

