/*!
 * @file        apm32l0xx_lpuart.c
 *
 * @brief       This file provides all the LPUART firmware functions
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

#include "apm32l0xx_lpuart.h"
#include "apm32l0xx_rcm.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup LPUART_Driver LPUART Driver
  @{
*/

/** @addtogroup LPUART_Fuctions Fuctions
  @{
*/

/*!
 * @brief       Reset lpuart peripheral registers to their default reset values
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_Reset(void)
{
    RCM_EnableAPB1PeriphReset(RCM_APB1_PERIPH_LPUART1);
    RCM_DisableAPB1PeriphReset(RCM_APB1_PERIPH_LPUART1);
}

/*!
 * @brief       Config the LPUART peripheral according to the specified parameters in the LPUART_InitStruct
 *
 *
 * @param       configStruct:   pointer to a LPUART_Config_T structure
 *
 * @retval      None
 *
 */
void LPUART_Config(LPUART_Config_T* configStruct)
{
    uint32_t temp, fCLK;

    /** Disable LPUART */
    LPUART1->CTRL1_B.UEN = BIT_RESET;

    /** WLS, PCEN, TXEN, RXEN */
    temp = LPUART1->CTRL1;
    temp &= 0xE9F3;
    temp |= (uint32_t)configStruct->mode | \
            (uint32_t)configStruct->parity | \
            (uint32_t)configStruct->wordLength;
    LPUART1->CTRL1 = temp;

    /** STOP bits */
    temp = LPUART1->CTRL2;
    temp &= 0xCFFF;
    temp |= configStruct->stopBits;
    LPUART1->CTRL2 = temp;

    /** Hardware Flow Control */
    temp = LPUART1->CTRL3;
    temp &= 0xFCFF;
    temp |= (uint32_t)configStruct->hardwareFlowCtrl;
    LPUART1->CTRL3 = temp;

    fCLK=RCM_ReadLPUART1ClockFreq();

    LPUART1->BR = 256 * fCLK / (configStruct->baudRate);
}

/*!
 * @brief       Fills each LPUART_InitStruct member with its default value
 *
 * @param       configStruct:   pointer to a LPUART_Config_T structure which will be initialized
 *
 * @retval      None
 */
void LPUART_ConfigStructInit(LPUART_Config_T* configStruct)
{
    configStruct->baudRate = 9600;
    configStruct->wordLength = LPUART_WORD_LEN_8B;
    configStruct->stopBits = LPUART_STOP_BITS_1;
    configStruct->parity = LPUART_PARITY_NONE;
    configStruct->mode = LPUART_MODE_TX_RX;
    configStruct->hardwareFlowCtrl = LPUART_FLOW_CTRL_NONE;
}

/*!
 * @brief       Enables the specified LPUART peripheral
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_Enable(void)
{
    LPUART1->CTRL1_B.UEN = BIT_SET;
}

/*!
 * @brief       Disables the specified LPUART peripheral
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_Disable(void)
{
    LPUART1->CTRL1_B.UEN = BIT_RESET;
}

/*!
 * @brief       Enables the LPUART direction mode
 *
 * @param       mode:   Specifies the LPUART direction
 *                      The parameter can be one of following values:
 *                      @arg LPUART_MODE_RX:    LPUART Transmitter
 *                      @arg LPUART_MODE_TX:    LPUART Receiver
 *                      @arg LPUART_MODE_TX_RX: LPUART Transmitter and Receiver
 *
 * @retval      None
 *
 */
void LPUART_EnableDirectionMode(LPUART_MODE_T mode)
{
    if (mode == LPUART_MODE_RX)
    {
        LPUART1->CTRL1_B.RXEN = BIT_SET;
    }

    if (mode == LPUART_MODE_TX)
    {
        LPUART1->CTRL1_B.TXEN = BIT_SET;
    }

    if (mode == LPUART_MODE_TX_RX)
    {
        LPUART1->CTRL1_B.TXEN = BIT_SET;
        LPUART1->CTRL1_B.RXEN = BIT_SET;
    }
}

/*!
 * @brief       Disables the LPUART direction mode
 *
 * @param       mode:   Specifies the LPUART direction
 *                      The parameter can be one of following values:
 *                      @arg LPUART_MODE_RX:    LPUART Transmitter
 *                      @arg LPUART_MODE_TX:    LPUART Receiver
 *                      @arg LPUART_MODE_TX_RX: LPUART Transmitter and Receiver
 *
 * @retval      None
 *
 */
