target_include_directories(quazi PUBLIC include)

target_sources(quazi PRIVATE
	src/quazi.c
	src/ble.c
	src/hog.c
	src/profile.c
)

target_sources(app PRIVATE src/main.c)

target_include_directories(qmk PRIVATE tmk_api)
target_include_directories(qmk PRIVATE include)

target_sources(qmk PRIVATE
	tmk_api/timer.c
	tmk_api/wait.c
	tmk_api/bootloader.c
	tmk_api/process_bluetooth.c
	src/matrix.c
)
