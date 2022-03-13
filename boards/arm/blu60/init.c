#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/flash.h>
//#include <logging/log.h>
#include <nrfx.h>

#include "keyboard.h"

#include <sys/printk.h>
#include <sys/reboot.h>

/* Requires config options:
 * CONFIG_FLASH=y
 * CONFIG_SOC_FLASH_NRF_UICR=y
 * CONFIG_REBOOT=y
 */

#if !CONFIG_FLASH || !CONFIG_SOC_FLASH_NRF_UICR || !CONFIG_REBOOT
#error Config options required for configuring UICR
#endif

#define FLASH_NODE DT_NODELABEL(flash0)

//LOG_MODULE_DECLARE(quazi);

static void configure_uicr(void) {
	// If pin 21 is mapped to nRESET
	if ((NRF_UICR->PSELRESET[0] & UICR_PSELRESET_CONNECT_Msk) == 0 ||
		(NRF_UICR->PSELRESET[1] & UICR_PSELRESET_CONNECT_Msk) == 0) {
		const struct device *flash = device_get_binding(DT_CHOSEN_ZEPHYR_FLASH_CONTROLLER_LABEL);

		// Erase UICR to disable reset pin function, and use pin 21 as gpio
		flash_write_protection_set(flash, false);
		flash_erase(flash, (off_t)NRF_UICR, sizeof(*NRF_UICR));
		flash_write_protection_set(flash, true);

		sys_reboot(SYS_REBOOT_COLD);
	}

	// If pin 9 and 10 are mapped to NFC pins
	if ((NRF_UICR->NFCPINS & UICR_NFCPINS_PROTECT_Msk) != 0) {
		const struct device *flash = device_get_binding(DT_CHOSEN_ZEPHYR_FLASH_CONTROLLER_LABEL);
		uint32_t nfcpins = 0;

		// Disable NFC function, and use pin 9 and 10 as gpio
		flash_write_protection_set(flash, false);
		flash_write(flash, (off_t)&NRF_UICR->NFCPINS, &nfcpins, 4); 
		flash_write_protection_set(flash, true);

		sys_reboot(SYS_REBOOT_COLD);
	}
}

void keyboard_pre_init_kb(void) {
	configure_uicr();

	keyboard_pre_init_user();
}
