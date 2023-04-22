CMAKE_MINIMUM_REQUIRED (VERSION 3.15.0 FATAL_ERROR)
PROJECT(cmake_utils VERSION 1.0.11 LANGUAGES C CXX)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("../cmake/MyPackageBootstrap.cmake")
ENDIF ()

#
# Start
#
MYPACKAGESTART ()

#
# Standard CMake things are of course supported
#
INCLUDE(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(memfd_create  "sys/mman.h" HAVE_MEMFD_CREATE) # Needs _GNU_SOURCE
CHECK_SYMBOL_EXISTS(secure_getenv "stdlib.h"   HAVE_SECURE_GETENV) # Needs _GNU_SOURCE

#
# Creation of static and shared library all-at-once
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

#
# Creation of executables (statically and dynamically linked)
#
MYPACKAGEEXECUTABLE(executable bin/executable.c)

#
# Creation of test executables (statically and dynamically linked)
#
MYPACKAGETESTEXECUTABLE(test_executable bin/executable.c)

#
# *.cmake export
#
MYPACKAGECMAKEEXPORT()

#
# *.pc export
#

MYPACKAGEPKGCONFIGEXPORT()
#
# Packaging
#
MYPACKAGEPACK("Vendor" "Summary")

#
# Verbose print directory's CMake important variables
#
MYPACKAGEPRINTSETUP()
