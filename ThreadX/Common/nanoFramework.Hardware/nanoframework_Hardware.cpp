#include "nanoframework_Hardware.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nanoframework_Hardware_nanoframework_Hardware_NativeFunctions::SetPinFunction___STATIC__BOOLEAN__I4__I4,
    Library_nanoframework_Hardware_nanoframework_Hardware_NativeFunctions::SetPinDriveStrength___STATIC__BOOLEAN__I4__nanoframeworkHardwareNominalDriveStrength,
    Library_nanoframework_Hardware_nanoframework_Hardware_NativeFunctions::SetPinSchmidtTrigger___STATIC__BOOLEAN__I4__nanoframeworkHardwareSchmittTrigger,
    Library_nanoframework_Hardware_nanoframework_Hardware_NativeFunctions::SetPinSlewRate___STATIC__BOOLEAN__I4__nanoframeworkHardwareSlewRateLimiting,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoframework_Hardware =
{
    "nanoframework.Hardware",
    0xBAF2335A,
    method_lookup,
    { 100, 0, 0, 0 }
};

// clang-format on
