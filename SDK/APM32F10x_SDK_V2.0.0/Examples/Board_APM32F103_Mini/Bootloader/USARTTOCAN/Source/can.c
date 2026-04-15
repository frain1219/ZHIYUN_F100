/*!
 * @file        can.c
 *
 * @brief       CAN Driver
 *
 * @version     V1.0.0
 *
 * @date        2025-04-23
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
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
#include "can.h"
#include "uart.h"
#include "config.h"
#include <string.h>

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOCAN
  @{
*/

/** @defgroup USARTTOCAN_Macros Macros
  @{
*/

#define LED2_OFF GPIOE->ODATA |= 0x00000040;
#define LED3_OFF GPIOE->ODATA |= 0x00000020;

#define LED2_ON GPIOE->ODATA &= ~0x00000040;
#define LED3_ON GPIOE->ODATA &= ~0x00000020;

#define LED2_Toggle GPIOE->ODATA ^= 0x00000040;
#define LED3_Toggle GPIOE->ODATA ^= 0x00000020;

/**@} end of group USARTTOCAN_Macros */

/** @defgroup USARTTOCAN_Variables Variables
  @{
*/

CAN_RxMessage_T g_canBuf[CAN_BUF_CNT];
uint8_t g_canBufRpt = 0;
uint8_t g_canBufWpt = 0;
uint8_t canSpeedCnt = 1;

/**@} end of group USARTTOCAN_Variables */

/** @defgroup USARTTOCAN_Functions Functions
  @{
*/

/*!
 * @brief   CAN Config
 *
 * @param   None
 *
 * @retval  None
 *
 */
void Can_Config(void)
{
    GPIO_Config_T      configStruct;
    CAN_Config_T        canConfig;
    CAN_FilterConfig_T  FilterStruct;

    /* CAN GPIOs configuration **************************************************/
    /* Enable GPIO clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOB | RCM_APB2_PERIPH_AFIO);

    GPIO_ConfigPinRemap(GPIO_REMAP1_CAN1);
    configStruct.pin   = GPIO_PIN_9;     // CAN1 Tx remap 1
    configStruct.mode  = GPIO_MODE_AF_PP;
    configStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &configStruct);

    configStruct.pin = GPIO_PIN_8;       // CAN1 Rx remap 1
    configStruct.mode = GPIO_MODE_IN_PU;
    GPIO_Config(GPIOB, &configStruct);

    NVIC_EnableIRQRequest(USBD1_LP_CAN1_RX0_IRQn, 5, 0);

    /* CAN configuration ********************************************************/
    /* Enable CAN clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN1);

    /* CAN register init */
    CAN_Reset(CAN1);
    CAN_ConfigStructInit(&canConfig);

    /* CAN cell init */
    canConfig.autoBusOffManage = DISABLE;
    canConfig.autoWakeUpMode   = DISABLE;
    canConfig.nonAutoRetran    = DISABLE;
    canConfig.rxFIFOLockMode   = DISABLE;
    canConfig.txFIFOPriority   = ENABLE;
    canConfig.mode             = CAN_MODE_NORMAL;
    canConfig.syncJumpWidth    = CAN_SJW_1;

    /* CAN Baudrate = SysClock / ((BS1 + BS2 + 1)*(prescaler)) */
    /* Default Speed = 48M / 48 / (1+3+4) = 125K */
    canConfig.timeSegment1     = CAN_TIME_SEGMENT1_4;
    canConfig.timeSegment2     = CAN_TIME_SEGMENT2_3;
    canConfig.prescaler = 48;
    CAN_Config(CAN1, &canConfig);

    /* CAN filter init "FIFO0" */
    FilterStruct.filterNumber = 1;
    FilterStruct.filterMode = CAN_FILTER_MODE_IDMASK;
    FilterStruct.filterScale = CAN_FILTER_SCALE_16BIT;
    FilterStruct.filterIdHigh = 0x0000;
    FilterStruct.filterIdLow  = 0x0000;
    FilterStruct.filterMaskIdHigh = 0x0000;
    FilterStruct.filterMaskIdLow  = 0x0000;
    FilterStruct.filterFIFO = CAN_FILTER_FIFO_0;
    FilterStruct.filterActivation =  ENABLE;
    CAN_ConfigFilter(CAN1, &FilterStruct);

    CAN_EnableDBGFreeze(CAN1);
    /* Enable FIFO 0 message pending Interrupt */
    CAN_EnableInterrupt(CAN1, CAN_INT_F0MP);
}

/*!
 * @brief   CAN Read
 *
 * @param   None
 *
 * @retval  None
 *
 */
uint8_t Can_Read(CAN_RxMessage_T *dat)
{
    if(g_canBufRpt != g_canBufWpt)
    {
        memcpy(dat->data, g_canBuf[g_canBufRpt].data, sizeof(dat->data));
        dat->dataLengthCode = g_canBuf[g_canBufRpt].dataLengthCode;
        dat->extID = g_canBuf[g_canBufRpt].extID;
        dat->filterMatchIndex = g_canBuf[g_canBufRpt].filterMatchIndex;
        dat->typeID = g_canBuf[g_canBufRpt].typeID;
        dat->remoteTxReq = g_canBuf[g_canBufRpt].remoteTxReq;
        dat->stdID = g_canBuf[g_canBufRpt].stdID;

        g_canBufRpt++;
        if(g_canBufRpt == CAN_BUF_CNT)
        {
            g_canBufRpt = 0;
        }
        return 1;
    }

    return 0;
}

