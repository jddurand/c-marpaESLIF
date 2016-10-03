#############################
# Global settings
#############################
CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT (marpaESLIF C CXX)

IF (POLICY CMP0063)
  CMAKE_POLICY (SET CMP0063 NEW)
ENDIF ()
IF (POLICY CMP0018)
  CMAKE_POLICY (SET CMP0018 NEW)
ENDIF ()
SET (CMAKE_C_VISIBILITY_PRESET   hidden)
SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

SET (MARPAESLIF_VERSION_MAJOR 1)
SET (MARPAESLIF_VERSION_MINOR 0)
SET (MARPAESLIF_VERSION_PATCH 0)
SET (MARPAESLIF_VERSION "${MARPAESLIF_VERSION_MAJOR}.${MARPAESLIF_VERSION_MINOR}.${MARPAESLIF_VERSION_PATCH}")

SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)           # General module search path
SET (INCLUDE_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/include)  # General include output path
SET (LIBRARY_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/lib)      # General library output path
SET (BINARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/output/bin)      # General binary output path
SET (3RDPARTY_OUTPUT_PATH           ${PROJECT_SOURCE_DIR}/output/3rdparty) # General 3rdparty output path

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
  ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF ()
IF (NOT CMAKE_BUILD_TYPE MATCHES Debug)
  ADD_DEFINITIONS(-DMARPAESLIF_NTRACE)
ENDIF (NOT CMAKE_BUILD_TYPE MATCHES Debug)
ADD_DEFINITIONS(-DMARPAESLIF_VERSION="${MARPAESLIF_VERSION}")

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

# -----------------------------------
# genericStack: required
# -----------------------------------
FIND_PACKAGE (genericStack)
IF (NOT GENERICSTACK_FOUND)
    MESSAGE (FATAL_ERROR "genericStack must be detected")
ENDIF ()

# -----------------------------------
# marpaWrapper: required
# -----------------------------------
FIND_PACKAGE (marpaWrapper)
IF (NOT MARPAWRAPPER_FOUND)
    MESSAGE (FATAL_ERROR "marpaWrapper must be detected")
ENDIF ()

# -----------------------------------
# tconv: required
# -----------------------------------
FIND_PACKAGE (tconv)
IF (NOT TCONV_FOUND)
    MESSAGE (FATAL_ERROR "tconv must be detected")
ENDIF ()

# -----------------------------------
# PCRE2: found or built-in
# -----------------------------------
FIND_PACKAGE (PCRE2)
IF (NOT PCRE2_FOUND)
    MESSAGE (STATUS "Using built-in PCRE2")
    EXECUTE_PROCESS(
      COMMAND "${CMAKE_COMMAND}" -E tar xzf "${PROJECT_SOURCE_DIR}/3rdparty/tar/pcre2-10.22.tar.gz"
      WORKING_DIRECTORY "${3RDPARTY_OUTPUT_PATH}"
      )
    INCLUDE(cmake/BuildPCRE2.cmake)
ENDIF (NOT PCRE2_FOUND)
# In any case, we want to use only the 8 bits version, that is always compiled everywhere AFAIK
ADD_DEFINITIONS(-DPCRE2_CODE_UNIT_WIDTH=8)

# --------------------------------------
# config.h (we will share it with marpa)
# --------------------------------------
INCLUDE (FindInline)
IF ("${C_INLINE}" EQUAL "inline")
  SET (C_INLINE_IS_INLINE TRUE)
ELSE ("${C_INLINE}" EQUAL "inline")
  SET (C_INLINE_IS_INLINE FALSE)
ENDIF ("${C_INLINE}" EQUAL "inline")

# ----------------------------------------------------
# Configuration
# ----------------------------------------------------
SET (CONFIG_H ${INCLUDE_OUTPUT_PATH}/config.h)
SET (CONFIG_H_IN ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in)
CONFIGURE_FILE(${CONFIG_H_IN} ${CONFIG_H})
ADD_CUSTOM_TARGET(marpaESLIFConfig SOURCES "${CONFIG_H}")

