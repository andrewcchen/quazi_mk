/*
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#include "keyboard.h"

LOG_MODULE_REGISTER(quazi, CONFIG_QUAZI_LOG_LEVEL);

void quazi_ble_init(void);

void main(void) {
	LOG_INF("QuaziMK Start");

	keyboard_setup();

	keyboard_init();

	quazi_ble_init();

	LOG_DBG("init done");

	//while (1) {
	//	keyboard_task();

	//	k_sleep(K_MSEC(10));
	//}
}
