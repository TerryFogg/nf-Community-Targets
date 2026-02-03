# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.


# The project command must come later for the RP2XXX series build system
if( NOT ${TARGET_FAMILY} STREQUAL "RP2XXX")
  project(nanoFramework VERSION ${BUILD_VERSION} LANGUAGES C CXX ASM)
endif()

add_executable(nanoCLR)

# Sources
   include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/DeviceFamilies/${TARGET_FAMILY}.cmake)
   include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/nf-Interpreter.cmake)
   include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/ThreadX.cmake)
   include(../${PARENT_DIRECTORY}/${TARGET_VENDOR}/${TARGET_FAMILY}/${TARGET_SERIES}/board.cmake)

# Optional Sources
    if(GRAPHICS_SUPPORT)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Options/Graphics.cmake)
     endif()
    if(NETWORKING_SUPPORT)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Options/netXDuo.cmake)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Options/Networking.cmake)
    endif()
    if(BLUETOOTH_SUPPORT)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Options/Bluetooth.cmake)
    endif()
    if(FILE_SYSTEM_SUPPORT)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Options/FileSystem.cmake)
    endif()
    if(USB_SUPPORT)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Options/Usb.cmake)
    endif()

    target_sources(nanoCLR PUBLIC
                   ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/main.c
                   ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/common/CLR_RT_InteropAssembliesTable.cpp
                   "../targetHAL.cpp"
                   ${NF_INTERPRETER_SOURCES}
                   ${THREADX_SOURCES}
                   ${${TARGET_FAMILY}_SOURCES}
                   ${OPTIONAL_SOURCES}
    )
    target_include_directories(nanoCLR PUBLIC
                               ${NF_INTERPRETER_INCLUDES}
                               ${THREADX_INCLUDES}
                               ${${TARGET_FAMILY}_INCLUDES}
                               ${OPTIONAL_INCLUDES}
    )
# Build parameters
    include( BuildParameters/Common.cmake)
    include( BuildParameters/DeviceFamilies/${TARGET_FAMILY}.cmake)
