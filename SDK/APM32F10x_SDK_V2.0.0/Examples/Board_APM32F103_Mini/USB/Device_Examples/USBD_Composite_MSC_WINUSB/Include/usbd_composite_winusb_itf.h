/*!
 * @file        usbd_composite_winusb_itf.h
 *
 * @brief       usb device WINUSB class interface handler header file
 *
 * @version     V1.0.0
 *
 * @date        2026-03-01
 *
 * @attention
 *
 *  Copyright (C) 2025-2026 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Define to prevent recursive inclusion */
#ifndef USBD_COMPOSITE_WINUSB_ITF_H
#define USBD_COMPOSITE_WINUSB_ITF_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "usbd_composite.h"

/* Exported macro *********************************************************/

#define USBD_WINUSB_TX_BUF_LEN         1024
#define USBD_WINUSB_RX_BUF_LEN         1024

/* Exported typedef *******************************************************/

/**
 * @brief   USB device WINUSB ITF status
 */
typedef enum
{
    USBD_WINUSB_ITF_IDLE,
    USBD_WINUSB_ITF_REV_UPDATE,
} USBD_WINUSB_ITF_STA_T;

/**
 * @brief   USB device WINUSB ITF information
 */
typedef struct
{
    uint32_t    rxUpdateLen;
    uint8_t     state;
} USBD_WINUSB_ITF_INFO_T;

/* Exported function prototypes *******************************************/
USBD_STA_T USBD_FS_WINUSB_ItfSend(uint8_t *buffer, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* USBD_COMPOSITE_WINUSB_ITF_H */
