//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Power.h>
#include <nanoHAL_v2.h>
#include "board.h"
#include "hardware/gpio.h"
#include "hardware/powman.h"
#include "Power.h"

static powman_power_state off_state;
static powman_power_state on_state;

inline void CPU_Reset()
{
    volatile uint32_t *aircr = (volatile uint32_t *)0xE000ED0C;
     *aircr = (0x05FA << 16) | 0x1 << 2;
    
    // wait until reset
    for (;;)
    {
        __asm__ volatile("nop");
    }
}
inline bool CPU_IsSoftRebootSupported()
{
    return true;
}
// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    CPU_SetPowerMode(PowerLevel__Sleep);
}
void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    switch (powerLevel)
    {
        case PowerLevel__Active:
            break;
        case PowerLevel__SelectiveOff:
            break;
        case PowerLevel__Sleep:
            break;
        case PowerLevel__DeepSleep:
            break;
        case PowerLevel__Off:
            powman_off();
            break;
        default:
            break;
    }
}
void powman_init(uint64_t abs_time_ms)
{
    // start powman and set the time
    powman_timer_start();
    powman_timer_set_ms(abs_time_ms);

    // Allow power down when debugger connected
    powman_set_debug_power_request_ignored(true);

    // Power states
    powman_power_state P1_7 = POWMAN_POWER_STATE_NONE;
    powman_power_state P0_3 = POWMAN_POWER_STATE_NONE;
    P0_3 = powman_power_state_with_domain_on(P0_3, POWMAN_POWER_DOMAIN_SWITCHED_CORE);
    P0_3 = powman_power_state_with_domain_on(P0_3, POWMAN_POWER_DOMAIN_XIP_CACHE);
    off_state = P1_7;
    on_state = P0_3;
}
int powman_off(void)
{
    // Set power states
    bool valid_state = powman_configure_wakeup_state(off_state, on_state);
    if (!valid_state)
    {
        return PICO_ERROR_INVALID_STATE;
    }
    // reboot to main
    powman_hw->boot[0] = 0;
    powman_hw->boot[1] = 0;
    powman_hw->boot[2] = 0;
    powman_hw->boot[3] = 0;
    // Switch to required power state
    int rc = powman_set_power_state(off_state);
    if (rc != PICO_OK)
    {
        return rc;
    }
    // Power down
    while (true)
        __wfi();
}
int powman_off_until_gpio_high(int gpio)
{
    gpio_init(gpio);
    gpio_set_dir(gpio, false);
    if (gpio_get(gpio))
    {
        while (gpio_get(gpio))
        {
            sleep_ms(100);
        }
    }
    printf("Powering off until GPIO %d goes high\n", gpio);
    powman_enable_gpio_wakeup(0, gpio, false, true);
    return powman_off();
}
int powman_off_until_time(uint64_t abs_time_ms)
{
    powman_enable_alarm_wakeup_at_ms(abs_time_ms);
    return powman_off();
}
int powman_off_for_ms(uint64_t duration_ms)
{
    uint64_t ms = powman_timer_get_ms();
    return powman_off_until_time(ms + duration_ms);
}
