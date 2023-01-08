MACRO (MYPACKAGESTART)
  #
  # Start
  #
  MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Starting version ${PROJECT_VERSION}")
  #
  # Options
  #
  OPTION (ALL_IN_ONE "Compile non-system wide dependencies locally" OFF)
  OPTION (MYPACKAGE_DEBUG "Debug message from MyPackage*.cmake" OFF)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] ALL_IN_ONE option is ${ALL_IN_ONE}")
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
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted CMAKE_MODULE_PATH to ${CMAKE_MODULE_PATH}")
  ENDIF ()
  #
  # General include output path
  #
  SET (INCLUDE_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/output/include")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted INCLUDE_OUTPUT_PATH to ${INCLUDE_OUTPUT_PATH}")
  ENDIF ()
  #
  # General library output path
  #
  SET (LIBRARY_OUTPUT_PATH  "${PROJECT_SOURCE_DIR}/output/lib")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted LIBRARY_OUTPUT_PATH to ${LIBRARY_OUTPUT_PATH}")
  ENDIF ()
  #
  # General binary output path
  #
  SET (BINARY_OUTPUT_PATH   "${PROJECT_SOURCE_DIR}/output/bin")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted BINARY_OUTPUT_PATH to ${BINARY_OUTPUT_PATH}")
  ENDIF ()
  #
  # General 3rdparty output path
  #
  SET (3RDPARTY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/output/3rdparty")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted 3RDPARTY_OUTPUT_PATH to ${3RDPARTY_OUTPUT_PATH}")
  ENDIF ()
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
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted all output directories to ${LIBRARY_OUTPUT_PATH}")
  ENDIF ()
  SET (CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted CMAKE_INSTALL_RPATH to ${CMAKE_INSTALL_RPATH}")
  ENDIF ()
  SET (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Setted CMAKE_INSTALL_RPATH_USE_LINK_PATH to ${CMAKE_INSTALL_RPATH_USE_LINK_PATH}")
  ENDIF ()
  #
  # Compilers settings
  #
  SET (CMAKE_C_VISIBILITY_PRESET   hidden)
  SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
  SET (CMAKE_POSITION_INDEPENDENT_CODE ON)
  SET (CMAKE_VISIBILITY_INLINES_HIDDEN 1)
  if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
    # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
    GET_DIRECTORY_PROPERTY (_DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Current COMPILE_DEFINITIONS is: ${_DirDefs}")
    ENDIF ()
    FOREACH (_definition WIN32_LEAN_AND_MEAN _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_DEPRECATE)
      LIST (FIND _DirDefs ${_definition} _index)
      IF (${_index} GREATER -1)
        IF (MYPACKAGE_DEBUG)
          MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Compile definition ${_definition} already set")
        ENDIF ()
      ELSE ()
        IF (MYPACKAGE_DEBUG)
          MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Adding compile definition ${_definition}")
        ENDIF ()
        ADD_DEFINITIONS("-D${_definition}")
        LIST(APPEND CMAKE_REQUIRED_DEFINITIONS -D${_definition})
      ENDIF ()
    ENDFOREACH ()
  ELSE ()
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] CMAKE_C_COMPILER_ID does not match MSVC: ${CMAKE_C_COMPILER_ID}")
    ENDIF ()
  ENDIF ()
  #
  # Check if it safe to define _GNU_SOURCE unless it is already defined
  #
  GET_DIRECTORY_PROPERTY (_DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS)
  LIST (FIND _DirDefs _GNU_SOURCE _index)
  IF (${_index} GREATER -1)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Compile definition _GNU_SOURCE already set")
    ENDIF ()
  ELSE ()
    INCLUDE(CheckSymbolExists)
    CHECK_SYMBOL_EXISTS(__GNU_LIBRARY__ "features.h" _GNU_SOURCE)
    IF (_GNU_SOURCE)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Adding compile definition D_GNU_SOURCE")
      ENDIF ()
      ADD_DEFINITIONS("-D_GNU_SOURCE")
      LIST(APPEND CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
    ENDIF ()
  ENDIF ()
  #
  # Regardless of compiler, we always set the following historical macros
  #
  GET_DIRECTORY_PROPERTY (_DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS)
  FOREACH (_definition _REENTRANT _THREAD_SAFE)
    LIST (FIND _DirDefs ${_definition} _index)
    IF (${_index} GREATER -1)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Compile definition ${_definition} already set")
      ENDIF ()
    ELSE ()
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-STATUS] Adding compile definition ${_definition}")
      ENDIF ()
      ADD_DEFINITIONS("-D${_definition}")
      LIST(APPEND CMAKE_REQUIRED_DEFINITIONS -D${_definition})
    ENDIF ()
  ENDFOREACH ()
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
  # We consider that every .h file in the include directory is to be installed
  # unless it is starting with an '_'.
  #
  FILE (GLOB_RECURSE _include include/*.h include/*.hpp)
  FOREACH (_file ${_include})
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Checking include ${_file}")
    ENDIF ()
    #
    # Hiden file
    #
    GET_FILENAME_COMPONENT(_basename ${_file} NAME)
    STRING (REGEX MATCH "^_" _hiden ${_basename})
    IF ("${_hiden}" STREQUAL "_")
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Skipping hiden include ${_file}")
      ENDIF ()
      CONTINUE ()
    ENDIF ()
    #
    # Internal file
    #
    GET_FILENAME_COMPONENT(_directory ${_file} DIRECTORY)
    STRING (REGEX MATCH "/internal/?" _internal ${_directory})
    IF (("${_internal}" STREQUAL "/internal") OR ("${_internal}" STREQUAL "/internal/"))
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Skipping internal include ${_file}")
      ENDIF ()
      CONTINUE ()
    ENDIF ()
    #
    # Ok
    #
    FILE (RELATIVE_PATH _relfile ${PROJECT_SOURCE_DIR} ${_file})
    GET_FILENAME_COMPONENT(_dir ${_relfile} DIRECTORY)
    INSTALL(FILES ${_file} DESTINATION ${_dir} COMPONENT HeaderComponent)
    SET (_HAVE_HEADERCOMPONENT TRUE CACHE INTERNAL "Have HeaderComponent" FORCE)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Adding install rule for ${_file}")
    ENDIF ()
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
  IF (EXISTS "${PROJECT_SOURCE_DIR}/README.pod")
    STRING (TOUPPER ${PROJECT_NAME} _PROJECTNAME)
    MYPACKAGEMAN("${PROJECT_SOURCE_DIR}/README.pod" ${_PROJECTNAME} "3" "${${PROJECT_NAME}}_VERSION")
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
  #
  # Execute repair tasks
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Checking for repair tasks")
  ENDIF ()
  MYPACKAGEREPAIR()
ENDMACRO()
