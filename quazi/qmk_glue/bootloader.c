/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>

#include "bootloader.h"

void bootloader_jump(void)
{
	k_oops();
}
