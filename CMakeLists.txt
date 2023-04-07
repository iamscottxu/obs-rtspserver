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

add_library(${PROJECT_NAME} MODULE)

if (NOT COMMAND setup_plugin_target)
    include(external/BuildHelper.cmake)
endif()

add_subdirectory(rtsp-server)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)

set(OBS_RTSPSERVER_SOURCES
	rtsp_main.cpp
	rtsp_output.cpp
	rtsp_properties.cpp
	rtsp_output_helper.cpp
	)

set(OBS_RTSPSERVER_HEADERS
	rtsp_output.h
	rtsp_properties.h
	rtsp_output_helper.h
	)

 file(GLOB OBS_RTSPSERVER_UI_SOURCES
	ui/*.cpp)

file(GLOB OBS_RTSPSERVER_MAIN_SOURCES
	*.cpp)

set(OBS_RTSPSERVER_SOURCES
	${OBS_RTSPSERVER_UI_SOURCES}
	${OBS_RTSPSERVER_MAIN_SOURCES}
	)

file(GLOB OBS_RTSPSERVER_UI_HEADERS
	ui/*.hpp)

file(GLOB OBS_RTSPSERVER_MAIN_HEADERS
	*.h)

set(OBS_RTSPSERVER_HEADERS
	${OBS_RTSPSERVER_UI_HEADERS}
	${OBS_RTSPSERVER_MAIN_HEADERS}
	)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY C_STANDARD 17)

if(ENABLE_UI)
  find_qt(COMPONENTS Widgets Core)
  set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)

  add_definitions(-DOBS_RTSPSERVER_ENABLE_UI)
endif()

target_sources(${PROJECT_NAME} PRIVATE
	${OBS_RTSPSERVER_SOURCES}
	${OBS_RTSPSERVER_HEADERS})

if(WIN32)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc.in ${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc)
	target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rtspoutput.rc)
endif()
	
target_include_directories(${PROJECT_NAME} PRIVATE
	${LIBOBS_INCLUDE_DIRS}
	"rtsp-server")

target_link_libraries(${PROJECT_NAME} PRIVATE
	rtsp-server
	libobs)

if(ENABLE_UI)
  # include all header files that are only relevant for OBS Studio
	target_include_directories(${PROJECT_NAME} PRIVATE
	    ${OBS_FRONTEND_API_INCLUDE_DIRS}
  )

  # link all obs frontend libraries
	target_link_libraries(${PROJECT_NAME} PRIVATE
		  obs-frontend-api
		  Qt::Core
		  Qt::Widgets
  )
endif()

add_definitions(-DVERSION_STRING="${OBS_PLUGUIN_VERSION}")

if(APPLE)
	set_target_properties(${PROJECT_NAME}
			PROPERTIES
			FOLDER "plugins"
			PRODUCTNAME "OBS RTSP Server Plugin")
else()
	set_target_properties(${PROJECT_NAME}
			PROPERTIES
			FOLDER "plugins"
			VERSION "${OBS_PLUGUIN_VERSION}"
			PRODUCTNAME "OBS RTSP Server Plugin")
endif()

setup_plugin_target(${PROJECT_NAME})

