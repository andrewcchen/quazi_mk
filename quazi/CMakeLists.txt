target_include_directories(quazi PRIVATE include)

target_sources(quazi PRIVATE
	src/main.c
	src/ble.c
	src/hog.c
	src/matrix.c
)

target_sources(qmk PRIVATE
	tmk_api/timer.c
	tmk_api/wait.c
	tmk_api/bootloader.c
)
