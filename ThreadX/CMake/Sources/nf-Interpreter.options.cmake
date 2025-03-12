#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Graphics
 if(GRAPHICS_SUPPORT)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/Graphics.cmake)
 endif()

# Touch Display
if(TOUCH_DISPLAY_SUPPORT)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/TouchDisplay.cmake)
endif()

if(GRAPHICS_SUPPORT AND NOT TOUCH_DISPLAY_SUPPORT)
    # If the Graphics option is selected there are included references to touch that have to be satisfied, although they are not required.
    list(APPEND GRAPHICS_SOURCES ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/TouchStubs.cpp )
endif()

# File System
if(FILEX_SYSTEM_SD OR FILEX_SYSTEM_NAND OR FILEX_SYSTEM_NOR OR FILEX_SYSTEM_RAM)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/FileX.cmake)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/LevelX.cmake)
    list(APPEND FILEX_INCLUDES
         ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem
    )
    list(APPEND FILEX_SOURCES
         ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem.cpp
         
         # Can't use the master copies as they have references to "ff.h"
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard.cpp

         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_Directory.cpp
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_File.cpp
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_FileStream.cpp
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/FileSystem.cpp
    )
endif()
if(FILEX_SYSTEM_SD)
    list(APPEND FILEX_SOURCES
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Sd_driver.c
    )
endif()
if(FILEX_SYSTEM_NAND)
    list(APPEND FILEX_SOURCES
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Nand_driver.c
    )
endif()
if(FILEX_SYSTEM_NOR)
    list(APPEND FILEX_SOURCES
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Nor_driver.c
    )
endif()
if(FILEX_SYSTEM_RAM)
    list(APPEND FILEX_SOURCES
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Ram_driver.c
    )
endif()



# Per Board fileX sources    
if( "${TARGET_FAMILY}" STREQUAL "STM32H7")
    list(APPEND FILEX_SOURCES
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_VENDOR}/${TARGET_FAMILY}/Common/SD_DoubleBufferedDMA.cpp
    )
endif()
if( "${TARGET_FAMILY}" STREQUAL "STM32U5")
    list(APPEND FILEX_INCLUDES
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_VENDOR}/${TARGET_FAMILY}/Common/STM32U5/DeviceIO
    )
endif()

# Usb
if(USB_DEVICE OR USB_HOST OR USB_HOST_CONTROLLER OR USB_NETWORK OR USB_PICTBRIDGE)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/UsbX.cmake)

    list(APPEND USB_INCLUDES
         ${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream
         ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device
        ${USBX_INCLUDES}
    )
    list(APPEND USB_SOURCES
          ${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream/sys_dev_usbstream_native.cpp
         #${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream/sys_dev_usbstream_native_System_Device_Usb_UsbStream_stubs.cpp
          ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device/System.Device.UsbStream.cpp
         ${USBX_SOURCES}
    )
endif()

# Networking
if(NETWORKING)
    include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/NetXDuo.cmake)
    list(APPEND NETXDUO_SOURCES
        ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi/sys_dev_wifi_native.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp/nf_networking_sntp.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp/nf_networking_sntp.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_IPAddress.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_IPGlobalProperties.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_NetworkInterface.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_CertificateManager.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_SslNative.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Sockets_NativeSocket.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate.cpp
        ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2.cpp
    )
endif()

list(APPEND OPTIONAL_INCLUDES
     ${GRAPHICS_INCLUDES}
     ${TOUCH_INCLUDES}
     ${FILEX_INCLUDES}
     ${LEVELX_INCLUDES}
     ${USB_INCLUDES}
     ${NETXDUO_INCLUDES} )

list(APPEND OPTIONAL_SOURCES
     ${GRAPHICS_SOURCES}
     ${TOUCH_SOURCES}
     ${FILEX_SOURCES}
     ${LEVELX_SOURCES}
     ${USB_SOURCES}
     ${NETXDUO_SOURCES} )
