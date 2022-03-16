/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>

#include "hid_leds.h"

#define HAS_NUM DT_HAS_CHOSEN(quazi_num_lock_led)
#define NUM_NODE DT_CHOSEN(quazi_num_lock_led)

#define HAS_CAPS DT_HAS_CHOSEN(quazi_caps_lock_led)
#define CAPS_NODE DT_CHOSEN(quazi_caps_lock_led)

#define HAS_SCROLL DT_HAS_CHOSEN(quazi_scroll_lock_led)
#define SCROLL_NODE DT_CHOSEN(quazi_scroll_lock_led)

#if HAS_NUM
static const struct device *num_dev;
static int num_pin;
#endif
#if HAS_CAPS
static const struct device *caps_dev;
static int caps_pin;
#endif
#if HAS_SCROLL
static const struct device *scroll_dev;
static int scroll_pin;
#endif

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

void quazi_hid_leds_init(void) {

#if HAS_NUM
	num_dev = device_get_binding(DT_GPIO_LABEL(NUM_NODE, gpios));
	num_pin = DT_GPIO_PIN(NUM_NODE, gpios);
	gpio_pin_configure(num_dev, num_pin,
			DT_GPIO_FLAGS(NUM_NODE, gpios) | GPIO_OUTPUT_INACTIVE);
#endif

#if HAS_CAPS
	caps_dev = device_get_binding(DT_GPIO_LABEL(CAPS_NODE, gpios));
	caps_pin = DT_GPIO_PIN(CAPS_NODE, gpios);
	gpio_pin_configure(caps_dev, caps_pin,
			DT_GPIO_FLAGS(CAPS_NODE, gpios) | GPIO_OUTPUT_INACTIVE);
#endif

#if HAS_SCROLL
	scroll_dev = device_get_binding(DT_GPIO_LABEL(SCROLL_NODE, gpios));
	scroll_pin = DT_GPIO_PIN(SCROLL_NODE, gpios);
	gpio_pin_configure(scroll_dev, scroll_pin,
			DT_GPIO_FLAGS(SCROLL_NODE, gpios) | GPIO_OUTPUT_INACTIVE);
#endif

}

void quazi_hid_leds_set(int leds) {
	/* HID keyboard output report:
	 * 0 NUM LOCK
	 * 1 CAPS LOCK
	 * 2 SCROLL LOCK
	 * 3 COMPOSE
	 * 4 KANA
	 */
#if HAS_NUM
	gpio_pin_set(num_dev, num_pin, leds & 1);
#endif
#if HAS_CAPS
	gpio_pin_set(caps_dev, caps_pin, leds & 2);
#endif
#if HAS_SCROLL
	gpio_pin_set(scroll_dev, scroll_pin, leds & 4);
#endif
}
