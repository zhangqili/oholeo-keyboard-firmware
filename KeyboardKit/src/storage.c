/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include"storage.h"
#include"lfs.h"
#include"keyboard.h"
#include"rgb.h"

int storage_mount(void)
{
    // mount the filesystem
    int err = lfs_mount(&g_lfs, &g_lfs_config);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err)
    {
        lfs_format(&g_lfs, &g_lfs_config);
        lfs_mount(&g_lfs, &g_lfs_config);
    }
    return err;
}

void storage_unmount(void)
{
    lfs_unmount(&g_lfs);
}

uint8_t storage_read_config_index(void)
{
    lfs_file_t lfs_file;
    uint8_t index = 0;
    lfs_file_open(&g_lfs, &lfs_file, "config_index", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &lfs_file);
    lfs_file_read(&g_lfs, &lfs_file, &index, sizeof(index));
    lfs_file_close(&g_lfs, &lfs_file);
    if (index >= STORAGE_CONFIG_FILE_NUM)
    {
        index = 0;
    }
    return index;
}

void storage_save_config_index(uint8_t index)
{
    lfs_file_t lfs_file;
    lfs_file_open(&g_lfs, &lfs_file, "config_index", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &lfs_file);
    lfs_file_write(&g_lfs, &lfs_file, &index, sizeof(index));
    lfs_file_close(&g_lfs, &lfs_file);
}

typedef struct __AdvancedKeyBuffer
{
    Key key;
    float value;
    float raw;
    float maximum;
    float minimum;

#ifdef OPTIMIZE_FOR_FLOAT_DIVISION
    float range_reciprocal;
#endif

    uint8_t mode;
    uint8_t calibration_mode;

    float activation_value;
    float deactivation_value;

    float trigger_distance;
    float release_distance;
    float trigger_speed;
    float release_speed;
    float upper_deadzone;
    float lower_deadzone;
    float upper_bound;
    float lower_bound;
} AdvancedKeyBuffer;

void storage_read_config(uint8_t index)
{
    lfs_file_t lfs_file;
    char config_file_name[8] = "config0";
    config_file_name[6] = index + '0';

    lfs_file_open(&g_lfs, &lfs_file, config_file_name, LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &lfs_file);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
#ifndef ENABLE_FIXED_POINT_EXPERIMENTAL
        lfs_file_read(&g_lfs, &lfs_file, ((void *)(&g_keyboard_advanced_keys[i])) + offsetof(AdvancedKey, mode),
            sizeof(AdvancedKey) - offsetof(AdvancedKey, mode));
        advanced_key_set_range(&g_keyboard_advanced_keys[i], g_keyboard_advanced_keys[i].upper_bound, g_keyboard_advanced_keys[i].lower_bound);
#else
        AdvancedKeyBuffer buffer;
        lfs_file_read(&g_lfs, &lfs_file, ((void *)(&buffer)) + offsetof(AdvancedKeyBuffer, mode),
                      sizeof(AdvancedKeyBuffer) - offsetof(AdvancedKeyBuffer, mode));
        g_keyboard_advanced_keys[i].mode = buffer.mode;
        g_keyboard_advanced_keys[i].calibration_mode = buffer.calibration_mode;
        g_keyboard_advanced_keys[i].trigger_distance = buffer.trigger_distance * (ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        g_keyboard_advanced_keys[i].release_distance = buffer.release_distance * (ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        g_keyboard_advanced_keys[i].trigger_speed = buffer.trigger_speed * (ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        g_keyboard_advanced_keys[i].release_speed = buffer.release_speed * (ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        g_keyboard_advanced_keys[i].upper_deadzone = buffer.upper_deadzone * (ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        g_keyboard_advanced_keys[i].lower_deadzone = buffer.lower_deadzone * (ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        g_keyboard_advanced_keys[i].upper_bound = buffer.upper_bound;
        g_keyboard_advanced_keys[i].lower_bound = buffer.lower_bound;
        advanced_key_set_range(&g_keyboard_advanced_keys[i], g_keyboard_advanced_keys[i].upper_bound, g_keyboard_advanced_keys[i].lower_bound);
#endif
    }
    lfs_file_read(&g_lfs, &lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_read(&g_lfs, &lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_read(&g_lfs, &lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    lfs_file_read(&g_lfs, &lfs_file, g_keyboard_dynamic_keys, sizeof(g_keyboard_dynamic_keys));
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&g_lfs, &lfs_file);
}

void storage_save_config(uint8_t index)
{
    lfs_file_t lfs_file;
    char config_file_name[8] = "config0";
    config_file_name[6] = index + '0';

    lfs_file_open(&g_lfs, &lfs_file, config_file_name, LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &lfs_file);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
#ifndef ENABLE_FIXED_POINT_EXPERIMENTAL
        lfs_file_write(&g_lfs, &lfs_file, ((void *)(&g_keyboard_advanced_keys[i])) + offsetof(AdvancedKey, mode),
                      sizeof(AdvancedKey) - offsetof(AdvancedKey, mode));
#else
        AdvancedKeyBuffer buffer;
        buffer.mode = g_keyboard_advanced_keys[i].mode;
        buffer.calibration_mode = g_keyboard_advanced_keys[i].calibration_mode;
        buffer.trigger_distance = (float)g_keyboard_advanced_keys[i].trigger_distance / (float)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        buffer.release_distance = (float)g_keyboard_advanced_keys[i].release_distance / (float)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        buffer.trigger_speed = (float)g_keyboard_advanced_keys[i].trigger_speed / (float)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        buffer.release_speed = (float)g_keyboard_advanced_keys[i].release_speed / (float)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        buffer.upper_deadzone = (float)g_keyboard_advanced_keys[i].upper_deadzone / (float)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        buffer.lower_deadzone = (float)g_keyboard_advanced_keys[i].lower_deadzone / (float)(ANALOG_VALUE_MAX - ANALOG_VALUE_MIN);
        buffer.upper_bound = g_keyboard_advanced_keys[i].upper_bound;
        buffer.lower_bound = g_keyboard_advanced_keys[i].lower_bound;
        lfs_file_write(&g_lfs, &lfs_file, ((void *)(&buffer)) + offsetof(AdvancedKeyBuffer, mode),
                      sizeof(AdvancedKeyBuffer) - offsetof(AdvancedKeyBuffer, mode));
#endif
    }
    lfs_file_write(&g_lfs, &lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_write(&g_lfs, &lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_write(&g_lfs, &lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    lfs_file_write(&g_lfs, &lfs_file, g_keyboard_dynamic_keys, sizeof(g_keyboard_dynamic_keys));
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&g_lfs, &lfs_file);
}
