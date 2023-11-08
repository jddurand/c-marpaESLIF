include(CheckSymbolExists)
include(CTest)
include(GNUInstallDirs)
include(CheckSymbolExists)
include(GenerateExportHeader)

# ====================================================================
# _cmake_all_in_one_pretty_print
# ====================================================================
function(_cmake_all_in_one_pretty_print output)
  set(_pretty_print)
  #
  # Try to discover keywords, that are assumed to start with a least
  # three capital letters
  #
  set(_prev)
  set(_first)
  set(_newline_inserted)
  foreach(_arg ${ARGN})
    if(NOT _pretty_print)
      set(_first ${_arg})
      list(APPEND _pretty_print ${_arg})
    else()
      if((_arg STREQUAL "(") OR (_prev STREQUAL "("))
        list(APPEND _pretty_print ${_arg})
      elseif(_arg STREQUAL ")")
        if(_newline_inserted)
          list(APPEND _pretty_print "\n${_arg}")
        else()
          list(APPEND _pretty_print ${_arg})
        endif()
      else()
        string(REGEX MATCH "^[A-Z][A-Z][A-Z]" _match ${_arg})
        if(_match)
          #
          # Special cases where it looks like a keyword but it is not
          #
          string(REGEX MATCH "^target_sources" _target_sources ${_first})
          string(REGEX MATCH "^install" _install ${_first})
          if(_target_sources AND (_prev STREQUAL "TYPE"))
            list(APPEND _pretty_print " ${_arg}")
          elseif(_install AND (_prev STREQUAL "INCLUDES"))
            list(APPEND _pretty_print " ${_arg}")
          else()
            list(APPEND _pretty_print "\n  ${_arg}")
            set(_newline_inserted TRUE)
          endif()
        else()
          list(APPEND _pretty_print " ${_arg}")
        endif()
      endif()
      set(_prev ${_arg})
    endif()
  endforeach()

  set(${output} ${_pretty_print} PARENT_SCOPE)
endfunction()

# ====================================================================
# _cmake_all_in_one_log
# ====================================================================
function(_cmake_all_in_one_log)
  if($ENV{CMAKE_ALL_IN_ONE_DEBUG})
    list(JOIN ARGN " " _argn)
    message(STATUS "[cmake_all_in_one/${_cmake_all_in_one_base_name}] ${_argn}")
  endif()
endfunction()

# ====================================================================
# _cmake_all_in_one_output
# ====================================================================
function(_cmake_all_in_one_output)
  if(_cmake_all_in_one_cmake_output_file)
    _cmake_all_in_one_pretty_print(_pretty_print ${ARGN})
    file(APPEND ${_cmake_all_in_one_cmake_output_file} ${_pretty_print})
  endif()
endfunction()

# ====================================================================
# _cmake_all_in_one_command_wrapper
# ====================================================================
function(_cmake_all_in_one_command_wrapper command)
  set(_argn ${ARGN})
  _cmake_all_in_one_log("${command}(${_argn})")
  _cmake_all_in_one_output("${command}" "(" "${_argn}" ")" "\n")
  cmake_language(CALL ${command} ${_argn})
endfunction()

# ====================================================================
# _cmake_all_in_one_match_regexes
# ====================================================================
function(_cmake_all_in_one_match_regexes value match_regexes default_value output)

  if(match_regexes)
    set(_matched FALSE)
    foreach(_match_regex ${match_regexes})
      string(REGEX MATCH ${_match_regex} _output ${value})
      if(_output)
        set(_matched TRUE)
        break()
      endif()
    endforeach()
  else()
    set(_matched ${default_value})
  endif()

  set(${output} ${_matched} PARENT_SCOPE)
endfunction()

