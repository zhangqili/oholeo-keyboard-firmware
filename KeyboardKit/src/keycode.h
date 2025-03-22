/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef KEYCODE_H_
#define KEYCODE_H_

#include "stdint.h"

typedef uint16_t Keycode;

enum LayerControlKeycode {
  LAYER_MOMENTARY = 0x00,
  LAYER_TURN_ON = 0x01,
  LAYER_TURN_OFF = 0x02,
  LAYER_TOGGLE = 0x03,
};

#define LAYER(code,layer) (((code) << 12) | ((layer) << 8) | LAYER_CONTROL)

enum ModifierKeycode
{
    /*------------------------- HID report data -------------------------*/
    KEY_NO_MODIFIER=0,
    KEY_LEFT_CTRL = 0x01,KEY_LEFT_SHIFT = 0x02,KEY_LEFT_ALT = 0x04,KEY_LEFT_GUI = 0x08,
    KEY_RIGHT_CTRL = 0x10,KEY_RIGHT_SHIFT = 0x20,KEY_RIGHT_ALT = 0x40,KEY_RIGHT_GUI = 0x80,
    /*------------------------- HID report data -------------------------*/
};

enum KeycodeEnum {
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
  DYNAMIC_KEY = 0xa7,
  CONSUMER_COLLECTION = 0xa8,
  SYSTEM_COLLECTION = 0xa9,
  JOYSTICK_COLLECTION = 0xaa,
  MIDI_COLLECTION = 0xab,
  MIDI_NOTE = 0xac,
  KEY_USER = 0xFD,
  KEYBOARD_OPERATION = 0xFE,
  KEY_TRANSPARENT = 0xFF,
};

enum MouseKeycode {
  MOUSE_LBUTTON = 0x00,
  MOUSE_RBUTTON = 0x01,
  MOUSE_MBUTTON = 0x02,
  MOUSE_FORWARD = 0x03,
  MOUSE_BACK = 0x04,
  MOUSE_WHEEL_UP = 0x05,
  MOUSE_WHEEL_DOWN = 0x06,
};

enum KeyboardKeycode {
  KEYBOARD_REBOOT = 0x00,
  KEYBOARD_FACTORY_RESET = 0x01,
  KEYBOARD_SAVE = 0x02,
  KEYBOARD_BOOTLOADER = 0x03,
  KEYBOARD_TOGGLE_DEBUG = 0x04,
  KEYBOARD_RESET_TO_DEFAULT = 0x05,
  KEYBOARD_TOGGLE_NKRO = 0x06,
  KEYBOARD_CONFIG0 = 0x10,
  KEYBOARD_CONFIG1 = 0x11,
  KEYBOARD_CONFIG2 = 0x12,
  KEYBOARD_CONFIG3 = 0x13,
};

