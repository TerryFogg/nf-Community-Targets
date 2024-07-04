//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "DeviceIO.h"
#include "board.h"

#pragma region STM32 definitions

GPIO_INTERRUPT_SERVICE_ROUTINE *SavedInterruptCallBack[16];
int16_t SavedInterruptPinNumber[16];

uint32_t GpioPortClockEnable[] = {
    LL_AHB4_GRP1_PERIPH_GPIOA,
    LL_AHB4_GRP1_PERIPH_GPIOB,
    LL_AHB4_GRP1_PERIPH_GPIOC,
    LL_AHB4_GRP1_PERIPH_GPIOD,
    LL_AHB4_GRP1_PERIPH_GPIOE,
    LL_AHB4_GRP1_PERIPH_GPIOF,
    LL_AHB4_GRP1_PERIPH_GPIOH,
#ifdef GPIOI
    LL_AHB4_GRP1_PERIPH_GPIOI,
#else
    0,
#endif
    LL_AHB4_GRP1_PERIPH_GPIOJ,
    LL_AHB4_GRP1_PERIPH_GPIOK};
uint32_t UsartPortClockEnable[] = {
    LL_APB2_GRP1_PERIPH_USART1,
    LL_APB1_GRP1_PERIPH_USART2,
    LL_APB1_GRP1_PERIPH_USART3,
    0,
    0,
    LL_APB2_GRP1_PERIPH_USART6,
    0,
    0,
    0,
    LL_APB2_GRP1_PERIPH_USART10};
uint32_t DmaClockEnable[] = {
    LL_AHB1_GRP1_PERIPH_DMA1,
    LL_AHB1_GRP1_PERIPH_DMA2,
};
uint32_t PinMask[] = {
    LL_GPIO_PIN_0,
    LL_GPIO_PIN_1,
    LL_GPIO_PIN_2,
    LL_GPIO_PIN_3,
    LL_GPIO_PIN_4,
    LL_GPIO_PIN_5,
    LL_GPIO_PIN_6,
    LL_GPIO_PIN_7,
    LL_GPIO_PIN_8,
    LL_GPIO_PIN_9,
    LL_GPIO_PIN_10,
    LL_GPIO_PIN_11,
    LL_GPIO_PIN_12,
    LL_GPIO_PIN_13,
    LL_GPIO_PIN_14,
    LL_GPIO_PIN_15,
    LL_GPIO_PIN_ALL};
GPIO_TypeDef *Port[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIOH,
#ifdef GPIOI
    GPIOI,
#else
    (GPIO_TypeDef *)-1,
#endif
    GPIOJ,
    GPIOK};
uint32_t EXTI_Line[] = {
    LL_EXTI_LINE_0,
    LL_EXTI_LINE_1,
    LL_EXTI_LINE_2,
    LL_EXTI_LINE_3,
    LL_EXTI_LINE_4,
    LL_EXTI_LINE_5,
    LL_EXTI_LINE_6,
    LL_EXTI_LINE_7,
    LL_EXTI_LINE_8,
    LL_EXTI_LINE_9,
    LL_EXTI_LINE_10,
    LL_EXTI_LINE_11,
    LL_EXTI_LINE_12,
    LL_EXTI_LINE_13,
    LL_EXTI_LINE_14,
    LL_EXTI_LINE_15};
uint32_t SyscfgPorts[] = {
    LL_SYSCFG_EXTI_PORTA,
    LL_SYSCFG_EXTI_PORTB,
    LL_SYSCFG_EXTI_PORTC,
    LL_SYSCFG_EXTI_PORTD,
    LL_SYSCFG_EXTI_PORTE,
    LL_SYSCFG_EXTI_PORTF,
    LL_SYSCFG_EXTI_PORTG,
    LL_SYSCFG_EXTI_PORTH,
#ifdef GPIOI
    LL_SYSCFG_EXTI_PORTI,
#else
    -1,
#endif
    LL_SYSCFG_EXTI_PORTJ,
    LL_SYSCFG_EXTI_PORTK};
// Arranged so index 1 == I2C1, etc...
IRQn_Type I2C_Interrupt_Channel[] = {
    I2C1_ER_IRQn,
    I2C2_ER_IRQn,
    I2C3_ER_IRQn,
    I2C4_ER_IRQn,
#ifdef I2C5
    I2C5_ER_IRQn
#endif
};
ADC_TypeDef *ADC_Channel[] = {
    NULL, // No Channel zero
    ADC1,
    ADC2,
#ifdef ADC3
    ADC3,
#else
    (ADC_TypeDef *)-1,
#endif
};
DMA_TypeDef *DMA_Channel[] = {
    NULL, // No Channel zero
    DMA1,
    DMA2};
DAC_TypeDef *DAC_Channel[] = {
    NULL, // No Channel zero
    DAC1,
#ifdef DAC2
    DAC2,
#endif
#ifdef DAC3
    DAC3,
#else
    (DAC_TypeDef *)-1,
#endif
};
I2C_TypeDef *I2C_Channel[] = {
    NULL, // No Channel zero
    I2C1,
    I2C2,
    I2C3,
    I2C4,
#ifdef I2C5
    I2C5,
#else
    (I2C_TypeDef *)-1,
#endif
#ifdef I2C6
    I2C6,
#else
    (I2C_TypeDef *)-1,
#endif
};
SPI_TypeDef *SPI_Channel[] = {
    NULL, // No Channel zero
    SPI1,
    SPI2,
    SPI2,
    SPI3,
    SPI4,
    SPI5,
#ifdef SPI6
    SPI6,
#else
    (SPI_TypeDef *)-1,
#endif
};
TIM_TypeDef *PWM_Channel[] = {
    NULL, // No Channel zero
    TIM2,
    TIM3,
    TIM4,
    TIM5};
DeviceRegistration::SerialChannel Serial_Channel[] = {
    {NULL, ' '}, // No Channel zero
    {USART1, 0x0D},
    {USART2, 0x0D},
    {USART2, 0x0D},
    {USART3, 0x0D},
#ifdef USART4
    USART4,
#else
    {(USART_TypeDef *)-1, ' '},
#endif
#ifdef USART5
    {USART5, 0x0D},
    USART5,
#else
    {(USART_TypeDef *)-1, ' '},
#endif
    {USART6, 0x0D},
#ifdef USART7
    {USART7, 0x0D},
#else
    {(USART_TypeDef *)-1, ' '},
#endif
#ifdef USART8
    {USART8, 0x0D},
#else
    {(USART_TypeDef *)-1, ' '},
#endif
#ifdef USART9
    {USART9, 0x0D},
#else
    {(USART_TypeDef *)-1, ' '},
#endif
    {USART10, 0x0D},
};
uint32_t AdcChannelNumber[] = {
    LL_ADC_CHANNEL_1,
    LL_ADC_CHANNEL_2,
    LL_ADC_CHANNEL_3,
    LL_ADC_CHANNEL_4,
    LL_ADC_CHANNEL_5,
    LL_ADC_CHANNEL_6,
    LL_ADC_CHANNEL_7,
    LL_ADC_CHANNEL_8,
    LL_ADC_CHANNEL_9,
    LL_ADC_CHANNEL_10,
    LL_ADC_CHANNEL_11,
    LL_ADC_CHANNEL_12,
    LL_ADC_CHANNEL_13,
    LL_ADC_CHANNEL_14,
    LL_ADC_CHANNEL_15,
};
uint32_t DmaStreamNumber[] = {
    LL_DMA_STREAM_0,
    LL_DMA_STREAM_1,
    LL_DMA_STREAM_2,
    LL_DMA_STREAM_3,
    LL_DMA_STREAM_4,
    LL_DMA_STREAM_5,
    LL_DMA_STREAM_6,
    LL_DMA_STREAM_7};

