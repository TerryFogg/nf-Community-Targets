#include <nanoHAL.h>
#include <nanoHAL_v2.h>

bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    (void)configurationBlock;
    (void)configuration;
    (void)configurationIndex;

    return true;
};

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
    (void)done;
    return true;
}
