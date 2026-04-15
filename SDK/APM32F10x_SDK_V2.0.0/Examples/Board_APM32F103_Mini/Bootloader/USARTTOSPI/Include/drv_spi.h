/*!
 * @file        drv_spi.c
 *
 * @brief       spi driver
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
/* Define to prevent recursive inclusion */
#ifndef  __DRV_SPI
#define  __DRV_SPI

/* Includes */
#include "config.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOSPI
  @{
*/

/** @defgroup USARTTOSPI_Macros Macros
  @{
*/

#define BYTE_TO_WORD(pByte)   (((uint32_t)pByte[0] << 24) | ((uint32_t)pByte[1] << 16) | ((uint32_t)pByte[2] << 8) | ((uint32_t)pByte[3]))

#define Drv_SPI_Write(txData, len)   Drv_SPI_TxRx(txData, (void*)0, len)

/**@} end of group USARTTOSPI_Macros */

/** @defgroup USARTTOSPI_Enumerates Enumerates
  @{
*/

enum
{
    DATA_FRAME_SOF_MASTER = 0x5A,
    DATA_FRAME_SOF_SLAVE  = 0xA5,
    DATA_FRAME_TEMP_00    = 0x00,
    DATA_FRAME_ACK        = 0x79,
    DATA_FRAME_NACK       = 0x1F,
};

typedef enum
{
    CMD_SPI_GET             = 0x00,
    CMD_SPI_GET_VERSION     = 0x01,
    CMD_SPI_GET_ID          = 0x02,
    CMD_SPI_READ_MEMORY     = 0x11,
    CMD_SPI_GO              = 0x21,
    CMD_SPI_WRITE_MEMORY    = 0x31,
    CMD_SPI_ERASE           = 0x44,
    CMD_SPI_WRITE_PROTECT   = 0x63,
    CMD_SPI_WRITE_UNPROTECT = 0x73,
    CMD_SPI_READ_PROTECT    = 0x82,
    CMD_SPI_READ_UNPROTECT  = 0x92
} CMD_SPI_T;

enum
{
    SPI_STATUS_ACK,
    SPI_STATUS_NACK,
    SPI_STATUS_ERROR,
};

/**@} end of group USARTTOSPI_Enumerates */

/** @defgroup USARTTOSPI_Enumerates Variables
  @{
*/

extern uint32_t systickCnt;

/**@} end of group USARTTOSPI_Variables */

/** @defgroup USARTTOSPI_Functions Functions
  @{
*/

void Drv_SPI_GPIO_Init(void);
void Drv_SPI_Init(void);
uint8_t Drv_SPI_WaitACK(void);
void Drv_SPI_TxRx(uint8_t *txData, uint8_t *rxData, uint32_t len);
void Drv_SPI_Read(uint8_t *rxData, uint32_t len);
uint8_t Drv_SPI_TxRxByte(uint8_t txData);
void SPI_Connect(void);
void Drv_SPI_Delay(uint32_t cnt);

/**@} end of group USARTTOSPI_Functions */
/**@} end of group USARTTOSPI */
/**@} end of group Examples */

#endif
