#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# Compiler definitions
# --------------------------------------------------------------
set(CMAKE_C_STANDARD 17 CACHE INTERNAL "C standard for all targets")
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "C++ standard for all targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")

 target_compile_definitions(nanoCLR PUBLIC -DPLATFORM_ARM)

target_compile_definitions(nanoCLR PUBLIC -DTX_SINGLE_MODE_NON_SECURE=1 -DARM_MATH_CM33  -DUSE_FULL_LL_DRIVER -DOCTOSPI -DHAL_XSPI_MODULE_ENABLED) 
target_compile_definitions(nanoCLR PUBLIC -DDEBUG)
target_compile_definitions(nanoCLR PUBLIC -D${TARGET_SERIES})

target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mcpu=cortex-m33 -ggdb -O0>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-ffunction-sections -fdata-sections>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -mcpu=cortex-m33 -ggdb -O0>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections -fdata-sections>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-frounding-math -fsignaling-nans>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-fno-builtin -fno-common -fno-unroll-loops>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-fshort-wchar -falign-functions=16>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-ffunction-sections -fdata-sections>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-mabi=aapcs -fcheck-new>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mfloat-abi=hard -frounding-math -fsignaling-nans >)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-builtin -fno-common -fno-unroll-loops>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fshort-wchar -falign-functions=16>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections -fdata-sections>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mabi=aapcs -fcheck-new>)
target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR PUBLIC $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m33 -x assembler-with-cpp>)

## Link Options
# Set the linker file and register to detect when linker script changes so a re-link is executed
target_link_options(nanoCLR PUBLIC "-T${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")
set_target_properties(nanoCLR PROPERTIES LINK_DEPENDS "${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")

target_link_options(nanoCLR PUBLIC "--specs=nosys.specs")
target_link_options(nanoCLR PUBLIC "-mthumb")
target_link_options(nanoCLR PUBLIC "-mcpu=cortex-m33")
target_link_options(nanoCLR PUBLIC "-mfpu=fpv5-sp-d16")
target_link_options(nanoCLR PUBLIC "-mfloat-abi=hard")
target_link_options(nanoCLR PUBLIC "-mabi=aapcs")
target_link_options(nanoCLR PUBLIC "-Wl,--gc-sections")
target_link_options(nanoCLR PUBLIC "-Wl,--no-wchar-size-warning")
target_link_options(nanoCLR PUBLIC "-Wl,--print-memory-usage")

# Post build commands to create logs and additional files
    add_custom_command(TARGET nanoCLR POST_BUILD
      COMMAND ${CMAKE_OBJCOPY}           $<TARGET_FILE:nanoCLR>   ${CMAKE_BINARY_DIR}/nanoCLR.elf
      COMMAND ${CMAKE_OBJCOPY} -Oihex    $<TARGET_FILE:nanoCLR>   ${CMAKE_BINARY_DIR}/nanoCLR.hex
      COMMAND ${CMAKE_OBJCOPY} -Obinary  $<TARGET_FILE:nanoCLR>   ${CMAKE_BINARY_DIR}/nanoCLR.bin
      COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:nanoCLR> > ${CMAKE_BINARY_DIR}/nanoCLR.lst
    )