int GetI2CPin_AlternateFunctionNumber(
    GPIO_PIN GPIOPortByNumber,
    int pinNumber,
    DeviceRegistration::DevicePinFunction dpf);

int GetI2CPin_AlternateFunctionNumber(
    GPIO_PIN GPIOPortByNumber,
    int pinNumber,
    DeviceRegistration::DevicePinFunction dpf)
{
    uint32_t alternate = -1;
    switch (dpf)
    {
        case DeviceRegistration::DevicePinFunction::NONE:
        case DeviceRegistration::DevicePinFunction::GPIO:
        case DeviceRegistration::DevicePinFunction::ADC:
        case DeviceRegistration::DevicePinFunction::CAN:
        case DeviceRegistration::DevicePinFunction::DAC:
        case DeviceRegistration::DevicePinFunction::SPI:
        case DeviceRegistration::DevicePinFunction::COUNTER:
        case DeviceRegistration::DevicePinFunction::PWM:
        case DeviceRegistration::DevicePinFunction::TIMER:
        case DeviceRegistration::DevicePinFunction::I2C:
            if (GPIOPortByNumber == 2 && pinNumber == 8)
            {
                // PB8 - I2C1_SCL,AF4
                alternate = LL_GPIO_AF_4;
            }
            else if (GPIOPortByNumber == 2 && pinNumber == 9)
            {
                // PB9 - I2C1_SDA,AF4
                alternate = LL_GPIO_AF_4;
            }
            else if (GPIOPortByNumber == 4 && pinNumber == 12)
            {
                // PD12 - I2C4_SCL,AF4
                alternate = LL_GPIO_AF_4;
            }
            else if (GPIOPortByNumber == 4 && pinNumber == 13)
            {
                // PD13 - I2C4_SCL,AF4
                alternate = LL_GPIO_AF_4;
            }
            break;
        case DeviceRegistration::DevicePinFunction::I2S:
        case DeviceRegistration::DevicePinFunction::SD:
        case DeviceRegistration::DevicePinFunction::USART:
        case DeviceRegistration::DevicePinFunction::WAKEUP:
        case DeviceRegistration::DevicePinFunction::LCD:
            break;
    }
    return alternate;
}
uint32_t I2C_GetTiming(I2cBusSpeed busSpeed)
{
    uint32_t timing;
    switch (busSpeed)
    {
        case I2cBusSpeed_StandardMode:
            timing = 0x20B0CCFF;
            break;
        case I2cBusSpeed_FastMode:
            timing = 0x00D04BFF;
            break;
        case I2cBusSpeed_FastModePlus:
            timing = 0x00601C5C;
            break;
        default:
            timing = -1;
            break;
    }
    return timing;
}

#pragma endregion

