#############################
# Global settings
#############################
CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT (marpaWrapper C CXX)

IF (POLICY CMP0063)
  CMAKE_POLICY (SET CMP0063 NEW)
ENDIF ()
IF (POLICY CMP0018)
  CMAKE_POLICY (SET CMP0018 NEW)
ENDIF ()
SET (CMAKE_C_VISIBILITY_PRESET   hidden)
SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

SET (MARPA_LIB_MAJOR_VERSION "8")
SET (MARPA_LIB_MINOR_VERSION "3")
SET (MARPA_LIB_MICRO_VERSION "0")

SET (MARPAWrapper_VERSION_MAJOR 1)
SET (MARPAWrapper_VERSION_MINOR 0)
SET (MARPAWrapper_VERSION_PATCH 0)
SET (MARPAWrapper_VERSION "${MARPAWrapper_VERSION_MAJOR}.${MARPAWrapper_VERSION_MINOR}.${MARPAWrapper_VERSION_PATCH}")

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
IF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
  ADD_DEFINITIONS(-DMARPAWrapper_NTRACE)
ENDIF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
ADD_DEFINITIONS("-DMARPA_LIB_MAJOR_VERSION=${MARPA_LIB_MAJOR_VERSION}")
ADD_DEFINITIONS("-DMARPA_LIB_MICRO_VERSION=${MARPA_LIB_MICRO_VERSION}")
ADD_DEFINITIONS("-DMARPA_LIB_MINOR_VERSION=${MARPA_LIB_MINOR_VERSION}")
ADD_DEFINITIONS(-DMARPAWrapper_VERSION="${MARPAWrapper_VERSION}")

# ---------
# Set paths
# ---------
SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)          # General module search path
SET (INCLUDE_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/include) # General include output path
SET (LIBRARY_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/lib)     # General library output path

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
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("genericStack.h" HAVE_GENERICSTACK_H)
IF (NOT HAVE_GENERICSTACK_H)
    MESSAGE (FATAL_ERROR "genericStack must be detected")
ENDIF ()

# --------------------------------------
# config.h (we will share it with marpa)
# --------------------------------------
CHECK_INCLUDE_FILE("dlfcn.h"      HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("inttypes.h"   HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("memory.h"     HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("memset.h"     HAVE_MEMSET_H)
CHECK_INCLUDE_FILE("stddef.h"     HAVE_STDDEF_H)
CHECK_INCLUDE_FILE("stdin.h"      HAVE_STDINT_H)
CHECK_INCLUDE_FILE("stdlib.h"     HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("strings.h"    HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h"     HAVE_STRING_H)
CHECK_INCLUDE_FILE("sys/stat.h"   HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("sys/types.h"  HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE("unistd.h"     HAVE_UNISTD_H)
INCLUDE (CheckTypeSize)
CHECK_TYPE_SIZE("int" SIZEOF_INT)
CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)
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
ADD_CUSTOM_TARGET(marpaWrapperConfig SOURCES "${CONFIG_H}")

# -------------
# marpa library
# -------------
EXECUTE_PROCESS(
  COMMAND "${CMAKE_COMMAND}" -E copy_directory "${PROJECT_SOURCE_DIR}/3rdparty/github/Marpa--R2-remote" "${3RDPARTY_OUTPUT_PATH}/Marpa--R2-remote"
  WORKING_DIRECTORY "${3RDPARTY_OUTPUT_PATH}"
  )
SET (MARPA__R2_FOUND TRUE)
SET (MARPA__R2_DIR            "${3RDPARTY_OUTPUT_PATH}/Marpa--R2-remote")
SET (MARPA__R2_INCLUDE_DIR    "${MARPA__R2_DIR}")
SET (MARPA__R2_LIBRARY_TARGET "marpa")
FILE (GLOB lib_marpa_src "${MARPA__R2_DIR}/cpan/engine/read_only/*.c")
ADD_LIBRARY (marpa STATIC ${lib_marpa_src})
ADD_DEPENDENCIES(marpa marpaWrapperConfig)

# ------------------
# marpaWrapper library
# -----------------
FILE (GLOB lib_src src/*.c)
ADD_LIBRARY (marpaWrapper SHARED ${lib_src})
INCLUDE_DIRECTORIES (
  "${MARPA__R2_DIR}/cpan/engine/read_only/"            # Include path for marpa
  "${INCLUDE_OUTPUT_PATH}"                             # Include path for generated files
  "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
  "${GENERICLOGGER_INCLUDE_DIRS}"                      # Include path for genericLogger (external - required)
  )
TARGET_LINK_LIBRARIES (marpaWrapper "${GENERICLOGGER_LINK_FLAGS}" marpa)
SET_TARGET_PROPERTIES (marpaWrapper
  PROPERTIES
  COMPILE_FLAGS "-DmarpaWrapper_EXPORTS"
  VERSION "${MARPAWrapper_VERSION}"
  SOVERSION "${MARPAWrapper_VERSION_MAJOR}")

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${INCLUDE_OUTPUT_PATH}/marpaWrapper")
GENERATE_EXPORT_HEADER(marpaWrapper
    BASE_NAME marpaWrapper
    EXPORT_MACRO_NAME MARPAWrapper_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/marpaWrapper/export.h"
    STATIC_DEFINE MARPAWrapper_STATIC)
ADD_CUSTOM_TARGET(marpaWrapperExport SOURCES "${INCLUDE_OUTPUT_PATH}/marpaWrapper/export.h")

# ----------------------------------------
# Explicit dependencies on generated files
# ----------------------------------------
ADD_DEPENDENCIES(marpaWrapper marpaWrapperConfig marpaWrapperExport)
