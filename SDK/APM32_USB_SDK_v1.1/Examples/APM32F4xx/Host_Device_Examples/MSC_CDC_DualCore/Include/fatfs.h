/*!
 * @file        fatfs.h
 *
 * @brief       Header for fatfs.c module
 *
 * @version     V1.0.0
 *
 * @date        2023-11-13
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
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
#ifndef __FATFS_H_
#define __FATFS_H_

/* Includes */
#include "ff.h"
#include "diskio.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup MSC_CDC_DualCore
  @{
*/

/** @defgroup MSC_CDC_DualCore_Variables Variables
  @{
  */

extern FATFS* fs[FF_VOLUMES];
extern char fileScanPath[255];

/**@} end of group MSC_CDC_DualCore_Variables*/

/** @defgroup MSC_CDC_DualCore_Functions Functions
  @{
  */

FRESULT FATFS_ScanFiles(char* path);
void FATFS_WriteFile(FIL* file, uint8_t* buf, uint32_t len);
void FATFS_ReadFile(FIL* file);

/**@} end of group MSC_CDC_DualCore_Functions */
/**@} end of group MSC_CDC_DualCore */
/**@} end of group Examples */

#endif
