#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>
#include <stdio.h>

#include "matrix.h"

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

static uint32_t matrix[MATRIX_ROWS];

void matrix_init(void) {
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
		gpio_pin_configure(col_port[i], col_pin[i],
				col_flags[i] | GPIO_INPUT | GPIO_PULL_DOWN);

		if (ret < 0) LOG_ERR("matrix pin configure failed: %d", ret);
	}
}
// TODO debounce
uint8_t matrix_scan(void) {
	bool changed = false;

	for (int i = 0; i < MATRIX_ROWS; i++) {
		gpio_pin_set(row_port[i], row_pin[i], 0);
	}

	for (int i = 0; i < MATRIX_ROWS; i++) {
		gpio_pin_set(row_port[i], row_pin[i], 1);
		
		uint32_t row_prev = matrix[i];
		matrix[i] = 0;

		for (int j = 0; j < MATRIX_COLS; j++) {
			matrix[i] |= gpio_pin_get_raw(col_port[j], col_pin[j]) << j;
		}

		gpio_pin_set(row_port[i], row_pin[i], 0);

		if (matrix[i] != row_prev)
			changed = true;
	}

	return changed;
}

uint32_t matrix_get_row(uint8_t row) {
	return matrix[row];
}

void matrix_print(void) {
	printf("0123456789ABCDEF\n");
	for (int i = 0; i < MATRIX_ROWS; i++) {
		for (int j = 0; j < MATRIX_COLS; j++) {
			printf((matrix[i] & (1 << j)) ? "1" : "0");
		}
		printf("\n");
	}
}
