#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/RP2350/board.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/RP2350/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/RP2350/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/RP2350/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/RP2350/tx_initialize_low_level.s

                # Wire Protocol support
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/Common/usb_cdc_descriptor.c

    )

if(GRAPHICS_SUPPORT)
    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/${GRAPHICS_LCD}
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Spi_To_Display_Pico.cpp
    )
endif()
