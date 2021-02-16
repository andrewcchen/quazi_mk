/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#include <settings/settings.h>

#include "profile.h"
#include "ble.h"
#include "quazi.h"
#include "qmk_glue.h"

LOG_MODULE_REGISTER(quazi, CONFIG_QUAZI_LOG_LEVEL);

void quazi_main(void)
{
	LOG_INF("QuaziMK Start");

	int err = settings_subsys_init();
	if (err) {
		LOG_ERR("settings_subsys_init failed (err %d)", err);
	}

	quazi_ble_init();

	quazi_profile_init();

	quazi_qmk_init();

	LOG_DBG("init done");

	while (1) {
		quazi_qmk_task();

		k_sleep(K_MSEC(10));
	}
}
