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

#include "matrix_scan.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

#define MATRIX_NODE DT_NODELABEL(matrix)

#define MATRIX_GPIO_PROP(i, row_or_col, prop) \
	DT_GPIO_##prop##_BY_IDX(MATRIX_NODE, row_or_col##_gpios, i)

#define MATRIX_GPIO_PIN_LIST(i, row_or_col) MATRIX_GPIO_PROP(i, row_or_col, PIN),
#define MATRIX_GPIO_FLAGS_LIST(i, row_or_col) MATRIX_GPIO_PROP(i, row_or_col, FLAGS),

static const struct device *row_port[MATRIX_ROWS];
static const struct device *col_port[MATRIX_COLS];

static gpio_pin_t row_pin[MATRIX_ROWS] =
	{ UTIL_LISTIFY(MATRIX_ROWS, MATRIX_GPIO_PIN_LIST, row) };
static gpio_pin_t col_pin[MATRIX_COLS] =
	{ UTIL_LISTIFY(MATRIX_COLS, MATRIX_GPIO_PIN_LIST, col) };

static const gpio_flags_t row_flags[MATRIX_ROWS] =
	{ UTIL_LISTIFY(MATRIX_ROWS, MATRIX_GPIO_FLAGS_LIST, row) };
static const gpio_flags_t col_flags[MATRIX_COLS] = 
	{ UTIL_LISTIFY(MATRIX_COLS, MATRIX_GPIO_FLAGS_LIST, col) };

//static idle_state

static void gpio_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	LOG_INF("gpio callback");
}

void quazi_matrix_scan_init(void)
{
	int ret;

#define MATRIX_GPIO_PORT_SET(i, row_or_col) \
	row_or_col##_port[i] = device_get_binding(MATRIX_GPIO_PROP(i, row_or_col, LABEL));

	UTIL_LISTIFY(MATRIX_ROWS, MATRIX_GPIO_PORT_SET, row)
	UTIL_LISTIFY(MATRIX_COLS, MATRIX_GPIO_PORT_SET, col)

	for (int i = 0; i < MATRIX_ROWS; i++) {
		ret = gpio_pin_configure(row_port[i], row_pin[i],
				row_flags[i] | GPIO_OUTPUT | GPIO_OUTPUT_LOW);

		if (ret < 0) LOG_ERR("matrix pin configure failed: %d", ret);
	}
	for (int i = 0; i < MATRIX_COLS; i++) {
		ret = gpio_pin_configure(col_port[i], col_pin[i],
				col_flags[i] | GPIO_INPUT | GPIO_PULL_DOWN);

		if (ret < 0) LOG_ERR("matrix pin configure failed: %d", ret);

		ret = gpio_pin_interrupt_configure(col_port[i], col_pin[i], GPIO_INT_DISABLE);

		if (ret < 0) LOG_ERR("matrix pin configure failed: %d", ret);
	}

	// TODO optimize
	static struct gpio_callback cb_data[MATRIX_COLS];
	for (int i = 0; i < MATRIX_COLS; i++) {
		gpio_init_callback(&cb_data[i], gpio_callback, BIT(col_pin[i]));
		gpio_add_callback(col_port[i], &cb_data[i]);
	}
}

// TODO debounce
uint32_t quazi_matrix_scan_row(int row)
{
	uint32_t r = 0;

	gpio_pin_set(row_port[row], row_pin[row], 1);

	for (int j = 0; j < MATRIX_COLS; j++) {
		r |= gpio_pin_get_raw(col_port[j], col_pin[j]) << j;
	}

	gpio_pin_set(row_port[row], row_pin[row], 0);

	return r;
}

void quazi_matrix_scan_enter_idle(void) {
	LOG_INF("matrix entering idle");

	for (int i = 0; i < MATRIX_ROWS; i++) {
		gpio_pin_set(row_port[i], row_pin[i], 1);
	}
	for (int i = 0; i < MATRIX_COLS; i++) {
		int ret = gpio_pin_interrupt_configure(col_port[i], col_pin[i], GPIO_INT_EDGE_RISING);

		if (ret < 0) LOG_ERR("matrix pin configure failed: %d", ret);
	}
}
