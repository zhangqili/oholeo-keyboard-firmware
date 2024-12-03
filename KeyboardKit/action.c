#include "action.h"
#include "keyboard.h"

void action_execute(Key*key, Action*action)
{
    switch (*action)
    {
    case ACTION_DYNAMIC_KEYSTROKE:
        
        break;
    case ACTION_MOD_TAP:    
        break;
    case ACTION_TOGGLE_KEY:
        
        break;
    case ACTION_RAPPY_SNAPPY:
        action_rs_execute(key, action);    
        
        break;
    default:
        break;
    }
}

void action_rs_execute(Key*key, Action*action)
{
    ActionRappySnappy*action_rs=(ActionRappySnappy*)action;
    if (key->id == action_rs->key1_id)
    {
        if (((g_keyboard_advanced_keys[action_rs->key1_id].value > g_keyboard_advanced_keys[action_rs->key2_id].value)&&
        (g_keyboard_advanced_keys[action_rs->key1_id].value > g_keyboard_advanced_keys[action_rs->key1_id].upper_deadzone)) || 
        ((g_keyboard_advanced_keys[action_rs->key1_id].value>= (1.0 - g_keyboard_advanced_keys[action_rs->key1_id].lower_deadzone))&&
        (g_keyboard_advanced_keys[action_rs->key2_id].value>= (1.0 - g_keyboard_advanced_keys[action_rs->key2_id].lower_deadzone))))
        {
            //KEYBOARD_REPORT_BUFFER_ADD(g_keymap[g_keyboard_current_layer][key->id]);
        }
    }
    else if  (key->id == action_rs->key2_id)
    {
        if (((g_keyboard_advanced_keys[action_rs->key1_id].value < g_keyboard_advanced_keys[action_rs->key2_id].value)&&
        (g_keyboard_advanced_keys[action_rs->key2_id].value > g_keyboard_advanced_keys[action_rs->key2_id].upper_deadzone)) || 
        ((g_keyboard_advanced_keys[action_rs->key1_id].value>= (1.0 - g_keyboard_advanced_keys[action_rs->key1_id].lower_deadzone))&&
        (g_keyboard_advanced_keys[action_rs->key2_id].value>= (1.0 - g_keyboard_advanced_keys[action_rs->key2_id].lower_deadzone))))
        {
            //KEYBOARD_REPORT_BUFFER_ADD(g_keymap[g_keyboard_current_layer][key->id]);
        }
    }
}