void LPUART_DisableDirectionMode(LPUART_MODE_T mode)
{
    if (mode == LPUART_MODE_RX)
    {
        LPUART1->CTRL1_B.RXEN = BIT_RESET;
    }

    if (mode == LPUART_MODE_TX)
    {
        LPUART1->CTRL1_B.TXEN = BIT_RESET;
    }

    if (mode == LPUART_MODE_TX_RX)
    {
        LPUART1->CTRL1_B.TXEN = BIT_RESET;
        LPUART1->CTRL1_B.RXEN = BIT_RESET;
    }
}

/*!
 * @brief       Enables the most significant bit first
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableMSBFirst(void)
{
    LPUART1->CTRL2_B.MSBFEN = BIT_SET;
}

/*!
 * @brief       Disables the most significant bit first
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableMSBFirst(void)
{
    LPUART1->CTRL2_B.MSBFEN = BIT_RESET;
}

/*!
 * @brief       Enables the LPUART's DataInv.
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableDataInv(void)
{
    LPUART1->CTRL2_B.BINVEN = BIT_SET;
}

/*!
 * @brief       Disables the LPUART's DataInv.
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableDataInv(void)
{
    LPUART1->CTRL2_B.BINVEN = BIT_RESET;
}

/*!
 * @brief       Enables the specified LPUART peripheral
 *
 * @param       invPin: specifies the LPUART pin(s) to invert
 *              This parameter can be one of the following values:
 *              @arg LPUART_INVERSION_RX: LPUART Tx pin active level inversion
 *              @arg LPUART_INVERSION_TX: LPUART Rx pin active level inversion
 *              @arg LPUART_INVERSION_TX_RX: LPUART TX Rx pin active level inversion
 *
 * @retval      None
 *
 */
void LPUART_EnableInvPin(LPUART_INVERSION_T invPin)
{
    if (invPin == LPUART_INVERSION_RX)
    {
        LPUART1->CTRL2_B.RXINVEN = BIT_SET;
    }

    if (invPin == LPUART_INVERSION_TX)
    {
        LPUART1->CTRL2_B.TXINVEN = BIT_SET;
    }

    if (invPin == (LPUART_INVERSION_TX_RX))
    {
        LPUART1->CTRL2_B.TXINVEN = BIT_SET;
        LPUART1->CTRL2_B.RXINVEN = BIT_SET;
    }
}

/*!
 * @brief       Disables the specified LPUART peripheral
 *
 * @param       invPin: specifies the LPUART pin(s) to invert
 *              This parameter can be one of the following values:
 *              @arg LPUART_INVERSION_RX: LPUART Tx pin active level inversion
 *              @arg LPUART_INVERSION_TX: LPUART Rx pin active level inversion
 *              @arg LPUART_INVERSION_TX_RX: LPUART TX Rx pin active level inversion
 *
 * @retval      None
 *
 */
void LPUART_DisableInvPin(LPUART_INVERSION_T invPin)
{
    if (invPin == LPUART_INVERSION_RX)
    {
        LPUART1->CTRL2_B.RXINVEN = BIT_RESET;
    }

    if (invPin == LPUART_INVERSION_TX)
    {
        LPUART1->CTRL2_B.TXINVEN = BIT_RESET;
    }

    if (invPin == LPUART_INVERSION_TX_RX)
    {
        LPUART1->CTRL2_B.TXINVEN = BIT_RESET;
        LPUART1->CTRL2_B.RXINVEN = BIT_RESET;
    }
}

/*!
 * @brief       Enables the swap Tx/Rx pins
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableSWAPPin(void)
{
    LPUART1->CTRL2_B.SWAPEN = BIT_SET;
}

/*!
 * @brief       Disables the swap Tx/Rx pins
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableSWAPPin(void)
{
    LPUART1->CTRL2_B.SWAPEN = BIT_RESET;
}

/*!
 * @brief       Enables the LPUART DMA interface when reception error occurs
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableOverrunDetection(void)
{
    LPUART1->CTRL3_B.OVRDEDIS = BIT_SET;
}

/*!
 * @brief       Disables the LPUART DMA interface when reception error occurs
 *
 * @param       None
 *
 * @retval      None
 *
 */
void  LPUART_DisableOverrunDetection(void)
{
    LPUART1->CTRL3_B.OVRDEDIS = BIT_RESET;
}

