/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "main.h"
#include "config.h"
#include "drv_spi.h"
#include "Board.h"
#include "uart.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup USARTTOSPI
  @{
  */

/** @defgroup USARTTOSPI_Macros Macros
  @{
*/

#define Is_ACK()  \
do                                             \
{                                              \
    switch (Drv_SPI_WaitACK())                 \
    {                                          \
        case DATA_FRAME_ACK:                   \
            USART_ACK();                    \
            break;                             \
        case DATA_FRAME_NACK:                  \
            USART_SendByte(0x1F);           \
            goto START;                        \
        default:                               \
            break;                             \
    }                                          \
} while(0)

/**@} end of group USARTTOSPI_Macros*/

/** @defgroup USARTTOSPI_Variables Variables
  @{
  */

uint32_t freq;
uint8_t spi_connect = 0;

/** 协议用到的缓存 */
uint8_t buf_spi_tx[512];
uint8_t buf_spi_rx[512];
uint8_t buf_data[256];

/**@} end of group USARTTOSPI_Variables*/

/** @defgroup USARTTOSPI_Functions
  @{
  */

void SPI_Protocol(void);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    /* System clocks configuration */
    SystemClockConfig();

    USART_Init();

    BOARD_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    BOARD_PBInit(BUTTON_KEY2, BUTTON_MODE_EINT);
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);
    BOARD_LEDOff(LED2);
    BOARD_LEDOff(LED3);

    Drv_SPI_GPIO_Init();
    Drv_SPI_Init();

    USART_Wait7F();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
    SPI_Connect();

    while(1)
    {
        SPI_Protocol();
    }
}

void SPI_Protocol(void)
{
    uint8_t cmd = 0xFF;
    uint32_t len, addr;

START:
    USART_Read(buf_data, 1);

    if (buf_data[0] == 0x7F)
    {
        buf_data[0] = 0x79;
        USART_Write(buf_data, 1);
        goto START;
    }

    USART_Read(&buf_data[1], 1);

    if ((buf_data[1] ^ buf_data[0]) != 0xFF)
    {
        buf_data[1] = 0x1F;
        USART_Write(buf_data, 1);
    }

    cmd = buf_data[0];

    GPIO_ResetBit(GPIOA, GPIO_PIN_4);

    Drv_SPI_TxRxByte(DATA_FRAME_SOF_MASTER);
    Drv_SPI_TxRxByte(cmd);
    Drv_SPI_TxRxByte(~cmd);

    Is_ACK();

    switch (cmd)
    {
        /* Get */
        case CMD_SPI_GET:
            Drv_SPI_Read(buf_spi_rx, 13);
            /* Disguised as Serial Port V3 Version */
            buf_spi_rx[1] = 0x31;
            USART_Write(buf_spi_rx, 13);
            goto ACK;

        /* Get Version */
        case CMD_SPI_GET_VERSION:
            Drv_SPI_TxRxByte(0);
            buf_spi_rx[1] = Drv_SPI_TxRxByte(0);
            /* Disguised as Serial Port V3 Version */
            buf_spi_rx[1] = 0x31;
            buf_spi_rx[2] = 0x0;
            buf_spi_rx[3] = 0x0;
            buf_spi_rx[4] = 0x79;
            USART_Write(&buf_spi_rx[1], 4);
            goto ACK;

        /* Get ID */
        case CMD_SPI_GET_ID:
            Drv_SPI_Read(buf_spi_rx, 3);
            USART_Write(buf_spi_rx, 3);
            goto ACK;

        /* Read */
        case CMD_SPI_READ_MEMORY:
            /* address */
            USART_Read(buf_data, 5);
            Drv_SPI_Write(buf_data, 5);
            Is_ACK();

            /* length */
            USART_Read(buf_data, 2);
            Drv_SPI_TxRxByte(buf_data[0]);
            Drv_SPI_TxRxByte(buf_data[1]);
            Is_ACK();
            len = buf_data[0] + 1;

            /* data */
            Drv_SPI_Read(buf_data, len);
            USART_Write(buf_data, len);
            break;
        /* Go */
        case CMD_SPI_GO:
            /* address */
            USART_Read(buf_data, 5);
            Drv_SPI_Write(buf_data, 5);
            Is_ACK();
            Is_ACK();
            goto RESET;
        /* Write */
        case CMD_SPI_WRITE_MEMORY:
            /* address */
            USART_Read(buf_data, 5);
            Drv_SPI_Write(buf_data, 5);
            addr = BYTE_TO_WORD(buf_data);
            Is_ACK();

            /* length */
            USART_Read(buf_data, 1);
            Drv_SPI_Write(buf_data, 1);
            len = buf_data[0] + 1;

            USART_Read(buf_data, len + 1);
            Drv_SPI_Write(buf_data, len + 1);
            if (addr == 0x1FFFF800)
            {
                Is_ACK();
                goto RESET;
            }
            goto ACK;
        case CMD_SPI_ERASE:
            /* page number */
            USART_Read(buf_data, 2);
            len = (buf_data[1] << 8)| buf_data[0];
            if ((len & 0xFFF0) == 0xFFF0)
            {
                USART_Read(&buf_data[2], 1);
                Drv_SPI_Write(buf_data, 3);

                if (Drv_SPI_WaitACK() == SPI_STATUS_ERROR)
                {
                    goto NACK;
                }
                USART_ACK();

                break;
            }
            /* page erase */
            else
            {
				buf_data[2] = 0x00;
				Drv_SPI_Write(buf_data, 3);
				Drv_SPI_WaitACK();
                /* page number */
                len = ((((buf_data[0] << 8) | buf_data[1]) +1)<<1);
                USART_Read(&buf_data[2], len + 1); //!< page number + check number
                Drv_SPI_Write(&buf_data[2], len + 1);
                Drv_SPI_Delay(500);
                goto ACK;
            }
        case CMD_SPI_WRITE_PROTECT:
            /* page number */
			buf_data[0] = 0X2;
			buf_data[1] = 0XFD;
            Drv_SPI_Write(buf_data, 2);
            len = buf_data[0] + 1;
            Is_ACK();
			buf_data[0] = 0X0;
			buf_data[1] = 0X1;
			buf_data[2] = 0X2;
			buf_data[3] = 0X3;
            Drv_SPI_Write(buf_data, len + 1);
            Is_ACK();
            goto RESET;
        case CMD_SPI_WRITE_UNPROTECT:
        case CMD_SPI_READ_PROTECT:
        case CMD_SPI_READ_UNPROTECT:
            Is_ACK();
            goto RESET;
        default:
            goto NACK;
    }
    GPIO_SetBit(GPIOA, GPIO_PIN_4);
    return;
ACK:
    Is_ACK();

    GPIO_SetBit(GPIOA, GPIO_PIN_4);
    return;
NACK:
    USART_SendByte(0x1F);
    return;
RESET:
    USART_Wait7F();
    SPI_Connect();
}

/**@} end of group USARTTOSPI_Functions */
/**@} end of group USARTTOSPI */
/**@} end of group Examples */
