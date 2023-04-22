MACRO (MYPACKAGELIBRARY config_in config_out)
  #
  # Call for the configuration
  #
  MYPACKAGECONFIG(${config_in} ${config_out})
  #
  # We always have four versions:
  # -----------------------------
  # ${PROJECT_NAME}                   SHARED library
  # ${PROJECT_NAME}_static            STATIC library
  # ${PROJECT_NAME}_objs              SHARED objects
  # ${PROJECT_NAME}_static_objs       STATIC objects
  #
  IF (MYPACKAGE_DEBUG)
    FOREACH (_source ${ARGN})
      MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Source: ${_source}")
    ENDFOREACH ()
  ENDIF ()
  #
  # Shared objects
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Creating SHARED objects ${PROJECT_NAME}_objs")
  ENDIF ()
  ADD_LIBRARY (${PROJECT_NAME}_objs OBJECT $<${build_local_interface}:${ARGN}>)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PRIVATE -D${PROJECT_NAME}_EXPORTS on ${PROJECT_NAME}_objs")
  ENDIF ()
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_objs PRIVATE -D${PROJECT_NAME}_EXPORTS)
  #
  # Shared library
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Creating SHARED library ${PROJECT_NAME}")
  ENDIF ()
  ADD_LIBRARY (${PROJECT_NAME} SHARED $<TARGET_OBJECTS:${PROJECT_NAME}_objs>)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PRIVATE -D${PROJECT_NAME}_EXPORTS on ${PROJECT_NAME}")
  ENDIF ()
  SET_TARGET_PROPERTIES(${PROJECT_NAME}
    PROPERTIES
    VERSION       ${${PROJECT_NAME}_VERSION}
    SOVERSION     ${${PROJECT_NAME}_VERSION_MAJOR}
  )
  #
  # Static objects
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Creating STATIC objects ${PROJECT_NAME}_static_objs")
  ENDIF ()
  ADD_LIBRARY (${PROJECT_NAME}_static_objs OBJECT $<${build_local_interface}:${ARGN}>)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PUBLIC -D${PROJECT_NAME}_STATIC on ${PROJECT_NAME}_objs")
  ENDIF ()
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_static_objs PUBLIC -D${PROJECT_NAME}_STATIC)
  #
  # Static library
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Creating STATIC target ${PROJECT_NAME}_static")
  ENDIF ()
  ADD_LIBRARY (${PROJECT_NAME}_static STATIC $<TARGET_OBJECTS:${PROJECT_NAME}_static_objs>)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PUBLIC -D${PROJECT_NAME}_STATIC on ${PROJECT_NAME}")
  ENDIF ()
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_static PUBLIC -D${PROJECT_NAME}_STATIC)
  #
  # ... Tracing
  #
  STRING (TOUPPER ${PROJECT_NAME} _PROJECTNAME)
  IF (NTRACE)
    FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PRIVATE -D${_PROJECTNAME}_NTRACE on ${_target}")
      ENDIF ()
      TARGET_COMPILE_DEFINITIONS(${_target} PRIVATE -D${_PROJECTNAME}_NTRACE)
    ENDFOREACH ()
  ENDIF ()
  #
  # ... Version information
  #
  FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PRIVATE version macros on ${_target}")
    ENDIF ()
    TARGET_COMPILE_DEFINITIONS(${_target}
      PRIVATE -D${_PROJECTNAME}_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
      PRIVATE -D${_PROJECTNAME}_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
      PRIVATE -D${_PROJECTNAME}_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
      PRIVATE -D${_PROJECTNAME}_VERSION="${${PROJECT_NAME}_VERSION}"
      )
  ENDFOREACH ()
  #
  # We always enable C99 when available
  #
  FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PROPERTY C_STANDARD 99 on ${_target}")
    ENDIF ()
    SET_PROPERTY(TARGET ${_target} PROPERTY C_STANDARD 99)
  ENDFOREACH ()
  #
  # OS Specifics
  #
  IF (CMAKE_SYSTEM_NAME MATCHES "NetBSD")
    #
    # On NetBSD, enable this platform features. This makes sure we always have "long long" btw.
    #
    FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs ${PROJECT_NAME} ${PROJECT_NAME}_static)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Setting PUBLIC -D_NETBSD_SOURCE=1 on ${_target}")
      ENDIF ()
      TARGET_COMPILE_DEFINITIONS (${_target} PUBLIC -D_NETBSD_SOURCE=1)
    ENDFOREACH ()
  ENDIF ()
  #
  # Project's own include directories
  #
  SET (_project_include_directories ${CMAKE_CURRENT_BINARY_DIR}/output/include ${PROJECT_SOURCE_DIR}/include)
  FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs ${PROJECT_NAME} ${PROJECT_NAME}_static)
    FOREACH (_include_directory ${_project_include_directories})
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Adding PUBLIC ${build_local_interface} ${_include_directory} include dependency to ${_target}")
      ENDIF ()
      TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC $<${build_local_interface}:${_include_directory}>)
    ENDFOREACH ()
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Adding PUBLIC INSTALL_INTERFACE ${_include_directory} include dependency to ${_target}")
    ENDIF ()
    TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
  ENDFOREACH ()
  #
  # Call for the export headers
  #
  MYPACKAGEEXPORT()
  FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-LIBRARY-DEBUG] Adding ${PROJECT_NAME}_export ${PROJECT_NAME}_config dependencies to ${_target}")
    ENDIF ()
    ADD_DEPENDENCIES(${_target} ${PROJECT_NAME}_export ${PROJECT_NAME}_config)
  ENDFOREACH ()
  #
  # Install library targets
  #
  IF (NOT CMAKE_VERSION VERSION_LESS "3.26")
    INSTALL (TARGETS ${PROJECT_NAME}
      EXPORT ${PROJECT_NAME}-targets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT LibraryComponent
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
    )
    INSTALL (TARGETS ${PROJECT_NAME}_static
      EXPORT ${PROJECT_NAME}-targets
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT LibraryComponent
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
    )
  ELSE ()
    INSTALL (TARGETS ${PROJECT_NAME}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT LibraryComponent
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
    )
    INSTALL (TARGETS ${PROJECT_NAME}_static
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT LibraryComponent
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT LibraryComponent
    )
  ENDIF ()
  #
  # For static library we want to debug information within the lib
  # For shared library we want to install the pdb file if it exists
  #
  IF (MSVC)
    TARGET_COMPILE_OPTIONS(${PROJECT_NAME}_static PRIVATE /Z7)
    INSTALL(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT LibraryComponent OPTIONAL)
  ENDIF ()
  #
  # We make sure that the directory where is ${config_out} is public
  #
  GET_FILENAME_COMPONENT(_config_out_dir ${config_out} DIRECTORY)
  FOREACH (_target ${PROJECT_NAME}_objs ${PROJECT_NAME}_static_objs)
    TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC $<${build_local_interface}:${_config_out_dir}>)
  ENDFOREACH ()

  SET (${PROJECT_NAME}_HAVE_LIBRARYCOMPONENT TRUE CACHE INTERNAL "Have LibraryComponent" FORCE)
ENDMACRO()
