//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// NOTE: order important here, as some of these headers depend on each other
#include "nanoCLR_Types.h"
#include "pico/cyw43_driver.h"
#include "cyw43_ll.h"
#include "cyw43_internal.h"
#include "cyw43_spi.h"
#include "cyw43_country.h"
#include "cyw43.h"
#include "cybt_shared_bus_driver.h"
#include "cyw43_configport.h"
#include "cyw43_internal.h"


#define UNUSED_PARAMETER 0

//cyw43_t cyw43_state;
cyw43_t *pCyw43_state = &cyw43_state;
//
//cyw43_ll_t self;
//cyw43_ll_t *pelf_in = &self;

void cyw43_low_level_init()
{
        cyw43_hal_pin_low(CYW43_PIN_WL_REG_ON);
        cyw43_delay_ms(20);
        cyw43_hal_pin_high(CYW43_PIN_WL_REG_ON);
        cyw43_delay_ms(50);
    
        cyw43_hal_get_mac(UNUSED_PARAMETER, pCyw43_state->mac);
        cyw43_ll_bus_init(&pCyw43_state->cyw43_ll, pCyw43_state->mac);





}
