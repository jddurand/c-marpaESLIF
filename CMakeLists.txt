#
# Usage: With nmake: cmake -G "NMake Makefiles", then nmake install
#        With  make: cmake -G "UNIX Makefiles", then make install
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (genericLogger)

# ---------------------------
# Export no symbol by default
# ---------------------------
IF (POLICY CMP0063)
  CMAKE_POLICY (SET CMP0063 NEW)
ENDIF (POLICY CMP0063)
SET (CMAKE_C_VISIBILITY_PRESET   hidden)
SET (CMAKE_CXX_VISIBILITY_PRESET hidden)

# -------
# Version
# -------
SET (GENERICLOGGER_VERSION_MAJOR 1)
SET (GENERICLOGGER_VERSION_MINOR 0)
SET (GENERICLOGGER_VERSION_PATCH 0)
SET (GENERICLOGGER_VERSION ${GENERICLOGGER_VERSION_MAJOR}.${GENERICLOGGER_VERSION_MINOR}.${GENERICLOGGER_VERSION_PATCH})

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
SET (CONFIG_H ${INCLUDE_OUTPUT_PATH}/genericLoggerConfig.h)
SET (CONFIG_H_IN ${CMAKE_CURRENT_SOURCE_DIR}/include/genericLoggerConfig.h.in)
CONFIGURE_FILE(${CONFIG_H_IN} ${CONFIG_H})

# ------------------------------------------------------------------------------
# Build both static and shared libraries - Note the GENERICLOGGER_STATIC keyword
# ------------------------------------------------------------------------------
FILE (GLOB lib_src src/*.c)

ADD_LIBRARY (genericLogger        SHARED ${lib_src})
ADD_LIBRARY (genericLogger_static STATIC ${lib_src})

SET_TARGET_PROPERTIES (genericLogger        PROPERTIES COMPILE_FLAGS "-DgenericLogger_EXPORTS"
                                                       VERSION       "${GENERICLOGGER_VERSION}"
                                                       SOVERSION     "${GENERICLOGGER_VERSION_MAJOR}")
SET_TARGET_PROPERTIES (genericLogger_static PROPERTIES COMPILE_FLAGS "-DGENERICLOGGER_STATIC")

# ----------------------------------------------------
# Export flags - Note the GENERICLOGGER_STATIC keyword
# ----------------------------------------------------
INCLUDE (GenerateExportHeader)
ADD_COMPILER_EXPORT_FLAGS ()
GENERATE_EXPORT_HEADER(genericLogger
    BASE_NAME genericLogger
    EXPORT_MACRO_NAME GENERICLOGGER_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/genericLoggerExport.h"
    STATIC_DEFINE GENERICLOGGER_STATIC)

# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (genericLoggerTester       EXCLUDE_FROM_ALL test/genericLoggerTester.c)
ADD_EXECUTABLE        (genericLoggerTesterStatic EXCLUDE_FROM_ALL test/genericLoggerTester.c)
#
# Painful but putting the binaries produced with dynamic linking in the same directory as the
# dynamic libraries is the only way to make sure the OS will always see the dynamic library
# at runtime, REGARDLESS of the OS (we are thinking to Windows and Visual Studio for instance).
# and since we must do that for the dynamic test, we do it as well for any runtime thingy, just for coherency.
#
SET_TARGET_PROPERTIES (genericLoggerTester       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
SET_TARGET_PROPERTIES (genericLoggerTesterStatic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH} COMPILE_FLAGS "-DGENERICLOGGER_STATIC")

TARGET_LINK_LIBRARIES (genericLoggerTester       genericLogger)
TARGET_LINK_LIBRARIES (genericLoggerTesterStatic genericLogger_static)

ADD_TEST              (NAME genericLoggerTest
                       COMMAND genericLoggerTester
                       WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_TEST              (NAME genericLoggerTestStatic
                       COMMAND genericLoggerTesterStatic
                       WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})

ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS genericLoggerTester genericLoggerTesterStatic)

# -------
# Install
# -------
INSTALL (TARGETS genericLogger        ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (TARGETS genericLogger_static ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (FILES ${INCLUDE_OUTPUT_PATH}/genericLoggerExport.h include/genericLogger.h DESTINATION include)
