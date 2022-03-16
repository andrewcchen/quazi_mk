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
#include <stdio.h>

#include "matrix_scan.h"

#include "matrix.h"

static uint32_t matrix[MATRIX_ROWS];

void matrix_init(void)
{
	// quazi_matrix_scan_init is called by quazi_main
}

uint8_t matrix_scan(void)
{
	bool changed = false;

	for (int i = 0; i < MATRIX_ROWS; i++) {
		uint32_t r = quazi_matrix_scan_row(i);

		if (r != matrix[i])
			changed = true;

		matrix[i] = r;
	}

	return changed;
}

uint32_t matrix_get_row(uint8_t row)
{
	return matrix[row];
}

void matrix_print(void)
{
	fputs("0123456789ABCDEF\n", stdout);
	for (int i = 0; i < MATRIX_ROWS; i++) {
		for (int j = 0; j < MATRIX_COLS; j++) {
			fputc((matrix[i] & (1 << j)) ? '1' : '0', stdout);
		}
		fputs("\n", stdout);
	}
}
