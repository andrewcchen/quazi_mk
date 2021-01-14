target_include_directories(qmk PUBLIC
	quantum
	quantum/process_keycode
	#quantum/audio
)

target_sources(qmk PRIVATE
	quantum/quantum.c
	quantum/keymap_common.c
	quantum/keycode_config.c
	#quantum/matrix_common.c
	#quantum/matrix.c
	quantum/debounce/sym_g.c
	quantum/process_keycode/process_space_cadet.c
	quantum/process_keycode/process_magic.c
	quantum/process_keycode/process_grave_esc.c
)

target_include_directories(qmk PUBLIC
	tmk_core/common
)

target_sources(qmk PRIVATE
	tmk_core/common/host.c
	tmk_core/common/keyboard.c
	tmk_core/common/action.c
	tmk_core/common/action_tapping.c
	tmk_core/common/action_macro.c
	tmk_core/common/action_layer.c
	tmk_core/common/action_util.c
	tmk_core/common/print.c
	tmk_core/common/debug.c
	tmk_core/common/sendchar_null.c
	tmk_core/common/util.c
	tmk_core/common/report.c
	tmk_core/common/magic.c
	tmk_core/common/eeconfig.c
)

target_include_directories(qmk PUBLIC
	drivers
)

target_sources(qmk PRIVATE
	drivers/eeprom/eeprom_driver.c
	drivers/eeprom/eeprom_transient.c
)
