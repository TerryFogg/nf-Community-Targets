//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_gpio_native_target.h"
#include <targetPAL.h>
#include <tx_api.h>
#include <tx_port.h>
#include <hardware/gpio.h>

#define GPIO_MAX_PIN 28

#define PAL_LOW 0U
#define PAL_HIGH 1U

static GPIO_PIN InterruptPin;
HAL_COMPLETION m_gpioCompletion; // TODO: ????????????

// Double linkedlist to hold the state of each Input pin
struct gpio_input_state : public HAL_DblLinkedNode<gpio_input_state>
{
    GPIO_PIN pinNumber;
    TX_TIMER debounceTimer;
    GPIO_INTERRUPT_SERVICE_ROUTINE isrPtr;
    uint32_t debounceMilliseconds;
    uint8_t interruptMode;
    void *param;
    bool expectedState;
    bool waitingForDebounce;
};

static HAL_DblLinkedList<gpio_input_state> gpioInputList; // Double Linked list for GPIO input status
static uint16_t pinReserved[GPIO_MAX_PIN];            //  reserved - 1 bit per pin

TX_INTERRUPT_SAVE_AREA

bool IsValidGpioPin(GPIO_PIN pinNumber)
{
    return (pinNumber <= GPIO_MAX_PIN);
}

gpio_input_state *GetGpioWithInterrupt(uint16_t gpioPin)
{
    gpio_input_state *ptr = gpioInputList.FirstNode();
    while (ptr->Next() != NULL)
    {
        if (GPIO_PIN(ptr->pinNumber) == gpioPin)
        {
            return ptr;
        }
        ptr = ptr->Next();
    }
    return NULL;
}

static void DebounceTimerCallback(uint32_t id)
{
}

void GpioEventCallback(void *arg)
{
    NATIVE_INTERRUPT_START
    TX_DISABLE

    gpio_input_state *pGpio = (gpio_input_state *)arg;

    if (pGpio != NULL)
    {
        // Ignore any pin changes during debounce
        if (!pGpio->waitingForDebounce)
        {
            // check if there is a debounce time set
            if (pGpio->debounceMilliseconds > 0)
            {
                // Set flag we are waiting for debounce on this pin
                pGpio->waitingForDebounce = true;

                // setup timer
                tx_timer_deactivate(&pGpio->debounceTimer);
                tx_timer_change(&pGpio->debounceTimer, 0, pGpio->debounceMilliseconds / 10);
                tx_timer_activate(&pGpio->debounceTimer);
            }
            else
            {
                bool pinSet = gpio_get(pGpio->pinNumber);
                // TODO: ??????????????? TX_RESTORE for isrPtr?
                TX_RESTORE pGpio->isrPtr(pGpio->pinNumber, pinSet, pGpio->param);
                TX_DISABLE
            }
        }
    }
    TX_RESTORE
    NATIVE_INTERRUPT_END
}

// Get pointer to gpio_input_state for Gpio pin
gpio_input_state *GetInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *ptr = gpioInputList.FirstNode();

    while (ptr->Next() != NULL)
    {
        if (ptr->pinNumber == pinNumber)
        {
            return ptr;
        }

        ptr = ptr->Next();
    }
    return NULL;
}

// Allocate a new gpio_input_state and add to end of list
// if already exist then just return current ptr
gpio_input_state *AllocateGpioInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *ptr = GetInputState(pinNumber);

    if (ptr == NULL)
    {
        ptr = (gpio_input_state *)platform_malloc(sizeof(gpio_input_state));

        // sanity check
        if (ptr != NULL)
        {
            memset(ptr, 0, sizeof(gpio_input_state));
            ptr->pinNumber = pinNumber;

            tx_timer_create(
                &ptr->debounceTimer,
                (char *)"GPIO debounce timer",
                DebounceTimerCallback,
                0,
                0,
                1,
                TX_NO_ACTIVATE);

            gpioInputList.LinkAtBack(ptr);
        }
    }
    return ptr;
}

