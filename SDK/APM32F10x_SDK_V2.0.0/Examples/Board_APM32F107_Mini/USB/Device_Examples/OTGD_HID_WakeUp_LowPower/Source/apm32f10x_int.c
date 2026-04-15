/*!
 * @file        apm32f10x_int.c
 *
 * @brief       Main Interrupt Service Routines
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
#include "main.h"
#include "usbd_board.h"
#include "apm32f10x_int.h"

/* Private includes *******************************************************/
#include "apm32f10x_usb_device.h"
#include "board_delay.h"

/* Private macro **********************************************************/

/* Private typedef ********************************************************/

/* Private variables ******************************************************/

/* Private function prototypes ********************************************/

/* External variables *****************************************************/
extern USBD_HANDLE_T usbDeviceHandler;
__IO extern uint8_t WakeupEnable;
/* External functions *****************************************************/

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
    /* Go to infinite loop when Hard Fault exception occurs */
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
    /* Go to infinite loop when Memory Manage exception occurs */
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
    /* Go to infinite loop when Bus Fault exception occurs */
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
    /* Go to infinite loop when Usage Fault exception occurs */
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
    BOARD_DelayIRQHandler();
}

/*!
 * @brief   This function handles EINT1 Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void EINT1_IRQHandler(void)
{
    if(EINT_ReadIntFlag(EINT_LINE_1))
    {
        WakeupEnable = 1;
        if((&usbDeviceHandler)->usbCfg.lowPowerStatus == ENABLE)
        {
            /* Reset SLEEPDEEP bit and SLEEPONEXIT SCR */
            SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
            SystemInit();
            SystemClockConfig();
            /* Start PHY clock */
            (&usbDeviceHandler)->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;
        }

        /*Clear EINT_LINE0 interrupt flag*/
        EINT_ClearIntFlag(EINT_LINE_1);
    }
}

/*!
 * @brief   This function handles USB FS wakeup Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void OTG_FS_WKUP_IRQHandler(void)
{
    if((&usbDeviceHandler)->usbCfg.lowPowerStatus == ENABLE)
    {
        /* Reset SLEEPDEEP bit and SLEEPONEXIT SCR */
        SCB->SCR &= (uint32_t)~((uint32_t)(SCB_SCR_SLEEPDEEP_Msk | SCB_SCR_SLEEPONEXIT_Msk));
        SystemInit();
        SystemClockConfig();
    }

    /* Start PHY clock */
    (&usbDeviceHandler)->usbPower->PCGCTRL_B.PCLKSTOP = BIT_RESET;

    USBD_OTG_FS_WAKEUP_EINT_CLEAR_FLAG();
}

/*!
 * @brief   This function handles USB FS or HS Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void OTG_FS_IRQHandler(void)
{
    USBD_OTG_IsrHandler(&usbDeviceHandler);
}
