# Module for locating luaunpanic, based on ICU module.
#
# Cutomizable variables:
#   luaunpanic_ROOT_DIR
#     This variable points to the luaunpanic root directory. On Windows the
#     library location typically will have to be provided explicitly using the
#     -D command-line option. Alternatively, an environment variable can be set.
#
# Read-Only variables:
#   luaunpanic_FOUND
#     Indicates whether the library has been found.
#
#   luaunpanic_INCLUDE_DIRS
#     Points to the luaunpanic include directory.
#
#   luaunpanic_LIBRARIES
#     Points to the luaunpanic libraries that should be passed to
#     target_link_libraries in dynamic mode.
#
#   luaunpanic_STATIC_LIBRARIES
#     Points to the luaunpanic libraries static that should be passed to
#     target_link_libraries in static mode.
#
#   luaunpanic_LINK_FLAGS
#     Linker flags when building linking against luaunpanic. Defaults to "libluaunpanic". Take care, this is a list.
#
#   luaunpanic_STATIC_LINK_FLAGS
#     Linker flags when building linking against static luaunpanic. Defaults to "libluaunpanic_static". Take care, this is a list.
#
#   luaunpanic_LDFLAGS
#     Stringified version of luaunpanic_LINK_FLAGS
#
#   luaunpanic_STATIC_LDFLAGS
#     Stringified version of luaunpanic_STATIC_LINK_FLAGS
#
INCLUDE (CMakeParseArguments)
INCLUDE (FindPackageHandleStandardArgs)

SET (_PF86 "ProgramFiles(x86)")
SET (_luaunpanic_POSSIBLE_DIRS
  ${luaunpanic_ROOT_DIR}
  "$ENV{luaunpanic_ROOT_DIR}"
  "C:/luaunpanic"
  "$ENV{PROGRAMFILES}/luaunpanic"
  "$ENV{${_PF86}}/luaunpanic")

SET (_luaunpanic_POSSIBLE_INCLUDE_SUFFIXES include)

IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET (_luaunpanic_POSSIBLE_LIB_SUFFIXES lib64)
  SET (_luaunpanic_POSSIBLE_BIN_SUFFIXES bin64)

  IF (NOT WIN32)
    LIST (APPEND _luaunpanic_POSSIBLE_LIB_SUFFIXES lib)
    LIST (APPEND _luaunpanic_POSSIBLE_BIN_SUFFIXES bin)
  ENDIF (NOT WIN32)
ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET (_luaunpanic_POSSIBLE_LIB_SUFFIXES lib)
  SET (_luaunpanic_POSSIBLE_BIN_SUFFIXES bin)
ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 8)

FIND_PATH (luaunpanic_ROOT_DIR
  NAMES include/luaunpanic.h
  PATHS ${_luaunpanic_POSSIBLE_DIRS}
  DOC "luaunpanic root directory")

IF (luaunpanic_ROOT_DIR)
  # Re-use the previous path:
  FIND_PATH (luaunpanic_INCLUDE_DIR
    NAMES luaunpanic.h
    PATHS ${luaunpanic_ROOT_DIR}
    PATH_SUFFIXES ${_luaunpanic_POSSIBLE_INCLUDE_SUFFIXES}
    DOC "luaunpanic include directory"
    # NO_DEFAULT_PATH
    )
ELSE (luaunpanic_ROOT_DIR)
  # Use default path search
  FIND_PATH (luaunpanic_INCLUDE_DIR
    NAMES luaunpanic.h
    DOC "luaunpanic include directory"
    )
ENDIF (luaunpanic_ROOT_DIR)

IF (NOT luaunpanic_FIND_COMPONENTS)
  SET (luaunpanic_FIND_COMPONENTS luaunpanic)
ENDIF (NOT luaunpanic_FIND_COMPONENTS)

