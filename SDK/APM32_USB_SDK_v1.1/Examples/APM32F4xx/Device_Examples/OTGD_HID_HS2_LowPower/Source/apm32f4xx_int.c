/*!
 * @file        apm32f4xx_int.c
 *
 * @brief       Main Interrupt Service Routines
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
#include "apm32f4xx_int.h"
#include "apm32f4xx_usb_device.h"
#include "bsp_delay.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup OTGD_HID_HS2_LowPower
  @{
*/

/** @defgroup OTGD_HID_HS2_LowPower_INT_Variables INT_Variables
  @{
  */

extern USBD_HANDLE_T usbDeviceHandler;

/**@} end of group OTGD_HID_HS2_LowPower_INT_Variables */

/** @defgroup OTGD_HID_HS2_LowPower_INT_Functions INT_Functions
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
    USBD_OTG_IsrHandler(&usbDeviceHandler);
}

/*!
 * @brief   This function handles EINT15_10 Handler
 *
 * @param   None
 *
 * @retval  None
 *
 * @note    For USB HS2 to wake-up from low power mode
 *
 */
void EINT15_10_IRQHandler(void)
{
    if(EINT_ReadIntFlag(EINT_LINE_15))
    {
        if((&usbDeviceHandler)->usbCfg.lowPowerStatus == ENABLE)
        {
            /* Reset SLEEPDEEP bit and SLEEPONEXIT SCR */
            SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
            SystemInit();
            /* Start PHY clock */
            (&usbDeviceHandler)->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;
            /* Unmount the usb bus to an external interrupt */
            EINT_Config_T     EINT_configStruct;
            EINT_configStruct.line = EINT_LINE_15;
            EINT_configStruct.mode = EINT_MODE_INTERRUPT;
            EINT_configStruct.trigger = EINT_TRIGGER_FALLING;
            EINT_configStruct.lineCmd = DISABLE;
            EINT_Config(&EINT_configStruct);
            NVIC_DisableIRQRequest(EINT15_10_IRQn);
        }
        /*Clear EINT_LINE0 interrupt flag*/
        EINT_ClearIntFlag(EINT_LINE_15);
    }
}
/**@} end of group OTGD_HID_HS2_LowPower_INT_Functions */
/**@} end of group OTGD_HID_HS2_LowPower */
/**@} end of group Examples */
