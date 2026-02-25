#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# Locals
 list(APPEND NETWORK_INCLUDES
    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net
    ${CMAKE_SOURCE_DIR}/src/boards/includes
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/Common/nx_driver
    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
)
 list(APPEND NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native.cpp
    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_CertificateManager.cpp
    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/ssl.cpp

    # Make local copies of these (next section) due to official source reliance on sockets and lwip
    # ------------------------------------------------------------------------------
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_IPAddress.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_IPGlobalProperties.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_NetworkInterface.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_SslNative.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Sockets_NativeSocket.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2.cpp
)

 list(APPEND NETWORK_SOURCES

    # Included by specific drivers
        #${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Device/nx_driver_framework.c
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Device/Network.Helpers.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Device/NetXDuo.cpp

    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Network.Information.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Network.Interface.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Network.Security.SslNative.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Network.Sockets.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Network.WirelessAPConfiguration.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Network.Wireless80211Configuration.cpp
    
)

 list(APPEND NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi/sys_dev_wifi_native.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Library/Wifi.Adapter.cpp
)

list(APPEND OPTIONAL_SOURCES
    ${NETWORK_SOURCES}
)

list(APPEND OPTIONAL_INCLUDES
    ${NETWORK_INCLUDES}
)
