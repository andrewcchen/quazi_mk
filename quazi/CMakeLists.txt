target_include_directories(quazi PUBLIC
	include
)

target_sources(quazi PRIVATE
	src/ble.c
	src/hid_leds.c
	src/hog.c
	src/idle.c
	src/matrix_scan.c
	src/profile.c
	src/quazi.c
)

target_include_directories(glue PRIVATE
	../quantum
	include
	qmk_include
)

target_sources(glue PRIVATE
	qmk_glue/timer.c
	qmk_glue/wait.c
	qmk_glue/bootloader.c
	qmk_glue/process_bluetooth.c
	qmk_glue/matrix.c
	qmk_glue/keyboard.c
	qmk_glue/host_driver.c
	qmk_glue/led_set.c
)

target_include_directories(qmk PUBLIC
	qmk_include
)

target_sources(app PRIVATE
	src/main.c
	src/shell.c
)