/*!
 * @brief       Enables the stop mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableStopMode(void)
{
    LPUART1->CTRL1_B.USWMEN = BIT_SET;
}

/*!
 * @brief       Disables the stop mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableStopMode(void)
{
    LPUART1->CTRL1_B.USWMEN = BIT_RESET;
}

/*!
 * @brief       Enables the lpuart clock in stop mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableStopModeClock(void)
{
    LPUART1->CTRL3_B.UCLKENS = BIT_SET;
}

/*!
 * @brief       Disables the lpuart clock in stop mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableStopModeClock(void)
{
    LPUART1->CTRL3_B.UCLKENS = BIT_RESET;
}

/*!
 * @brief       Selects the LPUART WakeUp method form stop mode.
 *
 * @param       source: Specifies the selected LPUART wakeup method
 *                      This parameter can be one of the following values:
 *                      @arg LPUART_WAKEUP_SOURCE_ADDRESS:  Wakeup active on address match.
 *                      @arg LPUART_WAKEUP_SOURCE_START: Wakeup active on Start bit detection.
 *                      @arg LPUART_WAKEUP_SOURCE_RXNE:  Wakeup active on RXNE.
 *
 * @retval      None
 *
 */
void LPUART_ConfigStopModeWakeUpSource(LPUART_WAKEUP_SOURCE_T source)
{
    LPUART1->CTRL3_B.WSIFLGSEL = source;
}

/*!
 * @brief       Sets the LPUART node address
 *
 * @param       address
 *
 * @retval      None
 *
 */
void LPUART_Address(uint8_t addressNode)
{
    LPUART1->CTRL2_B.ADDRL = ((uint8_t)addressNode & 0x0F);
    LPUART1->CTRL2_B.ADDRH = (((uint8_t)addressNode>>4) & 0x0F);
}

/*!
 * @brief       Configure the the LPUART Address detection length.
 *
 * @param       address: Specifies the selected LPUART auto baud rate method
 *                       This parameter can be one of the following values:
 *                       @arg LPUART_ADDRESS_MODE_4B: 4-bit address length detection
 *                       @arg LPUART_ADDRESS_MODE_7B: 7-bit address length detection
 *
 * @retval      None
 *
 */
void LPUART_ConfigAddressDetection(LPUART_ADDRESS_MODE_T address)
{
    LPUART1->CTRL2_B.ADDRLEN = address;
}

/*!
 * @brief       Transmit Data
 *
 * @param       data: Specifies the Transmits data value
 *
 * @retval      None
 *
 * @note        The value must less than 0x01FF
 */
void LPUART_TxData(uint16_t data)
{
    LPUART1->TXDATA_B.TXDATA = data;
}

/*!
 * @brief       Received Data
 *
 * @param       None
 *
 * @retval      Returns the received data value
 *
 */
uint16_t LPUART_RxData(void)
{
    return ((uint16_t)LPUART1->RXDATA_B.RXDATA);
}

/*!
 * @brief       Enables the LPUART mute mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableMuteMode(void)
{
    LPUART1->CTRL1_B.RXMUTEEN = BIT_SET;
}

/*!
 * @brief       Disables the LPUART mute mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableMuteMode(void)
{
    LPUART1->CTRL1_B.RXMUTEEN = BIT_RESET;
}

/*!
 * @brief       Selects the LPUART WakeUp method from mute mode
 *
 * @param       wakeup: Specifies the selected LPUART auto baud rate method
 *                      This parameter can be one of the following values:
 *                      @arg LPUART_WAKEUP_IDLE_LINE:    WakeUp by an idle line detection
 *                      @arg LPUART_WAKEUP_ADDRESS_MARK: WakeUp by an address mark
 *
 * @retval      None
 *
 */
void LPUART_ConfigMuteModeWakeUp(LPUART_WAKEUP_T wakeup)
{
    LPUART1->CTRL1_B.WUPMCFG = wakeup;
}

/*!
 * @brief       Enables the LPUART Half-duplex mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableHalfDuplex(void)
{
    LPUART1->CTRL3_B.HDEN = BIT_SET;
}

/*!
 * @brief       Disables the LPUART Half-duplex mode
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableHalfDuplex(void)
{
    LPUART1->CTRL3_B.HDEN = BIT_RESET;
}

/*!
 * @brief       Enables the LPUART DMA interface
 *
 * @param       dmaReq: Specifies the DMA request
 *                      This parameter can be any combination of the following values:
 *                      @arg LPUART_DMA_REQUEST_RX:  LPUART DMA receive request
 *                      @arg LPUART_DMA_REQUEST_TX:  LPUART DMA transmit request
 *                      @arg LPUART_DMA_REQUEST_RX_TX:  LPUART DMA transmit and receive request
 *
 * @retval      None
 *
 */
