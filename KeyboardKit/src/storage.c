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

void storage_read_config(uint8_t index)
{
    lfs_file_t lfs_file;
    char config_file_name[8] = "config0";
    config_file_name[6] = index + '0';

    lfs_file_open(&g_lfs, &lfs_file, config_file_name, LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_rewind(&g_lfs, &lfs_file);
    for (uint8_t i = 0; i < ADVANCED_KEY_NUM; i++)
    {
        lfs_file_read(&g_lfs, &lfs_file, ((void *)(&g_keyboard_advanced_keys[i])) + offsetof(AdvancedKey, mode),
                      sizeof(AdvancedKey) - offsetof(AdvancedKey, mode));
    }
    lfs_file_read(&g_lfs, &lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_read(&g_lfs, &lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_read(&g_lfs, &lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
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
        lfs_file_write(&g_lfs, &lfs_file, ((void *)(&g_keyboard_advanced_keys[i])) + sizeof(Key) + 4*sizeof(AnalogValue),
                       sizeof(AdvancedKey) - sizeof(Key) - 4*sizeof(AnalogValue));
    }
    lfs_file_write(&g_lfs, &lfs_file, g_keymap, sizeof(g_keymap));
    lfs_file_write(&g_lfs, &lfs_file, &g_rgb_switch, sizeof(g_rgb_switch));
    lfs_file_write(&g_lfs, &lfs_file, &g_rgb_configs, sizeof(g_rgb_configs));
    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&g_lfs, &lfs_file);
}
