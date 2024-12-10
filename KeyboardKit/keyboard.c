/*
 * keyboard.c
 *
 *  Created on: May 21, 2023
 *      Author: xq123
 */
#include "keyboard.h"
#include "analog.h"
#include "keyboard_conf.h"
#include "rgb.h"
#include "stdio.h"
#include "lfs.h"
#include "action.h"
#include "filter.h"
#include "mouse.h"

__WEAK const uint16_t g_default_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];
__WEAK AdvancedKey g_keyboard_advanced_keys[ADVANCED_KEY_NUM];
__WEAK Key g_keyboard_keys[KEY_NUM];

Action *g_keyboard_actions[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

uint8_t g_keyboard_current_layer;
uint16_t g_keymap[LAYER_NUM][ADVANCED_KEY_NUM + KEY_NUM];

#ifdef NKRO_ENABLE
Keyboard_NKROBuffer g_keyboard_nkro_buffer;
#else
Keyboard_6KROBuffer g_keyboard_6kro_buffer;
#endif

uint8_t g_keyboard_knob_flag;
volatile bool g_keyboard_send_report_enable = true;

volatile bool g_debug_enable;

void keyboard_key_add_buffer(Key *k)
{
    if (g_keyboard_actions[g_keyboard_current_layer][k->id])
    {
        action_execute(k, g_keyboard_actions[g_keyboard_current_layer][k->id]);
    }
    else if (k->state)
    {
        if ((g_keymap[g_keyboard_current_layer][k->id] & 0xFF) <= KEY_EXSEL)
        {
#ifdef NKRO_ENABLE
            keyboard_NKRObuffer_add(&g_keyboard_nkro_buffer, (g_keymap[g_keyboard_current_layer][k->id]));
#else
            keyboard_6KRObuffer_add(&g_keyboard_6kro_buffer, (g_keymap[g_keyboard_current_layer][k->id]));
#endif
        }
        else
        {
            switch (g_keymap[g_keyboard_current_layer][k->id])
            {
            case MOUSE_LBUTTON:
                g_mouse.buttons |= 0x01;
                break;
            case MOUSE_RBUTTON:
                g_mouse.buttons |= 0x02;
                break;
            case MOUSE_MBUTTON:
                g_mouse.buttons |= 0x04;
                break;
            case MOUSE_FORWARD:
                g_mouse.buttons |= 0x08;
                break;
            case MOUSE_BACK:
                g_mouse.buttons |= 0x10;
                break;
            case MOUSE_WHEEL_UP:
                g_mouse.wheel = 1;
                break;
            case MOUSE_WHEEL_DOWN:
                g_mouse.wheel = -1;
                break;
            default:
                break;
            }
        }
    }
}

void keyboard_buffer_send()
{
#ifdef NKRO_ENABLE
    keyboard_NKRObuffer_send(&g_keyboard_nkro_buffer);
#else
    keyboard_6KRObuffer_send(&g_keyboard_6kro_buffer);
#endif
}

void keyboard_buffer_clear()
{
#ifdef NKRO_ENABLE
    keyboard_NKRObuffer_clear(&g_keyboard_nkro_buffer);
#else
    keyboard_6KRObuffer_clear(&g_keyboard_6kro_buffer);
#endif
}

int keyboard_6KRObuffer_add(Keyboard_6KROBuffer *buf, uint16_t key)
{
    buf->buffer[0] |= KEY_MODIFIER(key);
    if (KEY_KEYCODE(key) != KEY_NO_EVENT && buf->keynum < 6)
    {
        buf->buffer[2 + buf->keynum] = KEY_KEYCODE(key);
        buf->keynum++;
        return 0;
    }
    else
    {
        return 1;
    }
}

void keyboard_6KRObuffer_send(Keyboard_6KROBuffer* buf)
{
    keyboard_hid_send(buf->buffer, sizeof(buf->buffer));
}

void keyboard_6KRObuffer_clear(Keyboard_6KROBuffer* buf)
{
    memset(buf, 0, sizeof(Keyboard_6KROBuffer));
}

void keyboard_NKRObuffer_init(Keyboard_NKROBuffer*buf,uint8_t* data_buf,uint8_t length)
{
    buf->buffer = data_buf;
    buf->length = length;
}

int keyboard_NKRObuffer_add(Keyboard_NKROBuffer*buf,uint16_t key)
{
    uint8_t index = KEY_KEYCODE(key)/8+1;
    if (index<buf->length)
    {
        buf->buffer[index] |= (1 << (KEY_KEYCODE(key)%8));
    }
    buf->buffer[0] |= KEY_MODIFIER(key);
    return 0;
}

void keyboard_NKRObuffer_send(Keyboard_NKROBuffer*buf)
{
    keyboard_hid_send(buf->buffer, buf->length);
}

void keyboard_NKRObuffer_clear(Keyboard_NKROBuffer*buf)
{
    memset(buf->buffer, 0, buf->length);
}

void keyboard_init()
{
#ifdef NKRO_ENABLE
    static uint8_t buffer[64];
    keyboard_NKRObuffer_init(&g_keyboard_nkro_buffer, buffer, sizeof(buffer));
#endif
}

void keyboard_factory_reset()
{
    memcpy(g_keymap, g_default_keymap, sizeof(g_keymap));
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        g_keyboard_advanced_keys[i].mode = DEFAULT_ADVANCED_KEY_MODE;
        g_keyboard_advanced_keys[i].trigger_distance = DEFAULT_TRIGGER_DISTANCE;
        g_keyboard_advanced_keys[i].release_distance = DEFAULT_RELEASE_DISTANCE;
        g_keyboard_advanced_keys[i].schmitt_parameter = DEFAULT_SCHMITT_PARAMETER;
        g_keyboard_advanced_keys[i].calibration_mode = KEY_AUTO_CALIBRATION_NEGATIVE;
        g_keyboard_advanced_keys[i].activation_value = 0.5;
        // Keyboard_AdvancedKeys[i].lower_deadzone = 0.32;
        advanced_key_set_deadzone(g_keyboard_advanced_keys + i, DEFAULT_UPPER_DEADZONE, DEFAULT_LOWER_DEADZONE);
        // Keyboard_AdvancedKeys[i].phantom_lower_deadzone = 0.32;
        // Keyboard_AdvancedKeys[i].key.keycode = default_keymap[0][Keyboard_AdvancedKeys[i].key.id];
    }
    rgb_factory_reset();
    keyboard_save();
    //keyboard_system_reset();
}

