/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <logging/log.h>

#include "quantum.h"

#include "profile.h"
#include "ble.h"

#include "quazi_process_bluetooth.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

#define QUAZI_BT_TAP_TIME 1000

bool process_bluetooth(uint16_t keycode, keyrecord_t *record)
{
	// key press times
	static uint16_t clear_time;
	static uint16_t profile_times[CONFIG_BT_ID_MAX];

	uint16_t time = record->event.time;
	int profile = keycode - BT_0;

	if (record->event.pressed) {
		if (keycode == BT_CLEAR) {
			clear_time = time;
			return false;
		} else if (keycode >= BT_0 && profile < CONFIG_BT_ID_MAX) {
			profile_times[profile] = time;
			return false;
		}
	} else {
		if (keycode == BT_CLEAR) {
			uint16_t diff = time - clear_time;
			if (diff >= QUAZI_BT_TAP_TIME) {
				quazi_profile_clear();
			}
			return false;
		} else if (keycode == BT_NEXT) {
			return false;
		} else if (keycode == BT_PREV) {
			return false;
		} else if (keycode >= BT_0 && profile < CONFIG_BT_ID_MAX) {
			uint16_t diff = time - profile_times[profile];

			quazi_profile_select(profile);

			if (diff >= QUAZI_BT_TAP_TIME)
				quazi_profile_pair();
			else
				quazi_profile_connect();

		} else if (keycode == OUT_AUTO) {
			return false;
		} else if (keycode == OUT_USB) {
			return false;
		} else if (keycode == OUT_BT) {
			return false;
		}
	}

	return true;
}

bool process_bluetooth_passkey(uint16_t keycode, keyrecord_t *record)
{
	if (record->event.pressed) {
		if (keycode == KC_0) {
			quazi_ble_passkey_digit(0);
		} else if (keycode >= KC_1 && keycode <= KC_9) {
			quazi_ble_passkey_digit(keycode - KC_1 + 1);
		}
	}
	return true;
}