enum ConsumerKeycode
{
  // 15.5 Display Controls
  CONSUMER_SNAPSHOT        = 0x01,
  CONSUMER_BRIGHTNESS_UP   = 0x02, // https://www.usb.org/sites/default/files/hutrr41_0.pdf
  CONSUMER_BRIGHTNESS_DOWN = 0x03,
  // 15.7 Transport Controls
  CONSUMER_TRANSPORT_RECORD       = 0x04,
  CONSUMER_TRANSPORT_FAST_FORWARD = 0x05,
  CONSUMER_TRANSPORT_REWIND       = 0x06,
  CONSUMER_TRANSPORT_NEXT_TRACK   = 0x07,
  CONSUMER_TRANSPORT_PREV_TRACK   = 0x08,
  CONSUMER_TRANSPORT_STOP         = 0x09,
  CONSUMER_TRANSPORT_EJECT        = 0x0A,
  CONSUMER_TRANSPORT_RANDOM_PLAY  = 0x0B,
  CONSUMER_TRANSPORT_STOP_EJECT   = 0x0C,
  CONSUMER_TRANSPORT_PLAY_PAUSE   = 0x0D,
  // 15.9.1 Audio Controls - Volume
  CONSUMER_AUDIO_MUTE     = 0x0E,
  CONSUMER_AUDIO_VOL_UP   = 0x0F,
  CONSUMER_AUDIO_VOL_DOWN = 0x10,
  // 15.15 Application Launch Buttons
  CONSUMER_AL_CC_CONFIG       = 0x11,
  CONSUMER_AL_EMAIL           = 0x12,
  CONSUMER_AL_CALCULATOR      = 0x13,
  CONSUMER_AL_LOCAL_BROWSER   = 0x14,
  CONSUMER_AL_LOCK            = 0x15,
  CONSUMER_AL_CONTROL_PANEL   = 0x16,
  CONSUMER_AL_ASSISTANT       = 0x17,
  CONSUMER_AL_KEYBOARD_LAYOUT = 0x18,
  // 15.16 Generic GUI Application Controls
  CONSUMER_AC_NEW                         = 0x19,
  CONSUMER_AC_OPEN                        = 0x1A,
  CONSUMER_AC_CLOSE                       = 0x1B,
  CONSUMER_AC_EXIT                        = 0x1C,
  CONSUMER_AC_MAXIMIZE                    = 0x1D,
  CONSUMER_AC_MINIMIZE                    = 0x1E,
  CONSUMER_AC_SAVE                        = 0x1F,
  CONSUMER_AC_PRINT                       = 0x20,
  CONSUMER_AC_PROPERTIES                  = 0x21,
  CONSUMER_AC_UNDO                        = 0x22,
  CONSUMER_AC_COPY                        = 0x23,
  CONSUMER_AC_CUT                         = 0x24,
  CONSUMER_AC_PASTE                       = 0x25,
  CONSUMER_AC_SELECT_ALL                  = 0x26,
  CONSUMER_AC_FIND                        = 0x27,
  CONSUMER_AC_SEARCH                      = 0x28,
  CONSUMER_AC_HOME                        = 0x29,
  CONSUMER_AC_BACK                        = 0x2A,
  CONSUMER_AC_FORWARD                     = 0x2B,
  CONSUMER_AC_STOP                        = 0x2C,
  CONSUMER_AC_REFRESH                     = 0x2D,
  CONSUMER_AC_BOOKMARKS                   = 0x2E,
  CONSUMER_AC_NEXT_KEYBOARD_LAYOUT_SELECT = 0x2F,
  CONSUMER_AC_DESKTOP_SHOW_ALL_WINDOWS    = 0x30,
  CONSUMER_AC_SOFT_KEY_LEFT               = 0x31,
};

enum SystemRawKeycode
{
  SYSTEM_POWER_DOWN             = 0x81,
  SYSTEM_SLEEP                  = 0x82,
  SYSTEM_WAKE_UP                = 0x83,
  SYSTEM_RESTART                = 0x8F,
  SYSTEM_DISPLAY_TOGGLE_INT_EXT = 0xB5,
};
/* Consumer Page (0x0C)
 *
 * See https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf#page=75
 */
enum ConsumerRawKeycode
{
  // 15.5 Display Controls
  SNAPSHOT        = 0x065,
  BRIGHTNESS_UP   = 0x06F, // https://www.usb.org/sites/default/files/hutrr41_0.pdf
  BRIGHTNESS_DOWN = 0x070,
  // 15.7 Transport Controls
  TRANSPORT_RECORD       = 0x0B2,
  TRANSPORT_FAST_FORWARD = 0x0B3,
  TRANSPORT_REWIND       = 0x0B4,
  TRANSPORT_NEXT_TRACK   = 0x0B5,
  TRANSPORT_PREV_TRACK   = 0x0B6,
  TRANSPORT_STOP         = 0x0B7,
  TRANSPORT_EJECT        = 0x0B8,
  TRANSPORT_RANDOM_PLAY  = 0x0B9,
  TRANSPORT_STOP_EJECT   = 0x0CC,
  TRANSPORT_PLAY_PAUSE   = 0x0CD,
  // 15.9.1 Audio Controls - Volume
  AUDIO_MUTE     = 0x0E2,
  AUDIO_VOL_UP   = 0x0E9,
  AUDIO_VOL_DOWN = 0x0EA,
  // 15.15 Application Launch Buttons
  AL_CC_CONFIG       = 0x183,
  AL_EMAIL           = 0x18A,
  AL_CALCULATOR      = 0x192,
  AL_LOCAL_BROWSER   = 0x194,
  AL_LOCK            = 0x19E,
  AL_CONTROL_PANEL   = 0x19F,
  AL_ASSISTANT       = 0x1CB,
  AL_KEYBOARD_LAYOUT = 0x1AE,
  // 15.16 Generic GUI Application Controls
  AC_NEW                         = 0x201,
  AC_OPEN                        = 0x202,
  AC_CLOSE                       = 0x203,
  AC_EXIT                        = 0x204,
  AC_MAXIMIZE                    = 0x205,
  AC_MINIMIZE                    = 0x206,
  AC_SAVE                        = 0x207,
  AC_PRINT                       = 0x208,
  AC_PROPERTIES                  = 0x209,
  AC_UNDO                        = 0x21A,
  AC_COPY                        = 0x21B,
  AC_CUT                         = 0x21C,
  AC_PASTE                       = 0x21D,
  AC_SELECT_ALL                  = 0x21E,
  AC_FIND                        = 0x21F,
  AC_SEARCH                      = 0x221,
  AC_HOME                        = 0x223,
  AC_BACK                        = 0x224,
  AC_FORWARD                     = 0x225,
  AC_STOP                        = 0x226,
  AC_REFRESH                     = 0x227,
  AC_BOOKMARKS                   = 0x22A,
  AC_NEXT_KEYBOARD_LAYOUT_SELECT = 0x29D,
  AC_DESKTOP_SHOW_ALL_WINDOWS    = 0x29F,
  AC_SOFT_KEY_LEFT               = 0x2A0
};

