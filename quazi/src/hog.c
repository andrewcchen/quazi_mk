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
#include <logging/log.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "ble.h"
#include "hog.h"
#include "hid_descriptor.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

enum {
	HIDS_REMOTE_WAKE = BIT(0),
	HIDS_NORMALLY_CONNECTABLE = BIT(1),
};

// HID Information
struct hids_info {
	uint16_t bcdHID;
	uint8_t bcountryCode;
	uint8_t flags;
} __packed;

enum {
	HIDS_INPUT = 0x01,
	HIDS_OUTPUT = 0x02,
	HIDS_FEATURE = 0x03,
};

// Report characteristic descriptor
struct report_chrc_desc {
	uint8_t id;
	uint8_t type;
} __packed;

static struct hids_info hid_info = {
	.bcdHID = 0x0112,
	.bcountryCode = 0x00,
	.flags = HIDS_NORMALLY_CONNECTABLE,
};

static struct report_chrc_desc keyboard_chrc_desc = {
	.id = REPORT_ID_KEYBOARD,
	.type = HIDS_INPUT,
};

static struct report_chrc_desc led_chrc_desc = {
	.id = REPORT_ID_KEYBOARD,
	.type = HIDS_OUTPUT,
};

#ifdef MOUSEKEY_ENABLE
static struct report_chrc_desc mouse_chrc_desc = {
	.id = REPORT_ID_MOUSE,
	.type = HIDS_INPUT,
};
#endif

#ifdef EXTRAKEY_ENABLE
static struct report_chrc_desc system_chrc_desc = {
	.id = REPORT_ID_SYSTEM,
	.type = HIDS_INPUT,
};

static struct report_chrc_desc consumer_chrc_desc = {
	.id = REPORT_ID_CONSUMER,
	.type = HIDS_INPUT,
};
#endif

static uint8_t keyboard_report[8];
static uint8_t led_report;

static ssize_t read_hid_info(struct bt_conn *conn,
		const struct bt_gatt_attr *attr, void *buf,
		uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
			&hid_info, sizeof(hid_info));
}

static ssize_t read_report_map(struct bt_conn *conn,
		const struct bt_gatt_attr *attr, void *buf,
		uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
			hid_report_descriptor, sizeof(hid_report_descriptor));
}

static ssize_t read_report_chrc_desc(struct bt_conn *conn,
		const struct bt_gatt_attr *attr, void *buf,
		uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
			attr->user_data, sizeof(struct report_chrc_desc));
}

static ssize_t read_keyboard_report(struct bt_conn *conn,
		const struct bt_gatt_attr *attr, void *buf,
		uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
			keyboard_report, sizeof(keyboard_report));
}


static ssize_t write_led_report(struct bt_conn *conn,
		const struct bt_gatt_attr *attr, const void *buf,
		uint16_t len, uint16_t offset, uint8_t flags)
{
	if (len != 1) {
		LOG_WRN("Invalid led report received: len = %d", len);
	}

	led_report = ((uint8_t *)buf)[0];

	return len;
}

static ssize_t write_ctrl_point(struct bt_conn *conn,
		const struct bt_gatt_attr *attr,
		const void *buf, uint16_t len, uint16_t offset,
		uint8_t flags)
{
	LOG_HEXDUMP_DBG(buf, len, "write_ctrl_point");

	return len;
}

// Token variables whose address we put in the user_data field of the report
// characteristic attribute structs. Then init can iterate over all the
// attributes and find the report characterstics
static char keyboard_attr_token;
#ifdef MOUSEKEY_ENABLE
static char mouse_attr_token;
#endif
#ifdef EXTRAKEY_ENABLE
static char system_attr_token;
static char consumer_attr_token;
#endif

