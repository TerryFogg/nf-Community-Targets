//-----------------------------------------------------------------------------
//
//                   ** WARNING! ** 
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#ifndef NANOFRAMEWORK_HARDWARE_H
#define NANOFRAMEWORK_HARDWARE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack AdcMode
{
    AdcMode_SingleSampling = 0,
    AdcMode_RoundRobinSampling = 1,
} AdcMode;

typedef enum __nfpack NominalDriveStrength
{
    NominalDriveStrength_Strength2ma = 0,
    NominalDriveStrength_Strength4ma = 1,
    NominalDriveStrength_Strength8ma = 2,
    NominalDriveStrength_Strength12ma = 3,
    NominalDriveStrength_StrengthDefault = 1,
} NominalDriveStrength;

typedef enum __nfpack SchmittTrigger
{
    SchmittTrigger_SchmittEnable = 1,
    SchmittTrigger_SchmittDisable = 2,
    SchmittTrigger_ScmittDefault = 1,
} SchmittTrigger;

typedef enum __nfpack SlewRateLimiting
{
    SlewRateLimiting_SlewSlow = 0,
    SlewRateLimiting_SlewFast = 1,
    SlewRateLimiting_SlewDefault = 0,
} SlewRateLimiting;

typedef enum __nfpack UartBaudRate
{
    UartBaudRate_B50 = 50,
    UartBaudRate_B75 = 75,
    UartBaudRate_B110 = 110,
    UartBaudRate_B134 = 134,
    UartBaudRate_B150 = 150,
    UartBaudRate_B200 = 200,
    UartBaudRate_B300 = 300,
    UartBaudRate_B600 = 600,
    UartBaudRate_B1200 = 1200,
    UartBaudRate_B1800 = 1800,
    UartBaudRate_B2400 = 2400,
    UartBaudRate_B4800 = 4800,
    UartBaudRate_B9600 = 9600,
    UartBaudRate_B19200 = 19200,
    UartBaudRate_B28800 = 28800,
    UartBaudRate_B38400 = 38400,
    UartBaudRate_B57600 = 57600,
    UartBaudRate_B76800 = 76800,
    UartBaudRate_B115200 = 115200,
    UartBaudRate_B230400 = 230400,
    UartBaudRate_B460800 = 460800,
    UartBaudRate_B576000 = 576000,
    UartBaudRate_B921600 = 921600,
    UartBaudRate_Maximum = 7800000,
} UartBaudRate;

struct Library_nanoframework_Hardware_nanoframework_Hardware_NativeFunctions
{
    NANOCLR_NATIVE_DECLARE(SetPinFunction___STATIC__BOOLEAN__I4__I4);
    NANOCLR_NATIVE_DECLARE(SetPinDriveStrength___STATIC__BOOLEAN__I4__nanoframeworkHardwareNominalDriveStrength);
    NANOCLR_NATIVE_DECLARE(SetPinSchmidtTrigger___STATIC__BOOLEAN__I4__nanoframeworkHardwareSchmittTrigger);
    NANOCLR_NATIVE_DECLARE(SetPinSlewRate___STATIC__BOOLEAN__I4__nanoframeworkHardwareSlewRateLimiting);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoframework_Hardware;

#endif // NANOFRAMEWORK_HARDWARE_H
