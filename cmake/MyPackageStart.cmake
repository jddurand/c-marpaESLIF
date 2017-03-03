MACRO (MYPACKAGESTART packageName versionMajor versionMinor versionPatch)
  #
  # Start
  #
  PROJECT (${packageName} VERSION ${versionMajor}, ${versionMinor}, ${versionPatch} LANGUAGES C CXX)
  MESSAGE (STATUS "[${packageName}] Starting version ${versionMajor}.${versionMinor}.${versionPatch}")
  #
  # Remember project name
  #
  SET_PROPERTY(DIRECTORY ${PROJECT_SOURCE_DIR} PROPERTY MYPACKAGESTART_NAME "${packageName}")
  #
  # Options
  #
  OPTION (ALL_IN_ONE "Compile non-system wide dependencies locally" OFF)
  MESSAGE (STATUS "[${packageName}] ALL_IN_ONE option is ${ALL_IN_ONE}")
  #
  # We want to be the first one that create check target, so that we own it
  #
  GET_PROPERTY(_target_check GLOBAL PROPERTY "MYPACKAGESTART_${packageName}_TARGET_check")
  IF (NOT ${_target_check})
    MESSAGE (STATUS "[${packageName}] Creating target check")
    SET_PROPERTY(GLOBAL PROPERTY "MYPACKAGESTART_${packageName}_TARGET_check" TRUE)
    ADD_CUSTOM_TARGET (check COMMAND ${CMAKE_CTEST_COMMAND})
  ENDIF ()
  #
  # Ditto for the man target, appended to install processing
  #
  GET_PROPERTY(_target_man GLOBAL PROPERTY "MYPACKAGESTART_${packageName}_TARGET_man")
  IF (NOT ${_target_man})
    MESSAGE (STATUS "[${packageName}] Creating target man")
    SET_PROPERTY(GLOBAL PROPERTY "MYPACKAGESTART_${packageName}_TARGET_man" TRUE)
    INSTALL (CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_MAKE_PROGRAM} man)")
    ADD_CUSTOM_TARGET (man)
  ENDIF ()
  #
  # Policies
  #
  FOREACH (_policy IN ITEMS CMP0063 CMP0018)
    IF (POLICY ${_policy})
      MESSAGE (STATUS "[${packageName}] Setting policy ${_policy} to NEW")
      CMAKE_POLICY (SET ${_policy} NEW)
    ENDIF ()
  ENDFOREACH ()
  #
  # Use GNUInstallDirs in order to enforce lib64 if needed
  #
  INCLUDE (GNUInstallDirs)
  #
  # Include system libraries if needed (like on Windows)
  #
  INCLUDE (InstallRequiredSystemLibraries)
  #
  # Paths - ensure MyPackageDependency is the one distributed with this version of MyPackageStart
  #
  GET_PROPERTY(${_mypackagestart_project_source_dir} GLOBAL PROPERTY MYPACKAGESTART_PROJECT_SOURCE_DIR)
  IF (NOT ${_mypackagestart_project_source_dir})
    SET (_mypackagestart_project_source_dir "${PROJECT_SOURCE_DIR}/3rdparty/github/cmake-utils-remote/cmake")
    SET_PROPERTY(GLOBAL PROPERTY MYPACKAGESTART_PROJECT_SOURCE_DIR "${_mypackagestart_project_source_dir}")
  ENDIF ()
  SET (CMAKE_MODULE_PATH    "${PROJECT_SOURCE_DIR}/cmake" "${_mypackagestart_project_source_dir}")                          # General module search path
  SET (INCLUDE_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/output/include")                                                         # General include output path
  SET (LIBRARY_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/output/lib")                                                             # General library output path
  SET (BINARY_OUTPUT_PATH   "${PROJECT_SOURCE_DIR}/output/bin")                                                             # General binary output path
  SET (3RDPARTY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/output/3rdparty")                                                        # General 3rdparty output path
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
  GET_FILENAME_COMPONENT(projectSourceDirAbsolute ${PROJECT_SOURCE_DIR} ABSOLUTE)
  SET (_include_directories "${projectSourceDirAbsolute}/output/include" "${projectSourceDirAbsolute}/include")
  SET_PROPERTY(GLOBAL "MYPACKAGEDEPENDENCY_${packageName}_INCLUDE_DIRECTORIES" ${_include_directories})
  FOREACH (_include_directory IN LISTS ${_include_directories})
    MESSAGE(STATUS "[${_packageName}] Adding include directory ${_include_directory}")
    INCLUDE_DIRECTORIES(${_include_directory})
  ENDFOREACH ()
  #
  # ... Tracing
  #
  STRING (TOUPPER ${packageName} _PACKAGENAME)
  IF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
    ADD_DEFINITIONS(-D${_PACKAGENAME}_NTRACE)
  ENDIF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
  #
  # ... Version information
  #
  SET (${_PACKAGENAME}_VERSION_MAJOR ${versionMajor})
  SET (${_PACKAGENAME}_VERSION_MINOR ${versionMinor})
  SET (${_PACKAGENAME}_VERSION_PATCH ${versionPatch})
  SET (${_PACKAGENAME}_VERSION "${${_PACKAGENAME}_VERSION_MAJOR}.${${_PACKAGENAME}_VERSION_MINOR}.${${_PACKAGENAME}_VERSION_PATCH}")

  ADD_DEFINITIONS(-D${_PACKAGENAME}_VERSION_MAJOR=${versionMajor})
  ADD_DEFINITIONS(-D${_PACKAGENAME}_VERSION_MINOR=${versionMinor})
  ADD_DEFINITIONS(-D${_PACKAGENAME}_VERSION_PATCH=${versionPatch})
  ADD_DEFINITIONS(-D${_PACKAGENAME}_VERSION="${${_PACKAGENAME}_VERSION}")
  #
  # Prepare dependencies
  #
  EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${3RDPARTY_OUTPUT_PATH}")
  #
  # Remember we were used, so that any invocation of MyPackageDependency will use the
  # macro if this current cmake directory. So that, if we depend on a package that has
  # not been updated, we always the version of MyPackageDependency distributed together
  # with MyPackageStart
  #
ENDMACRO()
