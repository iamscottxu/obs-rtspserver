set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(OBS_PLUGIN_OBS_SOURCE_DIR ${OBS_SOURCE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/external")

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/release")

if(OS_LINUX)
	set(CPACK_RPM_PACKAGE_LICENSE "GPL-2.0")
    set(CPACK_SOURCE_PACKAGE_FILE_NAME "${OBS_PLUGIN_PACKAGE_FILE_NAME}")
endif()

find_package(obs-frontend-api REQUIRED)
add_library(OBS::obs-frontend-api STATIC IMPORTED GLOBAL)
set_target_properties(OBS::obs-frontend-api PROPERTIES
    IMPORTED_LOCATION "${OBS_FRONTEND_API_LIB}"
    )
add_library(obs-frontend-api ALIAS OBS::obs-frontend-api)

find_package(libobs REQUIRED)
add_library(OBS::libobs STATIC IMPORTED GLOBAL)
set_target_properties(OBS::libobs PROPERTIES
    IMPORTED_LOCATION "${LIBOBS_LIB}"
    )
add_library(libobs ALIAS OBS::libobs)

include("${CMAKE_CURRENT_SOURCE_DIR}/external/ObsPluginHelpers.cmake")

function(install_obs_plugin_with_data target data)
    setup_plugin_target(${target} ${data})
endfunction(install_obs_plugin_with_data)
