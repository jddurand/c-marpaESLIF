#
# Multi-config generator, e.g. Visual Studio on Windows:
#
# cmake -S c-marpaESLIF -B c-marpaESLIF-build
# cmake --build c-marpaESLIF-build --config RelWithDebInfo
# ctest --test-dir c-marpaESLIF-build -C RelWithDebInfo
# Windows:
#   cmake --install c-marpaESLIF-build --config RelWithDebInfo --prefix %cd%/c-marpaESLIF-install
# Others:
#   cmake --install c-marpaESLIF-build --config RelWithDebInfo --prefix `pwd`/c-marpaESLIF-install
# cmake --build c-marpaESLIF-build --config RelWithDebInfo --target package
#
# Single-config generator, e.g. NMake Makefiles on Windows, Unix Makefiles on Linxu:
#
# cmake -S c-marpaESLIF -B c-marpaESLIF-build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# cmake --build c-marpaESLIF-build
# ctest --test-dir c-marpaESLIF-build
# Windows:
#   cmake --install c-marpaESLIF-build --prefix %cd%/c-marpaESLIF-install
# Others:
#   cmake --install c-marpaESLIF-build --prefix `pwd`/c-marpaESLIF-install
# cmake --build c-marpaESLIF-build --target package
#
# Local tests done with: cmake -S c-marpaESLIF -B c-marpaESLIF-build -DCMAKE_HELPERS_DEBUG=OFF -DICU_ROOT=C:\icu4c-74_2-Win64-MSVC2019
#
cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
project(marpaESLIF VERSION 6.0.31 LANGUAGES C CXX)
#
# Get library helper
#
include(FetchContent)
FetchContent_Declare(cmake-helpers GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(cmake-helpers)
#
# Specific options
#
# ... Stream buffer size
#
if(NOT MARPAESLIF_BUFSIZ)
  set(MARPAESLIF_BUFSIZ 1048576)
endif()
#
# ... Portable uint32 type
#
if(NOT CMAKE_HELPERS_UINT32_TYPEDEF)
  MESSAGE (FATAL_ERROR "Could not detect a valid unsigned 32-bit integer type")
ELSE ()
  MESSAGE (STATUS ".. unsigned 32-bit integer type is ${CMAKE_HELPERS_UINT32_TYPEDEF}")
ENDIF ()
set(MARPAESLIF_UINT32_T ${CMAKE_HELPERS_UINT32_TYPEDEF})
#
# ... Portable uint64 type
#
if(NOT CMAKE_HELPERS_UINT64_TYPEDEF)
  MESSAGE (FATAL_ERROR "Could not detect a valid unsigned 64-bit integer type")
ELSE ()
  MESSAGE (STATUS ".. unsigned 64-bit integer type is ${CMAKE_HELPERS_UINT64_TYPEDEF}")
ENDIF ()
set(MARPAESLIF_UINT64_T ${CMAKE_HELPERS_UINT64_TYPEDEF})
#
# We need to know endianness in case there is no signbit
#
include(TestBigEndian)
test_big_endian(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
  set(WORDS_BIGENDIAN TRUE CACHE BOOL "Big endian library")
else()
  set(WORDS_BIGENDIAN FALSE CACHE BOOL "Big endian library")
endif()
mark_as_advanced(WORDS_BIGENDIAN)
#
# Dependencies
#
set(marpaESLIF_depends)
set(marpaESLIF_depends_ext)
set(marpaESLIF_find_dependencies)
set(marpaESLIF_extra_licenses)
#
# genericLogger dependency : public
#
set(genericLogger_git https://github.com/jddurand/c-genericLogger.git)
cmake_helpers_depend(genericLogger
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${genericLogger_git}
    GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS
    REQUIRED CONFIG
)
list(APPEND marpaESLIF_find_dependencies "genericLogger REQUIRED CONFIG")
list(APPEND marpaESLIF_depends PUBLIC genericLogger::genericLogger)
#
# genericStack dependency: internal dependency
#
set(genericStack_git https://github.com/jddurand/c-genericStack.git)
cmake_helpers_depend(genericStack
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${genericStack_git}
    GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS
    REQUIRED CONFIG
  ALWAYS_GET_SOURCES TRUE
  MAKEAVAILABLE FALSE
  SOURCE_DIR_OUTVAR genericStack_source_dir
)
list(APPEND marpaESLIF_depends_ext PRIVATE BUILD_LOCAL_INTERFACE genericStack::genericStack)
list(APPEND marpaESLIF_extra_licenses genericStack ${genericStack_source_dir}/LICENSE)
#
# genericHash dependency: internal dependency
#
set(genericHash_git https://github.com/jddurand/c-genericHash.git)
cmake_helpers_depend(genericHash
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${genericHash_git}
    GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS
    REQUIRED CONFIG
  ALWAYS_GET_SOURCES TRUE
  MAKEAVAILABLE FALSE
  SOURCE_DIR_OUTVAR genericHash_source_dir
)
list(APPEND marpaESLIF_depends_ext PRIVATE BUILD_LOCAL_INTERFACE genericHash::genericHash)
list(APPEND marpaESLIF_extra_licenses genericHash ${genericHash_source_dir}/LICENSE)
#
# marpaWrapper dependency: public
#
set(marpaWrapper_git https://github.com/jddurand/c-marpaWrapper.git)
cmake_helpers_depend(marpaWrapper
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${marpaWrapper_git}
    GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS
    REQUIRED CONFIG
)
list(APPEND marpaESLIF_find_dependencies "marpaWrapper REQUIRED CONFIG")
list(APPEND marpaESLIF_depends PUBLIC marpaWrapper::marpaWrapper)
#
# tconv dependency : public
#
set(tconv_git https://github.com/jddurand/c-tconv.git)
cmake_helpers_depend(tconv
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${tconv_git}
    GIT_SHALLOW TRUE
  FIND_PACKAGE_ARGS
    REQUIRED CONFIG
)
list(APPEND marpaESLIF_find_dependencies "tconv REQUIRED CONFIG")
list(APPEND marpaESLIF_depends PUBLIC tconv::tconv)
#
# pcre2 dependency: CMake's of pcre2 are quite catastrophic, fortunately this is ok because
# we will embed PCRE2 in our object
#
set(pcre2_version 10.42)
if(pcre2_version)
  set(pcre2_url https://github.com/PCRE2Project/pcre2/releases/download/pcre2-${pcre2_version}/pcre2-${pcre2_version}.tar.gz)
  cmake_helpers_depend(pcre2
    EXTERNALPROJECT_ADD_ARGS
      URL ${pcre2_url}
    FIND FALSE
    CONFIGURE FALSE
    BUILD FALSE
    INSTALL FALSE
    MAKEAVAILABLE FALSE
    EXCLUDE_FROM_ALL TRUE
    SOURCE_DIR_OUTVAR pcre2_source_dir
    BINARY_DIR_OUTVAR pcre2_binary_dir
  )
else()
  set(pcre2_git https://github.com/PCRE2Project/pcre2.git)
  cmake_helpers_depend(pcre2
    EXTERNALPROJECT_ADD_ARGS
      GIT_REPOSITORY ${pcre2_git}
      GIT_SHALLOW TRUE
    FIND FALSE
    CONFIGURE FALSE
    BUILD FALSE
    INSTALL FALSE
    MAKEAVAILABLE FALSE
    EXCLUDE_FROM_ALL TRUE
    SOURCE_DIR_OUTVAR pcre2_source_dir
    BINARY_DIR_OUTVAR pcre2_binary_dir
  )
endif()
block()
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
  set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
  set(PCRE2_BUILD_PCRE2_8 ON CACHE BOOL "" FORCE)
  set(PCRE2_STATIC_PIC ON CACHE BOOL "" FORCE)
  set(PCRE2_SUPPORT_JIT ON CACHE BOOL "" FORCE)
  set(PCRE2_BUILD_PCRE2GREP OFF CACHE BOOL "" FORCE)
  set(PCRE2_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  #
  # On old OSes there is no inttypes.h and pcre2 needs this file - cmake-helpers will generate it if absent.
  # We make sure the cmake-helpers options for generating missing files are ON
  #
  set(CMAKE_HELPERS_GENERATE_STDINT_H ON CACHE BOOL "Forced CMAKE_HELPERS_GENERATE_STDINT_H to ON" FORCE)
  set(CMAKE_HELPERS_GENERATE_INTTYPES_H ON CACHE BOOL "Forced CMAKE_HELPERS_GENERATE_INTTYPES_H to ON" FORCE)
  add_subdirectory(${pcre2_source_dir} ${pcre2_binary_dir} EXCLUDE_FROM_ALL)
  #
  # Make an interface library out pcre2-8-static_iface
  #
  add_library(pcre2-8-static_iface INTERFACE)
  target_link_libraries(pcre2-8-static_iface INTERFACE pcre2-8-static $<TARGET_OBJECTS:pcre2-8-static>)
  #
  # Add where cmake will generate stdint.h or inttypes.h
  #
  if((EXISTS ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/stdint.h) OR (EXISTS ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/inttypes.h))
    message(STATUS "Adding ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR} to pcre2-8-static target")
    target_include_directories(pcre2-8-static PUBLIC ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR})
  endif()
endblock()
#
# Use and forward object requirements of pcre2-8-static
#
list(APPEND marpaESLIF_depends_ext PRIVATE BUILD_LOCAL_INTERFACE pcre2-8-static_iface)
list(APPEND marpaESLIF_extra_licenses pcre2 ${pcre2_source_dir}/LICENCE)
#
# luaunpanic dependency: there is a hook in marpaESLIFLua.c that is including it
#
set(luaunpanic_git https://github.com/jddurand/c-luaunpanic.git)
cmake_helpers_depend(luaunpanic
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${luaunpanic_git}
    GIT_SHALLOW TRUE
  FIND FALSE
  CONFIGURE FALSE
  BUILD FALSE
  INSTALL FALSE
  MAKEAVAILABLE TRUE
  EXCLUDE_FROM_ALL TRUE
  SOURCE_DIR_OUTVAR luaunpanic_source_dir
  BINARY_DIR_OUTVAR luaunpanic_binary_dir
)
list(APPEND marpaESLIF_extra_licenses luaunpanic ${luaunpanic_source_dir}/LICENSE)
#
# marpaESLIFLua dependency: we just want the source
#
set(marpaESLIFLua_git https://github.com/jddurand/c-marpaESLIFLua.git)
cmake_helpers_depend(marpaESLIFLua
  EXTERNALPROJECT_ADD_ARGS
    GIT_REPOSITORY ${marpaESLIFLua_git}
    GIT_SHALLOW TRUE
    GIT_TAG origin/main
  FIND FALSE
  CONFIGURE FALSE
  BUILD FALSE
  INSTALL FALSE
  MAKEAVAILABLE FALSE
  EXCLUDE_FROM_ALL TRUE
  SOURCE_DIR_OUTVAR marpaESLIFLua_source_dir
  BINARY_DIR_OUTVAR marpaESLIFLua_binary_dir
)
list(APPEND marpaESLIF_extra_licenses marpaESLIFLua ${marpaESLIFLua_source_dir}/LICENSE)
#
# Math library
#
if(CMAKE_MATH_LIBS)
  list(APPEND marpaESLIF_depends PUBLIC ${CMAKE_MATH_LIBS})
endif()
#
# Create library
#
cmake_helpers_library(marpaESLIF
  CONFIG_ARGS                           include/marpaESLIF/internal/config.h.in include/marpaESLIF/internal/config.h
  SOURCES                               ${CMAKE_CURRENT_SOURCE_DIR}/src/marpaESLIF.c
  PODS_AUTO                             FALSE
  FIND_DEPENDENCIES                     ${marpaESLIF_find_dependencies}
  DEPENDS                               ${marpaESLIF_depends}
  DEPENDS_EXT                           ${marpaESLIF_depends_ext}
  TARGETS_OUTVAR                        targets
)
#
# PODs
#
cmake_helpers_pod(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/README.pod NAME marpaESLIF SECTION 3)
cmake_helpers_pod(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/API/README.pod NAME marpaESLIF_API SECTION 3)
cmake_helpers_pod(INPUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/BNF/README.pod NAME marpaESLIF_BNF SECTION 3)
#
# Private thingies
#
foreach(_target IN LISTS targets)
  #
  # Lua embedded specifics:
  #
  if(WIN32 AND NOT CYGWIN)
    #
    # Enable support for DLLs
    #
    set(_lua_flags -DLUA_DL_DLL)
  else()
    #
    # Enable support for dlopen/dlclose
    #
    set(_lua_flags -DLUA_USE_DLOPEN -DLUA_USE_POSIX)
  endif()
  target_compile_definitions(${_target} PRIVATE
    #
    # Embedded lua
    #
    -DMARPAESLIFLUA_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
    -DMARPAESLIFLUA_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
    -DMARPAESLIFLUA_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
    -DMARPAESLIFLUA_VERSION="${${PROJECT_NAME}_VERSION}"
    -DMARPAESLIFLUA_EMBEDDED
    ${_lua_flags}
    #
    # Embedded pcre2
    #
    -DPCRE2_CODE_UNIT_WIDTH=8
    -DPCRE2_STATIC
    #
    # Internal constant
    #
    -DMARPAESLIF_BUFSIZ=${MARPAESLIF_BUFSIZ}
  )
  target_include_directories(${_target} PRIVATE
    ${luaunpanic_binary_dir}/include/luaunpanic/lua      # For luaconf.h
    ${luaunpanic_binary_dir}/include                     # For luaunpanic/export.h
    ${luaunpanic_source_dir}/include/luaunpanic/lua      # For lauxlib.h, lua.h, lualib.h
    ${luaunpanic_source_dir}/src                         # For luaunpanic_amalgamation.c, that is explicitly included by marpaESLIFLua.c thas has a hook
    ${luaunpanic_source_dir}/include                     # For luaunpanic.h
    ${marpaESLIFLua_source_dir}/src                      # For marpaESLIFLua.c
    ${marpaESLIFLua_source_dir}/include                  # For marpaESLIFLua.h
  )
endforeach()
#
# Tests
#
include(CTest)
cmake_helpers_exe(marpaESLIFTester            INSTALL FALSE TEST TRUE SOURCES test/marpaESLIFTester.c)
cmake_helpers_exe(adventTester                INSTALL FALSE TEST TRUE SOURCES test/adventTester.c)
cmake_helpers_exe(jsonTesterStrict            INSTALL FALSE TEST TRUE SOURCES test/jsonTesterStrict.c)
cmake_helpers_exe(jsonTesterNotStrict         INSTALL FALSE TEST TRUE SOURCES test/jsonTesterNotStrict.c)
cmake_helpers_exe(jsonTesterStrictExternal    INSTALL FALSE TEST TRUE SOURCES test/jsonTesterStrictExternal.c)
cmake_helpers_exe(jsonTesterNotStrictExternal INSTALL FALSE TEST TRUE SOURCES test/jsonTesterNotStrictExternal.c)
cmake_helpers_exe(jsonTesterLua               INSTALL FALSE TEST TRUE SOURCES test/jsonTesterLua.c)
cmake_helpers_exe(jsonTesterWithSharedStream  INSTALL FALSE TEST TRUE SOURCES test/jsonTesterWithSharedStream.c)
cmake_helpers_exe(selfTester                  INSTALL FALSE TEST TRUE SOURCES test/selfTester.c)
cmake_helpers_exe(exceptionTester             INSTALL FALSE TEST TRUE SOURCES test/exceptionTester.c)
cmake_helpers_exe(exceptionTester2            INSTALL FALSE TEST TRUE SOURCES test/exceptionTester2.c)
cmake_helpers_exe(exceptionTester3            INSTALL FALSE TEST TRUE SOURCES test/exceptionTester3.c)
cmake_helpers_exe(exceptionTester4            INSTALL FALSE TEST TRUE SOURCES test/exceptionTester4.c)
cmake_helpers_exe(parameterizedTester         INSTALL FALSE TEST TRUE SOURCES test/parameterizedTester.c)
cmake_helpers_exe(allluacallbacksTester       INSTALL FALSE TEST TRUE SOURCES test/allluacallbacksTester.c)
cmake_helpers_exe(swiftTester                 INSTALL FALSE TEST TRUE SOURCES test/swiftTester.c)
#
# Package
#
cmake_helpers_package(
  EXTRA_LICENSES ${marpaESLIF_extra_licenses}
)
