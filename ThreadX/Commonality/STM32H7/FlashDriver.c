//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FlashDriver.h"
#include "Delays.h"

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
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;
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

    uint32_t bank = GetBank(startAddress);
    __IO uint32_t *dest_addr = (__IO uint32_t *)(startAddress);
    __IO uint32_t *src_addr = (__IO uint32_t *)(buffer);

    int number_of_flash_words = numBytes / (FLASH_NB_32BITWORD_IN_FLASHWORD * 4U);
    int remaining_bytes = numBytes % (FLASH_NB_32BITWORD_IN_FLASHWORD * 4U);

    FlashUnlock();
    {
        switch (bank)
        {
            case 1:
                SET_BIT(FLASH->CR1, FLASH_CR_PG);
                break;
#if defined(DUAL_BANK)
            case 2:
                SET_BIT(FLASH->CR2, FLASH_CR_PG);
                break;
#endif
        }
        if (number_of_flash_words != 0) // Program the full flash word
        {
            for (int flash_word_index = 0; flash_word_index < number_of_flash_words; flash_word_index++)
            {
                WaitForLastOperation(bank);
                int row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;
                __ISB();
                __DSB();
                do
                {
                    *dest_addr = *src_addr;
                    dest_addr++;
                    src_addr++;
                    row_index--;
                } while (row_index != 0);
                __ISB();
                __DSB();
            }
        }
        if (remaining_bytes != 0)
        {
            __IO uint8_t *dest_addr_8_bit = (__IO uint8_t *)(dest_addr);
            __IO uint8_t *src_addr_8_bit = (__IO uint8_t *)(src_addr);
            WaitForLastOperation(bank);
            __ISB();
            __DSB();
            do
            {
                *dest_addr_8_bit = *src_addr_8_bit;
                dest_addr_8_bit++;
                src_addr_8_bit++;
                remaining_bytes--;
            } while (remaining_bytes != 0);
            __ISB();
            __DSB();

            switch (bank)
            {
                case 1:
                    SET_BIT(FLASH->CR1, FLASH_CR_FW);
                    break;
#if defined(DUAL_BANK)
                case 2:
                    SET_BIT(FLASH->CR2, FLASH_CR_FW);
                    break;
#endif
            }
        }
        switch (bank)
        {
            case 1:
                CLEAR_BIT(FLASH->CR1, FLASH_CR_PG);
                break;
#if defined(DUAL_BANK)
            case 2:
                CLEAR_BIT(FLASH->CR2, FLASH_CR_PG);
                break;
#endif
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
    uint32_t sector = GetSector(address);
    uint32_t bank = GetBank(address);

    switch (bank)
    {
        case 1:
        {
            WRITE_REG(FLASH->CCR1, FLASH_FLAG_ALL_ERRORS_BANK1);
#if defined(FLASH_CR_PSIZE)
            FLASH->CR1 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
            FLASH->CR1 |= (FLASH_CR_SER | FLASH_ERASE_SIZE | (sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#else
            FLASH->CR1 &= ~(FLASH_CR_SNB);
            FLASH->CR1 |= (FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#endif
        }
            success = WaitForLastOperation(bank);
            FLASH->CR1 &= ~(FLASH_CR_SER | FLASH_CR_SNB);
            break;
#if defined(DUAL_BANK)
        case 2:
            WRITE_REG(FLASH->CCR2, FLASH_FLAG_ALL_ERRORS_BANK2);

#if defined(FLASH_CR_PSIZE)
            FLASH->CR2 &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
            FLASH->CR2 |= (FLASH_CR_SER | FLASH_ERASE_SIZE | (sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#else
            FLASH->CR2 &= ~(FLASH_CR_SNB);
            FLASH->CR2 |= (FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos) | FLASH_CR_START);
#endif
            success = WaitForLastOperation(bank);
            FLASH->CR2 &= ~(FLASH_CR_SER | FLASH_CR_SNB);
            break;
#endif
    }

    return success;
}
bool FlashUnlock()
{
    //  The application software must not unlock a register that is already
    //  unlocked, otherwise this register will remain locked until next system
    //  reset.Similar constraints apply to bank erase requests.
    if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
    {
        // Authorize the FLASH Bank1 Registers access
        WRITE_REG(FLASH->KEYR1, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR1, FLASH_KEY2);
        if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) != 0U)
        {
            return false;
        }
    }
#if defined(DUAL_BANK)
    if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
    {
        // Authorize the FLASH Bank2 Registers access
        WRITE_REG(FLASH->KEYR2, FLASH_KEY1);
        WRITE_REG(FLASH->KEYR2, FLASH_KEY2);
        if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) != 0U)
        {
            return false;
        }
    }
#endif
    return true;
}
bool FlashLock()
{
    // Set the LOCK Bit to lock the FLASH Bank1 Control Register access */
    SET_BIT(FLASH->CR1, FLASH_CR_LOCK);
    if (READ_BIT(FLASH->CR1, FLASH_CR_LOCK) == 0U)
    {
        return false;
    }
#if defined(DUAL_BANK)
    // Set the LOCK Bit to lock the FLASH Bank2 Control Register access
    SET_BIT(FLASH->CR2, FLASH_CR_LOCK);
    if (READ_BIT(FLASH->CR2, FLASH_CR_LOCK) == 0U)
    {
        return false;
    }
#endif
    return true;
}
void WriteFlashWord(uint32_t bank, uint32_t *aligned_dest_addr, uint32_t *buffer_256bits)
{
    // dest_addr must be aligned on flash word boundary
    // Buffer must contain exactly 32 bytes ( 256 bits)
    // NOTE: Calling method must lock and unlock the flash before calling

    FlashUnlock();
    {
        if (WaitForLastOperation(bank))
        {
            switch (bank)
            {
                case 1:
                    int row_index = FLASH_NB_32BITWORD_IN_FLASHWORD;
                    __ISB();
                    __DSB();
                    do
                    {
                        *aligned_dest_addr = *buffer_256bits;
                        aligned_dest_addr++;
                        buffer_256bits++;
                        row_index--;
                    } while (row_index != 0);
                    __ISB();
                    __DSB();
                    break;
                case 2:
                    break;
            }
        }
    }
    FlashLock();
}
uint32_t GetSector(uint32_t Address)
{
    uint32_t sector = 0;
    if (Address < (FLASH_BASE + FLASH_BANK_SIZE))
    {
        sector = (Address - FLASH_BASE) / FLASH_SECTOR_SIZE;
    }
    else
    {
        sector = (Address - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_SECTOR_SIZE;
    }
    return sector;
}
uint32_t GetBank(uint32_t Address)
{
    int bank = 0;
#if defined(DUAL_BANK)
    if (Address >= FLASH_BANK1_BASE && Address < FLASH_BANK2_BASE)
    {
        bank = 1;
    }
    if (Address >= FLASH_BANK2_BASE && Address < FLASH_END)
    {
        bank = 2;
    }
#else
    if (Address >= FLASH_BANK1_BASE && Address < FLASH_END)
    {
        bank = 1;
    }
#endif
    return bank;
}
bool WaitForLastOperation(uint32_t bank)
{
    // Wait for the FLASH operation to complete by polling on QW flag to be reset.
    // Even if the FLASH operation fails, the QW flag will be reset and an error
    // flag will be set
    uint64_t flashTimeout = 50000; // 50 Milliseconds
    uint32_t errorflag = -1;
    uint32_t tickstart = GetCurrentMicroseconds(true);
    switch (bank)
    {
        case 1:
            while (READ_BIT(FLASH->SR1, FLASH_FLAG_QW_BANK1) == FLASH_FLAG_QW_BANK1)
            {
                if (((GetCurrentMicroseconds(false) - tickstart) > flashTimeout))
                {
                    return false;
                }
            }
            errorflag = FLASH->SR1 & FLASH_FLAG_ALL_ERRORS_BANK1;
            if ((errorflag & 0x7FFFFFFFU) != 0U)
            {
                // Clear error programming flags
                WRITE_REG(FLASH->CCR1, errorflag);
                return false;
            }
            if (READ_BIT(FLASH->SR1, FLASH_FLAG_EOP_BANK1) == FLASH_FLAG_EOP_BANK1)
            {
                WRITE_REG(FLASH->CCR1, FLASH_FLAG_EOP_BANK1);
            }
            break;
#if defined(DUAL_BANK)
        case 2:
            tickstart = GetCurrentMicroseconds(true);
            while (READ_BIT(FLASH->SR2, FLASH_FLAG_QW_BANK2) == FLASH_FLAG_QW_BANK2)
            {
                if (((GetCurrentMicroseconds(false) - tickstart) > flashTimeout))
                {
                    return false;
                }
            }
            errorflag = (FLASH->SR2 & FLASH_FLAG_ALL_ERRORS_BANK2) | 0x80000000U;
            if ((errorflag & 0x7FFFFFFFU) != 0U)
            {
                // Clear error programming flags
                WRITE_REG(FLASH->CCR2, errorflag & 0x7FFFFFFFU);
                return false;
            }
            if (READ_BIT(FLASH->SR2, FLASH_FLAG_EOP_BANK2 & 0x7FFFFFFFU) == (FLASH_FLAG_EOP_BANK2 & 0x7FFFFFFFU))
            {
                WRITE_REG(FLASH->CCR2, FLASH_FLAG_EOP_BANK2 & 0x7FFFFFFFU);
            }
            break;
#endif
    }
    return true;
}
