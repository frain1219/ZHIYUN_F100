/*!
 * @file        usb_host_user.c
 *
 * @brief       usb host user configuration
 *
 * @version     V1.0.0
 *
 * @date        2023-01-16
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
#include "usb_host_user.h"
#include "usbh_cdc.h"
#include "stdio.h"
#include <string.h>

/** @addtogroup Examples
  * @brief OTGH CDC examples
  @{
  */

/** @addtogroup OTGH_CDC
  @{
  */

/** @defgroup OTGH_CDC_Variables Variables
  @{
  */

USBH_INFO_T gUsbHostFS;

USBH_APP_STA_T gUsbHostAppStatus = USBH_APP_IDLE;

/**@} end of group OTGH_CDC_Variables*/

/** @defgroup OTGH_CDC_Functions Functions
  @{
  */

/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
__weak void USB_HostUserApplication(void)
{
    static USBH_APP_STA_T preAppStatus = USBH_APP_IDLE;

    uint8_t status = gUsbHostAppStatus;

    if (preAppStatus != gUsbHostAppStatus)
    {
        switch (status)
        {
            case USBH_APP_CONNECTION:
                USBH_USR_LOG("USBH_APP_CONNECTION");
                break;

            case USBH_APP_DISCONNECTION:
                USBH_USR_LOG("USBH_APP_DISCONNECTION");
                break;

            case USBH_APP_READY:
                USBH_USR_LOG("USBH_APP_READY");
                break;

            default:
                break;
        }

        preAppStatus = gUsbHostAppStatus;
    }
}

/*!
 * @brief       USB host user handler
 *
 * @param       usbInfo
 *
 * @param       userStatus
 *
 * @retval      None
 */
static void USB_HostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus)
{
    uint8_t classInterface;

    switch (userStatus)
    {
        case USBH_USER_CONNECTION:
            gUsbHostAppStatus = USBH_APP_CONNECTION;
            USBH_USR_Debug("USB device connected");
            break;

        case USBH_USER_DISCONNECTION:
            gUsbHostAppStatus = USBH_APP_DISCONNECTION;
            USBH_USR_LOG("USB device disconnected");
            break;

        case USBH_USER_DETECTED_SPEED:
            switch(usbInfo->devInfo.speed)
            {
                case USBH_PORT_SPEED_FS:
                    USBH_USR_LOG("USB device speed is FS");
                    break;
                
                case USBH_PORT_SPEED_LS:
                    USBH_USR_LOG("USB device speed is LS");
                    break;
                
                default:
                    USBH_USR_LOG("USB device speed is not support");
                    break;
            }
            break;
        
        case USBH_USER_ENUMERATION:
            USBH_USR_LOG("USB device enumration ok");

            classInterface = usbInfo->devInfo.desc.interface->interfaceDesc.bInterfaceClass;
            switch (classInterface)
            {
                case USBH_CLASS_MSC:
                    USBH_USR_LOG("This is a Mass Storage device");
                    break;

                case USBH_CLASS_HID:
                    USBH_USR_LOG("This is a HID device");
                    break;

                case USBH_CLASS_CDCD:
                    USBH_USR_LOG("This is a Commnunications data device");
                    break;

                default:
                    USBH_USR_LOG("Class code is: 0x%02X", classInterface);
                    break;
            }

            break;

        case USBH_USER_CLASS_LAUNCHED:
            gUsbHostAppStatus = USBH_APP_READY;
            USBH_USR_LOG("Class is ready");
            break;
        
        case USBH_USER_NOT_SUPPORT:
            USBH_USR_LOG("USB Host is not support this device");
            break;

        case USBH_USER_ERROR:
            USBH_USR_Debug("User error");
            break;

        default:
            break;
    }
}

/*!
 * @brief       USB host init
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostInit(void)
{
    /* USB host and class init */
    USBH_Init(&gUsbHostFS, USBH_SPEED_FS, &USBH_CDC_CLASS, USB_HostUserHandler);
}

/*!
 * @brief       USB host reset
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostReset(void)
{
    USBH_DeInit(&gUsbHostFS);
}

/*!
 * @brief       USB host process
 *
 * @param       None
 *
 * @retval      None
 */
void USB_HostProcess(void)
{
    USBH_PollingProcess(&gUsbHostFS);
}

/**@} end of group OTGH_CDC_Functions */
/**@} end of group OTGH_CDC */
/**@} end of group Examples */
