/*!
 * @file        usbd_custom_hid_if.h
 *
 * @brief       usb device Custom HID interface handler header file
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
#ifndef USBD_CUSTOM_HID_IF_H
#define USBD_CUSTOM_HID_IF_H

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "usbd_customhid.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/* Exported function prototypes *******************************************/
USBD_STA_T USBD_FS_CUSTOM_HID_ItfInit(void);
USBD_STA_T USBD_FS_CUSTOM_HID_ItfDeInit(void);
USBD_STA_T USBD_FS_CUSTOM_HID_ItfSendReport(uint8_t *buffer, uint8_t length);
USBD_STA_T USBD_FS_CUSTOM_HID_ItfReceive(uint8_t *buffer, uint8_t *length);

#ifdef __cplusplus
}
#endif

#endif /* USBD_CUSTOM_HID_IF_H */