/*!
 * @brief   CAN Write
 *
 * @param   id: slave id
 *
 * @param   dat: address for data to be sent
 *
 * @param   len: data length
 *
 * @retval  None
 *
 */
void Can_Write(uint8_t id, uint8_t *dat, uint16_t len)
{
    uint16_t delay = 0xf;
    CAN_TxMessage_T txMsg;

    txMsg.stdID = id;
    txMsg.dataLengthCode = len;
    txMsg.extID = 0;
    txMsg.typeID = CAN_TYPEID_STD;
    txMsg.remoteTxReq = CAN_RTXR_DATA;
    memcpy(txMsg.data, dat, len);
    if(canSpeedCnt < 3)
    {
        while(delay--)
        {
            __nop();
        }
    }
    CAN_TxMessage(CAN1, &txMsg);

}

/*!
 * @brief   Send the received response message through the serial port, return 1 if it's 0x79.
 *
 * @param   msg: CAN Receive Structure.
 *
 * @retval  the received response state.
 *
 */
uint8_t Can_ReadSlaveAck(CAN_RxMessage_T *msg)
{
    while(!Can_Read(msg));

    USART_Write(msg->data, 1);

    return(msg->data[0] == 0x79);
}

/*!
 * @brief   Receive data from the slave
 *
 * @param   dat: Receive buf address.
 *
 * @param   len: Receive buf length.
 *
 * @retval  None
 *
 */
void Can_ReadDataFromSlave(uint8_t *dat, uint16_t len)
{
    uint16_t i;
    uint8_t j;
    CAN_RxMessage_T rxMsg;

    for(i = 0; i < len; )
    {
        while(!Can_Read(&rxMsg));

        for(j = 0; j < rxMsg.dataLengthCode; j++)
        {
            dat[i++] = rxMsg.data[j];
            if(i == len)
            {
                return;
            }
        }
    }
}

/*!
 * @brief   CAN Interrupt Service Function
 *
 * @param   None
 *
 * @retval  None
 *
 */
void CAN_Interrupt(void)
{
    while(CAN_ReadStatusFlag(CAN1, CAN_FLAG_F0MP))
    {
        CAN_RxMessage(CAN1, CAN_RX_FIFO_0, &g_canBuf[g_canBufWpt++]);
        if(g_canBufWpt == CAN_BUF_CNT)
        {
            g_canBufWpt = 0;
        }
    }
}

/*!
 * @brief   CAN Adjusts Baud Rate
 *
 * @param   None
 *
 * @retval  None
 *
 */
void CAN_SwitchSpeed(void)
{
    CAN_TxMessage_T txMsg;
    CAN_RxMessage_T rxMsg;
    canSpeedCnt++;

    txMsg.stdID = 0x03;
    txMsg.dataLengthCode = 1;
    txMsg.extID = 0;
    txMsg.typeID = CAN_TYPEID_STD;
    txMsg.remoteTxReq = CAN_RTXR_DATA;
    txMsg.data[0] = canSpeedCnt;

    CAN_TxMessage(CAN1, &txMsg);

    /* Receive the ACK before the baud rate is changed */
    while (!(CAN1->RXF0));

    while(CAN_ReadStatusFlag(CAN1, CAN_FLAG_F0MP))
    {
        CAN_RxMessage(CAN1, CAN_RX_FIFO_0, &g_canBuf[g_canBufWpt++]);
        if(g_canBufWpt == CAN_BUF_CNT)
        {
            g_canBufWpt = 0;
        }
    }
    while(!Can_Read(&rxMsg));

    /* Configure CAN baud rate to be consistent with Bootloader */
    switch (canSpeedCnt)
    {
        case 1:
            LED2_OFF
            LED3_OFF

            /** 125K */
            CAN1->MCTRL = 1;
            while( (CAN1->MSTS & 0X1) != 0X1);
            CAN1->BITTIM = ((1 << 20) | (2 << 16) | (64-1));
            CAN1->MCTRL &= ~(uint32_t)0X1;

            break;
        case 2:
            LED2_OFF
            LED3_ON

            /** 250K */
            CAN1->MCTRL = 1;
            while( (CAN1->MSTS & 0X1) != 0X1);
            CAN1->BITTIM = ((1 << 20) | (2 << 16) | (32-1));
            CAN1->MCTRL &= ~(uint32_t)0X1;

            break;
        case 3:
            LED2_ON
            LED3_OFF

            /** 500K */
            CAN1->MCTRL = 1;
            while( (CAN1->MSTS & 0X1) != 0X1);
            CAN1->BITTIM = ((1 << 20) | (2 << 16) | (16-1));
            CAN1->MCTRL &= ~(uint32_t)0X1;

            break;
        case 4:
            LED2_ON
            LED3_ON

            /** 1M */
            CAN1->MCTRL = 1;
            while( (CAN1->MSTS & 0X1) != 0X1);
            CAN1->BITTIM = ((1 << 20) | (2 << 16) | (8-1));
            CAN1->MCTRL &= ~(uint32_t)0X1;

            canSpeedCnt = 0;
            break;
        default:
            break;
    }

    /* Receive ACK after modifying baud rate */
    while (!(CAN1->RXF0));

    while(CAN_ReadStatusFlag(CAN1, CAN_FLAG_F0MP))
    {
        CAN_RxMessage(CAN1, CAN_RX_FIFO_0, &g_canBuf[g_canBufWpt++]);
        if(g_canBufWpt == CAN_BUF_CNT)
        {
            g_canBufWpt = 0;
        }
    }
    while(!Can_Read(&rxMsg));
}

/**@} end of group USARTTOCAN_Functions */
/**@} end of group USARTTOCAN */
/**@} end of group Examples */
