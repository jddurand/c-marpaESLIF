#
# Usage: With nmake: cmake -G "NMake Makefiles", then nmake install
#        With  make: cmake -G "UNIX Makefiles", then make install
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (genericLogger)

# -------
# Version
# -------
SET (GENERICLOGGER_VERSION_MAJOR 0)
SET (GENERICLOGGER_VERSION_MINOR 1)
SET (GENERICLOGGER_VERSION_PATCH 1)

# ---------
# Set paths
# ---------
SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)          # General module search path
SET (INCLUDE_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/include) # General include output path
SET (LIBRARY_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/lib)     # General library output path
INCLUDE_DIRECTORIES (
  ${INCLUDE_OUTPUT_PATH}                               # Include path for generated files
  ${PROJECT_SOURCE_DIR}/include                        # Include path for the other headers
  )

# --------------------
# CFLAGS customization
# --------------------
if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_NONSTDC_NO_DEPRECATE")
ENDIF ()

# --------
# config.h
# --------
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("stdio.h"      HAVE_STDIO_H)
CHECK_INCLUDE_FILE("stddef.h"     HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdlib.h"     HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("stdarg.h"     HAVE_STDARG_H)
CHECK_INCLUDE_FILE("stdint.h"     HAVE_STDINT_H)
CHECK_INCLUDE_FILE("sys/time.h"   HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("sys/types.h"  HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("errno.h"      HAVE_ERRNO_H)
CHECK_INCLUDE_FILE("string.h"     HAVE_STRING_H)
CHECK_INCLUDE_FILE("unistd.h"     HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("io.h"         HAVE_IO_H)
CHECK_INCLUDE_FILE("time.h"       HAVE_TIME_H)
INCLUDE (FindInline)
INCLUDE (FindVaCopy)
INCLUDE (FindFileno)
INCLUDE (FindWrite)
INCLUDE (FindVsnprintf)
INCLUDE (FindLocaltime_r)
SET (CONFIG_H ${INCLUDE_OUTPUT_PATH}/config.h)
SET (CONFIG_H_IN ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in)
CONFIGURE_FILE(${CONFIG_H_IN} ${CONFIG_H})

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
ADD_COMPILER_EXPORT_FLAGS ()

# -------------
# Build library
# -------------
FILE(GLOB lib_src src/*.c)
FILE(GLOB include_src include/*.h ${INCLUDE_OUTPUT_PATH}/*.h)
ADD_LIBRARY (genericLogger SHARED ${lib_src} ${include_src})
SET (GENERICLOGGER_VERSION ${GENERICLOGGER_VERSION_MAJOR}.${GENERICLOGGER_VERSION_MINOR}.${GENERICLOGGER_VERSION_PATCH})
SET_TARGET_PROPERTIES(genericLogger PROPERTIES VERSION ${GENERICLOGGER_VERSION} SOVERSION ${GENERICLOGGER_VERSION_MAJOR})
GENERATE_EXPORT_HEADER(genericLogger
  EXPORT_MACRO_NAME EXPORT
  EXPORT_FILE_NAME ${INCLUDE_OUTPUT_PATH}/genericLoggerExport.h
  DEPRECATED_MACRO_NAME DEPRECATED
  NO_EXPORT_MACRO_NAME NO_EXPORT
  STATIC_DEFINE STATIC
  NO_DEPRECATED_MACRO_NAME NO_DEPRECATED
  DEFINE_NO_DEPRECATED
  PREFIX_NAME GENERICLOGGER
)

# -----
# Check
# -----
ENABLE_TESTING()
ADD_TEST              (genericLoggerTest genericLoggerTester)
ADD_EXECUTABLE        (genericLoggerTester EXCLUDE_FROM_ALL test/genericLoggerTester.c)
TARGET_LINK_LIBRARIES (genericLoggerTester genericLogger)
ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS genericLoggerTester)
