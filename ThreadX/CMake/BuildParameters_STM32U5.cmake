#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Common compiler definitions
# --------------------------------------------------------------
set(CMAKE_C_STANDARD 17 CACHE INTERNAL "C standard for all targets")
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "C++ standard for all targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")

target_compile_definitions(nanoCLR.elf PUBLIC -DUSE_FULL_LL_DRIVER -GPDMA1)

target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-mthumb -mcpu=cortex-m33 -mfpu=fpv5-d16 -mfloat-abi=hard -frounding-math -fsignaling-nans -ffloat-store>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-fno-exceptions -fno-builtin -fno-common -fno-unroll-loops -fno-math-errno>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-fshort-wchar -falign-functions=16>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-ffunction-sections -fdata-sections -fomit-frame-pointer>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-mabi=aapcs -mlong-calls -fdollars-in-identifiers -ftree-vectorize -fcheck-new>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-builtin -fno-common -fno-unroll-loops -fno-math-errno>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -frounding-math -fsignaling-nans -ffloat-store>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fshort-wchar -falign-functions=16>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections -fdata-sections -fomit-frame-pointer>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mabi=aapcs -mlong-calls -fdollars-in-identifiers -ftree-vectorize -fcheck-new>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m33 -x assembler-with-cpp>)

#if( CMAKE_BUILD_TYPE STREQUAL "Debug")
#   target_compile_options(nanoCLR.elf PUBLIC  "-Og -ggdb")
#elseif( CMAKE_BUILD_TYPE STREQUAL "Release")
#   target_compile_options(nanoCLR.elf PUBLIC  "-O3 -flto -ffat-lto-objects")
#elseif( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#   target_compile_options(nanoCLR.elf PUBLIC  "-Os -femit-class-debug-always -ggdb")
#elseif( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
#   target_compile_options(nanoCLR.elf PUBLIC  "-Os -flto -ffat-lto-objects")
#endif()

## Link Options
target_link_options(nanoCLR.elf PUBLIC "--specs=nano.specs")
target_link_options(nanoCLR.elf PUBLIC "--specs=nosys.specs")
target_link_options(nanoCLR.elf PUBLIC "-mthumb")
target_link_options(nanoCLR.elf PUBLIC "-mcpu=cortex-m33")
target_link_options(nanoCLR.elf PUBLIC "-mfpu=fpv5-sp-d16")
target_link_options(nanoCLR.elf PUBLIC "-mfloat-abi=hard")
target_link_options(nanoCLR.elf PUBLIC "-mabi=aapcs")
target_link_options(nanoCLR.elf PUBLIC "-Wl,--gc-sections")
target_link_options(nanoCLR.elf PUBLIC "-Wl,--no-wchar-size-warning")
target_link_options(nanoCLR.elf PUBLIC "-Wl,--print-memory-usage")
