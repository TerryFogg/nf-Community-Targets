#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#
#Region "INCLUDES NOT USED"
 #    ${CMAKE_SOURCE_DIR}/src/PAL/Lwip
 #    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl/mbedTLS
 #    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets/ssl
 #    ${CMAKE_SOURCE_DIR}/src/PAL/COM/sockets
#Region "SOURCES NOT USED"
 #    ${CMAKE_SOURCE_DIR}/src/CLR/nanoRingBuffer/nanoRingBuffer.c
 #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp/nf_networking_sntp.cpp
 #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp/nf_networking_sntp.cpp
 #    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net/sys_net_native.cpp
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
 #    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Device.Can/nf_device_can_native.cpp
 #    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Device.OneWire/nf_dev_onewire.cpp
 #    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.IO.Hashing/nf_sys_io_hashing.cpp
 #    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Security.Cryptography/nf_sys_sec_cryptography.cpp
 #    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Security.Cryptography/nf_sys_sec_cryptography_System_Security_Cryptography_HMACSHA256.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream/sys_dev_usbstream_native.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.Device.UsbStream/sys_dev_usbstream_native_System_Device_Usb_UsbStream_stubs.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.Device.Wifi/sys_dev_wifi_native.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem_nanoFramework_System_IO_FileSystem_SDCard.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_Directory.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_File.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem/nf_sys_io_filesystem_System_IO_FileStream.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.Ports/sys_io_ser_native.cpp
 #    ${CMAKE_SOURCE_DIR}/src/System.IO.Ports/sys_io_ser_native_System_IO_Ports_SerialPort__.cpp
#Region "CORE INCLUDES"
        list(APPEND NF_INTERPRETER_INCLUDES 
                    ${CMAKE_SOURCE_DIR}/src/CLR/Include
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib
                    ${CMAKE_SOURCE_DIR}/src/CLR/Debugger
                    ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics
                    ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/Base64
                    ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/nanoprintf
                    ${CMAKE_SOURCE_DIR}/src/CLR/NativeEventDispatcher
                    ${CMAKE_SOURCE_DIR}/src/CLR/Startup
                    ${CMAKE_SOURCE_DIR}/src/CLR/System.Math
                    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Network/Enc28j60
                    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/Networking.Sntp
                    ${CMAKE_SOURCE_DIR}/src/DeviceInterfaces/System.Net
                    ${CMAKE_SOURCE_DIR}/src/HAL/Include
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Devices.Can
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Devices.OneWire
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.ResourceManager
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Text
                    ${CMAKE_SOURCE_DIR}/src/PAL/Include
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Events
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Adc
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Dac
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Gpio
                    ${CMAKE_SOURCE_DIR}/src/System.Device.I2c
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Pwm
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Spi
                    ${CMAKE_SOURCE_DIR}/src/System.IO.Ports
                    ${CMAKE_SOURCE_DIR}/src/System.IO.FileSystem
                    ${CMAKE_SOURCE_DIR}/src/System.Net
                    ${CMAKE_SOURCE_DIR}/src/Windows.Storage)
