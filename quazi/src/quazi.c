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
#include "quazi.h"

#include "keyboard.h"

LOG_MODULE_REGISTER(quazi, CONFIG_QUAZI_LOG_LEVEL);

void quazi_ble_init(void);

void quazi_main(void) {
	LOG_INF("QuaziMK Start");

	//settings_load();

	quazi_ble_init();
	quazi_profile_init();

	keyboard_setup();

	keyboard_init();

	LOG_DBG("init done");

	while (1) {
		keyboard_task();

		k_sleep(K_MSEC(10));
	}
}
