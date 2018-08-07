CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("../../../3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART (marpaESLIFLua ${marpaESLIF_VERSION_MAJOR} ${marpaESLIF_VERSION_MINOR} ${marpaESLIF_VERSION_PATCH})

INCLUDE (FindLua)
INCLUDE_DIRECTORIES (${LUA_INCLUDE_DIR})

###########
# Library #
###########
MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
  ${INCLUDE_OUTPUT_PATH}/marpaESLIFLua/internal/config.h
  marpaESLIFLua.c)

################
# Dependencies #
################
MYPACKAGEDEPENDENCY (marpaESLIF "../../.." LIBS)

# ---------------------------------------
# Lua specifics: no prefix nor versioning
# ---------------------------------------
SET_TARGET_PROPERTIES (marpaESLIFLua
  PROPERTIES
  PREFIX ""
  NO_SONAME TRUE
  )
SET_TARGET_PROPERTIES (marpaESLIFLua_static
  PROPERTIES
  PREFIX ""
  NO_SONAME TRUE
  )

#############
# Packaging #
#############
MYPACKAGEPACK ("" "Marpa Extended Scanless Interface Lua (>= 5.3) Bindings")

#########
# Setup #
#########
MYPACKAGEPRINTSETUP()

########
# Test #
########
IF (${CMAKE_COMPILER_IS_GNUCC})
  #
  # Work only compiles having rdynamic-path
  #
  FIND_PROGRAM(LUA_EXECUTABLE NAMES lua${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR} lua)
  IF (LUA_EXECUTABLE)
    MESSAGE (STATUS "Found lua executable: ${LUA_EXECUTABLE}")
    GET_PROPERTY(_test_path GLOBAL PROPERTY MYPACKAGE_TEST_PATH)
    GET_FILENAME_COMPONENT(_absolute_library_output_path ${LIBRARY_OUTPUT_PATH} ABSOLUTE)
    ADD_TEST (
      NAME marpaESLIFLuaTest
      COMMAND ${CMAKE_COMMAND} -E env "PATH=${_test_path}" "${LUA_EXECUTABLE}" ../../test/marpaESLIFLuaTest.lua
      WORKING_DIRECTORY ${_absolute_library_output_path}
      )
    ADD_DEPENDENCIES(check marpaESLIFLua)
  ENDIF ()
ENDIF ()
