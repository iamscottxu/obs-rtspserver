cmake_minimum_required(VERSION 3.5)
project(obs-rtspserver)

add_subdirectory(rtsp-server)

find_package(FFmpeg COMPONENTS avutil avfilter REQUIRED)

set(CMAKE_PREFIX_PATH "${QTDIR}")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
	
set(obs-rtspserver_SOURCES
	rtsp_main.cpp
	rtspoutput.rc
	rtsp_output.cpp
	rtsp_properties.cpp
	rtsp_output_helper.cpp
	)

set(obs-rtspserver_HEADERS
	rtsp_output.h
	rtsp_properties.h
	rtsp_output_helper.h
	)
	
if(WIN32)
	set(OBS_RTSPSERVER_VERSION_MAJOR 2)
	set(OBS_RTSPSERVER_VERSION_MINOR 0)
	set(OBS_RTSPSERVER_VERSION_PATCH 0)
	set(OBS_RTSPSERVER_VERSION_STRING "2.0.0")
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc)
endif()

if(MSVC)
	set(obs-rtspserver_PLATFORM_DEPS
		w32-pthreads)
endif()

add_library(obs-rtspserver MODULE
	${obs-rtspserver_SOURCES}
	${obs-rtspserver_HEADERS})
	
include_directories(
	${FFMPEG_INCLUDE_DIRS}
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
	"rtsp-server")

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")
		
target_link_libraries(obs-rtspserver
	${FFMPEG_LIBRARIES}
	${obs-rtspserver_PLATFORM_DEPS}
	rtsp-server
	obs-frontend-api
	libobs
	Qt5::Core
	Qt5::Widgets)

if(APPLE)
	set_target_properties(obs-rtspserver
			PROPERTIES
			FOLDER "plugins"
			PRODUCTNAME "OBS RTSP Server Plugin")
else()
	set_target_properties(obs-rtspserver
			PROPERTIES
			FOLDER "plugins"
			VERSION "2.0.0"
			PRODUCTNAME "OBS RTSP Server Plugin")
endif()

install_obs_plugin_with_data(obs-rtspserver data)
