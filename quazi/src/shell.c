/*
 * Copyright (c) 2021 Andrew Chen <andrew@xortux.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr.h>
#include <logging/log.h>
#include <shell/shell.h>

#include <version.h>

#include "profile.h"
#include "ble.h"

LOG_MODULE_DECLARE(quazi, CONFIG_QUAZI_LOG_LEVEL);

static void select_profile(size_t argc, char **argv) {
	if (argc > 1) {
		int profile = argv[1][0] - '0';
		if (profile >= 0 && profile <= 9)
			quazi_profile_select(profile);
	}
}

static int cmd_pair(const struct shell *shell, size_t argc, char **argv) {
	ARG_UNUSED(shell);

	select_profile(argc, argv);
	quazi_profile_pair();

	return 0;
}

static int cmd_connect(const struct shell *shell, size_t argc, char **argv) {
	ARG_UNUSED(shell);

	select_profile(argc, argv);
	quazi_profile_connect();

	return 0;
}

static int cmd_clear(const struct shell *shell, size_t argc, char **argv) {
	ARG_UNUSED(shell);

	select_profile(argc, argv);
	quazi_profile_clear();

	return 0;
}

static int cmd_disconnect(const struct shell *shell, size_t argc, char **argv) {
	ARG_UNUSED(shell);
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	quazi_ble_disconnect();

	return 0;
}

static int cmd_passkey(const struct shell *shell, size_t argc, char **argv) {
	ARG_UNUSED(shell);

	char *p = argv[1];
	while (*p) {
		quazi_ble_passkey_digit(*p - '0');
		p++;
	}

	return 0;
}

SHELL_STATIC_SUBCMD_SET_CREATE(quazi_cmds,
	SHELL_CMD_ARG(pair, NULL,
		  "pair [profile]",
		  cmd_pair, 1, 1),
	SHELL_CMD_ARG(connect, NULL,
		  "connect [profile]",
		  cmd_connect, 1, 1),
	SHELL_CMD_ARG(disconnect, NULL,
		  "disconnect",
		  cmd_disconnect, 1, 0),
	SHELL_CMD_ARG(clear, NULL,
		  "clear [profile]",
		  cmd_clear, 1, 1),
	SHELL_CMD_ARG(passkey, NULL,
		  "passkey <passkey>",
		  cmd_passkey, 2, 0),
	SHELL_SUBCMD_SET_END /* Array terminated. */
);

SHELL_CMD_REGISTER(quazi, &quazi_cmds, "QuaziMK commands", NULL);
