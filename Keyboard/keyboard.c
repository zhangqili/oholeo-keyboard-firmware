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
lefl_bit_array_t Keyboard_KeyArray;
bool Keybaord_SendReport_Enable;

const key_binding_t default_keymap[5][64] = {
	{
		{ESC, NO_MODIFIER}/*0*/, {NUM_1, NO_MODIFIER}/*1*/, {NUM_2, NO_MODIFIER}/*2*/, {NUM_3, NO_MODIFIER}/*3*/, {NUM_4, NO_MODIFIER}/*4*/, {NUM_5, NO_MODIFIER}/*5*/, {NUM_6, NO_MODIFIER}/*6*/, {NUM_7, NO_MODIFIER}/*7*/, {NUM_8, NO_MODIFIER}/*8*/, {NUM_9, NO_MODIFIER}/*9*/, {NUM_0, NO_MODIFIER}/*10*/, {MINUS, NO_MODIFIER}/*11*/, {EQUAL, NO_MODIFIER}/*12*/, {BACKSPACE, NO_MODIFIER}/*13*/,
		{TAB, NO_MODIFIER}/*14*/, {Q, NO_MODIFIER}/*15*/, {W, NO_MODIFIER}/*16*/, {E, NO_MODIFIER}/*17*/, {R, NO_MODIFIER}/*18*/, {T, NO_MODIFIER}/*19*/, {Y, NO_MODIFIER}/*20*/, {U, NO_MODIFIER}/*21*/, {I, NO_MODIFIER}/*22*/, {O, NO_MODIFIER}/*23*/, {P, NO_MODIFIER}/*24*/, {LEFT_U_BRACE, NO_MODIFIER}/*25*/, {RIGHT_U_BRACE, NO_MODIFIER}/*26*/, {BACKSLASH, NO_MODIFIER}/*27*/,
		{CAP_LOCK, NO_MODIFIER}/*28*/, {A, NO_MODIFIER}/*29*/, {S, NO_MODIFIER}/*30*/, {D, NO_MODIFIER}/*31*/, {F, NO_MODIFIER}/*32*/, {G, NO_MODIFIER}/*33*/, {H, NO_MODIFIER}/*34*/, {J, NO_MODIFIER}/*35*/, {K, NO_MODIFIER}/*36*/, {L, NO_MODIFIER}/*37*/, {SEMI_COLON, NO_MODIFIER}/*38*/, {QUOTE, NO_MODIFIER}/*39*/, {ENTER, NO_MODIFIER}/*40*/,
		{NO_EVENT, LEFT_SHIFT}/*41*/, {Z, NO_MODIFIER}/*42*/, {X, NO_MODIFIER}/*43*/, {C, NO_MODIFIER}/*44*/, {V, NO_MODIFIER}/*45*/, {B, NO_MODIFIER}/*46*/, {N, NO_MODIFIER}/*47*/, {M, NO_MODIFIER}/*48*/, {COMMA, NO_MODIFIER}/*49*/, {PERIOD, NO_MODIFIER}/*50*/, {SLASH, NO_MODIFIER}/*51*/, {NO_EVENT, RIGHT_SHIFT}/*52*/, {UP_ARROW, NO_MODIFIER}/*53*/, {DELETE, NO_MODIFIER}/*54*/,
		{NO_EVENT, LEFT_CTRL}/*55*/, {NO_EVENT, LEFT_GUI}/*56*/, {NO_EVENT, LEFT_ALT}/*57*/, {SPACE, NO_MODIFIER}/*58*/, {NO_EVENT, RIGHT_ALT}/*59*/, {FN, NO_MODIFIER}/*60*/, {LEFT_ARROW, NO_MODIFIER}/*61*/, {DOWN_ARROW, NO_MODIFIER}/*62*/, {RIGHT_ARROW, NO_MODIFIER}/*63*/,
	},

	{
		{GRAVE_ACCENT, NO_MODIFIER}, {F1, NO_MODIFIER}, {F2, NO_MODIFIER}, {F3, NO_MODIFIER}, {F4, NO_MODIFIER}, {F5, NO_MODIFIER}, {F6, NO_MODIFIER}, {F7, NO_MODIFIER}, {F8, NO_MODIFIER}, {F9, NO_MODIFIER}, {F10, NO_MODIFIER}, {F11, NO_MODIFIER}, {F12, NO_MODIFIER}, {BACKSPACE, NO_MODIFIER},
		{TAB, NO_MODIFIER}, {Q, NO_MODIFIER}, {W, NO_MODIFIER}, {E, NO_MODIFIER}, {R, NO_MODIFIER}, {T, NO_MODIFIER}, {Y, NO_MODIFIER}, {U, NO_MODIFIER}, {I, NO_MODIFIER}, {O, NO_MODIFIER}, {P, NO_MODIFIER}, {LEFT_U_BRACE, NO_MODIFIER}, {RIGHT_U_BRACE, NO_MODIFIER}, {BACKSLASH, NO_MODIFIER},
		{CAP_LOCK, NO_MODIFIER}, {A, NO_MODIFIER}, {S, NO_MODIFIER}, {D, NO_MODIFIER}, {F, NO_MODIFIER}, {G, NO_MODIFIER}, {H, NO_MODIFIER}, {J, NO_MODIFIER}, {K, NO_MODIFIER}, {L, NO_MODIFIER}, {SEMI_COLON, NO_MODIFIER}, {QUOTE, NO_MODIFIER}, {ENTER, NO_MODIFIER},
		{NO_EVENT, LEFT_SHIFT}, {Z, NO_MODIFIER}, {X, NO_MODIFIER}, {C, NO_MODIFIER}, {V, NO_MODIFIER}, {B, NO_MODIFIER}, {N, NO_MODIFIER}, {M, NO_MODIFIER}, {COMMA, NO_MODIFIER}, {PERIOD, NO_MODIFIER}, {SLASH, NO_MODIFIER}, {NO_EVENT, RIGHT_SHIFT}, {UP_ARROW, NO_MODIFIER}, {DELETE, NO_MODIFIER},
		{NO_EVENT, LEFT_CTRL}, {NO_EVENT, LEFT_GUI}, {NO_EVENT, LEFT_ALT}, {SPACE, NO_MODIFIER}, {NO_EVENT, RIGHT_ALT}, {FN, NO_MODIFIER}, {LEFT_ARROW, NO_MODIFIER}, {DOWN_ARROW, NO_MODIFIER}, {RIGHT_ARROW, NO_MODIFIER}
	}

};

