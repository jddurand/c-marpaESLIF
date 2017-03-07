CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("3rdparty/github/cmake-utils-remote/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART (marpaESLIF 1 0 3)

###########
# Library #
###########
#
# We have some specifics -;
#
# ... Stream buffer size
#
IF (NOT MARPAESLIF_BUFSIZ)
  SET (MARPAESLIF_BUFSIZ "1048576")
ENDIF ()
#
# ... Portable uint32 type
#
INCLUDE (CheckTypeSize)
CHECK_TYPE_SIZE (uint32_t UINT32_T)
CHECK_TYPE_SIZE (__uint32 __UINT32)
CHECK_TYPE_SIZE ("unsigned long" UNSIGNED_LONG_INT)
CHECK_TYPE_SIZE ("unsigned int" UNSIGNED_INT)
IF (HAVE_UINT32_T)
  MESSAGE (STATUS "... Unsigned 32-bit integer type is uint32_t")
  SET (MARPSESLIF_UINT32_T uint32_t)
ELSEIF (HAVE___UINT32)
  MESSAGE (STATUS "... Unsigned 32-bit integer type is __uint32")
  SET (MARPSESLIF_UINT32 __uint32)
ELSEIF (HAVE_UNSIGNED_LONG_INT AND (${UNSIGNED_LONG_INT} EQUAL 4))
  MESSAGE (STATUS "... Unsigned 32-bit integer type is unsigned long")
  SET (MARPSESLIF_UINT32 "unsigned long")
ELSEIF (HAVE_UNSIGNED_INT AND (${UNSIGNED_INT} EQUAL 4))
  MESSAGE (STATUS "... Unsigned 32-bit integer type is unsigned int")
  SET (MARPSESLIF_UINT32 "unsigned int")
ELSE ()
  MESSAGE (FATAL_ERROR "Could not detect a valid unsigned 32-bit integer type")
ENDIF ()
#
# ... PCRE2 dependency
#
FIND_PACKAGE (PCRE2)
IF (NOT PCRE2_FOUND)
  INCLUDE(cmake/BuildPCRE2.cmake)
ENDIF ()
#
# ... Ok -;
#
MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in
  ${INCLUDE_OUTPUT_PATH}/marpaESLIF/internal/config.h
  src/marpaESLIF.c)
#
# ... Add our specifics
#
FOREACH (_target marpaESLIF marpaESLIF_static)
  TARGET_COMPILE_DEFINITIONS(${_target}
    PUBLIC -DMARPAESLIF_BUFSIZ=${MARPAESLIF_BUFSIZ}
    PUBLIC -DPCRE2_CODE_UNIT_WIDTH=8)
  TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC ${PCRE2_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES(${_target} PUBLIC ${PCRE2_LIBRARIES})
ENDFOREACH ()

################
# Dependencies #
################
MYPACKAGEDEPENDENCY (genericLogger "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericLogger-remote" LIBS)
MYPACKAGEDEPENDENCY (genericStack  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericStack-remote"  LIBS)
MYPACKAGEDEPENDENCY (marpaWrapper  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/marpaWrapper-remote"  LIBS)
MYPACKAGEDEPENDENCY (tconv         "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/tconv-remote"         LIBS)

#INCLUDE (cmake/CMakeTestSetup.txt)
#INCLUDE (cmake/CMakeDocumentationSetup.txt)
#INCLUDE (cmake/CMakeBindingsSetup.txt)
#INCLUDE (cmake/CMakePackagingSetup.txt)
