/*!
 * @file        usb_host_user.h
 *
 * @brief       usb host user function
 *
 * @version     V1.0.0
 *
 * @date        2023-11-13
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
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

/* Includes */
#include "apm32f4xx.h"
#include "usbh_core.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup OTGH_MSC_DualCore
  @{
  */

/** @defgroup OTGH_MSC_DualCore_Enumerates Enumerates
  @{
  */

/**
 * @brief    USB host application status
 */
typedef enum
{
    USBH_APP_IDLE,
    USBH_APP_DISCONNECTION,
    USBH_APP_CONNECTION,
    USBH_APP_READY,
} USBH_APP_STA_T;

/**@} end of group OTGH_MSC_DualCore_Enumerates*/

/** @defgroup OTGH_MSC_DualCore_Variables Variables
  @{
  */

extern USBH_APP_STA_T gUsbHSHostAppStatus;
extern USBH_APP_STA_T gUsbFSHostAppStatus;
extern USBH_INFO_T gUsbHostHS;
extern USBH_INFO_T gUsbHostFS;

/**@} end of group OTGH_MSC_DualCore_Variables*/

/** @defgroup OTGH_MSC_DualCore_Functions Functions
  @{
  */

void USB_HostInit(void);
void USB_HostReset(void);
void USB_HostProcess(void);
void USB_HostUserApplication(void);

/**@} end of group OTGH_MSC_DualCore_Functions */
/**@} end of group OTGH_MSC_DualCore */
/**@} end of group Examples */

#endif
