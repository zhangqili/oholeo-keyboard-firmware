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

uint8_t Keyboard_ReportBuffer[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
lefl_bit_array_t Keyboard_KeyArray;
bool Keybaord_SendReport_Enable;


const int16_t default_keymap[5][64] = {
	{
		ESC/*0*/, NUM_1/*1*/, NUM_2/*2*/, NUM_3/*3*/, NUM_4/*4*/, NUM_5/*5*/, NUM_6/*6*/, NUM_7/*7*/, NUM_8/*8*/, NUM_9/*9*/, NUM_0/*10*/, MINUS/*11*/, EQUAL/*12*/, BACKSPACE/*13*/,
		TAB/*14*/, Q/*15*/, W/*16*/, E/*17*/, R/*18*/, T/*19*/, Y/*20*/, U/*21*/, I/*22*/, O/*23*/, P/*24*/, LEFT_U_BRACE/*25*/, RIGHT_U_BRACE/*26*/, BACKSLASH/*27*/,
		CAP_LOCK/*28*/, A/*29*/, S/*30*/, D/*31*/, F/*32*/, G/*33*/, H/*34*/, J/*35*/, K/*36*/, L/*37*/, SEMI_COLON/*38*/, QUOTE/*39*/, ENTER/*40*/,
		LEFT_SHIFT/*41*/, Z/*42*/, X/*43*/, C/*44*/, V/*45*/, B/*46*/, N/*47*/, M/*48*/, COMMA/*49*/, PERIOD/*50*/, SLASH/*51*/, RIGHT_SHIFT/*52*/, UP_ARROW/*53*/, DELETE/*54*/,
		LEFT_CTRL/*55*/, LEFT_GUI/*56*/, LEFT_ALT/*57*/, SPACE/*58*/, RIGHT_ALT/*59*/, FN/*60*/, LEFT_ARROW/*61*/, DOWN_ARROW/*62*/, RIGHT_ARROW/*63*/,
	},

	{
		GRAVE_ACCENT, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, BACKSPACE,
		TAB, Q, W, E, R, T, Y, U, I, O, P, LEFT_U_BRACE, RIGHT_U_BRACE, BACKSLASH,
		CAP_LOCK, A, S, D, F, G, H, J, K, L, SEMI_COLON, QUOTE, ENTER,
		LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT, UP_ARROW, DELETE,
		LEFT_CTRL, LEFT_GUI, LEFT_ALT, SPACE, RIGHT_ALT, FN, LEFT_ARROW, DOWN_ARROW, RIGHT_ARROW,
	}

};


int16_t keymap[5][64];


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
    memcpy(keymap,default_keymap,sizeof(keymap));

    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        //lefl_advanced_key_set_range(Keyboard_AdvancedKeys+i, 4000, 0);
        //lefl_advanced_key_set_deadzone(Keyboard_AdvancedKeys+i, 0.01, 0.1);
        Keyboard_AdvancedKeys[i].mode=DEFAULT_ADVANCED_KEY_MODE;
        Keyboard_AdvancedKeys[i].trigger_distance=DEFAULT_TRIGGER_DISTANCE;
        Keyboard_AdvancedKeys[i].release_distance=DEFAULT_RELEASE_DISTANCE;
        Keyboard_AdvancedKeys[i].schmitt_parameter=DEFAULT_SCHMITT_PARAM;
        Keyboard_AdvancedKeys[i].key.keycode = keymap[0][Keyboard_AdvancedKeys[i].key.id];
    }
}

void Keyboard_Scan()
{
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
    layer = Keyboard_AdvancedKeys[49].key.state?1:0; //Fn key

    memset(Keyboard_ReportBuffer, 0, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    Keyboard_ReportBuffer[0] = 1;
    for (int i = 0; i < ADVANCED_KEY_NUM; i++)
    {
    	keycode = keymap[layer][Keyboard_AdvancedKeys[i].key.id];
    	index = (int16_t)(keycode/8 + 1);// +1 for modifier
    	bitIndex = (int16_t)(keycode%8);
        if (bitIndex < 0)
        {
            index -= 1;
            bitIndex += 8;
        } else if (keycode > 100)
            continue;
        if(Keyboard_AdvancedKeys[i].key.state)Keyboard_ReportBuffer[index + 1] |= 1 << (bitIndex); // +1 for Report-ID
    }
    //debug
//    memset(Keyboard_ReportBuffer, 0, USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,Keyboard_ReportBuffer,16+1);
}



