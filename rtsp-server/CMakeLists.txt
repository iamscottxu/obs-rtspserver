cmake_minimum_required(VERSION 3.5)
project(rtsp-server)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include_directories("3rdpart/md5")
include_directories("3rdpart/libb64/libb64/include")
add_subdirectory(3rdpart/libb64)

file(GLOB rtsp-server_net_SOURCES
	net/*.cpp)

file(GLOB rtsp-server_xop_SOURCES
	xop/*.cpp)

set(rtsp-server_SOURCES
	${rtsp-server_net_SOURCES}
	${rtsp-server_xop_SOURCES}
	)

file(GLOB rtsp-server_net_HEADERS
	net/*.h)

file(GLOB rtsp-server_xop_HEADERS
	xop/*.h)

set(rtsp-server_HEADERS
	${rtsp-server_net_HEADERS}
	${rtsp-server_xop_HEADERS}
	)

add_library(rtsp-server STATIC
	${rtsp-server_SOURCES}
	${rtsp-server_HEADERS}
	)

target_link_libraries(rtsp-server
	libb64)
