// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "NativeRpiPico.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_NativeRpiPico_nanoFramework_Hardware_RpiPico_Configuration::SetPinFunction___STATIC__VOID__I4__I4,
    Library_NativeRpiPico_nanoFramework_Hardware_RpiPico_Configuration::GetPinFunction___STATIC__I4__I4,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_RaspberryPiPico =
{
    "nanoFramework.Hardware.RaspberryPiPico",
    0x229C194B,
    method_lookup,
    { 100, 0, 8, 0 }
};

// clang-format on
