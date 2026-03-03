#pragma once
//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

void powman_init(uint64_t abs_time_ms);
int powman_off();
int powman_off_until_gpio_high(int gpio);
int powman_off_until_time(uint64_t abs_time_ms);
int powman_off_for_ms(uint64_t duration_ms);
