/*
 * lefl_keyboard.c
 */
#include "lefl.h"
const char *hid_usage_id_name[232] =
{
    "No Event",
    "Overrun Error",
    "POST Fail",
    "ErrorUndefined",
    "a A",
    "b B",
    "c C",
    "d D",
    "e E",
    "f F",
    "g G",
    "h H",
    "i I",
    "j J",
    "k K",
    "l L",
    "m M",
    "n N",
    "o O",
    "p P",
    "q Q",
    "r R",
    "s S",
    "t T",
    "u U",
    "v V",
    "w W",
    "x X",
    "y Y",
    "z Z",
    "1 !",
    "2 @",
    "3 #",
    "4 $",
    "5 %",
    "6 ^",
    "7 &",
    "8 *",
    "9 (",
    "0 )",
    "Return",
    "Escape",
    "Backspace",
    "Tab",
    "Space",
    "- _",
    "= +",
    "[ {",
    "] }",
    "\\ |",
    "Europe 1 (Note 2)",
    "; :",
    "\' \"",
    "` ~",
    ", <",
    ". >",
    "/ ?",
    "Caps Lock",
    "F1",
    "F2",
    "F3",
    "F4",
    "F5",
    "F6",
    "F7",
    "F8",
    "F9",
    "F10",
    "F11",
    "F12",
    "Print Screen (Note 1)",
    "Scroll Lock",
    "Break (Pause)",
    "Insert (Note 1)",
    "Home (Note 1)",
    "Page Up (Note 1)",
    "Delete (Note 1)",
    "End (Note 1)",
    "Page Down (Note 1)",
    "Right Arrow (Note 1)",
    "Left Arrow (Note 1)",
    "Down Arrow (Note 1)",
    "Up Arrow (Note 1)",
    "Num Lock",
    "Keypad / (Note 1)",
    "Keypad *",
    "Keypad -",
    "Keypad +",
    "Keypad Enter",
    "Keypad 1 End",
    "Keypad 2 Down",
    "Keypad 3 PageDn",
    "Keypad 4 Left",
    "Keypad 5",
    "Keypad 6 Right",
    "Keypad 7 Home",
    "Keypad 8 Up",
    "Keypad 9 PageUp",
    "Keypad 0 Insert",
    "Keypad . Delete",
    "Europe 2 (Note 2)",
    "App",
    "Keyboard Power",
    "Keypad =",
    "F13",
    "F14",
    "F15",
    "F16",
    "F17",
    "F18",
    "F19",
    "F20",
    "F21",
    "F22",
    "F23",
    "F24",
    "Keyboard Execute",
    "Keyboard Help",
    "Keyboard Menu",
    "Keyboard Select",
    "Keyboard Stop",
    "Keyboard Again",
    "Keyboard Undo",
    "Keyboard Cut",
    "Keyboard Copy",
    "Keyboard Paste",
    "Keyboard Find",
    "Keyboard Mute",
    "Keyboard Volume Up",
    "Keyboard Volume Dn",
    "Keyboard Locking Caps Lock",
    "Keyboard Locking Num Lock",
    "Keyboard Locking Scroll Lock",
    "Keypad , (Brazilian Keypad .)",
    "Keyboard Equal Sign",
    "Keyboard Int'l 1 ろ (Ro)",
    "Keyboard Intl'2 かかかかひひひかロロロ字 (Katakana/Hiragana)",
    "Keyboard Int'l 2 ¥ (Yen)",
    "Keyboard Int'l 4 前前前変変次前前 (    ) 全前前 (Henkan)",
    "Keyboard Int'l 5 無変変 (Muhenkan)",
    "Keyboard Int'l 6 (PC9800 Keypad , )",
    "Keyboard Int'l 7",
    "Keyboard Int'l 8",
    "Keyboard Int'l 9",
    "Keyboard Lang 1 한옝/ (Hanguel/English)",
    "Keyboard Lang 2 한한 (Hanja)",
    "Keyboard Lang 3 かかかか (Katakana)",
    "Keyboard Lang 4 ひひひか (Hiragana)",
    "Keyboard Lang 5 半半全半/(Zenkaku/Hankaku)",
    "Keyboard Lang 6",
    "Keyboard Lang 7",
    "Keyboard Lang 8",
    "Keyboard Lang 9",
    "Keyboard Alternate Erase",
    "Keyboard SysReq/Attention",
    "Keyboard Cancel",
    "Keyboard Clear",
    "Keyboard Prior",
    "Keyboard Return",
    "Keyboard Separator",
    "Keyboard Out",
    "Keyboard Oper",
    "Keyboard Clear/Again",
    "Keyboard CrSel/Props",
    "Keyboard ExSel",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "RESERVED",
    "Left Control",
    "Left Shift",
    "Left Alt",
    "Left GUI",
    "Right Control",
    "Right Shift",
    "Right Alt",
    "Right GUI",
};

void lefl_keyboard_x_increase(lefl_keyboard_t *keyboard,int8_t delta)
{
    keyboard->x+=delta;
    if(keyboard->x>=LEFL_KEYBOARD_MAX_X)
    {
        keyboard->x=0;
        keyboard->y++;
        if(keyboard->y>=LEFL_KEYBOARD_MAX_Y)
        {
            keyboard->x=0;
            keyboard->y=0;
        }
    }
    if(keyboard->x<0)
    {
        keyboard->x=LEFL_KEYBOARD_MAX_X-1;
        keyboard->y--;
        if(keyboard->y<0)
        {
            keyboard->x=LEFL_KEYBOARD_MAX_X-1;
            keyboard->y=LEFL_KEYBOARD_MAX_Y-1;
        }
    }
}
void lefl_keyboard_y_increase(lefl_keyboard_t *keyboard,int8_t delta)
{
    keyboard->y+=delta;
    if(keyboard->y>=LEFL_KEYBOARD_MAX_Y)
    {
        keyboard->y=0;
        keyboard->x++;
        if(keyboard->x>=LEFL_KEYBOARD_MAX_X)
        {
            keyboard->y=0;
            keyboard->x=0;
        }
    }
    if(keyboard->y<0)
    {
        keyboard->y=LEFL_KEYBOARD_MAX_Y-1;
        keyboard->x--;
        if(keyboard->x<0)
        {
            keyboard->y=LEFL_KEYBOARD_MAX_Y-1;
            keyboard->x=LEFL_KEYBOARD_MAX_X-1;
        }
    }
}

void lefl_keyboard_draw(lefl_keyboard_t *keyboard)
{

}
