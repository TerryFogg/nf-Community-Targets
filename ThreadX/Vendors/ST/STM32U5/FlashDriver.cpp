//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FlashDriver.h"
#include "stm32u5xx_hal_flash.h"

bool FlashDriver_InitializeDevice(void *context)
{
    UNUSED(context);
    return true;
}
bool FlashDriver_UninitializeDevice(void *context)
{
    UNUSED(context);
    return true;
}
DeviceBlockInfo *FlashDriver_GetDeviceInfo(void *context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    return config->BlockConfig.BlockDeviceInformation;
}
bool FlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    UNUSED(context);

    __IO uint8_t *cursor = (__IO uint8_t *)startAddress;
    __IO uint8_t *endAddress = (__IO uint8_t *)(startAddress + numBytes);
    while (cursor < endAddress)
    {
        *buffer++ = *cursor++;
    }
    return true;
}
bool FlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    UNUSED(context);
    UNUSED(readModifyWrite);

    uint32_t primask_bit;
    __IO uint32_t *dest_addr = (__IO uint32_t *)(startAddress);
    __IO uint32_t *src_addr = (__IO uint32_t *)(buffer);

    int number_of_flash_words = numBytes / (FLASH_TYPEPROGRAM_QUADWORD * 4U);
    int remaining_bytes = numBytes % (FLASH_TYPEPROGRAM_QUADWORD * 4U);

    FlashUnlock();
    {
        if (number_of_flash_words != 0) // Program the full flash word
        {
            for (int flash_word_index = 0; flash_word_index < number_of_flash_words; flash_word_index++)
            {
                WaitForLastOperation();
                int row_index = FLASH_TYPEPROGRAM_QUADWORD;

                // Disable interrupts
                primask_bit = __get_PRIMASK();
                __disable_irq();
                {
                    do
                    {
                        *dest_addr = *src_addr;
                        dest_addr++;
                        src_addr++;
                        row_index--;
                    } while (row_index != 0);
                }
                // Exit critical section: restore previous priority mask
                __set_PRIMASK(primask_bit);
            }
        }
        if (remaining_bytes != 0)
        {
            __IO uint8_t *dest_addr_8_bit = (__IO uint8_t *)(dest_addr);
            __IO uint8_t *src_addr_8_bit = (__IO uint8_t *)(src_addr);
            WaitForLastOperation();

            // Disable interrupts
            primask_bit = __get_PRIMASK();
            __disable_irq();
            {
                do
                {
                    *dest_addr_8_bit = *src_addr_8_bit;
                    dest_addr_8_bit++;
                    src_addr_8_bit++;
                    remaining_bytes--;
                } while (remaining_bytes != 0);
            }
            // Exit critical section: restore previous priority mask
            __set_PRIMASK(primask_bit);
        }
    }
    FlashLock();

    return true;
}
bool FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    UNUSED(context);
    UNUSED(blockAddress);
    UNUSED(length);

    // NOTE:
    // From the reference manual  "RM0468 Rev 3" , page 165/3357
    // If the application software reads back a word that has been erased, all
    // the bits will be read at 1, without ECC error
    //
    // TODO Need to check performance, which is quicker, Reading 128K bytes for
    // 1's and no ECC error or just erase?

    // It is recommended not to overwrite a byte that is not a "virgin", (erased)
    // At the moment, always return false. The Block(128K sector) is not erased.

    return false;
}
bool FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    UNUSED(context);
    bool success = false;
    uint32_t Page = GetPage(address);
    uint32_t bank = GetBank(address);
    __IO uint32_t *reg_cr = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECCR) : &(FLASH->NSCR);

    FlashUnlock();
    {

        if (WaitForLastOperation()) // Flash unlocked?
        {
            if (bank == 1)
            {
                CLEAR_BIT((*reg_cr), FLASH_NSCR_BKER);
            }
            else
            {
                SET_BIT((*reg_cr), FLASH_NSCR_BKER);
            }

            MODIFY_REG(
                (*reg_cr),
                (FLASH_NSCR_PNB | FLASH_NSCR_PER | FLASH_NSCR_STRT),
                ((Page << FLASH_NSCR_PNB_Pos) | FLASH_NSCR_PER | FLASH_NSCR_STRT));

            // If the erase operation is completed, disable the associated bits
            CLEAR_BIT((*reg_cr), FLASH_TYPEERASE_PAGES & (~(FLASH_NON_SECURE_MASK)));
        }
    }
    FlashLock();
    return success;
}
bool FlashUnlock()
{
    if (READ_BIT(FLASH->NSCR, FLASH_NSCR_LOCK) != 0U)
    {
        // Authorize and unlock the flash
        WRITE_REG(FLASH->NSKEYR, FLASH_KEY1);
        WRITE_REG(FLASH->NSKEYR, FLASH_KEY2);

        // Verify Flash is unlocked
        if (READ_BIT(FLASH->NSCR, FLASH_NSCR_LOCK) != 0U)
        {
            return false;
        }
    }
    return true;
}
bool FlashLock()
{
    // Lock (non secure) the flash
    SET_BIT(FLASH->NSCR, FLASH_NSCR_LOCK);
    // Verify Flash is locked
    if (READ_BIT(FLASH->NSCR, FLASH_NSCR_LOCK) == 0U)
    {
        return false;
    }
    // Disable internal buffer for write operations
    CLEAR_BIT(FLASH->NSCR, FLASH_NSCR_LOCK);
    return true;
}
void WriteFlashWord(uint32_t bank, uint32_t *aligned_dest_addr, uint32_t *buffer_128bits)
{
    (void)bank;
    __IO uint32_t *reg_cr = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECCR) : &(FLASH_NS->NSCR);
    uint32_t primask_bit;

    FlashUnlock();
    {
        if (WaitForLastOperation())
        {
            // Disable interrupts
            primask_bit = __get_PRIMASK();
            __disable_irq();
            {
                // Set the non secure programming bit
                SET_BIT((*reg_cr), FLASH_NSCR_PG);

                int row_index = 16;
                do
                {
                    *aligned_dest_addr = *buffer_128bits;
                    aligned_dest_addr++;
                    buffer_128bits++;
                    row_index--;
                } while (row_index != 0);
            }
            // Exit critical section: restore previous priority mask
            __set_PRIMASK(primask_bit);
        }
    }
    FlashLock();
}
uint32_t GetPage(uint32_t Addr)
{
    uint32_t page = 0;

    if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        // Bank 1
        page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        // Bank 2
        page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
    }

    return page;
}
uint32_t GetBank(uint32_t Addr)
{
    return (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) ? 1 : 2;
}
bool WaitForLastOperation()
{
    uint32_t errorflag = -1;

    // Access to SECSR or NSSR registers depends on operation type
    __IO uint32_t *reg_sr = IS_FLASH_SECURE_OPERATION() ? &(FLASH->SECSR) : &(FLASH_NS->NSSR);

    // Wait until flashed
    while (((*reg_sr) & (FLASH_FLAG_BSY | FLASH_FLAG_WDW)) != 0U)
    {
    }

    // Check for errors
    errorflag = ((*reg_sr) & FLASH_FLAG_SR_ERRORS);
    if (errorflag != 0U)
    {
        // Clear error programming flags
        (*reg_sr) = errorflag;
        return false;
    }

    // Check FLASH End of Operation flag
    if (((*reg_sr) & FLASH_FLAG_EOP) != 0U)
    {
        // Clear FLASH End of Operation pending bit
        (*reg_sr) = FLASH_FLAG_EOP;
    }
    return true;
}
