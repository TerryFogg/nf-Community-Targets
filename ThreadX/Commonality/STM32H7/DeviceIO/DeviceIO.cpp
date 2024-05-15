//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "board.h"
#include "DeviceIO.h"
#include "DeviceRegistration.h"
#include "wp_CircularBuffer.h"

#define MAXIMUM_NUMBER_OF_ACTIVE_USARTS 5

#pragma region USART BUFFERS

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

__attribute__((section(".dma_buffer")))
__attribute__((aligned(32)))
uint8_t UsartDmaReceiveBuffer[512][MAXIMUM_NUMBER_OF_ACTIVE_USARTS];
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32)))
uint8_t UsartTransmitData[512][MAXIMUM_NUMBER_OF_ACTIVE_USARTS];
//  Length of currently active TX DMA transfer
volatile size_t device_usart_tx_dma_current_len[MAXIMUM_NUMBER_OF_ACTIVE_USARTS];

#pragma endregion

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
SerialChannel Serial_Channel[] = {
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

GPIO_INTERRUPT_SERVICE_ROUTINE SavedInterruptCallBack[16];
int16_t SavedInterruptPinNumber[16];

#pragma region GPIO Pins
bool SetPinLowPower(GPIO_PIN pinNumber)
{
    bool status = false;
    if (IsValidPin(pinNumber))
    {
        int GPIOPortByNumber = pinNumber / 16;
        uint32_t pin = pinNumber % 16;

        // Set the unused pins as input(floating) to minimize current consumption.
        LL_GPIO_SetPinMode(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_MODE_INPUT);
        LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_PULL_NO);
        status = true;
    }
    return status;
}
bool SetGpioDriveMode(GPIO_PIN pinNumber, PinMode driveMode)
{
    int GPIOPortByNumber = pinNumber / 16;
    uint32_t pin = pinNumber % 16;
    bool status = false;
    if (IsValidPin(pinNumber))
    {
        LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
        switch (driveMode)
        {
            case PinMode_Input:
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_MODE_INPUT);
                LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_PULL_NO);
                status = true;
                break;
            case PinMode_InputPullDown:
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_MODE_INPUT);
                LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_PULL_DOWN);
                status = true;
                break;
            case PinMode_InputPullUp:
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_MODE_INPUT);
                LL_GPIO_SetPinPull(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_PULL_UP);
                status = true;
                break;
            case PinMode_Output:
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_OUTPUT_PUSHPULL);
                status = true;
                break;
            case PinMode_OutputOpenDrain:
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], PinMask[pin], LL_GPIO_OUTPUT_OPENDRAIN);
                status = true;
                break;
            default:
                status = false;
                break;
        }
    }
    return status;
}
bool SetPinState(GPIO_PIN pinNumber, GpioPinValue pinState)
{
    bool status = false;
    if (IsValidPin(pinNumber))
    {
        int GPIOPortByNumber = pinNumber / 16;
        uint32_t pin = pinNumber % 16;

        if (pinState == GpioPinValue_Low)
        {
            LL_GPIO_ResetOutputPin(Port[GPIOPortByNumber], PinMask[pin]);
            status = true;
        }
        else if (GpioPinValue_High)
        {
            LL_GPIO_SetOutputPin(Port[GPIOPortByNumber], PinMask[pin]);
            status = true;
        }
    }
    return status;
}
bool SetPinFunction(GPIO_PIN pinNumber, DevicePinFunction alternateFunction, int optional)
{
    bool status = false;
    if (IsValidPin(pinNumber))
    {
        int GPIOPortByNumber = pinNumber / 16;
        uint32_t pin = pinNumber % 16;
        LL_GPIO_InitTypeDef GPIO_InitStruct;

        int AlternateFunctionNumber = GetI2CPin_AlternateFunctionNumber(GPIOPortByNumber, pin, alternateFunction);
        switch ((DevicePinFunction)alternateFunction)
        {
            case NONE:
                status = true;
                break;
            case ADC:
                // Enable ADC gpio
                LL_AHB1_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
                LL_GPIO_SetPinMode(Port[GPIOPortByNumber], pin, LL_GPIO_MODE_ANALOG);
                status = true;
                break;
            case CAN:
                status = true;
                break;
            case DAC:
                status = true;
                break;
            case FDCAN:
                status = true;
                break;
            case GPIO_OUTPUT:
                status = true;
                break;
            case GPIO_INPUT:
                status = true;
                break;
            case SPI:
                status = true;
                break;
            case COUNTER:
                status = true;
                break;
            case PWM:
                status = true;
                break;
            case SD:
                // Enable the clock for SD1 or SD2 on different AHB buses
                if (optional == 1)
                {
                    LL_AHB3_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
                }
                else
                {
                    LL_AHB2_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
                }
                GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
                GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
                GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
                GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
                GPIO_InitStruct.Alternate = LL_GPIO_AF_12;
                LL_GPIO_Init(Port[GPIOPortByNumber], &GPIO_InitStruct);
                status = true;
                break;
            case TIMER:
                status = true;
                break;
            case TIMER_LOW_POWER:
                status = true;
                break;
            case I2C_SCL:
                LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
                LL_GPIO_SetPinMode(Port[GPIOPortByNumber], pin, LL_GPIO_MODE_ALTERNATE);
                if (pin <= 7)
                {
                    LL_GPIO_SetAFPin_8_15(Port[GPIOPortByNumber], pin, AlternateFunctionNumber);
                }
                if (pin >= 8)
                {
                    LL_GPIO_SetAFPin_8_15(Port[GPIOPortByNumber], pin, AlternateFunctionNumber);
                }
                LL_GPIO_SetPinSpeed(Port[GPIOPortByNumber], pin, LL_GPIO_SPEED_FREQ_HIGH);
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], pin, LL_GPIO_OUTPUT_OPENDRAIN);
                status = true;
                break;
            case I2C_SDA:
                LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
                LL_GPIO_SetPinMode(Port[GPIOPortByNumber], pin, LL_GPIO_MODE_ALTERNATE);
                if (pin <= 7)
                {
                    LL_GPIO_SetAFPin_8_15(Port[GPIOPortByNumber], pin, AlternateFunctionNumber);
                }
                if (pinNumber >= 8)
                {
                    LL_GPIO_SetAFPin_8_15(Port[GPIOPortByNumber], pin, AlternateFunctionNumber);
                }
                LL_GPIO_SetPinSpeed(Port[GPIOPortByNumber], pin, LL_GPIO_SPEED_FREQ_HIGH);
                LL_GPIO_SetPinOutputType(Port[GPIOPortByNumber], pin, LL_GPIO_OUTPUT_OPENDRAIN);
                status = true;
                break;
            case I2S:
                status = true;
                break;
            case USART_TX:
            case USART_RX:
                LL_AHB4_GRP1_EnableClock(GpioPortClockEnable[GPIOPortByNumber]);
                GPIO_InitStruct.Alternate = AlternateFunctionNumber;
                GPIO_InitStruct.Pin = pin;
                GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
                GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
                GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
                GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
                LL_GPIO_Init(Port[GPIOPortByNumber], &GPIO_InitStruct);
                status = true;
                break;
            case USART_LOW_POWER_TX:
                status = true;
                break;
            case USART_LOW_POWER_RX:
                status = true;
                break;
            case WAKEUP:
                status = true;
                break;
            default:
                status = false;
                break;
        }
    }
    return status;
}
GpioPinValue GetPinState(GPIO_PIN pinNumber)
{
    GpioPinValue pin_state = (GpioPinValue)-1;
    if (IsValidPin(pinNumber))
    {
        int GPIOPortByNumber = pinNumber / 16;
        uint32_t pin = pinNumber % 16;
        pin_state = (GpioPinValue)LL_GPIO_IsInputPinSet(Port[GPIOPortByNumber], PinMask[pin]);
    }
    return pin_state;
}
bool TogglePinState(GPIO_PIN pinNumber)
{
    bool status = false;
    if (IsValidPin(pinNumber))
    {
        int GPIOPortByNumber = pinNumber / 16;
        uint32_t pin = pinNumber % 16;
        LL_GPIO_TogglePin(Port[GPIOPortByNumber], PinMask[pin]);
        status = true;
    }
    return status;
}
bool PinHasInterrupt(int pin)
{
    bool status = false;
    for (int i = 0; i < 15; i++)
    {
        if (SavedInterruptPinNumber[i] == pin)
        {
            return true;
        }
    }
    return status;
}
#pragma endregion

