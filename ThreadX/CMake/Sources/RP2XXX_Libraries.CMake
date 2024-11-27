# pull in common dependencies
target_link_libraries(nanoCLR
    hardware_adc
    hardware_clocks
    hardware_divider
    hardware_dma
    hardware_exception
    hardware_flash
    hardware_gpio
    hardware_i2c
    hardware_interp
    hardware_irq
    hardware_pio
    hardware_pll
    hardware_pwm
    hardware_resets
    hardware_spi
    hardware_sync
    hardware_ticks
    hardware_timer
    hardware_uart
    hardware_vreg
    hardware_watchdog
    hardware_xosc
    pico_aon_timer
    pico_bit_ops
    pico_bootrom
   # pico_bootsel_via_double_reset
    pico_divider
    pico_double
    pico_fix_rp2040_usb_device_enumeration
    pico_flash
    pico_float
    pico_i2c_slave
    pico_int64_ops
    pico_malloc
    pico_mem_ops
    pico_multicore
    pico_platform
    pico_rand
    pico_stdlib
#    pico_stdio
    pico_sync
    pico_time
    pico_unique_id
    pico_util

    #tinyusb_board
)

if( "${TARGET_SERIES}" STREQUAL "RP2040")
   target_link_libraries(nanoCLR
                         hardware_rtc
   )
endif()

if( "${TARGET_SERIES}" STREQUAL "RP2350")
   target_link_libraries(nanoCLR
                         hardware_powman
                         pico_sha256
   )
endif()

# create map/bin/hex file etc.
pico_add_extra_outputs(nanoCLR)
