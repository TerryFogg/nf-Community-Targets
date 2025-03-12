#pragma once

#include "stdbool.h"
#include "board.h"

typedef unsigned char CLR_UINT8;
typedef unsigned short CLR_UINT16;
typedef unsigned int CLR_UINT32;
typedef signed char CLR_INT8;
typedef signed short CLR_INT16;

// Timing register value is computed with the STM32CubeMX Tool Fast Mode @400kHz
// with I2CCLK = 125 MHz Rise time = 100ns, Fall time = 10ns
#define I2C_TIMING (0x70D04648)
#define I2C_MEMADD_SIZE_8BIT (0x00000001U)

#ifdef __cplusplus
extern "C"
{
#endif
  void Initialize_I2C();
  bool I2C_WaitOnTXISFlagUntilTimeout();
  bool I2C_IsAcknowledgeFailed();
  bool Write8_Read_I2C(CLR_INT8 slaveAddress, CLR_UINT8 I2cRegister,  CLR_UINT8 *dataReturned, CLR_UINT16 numberValuesExpected);
  bool Write8Bits(CLR_UINT16 I2CAddress, uint8_t I2CRegister, uint8_t I2CData);

#ifdef __cplusplus
}
#endif
