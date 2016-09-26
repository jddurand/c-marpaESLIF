#
# Usage: With nmake: cmake -G "NMake Makefiles", then nmake install
#        With  make: cmake -G "UNIX Makefiles", then make install
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.4)
PROJECT (genericStack)

# -------
# Version
# -------
SET (GENERICSTACK_VERSION_MAJOR 1)
SET (GENERICSTACK_VERSION_MINOR 0)
SET (GENERICSTACK_VERSION_PATCH 23)
SET (GENERICSTACK_VERSION ${GENERICSTACK_VERSION_MAJOR}.${GENERICSTACK_VERSION_MINOR}.${GENERICSTACK_VERSION_PATCH})

# --------------------
# CFLAGS customization
# --------------------
if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS("-D_CRT_SECURE_NO_WARNINGS")
  ADD_DEFINITIONS("-D_CRT_NONSTDC_NO_DEPRECATE")
ENDIF ()

# ---------
# Set paths
# ---------
SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)          # General module search path
SET (BINARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/output/bin)     # General binary output path
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)

# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (genericStackTesterDefaultInitialStack       EXCLUDE_FROM_ALL test/genericStack_defaultinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterDefaultInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
ADD_TEST              (NAME genericStackTestDefaultInitialStack    COMMAND genericStackTesterDefaultInitialStack WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

ADD_EXECUTABLE        (genericStackTesterSmallInitialStack       EXCLUDE_FROM_ALL test/genericStack_smallinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterSmallInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
ADD_TEST              (NAME genericStackTestSmallInitialStack    COMMAND genericStackTesterSmallInitialStack WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

ADD_EXECUTABLE        (genericStackTesterNoInitialStack       EXCLUDE_FROM_ALL test/genericStack_noinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterNoInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
ADD_TEST              (NAME genericStackTestNoInitialStack    COMMAND genericStackTesterNoInitialStack WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS genericStackTesterDefaultInitialStack genericStackTesterSmallInitialStack genericStackTesterNoInitialStack)

# -------
# Install
# -------
INSTALL (FILES include/genericStack.h DESTINATION include)

IF (UNIX)
  INCLUDE (Pod2Man)
  ADD_MANPAGE_TARGET()
  POD2MAN(README.pod "GENERICSTACK" "3" "${GENERICSTACK_VERSION}")
ENDIF (UNIX)
