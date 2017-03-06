MACRO (MYPACKAGESTART packageName versionMajor versionMinor versionPatch)
  #
  # Start
  #
  PROJECT (${packageName} VERSION ${versionMajor}.${versionMinor}.${versionPatch} LANGUAGES C CXX)  # This is setting PROJECT_NAME -;
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Starting version ${versionMajor}.${versionMinor}.${versionPatch}")
  STRING (TOUPPER ${PROJECT_NAME} _PROJECTNAME)
  SET (${_PROJECTNAME}_VERSION_MAJOR ${versionMajor})
  SET (${_PROJECTNAME}_VERSION_MINOR ${versionMinor})
  SET (${_PROJECTNAME}_VERSION_PATCH ${versionPatch})
  SET (${_PROJECTNAME}_VERSION "${${_PROJECTNAME}_VERSION_MAJOR}.${${_PROJECTNAME}_VERSION_MINOR}.${${_PROJECTNAME}_VERSION_PATCH}")
  MARK_AS_ADVANCED(
    ${_PROJECTNAME}_VERSION_MAJOR
    ${_PROJECTNAME}_VERSION_MINOR
    ${_PROJECTNAME}_VERSION_PATH
    ${_PROJECTNAME}_VERSION
    )
  #
  # Options
  #
  OPTION (ALL_IN_ONE "Compile non-system wide dependencies locally" OFF)
  OPTION (MYPACKAGE_DEBUG "Debug message from MyPackage*.cmake" OFF)
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] ALL_IN_ONE option is ${ALL_IN_ONE}")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] MYPACKAGE_DEBUG option is ${MYPACKAGE_DEBUG}")
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
  # General module search path
  #
  SET (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted CMAKE_MODULE_PATH to ${CMAKE_MODULE_PATH}")
  #
  # General include output path
  #
  SET (INCLUDE_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/output/include")
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted INCLUDE_OUTPUT_PATH to ${INCLUDE_OUTPUT_PATH}")
  #
  # General library output path
  #
  SET (LIBRARY_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/output/lib")
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted LIBRARY_OUTPUT_PATH to ${LIBRARY_OUTPUT_PATH}")
  #
  # General binary output path
  #
  SET (BINARY_OUTPUT_PATH   "${PROJECT_SOURCE_DIR}/output/bin")
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted BINARY_OUTPUT_PATH to ${BINARY_OUTPUT_PATH}")
  #
  # General 3rdparty output path
  #
  SET (3RDPARTY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/output/3rdparty")
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted 3RDPARTY_OUTPUT_PATH to ${3RDPARTY_OUTPUT_PATH}")
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
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted all output directories to ${LIBRARY_OUTPUT_PATH}")
  SET (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted CMAKE_INSTALL_RPATH to ${CMAKE_INSTALL_RPATH}")
  SET (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted CMAKE_INSTALL_RPATH_USE_LINK_PATH to ${CMAKE_INSTALL_RPATH_USE_LINK_PATH}")
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
  #
  # Prepare output directories
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Creating directory ${3RDPARTY_OUTPUT_PATH}")
  ENDIF ()
  EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${3RDPARTY_OUTPUT_PATH}")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Creating directory ${INCLUDE_OUTPUT_PATH}/${PROJECT_NAME}")
  ENDIF ()
  EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${INCLUDE_OUTPUT_PATH}/${PROJECT_NAME}")
  #
  # Set-up packaging
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Configure packaging")
  ENDIF ()
  SET (CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
  SET (CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
  SET (CPACK_PACKAGE_VERSION_PATCH "${${PROJECT_NAME}_VERSION_PATCH}")
  IF (EXISTS "${PROJECT_SOURCE_DIR}/LICENSE")
    CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/LICENSE"  "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
    SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
  ELSE ()
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] No LICENSE...")
    ENDIF ()
    SET (CPACK_RESOURCE_FILE_LICENSE)
  ENDIF ()
  #
  # We consider that every .h file in the include directory is to be installed
  # unless it is starting with an '_'.
  #
  FILE (GLOB_RECURSE _include include/*.h)
  FOREACH (_file ${_include} )
    GET_FILENAME_COMPONENT(_basename ${_file} NAME)
    STRING (REGEX MATCH "^_" _hiden ${_basename})
    IF ("${_hiden}" STREQUAL "_")
      CONTINUE ()
    ENDIF ()
    FILE (RELATIVE_PATH _relfile ${PROJECT_SOURCE_DIR} ${_file})
    GET_FILENAME_COMPONENT(_dir ${_relfile} DIRECTORY)
    INSTALL(FILES ${_file} DESTINATION ${_dir})
  ENDFOREACH()
  #
  # Make sure current project have a property associating its default directories
  #
  SET (_project_fake_include_dirs)
  FOREACH (_include_directory output/include include)
    GET_FILENAME_COMPONENT(_absolute_include_directory ${_include_directory} ABSOLUTE)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] MYPACKAGE_DEPENDENCY_${PROJECT_NAME}_FAKE_INCLUDE_DIRS appended with ${_absolute_include_directory}")
    ENDIF ()
    LIST (APPEND _project_fake_include_dirs ${_absolute_include_directory})
  ENDFOREACH ()
  SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${PROJECT_NAME}_FAKE_INCLUDE_DIRS ${_project_fake_include_dirs})
  #
  # We consider that if there is a README.pod, then it is a candidate for installation
  #
  IF (EXISTS README.pod)
    MYPACKAGEMAN(README.pod ${_PROJECTNAME} "3" "${_PROJECTNAME}_VERSION")
  ENDIF ()
  #
  # Execute common tasks
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Checking for common include files")
  ENDIF ()
  MYPACKAGECHECKCOMMONINCLUDEFILES()
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Checking for common features")
  ENDIF ()
  MYPACKAGECHECKCOMMONFEATURES()
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Checking for sizes")
  ENDIF ()
  MYPACKAGECHECKCOMMONSIZES()
ENDMACRO()
