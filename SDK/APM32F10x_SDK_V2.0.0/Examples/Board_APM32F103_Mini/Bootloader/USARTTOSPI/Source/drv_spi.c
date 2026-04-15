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

/* Includes */
#include "drv_spi.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOSPI
  @{
*/

/** @defgroup USARTTOSPI_Variables Variables
  @{
*/

uint32_t systickCnt = 0;

/**@} end of group USARTTOSPI_Variables */

/** @defgroup USARTTOSPI_Functions Functions
  @{
*/

/*!
 * @brief       SPI GPIO Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Drv_SPI_GPIO_Init(void)
{
    GPIO_Config_T gpioConfig;

    /* Enable related Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_GPIOA | RCM_APB2_PERIPH_GPIOB);

    /* config PIN_6  MISO*/
    gpioConfig.pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;;
    gpioConfig.mode = GPIO_MODE_AF_PP;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &gpioConfig);

    gpioConfig.pin =  GPIO_PIN_4;
    gpioConfig.mode = GPIO_MODE_OUT_PP;
    GPIO_Config(GPIOA, &gpioConfig);
    GPIO_SetBit(GPIOA, GPIO_PIN_4);
}

/*!
 * @brief       SPI Init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Drv_SPI_Init(void)
{
    SPI_Config_T spiConfig;

    SPI_ConfigStructInit(&spiConfig);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_SPI2);

    spiConfig.length = SPI_DATA_LENGTH_8B;

    spiConfig.baudrateDiv = SPI_BAUDRATE_DIV_8;
    /*  2 line full duplex  */
    spiConfig.direction = SPI_DIRECTION_2LINES_FULLDUPLEX;
    /*  LSB first  */
    spiConfig.firstBit = SPI_FIRSTBIT_MSB;
    /*  Slave mode  */
    spiConfig.mode = SPI_MODE_MASTER;
    /*  Polarity is low  */
    spiConfig.polarity = SPI_CLKPOL_LOW;
    /*  Software select slave enable  */
    spiConfig.nss = SPI_NSS_SOFT;
    /*  Phase is 1 edge  */
    spiConfig.phase = SPI_CLKPHA_1EDGE;
    /*  SPI config  */
    SPI_Config(SPI1, &spiConfig);
    SPI_SetSoftwareNSS(SPI1);
    SPI_ConfigDataSize(SPI1, SPI_DATA_LENGTH_8B);
    SPI_Enable(SPI1);
}

/*!
 * @brief       SPI transmit/receive data buffer
 *
 * @param       txData: tx data address
 *
 * @param       rxData: rx data address
 *
 * @param       len: tx/rx data length
 *
 * @retval      None
 *
 */
void Drv_SPI_TxRx(uint8_t *txData, uint8_t *rxData, uint32_t len)
{
    uint32_t i;

    for (i = 0; i < len; i++)
    {
        while (!SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE));
        if (txData)
        {
            SPI_I2S_TxData(SPI1, txData[i]);
        }
        else
        {
            SPI_I2S_TxData(SPI1, 0);
        }

        while (!SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE));
        if (rxData)
        {
            rxData[i] = SPI_I2S_RxData(SPI1);
        }
        else
        {
            SPI_I2S_RxData(SPI1);
        }
    }
}

/*!
 * @brief       SPI transmit/receive byte
 *
 * @param       txData: tx data byte
 *
 * @retval      None
 *
 */
uint8_t Drv_SPI_TxRxByte(uint8_t txData)
{
    uint8_t rxData;
    volatile uint32_t timeOut = 0;

SEND:
    while (!SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE));
    SPI_I2S_TxData(SPI1, txData);

    while (!SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE))
    {
        if (timeOut++ > 0x100000)
        {
            goto SEND;
        }
    }
    rxData = SPI_I2S_RxData(SPI1);

    return rxData;
}

/*!
 * @brief       SPI receive data buffer
 *
 * @param       rxData: rx data buffer address
 *
 * @param       len: rx data buffer length
 *
 * @retval      None
 *
 */
void Drv_SPI_Read(uint8_t *rxData, uint32_t len)
{
    uint32_t i;
    /* Before reading the data, discard the first synchronization byte. */
    Drv_SPI_TxRxByte(0);

    for (i = 0; i < len; i++)
    {
        while (!SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_TXBE));
        SPI_I2S_TxData(SPI1, 0);

        while (!SPI_I2S_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE));
        rxData[i] = SPI_I2S_RxData(SPI1);
    }
}

/*!
 * @brief       SPI wait for ACK
 *
 * @param       None
 *
 * @retval      None
 *
 */
uint8_t Drv_SPI_WaitACK(void)
{
    uint8_t temp = 0;

    Drv_SPI_TxRxByte(0x00);
    /* delay 20s */
    systickCnt = 20000;
    while (1)
    {
        temp = Drv_SPI_TxRxByte(0x00);
        if ((temp == DATA_FRAME_ACK) || (temp == DATA_FRAME_NACK))
        {
            Drv_SPI_TxRxByte(DATA_FRAME_ACK);
            return temp;
        }
        if (!systickCnt)
        return SPI_STATUS_ERROR;
    }
}

/*!
 * @brief       SPI Connection Handling
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SPI_Connect(void)
{
    GPIO_ResetBit(GPIOA, GPIO_PIN_4);
    while (1)
    {
        if (Drv_SPI_TxRxByte(0x5A) == 0xA5)
        {
            break;
        }
    }
    while (Drv_SPI_WaitACK() == SPI_STATUS_ERROR);
    GPIO_SetBit(GPIOA, GPIO_PIN_4);
}

/*!
 * @brief       SPI Connection Handling
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Drv_SPI_Delay(uint32_t cnt)
{
    SysTick_Config(SystemCoreClock / 1000);
    systickCnt = cnt;

    while(systickCnt)
    {
        __NOP();
    }
}

/**@} end of group USARTTOSPI_Functions */
/**@} end of group USARTTOSPI */
/**@} end of group Examples */

