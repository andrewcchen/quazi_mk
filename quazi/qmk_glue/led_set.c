/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hid_leds.h"

#include "quazi_led_set.h"

void led_set_quazi(int usb_led) {
	quazi_hid_leds_set(usb_led);
}
