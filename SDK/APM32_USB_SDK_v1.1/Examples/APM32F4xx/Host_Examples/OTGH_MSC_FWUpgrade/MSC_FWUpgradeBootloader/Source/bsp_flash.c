/*!
 * @file        bsp_flash.c
 *
 * @brief       Include flash operation
 *
 * @version     V1.0.0
 *
 * @date        2023-01-16
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

/* Includes */
#include "bsp_flash.h"
#include "usb_host_user.h"

/** @addtogroup Examples
  * @brief OTG MSC Upgrade Bootloader examples
  @{
  */

/** @addtogroup OTG_MSC_FWUpgradeBootloader
  @{
  */

/** @defgroup OTG_MSC_FWUpgradeBootloader_Functions Functions
  @{
  */

/*!
 * @brief       Unlocks Flash for write access
 *
 * @param       None
 *
 * @retval      None
 *
 */
void FLASH_FlashInit(void)
{
    /* Unlock the flash memory */
    FMC_Unlock();

    /* Clear all FMC flags */
    FMC_ClearStatusFlag(FMC_FLAG_ENDOP | FMC_FLAG_ERROP \
                        | FMC_FLAG_ERRPGA | FMC_FLAG_ERRPGP | FMC_FLAG_ERRPGS | FMC_FLAG_ERRWRP);
}

/*!
 * @brief       Get readout protection status
 *
 * @param       None
 *
 * @retval      Readout protection status
 *
 */
uint8_t FLASH_ReadOutProtectionStatus(void)
{
    uint8_t errSta = RESET;

    if (FMC_OPT_ReadProtectLevel() == SET)
    {
        errSta = SET;
    }

    return errSta;
}

/*!
 * @brief       Get the flash sector number of the address
 *
 * @param       None
 *
 * @retval      The flash sector number of the address
 *
 */
static FMC_SECTOR_T FLASH_GetSectorNumber(uint32_t address)
{
    FMC_SECTOR_T flashSector = FMC_SECTOR_0;

    /* Flash sector less than 11 sector */
    if (address < FLASH_SECTOR_1_ADDR && address >= FLASH_SECTOR_0_ADDR)
    {
        flashSector = FMC_SECTOR_0;
    }
    else if (address < FLASH_SECTOR_2_ADDR && address >= FLASH_SECTOR_1_ADDR)
    {
        flashSector = FMC_SECTOR_1;
    }
    else if (address < FLASH_SECTOR_3_ADDR && address >= FLASH_SECTOR_2_ADDR)
    {
        flashSector = FMC_SECTOR_2;
    }
    else if (address < FLASH_SECTOR_4_ADDR && address >= FLASH_SECTOR_3_ADDR)
    {
        flashSector = FMC_SECTOR_3;
    }
    else if (address < FLASH_SECTOR_5_ADDR && address >= FLASH_SECTOR_4_ADDR)
    {
        flashSector = FMC_SECTOR_4;
    }
    else if (address < FLASH_SECTOR_6_ADDR && address >= FLASH_SECTOR_5_ADDR)
    {
        flashSector = FMC_SECTOR_5;
    }
    else if (address < FLASH_SECTOR_7_ADDR && address >= FLASH_SECTOR_6_ADDR)
    {
        flashSector = FMC_SECTOR_6;
    }
    else if (address < FLASH_SECTOR_8_ADDR && address >= FLASH_SECTOR_7_ADDR)
    {
        flashSector = FMC_SECTOR_7;
    }
    else if (address < FLASH_SECTOR_9_ADDR && address >= FLASH_SECTOR_8_ADDR)
    {
        flashSector = FMC_SECTOR_8;
    }
    else if (address < FLASH_SECTOR_10_ADDR && address >= FLASH_SECTOR_9_ADDR)
    {
        flashSector = FMC_SECTOR_9;
    }
    else if (address < FLASH_SECTOR_11_ADDR && address >= FLASH_SECTOR_10_ADDR)
    {
        flashSector = FMC_SECTOR_10;
    }
    else/*(address < FLASH_END_ADDR && address >= ADDR_FLASH_SECTOR_11)*/
    {
        flashSector = FMC_SECTOR_11;
    }

    return flashSector;
}

/*!
 * @brief       Erase of all user flash area
 *
 * @param       address: application address
 *
 * @retval      flash operation status
 *
 */
uint8_t FLASH_EraseSectors(uint32_t address)
{
    FMC_OP_T errSta = FMC_OP_OK;

    FMC_SECTOR_T sectorIndex = FMC_SECTOR_0;
    FMC_SECTOR_T startSector = FMC_SECTOR_0;
    FMC_STATUS_T eraseStatus = FMC_COMPLETE;

    startSector = FLASH_GetSectorNumber(address);
    sectorIndex = startSector;

    while ((eraseStatus == FMC_COMPLETE) && (sectorIndex <= FMC_SECTOR_11) && \
            (gUsbHostFS.devInfo.connectedStatus == ENABLE))
    {
        eraseStatus = FMC_EraseSector(sectorIndex, FMC_VOLTAGE_3);
        sectorIndex += 8;
    }

    if ((sectorIndex != (FMC_SECTOR_11 + 8)) || (eraseStatus != FMC_COMPLETE))
    {
        errSta = FMC_OP_ERROR;
    }

    return errSta;
}

/*!
 * @brief       Programs a word at a specified address
 *
 * @param       address: specifies the address to be programmed
 *
 * @param       data: specifies the data to be programmed
 *
 * @retval      flash status
 *
 */
FMC_STATUS_T FLASH_ProgramWord(uint32_t address, uint32_t data)
{
    FMC_STATUS_T errSta = FMC_COMPLETE;

    errSta = FMC_ProgramWord(address, data);

    return errSta;
}

/**@} end of group OTG_MSC_FWUpgradeBootloader_Functions */
/**@} end of group OTG_MSC_FWUpgradeBootloader */
/**@} end of group Examples */
