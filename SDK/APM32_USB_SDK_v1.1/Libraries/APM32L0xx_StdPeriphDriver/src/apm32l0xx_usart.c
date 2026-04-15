/*!
 * @file        apm32l0xx_usart.c
 *
 * @brief       This file provides all the USART firmware functions
 *
 * @version     V1.0.0
 *
 * @date        2021-12-25
 *
 * @attention
 *
 *  Copyright (C) 2020-2023 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "apm32l0xx_usart.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup USART_Driver USART Driver
  @{
*/

/** @addtogroup USART_Fuctions Fuctions
  @{
*/

/*!
 * @brief       Reset usart peripheral registers to their default reset values
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */

void USART_Reset(USART_T* usart)
{
    if (USART1 == usart)
    {
        RCM_EnableAPB2PeriphReset(RCM_APB2_PERIPH_USART1);
        RCM_DisableAPB2PeriphReset(RCM_APB2_PERIPH_USART1);
    }
    else if (USART2 == usart)
    {
        RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_USART2);
        RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_USART2);
    }
    else if (USART4 == usart)
    {
        RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_USART4);
        RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_USART4);
    }
    else if (USART5 == usart)
    {
        RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_USART5);
        RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_USART5);
    }
}

/*!
 * @brief       Config the USART peripheral according to the specified parameters in the USART_InitStruct
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       configStruct:   pointer to a USART_Config_T structure
 *
 * @retval      None
 *
 */
void USART_Config(USART_T* uart, USART_Config_T* configStruct)
{
    uint32_t temp, fCLK, intDiv, fractionalDiv;

    /** Disable USART */
    uart->CTRL1_B.UEN = BIT_RESET;

    /** WLS, PCEN, TXEN, RXEN */
    temp = uart->CTRL1;
    temp &= 0xE9F3;
    temp |= (uint32_t)configStruct->mode | \
            (uint32_t)configStruct->parity | \
            (uint32_t)configStruct->wordLength;
    uart->CTRL1 = temp;

    /** STOP bits */
    temp = uart->CTRL2;
    temp &= 0xCFFF;
    temp |= configStruct->stopBits;
    uart->CTRL2 = temp;

    /** Hardware Flow Control */
    temp = uart->CTRL3;
    temp &= 0xFCFF;
    temp |= (uint32_t)configStruct->hardwareFlowCtrl;
    uart->CTRL3 = temp;

    if (uart == USART1)
    {
        fCLK = RCM_ReadUSART1ClockFreq();
    }
    else if (uart == USART2)
    {
        fCLK = RCM_ReadUSART2ClockFreq();
    }
    else
    {
        fCLK = RCM_ReadAPB1ClockFreq();
    }

    intDiv = ((25 * fCLK) / (4 * (configStruct->baudRate)));
    temp = (intDiv / 100) << 4;
    fractionalDiv = intDiv - (100 * (temp >> 4));
    temp |= ((((fractionalDiv * 16) + 50) / 100)) & ((uint8_t)0x0F);

    uart->BR = temp;
}

/*!
 * @brief       Fills each USART_InitStruct member with its default value
 *
 * @param       configStruct:   pointer to a USART_Config_T structure which will be initialized
 *
 * @retval      None
 */
void USART_ConfigStructInit(USART_Config_T* configStruct)
{
    configStruct->baudRate = 9600;
    configStruct->wordLength = USART_WORD_LEN_8B;
    configStruct->stopBits = USART_STOP_BIT_1;
    configStruct->parity = USART_PARITY_NONE ;
    configStruct->mode = USART_MODE_TX_RX;
    configStruct->hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
}

/*!
 * @brief       Synchronous communication clock configuration
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       syncClockConfig:    Pointer to a USART_SyncClockConfig_T structure that
 *                                  contains the configuration information for the clock
 *
 * @retval      None
 *
 */
void USART_ConfigSyncClock(USART_T* usart, USART_SyncClockConfig_T* syncClockConfig)
{
    usart->CTRL2_B.CLKEN = syncClockConfig->clock;
    usart->CTRL2_B.CPHA = syncClockConfig->phase;
    usart->CTRL2_B.CPOL = syncClockConfig->polarity;
    usart->CTRL2_B.LBCPOEN = syncClockConfig->lastBitClock;
}

/*!
 * @brief       Fills each SyncClockConfig member with its default value
 *
 * @param       syncClockConfig:    Pointer to a USART_SyncClockConfig_T structure
 *
 * @retval      None
 */
void USART_ConfigSyncClockStructInit(USART_SyncClockConfig_T* syncClockConfig)
{
    syncClockConfig->clock = USART_CLKEN_DISABLE;
    syncClockConfig->phase = USART_CLKPHA_1EDGE;
    syncClockConfig->polarity = USART_CLKPOL_LOW;
    syncClockConfig->lastBitClock = USART_LBCP_DISABLE;
}

