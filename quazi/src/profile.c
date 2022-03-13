/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <logging/log.h>
#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/addr.h>

#include "profile.h"
#include "ble.h"
#include "main.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

#define UNSELECTED -1
static uint8_t selected_profile = UNSELECTED;

static int settings_set(const char *name, size_t len,
		settings_read_cb read_cb, void *cb_arg)
{
	const char *next;

	if (settings_name_steq(name, "selected", &next) && !next) {
		if (len != sizeof(selected_profile))
			return -EINVAL;

		int rc = read_cb(cb_arg, &selected_profile, sizeof(selected_profile));
		return (rc >= 0) ? 0 : rc;
	}

	return -ENOENT;
}

struct settings_handler settings_conf = {
	.name = "quazi/profile",
	.h_set = settings_set
};

/** Initialize the profile manager
 *
 * Must only be called after bt is enabled and bt settings loaded
 */
void quazi_profile_init(void)
{
	settings_register(&settings_conf);

	int err = settings_load_subtree("quazi/profile");
	if (err) {
		LOG_ERR("Failed to load profile settings (err %d)", err);
		return;
	}

	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t count = CONFIG_BT_ID_MAX;
	bt_id_get(addrs, &count);

	// create the profiles if they aren't loaded from settings
	for (int i = 1; i < count; i++) {
		// deleted profiles are set to BT_ADDR_LE_ANY
		if (0 == bt_addr_le_cmp(&addrs[i], BT_ADDR_LE_ANY)) {
			int rc = bt_id_reset(i, NULL, NULL);
			if (rc < 0) {
				LOG_ERR("Could not reset identity %d (err %d)", i, rc);
			}
		}
	}
	for (int i = count; i < CONFIG_BT_ID_MAX; i++) {
		int rc = bt_id_create(NULL, NULL);
		if (rc < 0) {
			LOG_ERR("Could not create identity %d (err %d)", i, rc);
		}
	}

	if (selected_profile != UNSELECTED) {
		quazi_profile_connect();
	}
};

/** Select profile
 */
void quazi_profile_select(int profile)
{
	if (profile != selected_profile) {
		selected_profile = profile;
		settings_save_one("quazi/profile/selected",
				&selected_profile, sizeof(selected_profile));
	}
}

/** Get the current selected profile / ble identity
 */
int quazi_profile_selected(void)
{
	return selected_profile;
}

/** Connect to paired host for selected profile
 */
void quazi_profile_connect(void)
{
	quazi_ble_connect(selected_profile);
}

/** Start pairing for selected profile
 */
void quazi_profile_pair(void)
{
	quazi_ble_pair(selected_profile);
}

/** Clear pairing information for selected profile
 */
void quazi_profile_clear(void)
{
	quazi_ble_unpair(selected_profile);
}

void quazi_profile_enter_idle(void)
{
	quazi_ble_disconnect();
}

void quazi_profile_leave_idle(void)
{
	quazi_profile_connect();
}
