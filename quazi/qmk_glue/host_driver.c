/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>

#include "qmk_glue.h"
#include "hog.h"

#include "host.h"

static uint8_t keyboard_leds(void)
{
	return quazi_hog_keyboard_leds();
}

static void send_keyboard(report_keyboard_t *report)
{
	return quazi_hog_send_keyboard((uint8_t *)report);
}

static void send_mouse(report_mouse_t *report)
{
	return quazi_hog_send_mouse((uint8_t *)report);
}

static void send_system(uint16_t data)
{
	return quazi_hog_send_system(data);
}

static void send_consumer(uint16_t data)
{
	return quazi_hog_send_consumer(data);
}

static host_driver_t host_driver = {
	.keyboard_leds = keyboard_leds,
	.send_keyboard = send_keyboard,
	.send_mouse = send_mouse,
	.send_system = send_system,
	.send_consumer = send_consumer,
};

void quazi_qmk_host_driver_init(void)
{
	host_set_driver(&host_driver);
}