# ------------------
# marpaESLIF library
# -----------------
FILE (GLOB lib_src src/*.c)
ADD_LIBRARY (marpaESLIF SHARED ${lib_src})
INCLUDE_DIRECTORIES (
  "${INCLUDE_OUTPUT_PATH}"                             # Include path for generated files
  "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
  "${GENERICLOGGER_INCLUDE_DIRS}"                      # Include path for genericLogger (external - required)
  "${GENERICSTACK_INCLUDE_DIRS}"                       # Include path for genericStack (external - required)
  "${MARPAWRAPPER_INCLUDE_DIRS}"                       # Include path for marpaWrapper (external - required)
  "${TCONV_INCLUDE_DIRS}"                              # Include path for tconv (external - required)
  "${PCRE2_INCLUDE_DIRS}"                              # Include path for PCRE2 (external - required or built-in)
  )
TARGET_LINK_LIBRARIES (marpaESLIF ${GENERICLOGGER_LIBRARIES} ${MARPAWRAPPER_LIBRARIES} ${TCONV_LIBRARIES} ${PCRE2_LIBRARIES})
SET_TARGET_PROPERTIES (marpaESLIF
  PROPERTIES
  COMPILE_FLAGS "-DmarpaESLIF_EXPORTS"
  VERSION "${MARPAESLIF_VERSION}"
  SOVERSION "${MARPAESLIF_VERSION_MAJOR}")

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${INCLUDE_OUTPUT_PATH}/marpaESLIF")
GENERATE_EXPORT_HEADER(marpaESLIF
    BASE_NAME marpaESLIF
    EXPORT_MACRO_NAME marpaESLIF_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/marpaESLIF/export.h"
    STATIC_DEFINE marpaESLIF_STATIC)
ADD_CUSTOM_TARGET(marpaESLIFExport SOURCES "${INCLUDE_OUTPUT_PATH}/marpaESLIF/export.h")

# ----------------------------------------
# Explicit dependencies on generated files
# ----------------------------------------
ADD_DEPENDENCIES(marpaESLIF marpaESLIFConfig marpaESLIFExport pcre2)

SET_TARGET_PROPERTIES(marpaESLIF PROPERTIES COMPILE_FLAGS "-DmarpaESLIF_EXPORTS"
                                               VERSION       "${MARPAESLIF_VERSION}"
                                               SOVERSION     "${MARPAESLIF_VERSION_MAJOR}"
                                               LINKER_LANGUAGE C)


# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (marpaESLIFTester EXCLUDE_FROM_ALL test/marpaESLIFTester.c)
#
# Painful but putting the binaries produced with dynamic linking in the same directory as the
# dynamic libraries is the only way to make sure the OS will always see the dynamic library
# at runtime, REGARDLESS of the OS (we are thinking to Windows and Visual Studio for instance).
# and since we must do that for the dynamic test, we do it as well for any runtime thingy, just for coherency.
#
SET_TARGET_PROPERTIES (marpaESLIFTester       PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (marpaESLIFTester       marpaESLIF)

ADD_TEST              (NAME marpaESLIFTest
  COMMAND marpaESLIFTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS marpaESLIFTester)

# -------
# Install
# -------
INSTALL (TARGETS marpaESLIF ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (FILES ${INCLUDE_OUTPUT_PATH}/marpaESLIF/export.h DESTINATION include/marpaESLIF)
FILE (GLOB include_marpa_src "include/marpaESLIF/*.h")
INSTALL (FILES ${include_marpa_src} DESTINATION include)
INSTALL (FILES include/marpaESLIF.h DESTINATION include)

IF (UNIX)
  INCLUDE (Pod2Man)
  ADD_MANPAGE_TARGET()
  POD2MAN(README.pod "MARPAESLIF" "3" "${MARPAESLIF_VERSION}")
ENDIF (UNIX)
