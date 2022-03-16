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
#include "profile.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

/// Handle for current connection, NULL if disconnected
struct bt_conn *quazi_ble_conn;

static int8_t connect_id, unpair_id, pair_id;
static bool advertising_active;

static int8_t passkey_digits_left;
static int passkey_entered;

static struct k_delayed_work stop_adv_work;
static struct k_work start_directed_adv_work;
static struct k_work start_pairing_adv_work;
static struct k_work disconnect_work;
static struct k_work unpair_work;


static void do_adv_stop(void) {
	advertising_active = false;

	int err = bt_le_adv_stop();
	if (err) {
		LOG_ERR("Failed to stop advertising (err %d)", err);
	}
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	int rc;

	do_adv_stop();

	k_delayed_work_cancel(&stop_adv_work);

	char addr[BT_ADDR_LE_STR_LEN];
	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (err) {
		LOG_ERR("Failed to connect to %s (%u)", log_strdup(addr), err);
		return;
	}

	bt_conn_ref(conn);
	quazi_ble_conn = conn;

	rc = bt_conn_set_security(conn, BT_SECURITY_L4);
	if (rc) {
		LOG_ERR("Failed to set security (err %d)", rc);
	}

	// TODO update connection parameters

	LOG_INF("Connected %s", log_strdup(addr));
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Disconnected from %s (reason 0x%02x)", log_strdup(addr), reason);

	if (quazi_ble_conn != conn)
		return;

	bt_conn_unref(conn);
	quazi_ble_conn = NULL;

	if (reason == BT_HCI_ERR_CONN_TIMEOUT) {
		k_work_submit(&start_directed_adv_work);
	}
}

static void security_changed(struct bt_conn *conn, bt_security_t level, enum bt_security_err err)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	if (!err) {
		LOG_INF("Security changed: %s level %u", log_strdup(addr), level);
	} else {
		LOG_ERR("Security failed: %s level %u (err %d)", log_strdup(addr), level, err);
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
	LOG_INF("Pairing cancelled");
}

static void unpair_other(const struct bt_bond_info *info, void *data) {
	bt_addr_le_t *addr = data;
	if (bt_addr_le_cmp(addr, &info->addr)) {
		int err = bt_unpair(unpair_id, &info->addr);
		if (err) {
			char addr_str[BT_ADDR_LE_STR_LEN];
			bt_addr_le_to_str(&info->addr, addr_str, sizeof(addr_str));
			LOG_WRN("Failed to unpair %s (err %d)", log_strdup(addr_str), err);
		}
	}
}

static void pairing_complete(struct bt_conn *conn, bool bonded)
{
	LOG_INF("Pairing Complete (bonded %d)", bonded);

	struct bt_conn_info info;
	bt_conn_get_info(conn, &info);
	bt_foreach_bond(info.id, unpair_other, (void *)bt_conn_get_dst(conn));
}

static void pairing_failed(struct bt_conn *conn, enum bt_security_err reason)
{
	LOG_INF("Pairing Failed (%d)", reason);
	bt_conn_disconnect(conn, BT_HCI_ERR_AUTH_FAIL);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.passkey_entry = auth_passkey_entry,
	.cancel = auth_cancel,
	.pairing_complete = pairing_complete,
	.pairing_failed = pairing_failed,
};

/** Advertising data */
static const struct bt_data ad_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_HIDS_VAL))
};

/** Stop advertising
 */
static void stop_adv(struct k_work *)
{
	do_adv_stop();

	LOG_INF("Stopped advertising");
}

static void get_bonded_addr_helper(const struct bt_bond_info *info, void *data)
{
	bt_addr_le_t *addr = data;
	if (bt_addr_le_cmp(addr, BT_ADDR_LE_NONE)) {
		LOG_WRN("Multiple bonds stored for id %d", connect_id);
	}
	bt_addr_le_copy(addr, &info->addr);
}

/** Start fast directed advertising to connect to bonded host
 */
