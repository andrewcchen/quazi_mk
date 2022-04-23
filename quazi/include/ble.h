/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

extern struct bt_conn *quazi_ble_conn;
extern bool quazi_ble_link_established;

void quazi_ble_init(void);

bool quazi_ble_is_active(void);

void quazi_ble_connect(int identity);
void quazi_ble_disconnect(void);
void quazi_ble_pair(int identity);
void quazi_ble_unpair(int identity);

void quazi_ble_passkey_digit(int digit);
