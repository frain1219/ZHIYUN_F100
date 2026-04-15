/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2022-12-01
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

/** @addtogroup Examples
  * @{
*/

/** @addtogroup RT5
  * @{
*/

/** @defgroup RT5_Macros Macros
  * @{
*/

#define DATA_BUF_SIZE       (32)

/**@} end of group RT5_Macros */

/** @defgroup RT5_Enumerations Enumerations
  * @{
*/

typedef enum {FALSE, TRUE} BOOL;

/**@} end of group RT5_Enumerations */

/** @defgroup RT5_Variables Variables
  * @{
*/

/* USART1 receive buffer */
uint8_t rxDataBufUSART1[DATA_BUF_SIZE] = {0};
/* USART1 transmit buffer */
uint8_t txDataBufUSART1[DATA_BUF_SIZE] = {0};

/* USART2 receive buffer */
uint8_t rxDataBufUSART2[DATA_BUF_SIZE] = {0};
/* USART2 transmit buffer */
uint8_t txDataBufUSART2[DATA_BUF_SIZE] = {0};

const osThreadAttr_t ThreadStart_Attr =
{
    .name = "osRtxStartThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityHigh4,
    .stack_size = 2048,
};

const osThreadAttr_t ThreadLedToggle_Attr =
{
    .name = "osRtxLedToggleThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityHigh2,
    .stack_size = 512,
};

const osThreadAttr_t ThreadUsart_Attr =
{
    .name = "osRtxUsartThread",
    .attr_bits = osThreadDetached,
    .priority = osPriorityHigh1,
    .stack_size = 1024,
};

/* Task handle */
osThreadId_t ThreadIdStart = NULL;
osThreadId_t ThreadIdTaskUsart = NULL;
osThreadId_t ThreadIdTaskLedToggle = NULL;

/**@} end of group RT5_Variables */

/** @defgroup RT5_Functions Functions
  * @{
*/

/* USART Write data */
void USART_Write(USART_T* usart,uint8_t *dat);

/* Delay */
void Delay(uint32_t count);

/* USART Init */
void USART_Init(void);

/* Buffer compare*/
BOOL BufferCompare(uint8_t *buf1, uint8_t *buf2, uint8_t size);

/* User create task */
static void xTaskUserCreate (void);

void UserTaskInit(void* pvParameters);

/* Led toggle task */
void vTaskLedToggle(void* pvParameters);

/* Usart test task */
void vTaskUsartTest(void* pvParameters);

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

    /* Initialize the RTOS Kernel */
    osKernelInitialize();

    /* Create a thread and add it to Active Threads */
    ThreadIdStart = osThreadNew(UserTaskInit, NULL, &ThreadStart_Attr);

    /* Start the RTOS Kernel scheduler */
    osKernelStart();

    while (1);
}

/*!
 * @brief       Start task
 *
 * @param       pvParameters - passed into the task function as the function parameters
 *
 * @retval      None
 */
void UserTaskInit(void* pvParameters)
{
    const uint16_t usFrequency = 1;
    uint32_t tick;

    /* Configures LED */
    BOARD_LEDInit(LED2);
    BOARD_LEDInit(LED3);

    /* USART Initialization */
    USART_Init();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);

    /* User task create */
    xTaskUserCreate();

    /* Get the RTOS kernel tick count */
    tick = osKernelGetTickCount();

    while (1)
    {
        tick += usFrequency;
        osDelayUntil(tick);
    }
}

/*!
 * @brief       usart task
 *
 * @param       pvParameters - passed into the task function as the function parameters
 *
 * @retval      None
 */
void vTaskUsartTest(void* pvParameters)
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
        /* Send Data from USART1 to USART2 */
        timeout = 0;
        for(int i = 0; i < DATA_BUF_SIZE; i++)
        {
            /* Wait until end of transmission */
            while(USART_ReadStatusFlag(COM1_PORT, USART_FLAG_TXBE) == RESET)
            {
                osDelay(10);
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
                osDelay(10);
                timeout++;
                if(timeout > 10)
                {
                    break;
                }
            }

            rxDataBufUSART2[i] = USART_RxData(COM2_PORT);
        }

        /* Verify data */
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
        osDelay(1000);

        timeout = 0;
        /* Send Data from USART2 to USART1 */
        for(int i = 0; i < DATA_BUF_SIZE; i++)
        {
            /* Wait until end of transmission */
            while(USART_ReadStatusFlag(COM2_PORT, USART_FLAG_TXBE) == RESET)
            {
                osDelay(10);
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
                osDelay(10);
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
        osDelay(1000);
    }
}

/*!
 * @brief       Led toggle task
 *
 * @param       pvParameters - passed into the task function as the function parameters
 *
 * @retval      None
 */
void vTaskLedToggle(void* pvParameters)
{
    const uint16_t usFrequency = 200;
    uint32_t tick;

    /* Get the RTOS kernel tick count */
    tick = osKernelGetTickCount();

    while (1)
    {
        /* Toggle LED2 */
        BOARD_LEDToggle(LED2);

        /* Wait until specified time */
        tick += usFrequency;
        osDelayUntil(tick);
    }
}

/*!
 * @brief       User task create
 *
 * @param       None
 *
 * @retval      None
 */
static void xTaskUserCreate (void)
{
    ThreadIdTaskLedToggle = osThreadNew(vTaskLedToggle, NULL, &ThreadLedToggle_Attr);
    ThreadIdTaskUsart = osThreadNew(vTaskUsartTest, NULL, &ThreadUsart_Attr);
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

/**@} end of group RT5_Variables */
/**@} end of group RT5 */
/**@} end of group Examples */
