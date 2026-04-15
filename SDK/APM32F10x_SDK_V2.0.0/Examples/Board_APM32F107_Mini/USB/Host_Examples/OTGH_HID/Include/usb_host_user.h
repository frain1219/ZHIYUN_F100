/*!
 * @file        usb_host_user.h
 *
 * @brief       usb host user function
 *
 * @version     V1.0.0
 *
 * @date        2026-03-01
 *
 * @attention
 *
 *  Copyright (C) 2023-2026 Geehy Semiconductor
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
#ifndef _USB_HOST_USER_H_
#define _USB_HOST_USER_H_

#ifdef __cplusplus
  extern "C" {
#endif

/* Includes ***************************************************************/
#include "apm32f10x.h"
#include "usbh_core.h"

/* Exported macro *********************************************************/

/* Exported typedef *******************************************************/

/**
 * @brief    USB host application status
 */
typedef enum
{
    USBH_APP_IDLE,
    USBH_APP_DISCONNECTION,
    USBH_APP_CONNECTION,
    USBH_APP_READY,
    USBH_APP_ERROR,
} USBH_APP_STA_T;

/**
 * @brief    USB host information
 */
typedef struct
{
    USBH_APP_STA_T  state;      /* USB host application status */
    USBH_APP_STA_T  prevState;  /* USB host application previous status */
    uint32_t        timer;      /* USB host timer */
} USB_HOST_INFO_T;

/* Exported function prototypes *******************************************/
void USB_HostInitalize(void);
void USB_HostReset(void);
void USB_HostProcess(void);
void USB_HostUserApplication(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _USB_HOST_USER_H_ */
