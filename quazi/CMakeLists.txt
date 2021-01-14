target_include_directories(app PRIVATE include)

target_sources(app PRIVATE
	src/main.c
	src/ble.c
	src/hog.c
	src/matrix.c
)

target_sources(app PRIVATE
	tmk_api/timer.c
	tmk_api/wait.c
	tmk_api/bootloader.c
)
