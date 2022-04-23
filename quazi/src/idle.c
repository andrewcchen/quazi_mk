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

static struct k_work_delayable enter_idle_work;
static struct k_work leave_idle_work;

static enum idle_level current_idle_level, next_idle_level;

static void enter_idle(struct k_work *)
{
	enum idle_level level = next_idle_level;

	LOG_INF("entering idle level %d", level);

	current_idle_level = level;

	if (level >= IDLE_MATRIX) {
		quazi_main_loop_stop();
		quazi_matrix_scan_enter_idle();
	}
	if (level >= IDLE_DISCONN) {
		quazi_profile_enter_idle();
	}
	if (level >= IDLE_SLEEP) {
		// TODO
	}

	if (level == IDLE_MATRIX) {
		next_idle_level = IDLE_DISCONN;
		k_work_reschedule(&enter_idle_work, K_MINUTES(5));
	}
}

static void leave_idle(struct k_work *)
{
	enum idle_level level = current_idle_level;

	current_idle_level = IDLE_NONE;

	LOG_INF("leaving idle");

	if (level >= IDLE_MATRIX) {
		// matrix_scan internally calls quazi_matrix_scan_leave_idle
		quazi_main_loop_start();
	}
	if (level >= IDLE_DISCONN) {
		quazi_profile_leave_idle();
	}
	// profile checks internally to reconnect on key press
}

void quazi_idle_enter(enum idle_level level)
{
	if (level < current_idle_level)
		return;
	next_idle_level = level;
	k_work_reschedule(&enter_idle_work, K_NO_WAIT);
}

void quazi_idle_leave(void)
{
	k_work_submit(&leave_idle_work);
}

/** Main loop task
 *
 * Check how long we have been idle and possibly enter idle state
 *
 * Call after qmk task in main loop
 */
void quazi_idle_check(bool key_down)
{
	if (key_down) {
		if (current_idle_level != IDLE_NONE) {
			k_work_submit(&leave_idle_work);
		}
		next_idle_level = IDLE_MATRIX;
		k_work_reschedule(&enter_idle_work, K_SECONDS(1));
	}
}

void quazi_idle_init(void)
{
	k_work_init_delayable(&enter_idle_work, enter_idle);
	k_work_init(&leave_idle_work, leave_idle);

	current_idle_level = next_idle_level = IDLE_DISCONN;
	k_work_schedule(&enter_idle_work, K_MSEC(100));
}
