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
