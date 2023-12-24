#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
    list(APPEND NF_INTERPRETER_SOURCES
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Core1Tasks.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/hard_fault_handler.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/nanoCRT.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/nanoHAL.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/target_common.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/targetHAL.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/targetHAL_board.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/targetHAL_ConfigurationManager.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/targetHAL_Power.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/targetHAL_Rtos.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/HAL/targetHAL_Time.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/main.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Memory/Device_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Memory/FlashDriver.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Memory/Memory.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Memory/platform_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Memory/target_BlockStorage.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/nanoFramework.Hardware.RpiPico/NativeRpiPico.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/nanoFramework.Hardware.RpiPico/NativeRpiPico_nanoFramework_Hardware_RpiPico_Configuration.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/PAL/platform_heap.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/PAL/targetPAL.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/PAL/targetPAL_Events.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/PAL/targetPAL_Time.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Startup/CLR_RT_InteropAssembliesTable.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Startup/CLR_Startup_Thread.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/Startup/nanoFramework_Threads.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/System.Device.Adc/nanoFramework.System.Device.Adc.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/System.Device.Gpio/nanoFramework.System.Device.Gpio.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/System.Device.I2c/nanoFramework.System.Device.I2c.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/System.Device.Pwm/nanoFramework.System.Device.Pwm.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/System.Device.Spi/nanoFramework.System.Device.Spi.cpp
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/System.IO.Ports/CircularBuffer.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/WireProtocol/usb_descriptors.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/WireProtocol/Wire_Protocol_HAL_Interface.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/WireProtocol/WireProtocol_ReceiverThread.c
                ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RP2040/nanoCLR/WireProtocol/wp_Communications.c
         )                     
# Graphics         
if(NANOCLR_GRAPHICS)
  list(APPEND NF_INTERPRETER_GRAPHICS_SOURCES
             ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Displays/Generic_SPI.cpp
             ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Displays/Spi_To_Display.cpp
             ${CMAKE_SOURCE_DIR}/ThreadX/RP2040/nanoCLR/nanoFramework.Graphics/Graphics_Memory.cpp
             ${CMAKE_SOURCE_DIR}/ThreadX/RP2040/nanoCLR/nanoFramework.Graphics/Initialize_Graphics.cpp
             ${CMAKE_SOURCE_DIR}/ThreadX/RP2040/nanoCLR/nanoFramework.Graphics/TouchDevice_NULL.cpp
  )
endif()

