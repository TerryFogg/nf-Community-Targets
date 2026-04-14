//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <memory.h>
#include <FlashDriver.h>

// Configuration region sizes
int network_config_region_size;
int wireless80211_config_region_size;
int wirelessap_config_region_size;
int x509_ca_root_bundle_region_size;
int x509_device_certificates_region_size;

uint32_t GetExistingConfigSize()
{
    uint32_t currentConfigSize =
        g_TargetConfiguration.NetworkInterfaceConfigs->Count * sizeof(HAL_Configuration_NetworkInterface) +
        g_TargetConfiguration.Wireless80211Configs->Count * sizeof(HAL_Configuration_Wireless80211);
    return currentConfigSize;
}
void ConfigurationManager_Initialize()
{
    // Configuration region sizes
    network_config_region_size = 2 * 1024;
    wireless80211_config_region_size = 2 * 1024;
    wirelessap_config_region_size = 4 * 1024;
    x509_ca_root_bundle_region_size = 16 * 1024;
    x509_device_certificates_region_size = 16 * 1024;

    // init g_TargetConfiguration
    memset(&g_TargetConfiguration, 0, sizeof(HAL_TARGET_CONFIGURATION));

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
}
bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    int sizeOfBlock = 0;
    uint8_t *blockAddress = NULL;
    bool status = false;
    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            memcpy(
                configurationBlock,
                (uint8_t *)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex],
                sizeof(HAL_Configuration_NetworkInterface));
            status = true;
            break;
        case DeviceConfigurationOption_Wireless80211Network:
            memcpy(
                configurationBlock,
                (uint8_t *)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex],
                sizeof(HAL_Configuration_Wireless80211));
            status = true;
            break;
        case DeviceConfigurationOption_X509CaRootBundle:
            memcpy(
                configurationBlock,
                (uint8_t *)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex],
                offsetof(HAL_Configuration_X509CaRootBundle, Certificate) +
                    ((HAL_Configuration_X509CaRootBundle *)blockAddress)->CertificateSize);
            status = true;
            break;

        // TODO : Do these need implementation? ( Called from Debugger.cpp with these options but not sure if we need
        // to support them )
        case DeviceConfigurationOption_X509DeviceCertificates:
            // This needs checking, code as a first guess based on the other cases
            // -------------------------------------------------------------------
            memcpy(
                configurationBlock,
                (uint8_t *)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex],
                offsetof(HAL_Configuration_X509DeviceCertificate, Certificate) +
                    ((HAL_Configuration_X509DeviceCertificate *)blockAddress)->CertificateSize);
            status = true;
            break;
        case DeviceConfigurationOption_WirelessNetworkAP:

            // This needs checking, code as a first guess based on the other cases
            // -------------------------------------------------------------------
            memcpy(
                configurationBlock,
                (uint8_t *)g_TargetConfiguration.WirelessAPConfigs->Configs[configurationIndex],
                offsetof(HAL_Configuration_WirelessAP, Authentication) +
                    ((HAL_Configuration_WirelessAP *)blockAddress)->Id);
            status = true;
            break;
        default:
            // unsupported configuration option
            status = false;
    }
    return status;
}
bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
    ByteAddress storageAddress = 0;
    bool success = false;

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
            storageAddress =
                (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex] + offset;
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));
            _ASSERTE(((HAL_Configuration_NetworkInterface *)configurationBlock)->StartupAddressMode > 0);
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            blockSize = sizeof(HAL_Configuration_Wireless80211);
            storageAddress =
                (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex] + offset;
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_WIRELESS80211_V1,
                sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));
            break;

        case DeviceConfigurationOption_X509CaRootBundle:
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate) +
                        ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;
            storageAddress =
                (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex] + offset;

            // Config block marker is required for the 1st entry in the certificate store because it's used to identify
            // if the config block is already created or not
            if (offset == 0)
            {
                memcpy(
                    configurationBlock,
                    c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                    sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));
            }
            break;

            // TODO
        case DeviceConfigurationOption_X509DeviceCertificates:
            blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate) +
                        ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;
            storageAddress =
                (ByteAddress)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex] + offset;
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));
            break;

        case DeviceConfigurationOption_WirelessNetworkAP:
            // ???????????
            blockSize = offsetof(HAL_Configuration_WirelessAP, Authentication) +
                        ((HAL_Configuration_WirelessAP *)configurationBlock)->Id;
            storageAddress = (ByteAddress)g_TargetConfiguration.WirelessAPConfigs->Configs[configurationIndex] + offset;
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_WIRELESS_AP_V1,
                sizeof(c_MARKER_CONFIGURATION_WIRELESS_AP_V1));
            break;

        case DeviceConfigurationOption_All:
            // Not supported
            break;

        default:
            // unsupported configuration option
            return false;
    }

    // Copy the config block content to the config block storage

    FlashDriver_Write(NULL, storageAddress, blockSize, (unsigned char *)configurationBlock, true);

    // If Successfully stored all the config chunks
    if (success && done)
    {
        // free the current allocation(s)
        platform_free(g_TargetConfiguration.NetworkInterfaceConfigs);
        platform_free(g_TargetConfiguration.Wireless80211Configs);
        platform_free(g_TargetConfiguration.CertificateStore);

        // Perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }
    return success;
}
void ConfigurationManager_EnumerateConfigurationBlocks()
{
    // find network configuration blocks
    HAL_CONFIGURATION_NETWORK *networkConfigs =
        (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(
            (uint32_t)&__nanoConfig_start__,
            (uint32_t)&__nanoConfig_end__);

    // check network configs count
    if (networkConfigs->Count == 0)
    {
        // there is no network config block available, create a default one and store it, then enumerate again to
        // pick it up
        HAL_Configuration_NetworkInterface *networkConfig =
            (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

        memset(networkConfig, 0, sizeof(HAL_Configuration_NetworkInterface));
        memcpy(networkConfig->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

        // currently only Wireless station is supported
        networkConfig->InterfaceType = NetworkInterfaceType_Wireless80211;
        networkConfig->StartupAddressMode = AddressMode_DHCP;
        networkConfig->AutomaticDNS = 1;
        networkConfig->SpecificConfigId = 0;

        // fill in MAX with 0xFF to allow it updating it later
        memset(networkConfig->MacAddress, 0xFF, sizeof(networkConfig->MacAddress));

        // config block created, store it
        ConfigurationManager_StoreConfigurationBlock(
            networkConfig,
            DeviceConfigurationOption_Network,
            0,
            sizeof(HAL_Configuration_NetworkInterface),
            0,
            false);

        // have to enumerate again to pick it up
        networkConfigs = (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(
            (uint32_t)&__nanoConfig_start__,
            (uint32_t)&__nanoConfig_end__);

        platform_free(networkConfig);
    }

    // find wireless 80211 network configuration blocks
    HAL_CONFIGURATION_NETWORK_WIRELESS80211 *networkWirelessConfigs =
        (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(
            (uint32_t)&__nanoConfig_start__,
            (uint32_t)&__nanoConfig_end__);

    // find X509 certificate blocks
    HAL_CONFIGURATION_X509_CERTIFICATE *certificateStore =
        (HAL_CONFIGURATION_X509_CERTIFICATE *)ConfigurationManager_FindX509CertificateConfigurationBlocks(
            (uint32_t)&__nanoConfig_start__,
            (uint32_t)&__nanoConfig_end__);

    // alloc memory for g_TargetConfiguration
    // because this is a struct of structs that use flexible members the memory has to be allocated from the heap
    // the malloc size for each struct is computed separately
    uint32_t sizeOfNetworkInterfaceConfigs =
        offsetof(HAL_CONFIGURATION_NETWORK, Configs) + networkConfigs->Count * sizeof(networkConfigs->Configs[0]);
    uint32_t sizeOfWireless80211Configs = offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) +
                                          networkWirelessConfigs->Count * sizeof(networkWirelessConfigs->Configs[0]);
    uint32_t sizeOfX509CertificateStore = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) +
                                          certificateStore->Count * sizeof(certificateStore->Certificates[0]);

    g_TargetConfiguration.NetworkInterfaceConfigs =
        (HAL_CONFIGURATION_NETWORK *)platform_malloc(sizeOfNetworkInterfaceConfigs);
    g_TargetConfiguration.Wireless80211Configs =
        (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)platform_malloc(sizeOfWireless80211Configs);
    g_TargetConfiguration.CertificateStore =
        (HAL_CONFIGURATION_X509_CERTIFICATE *)platform_malloc(sizeOfX509CertificateStore);

    // copy structs to g_TargetConfiguration
    memcpy(
        (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.NetworkInterfaceConfigs,
        networkConfigs,
        sizeOfNetworkInterfaceConfigs);
    memcpy(
        (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)g_TargetConfiguration.Wireless80211Configs,
        networkWirelessConfigs,
        sizeOfWireless80211Configs);
    memcpy(
        (HAL_CONFIGURATION_X509_CERTIFICATE *)g_TargetConfiguration.CertificateStore,
        certificateStore,
        sizeOfX509CertificateStore);

    // now free the memory of the original structs
    platform_free(networkConfigs);
    platform_free(networkWirelessConfigs);
    platform_free(certificateStore);
}
bool ConfigurationManager_CheckExistingConfigurationBlock(
    void *existingConfigBlock,
    void *newConfigBlock,
    uint32_t existingConfigBlockSize,
    uint32_t newConfigBlockSize)
{
    // config blocks parameters are addresses
    uint8_t *cursor1 = (uint8_t *)existingConfigBlock;
    uint8_t *cursor2 = (uint8_t *)newConfigBlock;

    // obvious check
    if (existingConfigBlockSize != newConfigBlockSize)
    {
        return false;
    }

    return memcmp(cursor1, cursor2, existingConfigBlockSize) == 0;
}
UpdateConfigurationResult ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    // Update the configuration block in the flash with a copy/modify/write

    ByteAddress storageAddress = NULL;
    uint32_t blockOffset;
    uint8_t *blockAddressInCopy;
    uint32_t blockSize;
    UpdateConfigurationResult status = UpdateConfigurationResult_Failed;

    switch (configuration)
    {
        case DeviceConfigurationOption_Network:
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex],
                    configurationBlock,
                    sizeof(HAL_Configuration_NetworkInterface),
                    sizeof(HAL_Configuration_NetworkInterface)))
            {
                status = UpdateConfigurationResult_NoChanges;
            }
            // Get storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];
            blockSize = sizeof(HAL_Configuration_NetworkInterface);
            _ASSERTE(((HAL_Configuration_NetworkInterface *)configurationBlock)->StartupAddressMode > 0);
            break;

        case DeviceConfigurationOption_Wireless80211Network:
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_WIRELESS80211_V1,
                sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex],
                    configurationBlock,
                    sizeof(HAL_Configuration_Wireless80211),
                    sizeof(HAL_Configuration_Wireless80211)))
            {
                status = UpdateConfigurationResult_NoChanges;
            }
            storageAddress = (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
            blockSize = sizeof(HAL_Configuration_Wireless80211);
            break;
        case DeviceConfigurationOption_X509CaRootBundle:
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));

            // Check if certificate is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.CertificateStore->Certificates[configurationIndex]->Certificate,
                    ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->Certificate,
                    g_TargetConfiguration.CertificateStore->Certificates[configurationIndex]->CertificateSize,
                    ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize))
            {
                status = UpdateConfigurationResult_NoChanges;
            }

            storageAddress = (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;
            break;

        case DeviceConfigurationOption_X509DeviceCertificates:
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));
            // check if certificate is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex]->Certificate,
                    ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->Certificate,
                    g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex]->CertificateSize,
                    ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize))
            {
                status = UpdateConfigurationResult_NoChanges;
            }
            storageAddress = (ByteAddress)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex];
            blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
            blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;
            break;
        default:
            status = UpdateConfigurationResult_Failed;
    }
    if (storageAddress != NULL)
    {
        FlashDriver_WriteConfig((unsigned char *)configurationBlock, blockSize, storageAddress);
    }
    return status;
}

void ConfigurationManager_GetOemModelSku(char *model, size_t modelSkuSize)
{
    memset(model, 0, modelSkuSize);
}

void ConfigurationManager_GetModuleSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);
}

void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    memset(serialNumber, 0, serialNumberSize);
}
