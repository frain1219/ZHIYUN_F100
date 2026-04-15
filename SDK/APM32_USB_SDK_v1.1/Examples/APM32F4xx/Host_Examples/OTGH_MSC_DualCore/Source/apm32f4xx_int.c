/*!
 * @file        apm32f4xx_int.c
 *
 * @brief       Main Interrupt Service Routines
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
#include "apm32f4xx_int.h"
#include "apm32f4xx_usb_host.h"
#include "bsp_delay.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup OTGH_MSC_DualCore
  @{
*/

/** @defgroup OTGH_MSC_DualCore_INT_Variables INT_Variables
  @{
  */

extern USBH_HANDLE_T usbHSHostHandler;
extern USBH_HANDLE_T usbFSHostHandler;

/**@} end of group OTGH_MSC_DualCore_INT_Variables */

/** @defgroup OTGH_MSC_DualCore_INT_Functions INT_Functions
  @{
*/

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /** Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /** Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /** Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}
/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /** Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles SVCall exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief   This function handles PendSV_Handler exception
 *
 * @param   None
 *
 * @retval  None
 *
 */

void PendSV_Handler(void)
{
}

/*!
 * @brief   This function handles SysTick Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SysTick_Handler(void)
{
    APM_DelayTickDec();
}

/*!
 * @brief   This function handles USB HS Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void OTG_HS1_IRQHandler(void)
{
    USBH_OTG_IsrHandler(&usbHSHostHandler);
}

/*!
 * @brief   This function handles USB HS Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void OTG_FS_IRQHandler(void)
{
    USBH_OTG_IsrHandler(&usbFSHostHandler);
}

/**@} end of group OTGH_MSC_DualCore_INT_Functions */
/**@} end of group OTGH_MSC_DualCore */
/**@} end of group Examples */
