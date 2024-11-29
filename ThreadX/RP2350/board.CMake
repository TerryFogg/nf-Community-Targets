#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/board.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/tx_initialize_low_level.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2350/Initialize_Graphics.cpp
         
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/RP2XXX/pico-LCD-1.14V2-ST7789VW.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/RP2XXX/Spi_To_Display_Pico.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/RP2XXX/usb_cdc_descriptor.c
    )