void UnlinkInputState(gpio_input_state *pState)
{
    tx_timer_delete(&pState->debounceTimer);

    // disable the EXT interrupt channel
    // it's OK to do always this, no matter if it's enabled or not
    //    GPIO_TypeDef *GPIO_port = GPIO_PORT(pState->pinNumber);
    //    uint32_t PinMask = GPIO_PIN(pState->pinNumber);

    ///////////////////////palDisablePadEventI(GPIO_port, PinMask);

    pState->Unlink();

    platform_free(pState);
}

// Delete gpio_input_state from List and tidy up ( Timer & ISR handler )
void DeleteInputState(GPIO_PIN pinNumber)
{
    gpio_input_state *pState = GetInputState(pinNumber);

    if (pState)
    {
        UnlinkInputState(pState);
    }
}

bool CPU_GPIO_Initialize()
{
    // Initialise Double linked list for input pin states
    gpioInputList.Initialize();

    // Make sure all pins are not reserved
    memset(pinReserved, 0, sizeof(pinReserved));

    return true;
}

bool CPU_GPIO_Uninitialize()
{
    NANOCLR_FOREACH_NODE(gpio_input_state, pGpio, gpioInputList)
    {
        UnlinkInputState(pGpio);
    }
    NANOCLR_FOREACH_NODE_END();

    return true;
}

// Set/reset reserved state of pin
bool CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
    // Check if valid pin number
    if (!IsValidGpioPin(pinNumber))
    {
        return false;
    }

    int port = pinNumber >> 4, bit = 1 << (pinNumber & 0x0F);
    bool ret = true;
    GLOBAL_LOCK();

    if (fReserve)
    {
        if (pinReserved[port] & bit)
        {
            ret = false; // already reserved
        }
        else
        {
            pinReserved[port] |= bit;
        }
    }
    else
    {
        pinReserved[port] &= ~bit;
    }

    GLOBAL_UNLOCK();
    return ret;
}

// Return if Pin is reserved
bool CPU_GPIO_PinIsBusy(GPIO_PIN pinNumber)
{
    // Check if valid pin number
    if (!IsValidGpioPin(pinNumber))
    {
        return false;
    }

    int port = pinNumber >> 4, sh = pinNumber & 0x0F;
    return (pinReserved[port] >> sh) & 1;
}

// Return maximum number of pins
int32_t CPU_GPIO_GetPinCount()
{
    return GPIO_MAX_PIN;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pin)
{
    return (GpioPinValue)gpio_get(pin);
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pin, GpioPinValue pinState)
{
    bool gpio_state = (pinState == GpioPinValue::GpioPinValue_High);
    gpio_put(pin, gpio_state);
}
#define PAL_EVENT_MODE_BOTH_EDGES 3U /**< @brief Both edges callback.    */

void CPU_GPIO_TogglePinState(GPIO_PIN pin)
{
    gpio_put(pin, !gpio_get(pin));
}

bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    uint32_t debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isrParam,
    GPIO_INT_EDGE intEdge,
    PinMode driveMode)
{

    gpio_input_state *pState;

    // Check Input drive mode
    if (driveMode >= (int)PinMode_Output)
    {
        return false;
    }

    gpio_init(pinNumber);

    // Set as Input GPIO_INT_EDGE intEdge, GPIO_RESISTOR ResistorState
    if (!CPU_GPIO_SetDriveMode(pinNumber, driveMode))
    {
        return false;
    }

    pState = AllocateGpioInputState(pinNumber);

    // Link ISR ptr supplied and not already set up
    // CPU_GPIO_EnableInputPin could be called a 2nd time with changed parameters
    if (pinISR != NULL && (pState->isrPtr == NULL))
    {
        // there are callbacks registered and...
        // the drive mode is input so need to setup the interrupt

        //        GPIO_TypeDef *GPIO_port = GPIO_PORT(pState->pinNumber);
        //        uint32_t PinMask = GPIO_PIN(pState->pinNumber);
        //
        //        ////////////////////pal_lld_enablelineevent(ioLine,
        //        PAL_EVENT_MODE_BOTH_EDGES);
        //
        //        NVIC_EnableIRQ((IRQn_Type)(InterruptPin));
        //        NVIC_DisableIRQ((IRQn_Type)(InterruptPin));

        /////////////////////////////palSetLineCallbackI(ioLine, GpioEventCallback, pState);

        // store parameters & configs
        pState->isrPtr = pinISR;
        pState->interruptMode = intEdge;
        pState->param = (void *)isrParam;
        pState->debounceMilliseconds = (uint32_t)(debounceTimeMilliseconds);

        switch (intEdge)
        {
        case GPIO_INT_EDGE_LOW:
        case GPIO_INT_LEVEL_LOW:
            pState->expectedState = PAL_LOW;
            break;

        case GPIO_INT_EDGE_HIGH:
        case GPIO_INT_LEVEL_HIGH:
            pState->expectedState = PAL_HIGH;
            break;

        case GPIO_INT_EDGE_BOTH:
            pState->expectedState = !CPU_GPIO_GetPinState(pinNumber); // expected NOT current state
            break;

        default:
            break;
        }
    }
    else if (pinISR == NULL && (pState->isrPtr != NULL))
    {
        // there is no managed handler setup anymore

        // disable the EXT interrupt channel
        // it's OK to do always this, no matter if it's enabled or not

        //        GPIO_TypeDef *GPIO_port = GPIO_PORT(pState->pinNumber);
        //        uint32_t PinMask = GPIO_PIN(pState->pinNumber);

        // clear parameters & configs
        pState->isrPtr = NULL;
        pState->interruptMode = GPIO_INT_NONE;
        pState->param = NULL;
        pState->debounceMilliseconds = 0;
    }

    return true;
}

// Enable an output pin
//
// pinNumber    -   Gpio pin number
// InitialState -   Initial state of pin
// driveMode    -   Drive mode and resistors
// return       -   True if succesful, false invalid pin, pin not putput,
// invalid drive mode for ouptput
//
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, PinMode driveMode)
{

    gpio_init(pinNumber);
    
    // check not an output drive mode
    if (driveMode < (int)PinMode_Output)
    {
        return false;
    }

    // If this is currently an input pin then clean up
    DeleteInputState(pinNumber);

    if (CPU_GPIO_SetDriveMode(pinNumber, driveMode) == false)
    {
        return false;
    }

    CPU_GPIO_SetPinState(pinNumber, InitialState);

    return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, PinMode driveMode, uint32_t alternateFunction)
{
    DeleteInputState(pinNumber);

    GLOBAL_LOCK();

    CPU_GPIO_SetDriveMode(pinNumber, driveMode);

    //    GPIO_TypeDef *GPIO_port = GPIO_PORT(pinNumber);
    //    uint32_t PinMask = GPIO_PIN(pinNumber);


    GLOBAL_UNLOCK();

    CPU_GPIO_ReservePin(pinNumber, false);
}
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, PinMode driveMode)
{
    switch (driveMode)
    {
    case PinMode_Input:
        gpio_set_dir(pinNumber, false);
        break;

    case PinMode_InputPullDown:
        gpio_set_dir(pinNumber, false);
        gpio_pull_down(pinNumber);
        break;

    case PinMode_InputPullUp:
        gpio_set_dir(pinNumber, false);
        gpio_pull_up(pinNumber);
        break;

    case PinMode_Output:
        gpio_set_dir(pinNumber, true);
        break;

    case PinMode_OutputOpenDrain:
        // TODO : What is the equivalent open drain for RPI Pico?
        gpio_set_dir(pinNumber, true);
        gpio_disable_pulls(pinNumber);
        break;

    default:
        // all other modes are NOT supported
        return false;
    }

    return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, PinMode driveMode)
{
    (void)pinNumber;
    return true;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
    gpio_input_state *ptr = GetInputState(pinNumber);

    if (ptr)
    {
        return ptr->debounceMilliseconds;
    }

    return 0;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, uint32_t debounceTimeMilliseconds)
{
    gpio_input_state *ptr = GetInputState(pinNumber);
    if (ptr)
    {
        ptr->debounceMilliseconds = debounceTimeMilliseconds;
        return true;
    }
    return false;
}
