#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

set(CMAKE_C_STANDARD 17 CACHE INTERNAL "C standard for all targets")
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "C++ standard for all targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")

# Raspberry Pi Pico uses the Pico-Sdk for build parameters


 if( "${TARGET_SERIES}" STREQUAL "RP2350")
    target_compile_definitions(nanoCLR PUBLIC -DTX_SINGLE_MODE_NON_SECURE=1) 
 endif()
    target_compile_definitions(nanoCLR PUBLIC -DCFG_TUSB_MCU=OPT_MCU_RP2040) 
 

# Post build commands to add a listing file
    add_custom_command(TARGET nanoCLR POST_BUILD
      COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:nanoCLR> > nanoCLR.lst
    )


