/*!
 * @file        apm32l0xx_lpuart.h
 *
 * @brief       This file contains all the functions prototypes for the LPUART firmware library
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

#ifndef __APM32L0xx_LPUART_H
#define __APM32L0xx_LPUART_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apm32l0xx.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup LPUART_Driver LPUART Driver
  @{
*/

/** @addtogroup LPUART_Enumerations Enumerations
  @{
*/

/**
 * @brief   LPUART Word Length define
 */
typedef enum
{
    LPUART_WORD_LEN_8B = 0,
    LPUART_WORD_LEN_9B = BIT12,
    LPUART_WORD_LEN_7B = BIT28
} LPUART_WORD_LEN_T;

/**
 * @brief   LPUART Parity define
 */
typedef enum
{
    LPUART_PARITY_NONE   = 0,
    LPUART_PARITY_EVEN   = BIT10,
    LPUART_PARITY_ODD    = BIT10 | BIT9
} LPUART_PARITY_T;

/**
 * @brief   LPUART mode define
 */
typedef enum
{
    LPUART_MODE_RX = BIT2,
    LPUART_MODE_TX = BIT3,
    LPUART_MODE_TX_RX = BIT2 | BIT3
} LPUART_MODE_T;

/**
 * @brief   LPUART Stop bits define
 */
typedef enum
{
    LPUART_STOP_BITS_1    = 0,
    LPUART_STOP_BITS_2    = BIT13
} LPUART_STOP_BITS_T;

/**
 * @brief   LPUART hardware flow control define
 */
typedef enum
{
    LPUART_FLOW_CTRL_NONE    = 0,
    LPUART_FLOW_CTRL_RTS     = BIT8,
    LPUART_FLOW_CTRL_CTS     = BIT9,
    LPUART_FLOW_CTRL_RTS_CTS = BIT8 | BIT9
} LPUART_FLOW_CTRL_T;

/**
 * @brief    LPUART inversion Pins
 */
typedef enum
{
    LPUART_INVERSION_RX,
    LPUART_INVERSION_TX,
    LPUART_INVERSION_TX_RX
} LPUART_INVERSION_T;

/**
 * @brief   Wakeup from Stop mode interrupt flag selection
 */
typedef enum
{
    LPUART_WAKEUP_SOURCE_ADDRESS = 0x00,
    LPUART_WAKEUP_SOURCE_START   = 0x02,
    LPUART_WAKEUP_SOURCE_RXNE    = 0x03
} LPUART_WAKEUP_SOURCE_T;

/**
 * @brief    LPUART address mode
 */
typedef enum
{
    LPUART_ADDRESS_MODE_4B,
    LPUART_ADDRESS_MODE_7B
} LPUART_ADDRESS_MODE_T;

/**
 * @brief    LPUART wakeup method
 */
typedef enum
{
    LPUART_WAKEUP_IDLE_LINE,
    LPUART_WAKEUP_ADDRESS_MARK
} LPUART_WAKEUP_T;

/**
 * @brief   LPUART DMA requests
 */
typedef enum
{
    LPUART_DMA_REQUEST_RX,
    LPUART_DMA_REQUEST_TX,
    LPUART_DMA_REQUEST_RX_TX
} LPUART_DMA_REQUEST_T;

/**
 * @brief   LPUART request
 */
typedef enum
{
    LPUART_REQUEST_MMQ,           //!< Mute Mode Request
    LPUART_REQUEST_RXDFQ,         //!< Receive data flush Request
    LPUART_REQUEST_TXBFQ          //!< Transmit data flush Request
} LPUART_REQUEST_T;

/**
 * @brief   LPUART interrupts source
 */
typedef enum
{
    LPUART_INT_WAKEUP       = BIT22,  //!< Wake up interrupt
    LPUART_INT_CMIE         = BIT14,  //!< Character match interrupt
    LPUART_INT_CTSIE        = BIT10,  //!< CTS change interrupt
    LPUART_INT_TXBEIE       = BIT7,   //!< Tansmit Data Register empty interrupt
    LPUART_INT_TXCIE        = BIT6,   //!< Transmission complete interrupt
    LPUART_INT_RXBNEIE      = BIT5,   //!< Receive Data buffer not empty interrupt
    LPUART_INT_IDLEIE       = BIT4,   //!< Idle line detection interrupt
    LPUART_INT_PEIE         = BIT8,   //!< Parity Error interrupt
    LPUART_INT_ERRIE        = BIT0    //!< Error interrupt
} LPUART_INT_T;

/**
 * @brief   LPUART Interrupt flag definition
 */
typedef enum
{
    LPUART_INT_FLAG_WAKEUP  = BIT20,  //!< Wake up flag
    LPUART_INT_FLAG_CMF     = BIT17,  //!< Character match flag
    LPUART_INT_FLAG_CTSIF   = BIT9,   //!< CTS interrupt flag
    LPUART_INT_FLAG_TXBE    = BIT7,   //!< Transmit data register empty flag
    LPUART_INT_FLAG_TXC     = BIT6,   //!< Transmission Complete flag
    LPUART_INT_FLAG_RXBNE   = BIT5,   //!< Receive data buffer not empty flag
    LPUART_INT_FLAG_IDLE    = BIT4,   //!< Idle Line detection flag
    LPUART_INT_FLAG_OVRE    = BIT3,   //!< OverRun Error flag
    LPUART_INT_FLAG_NE      = BIT2,   //!< Noise Error flag
    LPUART_INT_FLAG_FE      = BIT1,   //!< Framing Error flag
    LPUART_INT_FLAG_PE      = BIT0    //!< Parity Error flag
} LPUART_INT_FLAG_T;