void LPUART_EnableDMA(LPUART_DMA_REQUEST_T dmaReq)
{
    if (dmaReq == LPUART_DMA_REQUEST_RX)
    {
        LPUART1->CTRL3_B.DMARXEN = BIT_SET;
    }
    else if (dmaReq == LPUART_DMA_REQUEST_TX)
    {
        LPUART1->CTRL3_B.DMATXEN = BIT_SET;
    }
    else if (dmaReq == LPUART_DMA_REQUEST_RX_TX)
    {
        LPUART1->CTRL3_B.DMATXEN = BIT_SET;
        LPUART1->CTRL3_B.DMARXEN = BIT_SET;
    }
}

/*!
 * @brief       Disables the LPUART DMA interface
 *
 * @param       dmaReq: Specifies the DMA request
 *                      This parameter can be any combination of the following values:
 *                      @arg LPUART_DMA_REQUEST_RX:  LPUART DMA receive request
 *                      @arg LPUART_DMA_REQUEST_TX:  LPUART DMA transmit request
 *                      @arg LPUART_DMA_REQUEST_RX_TX:  LPUART DMA transmit and receive request
 *
 * @retval      None
 *
 */
void LPUART_DisableDMA(LPUART_DMA_REQUEST_T dmaReq)
{
    if (dmaReq == LPUART_DMA_REQUEST_RX)
    {
        LPUART1->CTRL3_B.DMARXEN = BIT_RESET;
    }
    else if (dmaReq == LPUART_DMA_REQUEST_TX)
    {
        LPUART1->CTRL3_B.DMATXEN = BIT_RESET;
    }
    else if (dmaReq == LPUART_DMA_REQUEST_RX_TX)
    {
        LPUART1->CTRL3_B.DMATXEN = BIT_RESET;
        LPUART1->CTRL3_B.DMARXEN = BIT_RESET;
    }
}

/*!
 * @brief       Enables the LPUART DMA interface when reception error occurs
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_EnableDMAReceptionError(void)
{
    LPUART1->CTRL3_B.DDISRXEEN = BIT_SET;
}

/*!
 * @brief       Disables the LPUART DMA interface when reception error occurs
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LPUART_DisableDMAReceptionError(void)
{
    LPUART1->CTRL3_B.DDISRXEEN = BIT_RESET;
}

/*!
 * @brief       Enables the specified LPUART's Request.
 *
 * @param       request: specifies the LPUART request
 *              This parameter can be one of the following values:
 *              @arg LPUART_REQUEST_MMQ:  Mute Mode Request
 *              @arg LPUART_REQUEST_RDFQ: Receive data flush Request
 *              @arg LPUART_REQUEST_TDFQ: Transmit data flush Request
 *
 * @retval      None
 *
 */
void LPUART_EnableRequest(LPUART_REQUEST_T request)
{
    if (request == LPUART_REQUEST_MMQ)
    {
        LPUART1->REQUEST_B.MUTEQ = BIT_SET;
    }

    if (request == LPUART_REQUEST_RXDFQ)
    {
        LPUART1->REQUEST_B.RXDFQ = BIT_SET;
    }

    if (request == LPUART_REQUEST_TXBFQ)
    {
        LPUART1->REQUEST_B.TXBFQ = BIT_SET;
    }
}

/*!
 * @brief       Enables the specified interrupts
 *
 * @param       interrupt:  Specifies the LPUART interrupts sources
 *                          The parameter can be one of following values:
 *                          @arg LPUART_INT_WAKEUP:  Wake up interrupt
 *                          @arg LPUART_INT_CMIE:    Character match interrupt
 *                          @arg LPUART_INT_CTSIE:   CTS change interrupt
 *                          @arg LPUART_INT_TXBEIE:  Tansmit Data Register empty interrupt
 *                          @arg LPUART_INT_TXCIE:   Transmission complete interrupt
 *                          @arg LPUART_INT_RXBNEIE: Receive Data register not empty interrupt
 *                          @arg LPUART_INT_IDLEIE:  Idle line detection interrupt
 *                          @arg LPUART_INT_PEIE:    Parity Error interrupt
 *                          @arg LPUART_INT_ERRIE:   Error interrupt
 *
 * @retval      None
 *
 */
