//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// The graphics library interface includes

#ifndef _TOUCHDEVICE_NULL
#define _TOUCHDEVICE_NULL 1

#include "TouchDevice.h"
#include "TouchInterface.h"

struct TouchDevice g_TouchDevice;
extern TouchInterface g_TouchInterface;

// read/write I2C data
//
bool TouchDevice::Initialize() {
  return false;
}

bool TouchDevice::Enable(GPIO_INTERRUPT_SERVICE_ROUTINE touchIsrProc) {
  UNUSED(touchIsrProc);
  return false;
}

bool TouchDevice::Disable() { return false; }

TouchPointDevice TouchDevice::GetPoint() {
  TouchPointDevice t1;
  return t1;
}

#endif