/*!
 * @brief       Enables the specified USART peripheral
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_Enable(USART_T* usart)
{
    usart->CTRL1_B.UEN = BIT_SET;
}

/*!
 * @brief       Disables the specified USART peripheral
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_Disable(USART_T* usart)
{
    usart->CTRL1_B.UEN = BIT_RESET;
}

/*!
 * @brief       Enables the USART direction mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       mode:   Specifies the USART direction
 *                      The parameter can be one of following values:
 *                      @arg USART_MODE_RX:    USART Transmitter
 *                      @arg USART_MODE_TX:    USART Receiver
 *                      @arg USART_MODE_TX_RX: USART Transmitter and Receiver
 *
 * @retval      None
 *
 */
void USART_EnableDirectionMode(USART_T* usart, USART_MODE_T mode)
{
    if (mode == USART_MODE_RX)
    {
        usart->CTRL1_B.RXEN = BIT_SET;
    }

    if (mode == USART_MODE_TX)
    {
        usart->CTRL1_B.TXEN = BIT_SET;
    }

    if (mode == USART_MODE_TX_RX)
    {
        usart->CTRL1_B.TXEN = BIT_SET;
        usart->CTRL1_B.RXEN = BIT_SET;
    }
}

/*!
 * @brief       Disables the USART direction mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       mode:   Specifies the USART direction
 *                      The parameter can be one of following values:
 *                      @arg USART_MODE_RX:    USART Transmitter
 *                      @arg USART_MODE_TX:    USART Receiver
 *                      @arg USART_MODE_TX_RX: USART Transmitter and Receiver
 *
 * @retval      None
 *
 */
void USART_DisableDirectionMode(USART_T* usart, USART_MODE_T mode)
{
    if (mode == USART_MODE_RX)
    {
        usart->CTRL1_B.RXEN = BIT_RESET;
    }

    if (mode == USART_MODE_TX)
    {
        usart->CTRL1_B.TXEN = BIT_RESET;
    }

    if (mode == USART_MODE_TX_RX)
    {
        usart->CTRL1_B.TXEN = BIT_RESET;
        usart->CTRL1_B.RXEN = BIT_RESET;
    }
}

/*!
 * @brief       Sets the system clock divider.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       divider: Specifies the prescaler clock value
 *
 * @retval      None
 *
 */
void USART_ConfigDivider(USART_T* usart, uint8_t divider)
{
    usart->GTPSC_B.PSC = (uint8_t)divider;
}

/*!
 * @brief       Enables the Over Sampling 8 mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 *
 */
void USART_EnableOverSampling8(USART_T* usart)
{
    usart->CTRL1_B.OSMCFG = BIT_SET;
}

/*!
 * @brief       Disables the the Over Sampling 8 mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableOverSampling8(USART_T* usart)
{
    usart->CTRL1_B.OSMCFG = BIT_RESET;
}

/*!
 * @brief       Enables the most significant bit first
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableMSBFirst(USART_T* usart)
{
    usart->CTRL2_B.MSBFEN = BIT_SET;
}

/*!
 * @brief       Disables the most significant bit first
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableMSBFirst(USART_T* usart)
{
    usart->CTRL2_B.MSBFEN = BIT_RESET;
}

/*!
 * @brief       Enables the USART's one bit sampling method.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableOneBitMethod(USART_T* usart)
{
    usart->CTRL3_B.SAMCFG = BIT_SET;
}

/*!
 * @brief       Disables the USART's one bit sampling method.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableOneBitMethod(USART_T* usart)
{
    usart->CTRL3_B.SAMCFG = BIT_RESET;
}

/*!
 * @brief       Enables the USART's DataInv.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableDataInv(USART_T* usart)
{
    usart->CTRL2_B.BINVEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's DataInv.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableDataInv(USART_T* usart)
{
    usart->CTRL2_B.BINVEN = BIT_RESET;
}

/*!
 * @brief       Enables the specified USART peripheral
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       invPin: specifies the USART pin(s) to invert
 *              This parameter can be one of the following values:
 *              @arg USART_INVERSION_RX: USART Tx pin active level inversion
 *              @arg USART_INVERSION_TX: USART Rx pin active level inversion
 *              @arg USART_INVERSION_TX_RX: USART TX Rx pin active level inversion
 * @retval      None
 *
 */
void USART_EnableInvPin(USART_T* usart, USART_INVERSION_T invPin)
{
    if (invPin == USART_INVERSION_RX)
    {
        usart->CTRL2_B.RXINVEN = BIT_SET;
    }

    if (invPin == USART_INVERSION_TX)
    {
        usart->CTRL2_B.TXINVEN = BIT_SET;
    }

    if (invPin == (USART_INVERSION_TX_RX))
    {
        usart->CTRL2_B.TXINVEN = BIT_SET;
        usart->CTRL2_B.RXINVEN = BIT_SET;
    }
}