#pragma region GPIO
bool GpioIO::Initialize(PinNameValue pinNameValue)
{
    int GPIOPortByNumber = pinNameValue / 16;
    if (!LL_AHB4_GRP1_IsEnabledClock(GpioPortClockEnable[GPIOPortByNumber]))
    {
        LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
    }
    return true;
}
bool GpioIO::Dispose(PinNameValue pinNameValue)
{
    GpioIO::InterruptRemove(pinNameValue);
    GpioIO::SetLowPower(pinNameValue);
    return true;
}
bool GpioIO::Read(PinNameValue pinNameValue)
{
    int GPIOPortByNumber = pinNameValue / 16;
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    return (bool)LL_GPIO_IsInputPinSet(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber]);
}
bool GpioIO::Write(PinNameValue pinNameValue, bool pinState)
{
    // true == 1
    // false == 0
    int GPIOPortByNumber = pinNameValue / 16;
    uint32_t gpioPortPinNumber = pinNameValue % 16;

    if (pinState == GpioPinValue_Low)
    {
        LL_GPIO_ResetOutputPin(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber]);
    }
    else if (GpioPinValue_High)
    {
        LL_GPIO_SetOutputPin(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber]);
    }
    return true;
}
bool GpioIO::Toggle(PinNameValue pinNameValue)
{
    bool status = false;
    int GPIOPortByNumber = pinNameValue / 16;
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    LL_GPIO_TogglePin(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber]);
    status = true;
    return status;
}
bool GpioIO::SetFunction(
    PinNameValue pinNameValue,
    DeviceRegistration::DevicePinFunction PinFunction,
    int alternateFunctionNumber)
{
    int GPIOPortByNumber = pinNameValue / 16;
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    bool status = false;
    switch (PinFunction)
    {
        case DeviceRegistration::DevicePinFunction::NONE:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::ADC:
            LL_GPIO_InitTypeDef GPIO_InitStruct;
            LL_GPIO_SetPinMode(Port[GPIOPortByNumber], gpioPortPinNumber, LL_GPIO_MODE_ANALOG);
            break;
        case DeviceRegistration::DevicePinFunction::CAN:
            // Not supported on this MCU
            status = false;
            break;
        case DeviceRegistration::DevicePinFunction::DAC:
            status = false;
            break;
        case DeviceRegistration::DevicePinFunction::GPIO:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::SPI:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::COUNTER:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::PWM:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::SD:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::TIMER:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::I2C:
        {
            LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
            LL_GPIO_SetPinMode(Port[GPIOPortByNumber], gpioPortPinNumber, LL_GPIO_MODE_ALTERNATE);
            if (gpioPortPinNumber <= 7)
            {
                LL_GPIO_SetAFPin_8_15(Port[GPIOPortByNumber], gpioPortPinNumber, alternateFunctionNumber);
            }
            if (gpioPortPinNumber >= 8)
            {
                LL_GPIO_SetAFPin_8_15(Port[GPIOPortByNumber], gpioPortPinNumber, alternateFunctionNumber);
            }
            LL_GPIO_SetPinSpeed(Port[GPIOPortByNumber], gpioPortPinNumber, LL_GPIO_SPEED_FREQ_HIGH);
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], gpioPortPinNumber, LL_GPIO_OUTPUT_OPENDRAIN);
        }
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::I2S:
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::USART:
            LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
            GPIO_InitStruct.Alternate = alternateFunctionNumber;
            GPIO_InitStruct.Pin = gpioPortPinNumber;
            GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
            GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
            GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
            GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
            LL_GPIO_Init(Port[GPIOPortByNumber], &GPIO_InitStruct);
            status = true;
            break;
        case DeviceRegistration::DevicePinFunction::WAKEUP:
            status = true;
            break;
        default:
            status = false;
            break;
    }
    return status;
}
bool GpioIO::SetLowPower(PinNameValue pinNumber)
{
    int GPIOPortByNumber = pinNumber / 16;
    uint32_t gpioPortPinNumber = pinNumber % 16;

    // Set the unused pins as input(floating) to minimize current consumption.
    LL_GPIO_SetPinMode(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_MODE_INPUT);
    LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_PULL_NO);

    return true;
}
bool GpioIO::SetMode(PinNameValue pinNameValue, PinMode pinMode)
{
    bool status = false;
    int GPIOPortByNumber = pinNameValue / 16;
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    switch (pinMode)
    {
        case PinMode_Input:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_MODE_INPUT);
            LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_PULL_NO);
            status = true;
            break;
        case PinMode_InputPullDown:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_MODE_INPUT);
            LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_PULL_DOWN);
            status = true;
            break;
        case PinMode_InputPullUp:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_MODE_INPUT);
            LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_PULL_UP);
            status = true;
            break;
        case PinMode_Output:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_OUTPUT_PUSHPULL);
            status = true;
            break;
        case PinMode_OutputOpenDrain:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_OUTPUT_OPENDRAIN);
            status = true;
            break;
        case PinMode_OutputOpenDrainPullUp:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_OUTPUT_OPENDRAIN);
            status = true;
            break;
        case PinMode_OutputOpenSource:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_OUTPUT_OPENDRAIN);
            status = true;
            break;
        case PinMode_OutputOpenSourcePullDown:
            LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[gpioPortPinNumber], LL_GPIO_OUTPUT_OPENDRAIN);
            status = true;
            break;
        default:
            status = false;
            break;
    }
    return status;
}
bool GpioIO::InterruptEnable(PinNameValue pinNameValue, GPIO_INT_EDGE events, GPIO_INTERRUPT_SERVICE_ROUTINE gpioIsrPtr)
{
    // NOTE: There is a limitation of 16 external interrupts on the GPIO lines with one 1 interrupt per line number.
    //      One interrupt on (PA0 or PB0 or PC0 or PD0...) and one interrupt on (PA1, or PB1 or PC1 ...) etc.
    bool expectedState = false;
    int GPIOPortByNumber = pinNameValue / 16;
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    uint32_t priority = 0;
    SavedInterruptCallBack[gpioPortPinNumber] = &gpioIsrPtr;
    LL_SYSCFG_SetEXTISource(SyscfgPorts[GPIOPortByNumber], PinMask[gpioPortPinNumber]);
    switch (events)
    {
        case GPIO_INT_NONE:
            LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            expectedState = false;
            break;
        case GPIO_INT_EDGE_LOW:
            LL_EXTI_EnableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            expectedState = true;
            break;
        case GPIO_INT_LEVEL_LOW:
            LL_EXTI_EnableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            expectedState = true;
            break;
        case GPIO_INT_EDGE_HIGH:
            LL_EXTI_EnableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            expectedState = false;
            break;
        case GPIO_INT_LEVEL_HIGH:
            LL_EXTI_EnableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            expectedState = false;
            break;
            LL_EXTI_EnableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_EnableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            expectedState = false;
            break;
        case GPIO_INT_EDGE_BOTH:
            LL_EXTI_EnableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            LL_EXTI_EnableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
            // Wait for the opposite of current state
            expectedState = !GpioIO::Read(pinNameValue);
            break;
    }
    switch (gpioPortPinNumber)
    {
        case 0:
            NVIC_SetPriority(EXTI0_IRQn, priority);
            NVIC_EnableIRQ(EXTI0_IRQn);
            break;
        case 1:
            NVIC_SetPriority(EXTI1_IRQn, priority);
            NVIC_EnableIRQ(EXTI1_IRQn);
            break;
        case 2:
            NVIC_SetPriority(EXTI2_IRQn, priority);
            NVIC_EnableIRQ(EXTI2_IRQn);
            break;
        case 3:
            NVIC_SetPriority(EXTI3_IRQn, priority);
            NVIC_EnableIRQ(EXTI3_IRQn);
            break;
        case 4:
            NVIC_SetPriority(EXTI4_IRQn, priority);
            NVIC_EnableIRQ(EXTI4_IRQn);
            break;
        case 5:
            NVIC_SetPriority(EXTI9_5_IRQn, priority);
            NVIC_EnableIRQ(EXTI9_5_IRQn);
            break;
        case 6:
            NVIC_SetPriority(EXTI9_5_IRQn, priority);
            NVIC_EnableIRQ(EXTI9_5_IRQn);
            break;
        case 7:
            NVIC_SetPriority(EXTI9_5_IRQn, priority);
            NVIC_EnableIRQ(EXTI9_5_IRQn);
            break;
        case 8:
            NVIC_SetPriority(EXTI9_5_IRQn, priority);
            NVIC_EnableIRQ(EXTI9_5_IRQn);
            break;
        case 9:
            NVIC_SetPriority(EXTI9_5_IRQn, priority);
            NVIC_EnableIRQ(EXTI9_5_IRQn);
            break;
        case 10:
            NVIC_SetPriority(EXTI15_10_IRQn, priority);
            NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
        case 11:
            NVIC_SetPriority(EXTI15_10_IRQn, priority);
            NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
        case 12:
            NVIC_SetPriority(EXTI15_10_IRQn, priority);
            NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
        case 13:
            NVIC_SetPriority(EXTI15_10_IRQn, priority);
            NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
        case 14:
            NVIC_SetPriority(EXTI15_10_IRQn, priority);
            NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
        case 15:
            NVIC_SetPriority(EXTI15_10_IRQn, priority);
            NVIC_EnableIRQ(EXTI15_10_IRQn);
            break;
    }
    LL_EXTI_EnableIT_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_ClearFlag_0_31(EXTI_Line[gpioPortPinNumber]);

    return expectedState;
}
bool GpioIO::InterruptDisable(PinNameValue pinNameValue)
{
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    LL_D3_EXTI_DisablePendMask_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_DisableIT_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
    SYSCFG->EXTICR[EXTI_Line[gpioPortPinNumber] & 0x3U] |= ~(EXTI_Line[gpioPortPinNumber] >> 16U);
    return true;
}
bool GpioIO::InterruptRemove(PinNameValue pinNameValue)
{
    uint32_t gpioPortPinNumber = pinNameValue % 16;
    // Clear the External Interrupt or Event for the current IO
    LL_D3_EXTI_DisablePendMask_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_DisableIT_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[gpioPortPinNumber]);
    LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[gpioPortPinNumber]);

    // Clear the config of the line
    SYSCFG->EXTICR[EXTI_Line[gpioPortPinNumber] & 0x3U] |= ~(EXTI_Line[gpioPortPinNumber] >> 16U);

    return true;
}
#pragma endregion

#pragma region ADC