key_binding_t keymap[5][64];

lefl_advanced_key_t Keyboard_AdvancedKeys[ADVANCED_KEY_NUM]=
{
    //Group 1 Begin
    {.key.id=31}
    ,
    {.key.id=30}
    ,
    {.key.id=29}
    ,
    {.key.id=28}
    ,
    {.key.id=41}//Left Shift
    ,
    {.key.id=42}
    ,
    {.key.id=43}
    ,
    {.key.id=44}
    ,
    {.key.id=14}
    ,
    {.key.id=15}
    ,
    {.key.id=16}
    ,
    {.key.id=17}
    ,
    {.key.id=3}
    ,
    {.key.id=2}
    ,
    {.key.id=1}
    ,
    {.key.id=0}
    ,
    //Group 2 Begin
    {.key.id=40}
    ,
    {.key.id=39}
    ,
    {.key.id=38}
    ,
    {.key.id=37}
    ,
    {.key.id=51}
    ,
    {.key.id=52}//Right Shift
    ,
    {.key.id=53}
    ,
    {.key.id=54}
    ,
    {.key.id=24}
    ,
    {.key.id=25}
    ,
    {.key.id=26}
    ,
    {.key.id=27}
    ,
    {.key.id=13}
    ,
    {.key.id=12}
    ,
    {.key.id=11}
    ,
    {.key.id=10}
    ,
    //Group 3 Begin
    {.key.id=58}
    ,
    {.key.id=47}
    ,
    {.key.id=34}
    ,
    {.key.id=20}
    ,
    {.key.id=19}
    ,
    {.key.id=6}
    ,
    {.key.id=33}
    ,
    {.key.id=46}
    ,
    {.key.id=45}
    ,
    {.key.id=32}
    ,
    {.key.id=18}
    ,
    {.key.id=5}
    ,
    {.key.id=4}
    ,
    {.key.id=55}//Left Control
    ,
    {.key.id=56}//Left GUI
    ,
    {.key.id=57}//Left Alt
    ,
    //Group 4 Begin
    {.key.id=59}//Right alt
    ,
    {.key.id=60} //Fn
    ,
    {.key.id=61}
    ,
    {.key.id=62}
    ,
    {.key.id=63}
    ,
    {.key.id=50}
    ,
    {.key.id=49}
    ,
    {.key.id=36}
    ,
    {.key.id=22}
    ,
    {.key.id=23}
    ,
    {.key.id=9}
    ,
    {.key.id=8}
    ,
    {.key.id=48}
    ,
    {.key.id=35}
    ,
    {.key.id=21}
    ,
    {.key.id=7}
};

void Keyboard_Init()
{
    Keyboard_FactoryReset();
}

void Keyboard_FactoryReset()
{
    memcpy(keymap,default_keymap,sizeof(keymap));
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        //lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, 4000, 0);
        //lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, 0.01, 0.1);
        Keyboard_AdvancedKeys[i].mode=DEFAULT_ADVANCED_KEY_MODE;
        Keyboard_AdvancedKeys[i].trigger_distance=DEFAULT_TRIGGER_DISTANCE;
        Keyboard_AdvancedKeys[i].release_distance=DEFAULT_RELEASE_DISTANCE;
        Keyboard_AdvancedKeys[i].schmitt_parameter=DEFAULT_SCHMITT_PARAMETER;
    }
    RGB_Recovery();
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
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, Keyboard_AdvancedKeys, sizeof(Keyboard_AdvancedKeys));
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, keymap, sizeof(keymap));
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &RGB_GlobalConfig, sizeof(RGB_GlobalConfig));
  lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &RGB_Configs, sizeof(RGB_Configs));
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
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, Keyboard_AdvancedKeys, sizeof(Keyboard_AdvancedKeys));
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, keymap, sizeof(keymap));
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &RGB_GlobalConfig, sizeof(RGB_GlobalConfig));
  lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &RGB_Configs, sizeof(RGB_Configs));
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

    uint8_t layer;
    key_binding_t key_binding;
    layer = Keyboard_AdvancedKeys[49].key.state?1:0; //Fn key

    memset(Keyboard_ReportBuffer, 0, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    Keyboard_ReportBuffer[0] = 1;
    for (int i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        if(Keyboard_AdvancedKeys[i].key.state)
        {
            key_binding = keymap[layer][Keyboard_AdvancedKeys[i].key.id];
            if(key_binding.keycode>=120)
                continue;
            Keyboard_ReportBuffer[key_binding.keycode/8 + 2] |= 1 << (key_binding.keycode%8); // +1 for Report-ID
            Keyboard_ReportBuffer[1] |= key_binding.modifier; // +1 for Report-ID
        }
    }
    //debug
//    memset(Keyboard_ReportBuffer, 0, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,16+1);
}



