CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("../cmake/MyPackageBootstrap.cmake")
ENDIF ()
#
# SET (MYPACKAGE_DEBUG TRUE)
#
MYPACKAGESTART (cmake_utils 1 0 0)

#
# Add a fake library that uses math
#
MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
  ${INCLUDE_OUTPUT_PATH}/test/internal/config.h
  src/test.c)

MYPACKAGEEXECUTABLE(executable bin/executable.c)

MYPACKAGETESTEXECUTABLE(test_executable bin/executable.c)

MYPACKAGEPRINTSETUP()