#Region "CORE SOURCES"
        list(APPEND NF_INTERPRETER_SOURCES
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Cache.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Checks.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_DblLinkedList.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Array.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_ArrayList.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_BinaryBlob.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Delegate.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Delegate_List.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Finalizer.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Lock.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_LockRequest.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Node.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Queue.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Stack.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_String.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_Timer.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapBlock_WaitForObject.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_HeapCluster.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_Interop.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_Memory.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_ObjectToEvent_Destination.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_ObjectToEvent_Source.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_RuntimeMemory.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_StackFrame.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_SystemAssembliesTable.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/CLR_RT_UnicodeHelper.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Core.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Execution.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/GarbageCollector.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/GarbageCollector_Compaction.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/GarbageCollector_ComputeReachabilityGraph.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/GarbageCollector_Info.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Interpreter.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/nanoSupport_CRC32.c
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Random.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Streams.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/StringTable.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/StringTableData.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Thread.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/TypeSystem.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/TypeSystemLookup.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Various.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Hardware/Hardware.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Hardware/Hardware_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/InterruptHandler/InterruptHandler.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/InterruptHandler/InterruptHandler_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/NativeEventDispatcher/NativeEventDispatcher.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/NativeEventDispatcher/NativeEventDispatcher_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/RPC/CLR_RT_HeapBlock_EndPoint.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/RPC/RPC_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Serialization/BinaryFormatter.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Core/Serialization/BinaryFormatter_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_AppDomain.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Array.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Attribute.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_BitConverter.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Collections_ArrayList.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Convert.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_DateTime.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Delegate.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Diagnostics_Debug.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Diagnostics_Debugger.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Double.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Enum.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Exception.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_GC.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Globalization_CultureInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Globalization_DateTimeFormat.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Guid.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_MarshalByRefObject.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_MathInternal.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_MulticastDelegate.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Number.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Object.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Random.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_Assembly.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_Binder.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_ConstructorInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_FieldInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_MemberInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_MethodBase.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_PropertyInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_RuntimeFieldInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Reflection_RuntimeMethodInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_RuntimeType.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Runtime_CompilerServices_RuntimeHelpers.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Runtime_Remoting_RemotingServices.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_String.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_AutoResetEvent.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_Interlocked.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_ManualResetEvent.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_Monitor.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_SpinWait.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_Thread.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_Timer.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Threading_WaitHandle.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_TimeSpan.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_Type.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_ValueType.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/CorLib/corlib_native_System_WeakReference.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Debugger/Debugger.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Debugger/Debugger_full.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Debugger/Debugger_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics/Diagnostics_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics/Info.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics/Info_Safeprintf.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics/Profile.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Diagnostics/Profiler.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/Base64/base64.c
                    ${CMAKE_SOURCE_DIR}/src/CLR/Helpers/nanoprintf/nanoprintf.c
                    ${CMAKE_SOURCE_DIR}/src/CLR/Messaging/Messaging.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Messaging/Messaging_stub.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/Startup/CLRStartup.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/System.Math/nf_native_system_math.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/System.Math/nf_native_system_math_System_Math.cpp
                    ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol/WireProtocol_App_Interface.c
                    ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol/WireProtocol_HAL_Interface.c
                    ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol/WireProtocol_Message.c
                    ${CMAKE_SOURCE_DIR}/src/CLR/WireProtocol/WireProtocol_MonitorCommands.c
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Boot.c
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Capabilites.c
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_ConfigurationManager.c
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_SystemEvents.c
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_SystemInformation.cpp
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Time.cpp
                    ${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Watchdog.c
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.ResourceManager/nf_system_resourcemanager.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.ResourceManager/nf_system_resourcemanager_nanoFramework_Runtime_Native_ResourceUtility.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.ResourceManager/nf_system_resourcemanager_System_Resources_ResourceManager.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Events/nf_rt_events_native.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_EventSink.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_NativeEventDispatcher.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Events/nf_rt_events_native_nanoFramework_Runtime_Events_WeakDelegate.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Hardware_SystemInfo.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_ExecutionConstraint.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_GC.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Power.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native_nanoFramework_Runtime_Native_Rtc_stubs.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Runtime.Native/nf_rt_native_System_Environment.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections/nf_system_collections.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Hashtable.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Hashtable__HashtableEnumerator.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Queue.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Collections/nf_system_collections_System_Collections_Stack.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Text/nf_system_text.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Text/nf_system_text_System_Text_UTF8Decoder.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.System.Text/nf_system_text_System_Text_UTF8Encoding.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall/AsyncCompletions.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall/AsyncContinuations.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/AsyncProcCall/Async_stubs.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/BlockStorage/nanoPAL_BlockStorage.c
                    ${CMAKE_SOURCE_DIR}/src/PAL/COM/COM_stubs.c
                    ${CMAKE_SOURCE_DIR}/src/PAL/COM/GenericPort_stdio.c
                    ${CMAKE_SOURCE_DIR}/src/PAL/Double/nanoPAL_NativeDouble.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/Events/nanoPAL_Events.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/Events/nanoPAL_Events_driver.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/Events/nanoPAL_Events_functions.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/nanoPAL_Network_stubs.cpp
                    ${CMAKE_SOURCE_DIR}/src/PAL/Profiler/nanoPAL_PerformanceCounters_stubs.cpp
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Adc/sys_dev_adc_native.cpp
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Dac/sys_dev_dac_native.cpp
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Gpio/sys_dev_gpio_native.cpp
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Gpio/sys_dev_gpio_native_System_Device_Gpio_GpioController.cpp
                    ${CMAKE_SOURCE_DIR}/src/System.Device.Gpio/sys_dev_gpio_native_System_Device_Gpio_GpioPin.cpp
                    ${CMAKE_SOURCE_DIR}/src/System.Device.I2c/sys_dev_i2c_native.cpp
        #            ${CMAKE_SOURCE_DIR}/src/System.Device.I2c/sys_dev_i2c_slave_native.cpp
        #            ${CMAKE_SOURCE_DIR}/src/System.Device.I2s/sys_dev_i2s_slave_native.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Device.Pwm/sys_dev_pwm_native.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Device.Spi/nanoHAL_Spi.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Device.Spi/sys_dev_spi_native.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Device.Spi/sys_dev_spi_native_System_Device_Spi_SpiBusInfo.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Device.Spi/sys_dev_spi_native_System_Device_Spi_SpiDevice.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Runtime.Serialization/nf_system_runtime_serialization.cpp
                     ${CMAKE_SOURCE_DIR}/src/System.Runtime.Serialization/nf_system_runtime_serialization_System_Runtime_Serialization_Formatters_Binary_BinaryFormatter.cpp
                     ${CMAKE_SOURCE_DIR}/src/Windows.Storage/win_storage_native.cpp
                     ${CMAKE_SOURCE_DIR}/src/Windows.Storage/win_storage_native_Windows_Storage_Devices_SDCard_stubs.cpp
                     ${CMAKE_SOURCE_DIR}/src/Windows.Storage/win_storage_native_Windows_Storage_FileIO_stubs.cpp
                     ${CMAKE_SOURCE_DIR}/src/Windows.Storage/win_storage_native_Windows_Storage_StorageFile_stubs.cpp
                     ${CMAKE_SOURCE_DIR}/src/Windows.Storage/win_storage_native_Windows_Storage_StorageFolder_stubs.cpp
        )
#Region "COMMON TARGET INCLUDES"
        list(APPEND NF_INTERPRETER_INCLUDES 
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/${TARGET_BOARD}
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics																																																																																																																																																																																							
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other																																																																																																																																																																																							
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/RequiredNotUsed
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Startup																																																																																																																																																																																							
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/ThreadXAdaption
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/WireProtocol
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO																																																																																																																																																																																							
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/WireProtocol)
#Region "COMMON TARGET SOURCES"
        list(APPEND NF_INTERPRETER_SOURCES 
            
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other/Memory.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other/nanoCRT.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other/sys_calls.c
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other/targetHAL_ConfigurationManager.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other/time_utilities.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Other/wp_CircularBuffer.c

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Startup/Thread_CLRStartup.c
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/Startup/Thread_WireProtocolReceiver.c

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/ThreadXAdaption/tx_adaption.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/ThreadXAdaption/tx_adaption_threads.c

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/WireProtocol/WireProtocol_HAL_Interface.c

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/main.c
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/hard_fault_handler.c
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/targetHAL.cpp

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.Adc.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.Dac.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.Gpio.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.I2c.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.Pwm.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.Device.SPI.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/DeviceIO/cpu_System.IO.Ports.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Commonality/${TARGET_FAMILY}/WireProtocol/wp_Communications.c)  
#Region "COMMON GRAPHICS INCLUDES"
    if(GRAPHICS_SUPPORT)
        list(APPEND NF_INTERPRETER_INCLUDES 
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Gif
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Support/Jpeg
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Displays
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Devices)
    endif()
#Region "COMMON GRAPHICS SOURCES"
    if(GRAPHICS_SUPPORT)
        list(APPEND NF_INTERPRETER_SOURCES
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Core/Graphics.cpp>
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
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core/Ink.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core/Gestures.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Core/TouchPanel.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/TouchPanel/Devices/ft6x06_I2C.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchCollectorConfiguration.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchEventProcessor.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_TouchPanel.cpp
                    ${CMAKE_SOURCE_DIR}/src/nanoFramework.Graphics/Graphics/Native/nanoFramework_Graphics_nanoFramework_UI_Ink.cpp

                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/Graphics_Memory.cpp
                    ${CMAKE_SOURCE_DIR}/targets-community/ThreadX/Common/nanoFramework.Graphics/${DISPLAY_CONTROLLER}
        )
        endif()

 set_source_files_properties(${CMAKE_SOURCE_DIR}/src/HAL/nanoHAL_Boot.c COMPILE_FLAGS -Wno-unused-parameter)
