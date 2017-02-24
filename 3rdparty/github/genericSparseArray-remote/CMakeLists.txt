#
# Usage: With nmake: cmake -G "NMake Makefiles", then nmake install
#        With  make: cmake -G "UNIX Makefiles", then make install
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.4)
PROJECT (genericSparseArray)

SET (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)           # General module search path

# -------
# Version
# -------
SET (GENERICSPARSEARRAY_VERSION_MAJOR 1)
SET (GENERICSPARSEARRAY_VERSION_MINOR 0)
SET (GENERICSPARSEARRAY_VERSION_PATCH 7)
SET (GENERICSPARSEARRAY_VERSION ${GENERICSPARSEARRAY_VERSION_MAJOR}.${GENERICSPARSEARRAY_VERSION_MINOR}.${GENERICSPARSEARRAY_VERSION_PATCH})

SET (INCLUDE_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/include)  # General include output path
SET (LIBRARY_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/lib)      # General library output path
SET (BINARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/output/bin)      # General binary output path

# For all outputs directories
# C.f. http://stackoverflow.com/questions/7747857/in-cmake-how-do-i-work-around-the-debug-and-release-directories-visual-studio-2
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
FOREACH (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    STRING( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
    SET ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
    SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
ENDFOREACH (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# --------------------
# CFLAGS customization
# --------------------
if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_NONSTDC_NO_DEPRECATE")
ENDIF ()

#############################
# Dependencies
#############################
# ----------------------
# genericStack: required
# ----------------------
FIND_PACKAGE (genericStack)
IF (NOT GENERICSTACK_FOUND)
    MESSAGE (FATAL_ERROR "genericStack must be detected")
ENDIF ()
# ----------------------
# genericHash: required
# ----------------------
FIND_PACKAGE (genericHash)
IF (NOT GENERICHASH_FOUND)
    MESSAGE (FATAL_ERROR "genericHash must be detected")
ENDIF ()

# -----------------------------------
# genericLogger: required (for tests)
# -----------------------------------
FIND_PACKAGE (genericLogger)
IF (NOT GENERICLOGGER_FOUND)
    MESSAGE (STATUS "genericLogger must be detected for tests")
ENDIF ()

# ---------
# Set paths
# ---------
SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)          # General module search path
SET (BINARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/output/bin)     # General binary output path
IF (GENERICLOGGER_FOUND)
  INCLUDE_DIRECTORIES (
    "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
    "${GENERICLOGGER_INCLUDE_DIRS}"                      # Include path for genericLogger (external - required)
    "${GENERICSTACK_INCLUDE_DIRS}"                       # Include path for genericStack (external - required)
    "${GENERICHASH_INCLUDE_DIRS}"                        # Include path for genericHash (external - required)
    )
ELSE (GENERICLOGGER_FOUND)
  INCLUDE_DIRECTORIES (
    "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
    "${GENERICSTACK_INCLUDE_DIRS}"                       # Include path for genericStack (external - required)
    "${GENERICHASH_INCLUDE_DIRS}"                        # Include path for genericHash (external - required)
    )
ENDIF (GENERICLOGGER_FOUND)

IF (GENERICLOGGER_FOUND)
  # -----
  # Check
  # -----
  ENABLE_TESTING()

  ADD_EXECUTABLE        (genericSparseArrayTester       EXCLUDE_FROM_ALL test/genericSparseArray.c)
  ADD_EXECUTABLE        (genericSparseArrayTesterStatic EXCLUDE_FROM_ALL test/genericSparseArray.c)

  TARGET_LINK_LIBRARIES (genericSparseArrayTester       ${GENERICLOGGER_LIBRARIES})
  TARGET_LINK_LIBRARIES (genericSparseArrayTesterStatic ${GENERICLOGGER_STATIC_LIBRARIES})

  SET_TARGET_PROPERTIES (genericSparseArrayTester       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
  SET_TARGET_PROPERTIES (genericSparseArrayTesterStatic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})

  ADD_TEST              (NAME genericSparseArrayTest       COMMAND genericSparseArrayTester WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})
  ADD_TEST              (NAME genericSparseArrayTestStatic COMMAND genericSparseArrayTester WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

  ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS genericSparseArrayTester genericSparseArrayTesterStatic)
ENDIF (GENERICLOGGER_FOUND)

# -------
# Install
# -------
INSTALL (FILES include/genericSparseArray.h DESTINATION include)

IF (UNIX)
  INCLUDE (Pod2Man)
  ADD_MANPAGE_TARGET()
  POD2MAN(README.pod "GENERICSPARSEARRAY" "3" "${GENERICSPARSEARRAY_VERSION}")
ENDIF (UNIX)