void LPUART_EnableInterrupt(LPUART_INT_T interrupt)
{
    if ((interrupt == LPUART_INT_ERRIE ) | (interrupt == LPUART_INT_CTSIE) | (interrupt == LPUART_INT_WAKEUP))
    {
        LPUART1->CTRL3 |= (uint32_t)interrupt;
    }
    else
    {
        LPUART1->CTRL1 |= (uint32_t)interrupt;
    }
}

/*!
 * @brief       Disables the specified interrupts
 *
 * @param       interrupt:  Specifies the LPUART interrupts sources
 *                          The parameter can be one of following values:
 *                          @arg LPUART_INT_WAKEUP:  Wake up interrupt
 *                          @arg LPUART_INT_CMIE:    Character match interrupt
 *                          @arg LPUART_INT_CTSIE:   CTS change interrupt
 *                          @arg LPUART_INT_TXBEIE:  Tansmit Data Register empty interrupt
 *                          @arg LPUART_INT_TXCIE:   Transmission complete interrupt
 *                          @arg LPUART_INT_RXBNEIE: Receive Data register not empty interrupt
 *                          @arg LPUART_INT_IDLEIE:  Idle line detection interrupt
 *                          @arg LPUART_INT_PEIE:    Parity Error interrupt
 *                          @arg LPUART_INT_ERRIE:   Error interrupt
 *
 * @retval      None
 *
 */
void LPUART_DisableInterrupt(LPUART_INT_T interrupt)
{
    if ((interrupt == LPUART_INT_ERRIE ) | (interrupt == LPUART_INT_CTSIE) | (interrupt == LPUART_INT_WAKEUP))
    {
        LPUART1->CTRL3 &= (uint32_t)~interrupt;
    }
    else
    {
        LPUART1->CTRL1 &= (uint32_t)~interrupt;
    }
}

/*!
 * @brief       Read the specified LPUART interrupt flag
 *
 * @param       flag:   Specifies the LPUART interrupt flag to check
 *                      The parameter can be one of following values:
 *                      @arg LPUART_INT_FLAG_WAKEUP: Wake up interrupt flag
 *                      @arg LPUART_INT_FLAG_CMF:    Character match interrupt flag
 *                      @arg LPUART_INT_FLAG_CTSIF:  CTS interrupt fla
 *                      @arg LPUART_INT_FLAG_TXBE:   Transmit data register empty interrupt flag
 *                      @arg LPUART_INT_FLAG_TXC:    Transmission Complete interrupt flag
 *                      @arg LPUART_INT_FLAG_RXBNE:  Receive data buffer not empty interrupt flag
 *                      @arg LPUART_INT_FLAG_IDLE:   Idle Line detection interrupt flag
 *                      @arg LPUART_INT_FLAG_OVRE:   OverRun Error interrupt flag
 *                      @arg LPUART_INT_FLAG_NE:     Noise Error interrupt flag
 *                      @arg LPUART_INT_FLAG_FE:     Framing Error interrupt flag
 *                      @arg LPUART_INT_FLAG_PE:     Parity Error interrupt flag
 *
 * @retval      The new state of flag (SET or RESET)
 *
 */