/*!
 * @brief       Disables the specified USART peripheral
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       invPin: specifies the USART pin(s) to invert
 *              This parameter can be one of the following values:
 *              @arg USART_INVERSION_RX: USART Tx pin active level inversion
 *              @arg USART_INVERSION_TX: USART Rx pin active level inversion
 *              @arg USART_INVERSION_TX_RX: USART TX Rx pin active level inversion
 *
 * @retval      None
 *
 */
void USART_DisableInvPin(USART_T* usart, USART_INVERSION_T invPin)
{
    if (invPin == USART_INVERSION_RX)
    {
        usart->CTRL2_B.RXINVEN = BIT_RESET;
    }

    if (invPin == USART_INVERSION_TX)
    {
        usart->CTRL2_B.TXINVEN = BIT_RESET;
    }

    if (invPin == USART_INVERSION_TX_RX)
    {
        usart->CTRL2_B.TXINVEN = BIT_RESET;
        usart->CTRL2_B.RXINVEN = BIT_RESET;
    }
}

/*!
 * @brief       Enables the swap Tx/Rx pins
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableSWAPPin(USART_T* usart)
{
    usart->CTRL2_B.SWAPEN = BIT_SET;
}

/*!
 * @brief       Disables the swap Tx/Rx pins
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableSWAPPin(USART_T* usart)
{
    usart->CTRL2_B.SWAPEN = BIT_RESET;
}

/*!
 * @brief       Enables the receiver Time Out feature
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableReceiverTimeOut(USART_T* usart)
{
    usart->CTRL2_B.RXTODEN = BIT_SET;
}

/*!
 * @brief       Disables the receiver Time Out feature
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_DisableReceiverTimeOut(USART_T* usart)
{
    usart->CTRL2_B.RXTODEN = BIT_RESET;
}

/*!
 * @brief       Sets the receiver Time Out value
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       timeOut: Specifies the Receiver Time Out value
 *
 * @retval      None
 *
 *
 * @note        The value must less than 0x00FFFFFF
 */
void USART_ReceiverTimeOutValue(USART_T* usart, uint32_t timeOut)
{
    if (timeOut <= (uint32_t)0x00FFFFFF)
    {
        usart->RXTO_B.RXTO = (uint32_t)timeOut;
    }
}

/*!
 * @brief       Enables the auto baud rate
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableAutoBaudRate(USART_T* usart)
{
    usart->CTRL2_B.ABRDEN = BIT_SET;
}

/*!
 * @brief       Disables the auto baud rate
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_DisableAutoBaudRate(USART_T* usart)
{
    usart->CTRL2_B.ABRDEN = BIT_RESET;
}

/*!
 * @brief       Config the auto baud rate
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       mode:   specifies the selected USART auto baud rate method
 *                      This parameter can be one of the following values:
 *                      @arg USART_AUTO_BAUD_RATE_MODE_STARTBIT:    Start Bit duration measurement
 *                      @arg USART_AUTO_BAUD_RATE_MODE_FALLINGEDGE: Falling edge to falling edge measurement
 *                      @arg USART_AUTO_BAUD_RATE_MODE_FRAME_7F:    0x7F frame detection
 *                      @arg USART_AUTO_BAUD_RATE_MODE_FRAME_55:    0x55 frame detection
 *
 * @retval
 *
 */
void USART_ConfigAutoBaudRate(USART_T* usart, USART_AUTO_BAUD_RATE_MODE_T mode)
{
    usart->CTRL2_B.ABRDCFG = mode;
}

/*!
 * @brief       Enables the USART DMA interface when reception error occurs
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 *
 * @retval      None
 *
 */
void USART_EnableOverrunDetection(USART_T* usart)
{
    usart->CTRL3_B.OVRDEDIS = BIT_SET;
}

/*!
 * @brief       Disables the USART DMA interface when reception error occurs
 *
* @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 *
 * @retval      None
 *
 */
void  USART_DisableOverrunDetection(USART_T* usart)
{
    usart->CTRL3_B.OVRDEDIS = BIT_RESET;
}

/*!
 * @brief       Enables the stop mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableStopMode(USART_T* usart)
{
    usart->CTRL1_B.USWMEN = BIT_SET;
}

/*!
 * @brief       Disables the stop mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_DisableStopMode(USART_T* usart)
{
    usart->CTRL1_B.USWMEN = BIT_RESET;
}

/*!
 * @brief       Selects the USART WakeUp method form stop mode.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       source: Specifies the selected USART wakeup method
 *                      This parameter can be one of the following values:
 *                      @arg USART_WAKEUP_SOURCE_ADDRESS:  Wakeup active on address match.
 *                      @arg USART_WAKEUP_SOURCE_START:    Wakeup active on Start bit detection.
 *                      @arg USART_WAKEUP_SOURCE_RXNE:     Wakeup active on RXNE.
 *
 */
