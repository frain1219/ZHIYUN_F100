/*!
 * @file       main.c
 *
 * @brief      Main program body
 *
 * @version    V1.0.0
 *
 * @date       2022-12-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
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
#include "Board.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "list.h"

/** @addtogroup Examples
  * @{
*/

/** @addtogroup FreeRTOS
  * @{
*/

/** @defgroup FreeRTOS_Macros Macros
  @{
*/

#define DATA_BUF_SIZE       (32)

/**@} end of group FreeRTOS_Macros */

/** @defgroup FreeRTOS_Functions Functions
  * @{
*/

typedef enum {FALSE, TRUE} BOOL;

/* USART1 receive buffer */
uint8_t rxDataBufUSART1[DATA_BUF_SIZE] = {0};
/* USART1 transmit buffer */
uint8_t txDataBufUSART1[DATA_BUF_SIZE] = {0};

/* USART2 receive buffer */
uint8_t rxDataBufUSART2[DATA_BUF_SIZE] = {0};
/* USART2 transmit buffer */
uint8_t txDataBufUSART2[DATA_BUF_SIZE] = {0};

/* USART Write data */
void USART_Write(USART_T* usart,uint8_t *dat);

/* USART Init */
void USART_Init(void);
/* Delay */
void Delay(uint32_t count);
/* Buffer compare*/
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size);

/* Task handle */
static TaskHandle_t xHandleTaskLedToggle = NULL;
static TaskHandle_t xHandleTaskUsartTest = NULL;

/* User create task */
static void UserTaskCreate(void);

/* Led toggle task */
void vTaskLedToggle(void *pvParameters);
/* Usart test task */
void vTaskUsartTest(void *pvParameters);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    /* System clocks configuration */
    SystemClockConfig();

    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /* USART Initialization */
    USART_Init();

    /* User create task */
    UserTaskCreate();

    /* Startup FreeRTOS */
    vTaskStartScheduler();

    while(1);
}

/*!
 * @brief       User Create Task
 *
 * @param       None
 *
 * @retval      None
 */
static void UserTaskCreate(void)
{
    xTaskCreate(vTaskLedToggle,
                "TaskLedToggle",
                 128,
                 NULL,
                 0,
                 &xHandleTaskLedToggle);

    xTaskCreate(vTaskUsartTest,
                "TaskUsartTest",
                128,
                NULL,
                2,
                &xHandleTaskUsartTest);
}

/*!
 * @brief       Led toggle task
 *
 * @param       pvParameters - passed into the task function as the function parameters
 *
 * @retval      None
 */
void vTaskLedToggle(void *pvParameters)
{
    while(1)
    {
        /* Toggle LED2 */
        BOARD_LEDToggle(LED2);
        /* Task blocking time Delay */
        vTaskDelay(500);
    }
}

/*!
 * @brief       Led toggle task
 *
 * @param       pvParameters - passed into the task function as the function parameters
 *
 * @retval      None
 */
void vTaskUsartTest(void *pvParameters)
{
    BOOL state = TRUE;
    int timeout = 0;
    /* Init txDataBufUSART2 */
    for(int i = 0; i < DATA_BUF_SIZE; i++)
    {
        txDataBufUSART1[i] = i;
        txDataBufUSART2[i] = i;
    }

    while(1)
    {
        /* Send Data from USART2 to USART1 */
        timeout = 0;
        for(int i = 0; i < DATA_BUF_SIZE; i++)
        {
            /* Wait until end of transmission */
            while(USART_ReadStatusFlag(COM1_PORT, USART_FLAG_TXBE) == RESET)
            {
                vTaskDelay(10);
                timeout++;
                if(timeout > 10)
                {
                    break;
                }
            }

            USART_TxData(COM1_PORT, txDataBufUSART1[i]);

            /* Wait until end of reception */
            while(USART_ReadStatusFlag(COM2_PORT, USART_FLAG_RXBNE) == RESET)
            {
                vTaskDelay(10);
                timeout++;
                if(timeout > 10)
                {
                    break;
                }
            }

            rxDataBufUSART2[i] = USART_RxData(COM2_PORT);
        }

         /* if send and received data pass, LED3 will be on all the time.
if send and received data fault, LED3 will be off all the time. data */
        state = BufferCompare(rxDataBufUSART2, txDataBufUSART1, DATA_BUF_SIZE);

        /* Data is ok then turn off LED3 */
        if(state == TRUE)
        {
            BOARD_LEDOn(LED3);
        }
        else
        {
            BOARD_LEDOff(LED3);
        }

        /* Task blocking time Delay */
        vTaskDelay(1000);

        timeout = 0;
        /* Send Data from USART2 to USART1 */
        for(int i = 0; i < DATA_BUF_SIZE; i++)
        {
            /* Wait until end of transmission */
            while(USART_ReadStatusFlag(COM2_PORT, USART_FLAG_TXBE) == RESET)
            {
                vTaskDelay(10);
                timeout++;
                if(timeout > 10)
                {
                    break;
                }
            }

            USART_TxData(COM2_PORT, txDataBufUSART2[i]);

            /* Wait until end of reception */
            while(USART_ReadStatusFlag(COM1_PORT, USART_FLAG_RXBNE) == RESET)
            {
                vTaskDelay(10);
                timeout++;
                if(timeout > 10)
                {
                    break;
                }
            }

            rxDataBufUSART1[i] = USART_RxData(COM1_PORT);
        }

         /* Verify data */
        state = BufferCompare(rxDataBufUSART1, txDataBufUSART2, DATA_BUF_SIZE);

        /* Data is ok then turn off LED3 */
        if(state == TRUE)
        {
            BOARD_LEDOn(LED3);
        }
        else
        {
            BOARD_LEDOff(LED3);
        }

        /* Task blocking time Delay */
        vTaskDelay(1000);
    }
}

/*!
 * @brief       Compares two buffers
 *
 * @param       buf1:    First buffer to be compared
 *
 * @param       buf1:    Second buffer to be compared
 *
 * @param       size:    Buffer size
 *
 * @retval      Return SET if buf1 = buf2. If not then return RESET
 */
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size)
{
    uint8_t i;

    for(i = 0; i < size; i++)
    {
        if(buf1[i] != buf2[i])
        {
            return FALSE;
        }
    }

    return TRUE;
}

/*!
 * @brief       USARTS Initialization
 *
 * @param       None
 *
 * @retval      None
 */
void USART_Init(void)
{
    USART_Config_T usartConfigStruct;

    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.hardwareFlow = USART_HARDWARE_FLOW_NONE;
    usartConfigStruct.mode = USART_MODE_TX_RX;
    usartConfigStruct.parity = USART_PARITY_NONE;
    usartConfigStruct.stopBits = USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;

    BOARD_COMInit(COM1, &usartConfigStruct);
    BOARD_COMInit(COM2, &usartConfigStruct);

    Delay(0x7FFF);

    /* Disable USART1 RXBNE interrput */
    USART_DisableInterrupt(COM1_PORT,USART_INT_RXBNE);
    USART_DisableInterrupt(COM2_PORT,USART_INT_RXBNE);

    /* Disable USART2 RXBNE interrput */
    USART_ClearStatusFlag(COM1_PORT, USART_FLAG_RXBNE);
    USART_ClearStatusFlag(COM2_PORT, USART_FLAG_RXBNE);
}

/*!
 * @brief       Delay
 *
 * @param       count:  delay count
 *
 * @retval      None
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;
    while(delay--);
}

/**@} end of group FreeRTOS_Functions */
/**@} end of group FreeRTOS */
/**@} end of group Examples */
