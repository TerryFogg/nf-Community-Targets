//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>

uint32_t GetExistingConfigSize()
{
    return 0;
}
void ConfigurationManager_Initialize()
{
}
void ConfigurationManager_EnumerateConfigurationBlocks()
{
}
bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    (void)configurationBlock;
    (void)configuration;
    (void)configurationIndex;
    return true;
}
bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
    (void)configurationBlock;
    (void)configuration;
    (void)configurationIndex;
    (void)blockSize;
    (void)offset;
    (void) done;
    return true;
}
UpdateConfigurationResult ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    (void)configurationBlock;
    (void)configuration;
    (void)configurationIndex;
    return UpdateConfigurationResult::UpdateConfigurationResult_Success;
    
}
void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *config, uint32_t configurationIndex)
{
}
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *config, uint32_t configurationIndex)
{
    (void)config;
    (void)configurationIndex;
    return true;
}
int32_t ConfigurationManager_FindNetworkConfigurationMatchingWirelessConfigurationFromId(uint32_t configurationId)
{
    (void)configurationId;
    return -1;
}