/**
 * @brief    LPUART flag definition
 */
typedef enum
{
    LPUART_FLAG_RXENACKF    = BIT22,  //!< Receive Enable Acknowledge Flag
    LPUART_FLAG_TXENACKF    = BIT21,  //!< Transmit Enable Acknowledge Flag
    LPUART_FLAG_WAKEUP      = BIT20,  //!< Wake Up from stop mode Flag
    LPUART_FLAG_RWF         = BIT19,  //!< Send Break flag
    LPUART_FLAG_SBF         = BIT18,  //!< Send Break flag
    LPUART_FLAG_CMF         = BIT17,  //!< Character match flag
    LPUART_FLAG_BUSY        = BIT16,  //!< Busy flag
    LPUART_FLAG_CTSF        = BIT10,  //!< CTS Change flag
    LPUART_FLAG_CTSIF       = BIT9,   //!< CTS interrupt flag
    LPUART_FLAG_TXBE        = BIT7,   //!< Transmit data register empty flag
    LPUART_FLAG_TXC         = BIT6,   //!< Transmission Complete flag
    LPUART_FLAG_RXBNE       = BIT5,   //!< Receive data buffer not empty flag
    LPUART_FLAG_IDLEF       = BIT4,   //!< Idle Line detection flag
    LPUART_FLAG_OVRE        = BIT3,   //!< OverRun Error flag
    LPUART_FLAG_NEF         = BIT2,   //!< Noise Error flag
    LPUART_FLAG_FEF         = BIT1,   //!< Framing Error flag
    LPUART_FLAG_PEF         = BIT0    //!< Parity Error flag
} LPUART_FLAG_T;

/**@} end of group LPUART_Enumerations*/

/** @addtogroup LPUART_Structure Data Structure
  @{
*/

/**
 * @brief   LPUART Config struct definition
 */
typedef struct
{
    uint32_t                     baudRate;           //!< Specifies the baud rate
    LPUART_WORD_LEN_T            wordLength;         //!< Specifies the word length
    LPUART_STOP_BITS_T           stopBits;           //!< Specifies the stop bits
    LPUART_PARITY_T              parity;             //!< Specifies the parity
    LPUART_MODE_T                mode;               //!< Specifies the mode
    LPUART_FLOW_CTRL_T           hardwareFlowCtrl;   //!< Specifies the hardware flow control
} LPUART_Config_T;

/**@} end of group LPUART_Structure*/

/** @addtogroup LPUART_Fuctions Fuctions
  @{
*/

/** LPUART peripheral Reset and Configuration */
void LPUART_Reset(void);
void LPUART_Config(LPUART_Config_T* configStruct);
void LPUART_ConfigStructInit(LPUART_Config_T* configStruct);
void LPUART_Enable(void);
void LPUART_Disable(void);
void LPUART_EnableDirectionMode(LPUART_MODE_T mode);
void LPUART_DisableDirectionMode(LPUART_MODE_T mode);
void LPUART_EnableMSBFirst(void);
void LPUART_DisableMSBFirst(void);
void LPUART_EnableDataInv(void);
void LPUART_DisableDataInv(void);
void LPUART_EnableInvPin(LPUART_INVERSION_T invPin);
void LPUART_DisableInvPin(LPUART_INVERSION_T invPin);
void LPUART_EnableSWAPPin(void);
void LPUART_DisableSWAPPin(void);
void LPUART_EnableOverrunDetection(void);
void LPUART_DisableOverrunDetection(void);

/** Stop mode */
void LPUART_EnableStopMode(void);
void LPUART_DisableStopMode(void);
void LPUART_EnableStopModeClock(void);
void LPUART_DisableStopModeClock(void);
void LPUART_ConfigStopModeWakeUpSource(LPUART_WAKEUP_SOURCE_T source);

/** Address */
void LPUART_Address(uint8_t addressNode);
void LPUART_ConfigAddressDetection(LPUART_ADDRESS_MODE_T address);

/** Transmit and receive */
void LPUART_TxData(uint16_t data);
uint16_t LPUART_RxData(void);

/** Mute mode */
void LPUART_EnableMuteMode(void);
void LPUART_DisableMuteMode(void);
void LPUART_ConfigMuteModeWakeUp(LPUART_WAKEUP_T wakeup);

/** Half-duplex mode  */
void LPUART_EnableHalfDuplex(void);
void LPUART_DisableHalfDuplex(void);

/** DMA */
void LPUART_EnableDMA(LPUART_DMA_REQUEST_T dmaReq);
void LPUART_DisableDMA(LPUART_DMA_REQUEST_T dmaReq);
void LPUART_EnableDMAReceptionError(void);
void LPUART_DisableDMAReceptionError(void);

/** Request */
void LPUART_EnableRequest(LPUART_REQUEST_T request);

/** Interrupt */
void LPUART_EnableInterrupt(LPUART_INT_T interrupt);
void LPUART_DisableInterrupt(LPUART_INT_T interrupt);
uint8_t LPUART_ReadIntFlag(LPUART_INT_FLAG_T flag);
void LPUART_ClearIntFlag(LPUART_INT_FLAG_T flag);

/** Flag */
uint8_t LPUART_ReadStatusFlag(LPUART_FLAG_T flag);
void LPUART_ClearStatusFlag(LPUART_FLAG_T flag);

/**@} end of group LPUART_Fuctions*/
/**@} end of group LPUART_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __APM32L0xx_LPUART_H */
