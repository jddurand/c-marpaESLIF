# Module for locating luaunpanic, based on ICU module.
#
# Cutomizable variables:
#   LUAunpanic_ROOT_DIR
#     This variable points to the luaunpanic root directory. On Windows the
#     library location typically will have to be provided explicitly using the
#     -D command-line option. Alternatively, an environment variable can be set.
#
# Read-Only variables:
#   LUAunpanic_FOUND
#     Indicates whether the library has been found.
#
#   LUAunpanic_INCLUDE_DIRS
#     Points to the luaunpanic include directory.
#
#   LUAunpanic_LIBRARIES
#     Points to the luaunpanic libraries that should be passed to
#     target_link_libraries in dynamic mode.
#
#   LUAunpanic_STATIC_LIBRARIES
#     Points to the luaunpanic libraries static that should be passed to
#     target_link_libraries in static mode.
#
#   LUAunpanic_LINK_FLAGS
#     Linker flags when building linking against luaunpanic. Defaults to "libluaunpanic". Take care, this is a list.
#
#   LUAunpanic_STATIC_LINK_FLAGS
#     Linker flags when building linking against static luaunpanic. Defaults to "libluaunpanic_static". Take care, this is a list.
#
#   LUAunpanic_LDFLAGS
#     Stringified version of LUAunpanic_LINK_FLAGS
#
#   LUAunpanic_STATIC_LDFLAGS
#     Stringified version of LUAunpanic_STATIC_LINK_FLAGS
#
INCLUDE (CMakeParseArguments)
INCLUDE (FindPackageHandleStandardArgs)

SET (_PF86 "ProgramFiles(x86)")
SET (_LUAunpanic_POSSIBLE_DIRS
  ${LUAunpanic_ROOT_DIR}
  "$ENV{LUAunpanic_ROOT_DIR}"
  "C:/luaunpanic"
  "$ENV{PROGRAMFILES}/luaunpanic"
  "$ENV{${_PF86}}/luaunpanic")

SET (_LUAunpanic_POSSIBLE_INCLUDE_SUFFIXES include)

IF (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET (_LUAunpanic_POSSIBLE_LIB_SUFFIXES lib64)
  SET (_LUAunpanic_POSSIBLE_BIN_SUFFIXES bin64)

  IF (NOT WIN32)
    LIST (APPEND _LUAunpanic_POSSIBLE_LIB_SUFFIXES lib)
    LIST (APPEND _LUAunpanic_POSSIBLE_BIN_SUFFIXES bin)
  ENDIF (NOT WIN32)
ELSE (CMAKE_SIZEOF_VOID_P EQUAL 8)
  SET (_LUAunpanic_POSSIBLE_LIB_SUFFIXES lib)
  SET (_LUAunpanic_POSSIBLE_BIN_SUFFIXES bin)
ENDIF (CMAKE_SIZEOF_VOID_P EQUAL 8)

FIND_PATH (LUAunpanic_ROOT_DIR
  NAMES include/luaunpanic.h
  PATHS ${_LUAunpanic_POSSIBLE_DIRS}
  DOC "luaunpanic root directory")

IF (LUAunpanic_ROOT_DIR)
  # Re-use the previous path:
  FIND_PATH (LUAunpanic_INCLUDE_DIR
    NAMES luaunpanic.h
    PATHS ${LUAunpanic_ROOT_DIR}
    PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_INCLUDE_SUFFIXES}
    DOC "luaunpanic include directory"
    # NO_DEFAULT_PATH
    )
ELSE (LUAunpanic_ROOT_DIR)
  # Use default path search
  FIND_PATH (LUAunpanic_INCLUDE_DIR
    NAMES luaunpanic.h
    DOC "luaunpanic include directory"
    )
ENDIF (LUAunpanic_ROOT_DIR)

IF (NOT LUAunpanic_FIND_COMPONENTS)
  SET (LUAunpanic_FIND_COMPONENTS luaunpanic)
ENDIF (NOT LUAunpanic_FIND_COMPONENTS)