// 2� ADCs with 16-bit max. resolution (up to 24 channels, up to 3.6 MSPS)
// 1� Analog and 1x Digital temperature sensors
// 3x Channels (1 single channel + 1 dual-channel interfaces)
// Analog temperature sensor generates a voltage that varies linearly with the temperature.
// This temperature sensor is internally connected to ADC2_IN18.
// The conversion range is between 1.7 V and 3.6 V.It can measure
// the device junction temperature ranging from ?40 to + 125 �C.bool
//

CLR_INT32 AdcIO::MaximumValue()
{
    return 65535;
}
int AdcIO::MinimumValue()
{
    return 0;
}
int AdcIO::Resolution()
{
    return 16;
}
int AdcIO::ChannelCount()
{
    return 3;
}
bool AdcIO::Initialize()
{
    {
        // int GPIOPortByNumber = pinNameValue / 16;
        // if (!LL_AHB1_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]))
        //{
        //     LL_AHB1_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
        // }
        // return true;

        //// Enable ADC gpio
        // LL_AHB1_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);

        // #define POT1_CLK_ENABLE()  __HAL_RCC_ADC12_CLK_ENABLE()
        // GPIO_InitTypeDef gpio_init_structure;
        // POT1_CLK_ENABLE();
        // POT1_CHANNEL_GPIO_CLK_ENABLE();
        //
        // /* ADC Channel GPIO pin configuration */
        // gpio_init_structure.Pin = POT1_CHANNEL_GPIO_PIN;
        // gpio_init_structure.Mode = GPIO_MODE_ANALOG;
        // gpio_init_structure.Pull = GPIO_NOPULL;
        // HAL_GPIO_Init(POT1_CHANNEL_GPIO_PORT, &gpio_init_structure);
    }

    return true;
}
bool AdcIO::Dispose(CLR_INT32 adc_channel_number)
{
    //  PinNameValue pinNameValue = GetAdcPinNameValueFromChannelNumber(adc_channel_number);

    (void)adc_channel_number;
    {
        // #define POT1_CLK_DISABLE() __HAL_RCC_ADC12_CLK_DISABLE()
        // GPIO_InitTypeDef gpio_init_structure;
        //
        //
        // /* POT GPIO pin configuration */
        // gpio_init_structure.Pin = POT1_CHANNEL_GPIO_PIN;
        // HAL_GPIO_DeInit(POT1_CHANNEL_GPIO_PORT, gpio_init_structure.Pin);
        //
        // /* Disable ADC clock */
        // POT1_CLK_DISABLE();
    }
    return true;
}
bool AdcIO::Open(CLR_INT32 adc_channel_number)
{
    (void)adc_channel_number;
    {

        // ADC1_MspInit(&hpot_adc[POT]);
        // if (MX_ADC1_Init(&hpot_adc[POT]) != HAL_OK)
        // {
        //     ret = BSP_ERROR_PERIPH_FAILURE;
        // }
    }
    return true;
}
CLR_UINT16 AdcIO::Read(CLR_INT32 adc_channel_number)
{
    (void)adc_channel_number;
    {
        // if (HAL_ADC_Start(&hpot_adc[POT]) == HAL_OK)
        // {
        //     if (HAL_ADC_PollForConversion(&hpot_adc[POT], POT_ADC_POLL_TIMEOUT) == HAL_OK)
        //     {
        //         if (HAL_ADC_GetValue(&hpot_adc[POT]) <= (uint32_t)0xFFF)
        //         {
        //             ret = (int32_t)POT_CONVERT2PERC((uint16_t)HAL_ADC_GetValue(&hpot_adc[POT]));
        //         }
        //     }
        // }
    }
    return 1;
}
//__weak HAL_StatusTypeDef MX_ADC1_Init(ADC_HandleTypeDef *hadc)
//{
//    HAL_StatusTypeDef ret = HAL_OK;
//    ADC_ChannelConfTypeDef sConfig;
//
//    /* ADC configuration */
//    hadc->Instance = POT1_ADC;
//    hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
//    hadc->Init.Resolution = ADC_RESOLUTION_12B;
//    hadc->Init.ScanConvMode = (uint32_t)DISABLE;
// #if defined(ADC_EOC_SINGLE_CONV)
//    hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
// #endif
//    hadc->Init.ContinuousConvMode = DISABLE;
//    hadc->Init.NbrOfConversion = 1;
//    hadc->Init.DiscontinuousConvMode = DISABLE;
//    hadc->Init.NbrOfDiscConversion = 0;
//    hadc->Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_CC1;
//    hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
//
//    if (HAL_ADC_Init(hadc) != HAL_OK)
//    {
//        ret = HAL_ERROR;
//    }
//    else
//    {
//        /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample
//        time.
//         */
//        sConfig.Channel = POT1_ADC_CHANNEL;
//        sConfig.Rank = 1;
//        sConfig.SamplingTime = POT1_SAMPLING_TIME;
//        sConfig.Offset = 0;
// #if defined(ADC_SINGLE_ENDED)
//        sConfig.SingleDiff = ADC_SINGLE_ENDED;
// #endif
// #if defined(ADC_OFFSET_NONE)
//        sConfig.OffsetNumber = ADC_OFFSET_NONE;
// #endif
//        if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
//        {
//            ret = HAL_ERROR;
//        }
//    }
//
//    return ret;
//}
PinNameValue AdcIO::ChannelToPin(CLR_INT32 adc_channel_number)
{
    (void)adc_channel_number;
    return (PinNameValue)1;
}
CLR_UINT16 AdcIO::IsModeSupported(AdcChannelMode requestedMode)
{
    bool modeSupported = false;
    if (requestedMode == AdcChannelMode::AdcChannelMode_SingleEnded)
    {
        modeSupported = true;
    }
    if (requestedMode == AdcChannelMode::AdcChannelMode_Differential)
    {
        modeSupported = false;
    }
    return modeSupported;
}
#pragma endregion

#pragma region DAC
// 1� 12-bit single-channel DAC (in SRD domain)
// 1� 12-bit dual-channel DAC
bool DacIO::Initialize(CLR_INT32 dac_channel_number)
{
    (void)dac_channel_number;
    return true;
}
bool DacIO::Dispose(CLR_INT32 dac_channel_number)
{
    (void)dac_channel_number;
    return true;
}
CLR_INT32 DacIO::Resolution()
{
    return 1;
}
CLR_INT32 DacIO::ChannelCount()
{
    return 1;
}
bool DacIO::Open(CLR_INT32 controllerId, CLR_INT32 dac_channel_number)
{
    (void)dac_channel_number;

    CLR_INT32 llDACChannel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(controllerId);
    if (dac_channel_number == 1)
    {
        llDACChannel = LL_DAC_CHANNEL_1;
    }
    else
    {
        llDACChannel = LL_DAC_CHANNEL_2;
    }
    LL_DAC_InitTypeDef DAC_InitStruct;
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC12);
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);

    DAC_InitStruct.TriggerSource = LL_DAC_TRIG_SOFTWARE;
    DAC_InitStruct.WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE;
    DAC_InitStruct.WaveAutoGenerationConfig = LL_DAC_NOISE_LFSR_UNMASK_BIT0;
    DAC_InitStruct.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE;
    DAC_InitStruct.OutputConnection = LL_DAC_OUTPUT_CONNECT_GPIO;
    DAC_InitStruct.OutputMode = LL_DAC_OUTPUT_MODE_NORMAL;
    LL_DAC_Init(DAC_Channel[dac_channel_number], llDACChannel, &DAC_InitStruct);
    return false;
}
char *DacIO::DeviceSelector(int controllerId)
{
    CLR_INT32 llDACChannel = __LL_DAC_DECIMAL_NB_TO_CHANNEL(controllerId);
    static char DeviceNameDummy[] = "NotSupported\0";
    switch (llDACChannel)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            break;
    }
    return DeviceNameDummy;
}
void DacIO::Write(CLR_INT32 dac_channel_number, CLR_INT32 value)
{
    (void)dac_channel_number;
    (void)value;
}
PinNameValue DacIO::ChannelToPin(CLR_INT32 dac_channel_number)
{
    (void)dac_channel_number;
    return (PinNameValue)1;
}
#pragma endregion

