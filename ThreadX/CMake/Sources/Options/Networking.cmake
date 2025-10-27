#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
# Locals
 list(APPEND NETWORK_INCLUDES
    ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi
    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net
    ${CMAKE_SOURCE_DIR}/src/boards/includes
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/RaspberryPi/RP2XXX/Common/nx_driver
    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
)
 list(APPEND NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native.cpp
    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/ssl.cpp

    # Make local copies of these (next section) due to official source reliance on sockets and lwip
    # ------------------------------------------------------------------------------
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_IPAddress.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_IPGlobalProperties.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_NetworkInterface.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_CertificateManager.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Security_SslNative.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Net_Sockets_NativeSocket.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate.cpp
    #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2.cpp
)

 list(APPEND NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Driver.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Helpers.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Information.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Interface.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.NetXDuo.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Security.SslNative.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Sockets.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.WirelessAPConfiguration.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Network.Wireless80211Configuration.cpp
    
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/ssl_ISM43362.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/dummy.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/ThreadXAdaption/Threads/NetworkThread.cpp
)

 list(APPEND NETWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi/sys_dev_wifi_native.cpp
    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/System.Device.Network/System.Device.Wifi.Adapter.cpp
)

list(APPEND OPTIONAL_SOURCES
    ${NETWORK_SOURCES}
)

list(APPEND OPTIONAL_INCLUDES
    ${NETWORK_INCLUDES}
)