__WEAK void keyboard_system_reset()
{
}

__WEAK void keyboard_scan()
{
}

void keyboard_recovery()
{
    // mount the filesystem
    int err = lfs_mount(&lfs_w25qxx, &lfs_cfg);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(&lfs_w25qxx, &lfs_cfg);
        lfs_mount(&lfs_w25qxx, &lfs_cfg);
    }
    lfs_file_open(&lfs_w25qxx, &lfs_file_w25qxx, "config1.dat", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&lfs_w25qxx, &lfs_file_w25qxx);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        //lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &(g_keyboard_advanced_keys[i].key.id),
        //              sizeof(g_keyboard_advanced_keys[i].key.id));
        lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, ((void *)(&g_keyboard_advanced_keys[i])) + sizeof(Key),
                      sizeof(AdvancedKey) - sizeof(Key));
    }
    lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, g_keymap, sizeof(g_keymap));
    lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_configs, sizeof(g_rgb_configs));
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs_w25qxx, &lfs_file_w25qxx);
    printf("recovery = %d", err);
    // release any resources we were using
    lfs_unmount(&lfs_w25qxx);
    // print the boot count
}

void keyboard_save()
{
    // mount the filesystem
    int err = lfs_mount(&lfs_w25qxx, &lfs_cfg);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(&lfs_w25qxx, &lfs_cfg);
        lfs_mount(&lfs_w25qxx, &lfs_cfg);
    }
    // read current count
    lfs_file_open(&lfs_w25qxx, &lfs_file_w25qxx, "config1.dat", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&lfs_w25qxx, &lfs_file_w25qxx);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        //lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &(g_keyboard_advanced_keys[i].key.id),
        //               sizeof(g_keyboard_advanced_keys[i].key.id));
        lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, ((void *)(&g_keyboard_advanced_keys[i])) + sizeof(Key),
                       sizeof(AdvancedKey) - sizeof(Key));
    }
    lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, g_keymap, sizeof(g_keymap));
    lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &g_rgb_configs, sizeof(g_rgb_configs));
    // remember the storage is not updated until the file is closed successfully
    err = lfs_file_close(&lfs_w25qxx, &lfs_file_w25qxx);
    printf("save = %d", err);
    // release any resources we were using
    lfs_unmount(&lfs_w25qxx);
    // print the boot count
}

void keyboard_send_report()
{
    static uint32_t mouse_value;
    keyboard_buffer_clear();
    mouse_buffer_clear(&g_mouse);
    
    for (int i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        keyboard_key_add_buffer(&g_keyboard_advanced_keys[i].key);
    }
    for (int i = 0; i < KEY_NUM; i++)
    {
        keyboard_key_add_buffer(&g_keyboard_keys[i]);
    }
    if (g_keyboard_send_report_enable)
    {
        keyboard_buffer_send();
        if ((*(uint32_t*)&g_mouse)!=mouse_value)
        {
            mouse_buffer_send(&g_mouse);
        }
    }
    mouse_value = *(uint32_t*)&g_mouse;
}

__WEAK void keyboard_task()
{
    keyboard_scan();
    analog_average();
    analog_check();
    keyboard_post_process();
    keyboard_send_report();
}

__WEAK void keyboard_hid_send(uint8_t *report, uint16_t len)
{
}
__WEAK void keyboard_delay(uint32_t ms)
{
}
__WEAK void keyboard_post_process()
{
}