uint8_t LPUART_ReadIntFlag(LPUART_INT_FLAG_T flag)
{
    uint32_t intEnable = 0;
    uint32_t intFlag = 0;

    if (flag & 0x0E)
    {
        intEnable = LPUART1->CTRL3_B.ERRIEN;
        intFlag = (LPUART1->STS) & flag;
    }
    else if (flag & 0xF0)
    {
        intEnable = (LPUART1->CTRL1) & flag;
        intFlag = (LPUART1->STS) & flag;
    }
    else if (flag & 0x01)
    {
        intEnable = LPUART1->CTRL1_B.PEIEN;
        intFlag = LPUART1->STS_B.PEFLG;
    }
    else if (flag & 0x200)
    {
        intEnable = LPUART1->CTRL3_B.CTSIEN;
        intFlag = LPUART1->STS_B.CTSFLG;
    }
    else if (flag & 0x20000)
    {
        intEnable = LPUART1->CTRL1_B.CMIEN;
        intFlag = LPUART1->STS_B.CMFLG;
    }
    else if (flag & 0x100000)
    {
        intEnable = LPUART1->CTRL3_B.WSMIEN;
        intFlag = LPUART1->STS_B.WSMFLG;
    }

    if (intFlag && intEnable)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clears the LPUART interrupt pending bits
 *
 * @param       flag:   Specifies the LPUART interrupt flag to clear
 *                      The parameter can be any combination following values:
 *                      @arg LPUART_INT_FLAG_WAKEUP: Wake up interrupt flag
 *                      @arg LPUART_INT_FLAG_CMF:    Character match interrupt flag
 *                      @arg LPUART_INT_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg LPUART_INT_FLAG_TXC:    Transmission Complete interrupt flag
 *                      @arg LPUART_INT_FLAG_IDLE:   Idle Line detection interrupt flag
 *                      @arg LPUART_INT_FLAG_OVRE:   OverRun Error interrupt flag
 *                      @arg LPUART_INT_FLAG_NE:     Noise Error interrupt flag
 *                      @arg LPUART_INT_FLAG_FE:     Framing Error interrupt flag
 *                      @arg LPUART_INT_FLAG_PE:     Parity Error interrupt flag
 *
 * @retval      None
 *
 */
void LPUART_ClearIntFlag(LPUART_INT_FLAG_T flag)
{
    LPUART1->INTFCLR |= (uint32_t)flag;
}

/*!
 * @brief       Read the specified LPUART flag
 *
 * @param       flag:   Specifies the flag to check
 *                      The parameter can be one of following values:
 *                      @arg LPUART_FLAG_RXENACKF: Receive Enable Acknowledge Flag
 *                      @arg LPUART_FLAG_TXENACKF: Transmit Enable Acknowledge Flag
 *                      @arg LPUART_FLAG_WAKEUP: Wake Up from stop mode Flag
 *                      @arg LPUART_FLAG_RWF:    Send Break flag
 *                      @arg LPUART_FLAG_SBF:    Send Break flag
 *                      @arg LPUART_FLAG_CMF:    Character match flag
 *                      @arg LPUART_FLAG_BUSY:   Busy flag
 *                      @arg LPUART_FLAG_CTSF:   CTS Change flag
 *                      @arg LPUART_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg LPUART_FLAG_TXBE:   Transmit data register empty flag
 *                      @arg LPUART_FLAG_TXC:    Transmission Complete flag
 *                      @arg LPUART_FLAG_RXBNE:  Receive data buffer not empty flag
 *                      @arg LPUART_FLAG_IDLEF:  Idle Line detection flag
 *                      @arg LPUART_FLAG_OVRE:   OverRun Error flag
 *                      @arg LPUART_FLAG_NEF:    Noise Error flag
 *                      @arg LPUART_FLAG_FEF:    Framing Error flag
 *                      @arg LPUART_FLAG_PEF:    Parity Error flag
 *
 * @retval      The new state of flag (SET or RESET)
 *
 */
uint8_t LPUART_ReadStatusFlag(LPUART_FLAG_T flag)
{
    if ((LPUART1->STS & (uint32_t)flag) != RESET)
    {
        return SET;
    }

    return RESET;
}

/*!
 * @brief       Clear the specified LPUART flag
 *
 * @param       flag:   Specifies the flag to clear
 *                      The parameter can be any combination of following values:
 *                      @arg LPUART_FLAG_WAKEUP: Wake Up from stop mode Flag
 *                      @arg LPUART_FLAG_CMF:    Character match flag
 *                      @arg LPUART_FLAG_CTSIF:  CTS interrupt flag
 *                      @arg LPUART_FLAG_TXC:    Transmission Complete flag
 *                      @arg LPUART_FLAG_IDLEF:  Idle Line detection flag
 *                      @arg LPUART_FLAG_OVRE:   OverRun Error flag
 *                      @arg LPUART_FLAG_NEF:    Noise Error flag
 *                      @arg LPUART_FLAG_FEF:    Framing Error flag
 *                      @arg LPUART_FLAG_PEF:    Parity Error flag
 *
 * @retval      Note
 *
 */
void LPUART_ClearStatusFlag(LPUART_FLAG_T flag)
{
    LPUART1->INTFCLR = (uint32_t)flag;
}

/**@} end of group LPUART_Fuctions*/
/**@} end of group LPUART_Driver*/
/**@} end of group Peripherals_Library*/