#pragma region I2C
bool I2cIO::Initialize(CLR_INT32 I2C_deviceId, I2cBusSpeed I2C_speed)
{
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);

    switch (I2C_deviceId)
    {
        case 1:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            break;
        case 2:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            break;
        case 3:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            break;
        case 4:
            LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_I2C4);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C4_CLKSOURCE_PCLK4);
            break;
#ifdef I2C5
        case 5:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C5);
            // LL_RCC_SetI2CClockSource(LL_RCC_I2C5_CLKSOURCE_PCLK1);
            break;
#endif
    }
    LL_I2C_Disable(I2C_Channel[I2C_deviceId]);
    {
        LL_I2C_ConfigFilters(I2C_Channel[I2C_deviceId], LL_I2C_ANALOGFILTER_ENABLE, 0);
        LL_I2C_SetTiming(I2C_Channel[I2C_deviceId], I2C_GetTiming(I2C_speed));
        LL_I2C_SetMode(I2C_Channel[I2C_deviceId], LL_I2C_MODE_I2C);
        LL_I2C_DisableClockStretching(I2C_Channel[I2C_deviceId]);
        LL_I2C_DisableGeneralCall(I2C_Channel[I2C_deviceId]);

        LL_I2C_SetOwnAddress1(I2C_Channel[I2C_deviceId], 0, LL_I2C_OWNADDRESS1_7BIT);
        LL_I2C_SetMasterAddressingMode(I2C_Channel[I2C_deviceId], LL_I2C_ADDRESSING_MODE_7BIT);
        LL_I2C_AcknowledgeNextData(I2C_Channel[I2C_deviceId], LL_I2C_ACK);
        LL_I2C_DisableOwnAddress1(I2C_Channel[I2C_deviceId]);
        LL_I2C_DisableOwnAddress2(I2C_Channel[I2C_deviceId]);

        // Interrupt versions ( TO BE CONTINUED )
        //// Enable Receive Interrupt
        // LL_I2C_EnableIT_RX(I2C_Channel[busIndex]);
        //// Enable Not acknowledge received interrupt
        // LL_I2C_EnableIT_NACK(I2C_Channel[busIndex]);
        //// Enable Error interrupts
        // LL_I2C_EnableIT_ERR(I2C_Channel[busIndex]);
        //// Enable Stop interrupt
        // LL_I2C_EnableIT_STOP(I2C_Channel[busIndex]);
    }
    LL_I2C_Enable(I2C_Channel[I2C_deviceId]);
    NVIC_SetPriority(I2C_Interrupt_Channel[I2C_deviceId], 0);
    NVIC_EnableIRQ(I2C_Interrupt_Channel[I2C_deviceId]);

    return true;
}
bool I2cIO::Dispose(CLR_INT32 I2C_deviceId)
{
    bool status = false;
    switch (I2C_deviceId)
    {
        case 1:
            LL_APB4_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);
            LL_APB4_GRP1_ForceReset(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            status = true;
            break;
        case 2:
            LL_APB4_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C2);
            LL_APB4_GRP1_ForceReset(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            status = true;
            break;
        case 3:
            LL_APB4_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C3);
            LL_APB4_GRP1_ForceReset(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            status = true;
            break;
        case 4:
            LL_APB4_GRP1_ReleaseReset(LL_APB4_GRP1_PERIPH_I2C4);
            LL_APB4_GRP1_ForceReset(LL_RCC_I2C4_CLKSOURCE_PCLK4);
            status = true;
            break;
#ifdef I2C5
        case 5:
            LL_APB4_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C5);
            // LL_APB4_GRP1_ForceReset(LL_RCC_I2C5_CLKSOURCE_PCLK1);
            status = true;
            break;
#endif
        default:
            status = false;
            break;
    }
    if (status)
    {
        PinNameValue pinNameValueSCL =
            DeviceRegistration::FindPinWithFunctionAndChannel(DeviceRegistration::DevicePinFunction::I2C, I2C_deviceId);
        PinNameValue pinNameValueSDA =
            DeviceRegistration::FindPinWithFunctionAndChannel(DeviceRegistration::DevicePinFunction::I2C, I2C_deviceId);
        GpioIO::SetLowPower(pinNameValueSCL);
        GpioIO::SetLowPower(pinNameValueSDA);
    }
    return status;
}
CLR_INT32 I2cIO::Write(
    CLR_INT32 I2C_deviceId,
    CLR_INT32 slaveAddress,
    CLR_UINT8 *writeBuffer,
    CLR_INT32 writeSize,
    CLR_INT32 *transferResult)
{
    bool status = false;
    LL_I2C_HandleTransfer(
        I2C_Channel[I2C_deviceId],
        slaveAddress,
        LL_I2C_ADDRSLAVE_7BIT,
        writeSize,
        LL_I2C_MODE_SOFTEND,
        LL_I2C_GENERATE_START_WRITE);
    LL_I2C_TransmitData8(I2C_Channel[I2C_deviceId], 0);
    while (!LL_I2C_IsActiveFlag_TXE(I2C_Channel[I2C_deviceId]))
        ;
    // sending the data
    for (uint8_t i = 0; i < writeSize; i++)
    {
        LL_I2C_TransmitData8(I2C_Channel[I2C_deviceId], writeBuffer[i]);
        while (!LL_I2C_IsActiveFlag_TXE(I2C_Channel[I2C_deviceId]))
            writeBuffer++;
    }
    status = true;
    *transferResult = 1;
    return status;
}

