// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "I2C_To_Touch_STM32.h"

void Initialize_I2C()
{
    ENABLE_CLOCK_ON_PORT_GPIOF;

    // SCL Pin as : Alternate function, High Speed, Open drain, No Pull
    LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_14, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOF, LL_GPIO_PIN_14, LL_GPIO_AF_4);
    LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_14, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOF, LL_GPIO_PIN_14, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOF, LL_GPIO_PIN_14, LL_GPIO_PULL_NO);

    // SDA Pin as : Alternate function, High Speed, Open drain, No Pull
    LL_GPIO_SetPinMode(GPIOF, LL_GPIO_PIN_15, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOF, LL_GPIO_PIN_15, LL_GPIO_AF_4);
    LL_GPIO_SetPinSpeed(GPIOF, LL_GPIO_PIN_15, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOF, LL_GPIO_PIN_15, LL_GPIO_OUTPUT_OPENDRAIN);
    LL_GPIO_SetPinPull(GPIOF, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);

    I2C_ENABLE_TOUCH_CLOCK;
    I2C_TOUCH_FORCE_RESET;
    I2C_TOUCH_FORCE_RELEASE_RESET;

    // Disable before updating configuration   registers
    LL_I2C_Disable(I2C4);
    {
        // SDA setup, hold time and the SCL high, low period.
        LL_I2C_SetTiming(I2C4, I2C_TIMING);
        LL_I2C_SetOwnAddress1(I2C4, 0x00, LL_I2C_OWNADDRESS1_7BIT);
        // TC flag is set when NBYTES data are transferred, stretching SCL low.
        LL_I2C_EnableAutoEndMode(I2C4);
        LL_I2C_AcknowledgeNextData(I2C4, LL_I2C_NACK);
        // Address2 configuration
        LL_I2C_DisableOwnAddress1(I2C4);
        LL_I2C_SetOwnAddress2(I2C4, 0x00, LL_I2C_OWNADDRESS2_NOMASK);
        LL_I2C_EnableClockStretching(I2C4);
        LL_I2C_DisableGeneralCall(I2C4);
    }
    LL_I2C_Enable(I2C4);
}
bool Write8_Read_I2C(
    CLR_INT8 slaveAddress,
    CLR_UINT8 I2cRegister,
    CLR_UINT8 *dataReturned,
    CLR_UINT16 numberValuesExpected)
{
    // Check bus busy flag
    while (LL_I2C_IsActiveFlag_BUSY(I2C4))
    {
    }
    // Prepare transfer parameters
    __IO uint16_t XferSize;
    uint8_t *pBuffPtr = dataReturned;
    __IO uint16_t XferCount = numberValuesExpected;

    LL_I2C_HandleTransfer(
        I2C4,
        slaveAddress,
        LL_I2C_ADDRSLAVE_7BIT,
        I2C_MEMADD_SIZE_8BIT,
        LL_I2C_MODE_SOFTEND,
        LL_I2C_GENERATE_START_WRITE);

    // Wait until TXIS flag is set
    if (I2C_WaitOnTXISFlagUntilTimeout() != true)
    {
        return false;
    }
    LL_I2C_TransmitData8(I2C4, I2cRegister);
    while (LL_I2C_IsActiveFlag_TC(I2C4))
    {
    }
    XferSize = XferCount;
    LL_I2C_HandleTransfer(
        I2C4,
        slaveAddress,
        LL_I2C_ADDRSLAVE_7BIT,
        XferSize,
        LL_I2C_MODE_AUTOEND,
        LL_I2C_GENERATE_START_READ);
    do
    {
        while (!LL_I2C_IsActiveFlag_RXNE(I2C4))
        {
        };
        *pBuffPtr = I2C4->RXDR;
        pBuffPtr++;
        XferSize--;
        XferCount--;
        if ((XferCount != 0U) && (XferSize == 0U))
        {
            while (LL_I2C_IsActiveFlag_TCR(I2C4))
            {
            };

            XferSize = XferCount;
            LL_I2C_HandleTransfer(
                I2C4,
                slaveAddress,
                LL_I2C_ADDRSLAVE_7BIT,
                XferSize,
                LL_I2C_MODE_AUTOEND,
                LL_I2C_GENERATE_NOSTARTSTOP);
        }
    } while (XferCount > 0U);

    // No need to Check TC flag, with AUTOEND mode the stop is automatically generated
    while (!LL_I2C_IsActiveFlag_STOP(I2C4))
    {
    };
    // Check if a NACK is detected
    if (I2C_IsAcknowledgeFailed() != true)
    {
        return false;
    }
    // Clear STOP Flag
    ((I2C_ISR_STOPF == I2C_ISR_TXE) ? (I2C4->ISR |= I2C_ISR_STOPF) : (I2C4->ICR = I2C_ISR_STOPF));

    // Clear Configuration Register 2
    (I2C4->CR2 &=
     (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));

    return true;
}
bool I2C_IsAcknowledgeFailed()
{
    if (READ_BIT(I2C4->ISR, I2C_ISR_NACKF) == SET)
    {
        // AutoEnd should be initiate after AF
        // Wait until STOP Flag is reset
        while (READ_BIT(I2C4->ISR, I2C_ISR_STOPF) == RESET)
        {
        }
        // Clear NACKF Flag
        ((I2C_ISR_NACKF == I2C_ISR_TXE) ? (I2C4->ISR |= I2C_ISR_NACKF) : (I2C4->ICR = I2C_ISR_NACKF));
        // Clear
        ((I2C_ISR_STOPF == I2C_ISR_TXE) ? (I2C4->ISR |= I2C_ISR_STOPF) : (I2C4->ICR = I2C_ISR_STOPF));

        // If a pending TXIS flag is set
        if (READ_BIT(I2C4->ISR, I2C_ISR_TXIS) == SET)
        {
            // Write a dummy data in TXDR to clear it
            I2C4->TXDR = 0x00U;
        }
        // Flush TX register if not empty
        if (READ_BIT(I2C4->ISR, I2C_ISR_TXIS) == RESET)
        {
            (I2C_ISR_TXE == I2C_ISR_TXE) ? (I2C4->ISR |= I2C_ISR_TXE) : (I2C4->ICR = I2C_ISR_TXE);
        }
        // Clear Configuration Register 2
        (I2C4->CR2 &=
         (uint32_t) ~((uint32_t)(I2C_CR2_SADD | I2C_CR2_HEAD10R | I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_RD_WRN)));
        return false;
    }
    return true;
}
bool I2C_WaitOnTXISFlagUntilTimeout()
{
    while (READ_BIT(I2C4->ISR, I2C_ISR_TXIS) == RESET)
    {
        if (I2C_IsAcknowledgeFailed() != true) // Check if a NACK is detected
        {
            return false;
        }
    }
    return true;
}
bool Write8Bits(CLR_UINT16 I2CAddress, uint8_t I2CRegister, uint8_t I2CData)
{
    // Check bus busy flag
    while (LL_I2C_IsActiveFlag_BUSY(I2C4))
    {
    }
    LL_I2C_HandleTransfer(I2C4, I2CAddress, LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

    while (!LL_I2C_IsActiveFlag_TXE(I2C4))
    {
    }
    // Wait until TXIS flag is set
    if (I2C_WaitOnTXISFlagUntilTimeout() != true)
    {
        return false;
    }
    LL_I2C_TransmitData8(I2C4, I2CRegister);
    while (LL_I2C_IsActiveFlag_TC(I2C4))
    {
    }
    LL_I2C_TransmitData8(I2C4, I2CData);
    while (LL_I2C_IsActiveFlag_TC(I2C4))
    {
    }
    return true;
}
