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
             ${PICO_SDK_SOURCE}
             ${PICO_SDK_SOURCE}/src/boards/include
             ${PICO_SDK_SOURCE}/src/common/pico_base/include
             ${PICO_SDK_SOURCE}/src/common/pico_binary_info/include
             ${PICO_SDK_SOURCE}/src/common/pico_binary_info/include/pico
             ${PICO_SDK_SOURCE}/src/common/pico_binary_info/include/pico/binary_info
             ${PICO_SDK_SOURCE}/src/common/pico_bit_ops/include
             ${PICO_SDK_SOURCE}/src/common/pico_divider/include
             ${PICO_SDK_SOURCE}/src/common/pico_stdlib/include
             ${PICO_SDK_SOURCE}/src/common/pico_sync/include
             ${PICO_SDK_SOURCE}/src/common/pico_time/include
             ${PICO_SDK_SOURCE}/src/common/pico_usb_reset_interface/include
             ${PICO_SDK_SOURCE}/src/common/pico_util/include
             ${PICO_SDK_SOURCE}/src/rp2_common/boot_stage2/include
             ${PICO_SDK_SOURCE}/src/rp2_common/cmsis/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_adc/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_base/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_claim/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_clocks/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_divider/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_dma/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_exception/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_flash/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_gpio/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_i2c/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_interp/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_irq/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_pio/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_pll/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_pwm/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_resets/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_rtc/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_spi/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_sync/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_timer/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_uart/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_vreg/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_watchdog/include
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_xosc/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_bootrom/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_double/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_float/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_int64_ops/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_malloc/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_multicore/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_platform/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_printf/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_runtime/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_stdio
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_stdio/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_stdio_uart/include
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_unique_id/include
             ${PICO_SDK_SOURCE}/src/rp2040/hardware_regs/include
             ${PICO_SDK_SOURCE}/src/rp2040/hardware_structs/include

             ${PICO_SDK_SOURCE}/lib/tinyusb/src
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/class/cdc
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/common
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/device
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/portable/raspberrypi/rp2040

 )
 list(APPEND PICO_SDK_SOURCES
             ${CMAKE_SOURCE_DIR}/targets/AzureRTOS/RaspberryPi/RP2040/nanoCLR/Startup/bs2_default_padded_checksummed.s

             ${PICO_SDK_SOURCE}/src/common/pico_sync/critical_section.c
             ${PICO_SDK_SOURCE}/src/common/pico_sync/lock_core.c
             ${PICO_SDK_SOURCE}/src/common/pico_sync/mutex.c
             ${PICO_SDK_SOURCE}/src/common/pico_sync/sem.c
             ${PICO_SDK_SOURCE}/src/common/pico_time/time.c
             ${PICO_SDK_SOURCE}/src/common/pico_time/timeout_helper.c
             ${PICO_SDK_SOURCE}/src/common/pico_util/datetime.c
             ${PICO_SDK_SOURCE}/src/common/pico_util/pheap.c
             ${PICO_SDK_SOURCE}/src/common/pico_util/queue.c

             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_adc/adc.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_claim/claim.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_clocks/clocks.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_dma/dma.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_exception/exception.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_flash/flash.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_gpio/gpio.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_i2c/i2c.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_interp/interp.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_irq/irq.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_irq/irq_handler_chain.S
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_pio/pio.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_pll/pll.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_rtc/rtc.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_spi/spi.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_sync/sync.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_timer/timer.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_uart/uart.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_vreg/vreg.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_watchdog/watchdog.c
             ${PICO_SDK_SOURCE}/src/rp2_common/hardware_xosc/xosc.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_bit_ops/bit_ops_aeabi.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_bootrom/bootrom.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_divider/divider.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_double/double_aeabi.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_double/double_init_rom.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_double/double_math.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_double/double_v1_rom_shim.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_float/float_aeabi.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_float/float_init_rom.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_float/float_math.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_float/float_v1_rom_shim.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_int64_ops/pico_int64_ops_aeabi.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_malloc/pico_malloc.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_mem_ops/mem_ops_aeabi.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_multicore/multicore.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_platform/platform.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_printf/printf.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_runtime/runtime.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_standard_link/binary_info.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_standard_link/crt0.S
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_standard_link/new_delete.cpp
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_stdio/stdio.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_stdio_uart/stdio_uart.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_stdlib/stdlib.c
             ${PICO_SDK_SOURCE}/src/rp2_common/pico_unique_id/unique_id.c

             ${PICO_SDK_SOURCE}/lib/tinyusb/src/tusb.c
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/class/cdc/cdc_device.c
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/common/tusb_fifo.c
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/device/usbd.c
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/device/usbd_control.c
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c
             ${PICO_SDK_SOURCE}/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c
 )

                    