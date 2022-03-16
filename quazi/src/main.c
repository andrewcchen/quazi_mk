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

#include "main.h"
#include "ble.h"
#include "hid_leds.h"
#include "idle.h"
#include "matrix_scan.h"
#include "profile.h"
#include "qmk_glue.h"

#include <pm/state.h>
#include <pm/pm.h>

LOG_MODULE_REGISTER(quazi, CONFIG_QUAZI_LOG_LEVEL);

static struct k_timer main_timer;
static struct k_work main_work;

static void main_timer_handler(struct k_timer *)
{
	k_work_submit(&main_work);
}

static void quazi_main_task(struct k_work *)
{
	quazi_matrix_scan_key_down = false;

	quazi_qmk_task();

	quazi_idle_check(quazi_matrix_scan_key_down);
}

void quazi_main_loop_start(void)
{
	k_timer_start(&main_timer, K_NO_WAIT, K_MSEC(10));
}

void quazi_main_loop_stop(void)
{
	k_timer_stop(&main_timer);
}

void quazi_main(void)
{
	LOG_INF("QuaziMK Start");

	int err = settings_subsys_init();
	if (err) {
		LOG_ERR("settings_subsys_init failed (err %d)", err);
	}

	quazi_matrix_scan_init();
	quazi_hid_leds_init();

	quazi_idle_init();
	quazi_ble_init();
	quazi_profile_init();
	quazi_qmk_init();

	k_timer_init(&main_timer, main_timer_handler, NULL);
	k_work_init(&main_work, quazi_main_task);

	quazi_main_loop_start();
}
