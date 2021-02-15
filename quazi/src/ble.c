/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/gap.h>

#include <logging/log.h>

#include "hog.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

// TODO performance all functions on a workqueue to avoid threading issues

enum advertising_state_machine {
	ADV_DISABLED,
	ADV_PAIRING,
	ADV_FAST,
	ADV_SLOW,
	ADV_CONNECTED
} static adv_state;

static int selected_identity;

static int passkey_digits_left;
static int passkey_entered;

struct bt_conn *quazi_ble_conn;

static void connected(struct bt_conn *conn, uint8_t err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		LOG_ERR("Failed to connect to %s (%u)", log_strdup(addr), err);
		return;
	}

	LOG_INF("Connected %s", log_strdup(addr));

	if (bt_le_adv_stop()) {
		LOG_ERR("Failed to stop advertising");
	}

	if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
		LOG_ERR("Failed to set security");
	}
	
	bt_conn_ref(conn);
	quazi_ble_conn = conn;
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Disconnected from %s (reason 0x%02x)", log_strdup(addr), reason);

	// TODO start advertising if not intentional disconnect
	
	if (quazi_ble_conn == conn) {
		quazi_ble_conn = NULL;
		bt_conn_unref(conn);
	}
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		LOG_INF("Security changed: %s level %u", log_strdup(addr), level);
	} else {
		LOG_ERR("Security failed: %s level %u err %d", log_strdup(addr), level, err);
		LOG_INF("Disconnecting.");
		bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
	}
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
	.security_changed = security_changed,
};

static void auth_passkey_entry(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Passkey entry for %s", log_strdup(addr));

	passkey_entered = 0;
	passkey_digits_left = 6;
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing cancelled: %s", log_strdup(addr));
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	LOG_INF("Pairing Complete (bonded %d)", bonded);
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	LOG_INF("Pairing Failed (%d). Disconnecting.", reason);

	bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_entry = auth_passkey_entry,
	.cancel = auth_cancel,
	.pairing_complete = pairing_complete,
	.pairing_failed = pairing_failed,
};

void quazi_ble_init(void)
{
	int err;

	err = bt_enable(NULL); // enable bt synchronously
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return;
	}

	err = settings_load_subtree("bt");
	if (err) {
		LOG_ERR("Load bluetooth settings failed (err %d)", err);
		return;
	}

	LOG_INF("Bluetooth initialized");

	quazi_hog_init();

	//quazi_ble_start_adv();

	//LOG_INF("Advertising successfully started");

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);
}

static void get_bonded_addr_helper(const struct bt_bond_info *info, void *data)
{
	const bt_addr_le_t **ppaddr = data;
	if (*ppaddr != NULL) {
		LOG_WRN("Multiple bonds stored for identity");
	}
	*ppaddr = &info->addr;
}

static bt_addr_le_t *get_bonded_addr(int identity)
{
	bt_addr_le_t *paddr = NULL;
	bt_foreach_bond(identity, get_bonded_addr_helper, &paddr);
	if (paddr != NULL) {
		LOG_WRN("No bonds stored for identity %d", identity);
	}
	return paddr;
}

// Advertisement data
static const struct bt_data ad_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL))
};

/*
static void update_advertising(void) {
	int err = 0;
	struct bt_le_adv_param adv_param = { 0 };
	adv_param.id = 

	LOG_DGB("adv_state %d", adv_state);

	switch (adv_state) {
	case ADV_PAIR:
		adv_param = 
		bt_le_whitelist_clear();
		err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
		
	}
}
*/

//static void start_directed_advertising() {
//	struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
//			BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME | BT_LE_ADV_OPT_DIR_ADDR_RPA,
//			0, 0, peer);
//}

static void start_pairing_advertising()
{
	// fast undirected advertising, 30-60ms interval
	struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
			BT_LE_ADV_OPT_CONNECTABLE | //BT_LE_ADV_OPT_ONE_TIME |
			BT_LE_ADV_OPT_USE_NAME,
			BT_GAP_ADV_FAST_INT_MIN_1, BT_GAP_ADV_FAST_INT_MAX_1, NULL);
	adv_param.id = selected_identity;

	//bt_le_whitelist_clear();

	int err = bt_le_adv_start(&adv_param, ad_data, ARRAY_SIZE(ad_data), NULL, 0);
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return;
	}

	LOG_INF("Started advertising");
}
 
/*void quazi_ble_adv_start(int identity) {
	int err;

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)", err);
		return err;
	}
}*/

static void disconnect_conn(struct bt_conn *conn, void *data)
{
	(void)data;

	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	int ret = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
	if (ret) {
		LOG_WRN("Failed to disconnect from %s (err %d)", log_strdup(addr), ret);
	}
}

void quazi_ble_disconnect(void)
{
	bt_conn_foreach(BT_CONN_TYPE_ALL, disconnect_conn, NULL);
	bt_le_adv_stop();
}

void quazi_ble_connect(int identity)
{
	LOG_DBG("connect id %d", identity);

	//if (selected_identity == identity)
	//	return;

	quazi_ble_disconnect();

	selected_identity = identity;

	start_pairing_advertising();
}

void quazi_ble_pair(int identity) {
	LOG_DBG("pair id %d", identity);

	quazi_ble_disconnect();

	selected_identity = identity;

	start_pairing_advertising();
}

void quazi_ble_clear(int identity) {
	LOG_DBG("clear id %d", identity);

	quazi_ble_disconnect();

	int ret = bt_unpair(identity, NULL);
	if (ret) {
		LOG_WRN("Failed to unpair all from id %d (err %d)", identity, ret);
	}
}

void quazi_ble_passkey_digit(int digit) {
	if (quazi_ble_conn && passkey_digits_left > 0) {
		passkey_digits_left--;
		passkey_entered = passkey_entered * 10 + digit;

		if (passkey_digits_left == 0) {
			LOG_INF("Passkey: %06d", passkey_entered);
			bt_conn_auth_passkey_entry(quazi_ble_conn, passkey_entered);
			passkey_entered = 0;
		}
	}
}
