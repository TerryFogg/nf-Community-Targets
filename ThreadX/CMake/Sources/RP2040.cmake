#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# RP2040 series

# All libraries within the SDK are INTERFACE libraries. (Note this does not include the C/C++ standard libraries provided by
# the compiler). Conceptually, a CMake INTERFACE library is a collection of:
# - Source files
# - Include paths
# - Compiler definitions (visible to code as #defines)
# - Compile and link options
# - Dependencies (on other INTERFACE libraries)


#list(APPEND PICO_SDK_LIBRARIES 
#            pico_stdlib
#
#            hardware_irq 
#            hardware_claim
#            hardware_divider
#            hardware_divider_headers
#            hardware_dma
#            hardware_exception
#            hardware_flash
#            hardware_interp
#            hardware_pll
#            hardware_regs
#            hardware_resets
#            hardware_rtc
#            hardware_sync
#            hardware_timer
#            hardware_vreg
#            hardware_watchdog
#            hardware_xosc
#            pico_bit_ops 
#            pico_divider
#            pico_double
#            pico_float
#            pico_int64_ops
#           # pico_mems_op         # follow up, provides optimised machine code versions of memset, memcpy
#            pico_standard_link
#            pico_async_context_base
#            pico_async_context_poll
#            pico_async_context_threadsafe_background
#            pico_binary_info
#            pico_bootrom
#            pico_bootsel_via_double_reset
#            pico_fix_rp2040_usb_device_enumeration
#            pico_rand
#            pico_runtime
#            pico_stdio_semihosting
#            pico_stdio_uart
#            pico_unique_id
#            pico_multicore
#            #lwip
#
#            #header
#            hardware_base
#            hardware_base_headers
#
#
#            hardware_clocks
#
#            #IO
#            hardware_adc
#            hardware_gpio
#            hardware_i2c
#            hardware_pio
#            hardware_pwm
#            hardware_spi
#            hardware_structs
#            hardware_uart
#            pico_usb_reset_interface
#            tinyusb_board
#            tinyusb_common_base
#            tinyusb_device
#)

 list(APPEND PICO_SDK_INCLUDES 
             ${pico_sdk_SOURCE_DIR}
             ${pico_sdk_SOURCE_DIR}/src/boards/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_base/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_binary_info/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_binary_info/include/pico
             ${pico_sdk_SOURCE_DIR}/src/common/pico_binary_info/include/pico/binary_info
             ${pico_sdk_SOURCE_DIR}/src/common/pico_bit_ops/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_divider/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_stdlib/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_sync/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_time/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_usb_reset_interface/include
             ${pico_sdk_SOURCE_DIR}/src/common/pico_util/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/boot_stage2/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/cmsis/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_adc/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_base/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_claim/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_clocks/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_divider/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_dma/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_exception/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_flash/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_gpio/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_i2c/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_interp/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_irq/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_pio/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_pll/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_pwm/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_resets/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_rtc/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_spi/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_sync/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_timer/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_uart/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_vreg/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_watchdog/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_xosc/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_bootrom/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_double/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_float/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_int64_ops/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_malloc/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_multicore/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_platform/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_printf/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_runtime/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_stdio
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_stdio/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_stdio_uart/include
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_unique_id/include
             ${pico_sdk_SOURCE_DIR}/src/rp2040/hardware_regs/include
             ${pico_sdk_SOURCE_DIR}/src/rp2040/hardware_structs/include

             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/class/cdc
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/common
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/device
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/rp2040

 )
 list(APPEND PICO_SDK_SOURCES
             ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/RaspberryPi/RP2040/nanoCLR/Startup/bs2_default_padded_checksummed.s

             ${pico_sdk_SOURCE_DIR}/src/common/pico_sync/critical_section.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_sync/lock_core.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_sync/mutex.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_sync/sem.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_time/time.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_time/timeout_helper.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_util/datetime.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_util/pheap.c
             ${pico_sdk_SOURCE_DIR}/src/common/pico_util/queue.c

             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_adc/adc.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_claim/claim.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_clocks/clocks.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_dma/dma.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_exception/exception.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_flash/flash.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_gpio/gpio.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_i2c/i2c.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_interp/interp.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_irq/irq.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_irq/irq_handler_chain.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_pio/pio.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_pll/pll.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_rtc/rtc.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_spi/spi.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_sync/sync.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_timer/timer.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_uart/uart.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_vreg/vreg.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_watchdog/watchdog.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/hardware_xosc/xosc.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_bootrom/bootrom.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_divider/divider.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_double/double_aeabi.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_double/double_init_rom.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_double/double_math.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_double/double_v1_rom_shim.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_float/float_aeabi.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_float/float_init_rom.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_float/float_math.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_float/float_v1_rom_shim.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_malloc/pico_malloc.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_multicore/multicore.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_platform/platform.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_printf/printf.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_runtime/runtime.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_standard_link/binary_info.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_standard_link/crt0.S
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_standard_link/new_delete.cpp
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_stdio/stdio.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_stdio_uart/stdio_uart.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_stdlib/stdlib.c
             ${pico_sdk_SOURCE_DIR}/src/rp2_common/pico_unique_id/unique_id.c

             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/tusb.c
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/class/cdc/cdc_device.c
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/common/tusb_fifo.c
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/device/usbd.c
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/device/usbd_control.c
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c
             ${pico_sdk_SOURCE_DIR}/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c
 )

                    