//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "tx_api.h"
#include <nanoCLR_Runtime.h>
#include "Diagnostics.h"

uint64_t CPU_MillisecondsToTicks(uint64_t ticks);

// timer for bool events
TX_TIMER boolEventsTimer;
extern TX_BYTE_POOL byte_pool_0;

static bool *saveTimerCompleteFlag = 0;
static TX_TIMER nextEventTimer;
void *nextEventCallbackDummyArg = NULL;


void RtosYield()
{
    // OK to call relinquish here despite it will only execute threads with same or higher priority
    tx_thread_relinquish();
}
void *platform_malloc(size_t size)
{
    void *pointer = TX_NULL;

    // Allocate the stack for thread 0
    tx_byte_allocate(&byte_pool_0, (VOID **)&pointer, size, TX_NO_WAIT);

    return pointer;
}
void platform_free(void *ptr)
{
    tx_byte_release(ptr);
}
void *platform_realloc(void *ptr, size_t size)
{
    (void)size;

    return ptr;
}


uint64_t CPU_MillisecondsToTicks(uint64_t ticks)
{
    return ((ticks * (uint64_t)TX_TIMER_TICKS_PER_SECOND) / 1000);
}

// Converts sysTicks to .NET ticks (100 nanoseconds)
uint64_t HAL_Time_SysTicksToTime(uint64_t sysTicks)
{
    return (((int64_t)sysTicks * (int64_t)1000000 + (int64_t)TX_TIMER_TICKS_PER_SECOND - 1) /
            (int64_t)TX_TIMER_TICKS_PER_SECOND) * 10;
}

static void local_Events_SetBoolTimer_Callback(ULONG input)
{
  (void)input;
  *saveTimerCompleteFlag = true;
}
bool Events_Initialize_Platform()
{
    return tx_timer_create(
               &boolEventsTimer,
               (char *)"BoolEvents Timer",
               local_Events_SetBoolTimer_Callback,
               0,
               1,
               0,
               TX_NO_ACTIVATE) == TX_SUCCESS;
}
bool Events_Uninitialize_Platform()
{
    tx_timer_delete(&boolEventsTimer);
    return true;
}
void Events_SetBoolTimer(bool *timerCompleteFlag, uint32_t millisecondsFromNow)
{
    if (timerCompleteFlag != NULL)
    {
        // As only one timer running at a time we will just save it
        saveTimerCompleteFlag = timerCompleteFlag;

        // need to stop the timer first
        tx_timer_deactivate(&boolEventsTimer);
        tx_timer_change(&boolEventsTimer, 0, TX_TICKS_PER_MILLISEC(millisecondsFromNow));
        tx_timer_activate(&boolEventsTimer);
    }
}
uint32_t Events_WaitForEvents(uint32_t powerLevel, uint32_t wakeupSystemEvents, uint32_t timeoutMilliseconds)
{
    // schedule an interrupt for this far in the future
    // timeout is in milliseconds, need to convert to ticks
    uint64_t countsRemaining = CPU_MillisecondsToTicks(timeoutMilliseconds);

#if defined(HAL_PROFILE_ENABLED)
    Events_WaitForEvents_Calls++;
#endif

    uint64_t expireTimeInTicks = HAL_Time_CurrentSysTicks() + countsRemaining;
    bool runContinuations = true;

    while (true)
    {
        uint32_t events = Events_MaskedRead(wakeupSystemEvents);
        if (events)
        {
            return events;
        }

        if (expireTimeInTicks <= HAL_Time_CurrentSysTicks())
        {
            break;
        }

        // first check and possibly run any continuations
        // but only if we have slept after stalling
        if (runContinuations && !SystemState_QueryNoLock(SYSTEM_STATE_NO_CONTINUATIONS))
        {
            // if we stall on time, don't check again until after we sleep
            runContinuations = HAL_CONTINUATION::Dequeue_And_Execute();
        }
        else
        {
            // try stalled continuations again after sleeping
            runContinuations = true;

            HAL_COMPLETION::WaitForInterrupts(expireTimeInTicks, powerLevel, wakeupSystemEvents);
        }

        // no events, pass control to the OS
        tx_thread_relinquish();

        // check if reboot or exit flags were set when the other OS threads executed
        if (CLR_EE_DBG_IS(RebootPending) || CLR_EE_DBG_IS(ExitPending))
        {
            break;
        }

        // feed the watchdog...
        Watchdog_Reset();
    }

    return 0;
}
void FreeManagedEvent(uint8_t category, uint8_t subCategory, uint16_t data1, uint32_t data2)
{
    (void)category;
    (void)subCategory;
    (void)data1;
    (void)data2;

    NATIVE_PROFILE_PAL_EVENTS();
}


static void NextEventTimer_Callback(uint32_t id)
{
    (void)id;

    // this call also schedules the next one, if there is one
    HAL_COMPLETION::DequeueAndExec();
}
HRESULT Time_Initialize()
{
    // create timer without activating it
    if (tx_timer_create(
            &nextEventTimer,
            (char *)"PAL Events Timer",
            NextEventTimer_Callback,
            0,
            1,
            0,
            TX_NO_ACTIVATE) == TX_SUCCESS)
    {
        return S_OK;
    }

    return S_FALSE;
}
HRESULT Time_Uninitialize()
{
    tx_timer_delete(&nextEventTimer);

    return S_OK;
}
void Time_SetCompare(uint64_t compareValueTicks)
{
    if (compareValueTicks == 0)
    {
        // compare value is 0 so dequeue and schedule immediately
        // need to stop the timer first
        tx_timer_deactivate(&nextEventTimer);
        // can't call tx_timer_change with 'immediate delay value', so use value 1
        // to get it executed ASAP
        tx_timer_change(&nextEventTimer, 1, 0);
        tx_timer_activate(&nextEventTimer);
    }
    else if (compareValueTicks == HAL_COMPLETION_IDLE_VALUE)
    {
        // wait for infinity, don't need to do anything here
    }
    else
    {
        if (HAL_Time_CurrentSysTicks() >= compareValueTicks)
        {
            // already missed the event, dequeue and execute immediately
            // need to stop the timer first
            tx_timer_deactivate(&nextEventTimer);
            // can't call tx_timer_change with 'immediate delay value', so use value 1
            // to get it executed ASAP
            tx_timer_change(&nextEventTimer, 1, 0);
            tx_timer_activate(&nextEventTimer);
        }
        else
        {
            // compareValueTicks is the time (in sys ticks) that is being requested to
            // fire an HAL_COMPLETION::DequeueAndExec() need to subtract the current
            // system time to set when the timer will fire compareValueTicks is in
            // CMSIS ticks (which equals to ms)
            compareValueTicks -= HAL_Time_CurrentSysTicks();

            // make sure that chVTSet does not get called with zero delay
            if (compareValueTicks == 0)
            {
                // compare value is 0 so dequeue and execute immediately
                // no need to call the timer
                HAL_COMPLETION::DequeueAndExec();
                return;
            }

            // need to stop the timer first
            tx_timer_deactivate(&nextEventTimer);
            tx_timer_change(&nextEventTimer, compareValueTicks, 0);
            tx_timer_activate(&nextEventTimer);
        }
    }
}

// implementation required for STM Cube package
// Provides a tick value in millisecond.
extern "C"
{

    //uint32_t HAL_GetTick(void)
    //{
    //    return tx_time_get();
    //}
}
