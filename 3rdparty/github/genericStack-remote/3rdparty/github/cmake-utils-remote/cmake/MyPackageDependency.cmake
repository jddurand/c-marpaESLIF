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

MACRO (MYPACKAGESTART packageName versionMajor versionMinor versionPath)
  STRING (TOUPPER ${packageName} _PACKAGENAME)
  #
  # Start
  #
  CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
  PROJECT (${packageName} C CXX)
  #
  # Policies
  #
  IF (POLICY CMP0063)
    CMAKE_POLICY (SET CMP0063 NEW)
  ENDIF ()
  IF (POLICY CMP0018)
    CMAKE_POLICY (SET CMP0018 NEW)
  ENDIF ()
  #
  # Options
  #
  OPTION (ALL_IN_ONE "Compile non-system wide dependencies locally" OFF)
  #
  # We want to be the first one that create check target, so that we own it
  #
  IF (NOT TARGET check)
    ADD_CUSTOM_TARGET (check COMMAND ${CMAKE_CTEST_COMMAND})
  ENDIF ()
  #
  # Use GNUInstallDirs in order to enforce lib64 if needed
  #
  INCLUDE (GNUInstallDirs)
  #
  # Include system libraries if needed (like on Windows)
  #
  INCLUDE (InstallRequiredSystemLibraries)
  #
  # Paths
  #
  SET (CMAKE_MODULE_PATH      "${PROJECT_SOURCE_DIR}/cmake")           # General module search path
  SET (INCLUDE_OUTPUT_PATH    "${PROJECT_SOURCE_DIR}/output/include")  # General include output path
  SET (LIBRARY_OUTPUT_PATH    "${PROJECT_SOURCE_DIR}/output/lib")      # General library output path
  SET (BINARY_OUTPUT_PATH     "${PROJECT_SOURCE_DIR}/output/bin")      # General binary output path
  SET (3RDPARTY_OUTPUT_PATH   "${PROJECT_SOURCE_DIR}/output/3rdparty") # General 3rdparty output path
  #
  # Output directories
  # C.f. http://stackoverflow.com/questions/7747857/in-cmake-how-do-i-work-around-the-debug-and-release-directories-visual-studio-2
  #
  SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
  SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
  SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
  FOREACH (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    STRING( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
    SET ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
    SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
  ENDFOREACH (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
  #
  # Compilers settings
  #
  SET (CMAKE_C_VISIBILITY_PRESET   hidden)
  SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
  SET (CMAKE_POSITION_INDEPENDENT_CODE ON)
  if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
    # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
    ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
    ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
  ENDIF ()
  IF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
    ADD_DEFINITIONS(-D${_PACKAGENAME}_NTRACE)
  ENDIF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
  SET (${_PACKAGENAME}_VERSION_MAJOR 1)
  SET (${_PACKAGENAME}_VERSION_MINOR 0)
  SET (${_PACKAGENAME}_VERSION_PATCH 16)
  SET (${_PACKAGENAME}_VERSION "${${_PACKAGENAME}_VERSION_MAJOR}.${${_PACKAGENAME}_VERSION_MINOR}.${${_PACKAGENAME}_VERSION_PATCH}")

  ADD_DEFINITIONS(-D${_PACKAGENAME}_VERSION="${${_PACKAGENAME}_VERSION}")
  #
  # Prepare dependencies
  #
  EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${3RDPARTY_OUTPUT_PATH}")
ENDMACRO()
