target_include_directories(quazi PUBLIC
	include
)

target_sources(quazi PRIVATE
	src/quazi.c
	src/ble.c
	src/hog.c
	src/profile.c
	src/matrix_scan.c
)

target_sources(app PRIVATE src/main.c)

target_include_directories(glue PRIVATE
	include
)

target_sources(glue PRIVATE
	qmk_glue/timer.c
	qmk_glue/wait.c
	qmk_glue/bootloader.c
	qmk_glue/process_bluetooth.c
	qmk_glue/matrix.c
	qmk_glue/keyboard.c
	qmk_glue/host_driver.c
)

target_include_directories(qmk PUBLIC
	qmk_include
)
