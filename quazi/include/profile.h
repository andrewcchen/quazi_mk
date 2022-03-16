/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

void quazi_profile_init(void);

void quazi_profile_select(int profile);
int quazi_profile_selected(void);

void quazi_profile_connect(void);
void quazi_profile_disconnect(void);
void quazi_profile_pair(void);
void quazi_profile_clear(void);

void quazi_profile_enter_idle(void);
void quazi_profile_leave_idle(void);

void quazi_profile_task(bool key_down);