void USART_ConfigStopModeWakeUpSource(USART_T* usart, USART_WAKEUP_SOURCE_T source)
{
    usart->CTRL3_B.WSIFLGSEL = source;
}

/*!
 * @brief       Sets USART the address
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_Address(USART_T* usart, uint8_t address)
{
    usart->CTRL2_B.ADDRL = ((uint8_t)address & 0x0F);
    usart->CTRL2_B.ADDRH = (((uint8_t)address>>4) & 0x0F);
}

/*!
 * @brief       Configure the the USART Address detection length.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       address: Specifies the selected USART auto baud rate method
 *                       This parameter can be one of the following values:
 *                       @arg USART_ADDRESS_MODE_4B: 4-bit address length detection
 *                       @arg USART_ADDRESS_MODE_7B: 7-bit address length detection
 *
 * @retval      None
 *
 */
void USART_ConfigAddressDetection(USART_T* usart, USART_ADDRESS_MODE_T address)
{
    usart->CTRL2_B.ADDRLEN = address;
}

/*!
 * @brief       Transmit Data
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       data:    Specifies the Transmits data value
 *
 * @retval      None
 *
 * @note        The value must less than 0x01FF
 */
void USART_TxData(USART_T* usart, uint16_t data)
{
    usart->TXDATA = (data & (uint16_t)0x01FF);
}

/*!
 * @brief       Received Data
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      Returns the received data value
 *
 */
uint16_t USART_RxData(USART_T* usart)
{
    return (uint16_t)(usart->RXDATA & (uint16_t)0x01FF);
}

/*!
 * @brief       Enables the USART's mute mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableMuteMode(USART_T* usart)
{
    usart->CTRL1_B.RXMUTEEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's mute mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableMuteMode(USART_T* usart)
{
    usart->CTRL1_B.RXMUTEEN = BIT_RESET;
}

/*!
 * @brief       Selects the USART WakeUp method from mute mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       wakeup: Specifies the selected USART auto baud rate method
 *                      This parameter can be one of the following values:
 *                      @arg USART_WAKEUP_IDLE_LINE:    WakeUp by an idle line detection
 *                      @arg USART_WAKEUP_ADDRESS_MARK: WakeUp by an address mark
 *
 * @retval      None
 *
 */
void USART_ConfigMuteModeWakeUp(USART_T* usart, USART_WAKEUP_T wakeup)
{
    usart->CTRL1_B.WUPMCFG = wakeup;
}

/*!
 * @brief       Enables the USART's LIN mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableLINmode(USART_T* usart)
{
    usart->CTRL2_B.LINMEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's LIN mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_DisableLINmode(USART_T* usart)
{
    usart->CTRL2_B.LINMEN = BIT_RESET;
}

/*!
 * @brief       Sets the USART LIN Break detection length.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       length:  Specifies the selected USART auto baud rate method
 *                       This parameter can be one of the following values:
 *                       @arg USART_BREAK_LENGTH_10B: 10-bit break detection
 *                       @arg USART_BREAK_LENGTH_11B: 11-bit break detection
 *
 * @retval      None
 *
 */
void USART_ConfigLINbreakDetectLength(USART_T* usart, USART_BREAK_LENGTH_T length)
{
    usart->CTRL2_B.LBDLCFG = length;
}

/*!
 * @brief       Enables the USART's Half-duplex mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 * @retval      None
 *
 */
