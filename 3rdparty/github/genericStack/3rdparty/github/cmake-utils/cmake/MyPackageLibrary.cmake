MACRO (MYPACKAGELIBRARY config_in config_out)
  #
  # Policies - an include is resetting them, so we have to
  # be explicit. We are doing ADD_LIBRARY here.
  #
  FOREACH (_policy CMP0063 CMP0018)
    IF (POLICY ${_policy})
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Setting policy ${_policy} to NEW")
      ENDIF ()
      CMAKE_POLICY (SET ${_policy} NEW)
    ENDIF ()
  ENDFOREACH ()

  IF (MYPACKAGE_DEBUG)
    FOREACH (_source ${ARGN})
      MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Source: ${_source}")
    ENDFOREACH ()
  ENDIF ()
  #
  # Shared library
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Creating target ${PROJECT_NAME}")
  ENDIF ()
  ADD_LIBRARY (${PROJECT_NAME} SHARED ${ARGN})
  SET_TARGET_PROPERTIES(${PROJECT_NAME}
    PROPERTIES
    COMPILE_FLAGS "-D${PROJECT_NAME}_EXPORTS"
    VERSION       "${${PROJECT_NAME}_VERSION}"
    SOVERSION     "${${PROJECT_NAME}_VERSION_MAJOR}"
    )
  #
  # Static library - in practice version information is not needed
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Creating target ${PROJECT_NAME}_static")
  ENDIF ()
  ADD_LIBRARY (${PROJECT_NAME}_static STATIC ${ARGN})
  SET_TARGET_PROPERTIES(${PROJECT_NAME}_static
    PROPERTIES
    COMPILE_FLAGS "-D${PROJECT_NAME}_STATIC"
    VERSION       "${${PROJECT_NAME}_VERSION}"
    SOVERSION     "${${PROJECT_NAME}_VERSION_MAJOR}"
    )
  #
  # ... Tracing
  #
  STRING (TOUPPER ${PROJECT_NAME} _PROJECTNAME)
  IF (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    FOREACH (_target ${PROJECT_NAME} ${PROJECT_NAME}_static)
      TARGET_COMPILE_DEFINITIONS(${_target} PRIVATE -D${_PROJECTNAME}_NTRACE)
    ENDFOREACH ()
  ENDIF ()
  #
  # ... Version information
  #
  FOREACH (_target ${PROJECT_NAME} ${PROJECT_NAME}_static)
    TARGET_COMPILE_DEFINITIONS(${_target}
      PRIVATE -D${_PROJECTNAME}_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
      PRIVATE -D${_PROJECTNAME}_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
      PRIVATE -D${_PROJECTNAME}_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
      PRIVATE -D${_PROJECTNAME}_VERSION="${${PROJECT_NAME}_VERSION}"
      )
  ENDFOREACH ()
  #
  # Project's own include directories
  #
  SET (_project_include_directories "${PROJECT_SOURCE_DIR}/output/include" "${PROJECT_SOURCE_DIR}/include")
  FOREACH (_target ${PROJECT_NAME} ${PROJECT_NAME}_static)
    FOREACH (_include_directory ${_project_include_directories})
      #
      # The "internal" include directory is always internal, indeed
      #
      STRING (TOLOWER "${_include_directory}" _include_directory_lower )
      IF ( _include_directory_lower MATCHES "/internal/?$" )
	SET (_include_directory_scope "PRIVATE")
      ELSE ()
	SET (_include_directory_scope "PUBLIC")
      ENDIF ()
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Adding ${_include_directory_scope} ${_include_directory} include dependency to ${_target}")
      ENDIF ()
      TARGET_INCLUDE_DIRECTORIES(${_target} ${_include_directory_scope} ${_include_directory})
    ENDFOREACH ()
  ENDFOREACH ()
  INSTALL (TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin COMPONENT DynamicLibraryComponent
    LIBRARY DESTINATION lib COMPONENT DynamicLibraryComponent
    ARCHIVE DESTINATION lib COMPONENT DynamicLibraryComponent
    )
  INSTALL (TARGETS ${PROJECT_NAME}_static
    RUNTIME DESTINATION bin COMPONENT StaticLibraryComponent
    LIBRARY DESTINATION lib COMPONENT StaticLibraryComponent
    ARCHIVE DESTINATION lib COMPONENT StaticLibraryComponent
    )
  SET (_HAVE_DYNAMICLIBRARYCOMPONENT TRUE CACHE INTERNAL "Have DynamicLibraryComponent" FORCE)
  SET (_HAVE_STATICLIBRARYCOMPONENT TRUE CACHE INTERNAL "Have StaticLibraryComponent" FORCE)
  #
  # Call for the configuration
  #
  MYPACKAGECONFIG(${config_in} ${config_out})
  #
  # Call for the export headers
  #
  MYPACKAGEEXPORT()
  #
  # Add dependencies to config and export headers
  #
  ADD_DEPENDENCIES(${PROJECT_NAME} ${PROJECT_NAME}Config ${PROJECT_NAME}Export)
ENDMACRO()
