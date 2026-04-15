/*!
 * @file        usb_device_user.c
 *
 * @brief       usb device user configuration
 *
 * @version     V1.0.0
 *
 * @date        2023-12-27
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

/* Includes */
#include "usb_device_user.h"
#include "usbd_descriptor.h"
#include "usbd_hid.h"
#include <stdio.h>

/** @addtogroup Examples
  * @brief OTGD HID HS2 examples
  @{
  */

/** @addtogroup OTGD_HID_HS2_LowPower
  @{
  */

/** @defgroup OTGD_HID_HS2_LowPower_Variables Variables
  @{
  */

USBD_INFO_T gUsbDeviceHS;

USBD_APP_STA_T gUsbDevAppStatus = USBD_APP_IDLE;

/**@} end of group OTGD_HID_HS2_LowPower_Variables*/

/** @defgroup OTGD_HID_HS2_LowPower_Functions Functions
  @{
  */

/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
__weak void USB_DevUserApplication(void)
{
    static USBD_APP_STA_T preAppStatus = USBD_APP_IDLE;

    uint8_t status = gUsbDevAppStatus;

    if (preAppStatus != gUsbDevAppStatus)
    {
        switch (status)
        {
            case USBD_APP_SUSPEND:
                USBD_USR_LOG("USBD_APP_SUSPEND");
                break;

            case USBD_APP_READY:
                USBD_USR_LOG("USBD_APP_READY");
                break;
        }

        preAppStatus = gUsbDevAppStatus;
    }
}

/*!
 * @brief       USB device user handler
 *
 * @param       usbInfo
 *
 * @param       userStatus
 *
 * @retval      None
 */
static void USB_DevUserHandler(USBD_INFO_T* usbInfo, uint8_t userStatus)
{
    switch (userStatus)
    {
        case USBD_USER_RESET:
            break;

        case USBD_USER_RESUME:
            break;

        case USBD_USER_SUSPEND:
            gUsbDevAppStatus = USBD_APP_SUSPEND;
            break;

        case USBD_USER_CONNECT:
            break;

        case USBD_USER_DISCONNECT:
            break;

        case USBD_USER_ERROR:
            USBD_USR_Debug("User error");
            break;

        default:
            break;
    }
}

/*!
 * @brief       USB device init
 *
 * @param       None
 *
 * @retval      None
 */
void USB_DeviceInit(void)
{
    /* USB device and class init */
    USBD_Init(&gUsbDeviceHS, USBD_SPEED_HS, &USBD_DESC_HS, &USBD_HID_CLASS, USB_DevUserHandler);
}

/*!
 * @brief       USB device reset
 *
 * @param       None
 *
 * @retval      None
 */
void USB_DeviceReset(void)
{
    USBD_DeInit(&gUsbDeviceHS);
}

/**@} end of group OTGD_HID_HS2_LowPower_Functions */
/**@} end of group OTGD_HID_HS2_LowPower */
/**@} end of group Examples */
