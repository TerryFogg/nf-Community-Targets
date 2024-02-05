#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

     list(APPEND NF_INTERPRETER_SOURCES 
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/Clock_configuration.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/Initialize_Graphics.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/MicroSD.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/startup_stm32h7b3xxq.s
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/STM32H7B3I_DK/tx_initialize_low_level.s
    
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.I2c.cpp


                             

                ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/Src/stm32h7xx_hal.c
                ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/Src/stm32h7xx_hal_cortex.c
                ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/Src/stm32h7xx_hal_rcc.c
                ${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/Src/stm32h7xx_hal_pwr.c
                
     )

    set_source_files_properties(${STM32H7_CUBE_SOURCE}/Drivers/STM32H7xx_Hal_Driver/src/stm32h7xx_hal_pwr.c COMPILE_FLAGS -Wno-unused-parameter)
   