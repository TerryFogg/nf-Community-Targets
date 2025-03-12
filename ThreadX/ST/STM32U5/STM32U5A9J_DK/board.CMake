#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
    list(APPEND NF_INTERPRETER_SOURCES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/startup_stm32u5a9njhxq.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H5/STM32U5A9J_DK/tx_initialize_low_level.S
    )
     if(GRAPHICS_SUPPORT)
       list(APPEND NF_INTERPRETER_SOURCES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Initialize_Graphics.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/LTDC_DSI_DMA2D_Controller.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32U5A9J_DK/Stm32U5a9_Video_Controller.cpp
                                       
    )
     endif()
