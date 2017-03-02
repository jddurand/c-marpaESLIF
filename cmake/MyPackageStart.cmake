MACRO (MYPACKAGESTART packageName versionMajor versionMinor versionPatch)
  #
  # Start
  #
  STRING (TOUPPER ${packageName} _PACKAGENAME)
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
  # Ditto for the man target, appended to install processing
  #
  IF (NOT TARGET man)
    INSTALL (CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_MAKE_PROGRAM} man)")
    ADD_CUSTOM_TARGET (man)
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
  SET (CMAKE_MODULE_PATH      "${PROJECT_SOURCE_DIR}/cmake" "${PROJECT_SOURCE_DIR}/3rdparty/github/cmake-utils-remote/cmake") # General module search path
  SET (INCLUDE_OUTPUT_PATH    "${PROJECT_SOURCE_DIR}/output/include")                                                         # General include output path
  SET (LIBRARY_OUTPUT_PATH    "${PROJECT_SOURCE_DIR}/output/lib")                                                             # General library output path
  SET (BINARY_OUTPUT_PATH     "${PROJECT_SOURCE_DIR}/output/bin")                                                             # General binary output path
  SET (3RDPARTY_OUTPUT_PATH   "${PROJECT_SOURCE_DIR}/output/3rdparty")                                                        # General 3rdparty output path
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
  SET (${_PACKAGENAME}_VERSION_MAJOR ${versionMajor})
  SET (${_PACKAGENAME}_VERSION_MINOR ${versionMinor})
  SET (${_PACKAGENAME}_VERSION_PATCH ${versionPatch})
  SET (${_PACKAGENAME}_VERSION "${${_PACKAGENAME}_VERSION_MAJOR}.${${_PACKAGENAME}_VERSION_MINOR}.${${_PACKAGENAME}_VERSION_PATCH}")

  ADD_DEFINITIONS(-D${_PACKAGENAME}_VERSION="${${_PACKAGENAME}_VERSION}")
  #
  # Prepare dependencies
  #
  EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${3RDPARTY_OUTPUT_PATH}")

ENDMACRO()
