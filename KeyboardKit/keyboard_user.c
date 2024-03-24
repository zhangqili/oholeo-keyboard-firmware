/*
 * keyboard_user.c
 *
 *  Created on: Mar 17, 2024
 *      Author: xq123
 */
#include "keyboard.h"
#include "rgb.h"
#include "usbd_conf.h"
#include "usb_device.h"
#include "usbd_custom_hid_if.h"

const uint16_t g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM] = {
    {
        ESC/*0*/, NUM_1/*1*/, NUM_2/*2*/, NUM_3/*3*/, NUM_4/*4*/, NUM_5/*5*/, NUM_6/*6*/, NUM_7/*7*/, NUM_8/*8*/, NUM_9/*9*/, NUM_0/*10*/, MINUS/*11*/, EQUAL/*12*/, BACKSPACE/*13*/,
        TAB/*14*/, Q/*15*/, W/*16*/, E/*17*/, R/*18*/, T/*19*/, Y/*20*/, U/*21*/, I/*22*/, O/*23*/, P/*24*/, LEFT_U_BRACE/*25*/, RIGHT_U_BRACE/*26*/, BACKSLASH/*27*/,
        CAP_LOCK/*28*/, A/*29*/, S/*30*/, D/*31*/, F/*32*/, G/*33*/, H/*34*/, J/*35*/, K/*36*/, L/*37*/, SEMI_COLON/*38*/, QUOTE/*39*/, ENTER/*40*/,
        LEFT_SHIFT/*41*/, Z/*42*/, X/*43*/, C/*44*/, V/*45*/, B/*46*/, N/*47*/, M/*48*/, COMMA/*49*/, PERIOD/*50*/, SLASH/*51*/, RIGHT_SHIFT/*52*/, UP_ARROW/*53*/, DELETE/*54*/,
        LEFT_CTRL/*55*/, LEFT_GUI/*56*/, LEFT_ALT/*57*/, SPACE/*58*/, RIGHT_ALT/*59*/, FN/*60*/, LEFT_ARROW/*61*/, DOWN_ARROW/*62*/, RIGHT_ARROW/*63*/,
    },

    {
        GRAVE_ACCENT, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, BACKSPACE,
        TAB, PROFILE1, PROFILE2, PROFILE3, PROFILE4, T, Y, U, I, O, P, LEFT_U_BRACE, RIGHT_U_BRACE, BACKSLASH,
        CAP_LOCK, A, S, D, F, G, H, J, K, L, SEMI_COLON, QUOTE, ENTER,
        LEFT_SHIFT, Z, X, C, V, B, N, M, COMMA, PERIOD, SLASH, RIGHT_SHIFT, UP_ARROW, DELETE,
        LEFT_CTRL, LEFT_GUI, LEFT_ALT, SPACE, RIGHT_ALT, FN, LEFT_ARROW, DOWN_ARROW, RIGHT_ARROW,
    }

};

const uint8_t g_rgb_mapping[ADVANCED_KEY_NUM]={26,25,24,23,9,10,11,12,36,37,38,39,53,52,51,50,
                                             35,34,33,32,19,20,21,22,46,47,48,49,63,62,61,60,
                                             3,15,29,42,41,56,28,14,13,27,40,55,54,0,1,2,
                                             4,5,6,7,8,18,17,31,44,45,59,58,16,30,43,57};
const RGBLocation g_rgb_locations[RGB_NUM]={{0,0.625},{0,1.875},{0,3.125},{0,6.875},{0,10.5},{0,11.5},{0,12.5},{0,13.5},{0,14.5},
                                             {1,1},{1,2.5},{1,3.5},{1,4.5},{1,5.5},{1,6.5},{1,7.5},{1,8.5},{1,9.5},{1,10.5},{1,11.5},{1,12.5},{1,13.5},{1,14.5},
                                             {2,0.875},{2,2.25},{2,3.25},{2,4.25},{2,5.25},{2,6.25},{2,7.25},{2,8.25},{2,9.25},{2,10.25},{2,11.25},{2,12.25},{2,13.875},
                                             {3,0.75},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7},{3,8},{3,9},{3,10},{3,11},{3,12},{3,13},{3,14.25},
                                             {4,0.5},{4,1.5},{4,2.5},{4,3.5},{4,4.5},{4,5.5},{4,6.5},{4,7.5},{4,8.5},{4,9.5},{4,10.5},{4,11.5},{4,12.5},{4,14}};

AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM] =
{
    //Group 1 Begin
    {.key.id = 31},
    {.key.id = 30},
    {.key.id = 29},
    {.key.id = 28},
    {.key.id = 41}, //Left Shift
    {.key.id = 42},
    {.key.id = 43},
    {.key.id = 44},
    {.key.id = 14},
    {.key.id = 15},
    {.key.id = 16},
    {.key.id = 17},
    {.key.id = 3},
    {.key.id = 2},
    {.key.id = 1},
    {.key.id = 0},
    //Group 2 Begin
    {.key.id = 40},
    {.key.id = 39},
    {.key.id = 38},
    {.key.id = 37},
    {.key.id = 51},
    {.key.id = 52}, //Right Shift
    {.key.id = 53},
    {.key.id = 54},
    {.key.id = 24},
    {.key.id = 25},
    {.key.id = 26},
    {.key.id = 27},
    {.key.id = 13},
    {.key.id = 12},
    {.key.id = 11},
    {.key.id = 10},
    //Group 3 Begin
    {.key.id = 58},
    {.key.id = 47},
    {.key.id = 34},
    {.key.id = 20},
    {.key.id = 19},
    {.key.id = 6},
    {.key.id = 33},
    {.key.id = 46},
    {.key.id = 45},
    {.key.id = 32},
    {.key.id = 18},
    {.key.id = 5},
    {.key.id = 4},
    {.key.id = 55}, //Left Control
    {.key.id = 56}, //Left GUI
    {.key.id = 57}, //Left Alt
    //Group 4 Begin
    {.key.id = 59}, //Right alt
    {.key.id = 60}, //Fn
    {.key.id = 61},
    {.key.id = 62},
    {.key.id = 63},
    {.key.id = 50},
    {.key.id = 49},
    {.key.id = 36},
    {.key.id = 22},
    {.key.id = 23},
    {.key.id = 9},
    {.key.id = 8},
    {.key.id = 48},
    {.key.id = 35},
    {.key.id = 21},
    {.key.id = 7}
};

void keyboard_hid_send(uint8_t*report,uint16_t len)
{
    USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS,g_keyboard_report_buffer,17+1);
}


float advanced_key_normalize(AdvancedKey* key, float value)
{
    float x = (key->upper_bound - value) / (key->upper_bound - key->lower_bound);
    if (x<0.225)
    {
        return x*(0.5/0.225);
    }
    else if (x<0.404)
    {
        return (x-0.225)*(0.25/(0.404-0.225))+0.5;
    }
    else
    {
        return (x-0.404)*(0.25/(1.0-0.404))+0.75;
    }
    
}