#pragma region GPIO Interrupt
bool EnablePinInterrupt(int pinNumber, GpioParameter *gpioParameter)
{
    // NOTE: There is a limitation of 16 external interrupts on the GPIO lines with one 1 interrupt per line number.
    //      One interrupt on (PA0 or PB0 or PC0 or PD0...) and one interrupt on (PA1, or PB1 or PC1 ...) etc.

    bool status = false;
    if (IsValidPin(pinNumber))
    {
        int GPIOPortByNumber = pinNumber / 16;
        uint32_t pin = pinNumber % 16;

        uint32_t priority = 0;
        if (!PinHasInterrupt(pin))
        {
            SavedInterruptCallBack[pin] = gpioParameter->isrPtr;
            LL_SYSCFG_SetEXTISource(SyscfgPorts[GPIOPortByNumber], PinMask[pin]);
            switch ((GPIO_INT_EDGE)gpioParameter->edgeTrigger)
            {
                case GPIO_INT_EDGE_LOW:
                case GPIO_INT_LEVEL_LOW:
                    gpioParameter->expectedState = false;
                    LL_EXTI_EnableFallingTrig_0_31(EXTI_Line[pin]);
                    LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[pin]);
                    status = true;
                    break;
                case GPIO_INT_EDGE_HIGH:
                case GPIO_INT_LEVEL_HIGH:
                    LL_EXTI_EnableRisingTrig_0_31(EXTI_Line[pin]);
                    LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[pin]);
                    gpioParameter->expectedState = true;
                    status = true;
                    break;
                case GPIO_INT_EDGE_BOTH:
                    LL_EXTI_EnableFallingTrig_0_31(EXTI_Line[pin]);
                    LL_EXTI_EnableRisingTrig_0_31(EXTI_Line[pin]);
                    gpioParameter->expectedState = true;
                    status = true;
                    break;
                default:
                    status = false;
                    break;
            }
            switch (pin)
            {
                case 0:
                    NVIC_SetPriority(EXTI0_IRQn, priority);
                    NVIC_EnableIRQ(EXTI0_IRQn);
                    status = true;
                    break;
                case 1:
                    NVIC_SetPriority(EXTI1_IRQn, priority);
                    NVIC_EnableIRQ(EXTI1_IRQn);
                    status = true;
                    break;
                case 2:
                    NVIC_SetPriority(EXTI2_IRQn, priority);
                    NVIC_EnableIRQ(EXTI2_IRQn);
                    status = true;
                    break;
                case 3:
                    NVIC_SetPriority(EXTI3_IRQn, priority);
                    NVIC_EnableIRQ(EXTI3_IRQn);
                    status = true;
                    break;
                case 4:
                    NVIC_SetPriority(EXTI4_IRQn, priority);
                    NVIC_EnableIRQ(EXTI4_IRQn);
                    status = true;
                    break;
                case 5:
                    NVIC_SetPriority(EXTI9_5_IRQn, priority);
                    NVIC_EnableIRQ(EXTI9_5_IRQn);
                    status = true;
                    break;
                case 6:
                    NVIC_SetPriority(EXTI9_5_IRQn, priority);
                    NVIC_EnableIRQ(EXTI9_5_IRQn);
                    status = true;
                    break;
                case 7:
                    NVIC_SetPriority(EXTI9_5_IRQn, priority);
                    NVIC_EnableIRQ(EXTI9_5_IRQn);
                    status = true;
                    break;
                case 8:
                    NVIC_SetPriority(EXTI9_5_IRQn, priority);
                    NVIC_EnableIRQ(EXTI9_5_IRQn);
                    status = true;
                    break;
                case 9:
                    NVIC_SetPriority(EXTI9_5_IRQn, priority);
                    NVIC_EnableIRQ(EXTI9_5_IRQn);
                    status = true;
                    break;
                case 10:
                    NVIC_SetPriority(EXTI15_10_IRQn, priority);
                    NVIC_EnableIRQ(EXTI15_10_IRQn);
                    status = true;
                    break;
                case 11:
                    NVIC_SetPriority(EXTI15_10_IRQn, priority);
                    NVIC_EnableIRQ(EXTI15_10_IRQn);
                    status = true;
                    break;
                case 12:
                    NVIC_SetPriority(EXTI15_10_IRQn, priority);
                    NVIC_EnableIRQ(EXTI15_10_IRQn);
                    status = true;
                    break;
                case 13:
                    NVIC_SetPriority(EXTI15_10_IRQn, priority);
                    NVIC_EnableIRQ(EXTI15_10_IRQn);
                    status = true;
                    break;
                case 14:
                    NVIC_SetPriority(EXTI15_10_IRQn, priority);
                    NVIC_EnableIRQ(EXTI15_10_IRQn);
                    status = true;
                    break;
                case 15:
                    NVIC_SetPriority(EXTI15_10_IRQn, priority);
                    NVIC_EnableIRQ(EXTI15_10_IRQn);
                    status = true;
                    break;
                default:
                    status = false;
                    break;
            }
            LL_EXTI_EnableIT_0_31(EXTI_Line[pin]);
            LL_EXTI_ClearFlag_0_31(EXTI_Line[pin]);
            status = true;
        }
    }
    return status;
}
bool RemovePinInterrupt(GPIO_PIN pinNumber)
{
    bool status = false;
    if (IsValidPin(pinNumber))
    {
        uint32_t pin = pinNumber % 16;
        // Clear the External Interrupt or Event for the current IO
        LL_D3_EXTI_DisablePendMask_0_31(EXTI_Line[pin]);
        LL_EXTI_DisableIT_0_31(EXTI_Line[pin]);
        LL_EXTI_DisableFallingTrig_0_31(EXTI_Line[pin]);
        LL_EXTI_DisableRisingTrig_0_31(EXTI_Line[pin]);

        // Clear the config of the line
        SYSCFG->EXTICR[EXTI_Line[pin] & 0x3U] |= ~(EXTI_Line[pin] >> 16U);
        status = true;
    }
    return status;
}
void EXTI0_IRQHandler(void)
{
    if (SavedInterruptCallBack[0] != NULL)
    {
        SavedInterruptCallBack[0](SavedInterruptPinNumber[0], true, NULL);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
    }
}
void EXTI1_IRQHandler(void)
{
    if (SavedInterruptCallBack[1] != NULL)
    {
        SavedInterruptCallBack[1](SavedInterruptPinNumber[1], true, NULL);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_1);
    }
}
void EXTI2_IRQHandler(void)
{
    if (SavedInterruptCallBack[2] != NULL)
    {
        SavedInterruptCallBack[2](SavedInterruptPinNumber[2], true, NULL);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
    }
}
void EXTI3_IRQHandler(void)
{
    if (SavedInterruptCallBack[3] != NULL)
    {
        SavedInterruptCallBack[3](SavedInterruptPinNumber[3], true, NULL);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
    }
}
void EXTI4_IRQHandler(void)
{
    if (SavedInterruptCallBack[4] != NULL)
    {
        SavedInterruptCallBack[4](SavedInterruptPinNumber[4], true, NULL);
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_4);
    }
}
void EXTI9_5_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_5))
    {
        if (SavedInterruptCallBack[5] != NULL)
        {
            SavedInterruptCallBack[5](SavedInterruptPinNumber[5], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_5);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_6))
    {
        if (SavedInterruptCallBack[6] != NULL)
        {
            SavedInterruptCallBack[6](SavedInterruptPinNumber[6], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_6);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_7))
    {
        if (SavedInterruptCallBack[7] != NULL)
        {
            SavedInterruptCallBack[7](SavedInterruptPinNumber[7], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_7);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_8))
    {
        if (SavedInterruptCallBack[8] != NULL)
        {
            SavedInterruptCallBack[8](SavedInterruptPinNumber[8], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_8);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_9))
    {
        if (SavedInterruptCallBack[9] != NULL)
        {
            SavedInterruptCallBack[9](SavedInterruptPinNumber[9], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_9);
        }
    }
}
void EXTI15_10_IRQHandler(void)
{
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_10))
    {
        if (SavedInterruptCallBack[10] != NULL)
        {
            SavedInterruptCallBack[10](SavedInterruptPinNumber[10], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_10);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_11))
    {
        if (SavedInterruptCallBack[11] != NULL)
        {
            SavedInterruptCallBack[11](SavedInterruptPinNumber[11], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_11);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_12))
    {
        if (SavedInterruptCallBack[12] != NULL)
        {
            SavedInterruptCallBack[12](SavedInterruptPinNumber[12], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_12);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13))
    {
        if (SavedInterruptCallBack[13] != NULL)
        {
            SavedInterruptCallBack[13](SavedInterruptPinNumber[13], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_14))
    {
        if (SavedInterruptCallBack[14] != NULL)
        {
            SavedInterruptCallBack[14](SavedInterruptPinNumber[14], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_14);
        }
    }
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_15))
    {
        if (SavedInterruptCallBack[15] != NULL)
        {
            SavedInterruptCallBack[15](SavedInterruptPinNumber[15], true, NULL);
            LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_15);
        }
    }
}
#pragma endregion

