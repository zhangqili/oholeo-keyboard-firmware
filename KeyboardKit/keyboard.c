/*
 * keyboard.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "analog.h"
#include "usbd_custom_hid_if.h"
#include "gpio.h"
#include "rgb.h"
#include "keyboard.h"
#include "keyboard_conf.h"
#include "lfs.h"
#include "stdio.h"

uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
bool Keybaord_SendReport_Enable;



uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];


void Keyboard_Init()
{
    Keyboard_FactoryReset();
}

void Keyboard_FactoryReset()
{
    memcpy(g_keymap,g_default_keymap,sizeof(g_keymap));
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        //advanced_key_set_range(Keyboard_AdvancedKeys+i, 4000, 0);
        //advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, 0.01, 0.1);
        g_keyboard_advanced_keys[i].mode=DEFAULT_ADVANCED_KEY_MODE;
        g_keyboard_advanced_keys[i].trigger_distance=DEFAULT_TRIGGER_DISTANCE;
        g_keyboard_advanced_keys[i].release_distance=DEFAULT_RELEASE_DISTANCE;
        g_keyboard_advanced_keys[i].schmitt_parameter=DEFAULT_SCHMITT_PARAMETER;
        g_keyboard_advanced_keys[i].activation_value=0.3;
        g_keyboard_advanced_keys[i].lower_deadzone=0.32;
        g_keyboard_advanced_keys[i].phantom_lower_deadzone=0.32;
    }
    rgb_recovery();
    Keyboard_Save();
}
void Keyboard_SystemReset()
{
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
}

void Keyboard_Scan()
{
}
void Keyboard_Recovery()
{
    
  // mount the filesystem
  int err = lfs_mount(&lfs_w25qxx, &cfg);
  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err)
  {
      lfs_format(&lfs_w25qxx, &cfg);
      lfs_mount(&lfs_w25qxx, &cfg);
  }
  lfs_file_open(&lfs_w25qxx, &lfs_file_w25qxx, "config1.dat", LFS_O_RDWR | LFS_O_CREAT);
  lfs_file_rewind(&lfs_w25qxx, &lfs_file_w25qxx);
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, g_keyboard_advanced_keys, sizeof(g_keyboard_advanced_keys));
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, g_keymap, sizeof(g_keymap));
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_global_config, sizeof(g_rgb_global_config));
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_configs, sizeof(g_rgb_configs));
  // remember the storage is not updated until the file is closed successfully
  lfs_file_close(&lfs_w25qxx, &lfs_file_w25qxx);
  printf("recovery = %d",err);
  // release any resources we were using
  lfs_unmount(&lfs_w25qxx);
  // print the boot count
}


void Keyboard_Save()
{
    
  // mount the filesystem
  int err = lfs_mount(&lfs_w25qxx, &cfg);
  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err)
  {
      lfs_format(&lfs_w25qxx, &cfg);
      lfs_mount(&lfs_w25qxx, &cfg);
  }
  // read current count
  lfs_file_open(&lfs_w25qxx, &lfs_file_w25qxx, "config1.dat", LFS_O_RDWR | LFS_O_CREAT);
  lfs_file_rewind(&lfs_w25qxx, &lfs_file_w25qxx);
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, g_keyboard_advanced_keys, sizeof(g_keyboard_advanced_keys));
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, g_keymap, sizeof(g_keymap));
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_global_config, sizeof(g_rgb_global_config));
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_configs, sizeof(g_rgb_configs));
  // remember the storage is not updated until the file is closed successfully
  err = lfs_file_close(&lfs_w25qxx, &lfs_file_w25qxx);
  printf("save = %d",err);
  // release any resources we were using
  lfs_unmount(&lfs_w25qxx);
  // print the boot count
}

void Keyboard_SendReport()
{
//    Keyboard_ReportBuffer[0] = 1;
//    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
//    {
//        //lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_AdvancedKeys[i].key.id & 0xFF, Keyboard_AdvancedKeys[i].key.state);
//    }
//
//    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

    int16_t index, bitIndex, keycode, layer;
    layer = g_keyboard_advanced_keys[49].key.state?1:0; //Fn key

    memset(Keyboard_ReportBuffer, 0, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    Keyboard_ReportBuffer[0] = 1;
    for (int i = 0; i < ADVANCED_KEY_NUM; i++)
    {
    	keycode = g_keymap[layer][g_keyboard_advanced_keys[i].key.id];
    	index = (int16_t)(keycode/8 + 1);// +1 for modifier
    	bitIndex = (int16_t)(keycode%8);
        if (bitIndex < 0)
        {
            index -= 1;
            bitIndex += 8;
        } else if (keycode > 100)
            continue;
        if(g_keyboard_advanced_keys[i].key.state)Keyboard_ReportBuffer[index + 1] |= 1 << (bitIndex); // +1 for Report-ID
    }
    //debug
//    memset(Keyboard_ReportBuffer, 0, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,16+1);
}



