//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <board.h>

#ifdef THIS_IS_A_WORK_IN_PROGRESS

// To select
#define DEFAULT_POLYNOMIALS

// X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 + X^10 +X^8 + X^7 + X^5 + X^4 + X^2+ X +1
#define DEFAULT_CRC32_POLY               0x04C11DB7U
#define CRC_POLYLENGTH_32B               0x00000000U
#define DEFAULT_CRC_INITVALUE            0xFFFFFFFFU
#define CRC_INPUTDATA_INVERSION_NONE     0x00000000U
#define CRC_OUTPUTDATA_INVERSION_DISABLE 0x00000000U

uint32_t SUPPORT_ComputeCRC(const void *rgBlock, const uint32_t nLength, const uint32_t initialCrc)
{
    uint32_t Pol;
    uint32_t PolyLength;
    CrcFormat crcFormat = N32Bits;

    ENABLE_CLOCK_ON_CRC;

    MODIFY_REG(CRC->CR, CRC_CR_REV_IN, CRC_INPUTDATA_INVERSION_NONE);
    MODIFY_REG(CRC->CR, CRC_CR_REV_OUT, CRC_OUTPUTDATA_INVERSION_DISABLE);

#ifdef DEFAULT_POLYNOMIALS
    WRITE_REG(CRC->POL, DEFAULT_CRC32_POLY);
    MODIFY_REG(CRC->CR, CRC_CR_POLYSIZE, CRC_POLYLENGTH_32B);
    WRITE_REG(CRC->INIT, DEFAULT_CRC_INITVALUE);

#else
    WRITE_REG(CRC->POL, Pol);
    MODIFY_REG(CRC->CR, CRC_CR_POLYSIZE, PolyLength);
    WRITE_REG(CRC->INIT, initialCrc);
#endif

    CRC->CR |= CRC_CR_RESET;
    uint32_t calculatedCrc = CalculateCRC(crcFormat, rgBlock, nLength);
    return calculatedCrc;
};

typedef enum CrcFormats
{
    N32Bits,
    N16Bits,
    N8Bits
} CrcFormat;
void CalculateCRC(CrcFormat crcFormat, uint32_t *pBuffer, uint32_t BufferLength)
{
    uint32_t calculatedCRC = 0;
    // input data buffer index
    uint32_t i;
    uint16_t data;
    __IO uint16_t *pReg;

    CRC->CR |= CRC_CR_RESET;

    switch (crcFormat)
    {
        case N32Bits:
            for (i = 0U; i < BufferLength; i++)
            {
                CRC->DR = pBuffer[i];
            }
            calculatedCRC = CRC->DR;
            break;
        case N8Bits:
            // Processing time optimization: 4 bytes are entered in a row with a single word write
            // last bytes must be carefully fed to the CRC calculator to ensure a correct type
            // handling by the peripheral
            for (uint32_t i = 0; i < (BufferLength / 4); i++)
            {
                CRC->DR = ((uint32_t)pBuffer[4 * i] << 24) | ((uint32_t)pBuffer[(4 * i) + 1] << 16) |
                          ((uint32_t)pBuffer[(4 * i) + 2] << 8) | (uint32_t)pBuffer[(4 * i) + 3];
            }
            /* last bytes specific handling */
            if ((BufferLength % 4) != 0)
            {
                if ((BufferLength % 4) == 1)
                {
                    // Derogation MisraC2012 R.11.5
                    *(__IO uint8_t *)(__IO void *)(&CRC->DR) = pBuffer[4U * i];
                }
                if ((BufferLength % 4) == 2)
                {
                    data = ((uint16_t)(pBuffer[4U * i]) << 8U) | (uint16_t)pBuffer[(4U * i) + 1U];
                    // Derogation MisraC2012 R.11.5
                    pReg = (__IO uint16_t *)(__IO void *)(&CRC->DR);
                    *pReg = data;
                }
                if ((BufferLength % 4) == 3)
                {
                    data = ((uint16_t)(pBuffer[4 * i]) << 8U) | (uint16_t)pBuffer[(4 * i) + 1];
                    // Derogation MisraC2012 R.11.5
                    pReg = (__IO uint16_t *)(__IO void *)(&CRC->DR);
                    *pReg = data;
                    // Derogation MisraC2012 R.11.5
                    *(__IO uint8_t *)(__IO void *)(&CRC->DR) = pBuffer[(4 * i) + 2];
                }
            }
            calculatedCRC = CRC->DR;
            break;

        case N16Bits:

            // Processing time optimization: 2 HalfWords are entered in a row with a single word write,
            // in case of odd length, last HalfWord must be carefully fed to the CRC calculator to ensure
            // a correct type handling by the peripheral
            for (uint32_t i = 0; i < (BufferLength / 2); i++)
            {
                CRC->DR = ((uint32_t)pBuffer[2 * i] << 16) | (uint32_t)pBuffer[(2 * i) + 1];
            }
            if ((BufferLength % 2) != 0U)
            {
                pReg = (__IO uint16_t *)(__IO void *)(&CRC->DR); /* Derogation MisraC2012 R.11.5 */
                *pReg = pBuffer[U * i];
            }
            calculatedCRC = CRC->DR;
            break;
        default:
            break;
    }
    return calculatedCRC;
}

#endif