IF (luaunpanic_INCLUDE_DIR)
  # Loop over each components
  FOREACH (_luaunpanic_COMPONENT ${luaunpanic_FIND_COMPONENTS})
    SET (_luaunpanic_COMPONENT_BASE ${_luaunpanic_COMPONENT})
    SET (_luaunpanic_COMPONENT_POSSIBLE_DEBUG_NAMES ${_luaunpanic_COMPONENT_BASE}d)
    SET (_luaunpanic_COMPONENT_POSSIBLE_RELEASE_NAMES ${_luaunpanic_COMPONENT_BASE})

    SET (_luaunpanic_STATIC_COMPONENT_BASE "${_luaunpanic_COMPONENT}_static")
    SET (_luaunpanic_STATIC_COMPONENT_POSSIBLE_DEBUG_NAMES ${_luaunpanic_STATIC_COMPONENT_BASE}d)
    SET (_luaunpanic_STATIC_COMPONENT_POSSIBLE_RELEASE_NAMES ${_luaunpanic_STATIC_COMPONENT_BASE})

    STRING (TOUPPER ${_luaunpanic_COMPONENT} _luaunpanic_COMPONENT_UPPER)
    SET (_luaunpanic_LIBRARY_BASE luaunpanic_${_luaunpanic_COMPONENT_UPPER}_LIBRARY)
    SET (_luaunpanic_STATIC_LIBRARY_BASE luaunpanic_STATIC_${_luaunpanic_COMPONENT_UPPER}_LIBRARY)

    IF (luaunpanic_ROOT_DIR)
      FIND_LIBRARY (${_luaunpanic_LIBRARY_BASE}_DEBUG
	NAMES ${_luaunpanic_COMPONENT_POSSIBLE_DEBUG_NAMES}
	PATHS ${luaunpanic_ROOT_DIR} PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
      FIND_LIBRARY (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG
	NAMES ${_luaunpanic_STATIC_COMPONENT_POSSIBLE_DEBUG_NAMES}
	PATHS ${luaunpanic_ROOT_DIR} PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
    ELSE (luaunpanic_ROOT_DIR)
      FIND_LIBRARY (${_luaunpanic_LIBRARY_BASE}_DEBUG
	NAMES ${_luaunpanic_COMPONENT_POSSIBLE_DEBUG_NAMES}
	PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	)
      FIND_LIBRARY (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG
	NAMES ${_luaunpanic_COMPONENT_STATIC_POSSIBLE_DEBUG_NAMES}
	PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	)
    ENDIF (luaunpanic_ROOT_DIR)

    IF (luaunpanic_ROOT_DIR)
      FIND_LIBRARY (${_luaunpanic_LIBRARY_BASE}_RELEASE
	NAMES ${_luaunpanic_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATHS ${luaunpanic_ROOT_DIR} PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
      FIND_LIBRARY (${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE
	NAMES ${_luaunpanic_STATIC_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATHS ${luaunpanic_ROOT_DIR} PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
    ELSE (ICU_ROOT_DIR)
      FIND_LIBRARY (${_luaunpanic_LIBRARY_BASE}_RELEASE
	NAMES ${_luaunpanic_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	)
      FIND_LIBRARY (${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE
	NAMES ${_luaunpanic_STATIC_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATH_SUFFIXES ${_luaunpanic_POSSIBLE_LIB_SUFFIXES}
	)
    ENDIF (luaunpanic_ROOT_DIR)

    SET (luaunpanic_${_luaunpanic_COMPONENT_UPPER}_FOUND TRUE)

    # Debug and release
    IF (${_luaunpanic_LIBRARY_BASE}_DEBUG AND ${_luaunpanic_LIBRARY_BASE}_RELEASE)
      SET (${_luaunpanic_LIBRARY_BASE}
        debug ${${_luaunpanic_LIBRARY_BASE}_DEBUG}
        optimized ${${_luaunpanic_LIBRARY_BASE}_RELEASE})
      # Debug only
    ELSEIF (${_luaunpanic_LIBRARY_BASE}_DEBUG)
      SET (${_luaunpanic_LIBRARY_BASE} ${${_luaunpanic_LIBRARY_BASE}_DEBUG})
      # Release only
    ELSEIF (${_luaunpanic_LIBRARY_BASE}_RELEASE)
      SET (${_luaunpanic_LIBRARY_BASE} ${${_luaunpanic_LIBRARY_BASE}_RELEASE})
    ELSE (${_luaunpanic_LIBRARY_BASE}_DEBUG AND ${_luaunpanic_LIBRARY_BASE}_RELEASE)
      # Component missing: record it for a later report
      LIST (APPEND _luaunpanic_MISSING_COMPONENTS ${_luaunpanic_COMPONENT})
      SET (luaunpanic_${__COMPONENT_UPPER}_FOUND FALSE)
    ENDIF (${_luaunpanic_LIBRARY_BASE}_DEBUG AND ${_luaunpanic_LIBRARY_BASE}_RELEASE)

    IF (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG AND ${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE)
      SET (${_luaunpanic_STATIC_LIBRARY_BASE}
        debug ${${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG}
        optimized ${${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE})
      # Debug only
    ELSEIF (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG)
      SET (${_luaunpanic_STATIC_LIBRARY_BASE} ${${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG})
      # Release only
    ELSEIF (${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE)
      SET (${_luaunpanic_STATIC_LIBRARY_BASE} ${${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE})
    ELSE (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG AND ${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE)
      # Component missing: record it for a later report
      LIST (APPEND _luaunpanic_STATIC_MISSING_COMPONENTS ${_luaunpanic_COMPONENT})
      SET (luaunpanic_STATIC_${__COMPONENT_UPPER}_FOUND FALSE)
    ENDIF (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG AND ${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE)

    MARK_AS_ADVANCED (${_luaunpanic_LIBRARY_BASE}_DEBUG ${_luaunpanic_LIBRARY_BASE}_RELEASE)
    MARK_AS_ADVANCED (${_luaunpanic_STATIC_LIBRARY_BASE}_DEBUG ${_luaunpanic_STATIC_LIBRARY_BASE}_RELEASE)

    # Make sure only libraries that have been actually found are registered
    IF (${_luaunpanic_LIBRARY_BASE})
      LIST (APPEND _luaunpanic_LIBRARIES ${${_luaunpanic_LIBRARY_BASE}})
    ENDIF (${_luaunpanic_LIBRARY_BASE})

    IF (${_luaunpanic_STATIC_LIBRARY_BASE})
      LIST (APPEND _luaunpanic_STATIC_LIBRARIES ${${_luaunpanic_STATIC_LIBRARY_BASE}})
    ENDIF (${_luaunpanic_STATIC_LIBRARY_BASE})

  ENDFOREACH (_luaunpanic_COMPONENT)

  IF (NOT DEFINED _luaunpanic_MISSING_COMPONENTS AND NOT DEFINED _luaunpanic_STATIC_MISSING_COMPONENTS)
    # Success: all components were found
    SET (luaunpanic_INCLUDE_DIRS "${luaunpanic_INCLUDE_DIR}")
    SET (luaunpanic_LIBRARIES ${_luaunpanic_LIBRARIES})
    SET (luaunpanic_STATIC_LIBRARIES ${_luaunpanic_STATIC_LIBRARIES} ${CMAKE_DL_LIBS})
  ELSE ()
    MESSAGE (STATUS "Several luaunpanic components were not found:")

    # Display missing components indented, each on a separate line
    FOREACH (_luaunpanic_MISSING_COMPONENT ${_luaunpanic_MISSING_COMPONENTS})
      MESSAGE (STATUS "  " ${_luaunpanic_MISSING_COMPONENT})
    ENDFOREACH ()
    FOREACH (_luaunpanic_STATIC_MISSING_COMPONENT ${_luaunpanic_STATIC_MISSING_COMPONENTS})
      MESSAGE (STATUS "  " ${_luaunpanic_STATIC_MISSING_COMPONENT})
    ENDFOREACH ()
  ENDIF ()

  #
  # Simulate hopefully correct flags based on heuristic discovery
  #
  IF (NOT _luaunpanic_C_FLAGS_SHARED)
    SET (_luaunpanic_C_FLAGS_SHARED "${CMAKE_SHARED_LIBRARY_C_FLAGS}")
  ENDIF ()

  IF (NOT _luaunpanic_CXX_FLAGS_SHARED)
    SET (_luaunpanic_CXX_FLAGS_SHARED "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
  ENDIF ()

  IF (NOT _luaunpanic_LINK_FLAGS)
    SET (_luaunpanic_LINK_FLAGS ${luaunpanic_LIBRARIES})
  ENDIF ()

  IF (NOT _luaunpanic_STATIC_LINK_FLAGS)
    SET (_luaunpanic_STATIC_LINK_FLAGS ${luaunpanic_STATIC_LIBRARIES})
  ENDIF ()

ENDIF (luaunpanic_INCLUDE_DIR)

SET (_luaunpanic_LDFLAGS " ")
FOREACH (_luaunpanic_link_flag ${_luaunpanic_LINK_FLAGS})
  SET (_luaunpanic_LDFLAGS "${_luaunpanic_LDFLAGS} ${_luaunpanic_link_flag}")
ENDFOREACH ()

SET (_luaunpanic_STATIC_LDFLAGS " ")
FOREACH (_luaunpanic_static_link_flag ${_luaunpanic_STATIC_LINK_FLAGS})
  SET (_luaunpanic_STATIC_LDFLAGS "${_luaunpanic_STATIC_LDFLAGS} ${_luaunpanic_static_link_flag}")
ENDFOREACH ()

MARK_AS_ADVANCED (luaunpanic_ROOT_DIR luaunpanic_INCLUDE_DIR)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (luaunpanic
  REQUIRED_VARS
  luaunpanic_INCLUDE_DIR
  luaunpanic_LIBRARIES)

IF (_luaunpanic_LINK_FLAGS AND _luaunpanic_STATIC_LINK_FLAGS)
  SET (luaunpanic_FOUND 1 CACHE STRING "luaunpanic is found")
ENDIF ()

SET (luaunpanic_C_FLAGS_SHARED    "${_luaunpanic_C_FLAGS_SHARED}"    CACHE STRING "C flags when compiling against shared luaunpanic")
SET (luaunpanic_CXX_FLAGS_SHARED  "${_luaunpanic_CXX_FLAGS_SHARED}"  CACHE STRING "C flags when compiling against shared luaunpanic")
SET (luaunpanic_LINK_FLAGS        "${_luaunpanic_LINK_FLAGS}"        CACHE STRING "Linker flags when linking against luaunpanic")
SET (luaunpanic_LDFLAGS           "${_luaunpanic_LDFLAGS}"           CACHE STRING "Stringified version of luaunpanic_LINK_FLAGS")
SET (luaunpanic_STATIC_LINK_FLAGS "${_luaunpanic_STATIC_LINK_FLAGS}" CACHE STRING "Linker flags when linking against static luaunpanic")
SET (luaunpanic_STATIC_LDFLAGS    "${_luaunpanic_STATIC_LDFLAGS}"    CACHE STRING "Stringified version of luaunpanic_STATIC_LINK_FLAGS")

IF(luaunpanic_FOUND)
  MESSAGE(STATUS "-----------------------------------------")
  MESSAGE(STATUS "Setup luaunpanic:")
  MESSAGE(STATUS "")
  MESSAGE(STATUS "           ROOT_DIR: ${luaunpanic_ROOT_DIR}")
  MESSAGE(STATUS "     C_FLAGS_SHARED: ${luaunpanic_C_FLAGS_SHARED}")
  MESSAGE(STATUS "   CXX_FLAGS_SHARED: ${luaunpanic_CXX_FLAGS_SHARED}")
  MESSAGE(STATUS "        INCLUDE_DIR: ${luaunpanic_INCLUDE_DIR}")
  MESSAGE(STATUS "          LIBRARIES: ${luaunpanic_LIBRARIES}")
  MESSAGE(STATUS "   STATIC LIBRARIES: ${luaunpanic_STATIC_LIBRARIES}")
  MESSAGE(STATUS "         LINK_FLAGS: ${luaunpanic_LINK_FLAGS}")
  MESSAGE(STATUS "  STATIC LINK_FLAGS: ${luaunpanic_STATIC_LINK_FLAGS}")
  MESSAGE(STATUS "            LDFLAGS: ${luaunpanic_LDFLAGS}")
  MESSAGE(STATUS "     STATIC LDFLAGS: ${luaunpanic_STATIC_LDFLAGS}")
  MESSAGE(STATUS "-----------------------------------------")
ENDIF()

MARK_AS_ADVANCED (
  luaunpanic_FOUND
  luaunpanic_C_FLAGS_SHARED
  luaunpanic_CXX_FLAGS_SHARED
  luaunpanic_LINK_FLAGS
  luaunpanic_LDFLAGS
  luaunpanic_STATIC_LINK_FLAGS
  luaunpanic_STATIC_LDFLAGS
  )
