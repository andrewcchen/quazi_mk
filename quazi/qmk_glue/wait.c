/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>

#include "wait.h"

void wait_ms(uint32_t ms)
{
	k_sleep(K_MSEC(ms));
}

void wait_us(uint32_t us)
{
	if (us < 1000) k_busy_wait(us);
	else k_sleep(K_MSEC(us / 1000));
}
