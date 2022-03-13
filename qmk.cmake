target_include_directories(qmk PUBLIC
	platforms
	platforms/zephyr
	quantum
	quantum/audio
	quantum/bootmagic
	quantum/logging
	quantum/process_keycode
	quantum/sequencer
	tmk_core/protocol
)

target_sources(qmk PRIVATE
	quantum/action.c
	quantum/action_layer.c
	quantum/action_macro.c
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
	quantum/process_keycode/process_grave_esc.c
	quantum/process_keycode/process_magic.c
	quantum/process_keycode/process_space_cadet.c
	quantum/quantum.c
	tmk_core/protocol/host.c
	tmk_core/protocol/report.c
)

target_include_directories(qmk PUBLIC
	drivers
)

target_sources(qmk PRIVATE
	drivers/eeprom/eeprom_driver.c
	drivers/eeprom/eeprom_transient.c
)
