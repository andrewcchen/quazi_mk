cmake_minimum_required(VERSION 3.13.1)

if (NOT DEFINED ZEPHYR_TOOLCHAIN_VARIANT)
	set(ZEPHYR_TOOLCHAIN_VARIANT gnuarmemb)
endif()

if (NOT DEFINED GNUARMEMB_TOOLCHAIN_PATH)
	set(GNUARMEMB_TOOLCHAIN_PATH /usr)
endif()

if (NOT DEFINED BOARD)
	set(BOARD blu60)
endif()

list(APPEND CFLAGS "-Wstrict-prototypes")

list(APPEND BOARD_ROOT ${CMAKE_SOURCE_DIR})

list(APPEND DTS_ROOT ${CMAKE_SOURCE_DIR}/quazi)

list(APPEND ZEPHYR_MODULES
	${CMAKE_SOURCE_DIR}/lib/cmsis
	${CMAKE_SOURCE_DIR}/lib/tinycrypt
	${CMAKE_SOURCE_DIR}/lib/hal_nordic
)

# Find Zephyr. This also loads Zephyr's build system.
find_package(Zephyr REQUIRED HINTS lib/zephyr)

project(quazi_mk)

add_subdirectory(${BOARD_DIR})
add_subdirectory(quazi)
add_subdirectory(quantum)
add_subdirectory(tmk_core)

target_include_directories(app PRIVATE drivers)
target_sources(app PRIVATE
	drivers/eeprom/eeprom_driver.c
	drivers/eeprom/eeprom_transient.c
)

add_definitions(-Dxprintf=printf -Dprint=printf)
add_definitions(-DMATRIX_ROWS=5 -DMATRIX_COLS=14)
