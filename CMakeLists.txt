cmake_minimum_required(VERSION 3.13.1)

set(CMAKE_C_FLAGS_DEBUG "-ggdb3 -O0" CACHE STRING "Flags used by the C compiler during DEBUG builds.")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3 -O0" CACHE STRING "Flags used by the CXX compiler during DEBUG builds.")

if (NOT DEFINED ZEPHYR_TOOLCHAIN_VARIANT)
	set(ZEPHYR_TOOLCHAIN_VARIANT gnuarmemb)
endif()

if (NOT DEFINED GNUARMEMB_TOOLCHAIN_PATH)
	set(GNUARMEMB_TOOLCHAIN_PATH /usr)
endif()

if (NOT DEFINED BOARD)
	set(BOARD blu60)
endif()

set_property(GLOBAL PROPERTY CSTD gnu11)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wstrict-prototypes")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration")

list(APPEND BOARD_ROOT ${CMAKE_SOURCE_DIR})

list(APPEND DTS_ROOT ${CMAKE_SOURCE_DIR}/quazi)

list(APPEND ZEPHYR_MODULES
	${CMAKE_SOURCE_DIR}/lib/cmsis
	${CMAKE_SOURCE_DIR}/lib/tinycrypt
	${CMAKE_SOURCE_DIR}/lib/hal_nordic
)

list(APPEND CONF_FILE ${CMAKE_SOURCE_DIR}/quazi/prj.conf)

if (EXISTS ${CMAKE_SOURCE_DIR}/prj.conf)
	list(APPEND CONF_FILE ${CMAKE_SOURCE_DIR}/prj.conf)
endif()

# Find Zephyr. This also loads Zephyr's build system.
find_package(Zephyr REQUIRED HINTS lib/zephyr)

project(quazi_mk)

# Zephyr calls add_subdirectory(${BOARD_DIR})

add_library(qmk)
add_library(glue)
add_library(quazi)

include(qmk.cmake)
add_subdirectory(quazi)

target_link_libraries(qmk PRIVATE
	-Wl,--defsym=quazi_printf=printk
	zephyr_interface)

target_link_libraries(glue PRIVATE
	zephyr_interface
	qmk)

target_link_libraries(quazi PRIVATE
	zephyr_interface
	glue)

target_link_libraries(app PRIVATE
	-Wl,--whole-archive
	qmk quazi
	-Wl,--no-whole-archive)

add_definitions(-DMATRIX_ROWS=5 -DMATRIX_COLS=14)
