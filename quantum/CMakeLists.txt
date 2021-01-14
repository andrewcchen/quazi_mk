target_include_directories(app PRIVATE .)
target_include_directories(app PRIVATE audio)
target_include_directories(app PRIVATE process_keycode)

target_sources(app PRIVATE
	quantum.c
	keymap_common.c
	keycode_config.c
	#matrix_common.c
	#matrix.c
	debounce/sym_g.c
	process_keycode/process_space_cadet.c
	process_keycode/process_magic.c
	process_keycode/process_grave_esc.c
)
