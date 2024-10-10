

# Access to include files for local code

     list(APPEND RP2XXX_INCLUDES 
             ${TARGET_GIT_LOCAL_PATH}
             ${TARGET_GIT_LOCAL_PATH}/src/boards/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/boot_picobin_headers/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/boot_picoboot_headers/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/hardware_claim/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_base_headers/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_standard_binary_info
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_bit_ops/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_binary_info/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_stdlib/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_stdlib_headers/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_sync/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_time/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_usb_reset_interface/include
             ${TARGET_GIT_LOCAL_PATH}/src/common/pico_util/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/cmsis/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_adc/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_base/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_boot_lock/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_clocks/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_divider/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_divider
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_dma/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_exception/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_flash/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_gpio/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_i2c/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_interp/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_irq/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_pio/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_pll/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_pwm/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_resets/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_rtc/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_spi/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_sync_spin_lock/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_sync/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_ticks/include   # check for rp2350 specific
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_timer/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_uart/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_vreg/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_watchdog/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_xosc/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_bootrom/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_double/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_float/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_int64_ops/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_i2c_slave/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_malloc/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_multicore/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_platform_compiler/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_platform_panic/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_platform_sections/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_printf/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_runtime/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_runtime_init/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_stdio
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_stdio/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_stdio_uart/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_time_adapter/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_unique_id/include
             
 #            ${TARGET_GIT_LOCAL_PATH}/src/rp2040/boot_stage2/include
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include   

 #            ${TARGET_GIT_LOCAL_PATH}/src/rp2040/pico_platform/include
 #            ${TARGET_GIT_LOCAL_PATH}/src/rp2040/hardware_regs/include
 #            ${TARGET_GIT_LOCAL_PATH}/src/rp2040/hardware_structs/include

             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/class/cdc
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/common
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/device

             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/portable/raspberrypi/rp2040
     )

     list(APPEND RP2XXX_SOURCES

             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/tusb.c
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/class/cdc/cdc_device.c
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/common/tusb_fifo.c
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/device/usbd.c
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/device/usbd_control.c
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c
             ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c
             ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c
     )

        set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/src/common/pico_sync/sem.c COMPILE_FLAGS -std=gnu17)
        set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_runtime_init/runtime_init.c COMPILE_FLAGS -std=gnu17)
        set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_multicore/multicore.c COMPILE_FLAGS -std=gnu17)
