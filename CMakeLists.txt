#
# Usage: With nmake: cmake -G "NMake Makefiles", then nmake install
#        With  make: cmake -G "UNIX Makefiles", then make install
#
#############################
# Global settings
#############################
CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT (tconv C CXX)

IF (POLICY CMP0063)
  #
  # Honor visibility properties for all target types.
  #
  CMAKE_POLICY (SET CMP0063 NEW)
ENDIF ()
IF (POLICY CMP0018)
  #
  # Ignore CMAKE_SHARED_LIBRARY_<Lang>_FLAGS variable.
  #
  CMAKE_POLICY (SET CMP0018 NEW)
ENDIF ()

SET (CMAKE_C_VISIBILITY_PRESET   hidden)
SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

SET (TCONV_VERSION_MAJOR 1)
SET (TCONV_VERSION_MINOR 0)
SET (TCONV_VERSION_PATCH 0)
SET (TCONV_VERSION "${TCONV_VERSION_MAJOR}.${TCONV_VERSION_MINOR}.${TCONV_VERSION_PATCH}")

SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)           # General module search path
SET (INCLUDE_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/include)  # General include output path
SET (LIBRARY_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/lib)      # General library output path
SET (3RDPARTY_OUTPUT_PATH           ${PROJECT_SOURCE_DIR}/output/3rdparty) # General 3rdparty output path

# --------------------
# CFLAGS customization
# --------------------
if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF ()
IF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
  ADD_DEFINITIONS(-DTCONV_NTRACE)
ENDIF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
ADD_DEFINITIONS(-DTCONV_VERSION="${TCONV_VERSION}")

#############################
# Dependencies
#############################
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${3RDPARTY_OUTPUT_PATH}")
# -----------------------------------
# genericLogger: required
# -----------------------------------
FIND_PACKAGE (genericLogger)
IF (NOT GENERICLOGGER_FOUND)
    MESSAGE (FATAL_ERROR "genericLogger must be detected")
ENDIF ()

# ----------------------------------
# ccharset Charset Provider: builtin
# ----------------------------------
EXECUTE_PROCESS(
  COMMAND "${CMAKE_COMMAND}" -E tar xzf "${PROJECT_SOURCE_DIR}/3rdparty/tar/cchardet-1.0.0.tar.gz"
  WORKING_DIRECTORY "${3RDPARTY_OUTPUT_PATH}"
  RESULT_VARIABLE CCHARDET_TAR_RESULT
  OUTPUT_VARIABLE CCHARDET_TAR_OUTPUT
  ERROR_VARIABLE CCHARDET_TAR_ERROR
  )
SET              (CCHARDET_DIR            "${3RDPARTY_OUTPUT_PATH}/cchardet-1.0.0/src/ext/libcharsetdetect")
SET              (CCHARDET_INCLUDE        "${CCHARDET_DIR}")
SET              (CCHARDET_LIBRARY_TARGET "charsetdetect")
FILE (GLOB lib_cchardet_src
  "${3RDPARTY_OUTPUT_PATH}/cchardet-1.0.0/src/ext/libcharsetdetect/mozilla/extensions/universalchardet/src/base/*.cpp"
  "${3RDPARTY_OUTPUT_PATH}/cchardet-1.0.0/src/ext/libcharsetdetect/charsetdetect.cpp"
)
ADD_LIBRARY (charsetdetect STATIC ${lib_cchardet_src})
SET_PROPERTY (TARGET charsetdetect
  PROPERTY
  INCLUDE_DIRECTORIES
  "${3RDPARTY_OUTPUT_PATH}/cchardet-1.0.0/src/ext/libcharsetdetect/mozilla/extensions/universalchardet/src/base"
  "${3RDPARTY_OUTPUT_PATH}/cchardet-1.0.0/src/ext/libcharsetdetect/nspr-emu"
  "${3RDPARTY_OUTPUT_PATH}/cchardet-1.0.0/src/ext/libcharsetdetect"
  )
# ------------------------------------
# tinycthread Thread Provider: builtin
# ------------------------------------
EXECUTE_PROCESS(
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/3rdparty/github/tinycthread-remote" "${3RDPARTY_OUTPUT_PATH}/tinycthread-remote"
  WORKING_DIRECTORY "${3RDPARTY_OUTPUT_PATH}"
  )
