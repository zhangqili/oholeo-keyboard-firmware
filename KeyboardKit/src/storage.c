/*
 * Copyright (c) 2024 Zhangqi Li (@zhangqili)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include"storage.h"
#include"lfs.h"
#include"keyboard.h"
#include"rgb.h"

void advanced_key_config_normalize(AdvancedKeyConfigurationNormalized* buffer, AdvancedKeyConfiguration* config)
{
    buffer->mode = config->mode;
    buffer->calibration_mode = config->calibration_mode;
    buffer->activation_value = config->activation_value / (float)ANALOG_VALUE_RANGE;
    buffer->deactivation_value = config->deactivation_value / (float)ANALOG_VALUE_RANGE;
    buffer->trigger_distance = config->trigger_distance / (float)ANALOG_VALUE_RANGE;
    buffer->release_distance = config->release_distance / (float)ANALOG_VALUE_RANGE;
    buffer->trigger_speed = config->trigger_speed / (float)ANALOG_VALUE_RANGE;
    buffer->release_speed = config->release_speed / (float)ANALOG_VALUE_RANGE;
    buffer->upper_deadzone = config->upper_deadzone / (float)ANALOG_VALUE_RANGE;
    buffer->lower_deadzone = config->lower_deadzone / (float)ANALOG_VALUE_RANGE;
    buffer->upper_bound = config->upper_bound;
    buffer->lower_bound = config->lower_bound;
}

void advanced_key_config_anti_normalize(AdvancedKeyConfiguration* config, AdvancedKeyConfigurationNormalized* buffer)
{
    config->mode = buffer->mode;
    config->calibration_mode = buffer->calibration_mode;
    config->activation_value = buffer->activation_value * ANALOG_VALUE_RANGE;
    config->deactivation_value = buffer->deactivation_value * ANALOG_VALUE_RANGE;
    config->trigger_distance = buffer->trigger_distance * ANALOG_VALUE_RANGE;
    config->release_distance = buffer->release_distance * ANALOG_VALUE_RANGE;
    config->trigger_speed = buffer->trigger_speed * ANALOG_VALUE_RANGE;
    config->release_speed = buffer->release_speed * ANALOG_VALUE_RANGE;
    config->upper_deadzone = buffer->upper_deadzone * ANALOG_VALUE_RANGE;
    config->lower_deadzone = buffer->lower_deadzone * ANALOG_VALUE_RANGE;
    config->upper_bound = buffer->upper_bound;
    config->lower_bound = buffer->lower_bound;
}

void dynamic_key_stroke_normalize(DynamicKeyStroke4x4Normalized* buffer, DynamicKeyStroke4x4* dks)
{
    memcpy(buffer, dks, offsetof(DynamicKeyStroke4x4,press_begin_distance));
    buffer->press_begin_distance = dks->press_begin_distance / (float)ANALOG_VALUE_RANGE;
    buffer->press_fully_distance = dks->press_fully_distance / (float)ANALOG_VALUE_RANGE;
    buffer->release_begin_distance = dks->release_begin_distance / (float)ANALOG_VALUE_RANGE;
    buffer->release_fully_distance = dks->release_fully_distance / (float)ANALOG_VALUE_RANGE;
    //memcpy(buffer->key_end_time, dks->key_end_time, sizeof(DynamicKeyStroke4x4) - offsetof(DynamicKeyStroke4x4,key_end_time));
}

void dynamic_key_stroke_anti_normalize(DynamicKeyStroke4x4* dks, DynamicKeyStroke4x4Normalized* buffer)
{
    memcpy(dks, buffer, offsetof(DynamicKeyStroke4x4,press_begin_distance));
    dks->press_begin_distance = buffer->press_begin_distance * ANALOG_VALUE_RANGE;
    dks->press_fully_distance = buffer->press_fully_distance * ANALOG_VALUE_RANGE;
    dks->release_begin_distance = buffer->release_begin_distance * ANALOG_VALUE_RANGE;
    dks->release_fully_distance = buffer->release_fully_distance * ANALOG_VALUE_RANGE;
    //memcpy(dks->key_end_time, buffer->key_end_time, sizeof(DynamicKeyStroke4x4) - offsetof(DynamicKeyStroke4x4,key_end_time));
}

static inline void save_advanced_key_config(lfs_t *lfs, lfs_file_t *file, AdvancedKey* key)
{
    AdvancedKeyConfigurationNormalized buffer;
    advanced_key_config_normalize(&buffer, &key->config);
    lfs_file_write(lfs, file, ((void *)(&buffer)),
        sizeof(AdvancedKeyConfigurationNormalized));
}

static inline void read_advanced_key_config(lfs_t *lfs, lfs_file_t *file, AdvancedKey* key)
{
    AdvancedKeyConfigurationNormalized buffer;
    lfs_file_read(lfs, file, &buffer,
        sizeof(AdvancedKeyConfigurationNormalized));
    advanced_key_config_anti_normalize(&key->config, &buffer);
    advanced_key_set_range(key, key->config.upper_bound, key->config.lower_bound);\
}

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

void storage_read_config(uint8_t index)
{
    lfs_file_t lfs_file;
    char config_file_name[8] = "config0";
    config_file_name[6] = index + '0';

    lfs_file_open(&g_lfs, &lfs_file, config_file_name, LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &lfs_file);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        read_advanced_key_config(&g_lfs, &lfs_file, &g_keyboard_advanced_keys[i]);
    }
    lfs_file_read(&g_lfs, &lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_read(&g_lfs, &lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_read(&g_lfs, &lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    lfs_file_read(&g_lfs, &lfs_file, g_keyboard_dynamic_keys, sizeof(g_keyboard_dynamic_keys));
    for (uint8_t i = 0; i < DYNAMIC_KEY_NUM; i++)
    {
        DynamicKey buffer;
        lfs_file_read(&g_lfs, &lfs_file, &buffer, sizeof(DynamicKey));
        switch (buffer.type)
        {
        case DYNAMIC_KEY_STROKE:
            dynamic_key_stroke_anti_normalize((DynamicKeyStroke4x4*)&g_keyboard_dynamic_keys[i], (DynamicKeyStroke4x4Normalized*)&buffer);
            break;
        default:
            memcpy(&g_keyboard_dynamic_keys[i], &buffer, sizeof(DynamicKey));
            break;
        }
    }
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
        save_advanced_key_config(&g_lfs, &lfs_file, &g_keyboard_advanced_keys[i]);
    }
    lfs_file_write(&g_lfs, &lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_write(&g_lfs, &lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_write(&g_lfs, &lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    for (uint8_t i = 0; i < DYNAMIC_KEY_NUM; i++)
    {
        switch (g_keyboard_dynamic_keys[i].type)
        {
        case DYNAMIC_KEY_STROKE:
            DynamicKeyStroke4x4Normalized buffer;
            dynamic_key_stroke_normalize(&buffer, (DynamicKeyStroke4x4*)&g_keyboard_dynamic_keys[i]);
            lfs_file_write(&g_lfs, &lfs_file, &buffer, sizeof(DynamicKey));
            break;
        default:
            lfs_file_write(&g_lfs, &lfs_file, &g_keyboard_dynamic_keys[i], sizeof(DynamicKey));
            break;
        }
    }
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&g_lfs, &lfs_file);
}
