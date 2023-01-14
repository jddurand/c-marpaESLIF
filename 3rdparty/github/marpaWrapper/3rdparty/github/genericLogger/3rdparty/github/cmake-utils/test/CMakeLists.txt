CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
PROJECT(cmake_utils VERSION 1.0.10 LANGUAGES C CXX)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("../cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART ()

#
# This line checks that _GNU_SOURCE is correctly propagated
INCLUDE(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(memfd_create  "sys/mman.h" HAVE_MEMFD_CREATE) # Needs _GNU_SOURCE
CHECK_SYMBOL_EXISTS(secure_getenv "stdlib.h"   HAVE_SECURE_GETENV) # Needs _GNU_SOURCE

#
# Add a fake library that uses math
#
MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
  ${INCLUDE_OUTPUT_PATH}/test/internal/config.h
  src/test.c)

#
# m dependency
#
IF (CMAKE_MATH_LIBS)
  FOREACH (_target ${PROJECT_NAME} ${PROJECT_NAME}_static)
    TARGET_LINK_LIBRARIES(${_target} PUBLIC ${CMAKE_MATH_LIBS})
  ENDFOREACH ()
ENDIF ()

MYPACKAGEEXECUTABLE(executable bin/executable.c)

MYPACKAGETESTEXECUTABLE(test_executable bin/executable.c)

MYPACKAGEPRINTSETUP()
