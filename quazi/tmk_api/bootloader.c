

#include <zephyr.h>

#include "bootloader.h"

void bootloader_jump(void) {
	k_oops();
}
