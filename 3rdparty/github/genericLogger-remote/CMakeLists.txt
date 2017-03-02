CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
INCLUDE ("3rdparty/github/cmake-utils-remote/cmake/MyPackageStart.cmake")
MYPACKAGESTART (genericLogger 1 0 10)

# --------
# config.h
# --------
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("stdio.h"      HAVE_STDIO_H)
CHECK_INCLUDE_FILE("stddef.h"     HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdlib.h"     HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("stdarg.h"     HAVE_STDARG_H)
IF (NOT HAVE_STDARG_H)
  MESSAGE (FATAL "<stdarg.h> must be present")
ENDIF (NOT HAVE_STDARG_H)
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

SET (CONFIG_H ${INCLUDE_OUTPUT_PATH}/genericLogger/config.h)
SET (CONFIG_H_IN ${CMAKE_CURRENT_SOURCE_DIR}/include/genericLogger_config.h.in)
CONFIGURE_FILE(${CONFIG_H_IN} ${CONFIG_H})
ADD_CUSTOM_TARGET(genericLoggerConfig SOURCES "${CONFIG_H}")

#######################
# Include directories #
#######################
INCLUDE_DIRECTORIES(
  "${INCLUDE_OUTPUT_PATH}"                                # Include path for generated files (internal)
  "${PROJECT_SOURCE_DIR}/include"                         # Include path for the other headers (internal)
  ${GENERICLOGGER_REAL_INCLUDE_DIRS}
  )

###########
# Targets #
###########
FILE (GLOB lib_src src/*.c)

ADD_LIBRARY (genericLogger        SHARED ${lib_src})
ADD_LIBRARY (genericLogger_static STATIC ${lib_src})

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${INCLUDE_OUTPUT_PATH}/genericLogger")
GENERATE_EXPORT_HEADER(genericLogger
    BASE_NAME genericLogger
    EXPORT_MACRO_NAME genericLogger_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/genericLogger/export.h"
    STATIC_DEFINE genericLogger_STATIC)
ADD_CUSTOM_TARGET(genericLoggerExport SOURCES "${INCLUDE_OUTPUT_PATH}/genericLogger/export.h")

# -----------------------------------------------------
# Targets dependencies and properties
# -----------------------------------------------------
ADD_DEPENDENCIES(genericLogger        genericLoggerConfig genericLoggerExport)
ADD_DEPENDENCIES(genericLogger_static genericLoggerConfig genericLoggerExport)

SET_TARGET_PROPERTIES(genericLogger        PROPERTIES COMPILE_FLAGS "-DgenericLogger_EXPORTS"
                                                      VERSION       "${GENERICLOGGER_VERSION}"
                                                      SOVERSION     "${GENERICLOGGER_VERSION_MAJOR}"
                                                      LINKER_LANGUAGE C)

SET_TARGET_PROPERTIES(genericLogger_static PROPERTIES COMPILE_FLAGS "-DgenericLogger_STATIC"
                                                      VERSION       "${GENERICLOGGER_VERSION}"
                                                      SOVERSION     "${GENERICLOGGER_VERSION_MAJOR}"
                                                      LINKER_LANGUAGE C)

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
SET_TARGET_PROPERTIES (genericLoggerTester       PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
SET_TARGET_PROPERTIES (genericLoggerTesterStatic PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH} COMPILE_FLAGS "-DgenericLogger_STATIC")

TARGET_LINK_LIBRARIES (genericLoggerTester       genericLogger)
TARGET_LINK_LIBRARIES (genericLoggerTesterStatic genericLogger_static)

INCLUDE (MyPackageCheck)
MYPACKAGECHECK (genericLoggerTest       "${LIBRARY_OUTPUT_PATH}/genericLoggerTester")
MYPACKAGECHECK (genericLoggerTestStatic "${LIBRARY_OUTPUT_PATH}/genericLoggerTesterStatic")
ADD_DEPENDENCIES (check genericLoggerTester genericLoggerTesterStatic)

# -------
# Install
# -------
INSTALL (TARGETS genericLogger        ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (TARGETS genericLogger_static ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (FILES ${INCLUDE_OUTPUT_PATH}/genericLogger/export.h DESTINATION include/genericLogger)
INSTALL (FILES include/genericLogger.h DESTINATION include)

INCLUDE (MyPackageMan)
MYPACKAGEMAN("${PROJECT_SOURCE_DIR}/README.pod" "GENERICLOGGER" "3" "${GENERICLOGGER_VERSION}")

# ---------
# Packaging
# ---------
# Section CPack
SET (CPACK_PACKAGE_VERSION_MAJOR "${GENERICLOGGER_VERSION_MAJOR}")
SET (CPACK_PACKAGE_VERSION_MINOR "${GENERICLOGGER_VERSION_MINOR}")
SET (CPACK_PACKAGE_VERSION_PATCH "${GENERICLOGGER_VERSION_PATCH}")
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/LICENSE"  "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
INCLUDE (CPack)
