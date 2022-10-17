

function(get_git_version git_tag_name git_tag_version_name git_tag_short_version_name git_tag_long_version_name)
	execute_process(COMMAND git describe --tags --always --dirty=-dev
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		TIMEOUT 10
		OUTPUT_VARIABLE git_tag
		OUTPUT_STRIP_TRAILING_WHITESPACE)
    
	string(REGEX MATCH "[0-9]+.[0-9]+.[0-9]+(-[a-z0-9]+)*$" git_tag_version "${git_tag}")
	string(REGEX MATCH "^[0-9]+.[0-9]+.[0-9]+"  git_tag_short_version "${git_tag_version}")

	if("${git_tag_version}" MATCHES "-[0-9]+-g")
		string(REGEX MATCH "-[0-9]+-g"  _git_tag_tweak_version_temp "${git_tag_version}")
		string(REGEX MATCH "[0-9]+"  _git_tag_tweak_version "${_git_tag_tweak_version_temp}")
	else()
		set(_git_tag_tweak_version "0")
	endif()

	if("${git_tag_short_version}" STREQUAL "")
		set(git_tag_long_version "0.0.1.${_git_tag_tweak_version}")
	else()
		set(git_tag_long_version "${git_tag_short_version}.${_git_tag_tweak_version}")
	endif()

	set(${git_tag_name} "${git_tag}" PARENT_SCOPE)
	set(${git_tag_version_name} "${git_tag_version}" PARENT_SCOPE)
	set(${git_tag_short_version_name} "${git_tag_short_version}" PARENT_SCOPE)
	set(${git_tag_long_version_name} "${git_tag_long_version}" PARENT_SCOPE)

	message("Git Tag:\t${git_tag}")
	message("Git Tag Version:\t${git_tag_version}")
	message("Git Tag Short Version:\t${git_tag_short_version}")
	message("Git Tag Long Version:\t${git_tag_long_version}")
endfunction(get_git_version)
