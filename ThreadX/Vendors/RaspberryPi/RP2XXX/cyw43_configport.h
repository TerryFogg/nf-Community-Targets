#pragma once

//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>
#include "hardware/gpio.h"
#include "pico/time.h"
#include "nanoHAL_v2.h"

// We are not using lwIP
//#define CYW43_LWIP     (0) // Disable LWIP
//#define CYW43_USE_LWIP (0) // Disable LWIP stack integration for netXduo implementation



#define CYW43_RESOURCE_VERIFY_DOWNLOAD (1)
#define CYW43_THREAD_ENTER             cyw43_thread_enter();
#define CYW43_THREAD_EXIT              cyw43_thread_exit();

// Country Code (optional)
// Helps with regulatory compliance and Wi-Fi channel selection
#define CYW43_COUNTRY_CODE "AU" // Uncomment and set if needed

#define CYW43_USE_SPI 1 // We are using SPI as the bus
#define CYW43_SPI_PIO 1 // That SPI is implemented using PIO


// The following pins can be defined, depending on the configuration:
#define CYW43_PIN_RFSW_SELECT (0) // RF-switch select
#define CYW43_PIN_WL_IRQ      (0) // the gSPI IRQ line
#define CYW43_PIN_RFSW_VDD    (2) // - RF - switch power(active high)
#define CYW43_PIN_WL_SDIO_1   (3) // - for SDIO transport

// Pimoroni pico plus2 rp2350
// gpio pin to power up the cyw43 chip
#define CYW43_DEFAULT_PIN_WL_REG_ON    23
#define CYW43_DEFAULT_PIN_WL_DATA_OUT  24
#define CYW43_DEFAULT_PIN_WL_DATA_IN   24
#define CYW43_DEFAULT_PIN_WL_HOST_WAKE 24
#define CYW43_DEFAULT_PIN_WL_CLOCK     29
#define CYW43_DEFAULT_PIN_WL_CS        25u
#define CYW43_WL_GPIO_COUNT            3
#define CYW43_WL_GPIO_LED_PIN          0

#define CYW43_PIN_WL_REG_ON    CYW43_DEFAULT_PIN_WL_REG_ON    // - WL power on(active high)
#define CYW43_PIN_WL_DATA_OUT  CYW43_DEFAULT_PIN_WL_DATA_OUT  // - spi data out
#define CYW43_PIN_WL_DATA_IN   CYW43_DEFAULT_PIN_WL_DATA_IN   // - spi data in
#define CYW43_PIN_WL_HOST_WAKE CYW43_DEFAULT_PIN_WL_HOST_WAKE // - for SDIO or SPI transport
#define CYW43_PIN_WL_CLOCK     CYW43_DEFAULT_PIN_WL_CLOCK
#define CYW43_PIN_WL_CS        CYW43_DEFAULT_PIN_WL_CS
#define CYW43_NUM_GPIOS        CYW43_WL_GPIO_COUNT

// If CYW43_WL_GPIO_VBUS_PIN is defined then a CYW43 GPIO has to be used to read VBUS.
// This can be passed to cyw43_arch_gpio_get to determine if the device is battery powered.
// PICO_VBUS_PIN and CYW43_WL_GPIO_VBUS_PIN should not both be defined.
#define CYW43_WL_GPIO_VBUS_PIN 2

// If CYW43_USES_VSYS_PIN is defined then CYW43 uses the VSYS GPIO (defined by PICO_VSYS_PIN) for other purposes.
// If this is the case, to use the VSYS GPIO it's necessary to ensure CYW43 is not using it.
// This can be achieved by wrapping the use of the VSYS GPIO in cyw43_thread_enter / cyw43_thread_exit.
#define CYW43_USES_VSYS_PIN 1

// Note, these are negated, because cyw43_driver negates them before returning!
#define CYW43_EPERM     (-PICO_ERROR_NOT_PERMITTED) // Operation not permitted
#define CYW43_EIO       (-PICO_ERROR_IO)            // I/O error
#define CYW43_EINVAL    (-PICO_ERROR_INVALID_ARG)   // Invalid argument
#define CYW43_ETIMEDOUT (-PICO_ERROR_TIMEOUT)       // Connection timed out

#define CYW43_HAL_PIN_MODE_INPUT  (GPIO_IN)
#define CYW43_HAL_PIN_MODE_OUTPUT (GPIO_OUT)
#define CYW43_HAL_PIN_PULL_NONE   (0)
#define CYW43_HAL_PIN_PULL_UP     (1)
#define CYW43_HAL_PIN_PULL_DOWN   (2)
#define CYW43_HAL_MAC_WLAN0       (0)

