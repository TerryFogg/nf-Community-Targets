#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Setup the compiler flags for the build
########################################


# Common compiler definitions
# --------------------------------------------------------------
set(CMAKE_C_STANDARD 17 CACHE INTERNAL "C standard for all targets")
set(CMAKE_C_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")
set(CMAKE_CXX_STANDARD 17 CACHE INTERNAL "C++ standard for all targets")
set(CMAKE_CXX_EXTENSIONS OFF CACHE INTERNAL "-std=c++17 instead of -std=gnu++17")


# NOTES:
# -mthumb              : 16-bit instructions instead of 32-bit instructionsfor smaller code size and better performance on systems with limited memory
# -mcpu=cortex-m7      : Optimized for cortex-m7
# -mfpu=fpv5-d16       : Specifies the Floating Point Unit optimized for the ARMv7-M architecture with the FPv5-D16 which is a 
#                        double-precision FPU that supports 16 double-precision registers.
# -mfloat-abi=hard     : Generate code that is optimized for the ARM architecture with hardware floating-point support. 
#                        This option is used when the code is compiled to run on a processor that has a hardware FPU1
# -frounding-math      : Specifies the rounding mode to use for floating-point operation (Default is round to nearest or even)
# -fno-exceptions      : Disables C++ exceptions to reduce significant overhead to the code, both in terms of size and performance.
# -fno-builtin         : Do not include support for built-in functions which can result in smaller and faster code
#                        but it also means that the program will not be able to use built-in functions for certain operations
# -fno-common          : The compiler generates a separate data section for each uninitialized global variable to ensure that each variable is defined only once
#                        and avoids any potential conflicts
# -fno-unroll-loops    : Generates code that does not use loop unrolling optimization. This can result in slower code, but it can also reduce the size of the code 
#                        and improve the effectiveness of other optimizations.
# -fshort-wchar        : The wchar_t type should be 16 bits instead of the default 32 bits reducing memory.It is important to note that using -fshort-wchar can cause 
#                        compatibility issues with code that expects wchar_t to be 32 bits.
# -falign-functions=16 : Align functions to a 16-byte boundary. This can improve performance on some processors by reducing the number of 
#                        cache lines that need to be loaded when calling a function1.
# -ffunction-sections  : Generate code in separate sections of the object file which can reduce the size of the final executable by eliminating unused code.
# -fdata-sections      : Place each global variable into a separate section of the object file to allow for elimination of unused data from the final executable, as the linker can discard entire sections that are not used.
# -mabi=aapcs          : Generate code that conforms to the AAPCS ABI ensuring code is compatible with other code that uses the same ABI
#                        This can improve performance by reducing the overhead of function calls and stack management.
# -mlong-calls -- do we need this?
#-fdollars-in-identifiers ??

target_compile_definitions(nanoCLR.elf PUBLIC -DUSE_FULL_LL_DRIVER)

target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-mthumb -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -frounding-math -fsignaling-nans>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-fno-builtin -fno-common -fno-unroll-loops>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-fshort-wchar -falign-functions=16>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-ffunction-sections -fdata-sections>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-mabi=aapcs -fcheck-new>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:C>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mthumb -mcpu=cortex-m7 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -frounding-math -fsignaling-nans >)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions -fno-builtin -fno-common -fno-unroll-loops>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-fshort-wchar -falign-functions=16>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-ffunction-sections -fdata-sections>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-mabi=aapcs -fcheck-new>)
target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-Wall -Wextra -Werror>)

target_compile_options(nanoCLR.elf  PUBLIC $<$<COMPILE_LANGUAGE:ASM>:-mcpu=cortex-m7 -x assembler-with-cpp>)


#-fomit-frame-pointer  : Generate code that does not use the frame pointer resulting in smaller and faster code
#                        It can make debugging more difficult, as the stack frame information is not available

if( CMAKE_BUILD_TYPE STREQUAL "Debug")
#   target_compile_options(nanoCLR.elf PUBLIC  "-Og -ggdb")
elseif( CMAKE_BUILD_TYPE STREQUAL "Release")
#   target_compile_options(nanoCLR.elf PUBLIC  "-O3 -flto -ffat-lto-objects")
   target_compile_options(nanoCLR.elf PUBLIC  "-fomit-frame-pointer")
   
elseif( CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
 #  target_compile_options(nanoCLR.elf PUBLIC  "-Os -femit-class-debug-always -ggdb")
elseif( CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
  # target_compile_options(nanoCLR.elf PUBLIC  "-Os -flto -ffat-lto-objects")
endif()


## Link Options
target_link_options(nanoCLR.elf PUBLIC "--specs=nano.specs")
target_link_options(nanoCLR.elf PUBLIC "--specs=nosys.specs")
target_link_options(nanoCLR.elf PUBLIC "-mthumb")
target_link_options(nanoCLR.elf PUBLIC "-mcpu=cortex-m7")
target_link_options(nanoCLR.elf PUBLIC "-mfpu=fpv5-sp-d16")
target_link_options(nanoCLR.elf PUBLIC "-mfloat-abi=hard")
target_link_options(nanoCLR.elf PUBLIC "-mabi=aapcs")
target_link_options(nanoCLR.elf PUBLIC "-Wl,--gc-sections")
target_link_options(nanoCLR.elf PUBLIC "-Wl,--no-wchar-size-warning")
target_link_options(nanoCLR.elf PUBLIC "-Wl,--print-memory-usage")



