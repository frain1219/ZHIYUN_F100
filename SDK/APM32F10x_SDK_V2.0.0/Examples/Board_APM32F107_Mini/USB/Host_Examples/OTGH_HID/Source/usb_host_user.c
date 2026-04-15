/*!
 * @file        usb_host_user.c
 *
 * @brief       usb host user configuration
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

/* Includes ***************************************************************/
#include "usb_host_user.h"

/* Private includes *******************************************************/
#include "usbh_hid.h"
#include "usbh_hid_keyboard.h"
#include "usbh_hid_mouse.h"
#include "board.h"
#include "board_delay.h"
#include "stdio.h"
#include <string.h>

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/
USBH_INFO_T gUsbHostFS;

USB_HOST_INFO_T hostInfo;

/* Private function prototypes ********************************************/
static void USB_HostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus);

/* External variables *****************************************************/

/* External functions *****************************************************/

/**
 * @brief   USB host init
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostInitalize(void)
{
    /* USB host and class init */
    USBH_Init(&gUsbHostFS, USBH_SPEED_FS, &USBH_HID_CLASS, USB_HostUserHandler);

    USBH_Start(&gUsbHostFS);
}

/*!
 * @brief   USB host reset
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostReset(void)
{
    USBH_DeInit(&gUsbHostFS);
}

/*!
 * @brief   USB host process
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostProcess(void)
{
    USBH_PollingProcess(&gUsbHostFS);
}

/**
 * @brief   User application
 *
 * @param   None
 *
 * @retval  None
 */
void USB_HostUserApplication(void)
{
    uint32_t timeout;

    switch (hostInfo.state)
    {
        case USBH_APP_CONNECTION:
            /* To avoid the enumeration error */
            if (((gUsbHostFS.xferState == USBH_XFER_WAITING) && \
                ((gUsbHostFS.hostEnumState == USBH_ENUM_IDLE) || \
                 (gUsbHostFS.hostEnumState == USBH_ENUM_GET_DEV_DESC))) ||
                 ((gUsbHostFS.hostState == USBH_HOST_ABORT) && (gUsbHostFS.devInfo.rstCnt > USBH_DEVICE_RESET_MAX_CNT)))
                 {
                     timeout = BOARD_ReadTick();

                    if (timeout - hostInfo.timer > 1000)
                    {
                        USBH_USR_DBG("USB host enumeration timeout");
                        hostInfo.state = USBH_APP_ERROR;
                    }
                 }
            break;

        case USBH_APP_DISCONNECTION:
            break;

        case USBH_APP_READY:
            break;

        case USBH_APP_ERROR:
            USBH_USR_ERR("USB host error, retry to enumerate the device");
            hostInfo.state = USBH_APP_IDLE;

            USB_HostReset();

            USB_HostInitalize();
            break;

        default:
            break;
    }
}

/*!
 * @brief       USB host HID poll callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
void USBH_HID_PollCallback(USBH_INFO_T* usbInfo)
{
    USBH_HID_KEYBOARD_INFO_T* hidkbInfo;
    USBH_HID_MOUSE_INFO_T* hidmouseInfo;
    uint8_t c;

    if (hostInfo.state == USBH_APP_READY)
    {
        if (USBH_HID_ReadDevType(usbInfo) == HID_KEYBOARD)
        {
            hidkbInfo = USBH_HID_KeyboardCallback(usbInfo);
            if (hidkbInfo != NULL)
            {
                c = USBH_HID_KeyboardValueToAsciiCode(hidkbInfo);
                if (c != 0)
                {
                    printf("%c", c);
                    fflush(stdout);
                }
            }
        }
        else if (USBH_HID_ReadDevType(usbInfo) == HID_MOUSE)
        {
            hidmouseInfo = USBH_HID_MouseCallback(usbInfo);
            if (hidmouseInfo != NULL)
            {
                USBH_USR_LOG("Mouse x:%d, y:%d, z:%d, button1:%d, button2:%d, button3:%d",
                             hidmouseInfo->x, hidmouseInfo->y, hidmouseInfo->z,
                             hidmouseInfo->button[0], hidmouseInfo->button[1], hidmouseInfo->button[2]);
            }
        }
    }
}

/**
 * @brief   USB host user handler
 *
 * @param   usbInfo: USB core information
 *
 * @param   userStatus: user status
 *
 * @retval  None
 */
static void USB_HostUserHandler(USBH_INFO_T* usbInfo, uint8_t userStatus)
{
    switch (userStatus)
    {
        case USBH_USER_CONNECTION:
            hostInfo.state = USBH_APP_CONNECTION;
            hostInfo.timer = BOARD_ReadTick();
            break;

        case USBH_USER_DISCONNECTION:
            hostInfo.state = USBH_APP_DISCONNECTION;
            break;

        case USBH_USER_DETECTED_SPEED:
            switch(usbInfo->devInfo.speed)
            {
                case USBH_PORT_SPEED_HS:
                    USBH_USR_LOG("USB device speed is HS");
                    break;

                case USBH_PORT_SPEED_FS:
                    USBH_USR_LOG("USB device speed is FS");
                    break;

                case USBH_PORT_SPEED_LS:
                    USBH_USR_LOG("USB device speed is LS");
                    if(usbInfo->hostSpeed == USBH_SPEED_HS)
                    {
                        USBH_USR_LOG("HS2:High speed host is not support for LS device.");
                        usbInfo->userCallback(usbInfo,USBH_USER_NOT_SUPPORT);
                    }
                    break;

                default:
                    USBH_USR_LOG("USB device speed is not support");
                    break;
            }
            break;

        case USBH_USER_ENUMERATION:
            USBH_USR_LOG("USB device enumration ok");
            break;

        case USBH_USER_CLASS_LAUNCHED:
            hostInfo.state = USBH_APP_READY;
            USBH_USR_LOG("Class is ready");
            break;

        case USBH_USER_NOT_SUPPORT:
            USBH_USR_LOG("USB device is Unsupported Device");
            break;

        case USBH_USER_ERROR:
            USBH_USR_DBG("User error");
            break;

        default:
            break;
    }
}
