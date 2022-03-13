/*
 * Copyright (c) 2022 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>

#include <zephyr.h>
#include <logging/log.h>

#include "idle.h"
#include "ble.h"
#include "main.h"
#include "matrix_scan.h"
#include "profile.h"

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

	if (current_idle_level >= IDLE_MATRIX) {
		quazi_profile_leave_idle();
		quazi_matrix_scan_leave_idle();
		quazi_main_loop_start();
	}

	current_idle_level = IDLE_NONE;
}

/** Check how long we have been idle and possibly enter idle state
 *
 * Call after qmk task in main loop
 */
bool quazi_idle_check(void)
{
	static uint32_t last_key_act_time;

	uint32_t time = k_uptime_get_32();

	bool key_down = quazi_matrix_scan_key_down;
	quazi_matrix_scan_key_down = false;

	if (!key_down) {
		if (time - last_key_act_time > 1000) {
			enter_idle(IDLE_MATRIX);
			return true;
		}
	} else {
		last_key_act_time = time;
	}

	return false;
}
