CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART (lua534 5 3 4)

OPTION (LUA_32BITS "Use 32-bit integers and 32-bit floats" ON)
OPTION (LUA_USE_C89 "Avoid the use of few C99 feature" ON)

###########
# Library #
###########
SET (liblua534_src
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
  src/luaunpanic.c
  )
#
# We moved some lua header to include/la
#
INCLUDE_DIRECTORIES ("${CMAKE_CURRENT_SOURCE_DIR}/include/lua")

#
# OS-specifics
#
IF (WIN32 AND NOT CYGWIN)
  ADD_DEFINITIONS(-DLUA_DL_DLL)
ELSE ()
  ADD_DEFINITIONS(-DLUA_USE_DLOPEN)
  ADD_DEFINITIONS(-DLUA_USE_POSIX)
  SET (CMAKE_MATH_LIBS m)
ENDIF ()

MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/luaconf.h.in
  ${INCLUDE_OUTPUT_PATH}/lua/luaconf.h
  ${liblua534_src})
#
# Add lua/luaconf.h to installed files
#
INSTALL(FILES ${INCLUDE_OUTPUT_PATH}/lua/luaconf.h DESTINATION include/lua COMPONENT HeaderComponent)
#
# dl and m dependency
#
IF (   (NOT ("${CMAKE_DL_LIBS}" STREQUAL ""))
    OR (NOT ("${CMAKE_MATH_LIBS}" STREQUAL "")))
  FOREACH (_target lua534 lua534_static)
    TARGET_LINK_LIBRARIES(${_target} PUBLIC ${CMAKE_DL_LIBS} ${CMAKE_MATH_LIBS})
  ENDFOREACH ()
ENDIF ()
#
# Those definitions have to propagate to those using the library
#
SET (LUA_PUBLIC_CFLAGS)
IF (LUA_32BITS)
  LIST (APPEND LUA_PUBLIC_CFLAGS "-DLUA_32BITS")
ENDIF ()
IF (LUA_USE_C89)
  LIST (APPEND LUA_PUBLIC_CFLAGS "-DLUA_USE_C89")
ENDIF ()

LIST(LENGTH LUA_PUBLIC_CFLAGS LUA_PUBLIC_CFLAGS_LENGTH)
IF (LUA_PUBLIC_CFLAGS_LENGTH GREATER 0)
  FOREACH (_target lua534 lua534_static)
    TARGET_COMPILE_DEFINITIONS(${_target} PUBLIC ${LUA_PUBLIC_CFLAGS})
  ENDFOREACH ()
ENDIF ()

###############
# Executables #
###############
MYPACKAGEEXECUTABLE(lua534Exe src/lua.c)
MYPACKAGEADDEXECUTABLESTOTESTS(lua534Exe)
FOREACH (_target lua534Exe lua534Exe_static)
  IF (${_target} STREQUAL "lua534Exe")
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME lua534)
  ELSE ()
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME lua534_static)
  ENDIF ()
ENDFOREACH ()

MYPACKAGEEXECUTABLE(lua534CompilerExe src/luac.c)
MYPACKAGEADDEXECUTABLESTOTESTS(lua534CompilerExe)
FOREACH (_target lua534CompilerExe lua534CompilerExe_static)
  IF (${_target} STREQUAL "lua534CompilerExe")
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME lua534c)
  ELSE ()
    SET_TARGET_PROPERTIES(${_target} PROPERTIES OUTPUT_NAME lua534c_static)
  ENDIF ()
ENDFOREACH ()

MYPACKAGETESTEXECUTABLE(lua534wrapperTester test/luawrapperTester.c)
MYPACKAGETESTEXECUTABLE(lua534wrapperTester2 test/luawrapperTester2.c)

#########
# Tests #
#########
MYPACKAGECHECK(lua534wrapperTester ${CMAKE_CURRENT_SOURCE_DIR}/test/luawrapperTester.lua)
MYPACKAGECHECK(lua534wrapperTester2 ${CMAKE_CURRENT_SOURCE_DIR}/test/luawrapperTester2.lua)

#############
# Packaging #
#############
MYPACKAGEPACK ("" "Lua 5.3.4 with unpanic support")

#########
# Setup #
#########
MYPACKAGEPRINTSETUP()