bool I2cIO::Read(
    CLR_INT32 I2C_deviceId,
    CLR_INT32 slaveAddress,
    CLR_UINT8 *readBuffer,
    CLR_INT32 readSize,
    CLR_INT32 *transferResult)
{
    (void)transferResult;

    bool status = false;
    LL_I2C_AcknowledgeNextData(I2C_Channel[I2C_deviceId], LL_I2C_ACK);
    LL_I2C_HandleTransfer(
        I2C_Channel[I2C_deviceId],
        (slaveAddress | 1),
        LL_I2C_ADDRSLAVE_7BIT,
        readSize,
        LL_I2C_MODE_AUTOEND,
        LL_I2C_GENERATE_START_READ);
    for (uint8_t i = 0; i < readSize; i++)
    {
        if (i < readSize - 1)
            LL_I2C_AcknowledgeNextData(I2C_Channel[I2C_deviceId], LL_I2C_ACK);
        else
            LL_I2C_AcknowledgeNextData(I2C_Channel[I2C_deviceId], LL_I2C_NACK);
        while (LL_I2C_IsActiveFlag_RXNE(I2C_Channel[I2C_deviceId]) == 0)
            ;
        readBuffer[i] = LL_I2C_ReceiveData8(I2C_Channel[I2C_deviceId]);
    }
    status = true;
    // Optional?
    // LL_I2C_GenerateStopCondition(I2C_Channel[busIndex]);
    return status;
}
#pragma endregion

#pragma region I2c_Slave
CLR_INT32 I2cIO_Slave::TimeoutMaximum()
{
    return 10000;
}
CLR_INT32 I2cIO_Slave::Timeout()
{
    return 100;
}
bool I2cIO_Slave::Initialize(CLR_INT32 I2C_deviceId, CLR_INT32 deviceAddress)
{
    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);

    switch (I2C_deviceId)
    {
        case 1:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            break;
        case 2:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            break;
        case 3:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            break;
        case 4:
            LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_I2C4);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C4_CLKSOURCE_PCLK4);
            break;
#ifdef I2C5
        case 5:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C5);
            // LL_RCC_SetI2CClockSource(LL_RCC_I2C5_CLKSOURCE_PCLK1);
            break;
#endif
    }

    I2cBusSpeed TODOSPEED = I2cBusSpeed::I2cBusSpeed_StandardMode;
    LL_I2C_Disable(I2C_Channel[I2C_deviceId]);
    {
        LL_I2C_ConfigFilters(I2C_Channel[I2C_deviceId], LL_I2C_ANALOGFILTER_ENABLE, 0);
        LL_I2C_SetTiming(I2C_Channel[I2C_deviceId], I2C_GetTiming(TODOSPEED));
        LL_I2C_SetMode(I2C_Channel[I2C_deviceId], LL_I2C_MODE_I2C);
        LL_I2C_DisableClockStretching(I2C_Channel[I2C_deviceId]);
        LL_I2C_DisableGeneralCall(I2C_Channel[I2C_deviceId]);

        // Reset Value is Clock stretching enabled
        LL_I2C_SetOwnAddress1(I2C_Channel[I2C_deviceId], deviceAddress, LL_I2C_OWNADDRESS1_7BIT);
        LL_I2C_SetOwnAddress2(I2C_Channel[I2C_deviceId], 0x00, LL_I2C_OWNADDRESS2_NOMASK);
        LL_I2C_EnableOwnAddress1(I2C_Channel[I2C_deviceId]);
        LL_I2C_DisableOwnAddress2(I2C_Channel[I2C_deviceId]);
        LL_I2C_AcknowledgeNextData(I2C_Channel[I2C_deviceId], LL_I2C_NACK);

        // Interrupt versions ( TO BE CONTINUED )
        //// Enable Receive Interrupt
        // LL_I2C_EnableIT_RX(I2C_Channel[busIndex]);
        //// Enable Not acknowledge received interrupt
        // LL_I2C_EnableIT_NACK(I2C_Channel[busIndex]);
        //// Enable Error interrupts
        // LL_I2C_EnableIT_ERR(I2C_Channel[busIndex]);
        //// Enable Stop interrupt
        // LL_I2C_EnableIT_STOP(I2C_Channel[busIndex]);
    }
    LL_I2C_Enable(I2C_Channel[I2C_deviceId]);
    NVIC_SetPriority(I2C_Interrupt_Channel[I2C_deviceId], 0);
    NVIC_EnableIRQ(I2C_Interrupt_Channel[I2C_deviceId]);

    return true;
}
bool I2cIO_Slave::Dispose(CLR_INT32 I2C_deviceId)
{
    (void)I2C_deviceId;
    return true;
}
bool I2cIO_Slave::Write(
    CLR_INT32 I2C_deviceId,
    CLR_UINT8 *writeBuffer,
    CLR_INT32 writeSize,
    CLR_INT32 timeoutMilliseconds,
    CLR_INT32 *readCount,
    CLR_INT32 *transferResult)
{
    (void)I2C_deviceId;
    (void)writeBuffer;
    (void)writeSize;
    (void)timeoutMilliseconds;
    (void)readCount;
    (void)transferResult;
    return true;
}
bool I2cIO_Slave::Read(
    CLR_INT32 I2C_deviceId,
    CLR_UINT8 *readBuffer,
    CLR_INT32 readSize,
    CLR_INT32 timeoutMilliseconds,
    CLR_INT32 *readCount,
    CLR_INT32 *transferResult)
{
    (void)I2C_deviceId;
    (void)readBuffer;
    (void)readSize;
    (void)timeoutMilliseconds;
    (void)readCount;
    (void)transferResult;
    return true;
}
#pragma endregion

#pragma region PWM
bool PwmIO::Initialize(
    CLR_INT32 channelId,
    CLR_INT32 timerId,
    CLR_INT32 pinNumber,
    CLR_INT32 polarity,
    CLR_INT32 desiredFrequency,
    CLR_INT32 dutyCycle)
{
    (void)channelId;
    (void)timerId;
    (void)pinNumber;
    (void)polarity;
    (void)desiredFrequency;
    (void)dutyCycle;
    return true;
}
bool PwmIO::Dispose(CLR_INT32 I2C_deviceId)
{
    // TODO: Deactive PWM, set pins to low power
    // Get each pin and set to low power function
    (void)I2C_deviceId;
    return true;
}
CLR_UINT32 PwmIO::SetDutyCycle(CLR_INT32 pinNumber, CLR_INT32 desiredFrequency)
{
    (void)pinNumber;
    (void)desiredFrequency;
    return 1;
}
CLR_UINT32 PwmIO::Start(CLR_INT32 pinNumber, CLR_INT32 timerId)
{
    (void)pinNumber;
    (void)timerId;
    return 1;
}
CLR_UINT32 PwmIO::Stop(CLR_INT32 pinNumber, CLR_INT32 timerId)
{
    (void)pinNumber;
    (void)timerId;
    return 1;
}
CLR_UINT32 PwmIO::DesiredFrequency(CLR_INT32 timerId, CLR_INT32 desiredFrequency)
{
    (void)timerId;
    (void)desiredFrequency;
    return 1;
}
CLR_UINT32 PwmIO::GetChannel(CLR_INT32 timerId, CLR_INT32 pinNumber)
{
    (void)timerId;
    (void)pinNumber;
    return 1;
}
#pragma endregion

#pragma region Serial IO
#include "CircularBuffer.h"

#define MAXIMUM_NUMBER_OF_ACTIVE_USARTS 5