enum MIDIKeycode 
{
  MIDI_ON                       = 0x00,
  MIDI_OFF                      = 0x01,
  MIDI_TOGGLE                   = 0x02,
  MIDI_NOTE_C_0                 = 0x03,
  MIDI_NOTE_C_SHARP_0           = 0x04,
  MIDI_NOTE_D_0                 = 0x05,
  MIDI_NOTE_D_SHARP_0           = 0x06,
  MIDI_NOTE_E_0                 = 0x07,
  MIDI_NOTE_F_0                 = 0x08,
  MIDI_NOTE_F_SHARP_0           = 0x09,
  MIDI_NOTE_G_0                 = 0x0A,
  MIDI_NOTE_G_SHARP_0           = 0x0B,
  MIDI_NOTE_A_0                 = 0x0C,
  MIDI_NOTE_A_SHARP_0           = 0x0D,
  MIDI_NOTE_B_0                 = 0x0E,
  MIDI_NOTE_C_1                 = 0x0F,
  MIDI_NOTE_C_SHARP_1           = 0x10,
  MIDI_NOTE_D_1                 = 0x11,
  MIDI_NOTE_D_SHARP_1           = 0x12,
  MIDI_NOTE_E_1                 = 0x13,
  MIDI_NOTE_F_1                 = 0x14,
  MIDI_NOTE_F_SHARP_1           = 0x15,
  MIDI_NOTE_G_1                 = 0x16,
  MIDI_NOTE_G_SHARP_1           = 0x17,
  MIDI_NOTE_A_1                 = 0x18,
  MIDI_NOTE_A_SHARP_1           = 0x19,
  MIDI_NOTE_B_1                 = 0x1A,
  MIDI_NOTE_C_2                 = 0x1B,
  MIDI_NOTE_C_SHARP_2           = 0x1C,
  MIDI_NOTE_D_2                 = 0x1D,
  MIDI_NOTE_D_SHARP_2           = 0x1E,
  MIDI_NOTE_E_2                 = 0x1F,
  MIDI_NOTE_F_2                 = 0x20,
  MIDI_NOTE_F_SHARP_2           = 0x21,
  MIDI_NOTE_G_2                 = 0x22,
  MIDI_NOTE_G_SHARP_2           = 0x23,
  MIDI_NOTE_A_2                 = 0x24,
  MIDI_NOTE_A_SHARP_2           = 0x25,
  MIDI_NOTE_B_2                 = 0x26,
  MIDI_NOTE_C_3                 = 0x27,
  MIDI_NOTE_C_SHARP_3           = 0x28,
  MIDI_NOTE_D_3                 = 0x29,
  MIDI_NOTE_D_SHARP_3           = 0x2A,
  MIDI_NOTE_E_3                 = 0x2B,
  MIDI_NOTE_F_3                 = 0x2C,
  MIDI_NOTE_F_SHARP_3           = 0x2D,
  MIDI_NOTE_G_3                 = 0x2E,
  MIDI_NOTE_G_SHARP_3           = 0x2F,
  MIDI_NOTE_A_3                 = 0x30,
  MIDI_NOTE_A_SHARP_3           = 0x31,
  MIDI_NOTE_B_3                 = 0x32,
  MIDI_NOTE_C_4                 = 0x33,
  MIDI_NOTE_C_SHARP_4           = 0x34,
  MIDI_NOTE_D_4                 = 0x35,
  MIDI_NOTE_D_SHARP_4           = 0x36,
  MIDI_NOTE_E_4                 = 0x37,
  MIDI_NOTE_F_4                 = 0x38,
  MIDI_NOTE_F_SHARP_4           = 0x39,
  MIDI_NOTE_G_4                 = 0x3A,
  MIDI_NOTE_G_SHARP_4           = 0x3B,
  MIDI_NOTE_A_4                 = 0x3C,
  MIDI_NOTE_A_SHARP_4           = 0x3D,
  MIDI_NOTE_B_4                 = 0x3E,
  MIDI_NOTE_C_5                 = 0x3F,
  MIDI_NOTE_C_SHARP_5           = 0x40,
  MIDI_NOTE_D_5                 = 0x41,
  MIDI_NOTE_D_SHARP_5           = 0x42,
  MIDI_NOTE_E_5                 = 0x43,
  MIDI_NOTE_F_5                 = 0x44,
  MIDI_NOTE_F_SHARP_5           = 0x45,
  MIDI_NOTE_G_5                 = 0x46,
  MIDI_NOTE_G_SHARP_5           = 0x47,
  MIDI_NOTE_A_5                 = 0x48,
  MIDI_NOTE_A_SHARP_5           = 0x49,
  MIDI_NOTE_B_5                 = 0x4A,
  MIDI_OCTAVE_N2                = 0x4B,
  MIDI_OCTAVE_N1                = 0x4C,
  MIDI_OCTAVE_0                 = 0x4D,
  MIDI_OCTAVE_1                 = 0x4E,
  MIDI_OCTAVE_2                 = 0x4F,
  MIDI_OCTAVE_3                 = 0x50,
  MIDI_OCTAVE_4                 = 0x51,
  MIDI_OCTAVE_5                 = 0x52,
  MIDI_OCTAVE_6                 = 0x53,
  MIDI_OCTAVE_7                 = 0x54,
  MIDI_OCTAVE_DOWN              = 0x55,
  MIDI_OCTAVE_UP                = 0x56,
  MIDI_TRANSPOSE_N6             = 0x57,
  MIDI_TRANSPOSE_N5             = 0x58,
  MIDI_TRANSPOSE_N4             = 0x59,
  MIDI_TRANSPOSE_N3             = 0x5A,
  MIDI_TRANSPOSE_N2             = 0x5B,
  MIDI_TRANSPOSE_N1             = 0x5C,
  MIDI_TRANSPOSE_0              = 0x5D,
  MIDI_TRANSPOSE_1              = 0x5E,
  MIDI_TRANSPOSE_2              = 0x5F,
  MIDI_TRANSPOSE_3              = 0x60,
  MIDI_TRANSPOSE_4              = 0x61,
  MIDI_TRANSPOSE_5              = 0x62,
  MIDI_TRANSPOSE_6              = 0x63,
  MIDI_TRANSPOSE_DOWN           = 0x64,
  MIDI_TRANSPOSE_UP             = 0x65,
  MIDI_VELOCITY_0               = 0x66,
  MIDI_VELOCITY_1               = 0x67,
  MIDI_VELOCITY_2               = 0x68,
  MIDI_VELOCITY_3               = 0x69,
  MIDI_VELOCITY_4               = 0x6A,
  MIDI_VELOCITY_5               = 0x6B,
  MIDI_VELOCITY_6               = 0x6C,
  MIDI_VELOCITY_7               = 0x6D,
  MIDI_VELOCITY_8               = 0x6E,
  MIDI_VELOCITY_9               = 0x6F,
  MIDI_VELOCITY_10              = 0x70,
  MIDI_VELOCITY_DOWN            = 0x71,
  MIDI_VELOCITY_UP              = 0x72,
  MIDI_CHANNEL_1                = 0x73,
  MIDI_CHANNEL_2                = 0x74,
  MIDI_CHANNEL_3                = 0x75,
  MIDI_CHANNEL_4                = 0x76,
  MIDI_CHANNEL_5                = 0x77,
  MIDI_CHANNEL_6                = 0x78,
  MIDI_CHANNEL_7                = 0x79,
  MIDI_CHANNEL_8                = 0x7A,
  MIDI_CHANNEL_9                = 0x7B,
  MIDI_CHANNEL_10               = 0x7C,
  MIDI_CHANNEL_11               = 0x7D,
  MIDI_CHANNEL_12               = 0x7E,
  MIDI_CHANNEL_13               = 0x7F,
  MIDI_CHANNEL_14               = 0x80,
  MIDI_CHANNEL_15               = 0x81,
  MIDI_CHANNEL_16               = 0x82,
  MIDI_CHANNEL_DOWN             = 0x83,
  MIDI_CHANNEL_UP               = 0x84,
  MIDI_ALL_NOTES_OFF            = 0x85,
  MIDI_SUSTAIN                  = 0x86,
  MIDI_PORTAMENTO               = 0x87,
  MIDI_SOSTENUTO                = 0x88,
  MIDI_SOFT                     = 0x89,
  MIDI_LEGATO                   = 0x8A,
  MIDI_MODULATION               = 0x8B,
  MIDI_MODULATION_SPEED_DOWN    = 0x8C,
  MIDI_MODULATION_SPEED_UP      = 0x8D,
  MIDI_PITCH_BEND_DOWN          = 0x8E,
  MIDI_PITCH_BEND_UP            = 0x8F,
};

