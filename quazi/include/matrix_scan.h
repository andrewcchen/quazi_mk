#pragma once

void quazi_matrix_scan_init(void);

uint32_t quazi_matrix_scan_row(int row);

extern bool quazi_matrix_scan_key_down;

void quazi_matrix_scan_enter_idle(void);
void quazi_matrix_scan_leave_idle(void);
