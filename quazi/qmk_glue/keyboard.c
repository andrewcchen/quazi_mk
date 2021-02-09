#include "qmk_glue.h"

#include "keyboard.h"

void quazi_qmk_init(void) {
	quazi_qmk_host_driver_init();
	keyboard_setup();
	keyboard_init();
}

void quazi_qmk_task(void) {
	keyboard_task();
}
