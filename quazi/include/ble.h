/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

extern struct bt_conn *quazi_ble_conn;

void quazi_ble_init(void);

void quazi_ble_disconnect(void);
void quazi_ble_connect(int identity);
void quazi_ble_pair(int identity);
void quazi_ble_unpair(int identity);
void quazi_ble_passkey_digit(int digit);
struct bt_conn *quazi_ble_get_conn();
