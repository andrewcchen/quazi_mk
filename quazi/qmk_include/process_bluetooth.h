/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

bool process_bluetooth(uint16_t keycode, keyrecord_t *record);
bool process_bluetooth_passkey(uint16_t keycode, keyrecord_t *record);