#pragma region ADC

// STM32H7 Series

// ADC input multiplexer
// The ADC has an input multiplexer that selects one of 20 channels to sample.
// There are 6 fast channels characterized with low input resistance.
// The other 14 channels have higher input resistances that require longer sampling times.
// These are hence referred to as slow channels.
//
// IO analog switch
// Connection between an IO pin and ADC is made through an analog switch, which increases
// the total ADC channel input resistance.Direct channels are essentially fast channels on
// the ADC input multiplexer side,but specifically bypass the analog switch on IO side

void InitADC()
{
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);
#ifdef ADC3
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_ADC3);
#endif
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC2), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#ifdef ADC3
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC3), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#endif
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);
    LL_ADC_REG_SetTriggerSource(ADC2, LL_ADC_REG_TRIG_SOFTWARE);
#ifdef ADC3
    LL_ADC_REG_SetTriggerSource(ADC3, LL_ADC_REG_TRIG_SOFTWARE);
#endif
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
    LL_ADC_REG_SetContinuousMode(ADC2, LL_ADC_REG_CONV_SINGLE);
#ifdef ADC3
    LL_ADC_REG_SetContinuousMode(ADC3, LL_ADC_REG_CONV_SINGLE);
#endif
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);
    LL_ADC_REG_SetSequencerLength(ADC2, LL_ADC_REG_SEQ_SCAN_DISABLE);
