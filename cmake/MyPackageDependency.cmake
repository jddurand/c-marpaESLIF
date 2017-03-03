MACRO (MYPACKAGEDEPENDENCY packageDepend packageDependSourceDir)
  #
  # Get current project name - empty and it is ok if caller was not using MyPackageStart
  #
  GET_PROPERTY(_packageName DIRECTORY "${PROJECT_SOURCE_DIR}" PROPERTY MYPACKAGESTART_NAME)
  IF (NOT ${_packageName})
    #
    # Was not using MyPackageStart ? Guess the name from the basename, removing -remote, -surround
    # if any.
    #
    GET_FILENAME_COMPONENT(_packageName ${PROJECT_SOURCE_DIR} NAME)
    STRING(REGEX REPLACE "-remote.*" ""  _packageName "${_packageName}")
    STRING(REGEX REPLACE "-surround.*" ""  _packageName "${_packageName}")
    SET_PROPERTY(_packageName DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY MYPACKAGESTART_NAME)
    MESSAGE (WARNING "[${_packageName}] Package name guess - use MyPackageStart to avoid this warning")
  ENDIF ()
  #
  # Check if inclusion was already done - via us or another mechanism... guessed with TARGET check
  #
  GET_PROPERTY(_depend GLOBAL PROPERTY "MYPACKAGEDEPENDENCY_${packageDepend}")
  IF ((${_depend}) OR (TARGET ${packageDepend}))
    IF (${_depend})
      IF (${_depend} GREATER 0)
        MESSAGE (STATUS "[${_packageName}] ${packageDepend} is already being processed")
      ELSE ()
        MESSAGE (STATUS "[${_packageName}] ${packageDepend} is already available")
      ENDIF ()
    ELSE ()
      MESSAGE (WARNING "[${_packageName}] Target ${packageDepend} already exist - use MyPackageDependency to avoid this warning")
      SET_PROPERTY(GLOBAL PROPERTY "MYPACKAGEDEPENDENCY_${packageDepend}" -1)
    ENDIF ()
  ELSE ()
    MESSAGE (STATUS "[${_packageName}] ${packageDepend} is not yet available")
    SET_PROPERTY(GLOBAL PROPERTY "MYPACKAGEDEPENDENCY_${packageDepend}" 1)
    IF (ALL_IN_ONE)
      GET_FILENAME_COMPONENT(packageDependSourceDirAbsolute ${packageDependSourceDir} ABSOLUTE)
      MESSAGE (STATUS "[${_packageName}] Adding subdirectory ${packageDependSourceDirAbsolute}")
      ADD_SUBDIRECTORY(${packageDependSourceDirAbsolute})
    ELSE ()
      MESSAGE(STATUS "[${_packageName}] Looking for ${packageDepend}")
      FIND_PACKAGE (${packageDepend})
      STRING (TOUPPER ${packageDepend} _FINDPACKAGE)
      IF (NOT ${_FINDPACKAGE}_FOUND)
        MESSAGE (FATAL_ERROR "[${_packageName}] Find ${packageDepend} failed")
      ENDIF ()
    ENDIF ()
    SET_PROPERTY(GLOBAL PROPERTY "MYPACKAGEDEPENDENCY_${packageDepend}" -1)
    #
    # Generate the dependencies on the current level, remembering them so that
    # higher levels inherit
    #
    # Include directories of packageDepend
    #
    GET_PROPERTY(_include_directories_packageDepend GLOBAL "MYPACKAGEDEPENDENCY_${packageDepend}_INCLUDE_DIRECTORIES")
    IF (NOT ${_include_directories_packageDepend})
      SET (_include_directories_packageDepend "${packageDependSourceDirAbsolute}/output/include" "${packageDependSourceDirAbsolute}/include")
      SET_PROPERTY(GLOBAL "MYPACKAGEDEPENDENCY_${packageDepend}_INCLUDE_DIRECTORIES" ${_include_directories_packageDepend})
    ENDIF ()
    #
    # Current project's include directories
    #
    FOREACH (_include_directory IN LISTS ${_include_directories_packageDepend})
      MESSAGE(STATUS "[${_packageName}] Adding include directory ${_include_directory}")
      INCLUDE_DIRECTORIES(${_include_directory})
    ENDFOREACH ()
    #
    # Test path management
    #
    SET (_dependLibraryRuntimeDirectory "${packageDependSourceDirAbsolute}/output/lib")
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      STRING(REGEX REPLACE "/" "\\\\"  _dependLibraryRuntimeDirectory "${_dependLibraryRuntimeDirectory}")
    ELSE ()
      STRING(REGEX REPLACE " " "\\\\ "  _dependLibraryRuntimeDirectory "${_dependLibraryRuntimeDirectory}")
    ENDIF ()
    SET(${_FINDPACKAGE}_RUNTIME_DIRECTORY "${_dependLibraryRuntimeDirectory}" CACHE INTERNAL "${packageDepend} Runtime Directory")
    IF (NOT ${TEST_PATH})
      MESSAGE(STATUS "Initializing TEST_PATH with PATH")
      SET (TEST_PATH "$ENV{PATH}" )
    ENDIF ()
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      SET (SEP "\\;")
    ELSE ()
      SET (SEP ":")
    ENDIF ()
    SET (TEST_PATH "${${_FINDPACKAGE}_RUNTIME_DIRECTORY}${SEP}${TEST_PATH}")
    MESSAGE(STATUS "Added ${${_FINDPACKAGE}_RUNTIME_DIRECTORY} to test path")
  ENDIF ()
ENDMACRO()
