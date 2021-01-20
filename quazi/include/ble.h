/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

//void quazi_ble_adv_start();
void quazi_ble_disconnect();
void quazi_ble_connect(int identity);
void quazi_ble_pair(int identity);
void quazi_ble_clear(int identity);
void quazi_ble_passkey_digit(int digit);
