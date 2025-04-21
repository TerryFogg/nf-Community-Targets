//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <Core.h>

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native;

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_Ports;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Math;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Runtime_Serialization;

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Collections;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Text;

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Adc;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Dac;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Gpio;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c_Slave;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2s;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Pwm;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Spi;

#ifdef CAN_SUPPORT
     extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Can;
#endif
#ifdef ONE_WIRE
     extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_OneWire;
#endif
#ifdef IO_HASHING
     extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_IO_Hashing;
#endif
#ifdef CRYPTOGRAPHY
     extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Security_Cryptography;
#endif
#ifdef FILE_SUPPORT
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_FileSystem;
#endif
#ifdef NANOCLR_GRAPHICS 
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Graphics;
#endif
#ifdef NETWORK_SUPPORT
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Net;
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Wifi;
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Sntp;
#endif
#ifdef USB_SUPPORT
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_UsbStream;
#endif
#ifdef NETWORK_THREAD_PROTOCOL
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Thread;
#endif
#ifdef BLUETOOTH
    extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Bluetooth,
#endif

const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[] = {
    &g_CLR_AssemblyNative_mscorlib,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Native,

    &g_CLR_AssemblyNative_System_IO_Ports,
    &g_CLR_AssemblyNative_System_Math,
    &g_CLR_AssemblyNative_System_Runtime_Serialization,
    &g_CLR_AssemblyNative_nanoFramework_ResourceManager,
    &g_CLR_AssemblyNative_nanoFramework_System_Collections,
    &g_CLR_AssemblyNative_nanoFramework_System_Text,

    &g_CLR_AssemblyNative_System_Device_Adc,
    &g_CLR_AssemblyNative_System_Device_Dac,
    &g_CLR_AssemblyNative_System_Device_Gpio,
    &g_CLR_AssemblyNative_System_Device_I2c_Slave,
    &g_CLR_AssemblyNative_System_Device_I2c,
    &g_CLR_AssemblyNative_System_Device_I2s,
    &g_CLR_AssemblyNative_System_Device_Pwm,
    &g_CLR_AssemblyNative_System_Device_Spi,

#ifdef CAN_SUPPORT
    &g_CLR_AssemblyNative_nanoFramework_Device_Can,
#endif
#ifdef ONE_WIRE
    &g_CLR_AssemblyNative_nanoFramework_Device_OneWire,
#endif
#ifdef IO_HASHING
    &g_CLR_AssemblyNative_nanoFramework_System_IO_Hashing,
#endif
#ifdef CRYPTOGRAPHY
        &g_CLR_AssemblyNative_nanoFramework_System_Security_Cryptography,
#endif
#ifdef FILE_SUPPORT
    &g_CLR_AssemblyNative_System_IO_FileSystem,
#endif
#if (NANOCLR_GRAPHICS == TRUE)
    &g_CLR_AssemblyNative_nanoFramework_Graphics,
#endif
#ifdef NETWORK_SUPPORT
    &g_CLR_AssemblyNative_System_Net,
    &g_CLR_AssemblyNative_System_Device_Wifi,
    &g_CLR_AssemblyNative_nanoFramework_Networking_Sntp,
#endif
#ifdef USB_SUPPORT
    &g_CLR_AssemblyNative_System_Device_UsbStream,
#endif
#ifdef NETWORK_THREAD_PROTOCOL
    &g_CLR_AssemblyNative_nanoFramework_Networking_Thread,
#endif
#ifdef BLUETOOTH
        &g_CLR_AssemblyNative_nanoFramework_Device_Bluetooth,
#endif
    NULL};

const uint16_t g_CLR_InteropAssembliesCount = sizeof(g_CLR_InteropAssembliesNativeData) / sizeof(g_CLR_InteropAssembliesNativeData[0]) - 1;