int CircularBufferAssignments[]{
    -1, // Index 0 not used
    0,  // Usart1 - CircularBuffer[0]
    1,  // Usart2 - CircularBuffer[0]
    2,  // Usart3 - CircularBuffer[0]
    -1, // Usart4 - Not used
    -1, // Usart5 - Not used
    3,  // Usart6 - CircularBuffer[0]
    -1, // Usart7 - Not used
    -1, // Usart8 - Not used
    -1, // Usart9 - Not used
    4   // Usart10 - CircularBuffer[0]
};
CircularBuffer_t DeviceReceiveCircularBuffer[MAXIMUM_NUMBER_OF_ACTIVE_USARTS];
CircularBuffer_t DeviceTransmitCircularBuffer[MAXIMUM_NUMBER_OF_ACTIVE_USARTS];

__attribute__((section(".dma_buffer"))) __attribute__((aligned(32)))
uint8_t UsartDmaReceiveBuffer[512][MAXIMUM_NUMBER_OF_ACTIVE_USARTS];
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32)))
uint8_t UsartTransmitData[512][MAXIMUM_NUMBER_OF_ACTIVE_USARTS];
//  Length of currently active TX DMA transfer
volatile size_t device_usart_tx_dma_current_len[MAXIMUM_NUMBER_OF_ACTIVE_USARTS];
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint8_t DmaReceiveBuffer[1024];
static_assert((sizeof(DmaReceiveBuffer) % 32) == 0, "Must be a multiple by 32");

int GetSerialDeviceBufferIndex(int UsartDeviceNumber)
{
    return CircularBufferAssignments[UsartDeviceNumber];
}