#ifdef ADC3
    LL_ADC_REG_SetSequencerLength(ADC3, LL_ADC_REG_SEQ_SCAN_DISABLE);
#endif

    NVIC_SetPriority(ADC_IRQn, 0);
    NVIC_EnableIRQ(ADC_IRQn);

    LL_ADC_EnableIT_OVR(ADC1);
    LL_ADC_EnableIT_OVR(ADC2);
#ifdef ADC3
    LL_ADC_EnableIT_OVR(ADC3);
#endif
}
void OpenAdcChannel(int channel)
{

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, AdcChannelNumber[channel]);
    LL_ADC_REG_SetSequencerRanks(ADC2, LL_ADC_REG_RANK_1, AdcChannelNumber[channel]);
#ifdef ADC3
    LL_ADC_REG_SetSequencerRanks(ADC3, LL_ADC_REG_RANK_1, AdcChannelNumber[channel]);
#endif
    LL_ADC_SetChannelSamplingTime(ADC1, AdcChannelNumber[channel], LL_ADC_SAMPLINGTIME_64CYCLES_5);
    LL_ADC_SetChannelSamplingTime(ADC2, AdcChannelNumber[channel], LL_ADC_SAMPLINGTIME_64CYCLES_5);
#ifdef ADC3
    LL_ADC_SetChannelSamplingTime(ADC3, AdcChannelNumber[channel], LL_ADC_SAMPLINGTIME_64CYCLES_5);