enum MIDINoteKeycode 
{
  MIDI_DIRECT_NOTE_C_0       ,
  MIDI_DIRECT_NOTE_C_SHARP_0 ,
  MIDI_DIRECT_NOTE_D_0       ,
  MIDI_DIRECT_NOTE_D_SHARP_0 ,
  MIDI_DIRECT_NOTE_E_0       ,
  MIDI_DIRECT_NOTE_F_0       ,
  MIDI_DIRECT_NOTE_F_SHARP_0 ,
  MIDI_DIRECT_NOTE_G_0       ,
  MIDI_DIRECT_NOTE_G_SHARP_0 ,
  MIDI_DIRECT_NOTE_A_0       ,
  MIDI_DIRECT_NOTE_A_SHARP_0 ,
  MIDI_DIRECT_NOTE_B_0       ,
  MIDI_DIRECT_NOTE_C_1       ,
  MIDI_DIRECT_NOTE_C_SHARP_1 ,
  MIDI_DIRECT_NOTE_D_1       ,
  MIDI_DIRECT_NOTE_D_SHARP_1 ,
  MIDI_DIRECT_NOTE_E_1       ,
  MIDI_DIRECT_NOTE_F_1       ,
  MIDI_DIRECT_NOTE_F_SHARP_1 ,
  MIDI_DIRECT_NOTE_G_1       ,
  MIDI_DIRECT_NOTE_G_SHARP_1 ,
  MIDI_DIRECT_NOTE_A_1       ,
  MIDI_DIRECT_NOTE_A_SHARP_1 ,
  MIDI_DIRECT_NOTE_B_1       ,
  MIDI_DIRECT_NOTE_C_2       ,
  MIDI_DIRECT_NOTE_C_SHARP_2 ,
  MIDI_DIRECT_NOTE_D_2       ,
  MIDI_DIRECT_NOTE_D_SHARP_2 ,
  MIDI_DIRECT_NOTE_E_2       ,
  MIDI_DIRECT_NOTE_F_2       ,
  MIDI_DIRECT_NOTE_F_SHARP_2 ,
  MIDI_DIRECT_NOTE_G_2       ,
  MIDI_DIRECT_NOTE_G_SHARP_2 ,
  MIDI_DIRECT_NOTE_A_2       ,
  MIDI_DIRECT_NOTE_A_SHARP_2 ,
  MIDI_DIRECT_NOTE_B_2       ,
  MIDI_DIRECT_NOTE_C_3       ,
  MIDI_DIRECT_NOTE_C_SHARP_3 ,
  MIDI_DIRECT_NOTE_D_3       ,
  MIDI_DIRECT_NOTE_D_SHARP_3 ,
  MIDI_DIRECT_NOTE_E_3       ,
  MIDI_DIRECT_NOTE_F_3       ,
  MIDI_DIRECT_NOTE_F_SHARP_3 ,
  MIDI_DIRECT_NOTE_G_3       ,
  MIDI_DIRECT_NOTE_G_SHARP_3 ,
  MIDI_DIRECT_NOTE_A_3       ,
  MIDI_DIRECT_NOTE_A_SHARP_3 ,
  MIDI_DIRECT_NOTE_B_3       ,
  MIDI_DIRECT_NOTE_C_4       ,
  MIDI_DIRECT_NOTE_C_SHARP_4 ,
  MIDI_DIRECT_NOTE_D_4       ,
  MIDI_DIRECT_NOTE_D_SHARP_4 ,
  MIDI_DIRECT_NOTE_E_4       ,
  MIDI_DIRECT_NOTE_F_4       ,
  MIDI_DIRECT_NOTE_F_SHARP_4 ,
  MIDI_DIRECT_NOTE_G_4       ,
  MIDI_DIRECT_NOTE_G_SHARP_4 ,
  MIDI_DIRECT_NOTE_A_4       ,
  MIDI_DIRECT_NOTE_A_SHARP_4 ,
  MIDI_DIRECT_NOTE_B_4       ,
  MIDI_DIRECT_NOTE_C_5       ,
  MIDI_DIRECT_NOTE_C_SHARP_5 ,
  MIDI_DIRECT_NOTE_D_5       ,
  MIDI_DIRECT_NOTE_D_SHARP_5 ,
  MIDI_DIRECT_NOTE_E_5       ,
  MIDI_DIRECT_NOTE_F_5       ,
  MIDI_DIRECT_NOTE_F_SHARP_5 ,
  MIDI_DIRECT_NOTE_G_5       ,
  MIDI_DIRECT_NOTE_G_SHARP_5 ,
  MIDI_DIRECT_NOTE_A_5       ,
  MIDI_DIRECT_NOTE_A_SHARP_5 ,
  MIDI_DIRECT_NOTE_B_5       ,
  MIDI_DIRECT_NOTE_C_6       ,
  MIDI_DIRECT_NOTE_C_SHARP_6 ,
  MIDI_DIRECT_NOTE_D_6       ,
  MIDI_DIRECT_NOTE_D_SHARP_6 ,
  MIDI_DIRECT_NOTE_E_6       ,
  MIDI_DIRECT_NOTE_F_6       ,
  MIDI_DIRECT_NOTE_F_SHARP_6 ,
  MIDI_DIRECT_NOTE_G_6       ,
  MIDI_DIRECT_NOTE_G_SHARP_6 ,
  MIDI_DIRECT_NOTE_A_6       ,
  MIDI_DIRECT_NOTE_A_SHARP_6 ,
  MIDI_DIRECT_NOTE_B_6       ,
  MIDI_DIRECT_NOTE_C_7       ,
  MIDI_DIRECT_NOTE_C_SHARP_7 ,
  MIDI_DIRECT_NOTE_D_7       ,
  MIDI_DIRECT_NOTE_D_SHARP_7 ,
  MIDI_DIRECT_NOTE_E_7       ,
  MIDI_DIRECT_NOTE_F_7       ,
  MIDI_DIRECT_NOTE_F_SHARP_7 ,
  MIDI_DIRECT_NOTE_G_7       ,
  MIDI_DIRECT_NOTE_G_SHARP_7 ,
  MIDI_DIRECT_NOTE_A_7       ,
  MIDI_DIRECT_NOTE_A_SHARP_7 ,
  MIDI_DIRECT_NOTE_B_7       ,
  MIDI_DIRECT_NOTE_C_8       ,
  MIDI_DIRECT_NOTE_C_SHARP_8 ,
  MIDI_DIRECT_NOTE_D_8       ,
  MIDI_DIRECT_NOTE_D_SHARP_8 ,
  MIDI_DIRECT_NOTE_E_8       ,
  MIDI_DIRECT_NOTE_F_8       ,
  MIDI_DIRECT_NOTE_F_SHARP_8 ,
  MIDI_DIRECT_NOTE_G_8       ,
  MIDI_DIRECT_NOTE_G_SHARP_8 ,
  MIDI_DIRECT_NOTE_A_8       ,
  MIDI_DIRECT_NOTE_A_SHARP_8 ,
  MIDI_DIRECT_NOTE_B_8       ,
  MIDI_DIRECT_NOTE_C_9       ,
  MIDI_DIRECT_NOTE_C_SHARP_9 ,
  MIDI_DIRECT_NOTE_D_9       ,
  MIDI_DIRECT_NOTE_D_SHARP_9 ,
  MIDI_DIRECT_NOTE_E_9       ,
  MIDI_DIRECT_NOTE_F_9       ,
  MIDI_DIRECT_NOTE_F_SHARP_9 ,
  MIDI_DIRECT_NOTE_G_9       ,
  MIDI_DIRECT_NOTE_G_SHARP_9 ,
  MIDI_DIRECT_NOTE_A_9       ,
  MIDI_DIRECT_NOTE_A_SHARP_9 ,
  MIDI_DIRECT_NOTE_B_9       ,
  MIDI_DIRECT_NOTE_C_10      ,
  MIDI_DIRECT_NOTE_C_SHARP_10,
  MIDI_DIRECT_NOTE_D_10      ,
  MIDI_DIRECT_NOTE_D_SHARP_10,
  MIDI_DIRECT_NOTE_E_10      ,
  MIDI_DIRECT_NOTE_F_10      ,
  MIDI_DIRECT_NOTE_F_SHARP_10,
  MIDI_DIRECT_NOTE_G_10      ,
};


