cmake_minimum_required(VERSION 3.5)

include(external/GitInfoHelper.cmake)
get_git_version(OBS_PLUGUN_GIT_TAG OBS_PLUGUIN_VERSION OBS_PLUGUIN_SHORT_VERSION OBS_PLUGUIN_LONG_VERSION)
project(obs-rtspserver VERSION ${OBS_PLUGUIN_LONG_VERSION} 
	HOMEPAGE_URL "https://obsproject.com/forum/resources/obs-rtspserver.1037"
	DESCRIPTION "RTSP server plugin for obs-studio")
set(LINUX_MAINTAINER_EMAIL "scottxu@scottxublog.com")
set(LINUX_MAINTAINER "Scott Xu")
set(MACOS_BUNDLEID "com.scottxu.obs-rtspserver")

set(OBS_PLUGIN_OBS_SOURCE_DIR ${CMAKE_SOURCE_DIR})
set(OBS_FRONTEND_API_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/UI/obs-frontend-api")

if (NOT COMMAND install_obs_plugin_with_data)
    include(external/BuildHelper.cmake)
endif()

add_subdirectory(rtsp-server)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

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
	Qt6::Core
	Qt6::Widgets)

add_definitions(-DVERSION_STRING="${OBS_PLUGUIN_VERSION}")
		
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
			VERSION "${OBS_PLUGUIN_VERSION}"
			PRODUCTNAME "OBS RTSP Server Plugin")
endif()

install_obs_plugin_with_data(obs-rtspserver data)