#endif

    LL_ADC_Enable(ADC_Channel[channel]);
}
int GetAdcChannelCount()
{
    return 1;
}
int ConvertChannelIndexToAdcInstance(int channelIndex)
{
    (void)channelIndex;
    return 1;
}
uint16_t AdcReadValue(int channelIndex)
{
    int AdcChannel = ConvertChannelIndexToAdcInstance(channelIndex);
    // Perform ADC group regular conversion start, poll for conversion completion.
    LL_ADC_REG_StartConversion(ADC_Channel[AdcChannel]);
    while (LL_ADC_IsActiveFlag_EOC(ADC_Channel[AdcChannel]) == 0)
    {
    }
    LL_ADC_ClearFlag_EOC(ADC_Channel[AdcChannel]);

    // Retrieve ADC conversion data  (data scale corresponds to ADC resolution: 16 bits)
    return LL_ADC_REG_ReadConversionData16(ADC_Channel[AdcChannel]);
}
int GetAdcSupportedMaxValue()
{
    // LL_ADC_RESOLUTION_16B
    // LL_ADC_RESOLUTION_14B
    // LL_ADC_RESOLUTION_12B
    // LL_ADC_RESOLUTION_10B
    // LL_ADC_RESOLUTION_8B

    return __LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_16B);
}
int GetAdcSupportedMinValue()
{
    return 0;
}
int GetAdcSupportedResolution()
{
    int AdcChannel = 0;
    return LL_ADC_GetResolution(ADC_Channel[AdcChannel]);
}
bool IsAdcModeSupported(AdcChannelMode mode)
{
    bool status = false;
    switch (mode)
    {
        case AdcChannelMode::AdcChannelMode_SingleEnded:
            status = true;
            break;
        case AdcChannelMode::AdcChannelMode_Differential:
            status = true;
            break;
    }
    return status;
}

#pragma endregion

