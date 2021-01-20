/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <logging/log.h>
#include <sys/__assert.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/addr.h>

#include "profile.h"
#include "ble.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

static int selected_profile;

/** Initialize the profile manager
 *
 * Must be called after bt_enable()
 */
int quazi_profile_init(void) {
	//selected_profile = 1;

	bt_addr_le_t addrs[CONFIG_BT_ID_MAX];
	size_t count = CONFIG_BT_ID_MAX;
	bt_id_get(addrs, &count);

	// create the profiles if they aren't loaded from settings

	for (int i = 1; i < count; i++) {
		// deleted profiles are set to BT_ADDR_LE_ANY
		if (0 == bt_addr_le_cmp(&addrs[i], BT_ADDR_LE_ANY)) {
			int ret = bt_id_reset(i, NULL, NULL);
			if (ret < 0) {
				LOG_ERR("Could not reset identity %d (err %d)", i, ret);
			}
		}
	}
	for (int i = count; i < CONFIG_BT_ID_MAX; i++) {
		int ret = bt_id_create(NULL, NULL);
		if (ret < 0) {
			LOG_ERR("Could not create identity %d (err %d)", i, ret);
		}
	}

	//quazi_ble_adv_start();
	// TODO load selected profile from settings
	quazi_profile_select(1);

	return 0;
};

/** Select profile
 */
void quazi_profile_select(int profile) {
	//__ASSERT(profile < CONFIG_BT_ID_MAX);
	selected_profile = profile;
	quazi_ble_connect(profile);
}

/** Get the current selected profile / ble identity
 */
int quazi_profile_selected(void) {
	return selected_profile;
}

/** Start pairing for profile
 */
void quazi_profile_pair(int profile) {
	quazi_ble_pair(profile);
}

/** Clear pairing information for selected profile
 */
void quazi_profile_clear_selected(void) {
	quazi_ble_clear(selected_profile);
}
