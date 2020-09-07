/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */
#include "board_wrapper.h"

/* Definitions of physical drive number for each drive */
#define EX_FLASH		0	/* SPI FLASH */

WORD fs_sector_size = 0;
WORD fs_sector_per_block = 0;
DWORD fs_flash_size = 0;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
    BYTE mid,did;
    hw_spi_flash_init();
    hw_spi_flash_read_manu_dev_id(&mid,&did);

    /* SPI flash size update */
    if(mid == WINBOND_MANUFACTURER_ID)
    {
        fs_flash_size = hw_spi_flash_get_size();
    }
    fs_sector_size = hw_spi_flash_get_sector_size();
    fs_sector_per_block = hw_spi_flash_get_block_size();
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,		/* Physical drive nmuber to identify the drive */
    BYTE *buff,		/* Data buffer to store read data */
    DWORD sector,	/* Start sector in LBA */
    UINT count		/* Number of sectors to read */
)
{
    if (!count)
    {
        return RES_PARERR;
    }

    for(; count>0; count--)
    {
        hw_spi_flash_read(buff,sector*fs_sector_size,fs_sector_size);
        sector++;
        buff += fs_sector_size;
    }
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
    BYTE pdrv,			/* Physical drive nmuber to identify the drive */
    const BYTE *buff,	/* Data to be written */
    DWORD sector,		/* Start sector in LBA */
    UINT count			/* Number of sectors to write */
)
{

    if (!count)
    {
        return RES_PARERR;
    }

    for(; count>0; count--)
    {
        hw_spi_flash_write((uint8_t *)buff,sector*fs_sector_size,fs_sector_size);
        sector++;
        buff += fs_sector_size;
    }

    return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE pdrv,		/* Physical drive nmuber (0..) */
    BYTE cmd,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;
    switch(cmd)
    {
    case CTRL_SYNC:
        res = RES_OK;
        break;
    case GET_SECTOR_COUNT:
        *(DWORD*)buff = fs_flash_size/fs_sector_size;
        res = RES_OK;
        break;
    case GET_SECTOR_SIZE:
        *(WORD*)buff = fs_sector_size;
        res = RES_OK;
        break;
    case GET_BLOCK_SIZE:
        *(WORD*)buff = fs_sector_per_block;
        res = RES_OK;
        break;

    default:
        res = RES_PARERR;
        break;
    }
    return res;
}

DWORD get_fattime (void)
{
    return 0;
}