#define CYW43_ENABLE_BLUETOOTH_OVER_UART (1)

#define CYW43_BT_FIRMWARE_INCLUDE_FILE "firmware/cyw43_btfw_4343A1.h"
#define CYW43_PIN_BT_REG_ON            (0) // BT power on (active high)
#define CYW43_PIN_BT_HOST_WAKE         (0) // for BT HCI UART transport
#define CYW43_PIN_BT_DEV_WAKE          (0) // for BT HCI UART transport
#define CYW43_PIN_BT_CTS               (0) // for BT HCI UART transport, used to wait for CTS to go low

#define cyw43_hal_pin_obj_t           uint
#define CYW43_HAL_MAC_BDADDR          (1)
#define CYW43_HOST_NAME               "PicoW"
#define CYW43_GPIO                    1
#define CYW43_LOGIC_DEBUG             0
#define CYW43_USE_OTP_MAC             1
#define CYW43_NO_NETUTILS             1
#define CYW43_IOCTL_TIMEOUT_US        1000000
#define CYW43_USE_STATS               0
#define CYW43_WIFI_NVRAM_INCLUDE_FILE "wifi_nvram_43439.h"

#define cyw43_arch_lwip_check() ((void)0)
#define CYW43_THREAD_LOCK_CHECK

// todo not 100% sure about the timeouts here; MP uses __WFI which will always wakeup periodically
#define CYW43_SDPCM_SEND_COMMON_WAIT cyw43_await_background_or_timeout_us(1000);
#define CYW43_DO_IOCTL_WAIT          cyw43_await_background_or_timeout_us(1000);
#define CYW43_POST_POLL_HOOK         cyw43_post_poll_hook();
#define cyw43_malloc                 platform_malloc
#define cyw43_free                   platform_free
#define CYW43_WL_GPIO_COUNT          3

// get the number of elements in a fixed-size array
#define CYW43_ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

// Combined for both Wifi and bluetooth
#define CYW43_CHIPSET_FIRMWARE_INCLUDE_FILE "wb43439A0_7_95_49_00_combined.h"

#ifdef __cplusplus
extern "C"
{
#endif
    void cyw43_delay_us(uint32_t ms);
    void cyw43_delay_ms(uint32_t ms);
    void cyw43_thread_enter(void);
    void cyw43_thread_exit(void);
    void cyw43_await_background_or_timeout_us(uint32_t timeout_us);
    void cyw43_delay_ms(uint32_t ms);
    void cyw43_schedule_internal_poll_dispatch(void (*func)(void));
    void cyw43_post_poll_hook(void);
    void cyw43_hal_generate_laa_mac(int idx, uint8_t buf[6]);
    void cyw43_hal_get_mac(int idx, uint8_t buf[6]);

    static inline void cyw43_hal_pin_config_irq_falling(int pin, int enable)
    {
        (void)pin;
        (void)enable;
    }
    static inline uint32_t cyw43_hal_ticks_us(void)
    {
        return time_us_32();
    }
    static inline uint32_t cyw43_hal_ticks_ms(void)
    {
        return to_ms_since_boot(get_absolute_time());
    }
    static inline int cyw43_hal_pin_read(cyw43_hal_pin_obj_t pin)
    {
        return gpio_get(pin);
    }
    static inline void cyw43_hal_pin_low(cyw43_hal_pin_obj_t pin)
    {
        gpio_put(pin, false);
    }
    static inline void cyw43_hal_pin_high(cyw43_hal_pin_obj_t pin)
    {
        gpio_put(pin, true);
    }
    static inline void cyw43_hal_pin_config(
        cyw43_hal_pin_obj_t pin,
        uint32_t mode,
        uint32_t pull,
        __unused uint32_t alt)
    {
        assert((mode == CYW43_HAL_PIN_MODE_INPUT || mode == CYW43_HAL_PIN_MODE_OUTPUT) && alt == 0);
        gpio_set_dir(pin, mode);
        gpio_set_pulls(pin, pull == CYW43_HAL_PIN_PULL_UP, pull == CYW43_HAL_PIN_PULL_DOWN);
    }
    // void cyw43_hal_generate_laa_mac(int idx, uint8_t buf[6]);

#ifdef __cplusplus
}
#endif
