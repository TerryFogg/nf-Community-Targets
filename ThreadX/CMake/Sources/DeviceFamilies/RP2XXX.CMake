#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# Check and find source repository

set(PICO_NO_CMSE true)
# Don't build pioasm use a prebuilt one
set(PIOASM_EXE "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Vendors/RaspberryPi/RP2XXX/pioasm.exe")

# Define an imported executable target so pioasm is not built.
add_executable(pioasm IMPORTED GLOBAL)
set_target_properties(pioasm PROPERTIES IMPORTED_LOCATION "${PIOASM_EXE}"
)


include(FetchContent)
if(IS_DIRECTORY ${TARGET_GIT_LOCAL_PATH})
    message(STATUS "Source code support is local folder ${TARGET_GIT_LOCAL_PATH}")
    FetchContent_Declare(
        Repository
        SOURCE_DIR ${TARGET_GIT_LOCAL_PATH}
    )
else()
    message(STATUS "Source code from GitHub repo version ${TARGET_GIT_REPOSITORY}")
    FetchContent_Declare(
        Repository
        GIT_REPOSITORY  ${TARGET_GIT_REPOSITORY}
        GIT_TAG ${TARGET_GIT_TAG}
    )
endif()                                
FetchContent_MakeAvailable(Repository)


# Configure the sizes in the linker script file based on CMakePresets.json variables
Set( LINKER_SCRIPT_FILE "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Vendors/RaspberryPi/RP2XXX/RP2XXX.ld")
configure_file(  ${LINKER_SCRIPT_FILE} "${CMAKE_BINARY_DIR}/configured_linker_Script.ld" @ONLY)
set_target_properties(nanoCLR PROPERTIES LINK_DEPENDS "${CMAKE_BINARY_DIR}/configured_linker_Script.ld")
set_target_properties(nanoCLR PROPERTIES PICO_TARGET_LINKER_SCRIPT ${CMAKE_BINARY_DIR}/configured_linker_Script.ld)

include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

pico_sdk_init()

# Add these includes for local code references
list(APPEND RP2XXX_INCLUDES 
     ${TARGET_GIT_LOCAL_PATH}
     ${TARGET_GIT_LOCAL_PATH}/src/common/boot_picobin_headers/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/boot_picoboot_headers/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/hardware_claim/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_base_headers/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_bit_ops/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_binary_info/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_standard_binary_info
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_stdlib/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_stdlib_headers/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_sync/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_time/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_util/include

     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_usb_reset_interface/include
     ${TARGET_GIT_LOCAL_PATH}/src/common/pico_usb_reset_interface_headers/include

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
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_ticks/include                    # check for rp2350 specific
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_timer/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_uart/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_vreg/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_watchdog/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/hardware_xosc/include

     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_async_context/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_bootrom/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_cyw43_arch/include
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_cyw43_driver/include
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
     ${TARGET_GIT_LOCAL_PATH}/lib/cyw43-driver
     ${TARGET_GIT_LOCAL_PATH}/lib/cyw43-driver/src
     ${TARGET_GIT_LOCAL_PATH}/lib/cyw43-driver/firmware
     
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/class/cdc
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/common
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/device
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/portable/raspberrypi/rp2040
     
     ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Vendors/${TARGET_VENDOR}/${TARGET_FAMILY}
     ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Vendors/${TARGET_VENDOR}/${TARGET_FAMILY}/${TARGET_SERIES}
      
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/include   
)

if( "${TARGET_SERIES}" STREQUAL "RP2040")
     list(APPEND RP2XXX_INCLUDES 
          ${TARGET_GIT_LOCAL_PATH}/src/rp2040/boot_stage2/include
          ${TARGET_GIT_LOCAL_PATH}/src/rp2040/hardware_regs/include
          ${TARGET_GIT_LOCAL_PATH}/src/rp2040/hardware_structs/include
          ${TARGET_GIT_LOCAL_PATH}/src/rp2040/pico_platform/include
     )
elseif( "${TARGET_SERIES}" STREQUAL "RP2350")
     list(APPEND RP2XXX_INCLUDES 
          ${TARGET_GIT_LOCAL_PATH}/src/rp2350/boot_stage2/include
          ${TARGET_GIT_LOCAL_PATH}/src/rp2350/hardware_regs/include
          ${TARGET_GIT_LOCAL_PATH}/src/rp2350/hardware_structs/include
          ${TARGET_GIT_LOCAL_PATH}/src/rp2350/pico_platform/include
     )
endif()

# Using NetxDuo so we only need the base code, other code has lwip embedded
# The physical device driver is a special 
#if(NETWORKING_SUPPORT)
#   list(APPEND RP2XXX_SOURCES
#        ${TARGET_GIT_LOCAL_PATH}/lib/cyw43-driver/src/cyw43_ctrl.c
#        ${TARGET_GIT_LOCAL_PATH}/lib/cyw43-driver/src/cyw43_ll.c
#
#        ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_async_context/async_context_base.c
#        ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_async_context/async_context_poll.c
#        
#        ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_cyw43_driver/cyw43_driver.c
#        ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_cyw43_arch/cyw43_arch.c 
#        ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_cyw43_arch/cyw43_arch_poll.c
#        ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_cyw43_driver/cyw43_bus_pio_spi.c
#
#   )
#endif()

# Add the TinyUsb sources - Note: RP2350 uses some RP2040 files
list(APPEND RP2XXX_SOURCES
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/tusb.c
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/class/cdc/cdc_device.c
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/common/tusb_fifo.c
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/device/usbd.c
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/device/usbd_control.c
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c
     ${TARGET_GIT_LOCAL_PATH}/lib/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c
     ${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_fix/rp2040_usb_device_enumeration/rp2040_usb_device_enumeration.c

     # Wire Protocol support
     ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Vendors/RaspberryPi/RP2XXX/tinyusb_cdc_descriptor.c
)

# Set properties on certain files to compile successfully
set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/src/common/pico_sync/sem.c COMPILE_FLAGS -std=gnu17)
set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_runtime_init/runtime_init.c COMPILE_FLAGS -std=gnu17)
set_source_files_properties(${TARGET_GIT_LOCAL_PATH}/src/rp2_common/pico_multicore/multicore.c COMPILE_FLAGS -std=gnu17)

if(NETWORKING_SUPPORT)
  #  target_link_libraries(nanoCLR cyw43_driver )
    target_link_libraries(nanoCLR pico_cyw43_arch_none )
endif()

# Pico SDK builds INTERFACE libraries
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
            #pico_stdio
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
