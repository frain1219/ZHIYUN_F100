/*!
 * @file        user_control.h
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


/** @addtogroup Examples
  * @brief OTGH DRD examples
  @{
  */

/** @addtogroup DRD_MSC_CDC_DualCore
  @{
  */

/** @defgroup DRD_MSC_CDC_DualCore_Enumerates Enumerates
  @{
  */

typedef enum
{
    USB_HOST,
    USB_DEVICE,
} USB_STA_T;

/**@} end of group DRD_MSC_CDC_DualCore_Enumerates*/

/** @defgroup DRD_MSC_CDC_DualCore_Variables Variables
  @{
  */

extern USB_STA_T usbstatus;

/**@} end of group DRD_MSC_CDC_DualCore_Variables*/

/** @defgroup DRD_MSC_CDC_DualCore_Functions Functions
  @{
  */

void USB_Process(void);

/**@} end of group DRD_MSC_CDC_DualCore_Functions */
/**@} end of group DRD_MSC_CDC_DualCore */
/**@} end of group Examples */
