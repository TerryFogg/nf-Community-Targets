//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoCLR_Types.h"
#include "CPU_SPI_DEcl.h"


// Initialise an SPI bus, called before any devices opened (optional)
bool CPU_SPI_Initialize(uint8_t bus, const SPI_DEVICE_CONFIGURATION &spiDeviceConfig)
{
    (void)bus;
    (void)spiDeviceConfig;
    return true;
}

// Unintialise spi bus, called after last device removed (optional)
bool CPU_SPI_Uninitialize(uint8_t bus)
{
    (void)bus;
    return true;
}

// Return status of current SPI operation
// Used to find status of an Async SPI call
SPI_OP_STATUS CPU_SPI_OP_Status(CLR_UINT8 busIndex, CLR_UINT32 deviceHandle)
{
    (void)busIndex;
    (void)deviceHandle;
    return SPI_OP_READY;
}

// Add a device to SPi Bus (Optional)
// Returns a device handle in the handle pointer.
// deviceHandle is a reference to underlying OS handle/address of device. If not required then return value other than
// 0. returns S_OK on success, else S_FALSE on failure.
HRESULT CPU_SPI_Add_Device(const SPI_DEVICE_CONFIGURATION &spiDeviceConfig, uint32_t &handle)
{
    (void)spiDeviceConfig;
    (void)handle;
    return 0;
}

// Remove device from bus (Optional)
// return true is successful
bool CPU_SPI_Remove_Device(uint32_t deviceHandle)
{
    (void)deviceHandle;
    return true;
}

// Write / read 8 bit data to device specified by handle
// return result 0=S_OK, CLR_E_BUSY async operation and operation still running or another error code
HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint8_t *writePtr,
    int32_t writeSize,
    uint8_t *readPtr,
    int32_t readSize)
{
    (void)deviceHandle;
    (void)sdev;
    (void)swrs;
    (void)writePtr;
    (void)writeSize;
    (void)readPtr;
    (void)readSize;
    return 0;
}

void CPU_SPI_Wait_Busy(uint32_t deviceHandle, SPI_DEVICE_CONFIGURATION &sdev)
{
    (void)deviceHandle;
    (void)sdev;
}

// Write / read 16 bit data to device specified by handle
// return result 0=S_OK, CLR_E_BUSY async operation and operation still running or another error code
HRESULT CPU_SPI_nWrite16_nRead16(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint16_t *writePtr,
    int32_t writeSize,
    uint16_t *readPtr,
    int32_t readSize)
{
    (void)deviceHandle;
    (void)sdev;
    (void)swrs;
    (void)writePtr;
    (void)writeSize;
    (void)readPtr;
    (void)readSize;
    return 0;
}

// Return bit map of available SPI buses
// bit 0 - SPI1
// bit 1 - SPI2
// bit 2 - SPI3
// etc
uint32_t CPU_SPI_PortsMap()
{
    return 0;
}

// Total number of SPI buses available
uint32_t CPU_SPI_PortsCount()
{
    return 0;
}

// Return pins used for SPI bus
// return -1 if not known
void CPU_SPI_GetPins(uint32_t busIndex, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    (void)busIndex;
    (void)clk;
    (void)miso;
    (void)mosi;
}

// Minimum and Maximum clock frequency available based on bus and configured pins
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t busIndex)
{
    (void)busIndex;
    return 0;
}
HRESULT CPU_SPI_MinClockFrequency(uint32_t busIndex, int32_t *frequency)
{
    (void)busIndex;
    (void)frequency;
    return 0;
}
HRESULT CPU_SPI_MaxClockFrequency(uint32_t busIndex, int32_t *frequency)
{
    (void)busIndex;
    (void)frequency;
    return 0;
}
