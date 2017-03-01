#
# All /my/ packages follow this convention:
# - a package Xx has a FindXx's cmake, that will set XX_FOUND, XX_INCLUDE_DIRS, XX_ROOT_DIR variables if success
#   In case of success XX_LIBRARIES is always XX_ROOT_DIR/lib
# - there is a target Xx that is the library
# - there is always an include directory
# - build always generate headers in output/include
# - Tests always overwrite PATH with value of variable TEST_PATH
# - Sourcedir of package Xx is always inside current package as 3rdparty/github/Xx
# - There all under github at https://github.com/jddurand/c-Xx.git
#
MACRO (MYPACKAGEDEPENDENCY dependTarget dependSourceDir dependIncludeDirsVarname)
  STRING (TOUPPER ${dependTarget} _FINDPACKAGE)
  IF (ALL_IN_ONE)
    IF (NOT TARGET ${dependTarget})
      #
      # Absolute path, otherwise using it as include dir for compilation will croak
      #
      GET_FILENAME_COMPONENT(dependSourceDirAbsolute ${dependSourceDir} ABSOLUTE)

      MESSAGE(STATUS "Using ${dependTarget} target from subdirectory ${dependSourceDirAbsolute}")
      ADD_SUBDIRECTORY(${dependSourceDirAbsolute})
      SET (${dependIncludeDirsVarname}
        "${dependSourceDirAbsolute}/output/include"
        "${dependSourceDirAbsolute}/include"
        )
      #
      # Test path management
      #
      SET (_dependLibraryRuntimeDirectory "${dependSourceDirAbsolute}/output/lib")
      IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
        STRING(REGEX REPLACE "/" "\\\\"  _dependLibraryRuntimeDirectory "${_dependLibraryRuntimeDirectory}")
      ELSE ()
        STRING(REGEX REPLACE " " "\\\\ "  _dependLibraryRuntimeDirectory "${_dependLibraryRuntimeDirectory}")
      ENDIF ()
      SET(${_FINDPACKAGE}_RUNTIME_DIRECTORY "${_dependLibraryRuntimeDirectory}" CACHE INTERNAL "${dependTarget} Runtime Directory")
      IF (NOT TEST_PATH)
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
    ELSE ()
      MESSAGE(STATUS "Re-using already existing ${dependTarget} target")
    ENDIF ()
  ELSE ()
    MESSAGE(STATUS "Looking for ${dependTarget}")
    FIND_PACKAGE (${dependTarget})
    IF (NOT ${_FINDPACKAGE}_FOUND)
      MESSAGE (FATAL_ERROR "find ${dependTarget} failure")
    ENDIF ()
    #
    # The following lines are experimental
    #
    # IF (NOT ${_FINDPACKAGE}_FOUND)
    #   SET (_GIT_REPOSITORY "https://github.com/jddurand/c-${dependTarget}.git")
    #   GET_FILENAME_COMPONENT(_INSTALL_DIR "output/3rdparty/${dependTarget}" ABSOLUTE)
    #    MESSAGE (STATUS "find ${dependTarget} failure - trying with ${_GIT_REPOSITORY}")
    #    INCLUDE(ExternalProject)
    #    ExternalProject_Add("${dependTarget}ExternalProject"
    #      GIT_REPOSITORY ${_GIT_REPOSITORY}
    #      INSTALL_DIR ${_INSTALL_DIR}
    #      LOG_DOWNLOAD 1
    #      LOG_CONFIGURE 1
    #      LOG_BUILD 1
    #      LOG_INSTALL 1
    #      )
    #    #
    #    # Cross the fingers that this will work -;
    #    #
    #    SET (${_FINDPACKAGE}_ROOT_DIR     ${_INSTALL_DIR})
    #    SET (${_FINDPACKAGE}_INCLUDE_DIRS "${_INSTALL_DIR}/include")
    #  ENDIF ()
    SET (${dependIncludeDirsVarname}  "${${_FINDPACKAGE}_INCLUDE_DIRS}" )
    #
    # Test path management
    #
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      SET(_${_FINDPACKAGE}_RUNTIME_DIRECTORY "${${_FINDPACKAGE}_ROOT_DIR}/bin")
      STRING(REGEX REPLACE "/" "\\\\"  _${_FINDPACKAGE}_RUNTIME_DIRECTORY "${_${_FINDPACKAGE}_RUNTIME_DIRECTORY}")
    ELSE ()
      SET(_${_FINDPACKAGE}_RUNTIME_DIRECTORY "${${_FINDPACKAGE}_ROOT_DIR}/lib")
      STRING(REGEX REPLACE " " "\\\\ "  _${_FINDPACKAGE}_RUNTIME_DIRECTORY "${_${_FINDPACKAGE}_RUNTIME_DIRECTORY}")
    ENDIF ()
    SET(${_FINDPACKAGE}_RUNTIME_DIRECTORY "${_${_FINDPACKAGE}_RUNTIME_DIRECTORY}" CACHE INTERNAL "${dependTarget} Runtime Directory")
    IF (NOT TEST_PATH)
      MESSAGE(STATUS "Initializing TEST_PATH with PATH")
      SET (TEST_PATH "$ENV{PATH}")
    ENDIF ()
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      SET (SEP "\\;")
    ELSE ()
      SET (SEP ":")
    ENDIF ()
    SET (TEST_PATH "${${_FINDPACKAGE}_RUNTIME_DIRECTORY}${SEP}${TEST_PATH}")
    MESSAGE(STATUS "Added ${${_FINDPACKAGE}_RUNTIME_DIRECTORY} to test path")
  ENDIF ()
  MESSAGE(STATUS "Setted ${dependIncludeDirsVarname} to ${${dependIncludeDirsVarname}}")
ENDMACRO()
