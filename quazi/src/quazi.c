/*
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

void quazi_main(void) {
	LOG_INF("QuaziMK Start");

	//settings_load();

	quazi_ble_init();
	quazi_qmk_init();

	LOG_DBG("init done");

	while (1) {
		quazi_qmk_task();

		k_sleep(K_MSEC(10));
	}
}
