

#include <zephyr.h>

#include "timer.h"

void timer_init(void) { /* do nothing */ }

void timer_clear(void) { /* not supported */ }

uint16_t timer_read(void) {
	return k_uptime_get_32();
}

uint32_t timer_read32(void) {
	return k_uptime_get_32();
}

uint16_t timer_elapsed(uint16_t last) {
	return TIMER_DIFF_16(timer_read(), last);
}

uint32_t timer_elapsed32(uint32_t last) {
	return TIMER_DIFF_32(timer_read32(), last);
}