/* HID Service Declaration */
BT_GATT_SERVICE_DEFINE(hog_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_HIDS),

	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_INFO, BT_GATT_CHRC_READ,
			BT_GATT_PERM_READ, read_hid_info, NULL, NULL),

	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT_MAP, BT_GATT_CHRC_READ,
			BT_GATT_PERM_READ, read_report_map, NULL, NULL),

	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
			BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
			BT_GATT_PERM_READ_ENCRYPT,
			read_keyboard_report, NULL, &keyboard_attr_token),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
			read_report_chrc_desc, NULL, &keyboard_chrc_desc),

	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
			BT_GATT_CHRC_WRITE,
			BT_GATT_PERM_WRITE_ENCRYPT,
			NULL, write_led_report, NULL),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
			read_report_chrc_desc, NULL, &led_chrc_desc),

#ifdef EXTRAKEY_ENABLE
	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
			BT_GATT_CHRC_NOTIFY,
			BT_GATT_PERM_NONE,
			NULL, NULL, &system_attr_token),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
			read_report_chrc_desc, NULL, &system_chrc_desc),

	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_REPORT,
			BT_GATT_CHRC_NOTIFY,
			BT_GATT_PERM_NONE,
			NULL, NULL, &consumer_attr_token),
	BT_GATT_CCC(NULL, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
	BT_GATT_DESCRIPTOR(BT_UUID_HIDS_REPORT_REF, BT_GATT_PERM_READ,
			read_report_chrc_desc, NULL, &consumer_chrc_desc),
#endif

	BT_GATT_CHARACTERISTIC(BT_UUID_HIDS_CTRL_POINT,
			BT_GATT_CHRC_WRITE_WITHOUT_RESP,
			BT_GATT_PERM_WRITE,
			NULL, write_ctrl_point, NULL),
);

static const struct bt_gatt_attr *keyboard_attr;
#ifdef MOUSEKEY_ENABLE
static const struct bt_gatt_attr *mouse_attr;
#endif
#ifdef EXTRAKEY_ENABLE
static const struct bt_gatt_attr *system_attr;
static const struct bt_gatt_attr *consumer_attr;
#endif

uint8_t quazi_hog_keyboard_leds(void)
{
	return led_report;
}

void quazi_hog_send_keyboard(uint8_t *report)
{
	memcpy(keyboard_report, report, sizeof(keyboard_report));
	if (quazi_ble_link_established) {
		int err = bt_gatt_notify(quazi_ble_conn, keyboard_attr, report, 8);
		if (err) {
			LOG_WRN("bt_gatt_notify failed (%d)", err);
		}
	}
}

void quazi_hog_send_mouse(uint8_t *report)
{
#ifdef MOUSEKEY_ENABLE
#endif
}

void quazi_hog_send_system(uint16_t data)
{
#ifdef EXTRAKEY_ENABLE
	if (quazi_ble_link_established) {
		int err = bt_gatt_notify(quazi_ble_conn, system_attr, &data, 2);
		if (err) {
			LOG_WRN("bt_gatt_notify failed (%d)", err);
		}
	}
#endif
}

void quazi_hog_send_consumer(uint16_t data)
{
#ifdef EXTRAKEY_ENABLE
	if (quazi_ble_link_established) {
		int err = bt_gatt_notify(quazi_ble_conn, consumer_attr, &data, 2);
		if (err) {
			LOG_WRN("bt_gatt_notify failed (%d)", err);
		}
	}
#endif
}

void quazi_hog_init(void)
{
	for (int i = 0; i < ARRAY_SIZE(attr_hog_svc); i++) {

		if (attr_hog_svc[i].user_data == &keyboard_attr_token)
			keyboard_attr = &attr_hog_svc[i];

#ifdef MOUSEKEY_ENABLE
		if (attr_hog_svc[i].user_data == &mouse_attr_token)
			mouse_attr = &attr_hog_svc[i];
#endif

#ifdef EXTRAKEY_ENABLE
		if (attr_hog_svc[i].user_data == &system_attr_token)
			system_attr = &attr_hog_svc[i];

		if (attr_hog_svc[i].user_data == &consumer_attr_token)
			consumer_attr = &attr_hog_svc[i];
#endif
	}
}
