cmake_minimum_required(VERSION 3.5)
project(obs-rtspserver)

add_subdirectory(rtsp-server)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(OBS_PLUGIN_OBS_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(OBS_FRONTEND_API_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")

if (NOT COMMAND install_obs_plugin_with_data)
    include(external/BuildHelper.cmake)
endif()
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(OBS_RTSPSERVER_SOURCES
	rtsp_main.cpp
	rtspoutput.rc
	rtsp_output.cpp
	rtsp_properties.cpp
	rtsp_output_helper.cpp
	)

set(OBS_RTSPSERVER_HEADERS
	rtsp_output.h
	rtsp_properties.h
	rtsp_output_helper.h
	)

add_library(obs-rtspserver MODULE
	${OBS_RTSPSERVER_SOURCES}
	${OBS_RTSPSERVER_HEADERS})
	
target_include_directories(obs-rtspserver PRIVATE
	${Qt5Core_INCLUDES}
	${Qt5Widgets_INCLUDES}
	${OBS_FRONTEND_API_INCLUDE_DIRS}
	${LIBOBS_INCLUDE_DIRS}
	"rtsp-server")

target_link_libraries(obs-rtspserver PRIVATE
	rtsp-server
	obs-frontend-api
	libobs
	Qt5::Core
	Qt5::Widgets)

set(OBS_RTSPSERVER_VERSION_MAJOR 2)
set(OBS_RTSPSERVER_VERSION_MINOR 2)
set(OBS_RTSPSERVER_VERSION_PATCH 0)
set(OBS_RTSPSERVER_VERSION_STRING "${OBS_RTSPSERVER_VERSION_MAJOR}.${OBS_RTSPSERVER_VERSION_MINOR}.${OBS_RTSPSERVER_VERSION_PATCH}")
add_definitions(-DVERSION_STRING="${OBS_RTSPSERVER_VERSION_STRING}")
		
if(WIN32)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc)
endif()

if(APPLE)
	set_target_properties(obs-rtspserver
			PROPERTIES
			FOLDER "plugins"
			PRODUCTNAME "OBS RTSP Server Plugin")
else()
	set_target_properties(obs-rtspserver
			PROPERTIES
			FOLDER "plugins"
			VERSION "${OBS_RTSPSERVER_VERSION_STRING}"
			PRODUCTNAME "OBS RTSP Server Plugin")
endif()

install_obs_plugin_with_data(obs-rtspserver data)

