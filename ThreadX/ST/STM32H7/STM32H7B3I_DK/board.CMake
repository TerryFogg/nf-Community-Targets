#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

     list(APPEND NF_INTERPRETER_INCLUDES
                             ${STM32H7_CUBE_SOURCE}/Middlewares/Third_Party/FatFs/src
     )

     list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/board.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/Clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/MPU_Config.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/STM32_FMC_SDRAM.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/startup_stm32h7b3xxq.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/STM32H7B3I_DK/tx_initialize_low_level.s
    
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/Common/LTDC_PARALLEL_DMA2D_To_Video.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/ST/STM32H7/Common/VideoMode.cpp

)    