#pragma region DAC
void InitDacChannel(int dacIndex)
{
    (void)dacIndex;
    return;
}
void OpenDacChannel(int dacIndex, int dacChannel)
{
    uint32_t llDACChannel;
    if (dacChannel == 1)
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
    LL_DAC_Init(DAC_Channel[dacIndex], llDACChannel, &DAC_InitStruct);
}
void WriteDacValue(int dacIndex, int channel, uint16_t value)
{
    LL_DAC_ConvertData12RightAligned(DAC_Channel[dacIndex], channel, value);
}
void StopDac(int dacIndex, int channel)
{
    if (IsValidI2CBus(dacIndex))
    {
        LL_DAC_Disable(DAC_Channel[dacIndex], channel);
    }
}
void DAC_Start_SoftwareTriggered(int channel)
{
    DAC_TypeDef *DACx = DAC1;
    uint32_t DAC_Channel = channel;
    if (DAC_Channel == 1)
    {
        LL_DAC_Enable(DACx, DAC_Channel);
        SET_BIT(DACx->SWTRIGR, DAC_SWTRIGR_SWTRIG1);
    }
    else
    {
        LL_DAC_Enable(DACx, DAC_Channel);
        SET_BIT(DACx->SWTRIGR, DAC_SWTRIGR_SWTRIG2);
    }
}
int GetDacChannelCount(int dacIndex)
{
    int channelCount = 0;
    switch (dacIndex)
    {
        case 1:
            channelCount = 20;
            break;
        case 2:
            channelCount = 20;
            break;
        case 3:
            channelCount = 20;
            break;
        default:
            channelCount = 0;
    }
    return channelCount;
}
int GetDacSupportedResolution(int dacIndex)
{
    int resolution = 0;
    switch (dacIndex)
    {
        case 1:
            resolution = 16;
            break;
        case 2:
            resolution = 16;
            break;
        case 3:
            resolution = 12;
            break;
        default:
            resolution = 0;
    }
    return resolution;
}

#pragma endregion

#pragma region I2C
bool Initialize_Native_I2C(int busIndex, I2cBusSpeed busSpeed, I2c_Bus_Type busType, uint8_t SlaveAddress)
{

    LL_RCC_ClocksTypeDef RCC_Clocks;
    LL_RCC_GetSystemClocksFreq(&RCC_Clocks);

    bool status = false;
    switch (busIndex)
    {
        case 1:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            status = true;
            break;
        case 2:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            status = true;
            break;
        case 3:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C3);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C123_CLKSOURCE_PCLK1);
            status = true;
            break;
        case 4:
            LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_I2C4);
            LL_RCC_SetI2CClockSource(LL_RCC_I2C4_CLKSOURCE_PCLK4);
            status = true;
            break;
#ifdef I2C5
        case 5:
            LL_APB4_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C5);
            // LL_RCC_SetI2CClockSource(LL_RCC_I2C5_CLKSOURCE_PCLK1);
            status = true;
            break;
#else
        case 5:
            status = false;
            break;
