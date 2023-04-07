set(OBS_PLUGIN_OBS_SOURCE_DIR ${OBS_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/external")

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/release")
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${QTDIR};${DepsPath}")

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${OBS_PLUGUN_GIT_TAG}-linux")
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
#set(CPACK_SOURCE_PACKAGE_FILE_NAME "${OBS_PLUGIN_PACKAGE_FILE_NAME}")
set(MACOSX_PLUGIN_GUI_IDENTIFIER "${MACOS_BUNDLEID}")
set(MACOSX_PLUGIN_BUNDLE_VERSION "${OBS_PLUGUN_LONG_VERSION}")
set(MACOSX_PLUGIN_SHORT_VERSION_STRING "${OBS_PLUGUN_VERSION}")

find_package(libobs REQUIRED)
#add_library(OBS::libobs STATIC IMPORTED GLOBAL)
#set_target_properties(OBS::libobs PROPERTIES
#    IMPORTED_LOCATION "${LIBOBS_LIB}"
#    )
add_library(OBS::libobs STATIC IMPORTED GLOBAL)
if (LIBOBS_LIB MATCHES "/([^/]+)\\.framework$")
    set(_libobs_fw "${LIBOBS_LIB}/${CMAKE_MATCH_1}")
    if(EXISTS "${_libobs_fw}.tbd")
        string(APPEND _libobs_fw ".tbd")
    endif()
    message("${_libobs_fw}")
    set_target_properties(OBS::libobs PROPERTIES
        IMPORTED_LOCATION "${_libobs_fw}"
    )
else()
    set_target_properties(OBS::libobs PROPERTIES
        IMPORTED_LOCATION "${LIBOBS_LIB}"
    )
endif()
add_library(libobs ALIAS OBS::libobs)

if(ENABLE_UI)
	find_package(obs-frontend-api REQUIRED)
	add_library(OBS::obs-frontend-api STATIC IMPORTED GLOBAL)
	set_target_properties(OBS::obs-frontend-api PROPERTIES
	    IMPORTED_LOCATION "${OBS_FRONTEND_API_LIB}"
	)
	add_library(obs-frontend-api ALIAS OBS::obs-frontend-api)
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/external/ObsPluginHelpers.cmake")

if(OS_WINDOWS)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /W3)
    endif()
elseif(OS_MACOS)
    configure_file(
		${CMAKE_SOURCE_DIR}/bundle/installer-macos.pkgproj.in
		${CMAKE_SOURCE_DIR}/bundle/installer-macos.generated.pkgproj)

    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()
