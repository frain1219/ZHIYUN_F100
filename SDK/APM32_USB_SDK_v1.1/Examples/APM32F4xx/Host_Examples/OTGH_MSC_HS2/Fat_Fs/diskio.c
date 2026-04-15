/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"         /* Obtains integer types */
#include "diskio.h"     /* Declarations of disk functions */
#include "usbh_msc.h"

#define USB_SECTOR_SIZE     512//4096//512
#define USB_BLOCK_SIZE      512//4096//512

extern USBH_INFO_T gUsbHostHS;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    BYTE lun = pdrv;

    DRESULT res = RES_ERROR;

    if (USBH_MSC_DevStatus(&gUsbHostHS, lun))
    {
        res = RES_OK;
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(
    BYTE pdrv               /* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE* buff,     /* Data buffer to store read data */
    LBA_t sector,   /* Start sector in LBA */
    UINT count      /* Number of sectors to read */
)
{
    DRESULT res = RES_PARERR;
    BYTE lun = pdrv;
    USBH_MSC_STORAGE_INFO_T device;

    if (USBH_MSC_DevRead(&gUsbHostHS, lun, sector, buff, count) == USBH_OK)
    {
        res = RES_OK;
    }
    else
    {
        USBH_MSC_ReadDevInfo(&gUsbHostHS, lun, &device);

        switch (device.sense.asc)
        {
            case USBH_SCSI_ASC_LOGICAL_UNIT_NOT_READY:
            case USBH_SCSI_ASC_MEDIUM_NOT_PRESENT:
            case USBH_SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                USBH_USR_LOG("USB disk is not ready");
                res = RES_NOTRDY;
                break;

            default:
                res = RES_ERROR;
                break;
        }
    }

    return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE* buff,   /* Data to be written */
    LBA_t sector,       /* Start sector in LBA */
    UINT count          /* Number of sectors to write */
)
{
    DRESULT res = RES_PARERR;
    BYTE lun = pdrv;
    USBH_MSC_STORAGE_INFO_T device;

    if (USBH_MSC_DevWrite(&gUsbHostHS, lun, sector, (BYTE*)buff, count) == USBH_OK)
    {
        res = RES_OK;
    }
    else
    {
        USBH_MSC_ReadDevInfo(&gUsbHostHS, lun, &device);

        switch (device.sense.asc)
        {
            case USBH_SCSI_ASC_LOGICAL_UNIT_NOT_READY:
            case USBH_SCSI_ASC_MEDIUM_NOT_PRESENT:
            case USBH_SCSI_ASC_NOT_READY_TO_READY_CHANGE:
                USBH_USR_LOG("USB disk is not ready");
                res = RES_NOTRDY;
                break;

            case USBH_SCSI_ASC_WRITE_PROTECTED:
                USBH_USR_LOG("USB disk is write protected");
                res = RES_WRPRT;
                break;

            default:
                res = RES_ERROR;
                break;
        }
    }
    return res;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void* buff      /* Buffer to send/receive control data */
)
{
    DRESULT res = RES_ERROR;;
    BYTE lun = pdrv;
    USBH_MSC_STORAGE_INFO_T device;

    switch (cmd)
    {
        case CTRL_SYNC:
            res = RES_OK;
            break;

        case GET_SECTOR_SIZE:
            if (USBH_MSC_ReadDevInfo(&gUsbHostHS, lun, &device) == USBH_OK)
            {
                *(DWORD*)buff = device.capacity.blockSize;
                res = RES_OK;
            }
            else
            {
                res = RES_ERROR;
            }
            break;

        case GET_BLOCK_SIZE:
            if (USBH_MSC_ReadDevInfo(&gUsbHostHS, lun, &device) == USBH_OK)
            {
                *(DWORD*)buff = device.capacity.blockSize;
                res = RES_OK;
            }
            else
            {
                res = RES_ERROR;
            }
            break;

        case GET_SECTOR_COUNT:
            if (USBH_MSC_ReadDevInfo(&gUsbHostHS, lun, &device) == USBH_OK)
            {
                *(DWORD*)buff = device.capacity.blockSize / USB_SECTOR_SIZE;
                res = RES_OK;
            }
            else
            {
                res = RES_ERROR;
            }
            break;

        default :
            res = RES_PARERR;
            break;
    }

    return res;
}
