#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifdef __cplusplus
extern "C"
{
#endif

    // the targets of this platform can declate these capabilities
    typedef enum TargetCapabilities
    {
        // JTAG update capable
        // using Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_TargetCapability_0
        TargetCapabilities_JtagUpdate = 0x10000000,

        // DFU update capable
        // using Debugging_Execution_QueryCLRCapabilities::c_CapabilityFlags_TargetCapability_1
        TargetCapabilities_DfuUpdate = 0x20000000,

    } TargetCapabilities;

#ifdef __cplusplus
}
#endif