# ====================================================================
# _cmake_all_in_one_match
# ====================================================================
function(_cmake_all_in_one_match value accept_regexes reject_regexes output_var)
  _cmake_all_in_one_match_regexes(${value} "${accept_regexes}" TRUE _accept_value)
  _cmake_all_in_one_match_regexes(${value} "${reject_regexes}" FALSE _reject_value )

  if (_accept_value AND (NOT _reject_value))
    set(_matched TRUE)
  else()
    set(_matched FALSE)
  endif()

  set(${output_var} ${_matched} PARENT_SCOPE)
endfunction()

# ====================================================================
# _cmake_all_in_one_find_files
# ====================================================================
function(_cmake_all_in_one_find_files accept_type output_var)
  if (_cmake_all_in_one_${accept_type}_auto AND _cmake_all_in_one_${accept_type}_base_dirs AND _cmake_all_in_one_${accept_type}_auto_extensions)
    set(_reject_types)
    foreach(_reject_type ${ARGN})
      list(APPEND _reject_types ${_reject_type})
    endforeach()

    _cmake_all_in_one_log("Auto-discovering ${accept_type}, rejections: ${_reject_types}")

    set(_found_files)
    foreach(_dir ${_cmake_all_in_one_${accept_type}_base_dirs})
      set(_globbing_expressions)
      _cmake_all_in_one_log("... Directory ${_dir}, glob: ${_cmake_all_in_one_${accept_type}_auto_extensions}")
      foreach(_extension ${_cmake_all_in_one_${accept_type}_auto_extensions})
        list(APPEND _globbing_expressions ${_dir}/${_extension})
      endforeach()
      file(GLOB_RECURSE _files ${_globbing_expressions})
      set(_dir_found_files)
      foreach(_file ${_files})
        #
        # Get category, if any
        #
        cmake_path(RELATIVE_PATH _file BASE_DIRECTORY ${_dir} OUTPUT_VARIABLE _relpath)
        #
        # Priority is given to the reject types
        #
        set(_matched FALSE)
        foreach(_reject_type ${_reject_types})
          _cmake_all_in_one_match(${_relpath} "${_cmake_all_in_one_${_reject_type}_auto_relpath_accept_regexes}" "${_cmake_all_in_one_${_reject_type}_auto_relpath_reject_regexes}" _matched)
          if(_matched)
            break()
          endif()
        endforeach()

        if(_matched)
          continue()
        endif()
        #
        # Then try with our accept regexes
        #
        _cmake_all_in_one_match(${_relpath} "${_cmake_all_in_one_${accept_type}_auto_relpath_accept_regexes}" "${_cmake_all_in_one_${accept_type}_auto_relpath_reject_regexes}" _matched)
        if(_matched)
          _cmake_all_in_one_log("... ... ${_relpath}")
	  list(APPEND _dir_found_files ${_file})
        endif()
      endforeach()
      #
      # Aggregate this files in a source group - validation ensured that _dir is a real directory
      #
      get_filename_component(_dir_name ${_dir} NAME)
      _cmake_all_in_one_command_wrapper(source_group TREE ${_dir} PREFIX ${_dir_name} FILES ${_dir_found_files})
      #
      # Remember we generated a source group
      #
      set(_cmake_all_in_one_${accept_type}_auto_source_group TRUE PARENT_SCOPE)
      #
      # Append files found for this directory
      #
      list(APPEND _found_files ${_dir_found_files})
    endforeach()

    set(${output_var} ${_found_files} PARENT_SCOPE)
  endif()
endfunction()

