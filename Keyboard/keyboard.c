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

uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
lefl_bit_array_t Keyboard_KeyArray;
lefl_advanced_key_t Keyboard_AdvancedKeys[ADVANCED_KEY_NUM]=
{
    //Group 1 Begin
    {.key.id=KEY_D}
    ,
    {.key.id=KEY_S}
    ,
    {.key.id=KEY_A}
    ,
    {.key.id=KEY_CAPS_LOCK}
    ,
    {.key.id=KEY_LEFT_SHIFT}
    ,
    {.key.id=KEY_Z}
    ,
    {.key.id=KEY_X}
    ,
    {.key.id=KEY_C}
    ,
    {.key.id=KEY_TAB}
    ,
    {.key.id=KEY_Q}
    ,
    {.key.id=KEY_W}
    ,
    {.key.id=KEY_E}
    ,
    {.key.id=KEY_3}
    ,
    {.key.id=KEY_2}
    ,
    {.key.id=KEY_1}
    ,
    {.key.id=KEY_ESC}
    ,
    //Group 2 Begin
    {.key.id=KEY_ENTER}
    ,
    {.key.id=KEY_APOSTROPHE}
    ,
    {.key.id=KEY_SEMICOLON}
    ,
    {.key.id=KEY_L}
    ,
    {.key.id=KEY_SLASH}
    ,
    {.key.id=KEY_RIGHT_SHIFT}
    ,
    {.key.id=KEY_UP_ARROW}
    ,
    {.key.id=KEY_DELETE}
    ,
    {.key.id=KEY_P}
    ,
    {.key.id=KEY_LEFT_BRACE}
    ,
    {.key.id=KEY_RIGHT_BRACE}
    ,
    {.key.id=KEY_BACKSLASH}
    ,
    {.key.id=KEY_BACKSPACE}
    ,
    {.key.id=KEY_EQUAL}
    ,
    {.key.id=KEY_MINUS}
    ,
    {.key.id=KEY_0}
    ,
    //Group 3 Begin
    {.key.id=KEY_SPACEBAR}
    ,
    {.key.id=KEY_N}
    ,
    {.key.id=KEY_H}
    ,
    {.key.id=KEY_Y}
    ,
    {.key.id=KEY_T}
    ,
    {.key.id=KEY_6}
    ,
    {.key.id=KEY_G}
    ,
    {.key.id=KEY_B}
    ,
    {.key.id=KEY_V}
    ,
    {.key.id=KEY_F}
    ,
    {.key.id=KEY_R}
    ,
    {.key.id=KEY_5}
    ,
    {.key.id=KEY_4}
    ,
    {.key.id=KEY_LEFT_CONTROL}
    ,
    {.key.id=KEY_LEFT_GUI}
    ,
    {.key.id=KEY_LEFT_ALT}
    ,
    //Group 4 Begin
    {.key.id=KEY_RIGHT_CONTROL}
    ,
    {.key.id=0}
    ,
    {.key.id=KEY_LEFT_ARROW}
    ,
    {.key.id=KEY_DOWN_ARROW}
    ,
    {.key.id=KEY_RIGHT_ARROW}
    ,
    {.key.id=KEY_DOT}
    ,
    {.key.id=KEY_COMMA}
    ,
    {.key.id=KEY_K}
    ,
    {.key.id=KEY_I}
    ,
    {.key.id=KEY_O}
    ,
    {.key.id=KEY_9}
    ,
    {.key.id=KEY_8}
    ,
    {.key.id=KEY_M}
    ,
    {.key.id=KEY_J}
    ,
    {.key.id=KEY_U}
    ,
    {.key.id=KEY_7}
};


void Keyboard_Init()
{
    lefl_key_attach(&(Keyboard_AdvancedKeys[0].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+0, 0.0);}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[1].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+1, 0.0);}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[2].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+2, 0.0);}));
    lefl_key_attach(&(Keyboard_AdvancedKeys[3].key), KEY_EVENT_DOWN, LAMBDA(void,(void*k){lefl_loop_queue_enqueue(RGB_Argument_Queues+3, 0.0);}));
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, 4000, 0);
        lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, 0.01, 0.1);
        Keyboard_AdvancedKeys[i].mode=LEFL_KEY_ANALOG_RAPID_MODE;
        Keyboard_AdvancedKeys[i].trigger_distance=0.03;
        Keyboard_AdvancedKeys[i].release_distance=0.03;
    }
}

void Keyboard_Scan()
{
}

void Keyboard_SendReport()
{
    /*
    lefl_bit_array_set(&Keyboard_KeyArray, KEY1_BINDING, !(rand()%16));
    lefl_bit_array_set(&Keyboard_KeyArray, KEY2_BINDING, !(rand()%16));
    lefl_bit_array_set(&Keyboard_KeyArray, KEY3_BINDING, !(rand()%16));
    lefl_bit_array_set(&Keyboard_KeyArray, KEY4_BINDING, !(rand()%16));
    */
    memset(Keyboard_ReportBuffer,0,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        Keyboard_ReportBuffer[0]|= Keyboard_AdvancedKeys[i].key.state?((Keyboard_AdvancedKeys[i].key.id>>8) & 0xFF):0;
        lefl_bit_array_set_or(&Keyboard_KeyArray, Keyboard_AdvancedKeys[i].key.id & 0xFF, Keyboard_AdvancedKeys[i].key.state);
    }
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
}