IF (LUAunpanic_INCLUDE_DIR)
  # Loop over each components
  FOREACH (_LUAunpanic_COMPONENT ${LUAunpanic_FIND_COMPONENTS})
    SET (_LUAunpanic_COMPONENT_BASE ${_LUAunpanic_COMPONENT})
    SET (_LUAunpanic_COMPONENT_POSSIBLE_DEBUG_NAMES ${_LUAunpanic_COMPONENT_BASE}d)
    SET (_LUAunpanic_COMPONENT_POSSIBLE_RELEASE_NAMES ${_LUAunpanic_COMPONENT_BASE})

    SET (_LUAunpanic_STATIC_COMPONENT_BASE "${_LUAunpanic_COMPONENT}_static")
    SET (_LUAunpanic_STATIC_COMPONENT_POSSIBLE_DEBUG_NAMES ${_LUAunpanic_STATIC_COMPONENT_BASE}d)
    SET (_LUAunpanic_STATIC_COMPONENT_POSSIBLE_RELEASE_NAMES ${_LUAunpanic_STATIC_COMPONENT_BASE})

    STRING (TOUPPER ${_LUAunpanic_COMPONENT} _LUAunpanic_COMPONENT_UPPER)
    SET (_LUAunpanic_LIBRARY_BASE LUAunpanic_${_LUAunpanic_COMPONENT_UPPER}_LIBRARY)
    SET (_LUAunpanic_STATIC_LIBRARY_BASE LUAunpanic_STATIC_${_LUAunpanic_COMPONENT_UPPER}_LIBRARY)

    IF (LUAunpanic_ROOT_DIR)
      FIND_LIBRARY (${_LUAunpanic_LIBRARY_BASE}_DEBUG
	NAMES ${_LUAunpanic_COMPONENT_POSSIBLE_DEBUG_NAMES}
	PATHS ${LUAunpanic_ROOT_DIR} PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
      FIND_LIBRARY (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG
	NAMES ${_LUAunpanic_STATIC_COMPONENT_POSSIBLE_DEBUG_NAMES}
	PATHS ${LUAunpanic_ROOT_DIR} PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
    ELSE (LUAunpanic_ROOT_DIR)
      FIND_LIBRARY (${_LUAunpanic_LIBRARY_BASE}_DEBUG
	NAMES ${_LUAunpanic_COMPONENT_POSSIBLE_DEBUG_NAMES}
	PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	)
      FIND_LIBRARY (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG
	NAMES ${_LUAunpanic_COMPONENT_STATIC_POSSIBLE_DEBUG_NAMES}
	PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	)
    ENDIF (LUAunpanic_ROOT_DIR)

    IF (LUAunpanic_ROOT_DIR)
      FIND_LIBRARY (${_LUAunpanic_LIBRARY_BASE}_RELEASE
	NAMES ${_LUAunpanic_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATHS ${LUAunpanic_ROOT_DIR} PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
      FIND_LIBRARY (${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE
	NAMES ${_LUAunpanic_STATIC_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATHS ${LUAunpanic_ROOT_DIR} PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	# NO_DEFAULT_PATH
	)
    ELSE (ICU_ROOT_DIR)
      FIND_LIBRARY (${_LUAunpanic_LIBRARY_BASE}_RELEASE
	NAMES ${_LUAunpanic_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	)
      FIND_LIBRARY (${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE
	NAMES ${_LUAunpanic_STATIC_COMPONENT_POSSIBLE_RELEASE_NAMES}
	PATH_SUFFIXES ${_LUAunpanic_POSSIBLE_LIB_SUFFIXES}
	)
    ENDIF (LUAunpanic_ROOT_DIR)

    SET (LUAunpanic_${_LUAunpanic_COMPONENT_UPPER}_FOUND TRUE)

    # Debug and release
    IF (${_LUAunpanic_LIBRARY_BASE}_DEBUG AND ${_LUAunpanic_LIBRARY_BASE}_RELEASE)
      SET (${_LUAunpanic_LIBRARY_BASE}
        debug ${${_LUAunpanic_LIBRARY_BASE}_DEBUG}
        optimized ${${_LUAunpanic_LIBRARY_BASE}_RELEASE})
      # Debug only
    ELSEIF (${_LUAunpanic_LIBRARY_BASE}_DEBUG)
      SET (${_LUAunpanic_LIBRARY_BASE} ${${_LUAunpanic_LIBRARY_BASE}_DEBUG})
      # Release only
    ELSEIF (${_LUAunpanic_LIBRARY_BASE}_RELEASE)
      SET (${_LUAunpanic_LIBRARY_BASE} ${${_LUAunpanic_LIBRARY_BASE}_RELEASE})
    ELSE (${_LUAunpanic_LIBRARY_BASE}_DEBUG AND ${_LUAunpanic_LIBRARY_BASE}_RELEASE)
      # Component missing: record it for a later report
      LIST (APPEND _LUAunpanic_MISSING_COMPONENTS ${_LUAunpanic_COMPONENT})
      SET (LUAunpanic_${__COMPONENT_UPPER}_FOUND FALSE)
    ENDIF (${_LUAunpanic_LIBRARY_BASE}_DEBUG AND ${_LUAunpanic_LIBRARY_BASE}_RELEASE)

    IF (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG AND ${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE)
      SET (${_LUAunpanic_STATIC_LIBRARY_BASE}
        debug ${${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG}
        optimized ${${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE})
      # Debug only
    ELSEIF (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG)
      SET (${_LUAunpanic_STATIC_LIBRARY_BASE} ${${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG})
      # Release only
    ELSEIF (${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE)
      SET (${_LUAunpanic_STATIC_LIBRARY_BASE} ${${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE})
    ELSE (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG AND ${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE)
      # Component missing: record it for a later report
      LIST (APPEND _LUAunpanic_STATIC_MISSING_COMPONENTS ${_LUAunpanic_COMPONENT})
      SET (LUAunpanic_STATIC_${__COMPONENT_UPPER}_FOUND FALSE)
    ENDIF (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG AND ${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE)

    MARK_AS_ADVANCED (${_LUAunpanic_LIBRARY_BASE}_DEBUG ${_LUAunpanic_LIBRARY_BASE}_RELEASE)
    MARK_AS_ADVANCED (${_LUAunpanic_STATIC_LIBRARY_BASE}_DEBUG ${_LUAunpanic_STATIC_LIBRARY_BASE}_RELEASE)

    # Make sure only libraries that have been actually found are registered
    IF (${_LUAunpanic_LIBRARY_BASE})
      LIST (APPEND _LUAunpanic_LIBRARIES ${${_LUAunpanic_LIBRARY_BASE}})
    ENDIF (${_LUAunpanic_LIBRARY_BASE})

    IF (${_LUAunpanic_STATIC_LIBRARY_BASE})
      LIST (APPEND _LUAunpanic_STATIC_LIBRARIES ${${_LUAunpanic_STATIC_LIBRARY_BASE}})
    ENDIF (${_LUAunpanic_STATIC_LIBRARY_BASE})

  ENDFOREACH (_LUAunpanic_COMPONENT)

  IF (NOT DEFINED _LUAunpanic_MISSING_COMPONENTS AND NOT DEFINED _LUAunpanic_STATIC_MISSING_COMPONENTS)
    # Success: all components were found
    SET (LUAunpanic_INCLUDE_DIRS "${LUAunpanic_INCLUDE_DIR}")
    SET (LUAunpanic_LIBRARIES ${_LUAunpanic_LIBRARIES})
    SET (LUAunpanic_STATIC_LIBRARIES ${_LUAunpanic_STATIC_LIBRARIES} ${CMAKE_DL_LIBS})
  ELSE ()
    MESSAGE (STATUS "Several LUAunpanic components were not found:")

    # Display missing components indented, each on a separate line
    FOREACH (_LUAunpanic_MISSING_COMPONENT ${_LUAunpanic_MISSING_COMPONENTS})
      MESSAGE (STATUS "  " ${_LUAunpanic_MISSING_COMPONENT})
    ENDFOREACH ()
    FOREACH (_LUAunpanic_STATIC_MISSING_COMPONENT ${_LUAunpanic_STATIC_MISSING_COMPONENTS})
      MESSAGE (STATUS "  " ${_LUAunpanic_STATIC_MISSING_COMPONENT})
    ENDFOREACH ()
  ENDIF ()

  #
  # Simulate hopefully correct flags based on heuristic discovery
  #
  IF (NOT _LUAunpanic_C_FLAGS_SHARED)
    SET (_LUAunpanic_C_FLAGS_SHARED "${CMAKE_SHARED_LIBRARY_C_FLAGS}")
  ENDIF ()

  IF (NOT _LUAunpanic_CXX_FLAGS_SHARED)
    SET (_LUAunpanic_CXX_FLAGS_SHARED "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
  ENDIF ()

  IF (NOT _LUAunpanic_LINK_FLAGS)
    SET (_LUAunpanic_LINK_FLAGS ${LUAunpanic_LIBRARIES})
  ENDIF ()

  IF (NOT _LUAunpanic_STATIC_LINK_FLAGS)
    SET (_LUAunpanic_STATIC_LINK_FLAGS ${LUAunpanic_STATIC_LIBRARIES})
  ENDIF ()

ENDIF (LUAunpanic_INCLUDE_DIR)

SET (_LUAunpanic_LDFLAGS " ")
FOREACH (_luaunpanic_link_flag ${_LUAunpanic_LINK_FLAGS})
  SET (_LUAunpanic_LDFLAGS "${_LUAunpanic_LDFLAGS} ${_luaunpanic_link_flag}")
ENDFOREACH ()

SET (_LUAunpanic_STATIC_LDFLAGS " ")
FOREACH (_luaunpanic_static_link_flag ${_LUAunpanic_STATIC_LINK_FLAGS})
  SET (_LUAunpanic_STATIC_LDFLAGS "${_LUAunpanic_STATIC_LDFLAGS} ${_luaunpanic_static_link_flag}")
ENDFOREACH ()

MARK_AS_ADVANCED (LUAunpanic_ROOT_DIR LUAunpanic_INCLUDE_DIR)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (LUAunpanic
  REQUIRED_VARS
  LUAunpanic_INCLUDE_DIR
  LUAunpanic_LIBRARIES)

IF (_LUAunpanic_LINK_FLAGS AND _LUAunpanic_STATIC_LINK_FLAGS)
  SET (LUAunpanic_FOUND 1 CACHE STRING "LUAunpanic is found")
ENDIF ()

SET (LUAunpanic_C_FLAGS_SHARED    "${_LUAunpanic_C_FLAGS_SHARED}"    CACHE STRING "C flags when compiling against shared LUAunpanic")
SET (LUAunpanic_CXX_FLAGS_SHARED  "${_LUAunpanic_CXX_FLAGS_SHARED}"  CACHE STRING "C flags when compiling against shared LUAunpanic")
SET (LUAunpanic_LINK_FLAGS        "${_LUAunpanic_LINK_FLAGS}"        CACHE STRING "Linker flags when linking against LUAunpanic")
SET (LUAunpanic_LDFLAGS           "${_LUAunpanic_LDFLAGS}"           CACHE STRING "Stringified version of LUAunpanic_LINK_FLAGS")
SET (LUAunpanic_STATIC_LINK_FLAGS "${_LUAunpanic_STATIC_LINK_FLAGS}" CACHE STRING "Linker flags when linking against static LUAunpanic")
SET (LUAunpanic_STATIC_LDFLAGS    "${_LUAunpanic_STATIC_LDFLAGS}"    CACHE STRING "Stringified version of LUAunpanic_STATIC_LINK_FLAGS")

IF(LUAunpanic_FOUND)
  MESSAGE(STATUS "-----------------------------------------")
  MESSAGE(STATUS "Setup LUAunpanic:")
  MESSAGE(STATUS "")
  MESSAGE(STATUS "           ROOT_DIR: ${LUAunpanic_ROOT_DIR}")
  MESSAGE(STATUS "     C_FLAGS_SHARED: ${LUAunpanic_C_FLAGS_SHARED}")
  MESSAGE(STATUS "   CXX_FLAGS_SHARED: ${LUAunpanic_CXX_FLAGS_SHARED}")
  MESSAGE(STATUS "        INCLUDE_DIR: ${LUAunpanic_INCLUDE_DIR}")
  MESSAGE(STATUS "          LIBRARIES: ${LUAunpanic_LIBRARIES}")
  MESSAGE(STATUS "   STATIC LIBRARIES: ${LUAunpanic_STATIC_LIBRARIES}")
  MESSAGE(STATUS "         LINK_FLAGS: ${LUAunpanic_LINK_FLAGS}")
  MESSAGE(STATUS "  STATIC LINK_FLAGS: ${LUAunpanic_STATIC_LINK_FLAGS}")
  MESSAGE(STATUS "            LDFLAGS: ${LUAunpanic_LDFLAGS}")
  MESSAGE(STATUS "     STATIC LDFLAGS: ${LUAunpanic_STATIC_LDFLAGS}")
  MESSAGE(STATUS "-----------------------------------------")
ENDIF()

MARK_AS_ADVANCED (
  LUAunpanic_FOUND
  LUAunpanic_C_FLAGS_SHARED
  LUAunpanic_CXX_FLAGS_SHARED
  LUAunpanic_LINK_FLAGS
  LUAunpanic_LDFLAGS
  LUAunpanic_STATIC_LINK_FLAGS
  LUAunpanic_STATIC_LDFLAGS
  )