bool SerialIO::Initialize(CLR_INT32 usartDeviceNumber, CLR_INT32 baudrate)
{
    bool status = false;
    USART_TypeDef *USARTx = (USART_TypeDef *)Serial_Channel[usartDeviceNumber].controllerId;
    LL_USART_InitTypeDef USART_InitStruct;

    switch (usartDeviceNumber)
    {
        case 1:
        case 6:
        case 10:
            LL_APB2_GRP1_EnableClock(UsartPortClockEnable[usartDeviceNumber]);
            status = true;
            break;
        case 2:
        case 3:
            LL_APB1_GRP1_EnableClock(UsartPortClockEnable[usartDeviceNumber]);
            status = true;
            break;
        default:
            status = false;
            break;
    }
    if (status)
    {
        LL_AHB1_GRP1_EnableClock(DmaClockEnable[usartDeviceNumber]);
        DMA_TypeDef *DMAx = DMA_Channel[usartDeviceNumber];

        // wpUSART Receive Initialize
        LL_DMA_SetPeriphRequest(DMAx, wpDMA_ReceiveStream, wpDMA_ReceiveMux);
        LL_DMA_SetDataTransferDirection(DMAx, wpDMA_ReceiveStream, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
        LL_DMA_SetStreamPriorityLevel(DMAx, wpDMA_ReceiveStream, LL_DMA_PRIORITY_LOW);
        LL_DMA_SetMode(DMAx, wpDMA_ReceiveStream, LL_DMA_MODE_CIRCULAR);
        LL_DMA_SetPeriphIncMode(DMAx, wpDMA_ReceiveStream, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMAx, wpDMA_ReceiveStream, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetMemorySize(DMAx, wpDMA_ReceiveStream, LL_DMA_MDATAALIGN_BYTE);
        LL_DMA_SetPeriphSize(DMAx, wpDMA_ReceiveStream, LL_DMA_PDATAALIGN_BYTE);
        LL_DMA_DisableFifoMode(DMAx, wpDMA_ReceiveStream);
        LL_DMA_SetPeriphAddress(
            DMAx,
            wpDMA_ReceiveStream,
            LL_USART_DMA_GetRegAddr(USARTx, LL_USART_DMA_REG_DATA_RECEIVE));
        LL_DMA_SetMemoryAddress(DMAx, wpDMA_ReceiveStream, (uint32_t)DmaReceiveBuffer);
        LL_DMA_SetDataLength(DMAx, wpDMA_ReceiveStream, ARRAY_LEN(DmaReceiveBuffer));

        // wpUSART Transmit Initialize
        LL_DMA_SetPeriphRequest(DMAx, wpDMA_TransmitStream, wpDMA_TransmitMux);
        LL_DMA_SetDataTransferDirection(DMAx, wpDMA_TransmitStream, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
        LL_DMA_SetStreamPriorityLevel(DMAx, wpDMA_TransmitStream, LL_DMA_PRIORITY_LOW);
        LL_DMA_SetMode(DMAx, wpDMA_TransmitStream, LL_DMA_MODE_NORMAL);
        LL_DMA_SetPeriphIncMode(DMAx, wpDMA_TransmitStream, LL_DMA_PERIPH_NOINCREMENT);
        LL_DMA_SetMemoryIncMode(DMAx, wpDMA_TransmitStream, LL_DMA_MEMORY_INCREMENT);
        LL_DMA_SetPeriphSize(DMAx, wpDMA_TransmitStream, LL_DMA_PDATAALIGN_BYTE);
        LL_DMA_SetMemorySize(DMAx, wpDMA_TransmitStream, LL_DMA_MDATAALIGN_BYTE);
        LL_DMA_DisableFifoMode(DMAx, wpDMA_TransmitStream);
        LL_DMA_SetPeriphAddress(
            DMAx,
            wpDMA_TransmitStream,
            LL_USART_DMA_GetRegAddr(USARTx, LL_USART_DMA_REG_DATA_TRANSMIT));

        // Enable receive half transfer interrupt
        LL_DMA_EnableIT_HT(DMAx, wpDMA_ReceiveStream);

        // Enable receive transfer complete interrupt
        LL_DMA_EnableIT_TC(DMAx, wpDMA_ReceiveStream);

        // Enable transmit transfer complete interrupt
        LL_DMA_EnableIT_TC(DMAx, wpDMA_TransmitStream);

        // DMA interrupt initialize
        NVIC_SetPriority(wpDMA_ReceiveStreamInterrupt, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(wpDMA_ReceiveStreamInterrupt);
        NVIC_SetPriority(wpDMA_TransmitStreamInterrupt, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
        NVIC_EnableIRQ(wpDMA_TransmitStreamInterrupt);

        // Configure WireProtocol wpUSART
        USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
        USART_InitStruct.BaudRate = baudrate;
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
        USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
        USART_InitStruct.Parity = LL_USART_PARITY_NONE;
        USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
        USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
        USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

        LL_USART_Disable(USARTx);
        {
            LL_USART_Init(USARTx, &USART_InitStruct);
            LL_USART_SetTXFIFOThreshold(USARTx, LL_USART_FIFOTHRESHOLD_7_8);
            LL_USART_SetRXFIFOThreshold(USARTx, LL_USART_FIFOTHRESHOLD_7_8);
            LL_USART_EnableFIFO(USARTx);
            LL_USART_ConfigAsyncMode(USARTx);
            LL_USART_EnableDMAReq_RX(USARTx);
            LL_USART_EnableDMAReq_TX(USARTx);
            LL_USART_EnableIT_IDLE(USARTx);

            // USART interrupt, same priority as DMA channel
            NVIC_SetPriority(wpUSART_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
            NVIC_EnableIRQ(wpUSART_IRQn);

            LL_DMA_EnableStream(wpDMA, wpDMA_ReceiveStream);
        }
        LL_USART_Enable(USARTx);

        // Polling USARTx initialization
        while (!LL_USART_IsActiveFlag_TEACK(USARTx) || !LL_USART_IsActiveFlag_REACK(USARTx))
        {
        }
    }
    return status;
}
bool SerialIO::Dispose(CLR_INT32 usartDeviceNumber)
{
    DMA_TypeDef *DMAx = DMA_Channel[usartDeviceNumber];
    LL_DMA_DeInit(DMAx, DmaStreamNumber[usartDeviceNumber]);
    return true;
}
CLR_INT32 SerialIO::BytesAvailable(CLR_INT32 usartDeviceNumber)
{
    int index = GetSerialDeviceBufferIndex(usartDeviceNumber);
    return wp_BufferBytesWaiting(&DeviceReceiveCircularBuffer[index]);
}
bool SerialIO::ReadBytes(CLR_INT32 usartDeviceNumber, CLR_UINT8 *data, CLR_INT32 bytesRequested)
{
    int bytesAvailableToRead = SerialIO::BytesAvailable(usartDeviceNumber);
    if (bytesRequested > bytesAvailableToRead)
    {
        bytesRequested = bytesAvailableToRead;
    }
    int index = GetSerialDeviceBufferIndex(usartDeviceNumber);
    ULONG read = wp_ReadBuffer(&DeviceReceiveCircularBuffer[index], data, bytesRequested);
    return read;
}
bool SerialIO::ReadLine(CLR_INT32 usartDeviceNumber, char *newLine, CLR_UINT8 length, char *line)
{
    UNUSED(usartDeviceNumber);
    UNUSED(newLine);
    UNUSED(line);
    UNUSED(length);
    return true;
}
bool SerialIO::SetSignalLevels(CLR_INT32 usartDeviceNumber, bool inversion)
{
    UNUSED(usartDeviceNumber);
    UNUSED(inversion);
    return true;
}
bool SerialIO::GetSignalLevels(CLR_INT32 usartDeviceNumber)
{
    UNUSED(usartDeviceNumber);
    return true;
}
bool SerialIO::WriteBytes(CLR_INT32 usartDeviceNumber, CLR_UINT8 *data, CLR_INT32 size)
{
    int index = GetSerialDeviceBufferIndex(usartDeviceNumber);
    wp_WriteBuffer(&DeviceTransmitCircularBuffer[index], data, size);
    return true;
}
bool SerialIO::SetWatchCharacter(CLR_INT32 usartDeviceNumber, CLR_UINT8 watch_character)
{
    int index = GetSerialDeviceBufferIndex(usartDeviceNumber);
    UNUSED(index);
    UNUSED(watch_character);
    return true;
}
bool SerialIO::SetReceiveThreshold(CLR_INT32 usartDeviceNumber, CLR_INT32 threshold)
{
    int index = GetSerialDeviceBufferIndex(usartDeviceNumber);
    UNUSED(threshold);
    UNUSED(index);
    return 1;
}
char *SerialIO::GetDevice(CLR_INT32 usartDeviceNumber)
{
    static char DeviceSelector[1];
    int index = GetSerialDeviceBufferIndex(usartDeviceNumber);

    UNUSED(index);
    DeviceSelector[0] = ' ';
    return DeviceSelector;
}
bool SerialIO::InvertSignalLevels(CLR_INT32 usartDeviceNumber, bool InvertSignal)
{
    USART_TypeDef *USARTx = (USART_TypeDef *)Serial_Channel[usartDeviceNumber].controllerId;
    if (InvertSignal)
    {
        LL_USART_SetRXPinLevel(USARTx, LL_USART_RXPIN_LEVEL_INVERTED);
    }
    else
    {
        LL_USART_SetRXPinLevel(USARTx, LL_USART_RXPIN_LEVEL_STANDARD);
    }
    return true;
}
bool SerialIO::SetBaudRate(CLR_INT32 usartDeviceNumber, CLR_INT32 baudRate)
{
    UNUSED(usartDeviceNumber);
    UNUSED(baudRate);
    return true;
}
bool SerialIO::SetConfig(CLR_INT32 usartDeviceNumber, CLR_INT32 stopBits, CLR_INT32 dataBits, CLR_INT32 RequestedParity)
{
    UNUSED(usartDeviceNumber);
    UNUSED(stopBits);
    UNUSED(dataBits);
    UNUSED(RequestedParity);
    return true;
}
bool SerialIO::SetHandshake(CLR_INT32 usartDeviceNumber, Handshake handshake)
{
    UNUSED(usartDeviceNumber);

    bool status = false;
    switch (handshake)
    {
        case Handshake::Handshake_None:
            status = true;
            break;
        case Handshake_XOnXOff:
            break;
        case Handshake::Handshake_RequestToSend:
            status = true;
            break;
        case Handshake::Handshake_RequestToSendXOnXOff:
            status = true;
            break;
    }
    return status;
}
bool SerialIO::SetMode(CLR_INT32 usartDeviceNumber, CLR_INT32 mode)
{
    UNUSED(usartDeviceNumber);
    UNUSED(mode);
    return true;
}

HRESULT SerialIO::SetupWriteLine(CLR_RT_StackFrame &stack, char **buffer, uint32_t *length, bool *isNewAllocation)
{
    UNUSED(stack);
    UNUSED(buffer);
    UNUSED(length);
    UNUSED(isNewAllocation);
    return 1;
}

#pragma endregion

#pragma region SPI
CLR_INT32 SpiIO::MaximumClockFrequency(CLR_INT32 deviceId)
{
    UNUSED(deviceId);
    return 1;
}
CLR_INT32 SpiIO::MinimumClockFrequency(CLR_INT32 deviceId)
{
    UNUSED(deviceId);
    return 1;
}
bool SpiIO::Initialize()
{
    return true;
}
bool SpiIO::Dispose(CLR_INT32 deviceId)
{
    UNUSED(deviceId);
    return true;
}
bool SpiIO::WriteRead(
    CLR_INT32 deviceId,
    SPI_WRITE_READ_SETTINGS rws,
    CLR_UINT8 *writeData,
    CLR_UINT16 writeSize,
    CLR_UINT8 *readData,
    CLR_UINT16 readSize)
{
    UNUSED(deviceId);
    UNUSED(rws);
    UNUSED(*writeData);
    UNUSED(writeSize);
    UNUSED(*readData);
    UNUSED(readSize);
    return false;
}
bool SpiIO::Open(SPI_DEVICE_CONFIGURATION spiConfig, CLR_UINT32 handle)
{
    (void)spiConfig;
    (void)handle;

    return false;
}

CLR_INT32 SpiIO::ByteTime()
{
    return 1;
}
SPI_OP_STATUS SpiIO::Completed(CLR_INT32 deviceId)
{
    UNUSED(deviceId);
    return SPI_OP_STATUS::SPI_OP_COMPLETE;
}

#pragma endregion
