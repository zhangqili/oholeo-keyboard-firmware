#include "lfs.h"
#include "sfud.h"
#include "main.h"

extern sfud_flash sfud_norflash0;
/**
 * lfs与底层flash读数据接口
 * @param  c
 * @param  block  块编号
 * @param  off    块内偏移地址
 * @param  buffer 用于存储读取到的数据
 * @param  size   要读取的字节数
 * @return
 */
static int lfs_deskio_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    sfud_read(&sfud_norflash0, c->block_size * block + off, size, (uint8_t *)buffer);
    //w25qxx_advance_read(c->block_size * block + off, (uint8_t *)buffer ,size);
    return LFS_ERR_OK;
}

/**
 * lfs与底层flash写数据接口
 * @param  c
 * @param  block  块编号w25qxx_advance_sector_erase_4k
 * @param  off    块内偏移地址
 * @param  buffer 待写入的数据
 * @param  size   待写入数据的大小
 * @return
 */
static int lfs_deskio_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    sfud_write(&sfud_norflash0, c->block_size * block + off, size, (uint8_t *)buffer);
    //w25qxx_advance_write(c->block_size * block + off, (uint8_t *)buffer, size);
    return LFS_ERR_OK;
}

/**
 * lfs与底层flash擦除接口
 * @param  c
 * @param  block 块编号
 * @return
 */
static int lfs_deskio_erase(const struct lfs_config *c, lfs_block_t block)
{
    sfud_erase(&sfud_norflash0, (block * c->block_size), c->block_size);
    //w25qxx_advance_sector_erase_4k(block);
    return LFS_ERR_OK;
}

static int lfs_deskio_sync(const struct lfs_config *c)
{
    
    UNUSED(c);
    return LFS_ERR_OK;
}

////////////////////////////////////////////////////////
///
/// 静态内存使用方式必须设定这四个缓存
///
static uint8_t read_buffer[16];
static uint8_t prog_buffer[16];
static uint8_t lookahead_buffer[16];

// lfs句柄
lfs_t lfs_w25qxx;

lfs_file_t lfs_file_w25qxx;

const struct lfs_config lfs_cfg =
{
    // block device operations
    .read  = lfs_deskio_read,
    .prog  = lfs_deskio_prog,
    .erase = lfs_deskio_erase,
    .sync  = lfs_deskio_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = 4096,

    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,

    //
    // 使用静态内存必须设置这几个缓存
    //
    .read_buffer = read_buffer,
    .prog_buffer = prog_buffer,
    .lookahead_buffer = lookahead_buffer,
};

// entry point
void lfs_test(void)
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
    uint32_t boot_count = 0;


    lfs_file_open(&lfs_w25qxx, &lfs_file_w25qxx, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&lfs_w25qxx, &lfs_file_w25qxx, &boot_count, sizeof(boot_count));

    // update boot count
    boot_count += 1;
    lfs_file_rewind(&lfs_w25qxx, &lfs_file_w25qxx);
    lfs_file_write(&lfs_w25qxx, &lfs_file_w25qxx, &boot_count, sizeof(boot_count));

    // remember the storage is not updated until the file is closed successfully
    lfs_file_close(&lfs_w25qxx, &lfs_file_w25qxx);

    // release any resources we were using
    lfs_unmount(&lfs_w25qxx);

    // print the boot count
    printf("boot_count: %ld\n", boot_count);
}

