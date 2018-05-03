CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART (lua 5 3 4)

OPTION (LUA_32BITS "Use 32-bit integers and 32-bit floats" ON)
OPTION (LUA_USE_C89 "Avoid the use of few C99 feature" ON)

IF (LUA_32BITS)
  ADD_DEFINITIONS(-DLUA_32BITS)
ENDIF ()
IF (LUA_USE_C89)
  ADD_DEFINITIONS(-DLUA_USE_C89)
ENDIF ()

#
# Historicall, some C functions required _REENTRANT or _THREAD_SAFE
# to be used in all multithreaded applications.
#
# For example, _REENTRANT and getc_unlocked() on SunOS.
#
ADD_DEFINITIONS(-D_REENTRANT)
ADD_DEFINITIONS(-D_THREAD_SAFE)

###########
# Library #
###########
SET (liblua_src
  src/lapi.c
  src/lauxlib.c
  src/lbaselib.c
  src/lbitlib.c
  src/lcode.c
  src/lcorolib.c
  src/lctype.c
  src/ldblib.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/linit.c
  src/liolib.c
  src/llex.c
  src/lmathlib.c
  src/lmem.c
  src/loadlib.c
  src/lobject.c
  src/lopcodes.c
  src/loslib.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/lstrlib.c
  src/ltable.c
  src/ltablib.c
  src/ltm.c
  src/lundump.c
  src/lutf8lib.c
  src/lvm.c
  src/lzio.c
  )

#
# OS-specifics
#
IF (WIN32 AND NOT CYGWIN)
  SET (LUA_USE_WINDOWS TRUE)
  MARK_AS_ADVANCED(LUA_USE_WINDOWS)
ELSE ()
  FIND_PACKAGE (Readline)
  INCLUDE_DIRECTORIES (${READLINE_INCLUDE_DIR})
  IF (APPLE)
    SET (LUA_USE_MACOSX TRUE)
    MARK_AS_ADVANCED(LUA_USE_MACOSX)
  ELSE ()
    SET (LUA_USE_LINUX TRUE)
    MARK_AS_ADVANCED(LUA_USE_LINUX)
  ENDIF ()
  SET (CMAKE_MATH_LIBS m)
ENDIF ()

MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/luaconf.h.in
  ${INCLUDE_OUTPUT_PATH}/luaconf.h
  ${liblua_src})

#
# dl dependency
#
IF (   (NOT ("${CMAKE_DL_LIBS}" STREQUAL ""))
    OR (NOT ("${CMAKE_MATH_LIBS}" STREQUAL ""))
    OR (NOT ("${READLINE_LIBRARIES}" STREQUAL "")))
  FOREACH (_target lua lua_static)
    TARGET_LINK_LIBRARIES(${_target} PUBLIC ${CMAKE_DL_LIBS} ${CMAKE_MATH_LIBS} ${READLINE_LIBRARIES})
  ENDFOREACH ()
ENDIF ()

###############
# Executables #
###############
MYPACKAGEEXECUTABLE(luaExe src/lua.c)
MYPACKAGEADDEXECUTABLESTOTESTS(luaExe)
FOREACH (_target luaExe luaExe_static)
  IF (${_target} STREQUAL "luaExe")
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME lua)
    IF (WIN32 AND NOT CYGWIN)
      #
      # DLL import
      #
      TARGET_COMPILE_DEFINITIONS(${_target} PRIVATE -DLUA_BUILD_AS_DLL)
    ENDIF ()
  ELSE ()
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME lua_static)
  ENDIF ()
ENDFOREACH ()

MYPACKAGEEXECUTABLE(luaCompilerExe src/luac.c)
MYPACKAGEADDEXECUTABLESTOTESTS(luaCompilerExe)
FOREACH (_target luaCompilerExe luaCompilerExe_static)
  IF (${_target} STREQUAL "luaCompilerExe")
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME luac)
    IF (WIN32 AND NOT CYGWIN)
      #
      # DLL import
      #
      TARGET_COMPILE_DEFINITIONS(${_target} PRIVATE -DLUA_BUILD_AS_DLL)
    ENDIF ()
  ELSE ()
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME luac_static)
  ENDIF ()
ENDFOREACH ()

#########
# Tests #
#########
#MYPACKAGECHECK(luaExe xxx)
#MYPACKAGECHECK(luaCompilerExe yyy)

#############
# Packaging #
#############
MYPACKAGEPACK ("" "Lua 5.3.4")

#########
# Setup #
#########
MYPACKAGEPRINTSETUP()