static inline uint16_t CONSUMER_KEYCODE_TO_RAWCODE(uint8_t key) {
  switch (key) {
  case CONSUMER_AUDIO_MUTE:
    return AUDIO_MUTE;
  case CONSUMER_AUDIO_VOL_UP:
    return AUDIO_VOL_UP;
  case CONSUMER_AUDIO_VOL_DOWN:
    return AUDIO_VOL_DOWN;
  case CONSUMER_TRANSPORT_NEXT_TRACK:
    return TRANSPORT_NEXT_TRACK;
  case CONSUMER_TRANSPORT_PREV_TRACK:
    return TRANSPORT_PREV_TRACK;
  case CONSUMER_TRANSPORT_FAST_FORWARD:
    return TRANSPORT_FAST_FORWARD;
  case CONSUMER_TRANSPORT_REWIND:
    return TRANSPORT_REWIND;
  case CONSUMER_TRANSPORT_STOP:
    return TRANSPORT_STOP;
  case CONSUMER_TRANSPORT_STOP_EJECT:
    return TRANSPORT_STOP_EJECT;
  case CONSUMER_TRANSPORT_PLAY_PAUSE:
    return TRANSPORT_PLAY_PAUSE;
  case CONSUMER_AL_CC_CONFIG:
    return AL_CC_CONFIG;
  case CONSUMER_AL_EMAIL:
    return AL_EMAIL;
  case CONSUMER_AL_CALCULATOR:
    return AL_CALCULATOR;
  case CONSUMER_AL_LOCAL_BROWSER:
    return AL_LOCAL_BROWSER;
  case CONSUMER_AL_CONTROL_PANEL:
    return AL_CONTROL_PANEL;
  case CONSUMER_AL_ASSISTANT:
    return AL_ASSISTANT;
  case CONSUMER_AC_SEARCH:
    return AC_SEARCH;
  case CONSUMER_AC_HOME:
    return AC_HOME;
  case CONSUMER_AC_BACK:
    return AC_BACK;
  case CONSUMER_AC_FORWARD:
    return AC_FORWARD;
  case CONSUMER_AC_STOP:
    return AC_STOP;
  case CONSUMER_AC_REFRESH:
    return AC_REFRESH;
  case CONSUMER_BRIGHTNESS_UP:
    return BRIGHTNESS_UP;
  case CONSUMER_BRIGHTNESS_DOWN:
    return BRIGHTNESS_DOWN;
  case CONSUMER_AC_BOOKMARKS:
    return AC_BOOKMARKS;
  case CONSUMER_AC_DESKTOP_SHOW_ALL_WINDOWS:
    return AC_DESKTOP_SHOW_ALL_WINDOWS;
  case CONSUMER_AC_SOFT_KEY_LEFT:
    return AC_SOFT_KEY_LEFT;
  default:
    return key;
  }
}

#endif /* KEYCODE_H_ */
