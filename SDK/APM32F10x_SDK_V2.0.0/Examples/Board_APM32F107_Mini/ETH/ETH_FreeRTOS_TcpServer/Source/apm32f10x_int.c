/*!
 * @file       apm32f10x_int.c
 *
 * @brief      Main Interrupt Service Routines
 *
 * @version    V1.0.0
 *
 * @date       2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2022 Geehy Semiconductor
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
#include "main.h"
#include "apm32f10x_int.h"

/** @addtogroup Examples
  * @{
  */

/** @addtogroup ETH_FreeRTOS_TcpServer
  * @{
  */ 

/** @defgroup ETH_FreeRTOS_TcpServer_INT_Functions INT_Functions
  @{
*/

/*!
 * @brief     This function handles NMI exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief     This function handles Hard Fault exception
 *
 * @param     None
 *
 * @retval    None
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
 * @brief     This function handles Memory Manage exception
 *
 * @param     None
 *
 * @retval    None
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
 * @brief     This function handles Bus Fault exception
 *
 * @param     None
 *
 * @retval    None
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
 * @brief     This function handles Usage Fault exception
 *
 * @param     None
 *
 * @retval    None
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
 * @brief     This function handles Debug Monitor exception
 *
 * @param     None
 *
 * @retval    None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief     This function handles ETH Handler
 *
 * @param     None
 *
 * @retval    None
 *
 */
void ETH_IRQHandler(void)
{
    uint32_t ulReturn;
    
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    
    ulReturn = taskENTER_CRITICAL_FROM_ISR(); 
    
    /* check if any packet received */
    if (ETH_ReadDMAIntFlag(ETH_DMA_INT_RX) == SET)
    {
        /* Give the semaphore to wakeup eth receive task */
        xSemaphoreGiveFromISR(eth_rx_sem, &xHigherPriorityTaskWoken);
        ETH_ClearDMAIntFlag(ETH_DMA_INT_RX);
    }
    
    ETH_ClearDMAIntFlag(ETH_DMA_INT_NIS);
  
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

/**@} end of group ETH_FreeRTOS_TcpServer_INT_Functions */
/**@} end of group ETH_FreeRTOS_TcpServer */
/**@} end of group Examples */
