/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KEYCODE_H_
#define KEYCODE_H_

enum LAYER_CONTROL_KEYCODE {
  LAYER_MOMENTARY = 0x00,
  LAYER_TURN_ON = 0x01,
  LAYER_TURN_OFF = 0x02,
  LAYER_TOGGLE = 0x03,
};

#define LAYER(code,layer) (((code) << 12) | ((layer) << 8) | LAYER_CONTROL)

enum USB_HID_MODIFIER
{
    /*------------------------- HID report data -------------------------*/
    KEY_NO_MODIFIER=0,
    KEY_LEFT_CTRL = 0x01,KEY_LEFT_SHIFT = 0x02,KEY_LEFT_ALT = 0x04,KEY_LEFT_GUI = 0x08,
    KEY_RIGHT_CTRL = 0x10,KEY_RIGHT_SHIFT = 0x20,KEY_RIGHT_ALT = 0x40,KEY_RIGHT_GUI = 0x80,
    /*------------------------- HID report data -------------------------*/
};

enum USB_HID_KEYCODES {
  KEY_NO_EVENT                 = 0x00,   // Keyboard NoEvent
  KEY_ERR_OVF                  = 0x01,   // Keyboard ErrorRollOver
  KEY_POST_FAIL                = 0x02,   // Keyboard POSTFail
  KEY_ERR_UNDEF                = 0x03,   // Keyboard ErrorUndefined
  KEY_A                        = 0x04,   // Keyboard a and A
  KEY_B                        = 0x05,   // Keyboard b and B
  KEY_C                        = 0x06,   // Keyboard c and C
  KEY_D                        = 0x07,   // Keyboard d and D
  KEY_E                        = 0x08,   // Keyboard e and E
  KEY_F                        = 0x09,   // Keyboard f and F
  KEY_G                        = 0x0a,   // Keyboard g and G
  KEY_H                        = 0x0b,   // Keyboard h and H
  KEY_I                        = 0x0c,   // Keyboard i and I
  KEY_J                        = 0x0d,   // Keyboard j and J
  KEY_K                        = 0x0e,   // Keyboard k and K
  KEY_L                        = 0x0f,   // Keyboard l and L
  KEY_M                        = 0x10,   // Keyboard m and M
  KEY_N                        = 0x11,   // Keyboard n and N
  KEY_O                        = 0x12,   // Keyboard o and O
  KEY_P                        = 0x13,   // Keyboard p and P
  KEY_Q                        = 0x14,   // Keyboard q and Q
  KEY_R                        = 0x15,   // Keyboard r and R
  KEY_S                        = 0x16,   // Keyboard s and S
  KEY_T                        = 0x17,   // Keyboard t and T
  KEY_U                        = 0x18,   // Keyboard u and U
  KEY_V                        = 0x19,   // Keyboard v and V
  KEY_W                        = 0x1a,   // Keyboard w and W
  KEY_X                        = 0x1b,   // Keyboard x and X
  KEY_Y                        = 0x1c,   // Keyboard y and Y
  KEY_Z                        = 0x1d,   // Keyboard z and Z
  KEY_1                        = 0x1e,   // Keyboard 1 and !
  KEY_2                        = 0x1f,   // Keyboard 2 and @
  KEY_3                        = 0x20,   // Keyboard 3 and #
  KEY_4                        = 0x21,   // Keyboard 4 and $
  KEY_5                        = 0x22,   // Keyboard 5 and %
  KEY_6                        = 0x23,   // Keyboard 6 and ^
  KEY_7                        = 0x24,   // Keyboard 7 and &
  KEY_8                        = 0x25,   // Keyboard 8 and *
  KEY_9                        = 0x26,   // Keyboard 9 and (
  KEY_0                        = 0x27,   // Keyboard 0 and )
  KEY_ENTER                    = 0x28,   // Keyboard Return (ENTER)
  KEY_ESC                      = 0x29,   // Keyboard ESCAPE
  KEY_BACKSPACE                = 0x2a,   // Keyboard DELETE (Backspace)
  KEY_TAB                      = 0x2b,   // Keyboard Tab
  KEY_SPACEBAR                 = 0x2c,   // Keyboard Spacebar
  KEY_MINUS                    = 0x2d,   // Keyboard - and _
  KEY_EQUAL                    = 0x2e,   // Keyboard = and +
  KEY_LEFT_BRACE               = 0x2f,   // Keyboard [ and {
  KEY_RIGHT_BRACE              = 0x30,   // Keyboard ] and }
  KEY_BACKSLASH                = 0x31,   // Keyboard \ and |
  KEY_NUMBER_SIGN              = 0x32,   // Keyboard Non-US # and ~
  KEY_SEMICOLON                = 0x33,   // Keyboard ; and :
  KEY_APOSTROPHE               = 0x34,   // Keyboard ‘ and “
  KEY_GRAVE                    = 0x35,   // Keyboard Grave Accent and Tilde
  KEY_COMMA                    = 0x36,   // Keyboard , and <
  KEY_DOT                      = 0x37,
  KEY_SLASH                    = 0x38,   // Keyboard . and >
  KEY_CAPS_LOCK                = 0x39,   // Keyboard Caps Lock
  KEY_F1                       = 0x3a,   // Keyboard F1
  KEY_F2                       = 0x3b,   // Keyboard F2
  KEY_F3                       = 0x3c,   // Keyboard F3
  KEY_F4                       = 0x3d,   // Keyboard F4
  KEY_F5                       = 0x3e,   // Keyboard F5
  KEY_F6                       = 0x3f,   // Keyboard F6
  KEY_F7                       = 0x40,   // Keyboard F7
  KEY_F8                       = 0x41,   // Keyboard F8
  KEY_F9                       = 0x42,   // Keyboard F9
  KEY_F10                      = 0x43,   // Keyboard F10
  KEY_F11                      = 0x44,   // Keyboard F11
  KEY_F12                      = 0x45,   // Keyboard F12
  KEY_PRINT_SCREEN             = 0x46,   // Keyboard PrintScreen
  KEY_SCROLL_LOCK              = 0x47,   // Keyboard Scroll Lock
  KEY_PAUSE                    = 0x48,   // Keyboard Pause
  KEY_INSERT                   = 0x49,   // Keyboard Insert
  KEY_HOME                     = 0x4a,   // Keyboard Home
  KEY_PAGE_UP                  = 0x4b,   // Keyboard PageUp
  KEY_DELETE                   = 0x4c,   // Keyboard Delete Forward
  KEY_END                      = 0x4d,   // Keyboard End
  KEY_PAGE_DOWN                = 0x4e,   // Keyboard PageDown
  KEY_RIGHT_ARROW              = 0x4f,   // Keyboard RightArrow
  KEY_LEFT_ARROW               = 0x50,   // Keyboard LeftArrow
  KEY_DOWN_ARROW               = 0x51,   // Keyboard DownArrow
  KEY_UP_ARROW                 = 0x52,   // Keyboard UpArrow
  KEY_NUM_LOCK                 = 0x53,   // Keypad Num Lock and Clear
  KEYPAD_DIVIDE                = 0x54,   // Keypad /
  KEYPAD_MULTIPLY              = 0x55,   // Keypad *
  KEYPAD_MINUS                 = 0x56,   // Keypad -
  KEYPAD_PLUS                  = 0x57,   // Keypad +
  KEYPAD_ENTER                 = 0x58,   // Keypad ENTER
  KEYPAD_1                     = 0x59,   // Keypad 1 and End
  KEYPAD_2                     = 0x5a,   // Keypad 2 and Down Arrow
  KEYPAD_3                     = 0x5b,   // Keypad 3 and PageDn
  KEYPAD_4                     = 0x5c,   // Keypad 4 and Left Arrow
  KEYPAD_5                     = 0x5d,   // Keypad 5
  KEYPAD_6                     = 0x5e,   // Keypad 6 and Right Arrow
  KEYPAD_7                     = 0x5f,   // Keypad 7 and Home
  KEYPAD_8                     = 0x60,   // Keypad 8 and Up Arrow
  KEYPAD_9                     = 0x61,   // Keypad 9 and PageUp
  KEYPAD_0                     = 0x62,   // Keypad 0 and Insert
  KEYPAD_DOT                   = 0x63,   // Keypad . and Delete
  KEY_NON_US_BS                = 0x64,   // Keyboard Non-US \ and |
  KEY_APPLICATION              = 0x65,   // Keyboard Application
  KEY_POWER                    = 0x66,   // Keyboard Power
  KEY_EQUAL_PLUS               = 0x67,   // Keypad =
  KEY_F13                      = 0x68,   // Keyboard F13
  KEY_F14                      = 0x69,   // Keyboard F14
  KEY_F15                      = 0x6a,   // Keyboard F15
  KEY_F16                      = 0x6b,   // Keyboard F16
  KEY_F17                      = 0x6c,   // Keyboard F17
  KEY_F18                      = 0x6d,   // Keyboard F
  KEY_F19                      = 0x6e,   // Keyboard F19
  KEY_F20                      = 0x6f,   // Keyboard F20
  KEY_F21                      = 0x70,   // Keyboard F21
  KEY_F22                      = 0x71,   // Keyboard F22
  KEY_F23                      = 0x72,   // Keyboard F23
  KEY_F24                      = 0x73,   // Keyboard F24
  KEY_EXECUTE                  = 0x74,   // Keyboard Execute
  KEY_HELP                     = 0x75,   // Keyboard Help
  KEY_MENU                     = 0x76,   // Keyboard Menu
  KEY_SELECT                   = 0x77,   // Keyboard Select
  KEY_STOP                     = 0x78,   // Keyboard Stop
  KEY_AGAIN                    = 0x79,   // Keyboard Again
  KEY_UNDO                     = 0x7a,   // Keyboard Undo
  KEY_CUT                      = 0x7b,   // Keyboard Cut
  KEY_COPY                     = 0x7c,   // Keyboard Copy
  KEY_PASTE                    = 0x7d,   // Keyboard Paste
  KEY_FIND                     = 0x7e,   // Keyboard Find
  KEY_MUTE                     = 0x7f,   // Keyboard Mute
  KEY_VOLUME_UP                = 0x80,   // Keyboard Volume Up
  KEY_VOLUME_DOWN              = 0x81,   // Keyboard Volume Down
  KEY_LOCKING_CAPS_LOCK        = 0x82,   // Keyboard Locking Caps Lock
  KEY_LOCKING_NUM_LOCK         = 0x83,   // Keyboard Locking Num Lock
  KEY_LOCKING_SCROLL_LOCK      = 0x84,   // Keyboard Locking Scroll Lock
  KEYPAD_COMMA                 = 0x85,   // Keypad Comma
  KEY_EQUAL_SIGN               = 0x86,   // Keypad Equal Sign
  KEY_INTL1                    = 0x87,   // Keyboard International1
  KEY_INTL2                    = 0x88,   // Keyboard International2
  KEY_INTL3                    = 0x89,   // Keyboard International3
  KEY_INTL4                    = 0x8a,   // Keyboard International4
  KEY_INTL5                    = 0x8b,   // Keyboard International5
  KEY_INTL6                    = 0x8c,   // Keyboard International6
  KEY_INTL7                    = 0x8d,   // Keyboard International7
  KEY_INTL8                    = 0x8e,   // Keyboard International8
  KEY_INTL9                    = 0x8f,   // Keyboard International9
  KEY_LANG1                    = 0x90,   // Keyboard LANG1
  KEY_LANG2                    = 0x91,   // Keyboard LANG2
  KEY_LANG3                    = 0x92,   // Keyboard LANG3
  KEY_LANG4                    = 0x93,   // Keyboard LANG4
  KEY_LANG5                    = 0x94,   // Keyboard LANG5
  KEY_LANG6                    = 0x95,   // Keyboard LANG6
  KEY_LANG7                    = 0x96,   // Keyboard LANG7
  KEY_LANG8                    = 0x97,   // Keyboard LANG8
  KEY_LANG9                    = 0x98,   // Keyboard LANG9
  KEY_ALTERNATE_ERASE          = 0x99,   // Keyboard AlternateErase
  KEY_SYSREQ_ATTENTION         = 0x9a,   // Keyboard SysReq/Attention
  KEY_CANCEL                   = 0x9b,   // Keyboard Cancel
  KEY_CLEAR                    = 0x9c,   // Keyboard Clear
  KEY_PRIOR                    = 0x9d,   // Keyboard Prior
  KEY_RETURN                   = 0x9e,   // Keyboard Return
  KEY_SEPARATOR                = 0x9f,   // Keyboard Separator
  KEY_OUT                      = 0xa0,   // Keyboard Out
  KEY_OPER                     = 0xa1,   // Keyboard Oper
  KEY_CLEAR_AGAIN              = 0xa2,   // Keyboard Clear/Again
  KEY_CRSEL_PROPS              = 0xa3,   // Keyboard CrSel/Props
  KEY_EXSEL                    = 0xa4,   // Keyboard ExSel
  //Reserved b5-DF
  MOUSE_COLLECTION = 0xa5,
  LAYER_CONTROL = 0xa6,
  FN = 0xB0,
  KEY_USER = 0xFD,
  KEY_SYSTEM = 0xFE,
  KEY_TRANSPARENT = 0xFF,
};

enum USB_HID_MOUSE_KEYCODES {
  MOUSE_LBUTTON = 0x00,
  MOUSE_RBUTTON = 0x01,
  MOUSE_MBUTTON = 0x02,
  MOUSE_FORWARD = 0x03,
  MOUSE_BACK = 0x04,
  MOUSE_WHEEL_UP = 0x05,
  MOUSE_WHEEL_DOWN = 0x06,
};

enum SYSTEM_KEYCODES {
  SYSTEM_RESET = 0x00,
  SYSTEM_FACTORY_RESET = 0x01,
  SYSTEM_SAVE = 0x02,
  SYSTEM_BOOTLOADER = 0x03,
  SYSTEM_DEBUG = 0x04,
  SYSTEM_RESET_TO_DEFAULT = 0x05,
  SYSTEM_CONFIG0 = 0x10,
  SYSTEM_CONFIG1 = 0x11,
  SYSTEM_CONFIG2 = 0x12,
  SYSTEM_CONFIG3 = 0x13,
};

#endif /* KEYCODE_H_ */
