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
