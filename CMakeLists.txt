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

#list(APPEND CMAKE_C_FLAGS "-Wstrict-prototypes")

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

zephyr_library_named(qmk)
include(qmk.cmake)

zephyr_library_named(quazi)
add_subdirectory(quazi)

# CMake doesn't like targets with no sources
target_sources(app PRIVATE quazi/null.c)

#target_link_libraries(qmk PRIVATE quazi)
target_link_libraries(quazi PRIVATE qmk)
target_link_libraries(app PRIVATE qmk quazi)

add_definitions(-Dxprintf=printf -Dprint=printf)
add_definitions(-DMATRIX_ROWS=5 -DMATRIX_COLS=14)
