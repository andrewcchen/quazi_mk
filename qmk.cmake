target_include_directories(qmk PUBLIC
	drivers
	platforms
	quantum
	quantum
	quantum/audio
	quantum/bootmagic
	quantum/logging
	quantum/process_keycode
	quantum/sequencer
	tmk_core/protocol
)

target_sources(qmk PRIVATE
	drivers/eeprom/eeprom_driver.c
	drivers/eeprom/eeprom_transient.c
	quantum/action.c
	quantum/action_layer.c
	quantum/action_tapping.c
	quantum/action_util.c
	quantum/bitwise.c
	quantum/bootmagic/magic.c
	quantum/eeconfig.c
	quantum/keyboard.c
	quantum/keycode_config.c
	quantum/keymap_common.c
	quantum/led.c
	quantum/logging/debug.c
	quantum/logging/print.c
	quantum/logging/sendchar.c
	quantum/process_keycode/process_grave_esc.c
	quantum/process_keycode/process_magic.c
	quantum/process_keycode/process_space_cadet.c
	quantum/quantum.c
	tmk_core/protocol/host.c
	tmk_core/protocol/report.c
)
