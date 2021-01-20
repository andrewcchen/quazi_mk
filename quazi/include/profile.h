/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

int quazi_profile_init(void);

void quazi_profile_select(int profile);
int quazi_profile_selected(void);

void quazi_profile_pair(int profile);

void quazi_profile_clear_selected(void);
