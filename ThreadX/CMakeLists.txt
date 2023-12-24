#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)

include( ${CMAKE_CURRENT_SOURCE_DIR}/CMake/nf-Interpreter.cmake)
include( ${CMAKE_CURRENT_SOURCE_DIR}/CMake/ThreadX.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/${TARGET_FAMILY}.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/${TARGET_BOARD}/board.cmake)

add_executable(nanoCLR.elf ${NF_INTERPRETER_COMMON_SOURCES}
                           ${NF_INTERPRETER_TARGET_COMMON_SOURCES}
                           ${NF_INTERPRETER_TARGET_SOURCES}
                           ${THREADX_SOURCES}
                           ${${TARGET_FAMILY}_SOURCES}
)

target_include_directories(nanoCLR.elf PUBLIC ${NF_INTERPRETER_COMMON_INCLUDES}
                                              ${NF_INTERPRETER_TARGET_COMMON_INCLUDES}
                                              ${THREADX_INCLUDES}
                                              ${${TARGET_FAMILY}_INCLUDES}
)

if(GRAPHICS_SUPPORT)
      target_sources(nanoCLR.elf PUBLIC "${NF_INTERPRETER_GRAPHICS_SOURCES}")
      target_include_directories(nanoCLR.elf PUBLIC "${NF_INTERPRETER_GRAPHICS_INCLUDES}")
      #target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=TRUE)
else()
      #target_compile_definitions(nanoCLR.elf PUBLIC -DNANOCLR_GRAPHICS=FALSE)
endif()

include( CMake/CommonDefinitions.cmake)
include( CMake/BuildParameters_${TARGET_FAMILY}.cmake)
target_link_options(nanoCLR.elf PUBLIC "-T${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}/Linker_script.ld")

add_custom_command(TARGET nanoCLR.elf POST_BUILD
      COMMAND ${CMAKE_OBJCOPY}           $<TARGET_FILE:nanoCLR.elf>   ${CMAKE_BINARY_DIR}/nanoCLR.elf
      COMMAND ${CMAKE_OBJCOPY} -Oihex    $<TARGET_FILE:nanoCLR.elf>   nanoCLR.hex
      COMMAND ${CMAKE_OBJCOPY} -Obinary  $<TARGET_FILE:nanoCLR.elf>   nanoCLR.bin
      COMMAND ${CMAKE_OBJDUMP} -d -EL -S $<TARGET_FILE:nanoCLR.elf> > nanoCLR.lst
)
