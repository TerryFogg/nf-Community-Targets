#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

    list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/board.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/Clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/MicroSD.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/OSPI_memory.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/startup_stm32h735xx.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/tx_initialize_low_level.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H735G_DK/tx_initialize_low_level.s

                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/RaspberryPi/STM32H7/Common/LTDC_PARALLEL_DMA2D_To_Video.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/RaspberryPi/STM32H7/Common/VideoMode.cpp
    )