void USART_EnableHalfDuplex(USART_T* usart)
{
    usart->CTRL3_B.HDEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's Half-duplex mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 * @retval      None
 *
 */
void USART_DisableHalfDuplex(USART_T* usart)
{
    usart->CTRL3_B.HDEN = BIT_RESET;
}

/*!
 * @brief       Enables the USART's Smart Card mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableSmartCard(USART_T* usart)
{
    usart->CTRL3_B.SCEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's Smart Card mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_DisableSmartCard(USART_T* usart)
{
    usart->CTRL3_B.SCEN = BIT_RESET;
}

/*!
 * @brief       Enables the USART's NACK transmission
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableSmartCardNACK(USART_T* usart)
{
    usart->CTRL3_B.SCNACKEN = BIT_SET;
}

/*!
 * @brief       Disables the USART's NACK transmission
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_DisableSmartCardNACK(USART_T* usart)
{
    usart->CTRL3_B.SCNACKEN = BIT_RESET;
}

/*!
 * @brief       Config the specified USART guard time.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       guardTime: specifies the guard time value
 *
 * @retval      None
 *
 */
void USART_ConfigGuardTime(USART_T* usart, uint8_t guardTime)
{
    usart->GTPSC_B.GRDT = (uint8_t)guardTime;
}

/*!
 * @brief       Config the specified USART Smart Card number.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       autoCount: specifies the Smart Card auto retry count.
 *
 * @retval      None
 *
 * @note        The value must less than 0x07.
 *
 */
void USART_ConfigAutoCount(USART_T* usart, uint8_t autoCount)
{
    usart->CTRL3_B.SCARCCFG = autoCount;
}

/*!
 * @brief       Config the Smart Card Block length.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       blockSize:  specifies the Smart Card block size.
 *
 * @retval      None
 *
 */
void USART_ConfigBlockSize(USART_T* usart, uint8_t blockSize)
{
    usart->RXTO_B.BLEN = (uint8_t)blockSize;
}

/*!
 * @brief       Enables the USART's IrDA mode.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @retval      None
 *
 */
void USART_EnableIrDA(USART_T* usart)
{
    usart->CTRL3_B.IREN = BIT_SET;
}

/*!
 * @brief       Disables the USART's IrDA mode.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 *
 * @retval      None
 *
 */
void USART_DisableIrDA(USART_T* usart)
{
    usart->CTRL3_B.IREN = BIT_RESET;
}

/*!
 * @brief       Configures the USART's IrDA interface.
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2.
 *
 * @param       IrDAMode:Specifies the selected USART auto baud rate method
 *                       This parameter can be one of the following values:
 *                       @arg USART_IRDA_MODE_NORMAL:   Normal
 *                       @arg USART_IRDA_MODE_LOWPOWER: Low-Power
 *
 * @retval      None
 *
 */
void USART_ConfigIrDAMode(USART_T* usart, USART_IRDA_MODE_T IrDAMode)
{
    usart->CTRL3_B.IRLPEN = IrDAMode;
}

/*!
 * @brief       Enables the Driver mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableDE(USART_T* usart)
{
    usart->CTRL3_B.DEN = BIT_SET;
}

/*!
 * @brief       Disables the Driver mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableDE(USART_T* usart)
{
    usart->CTRL3_B.DEN = BIT_RESET;
}

/*!
 * @brief       Selects the USART WakeUp method from mute mode
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       polarity: Specifies the selected USART auto baud rate method
 *                        This parameter can be one of the following values:
 *                        @arg USART_DE_POL_HIGH:  DE signal is active high
 *                        @arg USART_DE_POL_LOW:   DE signal is active low
 *
 * @retval      None
 *
 */
void USART_ConfigDEPolarity(USART_T* usart, USART_DE_POL_T polarity)
{
    usart->CTRL3_B.DPCFG = polarity;
}

/*!
 * @brief       Sets the driver enable assertion time value
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       value:  Specifies the DE assertion time value
 *
 * @retval      None
 *
 */
void USART_DEAssertionTimeValue(USART_T* usart, uint8_t value)
{
    usart->CTRL1_B.DLTEN = (uint8_t)0x00;

    if (value <= (uint8_t)0x1F)
    {
        usart->CTRL1_B.DLTEN = (uint8_t)value;
    }
}

/*!
 * @brief       Sets the driver enable deassertion time value
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       value:  Specifies the DE deassertion time value
 *
 * @retval      None
 *
 */
void USART_DEDeassertionTimeValue(USART_T* usart, uint8_t value)
{
    usart->CTRL1_B.DDLTEN = (uint8_t)0x00;

    if (value <= (uint8_t)0x1F)
    {
        usart->CTRL1_B.DDLTEN = (uint8_t)value;
    }
}

/*!
 * @brief       Enables the USART DMA interface
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       dmaReq: Specifies the DMA request
 *                      This parameter can be any combination of the following values:
 *                      @arg USART_DMA_REQUEST_RX:  USART DMA receive request
 *                      @arg USART_DMA_REQUEST_TX:  USART DMA transmit request
 *                      @arg USART_DMA_REQUEST_RX_TX:  USART DMA transmit and receive request
 *
 * @retval      None
 *
 */
void USART_EnableDMA(USART_T* usart, USART_DMA_REQUEST_T dmaReq)
{
    if (dmaReq == USART_DMA_REQUEST_RX)
    {
        usart->CTRL3_B.DMARXEN = BIT_SET;
    }
    else if (dmaReq == USART_DMA_REQUEST_TX)
    {
        usart->CTRL3_B.DMATXEN = BIT_SET;
    }
    else if (dmaReq == USART_DMA_REQUEST_RX_TX)
    {
        usart->CTRL3_B.DMATXEN = BIT_SET;
        usart->CTRL3_B.DMARXEN = BIT_SET;
    }
}

/*!
 * @brief       Disables the USART DMA interface
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       dmaReq: Specifies the DMA request
 *                      This parameter can be any combination of the following values:
 *                      @arg USART_DMA_REQUEST_RX:    USART DMA receive request
 *                      @arg USART_DMA_REQUEST_TX:    USART DMA transmit request
 *                      @arg USART_DMA_REQUEST_RX_TX: USART DMA transmit and receive request
 *
 * @retval      None
 *
 */
void USART_DisableDMA(USART_T* usart, USART_DMA_REQUEST_T dmaReq)
{
    if (dmaReq == USART_DMA_REQUEST_RX)
    {
        usart->CTRL3_B.DMARXEN = BIT_RESET;
    }
    else if (dmaReq == USART_DMA_REQUEST_TX)
    {
        usart->CTRL3_B.DMATXEN = BIT_RESET;
    }
    else if (dmaReq == USART_DMA_REQUEST_RX_TX)
    {
        usart->CTRL3_B.DMATXEN = BIT_RESET;
        usart->CTRL3_B.DMARXEN = BIT_RESET;
    }
}

/*!
 * @brief       Enables the USART DMA interface when reception error occurs
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_EnableDMAReceptionError(USART_T* usart)
{
    usart->CTRL3_B.DDISRXEEN = BIT_SET;
}

/*!
 * @brief       Disables the USART DMA interface when reception error occurs
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @retval      None
 *
 */
void USART_DisableDMAReceptionError(USART_T* usart)
{
    usart->CTRL3_B.DDISRXEEN = BIT_RESET;
}

/*!
 * @brief       Enables the specified USART's Request
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       request: specifies the USART request
 *              This parameter can be one of the following values:
 *              @arg USART_REQUEST_ABRQ: Auto Baud Rate Request
 *              @arg USART_REQUEST_SBQ:  Send Break Request
 *              @arg USART_REQUEST_MMQ:  Mute Mode Request
 *              @arg USART_REQUEST_RDFQ: Receive data flush Request
 *              @arg USART_REQUEST_TDFQ: Transmit data flush Request
 *
 * @retval      None
 *
 */
void USART_EnableRequest(USART_T* usart, USART_REQUEST_T request)
{
    if (request == USART_REQUEST_ABRQ)
    {
        usart->REQUEST_B.ABRDQ = BIT_SET;
    }

    if (request == USART_REQUEST_SBQ)
    {
        usart->REQUEST_B.TXBFQ = BIT_SET;
    }

    if (request == USART_REQUEST_MMQ)
    {
        usart->REQUEST_B.MUTEQ = BIT_SET;
    }

    if (request == USART_REQUEST_RDFQ)
    {
        usart->REQUEST_B.RXDFQ = BIT_SET;
    }

    if (request == USART_REQUEST_TDFQ)
    {
        usart->REQUEST_B.TXDFQ = BIT_SET;
    }
}

/*!
 * @brief       Enables the specified interrupts
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       interrupt:  Specifies the USART interrupts sources
 *                          The parameter can be one of following values:
 *                          @arg USART_INT_WAKEUP:  Wake up interrupt
 *                          @arg USART_INT_CMIE:    Character match interrupt
 *                          @arg USART_INT_EOBIE:   End of Block interrupt
 *                          @arg USART_INT_RXTOIE:  Receive time out interrupt
 *                          @arg USART_INT_CTSIE:   CTS change interrupt
 *                          @arg USART_INT_LBDIE:   LIN Break detection interrupt
 *                          @arg USART_INT_TXBEIE:  Tansmit Data Register empty interrupt
 *                          @arg USART_INT_TXCIE:   Transmission complete interrupt
 *                          @arg USART_INT_RXBNEIE: Receive Data register not empty interrupt
 *                          @arg USART_INT_IDLEIE:  Idle line detection interrupt
 *                          @arg USART_INT_PEIE:    Parity Error interrupt
 *                          @arg USART_INT_ERRIE:   Error interrupt
 *
 * @retval      None
 *
 */
void USART_EnableInterrupt(USART_T* usart, USART_INT_T interrupt)
{
    if ((interrupt == USART_INT_ERRIE ) | (interrupt == USART_INT_CTSIE) | (interrupt == USART_INT_WAKEUP))
    {
        usart->CTRL3 |= (uint32_t)interrupt;
    }
    else if(interrupt == USART_INT_LBDIE)
    {
        usart->CTRL2 |= (uint32_t)interrupt;
    }
    else
    {
        usart->CTRL1 |= (uint32_t)interrupt;
    }
}

/*!
 * @brief       Disables the specified interrupts
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       interrupt:  Specifies the USART interrupts sources
 *                          The parameter can be one of following values:
 *                          @arg USART_INT_WAKEUP:   Wake up interrupt
 *                          @arg USART_INT_CMIE:    Character match interrupt
 *                          @arg USART_INT_EOBIE:   End of Block interrupt
 *                          @arg USART_INT_RXTOIE:  Receive time out interrupt
 *                          @arg USART_INT_CTSIE:   CTS change interrupt
 *                          @arg USART_INT_LBDIE:   LIN Break detection interrupt
 *                          @arg USART_INT_TXBEIE:  Tansmit Data Register empty interrupt
 *                          @arg USART_INT_TXCIE:   Transmission complete interrupt
 *                          @arg USART_INT_RXBNEIE: Receive Data register not empty interrupt
 *                          @arg USART_INT_IDLEIE:  Idle line detection interrupt
 *                          @arg USART_INT_PEIE:    Parity Error interrupt
 *                          @arg USART_INT_ERRIE:   Error interrupt
 *
 * @retval      None
 *
 */
void USART_DisableInterrupt(USART_T* usart, USART_INT_T interrupt)
{
    if ((interrupt == USART_INT_ERRIE ) | (interrupt == USART_INT_CTSIE) | (interrupt == USART_INT_WAKEUP))
    {
        usart->CTRL3 &= (uint32_t)~interrupt;
    }
    else if(interrupt == USART_INT_LBDIE)
    {
        usart->CTRL2 &= (uint32_t)~interrupt;
    }
    else
    {
        usart->CTRL1 &= (uint32_t)~interrupt;
    }
}

/*!
 * @brief       Read the specified USART interrupt flag
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       flag:   Specifies the USART interrupt flag to check
 *                      The parameter can be one of following values:
 *                      @arg USART_INT_FLAG_WAKEUP: Wake up interrupt flag
 *                      @arg USART_INT_FLAG_CMF:    Character match interrupt flag
 *                      @arg USART_INT_FLAG_EOBF:   End of block interrupt flag
 *                      @arg USART_INT_FLAG_RXTOF:  Receive time out interrupt flag
 *                      @arg USART_INT_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg USART_INT_FLAG_LBDF:   LIN Break detection interrupt flag
 *                      @arg USART_INT_FLAG_TXBE:   Transmit data register empty interrupt flag
 *                      @arg USART_INT_FLAG_TXC:    Transmission Complete interrupt flag
 *                      @arg USART_INT_FLAG_RXBNE:  Receive data buffer not empty interrupt flag
 *                      @arg USART_INT_FLAG_IDLE:   Idle Line detection interrupt flag
 *                      @arg USART_INT_FLAG_OVRE:   OverRun Error interrupt flag
 *                      @arg USART_INT_FLAG_NE:     Noise Error interrupt flag
 *                      @arg USART_INT_FLAG_FE:     Framing Error interrupt flag
 *                      @arg USART_INT_FLAG_PE:     Parity Error interrupt flag
 *
 * @retval      The new state of flag (SET or RESET)
 *
 */

uint8_t USART_ReadIntFlag(USART_T* usart, USART_INT_FLAG_T flag)
{
    uint32_t intEnable = 0;
    uint32_t intFlag = 0;

    if (flag & 0x0E)
    {
        intEnable = usart->CTRL3_B.ERRIEN;
        intFlag = (usart->STS) & flag;
    }
    else if (flag & 0xF0)
    {
        intEnable = (usart->CTRL1) & flag;
        intFlag = (usart->STS) & flag;
    }
    else if (flag & 0x01)
    {
        intEnable = usart->CTRL1_B.PEIEN;
        intFlag = usart->STS_B.PEFLG;
    }
    else if (flag & 0x200)
    {
        intEnable = usart->CTRL3_B.CTSIEN;
        intFlag = usart->STS_B.CTSFLG;
    }
    else if (flag & 0x100)
    {
        intEnable = usart->CTRL2_B.LBDIEN;
        intFlag = usart->STS_B.LBDFLG;
    }
    else if (flag & 0x800)
    {
        intEnable = usart->CTRL1_B.RXTOIEN;
        intFlag = usart->STS_B.RXTOFLG;
    }
    else if (flag & 0x1000)
    {
        intEnable = usart->CTRL1_B.EOBIEN;
        intFlag = usart->STS_B.EOBFLG;
    }
    else if (flag & 0x20000)
    {
        intEnable = usart->CTRL1_B.CMIEN;
        intFlag = usart->STS_B.CMFLG;
    }
    else if (flag & 0x100000)
    {
        intEnable = usart->CTRL3_B.WSMIEN;
        intFlag = usart->STS_B.WSMFLG;
    }

    if (intFlag && intEnable)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clears the USART interrupt pending bits
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       flag:   Specifies the USART interrupt flag to clear
 *                      The parameter can be any combination following values:
 *                      @arg USART_INT_FLAG_WAKEUP: Wake up interrupt flag
 *                      @arg USART_INT_FLAG_CMF:    Character match interrupt flag
 *                      @arg USART_INT_FLAG_EOBF:   End of block interrupt flag
 *                      @arg USART_INT_FLAG_RXTOF:  Receive time out interrupt flag
 *                      @arg USART_INT_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg USART_INT_FLAG_LBDF:   LIN Break detection interrupt flag
 *                      @arg USART_INT_FLAG_TXC:    Transmission Complete interrupt flag
 *                      @arg USART_INT_FLAG_IDLE:   Idle Line detection interrupt flag
 *                      @arg USART_INT_FLAG_OVRE:   OverRun Error interrupt flag
 *                      @arg USART_INT_FLAG_NE:     Noise Error interrupt flag
 *                      @arg USART_INT_FLAG_FE:     Framing Error interrupt flag
 *                      @arg USART_INT_FLAG_PE:     Parity Error interrupt flag
 *
 * @retval      None
 *
 */
void USART_ClearIntFlag(USART_T* usart, uint32_t flag)
{
    usart->INTFCLR |= (uint32_t)flag;
}

/*!
 * @brief       Read the specified USART flag
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       flag:   Specifies the flag to check
 *                      The parameter can be one of following values:
 *                      @arg USART_FLAG_RXENACKF: Receive Enable Acknowledge Flag
 *                      @arg USART_FLAG_TXENACKF: Transmit Enable Acknowledge Flag
 *                      @arg USART_FLAG_WAKEUP: Wake Up from stop mode Flag
 *                      @arg USART_FLAG_RWF:    Send Break flag
 *                      @arg USART_FLAG_SBF:    Send Break flag
 *                      @arg USART_FLAG_CMF:    Character match flag
 *                      @arg USART_FLAG_BUSY:   Busy flag
 *                      @arg USART_FLAG_ABRTF:  Auto baud rate flag
 *                      @arg USART_FLAG_ABRTE:  Auto baud rate error flag
 *                      @arg USART_FLAG_EOBF:   End of block flag
 *                      @arg USART_FLAG_RXTOF:  Receive time out flag
 *                      @arg USART_FLAG_CTSF:   CTS Change flag
 *                      @arg USART_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg USART_FLAG_LBDF:   LIN Break Detection Flag
 *                      @arg USART_FLAG_TXBE:   Transmit data register empty flag
 *                      @arg USART_FLAG_TXC:    Transmission Complete flag
 *                      @arg USART_FLAG_RXBNE:  Receive data buffer not empty flag
 *                      @arg USART_FLAG_IDLEF:  Idle Line detection flag
 *                      @arg USART_FLAG_OVRE:   OverRun Error flag
 *                      @arg USART_FLAG_NEF:    Noise Error flag
 *                      @arg USART_FLAG_FEF:    Framing Error flag
 *                      @arg USART_FLAG_PEF:    Parity Error flag
 *
 * @retval      The new state of flag (SET or RESET)
 *
 */

uint8_t USART_ReadStatusFlag(USART_T* usart, USART_FLAG_T flag)
{
    if ((usart->STS & (uint32_t)flag) != RESET)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clear the specified USART flag
 *
 * @param       usart:  Select the the USART peripheral.
 *                      It can be USART1/USART2/USART4/USART5.
 *
 * @param       flag:   Specifies the flag to clear
 *                      The parameter can be any combination of following values:
 *                      @arg USART_FLAG_WAKEUP: Wake Up from stop mode Flag
 *                      @arg USART_FLAG_CMF:    Character match flag
 *                      @arg USART_FLAG_EOBF:   End of block flag
 *                      @arg USART_FLAG_RXTOF:  Receive time out flag
 *                      @arg USART_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg USART_FLAG_LBDF:   LIN Break Detection Flag
 *                      @arg USART_FLAG_TXC:    Transmission Complete flag
 *                      @arg USART_FLAG_IDLEF:  Idle Line detection flag
 *                      @arg USART_FLAG_OVRE:   OverRun Error flag
 *                      @arg USART_FLAG_NEF:    Noise Error flag
 *                      @arg USART_FLAG_FEF:    Framing Error flag
 *                      @arg USART_FLAG_PEF:    Parity Error flag
 *
 * @retval      None
 *
 */

void USART_ClearStatusFlag(USART_T* usart, uint32_t flag)
{
    usart->INTFCLR = (uint32_t)flag;
}

/**@} end of group USART_Fuctions*/
/**@} end of group USART_Driver*/
/**@} end of group Peripherals_Library*/
