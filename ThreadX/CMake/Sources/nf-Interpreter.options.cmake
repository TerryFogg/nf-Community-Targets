#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# Graphics
    if(GRAPHICS_SUPPORT)
        list(APPEND OPTIONAL_INCLUDES 
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Displays
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native
        )
        list(APPEND OPTIONAL_SOURCES
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Graphics.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/GraphicsDriver.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/GraphicsMemoryHeap.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Bmp/Bitmap_Decoder.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Fonts/Font.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif/Gif.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif/GifDecoder.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif/lzwread.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jbytearraydatasrc.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcapimin.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcapistd.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jccoefct.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jccolor.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcdctmgr.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jchuff.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jchuff.h
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcinit.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcmainct.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcmarker.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcmaster.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcomapi.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jconfig.h
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcparam.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcphuff.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcprepct.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jcsample.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jctrans.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdapimin.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdapistd.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdcoefct.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdcolor.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jddctmgr.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdhuff.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdinput.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmainct.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmarker.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmaster.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdmerge.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdphuff.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdpostct.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdsample.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jdtrans.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jerror.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jfdctflt.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jfdctfst.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jfdctint.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctflt.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctfst.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctint.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jidctred.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jmemmgr.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jmemnanoclr.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/Jpeg.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jquant1.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jquant2.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/jutils.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/mcbcr.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/mfint.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/miint.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/pfint.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/piint.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg/transupp.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics.h
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_Bitmap.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_DisplayControl.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_Font.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Graphics_Memory.cpp
        )
     endif()

# Touch
     if(TOUCH_DISPLAY_SUPPORT)
        list(APPEND OPTIONAL_INCLUDES
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Devices
        )
        list(APPEND OPTIONAL_SOURCES
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchCollectorConfiguration.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchPanel.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_Ink.cpp

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Ink.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Gestures.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/TouchPanel.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/TouchDevice.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/${TOUCH_INTERFACE}
        )
     endif()
     if(GRAPHICS_SUPPORT AND NOT TOUCH_DISPLAY_SUPPORT)
        # If the Graphics option is selected there are included references to touch that have to be satisfied, although they are not required.
        # 
        list(APPEND OPTIONAL_SOURCES
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/TouchStubs.cpp
        )
     endif()

# File System
    if(FILEX_SYSTEM_SD OR FILEX_SYSTEM_NAND OR FILEX_SYSTEM_NOR OR FILEX_SYSTEM_RAM)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/FileX.cmake)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/LevelX.cmake)
        list(APPEND OPTIONAL_INCLUDES
             ${FILEX_INCLUDES}
             ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem
        )
        list(APPEND OPTIONAL_SOURCES
             ${FILEX_SOURCES}
             ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem.cpp
             
             # Can't use the master copies as they have references to "ff.h"
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_Directory.cpp
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_File.cpp
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_FileStream.cpp
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard.cpp
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/FileSystem.cpp
        )
    endif()
    if(FILEX_SYSTEM_SD)
        list(APPEND OPTIONAL_SOURCES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Sd_driver.c
        )
    endif()
    if(FILEX_SYSTEM_NAND)
        list(APPEND OPTIONAL_SOURCES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Nand_driver.c
        )
    endif()
    if(FILEX_SYSTEM_NOR)
        list(APPEND OPTIONAL_SOURCES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Nor_driver.c
        )
    endif()
    if(FILEX_SYSTEM_RAM)
        list(APPEND OPTIONAL_SOURCES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.IO.FileSystem/Ram_driver.c
        )
    endif()

    # Per Board fileX sources    
    if( "${TARGET_FAMILY}" STREQUAL "STM32H7")
        list(APPEND OPTIONAL_INCLUDES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32H7/DeviceIO
        )
        list(APPEND OPTIONAL_SOURCES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32H7/DeviceIO/SD_DoubleBufferedDMA.cpp
        )
    endif()
    if( "${TARGET_FAMILY}" STREQUAL "STM32U5")
        list(APPEND OPTIONAL_INCLUDES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32H7/DeviceIO
        )
        list(APPEND OPTIONAL_SOURCES
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/STM32H7/DeviceIO/SD_DoubleBufferedDMA.cpp
        )
    endif()
    if( "${TARGET_FAMILY}" STREQUAL "RP2040")
        list(APPEND OPTIONAL_INCLUDES
        )
        list(APPEND OPTIONAL_SOURCES
        )
    endif()

# Usb
    if(USB_DEVICE OR USB_HOST OR USB_HOST_CONTROLLER OR USB_NETWORK OR USB_PICTBRIDGE)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/UsbX.cmake)

        list(APPEND OPTIONAL_INCLUDES
             ${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream
             ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device
            ${USBX_INCLUDES}
        )
        list(APPEND OPTIONAL_SOURCES
              ${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream/sys_dev_usbstream_native.cpp
             #${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream/sys_dev_usbstream_native_System_Device_Usb_UsbStream_stubs.cpp
              ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device/System.Device.UsbStream.cpp
             ${USBX_SOURCES}
        )
    endif()

# Networking
    if(NETWORKING)
        include( ${CMAKE_CURRENT_SOURCE_DIR}/Sources/NetXDuo.cmake)

        list(APPEND OPTIONAL_INCLUDES
            ${NETXDUO_INCLUDES}
     )
        list(APPEND OPTIONAL_SOURCES
            ${NETXDUO_SOURCES}
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

