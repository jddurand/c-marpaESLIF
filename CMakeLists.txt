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
IF (NOT HAVE_MYPACKAGE_UINT32)
  MESSAGE (FATAL_ERROR "Could not detect a valid unsigned 32-bit integer type")
ELSE ()
  MESSAGE (STATUS ".. unsigned 32-bit integer type is ${MYPACKAGE_UINT32_TYPEDEF}")
ENDIF ()
SET (MARPSESLIF_UINT32_T ${MYPACKAGE_UINT32_TYPEDEF})
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
  TARGET_COMPILE_DEFINITIONS(${_target} PUBLIC -DMARPAESLIF_BUFSIZ=${MARPAESLIF_BUFSIZ} PUBLIC -DPCRE2_CODE_UNIT_WIDTH=8)
  TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC ${PCRE2_INCLUDE_DIRS})
  TARGET_LINK_LIBRARIES     (${_target} PUBLIC ${PCRE2_LIBRARIES})
  IF (NOT PCRE2_FOUND)
    ADD_DEPENDENCIES        (${_target} pcre2)
  ENDIF ()
ENDFOREACH ()

###############
# Executables #
###############
MYPACKAGETESTEXECUTABLE(marpaESLIFTester     test/marpaESLIFTester.c)
MYPACKAGETESTEXECUTABLE(adventTester         test/adventTester.c)
MYPACKAGETESTEXECUTABLE(russellParadoxTester test/russellParadoxTester.c)
MYPACKAGETESTEXECUTABLE(jsonTester           test/jsonTester.c)
MYPACKAGETESTEXECUTABLE(selfTester           test/selfTester.c)

################
# Dependencies #
################
MYPACKAGEDEPENDENCY (genericLogger "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericLogger-remote" LIBS)
MYPACKAGEDEPENDENCY (genericStack  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericStack-remote"  LIBS)
MYPACKAGEDEPENDENCY (marpaWrapper  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/marpaWrapper-remote"  LIBS)
MYPACKAGEDEPENDENCY (tconv         "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/tconv-remote"         LIBS)

#########
# Tests #
#########
MYPACKAGECHECK(marpaESLIFTester)
MYPACKAGECHECK(adventTester)
MYPACKAGECHECK(russellParadoxTester)
MYPACKAGECHECK(jsonTester)
MYPACKAGECHECK(selfTester)

###########
# Install #
###########
MYPACKAGEMAN("doc/Tutorial/Calculator/README.pod" "MARPAESLIF_CALCULATOR" "3" "${MARPAESLIF_VERSION}")
MYPACKAGEMAN("doc/BNF/README.pod"                 "MARPAESLIF_BNF"        "3" "${MARPAESLIF_VERSION}")
MYPACKAGEMAN("doc/API/README.pod"                 "MARPAESLIF_API"        "3" "${MARPAESLIF_VERSION}")

#INCLUDE (cmake/CMakeBindingsSetup.txt)
