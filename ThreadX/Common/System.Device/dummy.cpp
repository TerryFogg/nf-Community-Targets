//
#include <nanoHAL_v2.h>
#include <nanoHAL_Network.h>


int SOCK_getlasterror()
{
    return 0;
}


HAL_Configuration_Wireless80211 *ConfigurationManager_GetWirelessConfigurationFromId(uint32_t configurationId)
{
    for (int i = 0; i < g_TargetConfiguration.Wireless80211Configs->Count; i++)
    {
        if (g_TargetConfiguration.Wireless80211Configs->Configs[i]->Id == configurationId)
        {
            // need to make a copy
            HAL_Configuration_Wireless80211 *configBlock =
                (HAL_Configuration_Wireless80211 *)platform_malloc(sizeof(HAL_Configuration_Wireless80211));

            // check allocation
            if (configBlock)
            {
                memcpy(
                    configBlock,
                    g_TargetConfiguration.Wireless80211Configs->Configs[i],
                    sizeof(HAL_Configuration_Wireless80211));

                return configBlock;
            }
        }
    }

    // not found
    return NULL;
}
