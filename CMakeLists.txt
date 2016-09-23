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

SET (MARPAWRAPPER_VERSION_MAJOR 1)
SET (MARPAWRAPPER_VERSION_MINOR 0)
SET (MARPAWRAPPER_VERSION_PATCH 11)
SET (MARPAWRAPPER_VERSION "${MARPAWRAPPER_VERSION_MAJOR}.${MARPAWRAPPER_VERSION_MINOR}.${MARPAWRAPPER_VERSION_PATCH}")

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
  ADD_DEFINITIONS(-DMARPAWRAPPER_NTRACE)
ENDIF (NOT CMAKE_BUILD_TYPE MATCHES Debug)
#
# We are using an embedeed static version of marpa, therefore the library version always matches
#
ADD_DEFINITIONS("-DMARPA_LIB_MAJOR_VERSION=MARPA_MAJOR_VERSION")
ADD_DEFINITIONS("-DMARPA_LIB_MICRO_VERSION=MARPA_MICRO_VERSION")
ADD_DEFINITIONS("-DMARPA_LIB_MINOR_VERSION=MARPA_MINOR_VERSION")
ADD_DEFINITIONS(-DMARPAWRAPPER_VERSION="${MARPAWRAPPER_VERSION}")

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
# genericHash: required
# -----------------------------------
FIND_PACKAGE (genericHash)
IF (NOT GENERICHASH_FOUND)
    MESSAGE (FATAL_ERROR "genericHash must be detected")
ENDIF ()

# -----------------------------------
# genericStack: required
# -----------------------------------
FIND_PACKAGE (genericStack)
IF (NOT GENERICSTACK_FOUND)
    MESSAGE (STATUS "genericStack must be detected")
ENDIF ()

# --------------------------------------
# config.h (we will share it with marpa)
# --------------------------------------
INCLUDE (CheckTypeSize)
CHECK_TYPE_SIZE("size_t" SIZEOF_SIZE_T)
INCLUDE (FindInline)
INCLUDE (FindVaCopy)
INCLUDE (FindVsnprintf)
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

# ------------------
# marpaWrapper library
# -----------------
FILE (GLOB lib_src ${lib_marpa_src} src/internal/*.c src/*.c)
ADD_LIBRARY (marpaWrapper SHARED ${lib_src})
IF (GENERICSTACK_FOUND)
  INCLUDE_DIRECTORIES (
    "${MARPA__R2_DIR}/cpan/engine/read_only/"            # Include path for marpa
    "${INCLUDE_OUTPUT_PATH}"                             # Include path for generated files
    "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
    "${GENERICLOGGER_INCLUDE_DIRS}"                      # Include path for genericLogger (external - required)
    "${GENERICHASH_INCLUDE_DIRS}"                        # Include path for genericHash (external - required)
    "${GENERICSTACK_INCLUDE_DIRS}"                       # Include path for genericStack (external - optional)
    )
ELSE (GENERICSTACK_FOUND)
  INCLUDE_DIRECTORIES (
    "${MARPA__R2_DIR}/cpan/engine/read_only/"            # Include path for marpa
    "${INCLUDE_OUTPUT_PATH}"                             # Include path for generated files
    "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
    "${GENERICLOGGER_INCLUDE_DIRS}"                      # Include path for genericLogger (external - required)
    "${GENERICHASH_INCLUDE_DIRS}"                        # Include path for genericHash (external - required)
    )
ENDIF (GENERICSTACK_FOUND)
TARGET_LINK_LIBRARIES (marpaWrapper "${GENERICLOGGER_LINK_FLAGS}")
SET_TARGET_PROPERTIES (marpaWrapper
  PROPERTIES
  COMPILE_FLAGS "-DmarpaWrapper_EXPORTS"
  VERSION "${MARPAWRAPPER_VERSION}"
  SOVERSION "${MARPAWRAPPER_VERSION_MAJOR}")

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${INCLUDE_OUTPUT_PATH}/marpaWrapper")
GENERATE_EXPORT_HEADER(marpaWrapper
    BASE_NAME marpaWrapper
    EXPORT_MACRO_NAME marpaWrapper_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/marpaWrapper/export.h"
    STATIC_DEFINE marpaWrapper_STATIC)
ADD_CUSTOM_TARGET(marpaWrapperExport SOURCES "${INCLUDE_OUTPUT_PATH}/marpaWrapper/export.h")

# ----------------------------------------
# Explicit dependencies on generated files
# ----------------------------------------
ADD_DEPENDENCIES(marpaWrapper marpaWrapperConfig marpaWrapperExport)
SET_TARGET_PROPERTIES(marpaWrapper PROPERTIES COMPILE_FLAGS "-DmarpaWrapper_EXPORTS"
                                               VERSION       "${MARPAWRAPPER_VERSION}"
                                               SOVERSION     "${MARPAWRAPPER_VERSION_MAJOR}"
                                               LINKER_LANGUAGE C)


IF (GENERICSTACK_FOUND)
  # -----
  # Check
  # -----
  ENABLE_TESTING()

  #
  # Painful but putting the binaries produced with dynamic linking in the same directory as the
  # dynamic libraries is the only way to make sure the OS will always see the dynamic library
  # at runtime, REGARDLESS of the OS (we are thinking to Windows and Visual Studio for instance).
  # and since we must do that for the dynamic test, we do it as well for any runtime thingy, just for coherency.
  #
  ADD_EXECUTABLE        (marpaWrapperTester EXCLUDE_FROM_ALL test/marpaWrapperTester.c)
  SET_TARGET_PROPERTIES (marpaWrapperTester PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
  TARGET_LINK_LIBRARIES (marpaWrapperTester marpaWrapper)

  ADD_TEST              (NAME marpaWrapperTest
    COMMAND marpaWrapperTester
    WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})

  ADD_EXECUTABLE        (marpaWrapperAsfTester EXCLUDE_FROM_ALL test/marpaWrapperAsfTester.c)
  SET_TARGET_PROPERTIES (marpaWrapperAsfTester PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
  TARGET_LINK_LIBRARIES (marpaWrapperAsfTester marpaWrapper)
  ADD_TEST              (NAME marpaWrapperAsfTest
    COMMAND marpaWrapperAsfTester
    WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})

  ADD_EXECUTABLE        (marpaWrapperAsfValueTester EXCLUDE_FROM_ALL test/marpaWrapperAsfValueTester.c)
  SET_TARGET_PROPERTIES (marpaWrapperAsfValueTester PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
  TARGET_LINK_LIBRARIES (marpaWrapperAsfValueTester marpaWrapper)
  ADD_TEST              (NAME marpaWrapperAsfValueTest
    COMMAND marpaWrapperAsfValueTester
    WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})

  ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS marpaWrapperTester marpaWrapperAsfTester marpaWrapperAsfValueTester)
ENDIF (GENERICSTACK_FOUND)

# -------
# Install
# -------
INSTALL (TARGETS marpaWrapper ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (FILES ${INCLUDE_OUTPUT_PATH}/marpaWrapper/export.h DESTINATION include/marpaWrapper)
FILE (GLOB include_marpa_src "include/marpaWrapper/*.h")
INSTALL (FILES ${include_marpa_src} DESTINATION include/marpaWrapper)
INSTALL (FILES include/marpaWrapper.h DESTINATION include)

IF (UNIX)
  INCLUDE (Pod2Man)
  ADD_MANPAGE_TARGET()
  POD2MAN(README.pod "MARPAWRAPPER" "3" "${MARPAWRAPPER_VERSION}")
ENDIF (UNIX)
