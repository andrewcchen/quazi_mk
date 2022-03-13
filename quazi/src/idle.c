/*
 * Copyright (c) 2022 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>

#include <zephyr.h>
#include <logging/log.h>

#include "ble.h"
#include "matrix_scan.h"
#include "profile.h"
#include "quazi.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

enum idle_level {
	IDLE_NONE,
	IDLE_MATRIX,
	IDLE_DISCONN,
	IDLE_SLEEP,
};

static enum idle_level current_idle_level;

static void enter_idle(enum idle_level level)
{
	LOG_INF("entering idle level %d", level);

	current_idle_level = level;

	if (level >= IDLE_MATRIX) {
		quazi_matrix_scan_enter_idle();
		quazi_main_loop_stop();
	}
	if (level >= IDLE_DISCONN) {
		quazi_profile_enter_idle();
	}
	if (level >= IDLE_SLEEP) {
		// TODO
	}
}

void quazi_idle_leave(void)
{
	LOG_INF("leaving idle");

	if (current_idle_level >= IDLE_DISCONN) {
		quazi_profile_leave_idle();
	}

	if (current_idle_level >= IDLE_NONE) {
		quazi_matrix_scan_leave_idle();
		quazi_main_loop_start();
	}

	current_idle_level = IDLE_NONE;
}

bool quazi_idle_check(void)
{
	static uint32_t last_ble_act_time;

	uint32_t time = k_uptime_get_32();

	if (last_ble_act_time != 0 && !quazi_ble_is_active()) {
		if (time - last_ble_act_time > 30) {
			last_ble_act_time = 0;
			enter_idle(IDLE_DISCONN);
			return true;
		}
	} else {
		last_ble_act_time = time;
	}

	return false;
}