# ====================================================================
# _cmake_all_in_one_init
# ====================================================================
function(_cmake_all_in_one_init)
  #
  # We need BUILD_LOCAL_INTERFACE
  #
  if(CMAKE_VERSION VERSION_LESS 3.26)
    message(FATAL_ERROR "Version 3.26 at least is required")
  endif()
  #
  # Policies
  #
  foreach(_policy) # CMP0018 CMP0063 CMP0075 CMP0057)
    if(POLICY ${_policy})
      _cmake_all_in_one_command_wrapper(cmake_policy SET ${_policy} NEW)
    endif()
  endforeach()
  #
  # Compiler settings
  #
  foreach(_language ${_cmake_all_in_one_languages})
    _cmake_all_in_one_command_wrapper(set CMAKE_${language}_VISIBILITY_PRESET hidden)
  endforeach()
  _cmake_all_in_one_command_wrapper(set CMAKE_POSITION_INDEPENDENT_CODE ON)
  _cmake_all_in_one_command_wrapper(set CMAKE_VISIBILITY_INLINES_HIDDEN 1)
  if(MSVC)
    _cmake_all_in_one_command_wrapper(add_definitions -D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
    _cmake_all_in_one_command_wrapper(add_definitions -D_SCL_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_WARNINGS)
    _cmake_all_in_one_command_wrapper(add_definitions -DWIN32_LEAN_AND_MEAN -DVC_EXTRALEAN)
  endif()
endfunction()

# ====================================================================
# _cmake_all_in_one_pods_process
# ====================================================================
function(_cmake_all_in_one_pods_process)
  _cmake_all_in_one_log("Processing pods")
  if(NOT TARGET man)
    _cmake_all_in_one_command_wrapper(add_custom_target man)
    _cmake_all_in_one_command_wrapper(install CODE "execute_process(COMMAND \${CMAKE_MAKE_PROGRAM} man)")
  endif()
  foreach(pod ${ARGN})
    _cmake_all_in_one_log("... ${pod}")
    get_filename_component(_pod_absolute ${pod} ABSOLUTE)
    get_filename_component(_pod_name_wle ${pod} NAME_WLE)
    #
    # Determine the section
    #
    string(REGEX MATCH "[0-9]+$" _section ${pod})
    if(NOT _section)
      set(_section ${_cmake_all_in_one_pods_auto_section})
    endif()
    set(_target ${_pod_name_wle})
    string(REGEX REPLACE "[^a-zA-Z0-9_]" "_" _target ${_target})
    if(NOT TARGET ${_target})
      set(_output ${_cmake_all_in_one_output_dir}/${CMAKE_INSTALL_MANDIR}/${_target}.${_section})
      #
      # Pod2man
      #
      set(_pod2man_args
        --section ${_section}
        --center \"${_cmake_all_in_one_base_name}\"
        --stderr
        --name \"${_pod_name_wle}\"
      )
      if(_cmake_all_in_one_version_major)
        list(APPEND _pod2man_args -r \"${_cmake_all_in_one_version_major}\")
      endif()
      list(APPEND _pod2man_args \"${_output}\")
      _cmake_all_in_one_command_wrapper(add_custom_command
        OUTPUT ${_output}
        MAIN_DEPENDENCY ${_pod_absolute}
	DEPENDS ${_cmake_all_in_one_pod2man}
	COMMAND ${_cmake_all_in_one_pod2man}
	ARGS ${_pod2man_args}
        VERBATIM
        USES_TERMINAL
      )
      #
      # Gzip
      #
      set(_gzip_args < \"${_output}\" > \"${_output}.gz\")
      _cmake_all_in_one_command_wrapper(add_custom_command
        OUTPUT ${_output}.gz
	MAIN_DEPENDENCY ${_output}
	DEPENDS ${_cmake_all_in_one_gzip}
	COMMAND ${_cmake_all_in_one_gzip}
	ARGS ${_gzip_args}
        VERBATIM
        USES_TERMINAL
      )
      _cmake_all_in_one_command_wrapper(add_custom_target ${_target} DEPENDS ${_output}.gz)
      _cmake_all_in_one_command_wrapper(add_dependencies man ${_target})
      _cmake_all_in_one_command_wrapper(install FILES ${_output}.gz DESTINATION ${CMAKE_INSTALL_MANDIR}/man${_section} COMPONENT ManpageComponent)
      #
      # JDD CPACK_INSTALL_SCRIPT_MAN
      #
    else()
      message(WARNING "${pod}: Target \"${_target}\" already created")
    endif()
  endforeach()
endfunction()

# ====================================================================
# _cmake_all_in_one_process
# ====================================================================
function(_cmake_all_in_one_process)
  #
  # Initialize output
  #
  if(_cmake_all_in_one_cmake_output_file)
    file(WRITE ${_cmake_all_in_one_cmake_output_file} "")
  endif()
  #
  # Initializations
  #
  _cmake_all_in_one_init()
  #
  # Always create an interface
  #
  _cmake_all_in_one_output("#\n")
  _cmake_all_in_one_output("# Interface\n")
  _cmake_all_in_one_output("#\n")
  _cmake_all_in_one_command_wrapper(add_library ${_cmake_all_in_one_iface_name} INTERFACE)
  _cmake_all_in_one_command_wrapper(target_include_directories ${_cmake_all_in_one_iface_name} INTERFACE $<BUILD_LOCAL_INTERFACE:${_cmake_all_in_one_output_dir}/${CMAKE_INSTALL_INCLUDEDIR}>)
  _cmake_all_in_one_command_wrapper(target_include_directories ${_cmake_all_in_one_iface_name} INTERFACE $<BUILD_INTERFACE:${_cmake_all_in_one_output_dir}/${CMAKE_INSTALL_INCLUDEDIR}>)
  _cmake_all_in_one_command_wrapper(target_include_directories ${_cmake_all_in_one_iface_name} INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  #
  # Interface headers
  #
  if(NOT _cmake_all_in_one_public_headers)
    _cmake_all_in_one_find_files(public_headers _cmake_all_in_one_public_headers private_headers)
  endif()
  if(NOT _cmake_all_in_one_private_headers)
    _cmake_all_in_one_find_files(private_headers _cmake_all_in_one_private_headers)
  endif()
  foreach(_type public private)
    string(TOUPPER "${_type}" _category)
    if(_cmake_all_in_one_${_type}_headers)
      _cmake_all_in_one_output("#\n")
      _cmake_all_in_one_output("# Interface ${_type} headers\n")
      _cmake_all_in_one_output("#\n")
      _cmake_all_in_one_command_wrapper(target_sources ${_cmake_all_in_one_iface_name} ${_category} FILE_SET ${_type}_headers BASE_DIRS ${_cmake_all_in_one_${_type}_headers_base_dirs} TYPE HEADERS FILES ${_cmake_all_in_one_${_type}_headers})
    endif()
    if (_cmake_all_in_one_${_type}_headers_base_dirs)
      _cmake_all_in_one_command_wrapper(target_include_directories ${_cmake_all_in_one_iface_name} INTERFACE $<BUILD_LOCAL_INTERFACE:${_cmake_all_in_one_${_type}_headers_base_dirs}>)
    endif()
  endforeach()
  #
  # Interface pod
  #
  if(_cmake_all_in_one_pod2man AND _cmake_all_in_one_gzip)
    if(NOT _cmake_all_in_one_pods)
      _cmake_all_in_one_find_files(pods _cmake_all_in_one_pods)
    endif()
    if(_cmake_all_in_one_pods)
      _cmake_all_in_one_output("#\n")
      _cmake_all_in_one_output("# Interface pods\n")
      _cmake_all_in_one_output("#\n")
      _cmake_all_in_one_pods_process(${_cmake_all_in_one_pods})
    endif()
  endif()
  #
  # Interface install
  #
  if(_cmake_all_in_one_public_headers)
    #
    # Set and propagate to caller _cmake_all_in_one_have_header_component variable
    #
    _cmake_all_in_one_output("#\n")
    _cmake_all_in_one_output("# Interface install\n")
    _cmake_all_in_one_output("#\n")
    set(_cmake_all_in_one_have_header_component TRUE PARENT_SCOPE)
    _cmake_all_in_one_command_wrapper(install TARGETS ${_cmake_all_in_one_iface_name} EXPORT ${_cmake_all_in_one_base_name}-targets FILE_SET public_headers INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT HeaderComponent)
  endif()
  #
  # Sources
  #
  foreach(_type shared static module)
    if(NOT _cmake_all_in_one_${_type}_sources)
      _cmake_all_in_one_find_files(${_type}_sources _cmake_all_in_one_${_type}_sources)
    endif()
    if(_cmake_all_in_one_${_type}_sources)
      string(TOUPPER "${_type}" _category)
      set(_cmake_all_in_one_have_${_type}_component TRUE PARENT_SCOPE)
      _cmake_all_in_one_command_wrapper(add_library ${_cmake_all_in_one_${_type}_name} ${_category} ${_cmake_all_in_one_${_type}_sources})
      if(_type STREQUAL "shared" AND _cmake_all_in_one_export)
        #
        # Generate an export header
        #
	set(_args)
	if(NOT _cmake_all_in_one_export_file_name)
	  set(_cmake_all_in_one_export_file_name ${_cmake_all_in_one_${_type}_name}_export.h) # CMake default
	endif()
	if(_cmake_all_in_one_export_macro_name)
	  list(APPEND _args EXPORT_MACRO_NAME ${_cmake_all_in_one_export_macro_name})
	endif()
	if(_cmake_all_in_one_static_define)
	  list(APPEND _args STATIC_DEFINE ${_cmake_all_in_one_static_define})
	endif()
	set(_export_file_name ${_cmake_all_in_one_output_dir}/${CMAKE_INSTALL_INCLUDEDIR}/${_cmake_all_in_one_export_file_name})
        _cmake_all_in_one_command_wrapper(generate_export_header ${_cmake_all_in_one_${_type}_name} BASE_NAME ${_cmake_all_in_one_base_name} EXPORT_FILE_NAME ${_export_file_name} ${_args})
	#
	# Add it to the interface
	#
	_cmake_all_in_one_command_wrapper(target_sources ${_cmake_all_in_one_iface_name} PUBLIC FILE_SET public_headers BASE_DIRS ${_cmake_all_in_one_output_dir} TYPE HEADERS FILES ${_export_file_name})
	#
	# Make sure the directory where we generated it is public and is in the interface
	#
	get_filename_component(_dir ${_export_file_name} DIRECTORY)
	_cmake_all_in_one_command_wrapper(target_include_directories ${_cmake_all_in_one_iface_name} INTERFACE $<BUILD_LOCAL_INTERFACE:${_dir}>)
	_cmake_all_in_one_command_wrapper(target_include_directories ${_cmake_all_in_one_iface_name} INTERFACE $<BUILD_INTERFACE:${_dir}>)
	#
	# If we generated a source group for public headers, add it
	#
	if(_cmake_all_in_one_public_headers_auto_source_group)
	  get_filename_component(_dir_name ${_cmake_all_in_one_output_dir} NAME)
	  _cmake_all_in_one_command_wrapper(source_group TREE ${_cmake_all_in_one_output_dir} PREFIX ${_dir_name} FILES ${_export_file_name})
	endif()
        #
        # Special definition saying that we are building this library
        #
        _cmake_all_in_one_command_wrapper(target_compile_definitions ${_cmake_all_in_one_${_type}_name} PRIVATE -D${_cmake_all_in_one_base_name}_EXPORTS)
        #
        # Some properties specific to a shared library
        #
        _cmake_all_in_one_command_wrapper(set_target_properties ${_cmake_all_in_one_${_type}_name} PROPERTIES VERSION ${_cmake_all_in_one_version} SOVERSION ${_cmake_all_in_one_version_major})
      endif()
      #
      # Output name
      #
      _cmake_all_in_one_command_wrapper(set_target_properties ${_cmake_all_in_one_${_type}_name} PROPERTIES OUTPUT_NAME ${_cmake_all_in_one_${_type}_output_name})
      #
      # It always depend on the interface
      #
      _cmake_all_in_one_command_wrapper(target_link_libraries ${_cmake_all_in_one_${_type}_name} ${_cmake_all_in_one_iface_name})
    endif()
  endforeach()
  #
  # Check math library
  #
  _cmake_all_in_one_command_wrapper(find_library MATH_LIB m)
  _cmake_all_in_one_log("Found MATH_LIB: ${MATH_LIB}")
endfunction()

# ====================================================================
# _cmake_all_in_one_log_config
# ====================================================================
function(_cmake_all_in_one_log_config)
  if($ENV{CMAKE_ALL_IN_ONE_DEBUG})
    foreach(_variable ${ARGN})
      string(TOLOWER "${_variable}" _variable)
      _cmake_all_in_one_log("Config: ${_variable}=${_cmake_all_in_one_${_variable}}")
    endforeach()
  endif()
endfunction()

# ====================================================================
# cmake_all_in_one
# ====================================================================
function(cmake_all_in_one)
  #
  # Policy push
  #
  if($ENV{CMAKE_ALL_IN_ONE_DEBUG})
    message(STATUS "[cmake_all_in_one] cmake_policy(PUSH)")
  endif()
  cmake_policy(PUSH)
  #
  # Fixups
  #
  if(NOT CMAKE_INSTALL_MANDIR)
    set(CMAKE_INSTALL_MANDIR "man")
  endif()
  if(NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR "include")
  endif()
  #
  # Arguments
  #
  set(options)
  set(oneValueArgs
    SOURCE_DIR
    OUTPUT_DIR
    CMAKE_OUTPUT_FILE
    SHARED_SOURCES_AUTO
    STATIC_SOURCES_AUTO
    MODULE_SOURCES_AUTO
    PUBLIC_HEADERS_AUTO
    PRIVATE_HEADERS_AUTO
    POD2MAN
    GZIP
    PODS_AUTO
    VERSION
    VERSION_MAJOR
    BASE_NAME
    IFACE_NAME
    STATIC_NAME
    STATIC_OUTPUT_NAME
    STATIC_DEFINE
    SHARED_NAME
    SHARED_OUTPUT_NAME
    MODULE_NAME
    MODULE_OUTPUT_NAME
    EXPORT
    EXPORT_FILE_NAME
    EXPORT_MACRO_NAME
    LANGUAGES
  )
  set(multiValueArgs
    SHARED_SOURCES
    SHARED_SOURCES_BASE_DIRS
    SHARED_SOURCES_AUTO_EXTENSIONS
    SHARED_SOURCES_AUTO_RELPATH_ACCEPT_REGEXES
    SHARED_SOURCES_AUTO_RELPATH_REJECT_REGEXES
    STATIC_SOURCES
    STATIC_SOURCES_BASE_DIRS
    STATIC_SOURCES_AUTO_EXTENSIONS
    STATIC_SOURCES_AUTO_RELPATH_ACCEPT_REGEXES
    STATIC_SOURCES_AUTO_RELPATH_REJECT_REGEXES
    MODULE_SOURCES
    MODULE_SOURCES_BASE_DIRS
    MODULE_SOURCES_AUTO_EXTENSIONS
    MODULE_SOURCES_AUTO_RELPATH_ACCEPT_REGEXES
    MODULE_SOURCES_AUTO_RELPATH_REJECT_REGEXES
    PUBLIC_HEADERS
    PUBLIC_HEADERS_BASE_DIRS
    PUBLIC_HEADERS_AUTO_EXTENSIONS
    PUBLIC_HEADERS_AUTO_RELPATH_ACCEPT_REGEXES
    PUBLIC_HEADERS_AUTO_RELPATH_REJECT_REGEXES
    PRIVATE_HEADERS
    PRIVATE_HEADERS_BASE_DIRS
    PRIVATE_HEADERS_AUTO_EXTENSIONS
    PRIVATE_HEADERS_AUTO_RELPATH_ACCEPT_REGEXES
    PRIVATE_HEADERS_AUTO_RELPATH_REJECT_REGEXES
    PODS
    PODS_BASE_DIRS
    PODS_AUTO_EXTENSIONS
    PODS_AUTO_RELPATH_ACCEPT_REGEXES
    PODS_AUTO_RELPATH_REJECT_REGEXES
    PODS_AUTO_SECTION
    PODS_AUTO_VERSION
  )
  #
  # Arguments default values
  #
  # options
  #
  # oneValueArgs
  #
  set(_cmake_all_in_one_source_dir ${CMAKE_CURRENT_SOURCE_DIR})
  set(_cmake_all_in_one_output_dir ${CMAKE_CURRENT_BINARY_DIR}/output)
  set(_cmake_all_in_one_cmake_output_file ${_cmake_all_in_one_output_dir}/cmake/all_in_one.cmake)
  set(_cmake_all_in_one_shared_sources_auto TRUE)
  set(_cmake_all_in_one_static_sources_auto TRUE)
  set(_cmake_all_in_one_module_sources_auto TRUE)
  set(_cmake_all_in_one_public_headers_auto TRUE)
  set(_cmake_all_in_one_private_headers_auto TRUE)
  find_program(CMAKE_ALL_IN_ONE_PROGRAM_POD2MAN pod2man)
  set(_cmake_all_in_one_pod2man ${CMAKE_ALL_IN_ONE_PROGRAM_POD2MAN})
  find_program(CMAKE_ALL_IN_ONE_PROGRAM_GZIP gzip)
  set(_cmake_all_in_one_gzip ${CMAKE_ALL_IN_ONE_PROGRAM_GZIP})
  set(_cmake_all_in_one_pods_auto TRUE)
  if(DEFINED CMAKE_PROJECT_VERSION)
    set(_cmake_all_in_one_version ${CMAKE_PROJECT_VERSION})
  else()
    message(FATAL_ERROR "Missing CMAKE_PROJECT_VERSION")
  endif()
  if(DEFINED CMAKE_PROJECT_VERSION_MAJOR)
    set(_cmake_all_in_one_version_major ${CMAKE_PROJECT_VERSION_MAJOR})
  else()
    message(FATAL_ERROR "Missing CMAKE_PROJECT_VERSION_MAJOR")
  endif()
  if(DEFINED CMAKE_PROJECT_NAME)
    set(_cmake_all_in_one_base_name ${CMAKE_PROJECT_NAME})
  else()
    message(FATAL_ERROR "Missing CMAKE_PROJECT_NAME")
  endif()
  set(_cmake_all_in_one_iface_name         ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major}_iface)

  set(_cmake_all_in_one_shared_name        ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major}_shared)
  set(_cmake_all_in_one_shared_output_name ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major})

  set(_cmake_all_in_one_static_name        ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major}_static)
  set(_cmake_all_in_one_static_output_name ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major}_static)
  set(_cmake_all_in_one_static_define      ${_cmake_all_in_one_base_name}_STATIC)

  set(_cmake_all_in_one_module_name        ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major}_plugin)
  set(_cmake_all_in_one_module_output_name ${_cmake_all_in_one_base_name}-${_cmake_all_in_one_version_major}_plugin)

  set(_cmake_all_in_one_export             TRUE)
  set(_cmake_all_in_one_export_file_name   ${_cmake_all_in_one_base_name}/export.h)
  set(_cmake_all_in_one_export_macro_name  ${_cmake_all_in_one_base_name}_EXPORT)

  set(_cmake_all_in_one_languages C CXX)
  #
  # multiValueArgs
  #
  set(_cmake_all_in_one_shared_sources)
  set(_cmake_all_in_one_shared_sources_base_dirs ${CMAKE_CURRENT_SOURCE_DIR}/src)
  set(_cmake_all_in_one_shared_sources_auto_extensions *.c *.cpp *.cxx)
  set(_cmake_all_in_one_shared_sources_auto_relpath_accept_regexes)
  set(_cmake_all_in_one_shared_sources_auto_relpath_reject_regexes)
  set(_cmake_all_in_one_static_sources)
  set(_cmake_all_in_one_static_sources_base_dirs ${CMAKE_CURRENT_SOURCE_DIR}/src)
  set(_cmake_all_in_one_static_sources_auto_extensions *.c *.cpp *.cxx)
  set(_cmake_all_in_one_static_sources_auto_relpath_accept_regexes)
  set(_cmake_all_in_one_static_sources_auto_relpath_reject_regexes)
  set(_cmake_all_in_one_module_sources)
  set(_cmake_all_in_one_module_sources_base_dirs ${CMAKE_CURRENT_SOURCE_DIR}/src)
  set(_cmake_all_in_one_module_sources_auto_extensions *.c *.cpp *.cxx)
  set(_cmake_all_in_one_module_sources_auto_relpath_accept_regexes "/plugin/")
  set(_cmake_all_in_one_module_sources_auto_relpath_reject_regexes)
  set(_cmake_all_in_one_public_headers)
  set(_cmake_all_in_one_public_headers_base_dirs ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
  set(_cmake_all_in_one_public_headers_auto_extensions *.h *.hpp *.hxx)
  set(_cmake_all_in_one_public_headers_auto_relpath_accept_regexes)
  set(_cmake_all_in_one_public_headers_auto_relpath_reject_regexes)
  set(_cmake_all_in_one_private_headers)
  set(_cmake_all_in_one_private_headers_base_dirs ${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
  set(_cmake_all_in_one_private_headers_auto_extensions *.h *.hpp *.hxx)
  set(_cmake_all_in_one_private_headers_auto_relpath_accept_regexes "/internal/")
  set(_cmake_all_in_one_private_headers_auto_relpath_reject_regexes)
  set(_cmake_all_in_one_pods)
  set(_cmake_all_in_one_pods_base_dirs ${CMAKE_CURRENT_SOURCE_DIR}/pod)
  set(_cmake_all_in_one_pods_auto_extensions *.pod*)
  set(_cmake_all_in_one_pods_auto_relpath_accept_regexes)
  set(_cmake_all_in_one_pods_auto_relpath_reject_regexes)
  set(_cmake_all_in_one_pods_auto_section 3)
  set(_cmake_all_in_one_pods_auto_version ${CMAKE_PROJECT_VERSION_MAJOR})
  #
  # Parse Arguments
  #
  cmake_parse_arguments(CMAKE_ALL_IN_ONE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
  #
  # Recuperate arguments that are set
  #
  foreach(_variable ${options} ${oneValueArgs} ${multiValueArgs})
    if(DEFINED CMAKE_ALL_IN_ONE_${_variable})
      set(_name CMAKE_ALL_IN_ONE_${_variable})
      set(_value ${CMAKE_ALL_IN_ONE_${_variable}})
      string(TOLOWER "${_name}" _name)
      set(_${_name} ${_value})
    endif()
  endforeach()
  _cmake_all_in_one_log_config(${options} ${oneValueArgs} ${multiValueArgs})
  #
  # Validate arguments
  #
  foreach(_type shared_sources static_sources module_sources public_headers private_headers pods)
    foreach(_base_dir ${_cmake_all_in_one_${_type}_base_dirs})
      if(EXISTS ${_base_dir})
	get_filename_component(_fullpath ${_base_dir} ABSOLUTE)
	if(NOT IS_DIRECTORY ${_fullpath})
	  #
	  # For source_group we need to make sure a base dir is really a directory
	  #
	  message(FATAL_ERROR "${_type} base dir ${_base_dir} is not a directory")
	endif()
      endif()
    endforeach()
  endforeach()
  #
  # Process
  #
  _cmake_all_in_one_process()
  #
  # Policy pop
  #
  if($ENV{CMAKE_ALL_IN_ONE_DEBUG})
    message(STATUS "[cmake_all_in_one] cmake_policy(POP)")
  endif()
  cmake_policy(POP)
endfunction()