#endif
        default:
            status = false;
    }

    if (status)
    {
        LL_I2C_Disable(I2C_Channel[busIndex]);
        {
            LL_I2C_ConfigFilters(I2C_Channel[busIndex], LL_I2C_ANALOGFILTER_ENABLE, 0);
            LL_I2C_SetTiming(I2C_Channel[busIndex], I2C_GetTiming(busSpeed));
            LL_I2C_SetMode(I2C_Channel[busIndex], LL_I2C_MODE_I2C);
            LL_I2C_DisableClockStretching(I2C_Channel[busIndex]);
            LL_I2C_DisableGeneralCall(I2C_Channel[busIndex]);

            switch (busType)
            {
                case Master:
                    LL_I2C_SetOwnAddress1(I2C_Channel[busIndex], 0, LL_I2C_OWNADDRESS1_7BIT);
                    LL_I2C_SetMasterAddressingMode(I2C_Channel[busIndex], LL_I2C_ADDRESSING_MODE_7BIT);
                    LL_I2C_AcknowledgeNextData(I2C_Channel[busIndex], LL_I2C_ACK);
                    LL_I2C_DisableOwnAddress1(I2C_Channel[busIndex]);
                    LL_I2C_DisableOwnAddress2(I2C_Channel[busIndex]);
                    status = true;
                    break;
                case Slave:
                    // Reset Value is Clock stretching enabled
                    LL_I2C_SetOwnAddress1(I2C_Channel[busIndex], SlaveAddress, LL_I2C_OWNADDRESS1_7BIT);
                    LL_I2C_SetOwnAddress2(I2C_Channel[busIndex], 0x00, LL_I2C_OWNADDRESS2_NOMASK);
                    LL_I2C_EnableOwnAddress1(I2C_Channel[busIndex]);
                    LL_I2C_DisableOwnAddress2(I2C_Channel[busIndex]);
                    LL_I2C_AcknowledgeNextData(I2C_Channel[busIndex], LL_I2C_NACK);
                    status = true;
                    break;
                default:
                    status = false;
                    break;
            }

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
        LL_I2C_Enable(I2C_Channel[busIndex]);

        if (status)
        {
            NVIC_SetPriority(I2C_Interrupt_Channel[busIndex], 0);
            NVIC_EnableIRQ(I2C_Interrupt_Channel[busIndex]);
        }
    }
    return status;
}
bool DeInitialize_Native_I2C(int busIndex)
{
    bool status = false;
    switch (busIndex)
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
        int pinNumberSCL = FindPinWithFunctionAndChannel(DevicePinFunction::I2C_SCL, busIndex);
        int pinNumberSDA = FindPinWithFunctionAndChannel(DevicePinFunction::I2C_SDA, busIndex);
        SetPinLowPower(pinNumberSDA);
        SetPinLowPower(pinNumberSCL);
    }
    return status;
}
bool I2CWriteData(int busIndex, uint8_t *writeData, int writeSize, uint8_t slaveAddress)
{
    bool status = false;
    if (IsValidI2CBus(busIndex))
    {
        LL_I2C_HandleTransfer(
            I2C_Channel[busIndex],
            slaveAddress,
            LL_I2C_ADDRSLAVE_7BIT,
            writeSize,
            LL_I2C_MODE_SOFTEND,
            LL_I2C_GENERATE_START_WRITE);
        LL_I2C_TransmitData8(I2C_Channel[busIndex], 0);
        while (!LL_I2C_IsActiveFlag_TXE(I2C_Channel[busIndex]))
            ;
        // sending the data
        for (uint8_t i = 0; i < writeSize; i++)
        {
            LL_I2C_TransmitData8(I2C_Channel[busIndex], writeData[i]);
            while (!LL_I2C_IsActiveFlag_TXE(I2C_Channel[busIndex]))
                writeData++;
        }
        status = true;
    }
    return status;
}
bool I2CReadData(int busIndex, uint8_t *readData, int readSize, uint8_t slaveAddress)
{
    bool status = false;
    if (IsValidI2CBus(busIndex))
    {
        LL_I2C_AcknowledgeNextData(I2C_Channel[busIndex], LL_I2C_ACK);
        LL_I2C_HandleTransfer(
            I2C_Channel[busIndex],
            (slaveAddress | 1),
            LL_I2C_ADDRSLAVE_7BIT,
            readSize,
            LL_I2C_MODE_AUTOEND,
            LL_I2C_GENERATE_START_READ);
        for (uint8_t i = 0; i < readSize; i++)
        {
            if (i < readSize - 1)
                LL_I2C_AcknowledgeNextData(I2C_Channel[busIndex], LL_I2C_ACK);
            else
                LL_I2C_AcknowledgeNextData(I2C_Channel[busIndex], LL_I2C_NACK);
            while (LL_I2C_IsActiveFlag_RXNE(I2C_Channel[busIndex]) == 0)
                ;
            readData[i] = LL_I2C_ReceiveData8(I2C_Channel[busIndex]);
        }
        status = true;
    }
    // Optional?
    // LL_I2C_GenerateStopCondition(I2C_Channel[busIndex]);
    return status;
}
#pragma endregion

#pragma region Pwm
int SetDesiredFrequency(int pwmID, int desiredFrequency)
{
    (void)pwmID;
    (void)desiredFrequency;
    return 1;
}
double SetDutyCycle(int pwmID, int channelId, double dutyCycle)
{
    (void)pwmID;
    (void)channelId;
    (void)dutyCycle;
    return 1.0;
}
void StartPwm(int pwmID, double dutyCycle, int channelId, int pinNumber)
{
    (void)pwmID;
    (void)dutyCycle;
    (void)channelId;
    (void)pinNumber;
    return;
}
void StopPwm(int pwmID, int channelId)
{
    (void)pwmID;
    (void)channelId;
    return;
}
bool CheckChannel(int pwmID, int channelId)
{
    (void)pwmID;
    (void)channelId;
    return true;
}

#pragma endregion

#pragma region Serial
__attribute__((section(".dma_buffer"))) __attribute__((aligned(32))) uint8_t DmaReceiveBuffer[1024];
static_assert((sizeof(DmaReceiveBuffer) % 32) == 0, "Must be a multiple by 32");