static void start_directed_adv(struct k_work *)
{
	if (quazi_ble_conn != NULL) {
		bt_conn_unref(quazi_ble_conn);
		quazi_ble_conn = NULL;
	}

	bt_addr_le_t _addr, *addr = &_addr;
	bt_addr_le_copy(addr, BT_ADDR_LE_NONE);

	bt_foreach_bond(connect_id, get_bonded_addr_helper, addr);
	if (!bt_addr_le_cmp(addr, BT_ADDR_LE_NONE)) {
		LOG_ERR("No bonds stored for id %d", connect_id);
		return;
	}

	struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
			BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME | BT_LE_ADV_OPT_DIR_ADDR_RPA,
			0, 0, addr);
	adv_param.id = connect_id;

	if (IS_ENABLED(CONFIG_BT_WHITELIST)) {
		bt_le_whitelist_clear();
		bt_le_whitelist_add(addr);
	}

	int err = bt_le_adv_start(&adv_param, ad_data, ARRAY_SIZE(ad_data), NULL, 0);
	if (err) {
		LOG_ERR("Failed to start directed advertising (err %d)", err);
		return;
	}

	advertising_active = true;

	LOG_INF("Started directed advertising");
}

/** Start fast undirected advertising to connect to non-bonded host
 *
 * 180s advertising duration, 20ms to 30ms advertising interval
 */
static void start_pairing_adv(struct k_work *)
{
	if (quazi_ble_conn != NULL) {
		bt_conn_unref(quazi_ble_conn);
		quazi_ble_conn = NULL;
	}

	struct bt_le_adv_param adv_param = BT_LE_ADV_PARAM_INIT(
			BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_ONE_TIME | BT_LE_ADV_OPT_USE_NAME,
			0x20, 0x30, NULL);
	adv_param.id = pair_id;

	if (IS_ENABLED(CONFIG_BT_WHITELIST)) {
		bt_le_whitelist_clear();
	}

	int err = bt_le_adv_start(&adv_param, ad_data, ARRAY_SIZE(ad_data), NULL, 0);
	if (err) {
		LOG_ERR("Failed to start pairing advertising (err %d)", err);
		return;
	}

	k_delayed_work_submit(&stop_adv_work, K_SECONDS(180));

	advertising_active = true;

	LOG_INF("Started pairing advertising");
}

static void disconnect_conn(struct bt_conn *conn, void *)
{
	bt_conn_ref(conn);

	bt_addr_t *addr = bt_conn_get_dst(conn);
	if (bt_addr_cmp(addr, BT_ADDR_LE_NONE)) {

		int err = bt_conn_disconnect(conn, BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		if (err) {
			char addr_str[BT_ADDR_LE_STR_LEN];
			bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
			LOG_WRN("Failed to disconnect from %s (err %d)", log_strdup(addr_str), err);
		}
	}

	bt_conn_unref(conn);
}

static void disconnect(struct k_work *work)
{
	do_adv_stop();

	bt_conn_foreach(BT_CONN_TYPE_ALL, disconnect_conn, NULL);
}

static void unpair(struct k_work *work)
{
	int err = bt_unpair(unpair_id, NULL);
	if (err) {
		LOG_WRN("Failed to unpair for id %d (err %d)", unpair_id, err);
	}
}


void quazi_ble_connect(int identity)
{
	// check if already connected to same identity
	if (quazi_ble_conn == NULL || identity != connect_id) {

		connect_id = identity;

		if (!k_work_pending(&start_directed_adv_work)) {
			k_work_submit(&disconnect_work);
			k_work_submit(&start_directed_adv_work);
		}
	}
}

void quazi_ble_disconnect(void)
{
	k_work_submit(&disconnect_work);
}

void quazi_ble_pair(int identity)
{
	pair_id = identity;

	if (!k_work_pending(&start_pairing_adv_work)) {
		k_work_submit(&disconnect_work);
		k_work_submit(&start_pairing_adv_work);
	}
}

void quazi_ble_unpair(int identity)
{
	unpair_id = identity;

	if (!k_work_pending(&unpair_work)) {
		k_work_submit(&disconnect_work);
		k_work_submit(&unpair_work);
	}
}

void quazi_ble_passkey_digit(int digit)
{
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

void quazi_ble_is_active(void) {
	return quazi_ble_conn != NULL || advertising_active;
}

void quazi_ble_init(void)
{
	k_delayed_work_init(&stop_adv_work, stop_adv);
	k_work_init(&start_directed_adv_work, start_directed_adv);
	k_work_init(&start_pairing_adv_work, start_pairing_adv);
	k_work_init(&disconnect_work, disconnect);
	k_work_init(&unpair_work, unpair);

	advertising_active = false;

	int err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)", err);
		return;
	}

	err = settings_load_subtree("bt");
	if (err) {
		LOG_ERR("Failed to load bluetooth settings (err %d)", err);
		return;
	}

	quazi_hog_init();

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);

	LOG_INF("Bluetooth initialized");
}