SET              (TINYCTHREAD_DIR            "${3RDPARTY_OUTPUT_PATH}/tinycthread-remote")
SET              (TINYCTHREAD_INCLUDE        "${TINYCTHREAD_DIR}/source")
SET              (TINYCTHREAD_LIBRARY_TARGET "tinycthread")
ADD_LIBRARY (tinycthread STATIC "${TINYCTHREAD_DIR}/source/tinycthread.c")

# -----------------------------------------
# portable dlopen and al. Provider: builtin
# -----------------------------------------
IF (WIN32 AND NOT CYGWIN)
  MESSAGE (STATUS "dlfcn-win32 third party will be used")
  EXECUTE_PROCESS(
    COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/3rdparty/github/dlfcn-win32-remote" "${3RDPARTY_OUTPUT_PATH}/dlfcn-win32-remote"
    WORKING_DIRECTORY "${3RDPARTY_OUTPUT_PATH}"
    )
  SET              (DLFCNWIN32_DIR            "${3RDPARTY_OUTPUT_PATH}/dlfcn-win32-remote")
  SET              (DLFCNWIN32_INCLUDE        "${DLFCNWIN32_DIR}/source")
  SET              (DLFCNWIN32_LIBRARY_TARGET "dlfcn-win32")
  ADD_LIBRARY (dlfcn-win32 STATIC "${DLFCNWIN32_DIR}/dlfcn.c")
ENDIF (WIN32 AND NOT CYGWIN)

# -----------------------------------
# ICU: optional
# -----------------------------------
FIND_PACKAGE (ICU)
IF (NOT ICU_FOUND)
    MESSAGE (STATUS "ICU is not detected")
ENDIF (NOT ICU_FOUND)

# -----------------------------------
# ICONV: optional
# -----------------------------------
FIND_PACKAGE (Iconv)
IF (NOT ICONV_FOUND)
  IF (WIN32 AND NOT CYGWIN)
    # -------------------------
    # win-iconv Text Provider
    # -------------------------
    MESSAGE (STATUS "win-iconv third party will be used")
    EXECUTE_PROCESS(
      COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/3rdparty/github/winiconv-remote" "${3RDPARTY_OUTPUT_PATH}/winiconv-remote"
      WORKING_DIRECTORY "${3RDPARTY_OUTPUT_PATH}"
      )
    SET (WINICONV_DIR            "${3RDPARTY_OUTPUT_PATH}/winiconv-remote")
    SET (WINICONV_INCLUDE        "${WINICONV_DIR}")
    SET (WINICONV_SRC            "${WINICONV_DIR}/win_iconv.c")
    SET (ICONV_FOUND TRUE)
    SET (ICONV_INCLUDE_DIR       "${WINICONV_INCLUDE}")
    SET (ICONV_SECOND_ARGUMENT_IS_CONST TRUE)
  ELSE (WIN32 AND NOT CYGWIN)
    MESSAGE (STATUS "Iconv is not detected")
  ENDIF (WIN32 AND NOT CYGWIN)
ENDIF (NOT ICONV_FOUND)

#############################
# Configuration
#############################
SET               (CONFIG_H ${INCLUDE_OUTPUT_PATH}/tconv_config.h)
SET               (CONFIG_H_IN ${CMAKE_CURRENT_SOURCE_DIR}/include/tconv_config.h.in)
INCLUDE           (FindInline)
SET               (TCONV_C_INLINE "${C_INLINE}")
CONFIGURE_FILE    (${CONFIG_H_IN} ${CONFIG_H})

#############################
# Configuration result impact
#############################
INCLUDE_DIRECTORIES(
  "${INCLUDE_OUTPUT_PATH}"                                # Include path for generated files (internal)
  "${PROJECT_SOURCE_DIR}/include"                         # Include path for the other headers (internal)
  "${CCHARDET_INCLUDE}"                                   # Include path for ccharset (internal)
  "${GENERICLOGGER_INCLUDE_DIRS}"                         # Include path for genericLogger (external - required)
  )
IF (WINICONV_INCLUDE)
  INCLUDE_DIRECTORIES ("${WINICONV_INCLUDE}")