void InitUSART(int UsartDeviceNumber)
{
    USART_TypeDef *USARTx = Serial_Channel[UsartDeviceNumber].controllerId;
    LL_USART_InitTypeDef USART_InitStruct;

    switch (UsartDeviceNumber)
    {
        case 1:
        case 6:
        case 10:
            LL_APB2_GRP1_EnableClock(UsartPortClockEnable[UsartDeviceNumber]);
            break;
        case 2:
        case 3:
            LL_APB1_GRP1_EnableClock(UsartPortClockEnable[UsartDeviceNumber]);
        default:
            return;
            break;
    }

    LL_AHB1_GRP1_EnableClock(DmaClockEnable[UsartDeviceNumber]);
    DMA_TypeDef *DMAx = DMA_Channel[UsartDeviceNumber];

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
    LL_DMA_SetPeriphAddress(DMAx, wpDMA_ReceiveStream, LL_USART_DMA_GetRegAddr(USARTx, LL_USART_DMA_REG_DATA_RECEIVE));
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
    USART_InitStruct.BaudRate = wpBAUD_RATE;
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
void DeInitUSART(int UsartDeviceNumber)
{
    DMA_TypeDef *DMAx = DMA_Channel[UsartDeviceNumber];
    LL_DMA_DeInit(DMAx, DmaStreamNumber[UsartDeviceNumber]);
}
int SerialBytesAvailable(int UsartDeviceNumber)
{
    int index = GetSerialDeviceBufferIndex(UsartDeviceNumber);
    return wp_BufferBytesWaiting(&DeviceReceiveCircularBuffer[index]);
}
int GetSerialDeviceBufferIndex(int UsartDeviceNumber)
{
    return CircularBufferAssignments[UsartDeviceNumber];
}
void SetSerialDeviceSignalsInverted(int UsartDeviceNumber, bool InvertSignals)
{
    USART_TypeDef *USARTx = Serial_Channel[UsartDeviceNumber].controllerId;
    if (InvertSignals)
    {
        LL_USART_SetRXPinLevel(USARTx, LL_USART_RXPIN_LEVEL_INVERTED);
    }
    else
    {
        LL_USART_SetRXPinLevel(USARTx, LL_USART_RXPIN_LEVEL_STANDARD);
    }
    return;
}
bool IsSerialSignalInverted(int UsartDeviceNumber)
{
    USART_TypeDef *USARTx = Serial_Channel[UsartDeviceNumber].controllerId;
    return (LL_USART_GetRXPinLevel(USARTx) == LL_USART_RXPIN_LEVEL_INVERTED);
}
int SerialReadBytes(int UsartDeviceNumber, uint8_t *data, int bytesRequested)
{
    int bytesAvailableToRead = SerialBytesAvailable(UsartDeviceNumber);
    if (bytesRequested > bytesAvailableToRead)
    {
        bytesRequested = bytesAvailableToRead;
    }
    int index = GetSerialDeviceBufferIndex(UsartDeviceNumber);
    ULONG read = wp_ReadBuffer(&DeviceReceiveCircularBuffer[index], data, bytesRequested);
    return read;
}
int SerialReadLine(int UsartDeviceNumber, const char *newLineCharacters, int newLineLength, uint8_t *&line)
{
    int index = GetSerialDeviceBufferIndex(UsartDeviceNumber);
    ULONG read = wp_ReadLine(&DeviceReceiveCircularBuffer[index], newLineCharacters, newLineLength, line);
    return read;
}
bool SerialWriteBytes(int UsartDeviceNumber, uint8_t *data, int size)
{
    int index = GetSerialDeviceBufferIndex(UsartDeviceNumber);
    wp_WriteBuffer(&DeviceTransmitCircularBuffer[index], data, size);
    return true;
}
void SerialSetStopBits(int UsartDeviceNumber, int RequestedStopBits)
{
    (void)UsartDeviceNumber;
    (void)RequestedStopBits;
    return;
}
int SerialSetThreshold(int UsartDeviceNumber, int RequestedThreshold)
{
    (void)UsartDeviceNumber;
    (void)RequestedThreshold;
    return 1;
}
void SerialSetBaudRate(int UsartDeviceNumber, int RequestedBaudRate)
{
    (void)UsartDeviceNumber;
    (void)RequestedBaudRate;
    return;
}
void SerialSetWatchCharacter(int UsartDeviceNumber, uint8_t watchCharacter)
{
    (void)UsartDeviceNumber;
    (void)watchCharacter;
    return;
}

#pragma endregion

#pragma region Spi
#pragma endregion

