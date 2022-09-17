# This module can be copied and used by external plugins for OBS
#
# Once done these will be defined:
#
#  OBS_FRONTEND_API_FOUND
#  OBS_FRONTEND_API_INCLUDE_DIRS
#  OBS_FRONTEND_API_LIBRARIES

find_package(PkgConfig QUIET)
if (PKG_CONFIG_FOUND)
	pkg_check_modules(_OBS QUIET obs obs-frontend-api)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(_lib_suffix 64)
else()
	set(_lib_suffix 32)
endif()

if(DEFINED CMAKE_BUILD_TYPE)
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		set(_build_type_base "debug")
	else()
		set(_build_type_base "release")
	endif()
endif()

find_path(OBS_FRONTEND_API_INCLUDE_DIR
	NAMES obs-frontend-api.h
	HINTS
		ENV OBS_SOURCE_DIR${_lib_suffix}
		ENV OBS_SOURCE_DIR
		${OBS_SOURCE_DIR}
	PATHS
		/usr/include /usr/local/include /opt/local/include /sw/include
	PATH_SUFFIXES
        UI/obs-frontend-api
	)

function(find_obs_lib base_name repo_build_path lib_name)
	string(TOUPPER "${base_name}" base_name_u)

	if(DEFINED _build_type_base)
		set(_build_type_${repo_build_path} "${_build_type_base}/${repo_build_path}")
		set(_build_type_${repo_build_path}${_lib_suffix} "${_build_type_base}${_lib_suffix}/${repo_build_path}")
	endif()

	find_library(${base_name_u}_LIB
		NAMES ${_${base_name_u}_LIBRARIES} ${lib_name} lib${lib_name}
		HINTS
			ENV OBS_SOURCE_DIR${_lib_suffix}
			ENV OBS_SOURCE_DIR
			${OBS_SOURCE_DIR}
			${_${base_name_u}_LIBRARY_DIRS}
		PATHS
			/usr/lib /usr/local/lib /opt/local/lib /sw/lib
		PATH_SUFFIXES
			lib${_lib_suffix} lib
			libs${_lib_suffix} libs
			bin${_lib_suffix} bin
			../lib${_lib_suffix} ../lib
			../libs${_lib_suffix} ../libs
			../bin${_lib_suffix} ../bin
			# base repo non-msvc-specific search paths
			${_build_type_${repo_build_path}}
			${_build_type_${repo_build_path}${_lib_suffix}}
			plugin_build/${repo_build_path}
			plugin_build${_lib_suffix}/${repo_build_path}
			# base repo msvc-specific search paths on windows
			plugin_build${_lib_suffix}/${repo_build_path}/Debug
			plugin_build${_lib_suffix}/${repo_build_path}/RelWithDebInfo
			plugin_build/${repo_build_path}/Debug
			plugin_build/${repo_build_path}/RelWithDebInfo
		)
endfunction()

find_obs_lib(OBS_FRONTEND_API UI/obs-frontend-api obs-frontend-api)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(obs-frontend-api DEFAULT_MSG OBS_FRONTEND_API_LIB OBS_FRONTEND_API_INCLUDE_DIR)
mark_as_advanced(OBS_FRONTEND_API_INCLUDE_DIR OBS_FRONTEND_API_LIB)

if(OBS-FRONTEND-API_FOUND)
	set(OBS_FRONTEND_API_INCLUDE_DIRS ${OBS_FRONTEND_API_INCLUDE_DIR})
	set(OBS_FRONTEND_API_LIBRARIES ${OBS_FRONTEND_API_LIB})
	# include(${OBS_FRONTEND_API_INCLUDE_DIR}/../../cmake/external/ObsPluginHelpers.cmake)

	# allows external plugins to easily use/share common dependencies that are often included with obs-frontend-api (such as FFmpeg)
	if(NOT DEFINED INCLUDED_OBS_FRONTEND_API_CMAKE_MODULES)
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${OBS_FRONTEND_API_INCLUDE_DIR}/../../cmake/Modules/")
		set(INCLUDED_OBS_FRONTEND_API_CMAKE_MODULES true)
	endif()
else()
	message(FATAL_ERROR "Could not find the obs-frontend-api library" )
endif()
