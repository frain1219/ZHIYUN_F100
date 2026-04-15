/*!
 * @file        can.h
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

/* Define to prevent recursive inclusion */
#ifndef __CAN_H
#define __CAN_H

/* Includes */
#include "config.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USARTTOCAN
  @{
*/

/** @defgroup USARTTOCAN_Macros Macros
  @{
*/

#define CAN_BUF_CNT     (128)

/* Define one bit mask */
#define BIT0                    ((uint32_t)0x00000001)
#define BIT1                    ((uint32_t)0x00000002)
#define BIT2                    ((uint32_t)0x00000004)
#define BIT3                    ((uint32_t)0x00000008)
#define BIT4                    ((uint32_t)0x00000010)
#define BIT5                    ((uint32_t)0x00000020)
#define BIT6                    ((uint32_t)0x00000040)
#define BIT7                    ((uint32_t)0x00000080)
#define BIT8                    ((uint32_t)0x00000100)
#define BIT9                    ((uint32_t)0x00000200)
#define BIT10                   ((uint32_t)0x00000400)
#define BIT11                   ((uint32_t)0x00000800)
#define BIT12                   ((uint32_t)0x00001000)
#define BIT13                   ((uint32_t)0x00002000)
#define BIT14                   ((uint32_t)0x00004000)
#define BIT15                   ((uint32_t)0x00008000)
#define BIT16                   ((uint32_t)0x00010000)
#define BIT17                   ((uint32_t)0x00020000)
#define BIT18                   ((uint32_t)0x00040000)
#define BIT19                   ((uint32_t)0x00080000)
#define BIT20                   ((uint32_t)0x00100000)
#define BIT21                   ((uint32_t)0x00200000)
#define BIT22                   ((uint32_t)0x00400000)
#define BIT23                   ((uint32_t)0x00800000)
#define BIT24                   ((uint32_t)0x01000000)
#define BIT25                   ((uint32_t)0x02000000)
#define BIT26                   ((uint32_t)0x04000000)
#define BIT27                   ((uint32_t)0x08000000)
#define BIT28                   ((uint32_t)0x10000000)
#define BIT29                   ((uint32_t)0x20000000)
#define BIT30                   ((uint32_t)0x40000000)
#define BIT31                   ((uint32_t)0x80000000)

/**@} end of group USARTTOCAN_Macros */

/** @defgroup USARTTOCAN_Functions Functions
  @{
*/

void Can_Config(void);

uint8_t Can_Read(CAN_RxMessage_T *dat);
void Can_Write(uint8_t id, uint8_t *dat, uint16_t len);

uint8_t Can_ReadSlaveAck(CAN_RxMessage_T *msg);
uint8_t Can_ReadSlaveAck92(CAN_RxMessage_T *msg);
void Can_ReadDataFromSlave(uint8_t *dat, uint16_t len);

void CAN_Interrupt(void);
void CAN_SwitchSpeed(void);

/**@} end of group USARTTOCAN_Functions */
/**@} end of group USARTTOCAN */
/**@} end of group Examples */

#endif
