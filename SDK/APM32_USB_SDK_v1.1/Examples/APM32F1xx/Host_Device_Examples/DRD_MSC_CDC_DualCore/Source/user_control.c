/*!
 * @file        user_control.c
 *
 * @brief       Control USB switch role file
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

/* Includes */
#include "board.h"
#include "user_control.h"
#include "usb_device_user.h"
#include "usb_host_user.h"

/** @addtogroup Examples
  * @brief OTGH DRD examples
  @{
  */

/** @addtogroup DRD_MSC_CDC_DualCore
  @{
  */

/** @defgroup DRD_MSC_CDC_DualCore_Variables Variables
  @{
  */

USB_STA_T usbstatus = USB_HOST;

extern USBH_APP_STA_T gUsbHostAppStatus;

/**@} end of group DRD_MSC_CDC_DualCore_Variables*/

/** @defgroup DRD_MSC_CDC_DualCore_Functions Functions
  @{
  */

/*!
 * @brief       USB Select
 *
 * @param       None
 *
 * @retval      None
 */
static void USB_Select(void)
{
    USB_STA_T pstatus = usbstatus;

    /* Press KEY3 to switch USB role */
    if (!APM_MINI_PBGetState(BUTTON_KEY2))
    {
        APM_DelayMs(10);
        if (!APM_MINI_PBGetState(BUTTON_KEY2))
        {
            /* Close HS IRQ */
            NVIC_DisableIRQRequest(OTG_FS_IRQn);

            usbstatus = (USB_STA_T)((usbstatus + 1) % 2);
            APM_MINI_LEDToggle(LED2);
            APM_MINI_LEDToggle(LED3);
            while (!APM_MINI_PBGetState(BUTTON_KEY2));
        }
    }

    if(usbstatus != pstatus)
    {
        switch(usbstatus)
        {
            case USB_HOST:
                printf("Switch to HOST.\r\n");
                USB_DeviceReset();
                /* Clear USB HS Pending */
                __NVIC_ClearPendingIRQ(OTG_FS_IRQn);

                USB_HostInit();
                break;
            case USB_DEVICE:
                printf("Switch to DEVICE.\r\n");
                USB_HostReset();
                /* Set USB Host state */
                gUsbHostAppStatus = USBH_APP_DISCONNECTION;

                /* Clear USB HS Pending */
                __NVIC_ClearPendingIRQ(OTG_FS_IRQn);

                USB_DeviceInit();
                break;
            default:
                break;
        }
        NVIC_EnableIRQRequest(OTG_FS_IRQn, 1, 0);
    }
}

/*!
 * @brief       USB Process
 *
 * @param       None
 *
 * @retval      None
 */
void USB_Process(void)
{
    if(usbstatus == USB_HOST)
    {
        USB_HostProcess();
    }
    USB_Select();
}

/**@} end of group DRD_MSC_CDC_DualCore_Functions */
/**@} end of group DRD_MSC_CDC_DualCore */
/**@} end of group Examples */
