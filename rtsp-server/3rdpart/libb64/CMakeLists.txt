cmake_minimum_required(VERSION 3.5)
project(libb64)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include_directories("libb64/include")

file(GLOB libb64_SOURCES
	libb64/src/*.c)

file(GLOB libb64_HEADERS
	libb64/include/b64/*.h)

add_library(libb64 STATIC
	${libb64_SOURCES}
	${libb64_HEADERS})
