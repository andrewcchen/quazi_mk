/*
 * Copyright (c) 2022 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

enum idle_level {
	IDLE_NONE,
	IDLE_MATRIX,
	IDLE_DISCONN,
	IDLE_SLEEP,
};

void quazi_idle_enter(enum idle_level level);

void quazi_idle_check(bool key_down);

void quazi_idle_leave(void);

void quazi_idle_init(void);