ENDIF (WINICONV_INCLUDE)
IF (ICU_INCLUDE_DIRS)
  INCLUDE_DIRECTORIES ("${ICU_INCLUDE_DIRS}")
ENDIF (ICU_INCLUDE_DIRS)

# SET (CMAKE_CPP_FLAGS           "${CMAKE_CPP_FLAGS}                $ENV{CPPFLAGS} ${ICU_CPP_FLAGS}")
SET (CMAKE_C_FLAGS             "${CMAKE_C_FLAGS}                  $ENV{CFLAGS}   ${ICU_C_FLAGS_SHARED} ${GENERICLOGGER_C_FLAGS_SHARED}")
SET (CMAKE_CXX_FLAGS           "${CMAKE_CXX_FLAGS}                $ENV{CFLAGS}   ${ICU_CXX_FLAGS_SHARED} ${GENERICLOGGER_C_FLAGS_SHARED}")
SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} $ENV{LDFLAGS}  ${ICU_LDFLAGS} ${GENERICLOGGER_LDFLAGS}")
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS_INIT} $ENV{LDFLAGS}  ${ICU_LDFLAGS} ${GENERICLOGGER_LDFLAGS}")
SET (CMAKE_EXE_LINKER_FLAGS    "${CMAKE_EXE_LINKER_FLAGS_INIT}    $ENV{LDFLAGS}  ${ICU_LDFLAGS} ${GENERICLOGGER_LDFLAGS}")

#############################
# Makefile details
#############################
FILE (GLOB lib_src src/*.c)
FILE (GLOB lib_src_charset_cchardet src/charset/cchardet.c)
IF (ICU_FOUND)
  FILE (GLOB lib_src_charset_ICU src/charset/ICU.c)
  FILE (GLOB lib_src_convert_ICU src/convert/ICU.c)
ENDIF (ICU_FOUND)
IF (ICONV_FOUND)
  IF (WINICONV_SRC)
    FILE (GLOB lib_src_convert_iconv src/convert/iconv.c "${WINICONV_SRC}")
  ELSE (WINICONV_SRC)
    FILE (GLOB lib_src_convert_iconv src/convert/iconv.c)
  ENDIF (WINICONV_SRC)
ENDIF (ICONV_FOUND)
ADD_LIBRARY (tconv SHARED
  ${lib_src}
  ${lib_src_charset_cchardet}
  ${lib_src_charset_ICU}
  ${lib_src_convert_ICU}
  ${lib_src_convert_iconv})
TARGET_LINK_LIBRARIES (tconv
  "${CCHARDET_LIBRARY_TARGET}"
  "${TINYCTHREAD_LIBRARY_TARGET}"
  "${DLFCNWIN32_LIBRARY_TARGET}"
)
SET_TARGET_PROPERTIES (tconv
                       PROPERTIES COMPILE_FLAGS "-Dtconv_EXPORTS"
                       VERSION                  "${TCONV_VERSION}"
                       SOVERSION                "${TCONV_VERSION_MAJOR}")

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
ADD_COMPILER_EXPORT_FLAGS ()
GENERATE_EXPORT_HEADER(tconv
    BASE_NAME tconv
    EXPORT_MACRO_NAME TCONV_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/tconv_export.h"
    STATIC_DEFINE TCONV_STATIC)

# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE (tconvTester EXCLUDE_FROM_ALL test/tconv.c)
#
# Painful but putting the binaries produced with dynamic linking in the same directory as the
# dynamic libraries is the only way to make sure the OS will always see the dynamic library
# at runtime, REGARDLESS of the OS (we are thinking to Windows and Visual Studio for instance).
# and since we must do that for the dynamic test, we do it as well for any runtime thingy, just for coherency.
#
SET_TARGET_PROPERTIES (tconvTester PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (tconvTester tconv)
ADD_TEST              (NAME tconvTest
                       COMMAND tconvTester
                       WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS tconvTester)
#
# Please note that test is likely to fail if libraries are not already in your path
# (I did not want to play with PATH/LD_LIBRARY_PATH/etc within cmake)

# -------
# Install
# -------
INSTALL (TARGETS tconv ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (FILES ${INCLUDE_OUTPUT_PATH}/tconvExport.h include/tconv.h DESTINATION include)
