CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART (marpaESLIF 1 0 50)

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
  TARGET_COMPILE_DEFINITIONS(${_target} PUBLIC -DMARPAESLIF_BUFSIZ=${MARPAESLIF_BUFSIZ} -DPCRE2_CODE_UNIT_WIDTH=8)
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
MYPACKAGEDEPENDENCY (genericLogger "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericLogger" LIBS)
MYPACKAGEDEPENDENCY (genericStack  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericStack"  LIBS)
MYPACKAGEDEPENDENCY (marpaWrapper  "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/marpaWrapper"  LIBS)
MYPACKAGEDEPENDENCY (tconv         "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/tconv"         LIBS)

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

############
# Bindings #
############
FIND_PACKAGE(Java)
FIND_PACKAGE(JNI)
IF (JNI_FOUND AND JAVA_FOUND)
  MESSAGE(STATUS "")
  MESSAGE(STATUS "JAVA Setup:")
  MESSAGE(STATUS "----------")
  MESSAGE(STATUS "Path to javac    : " ${Java_JAVAC_EXECUTABLE})
  MESSAGE(STATUS "Path to javadoc  : " ${Java_JAVADOC_EXECUTABLE})
  MESSAGE(STATUS "Path to jar      : " ${Java_JAR_EXECUTABLE})
  MESSAGE(STATUS "")
  MESSAGE(STATUS "JNI Setup:")
  MESSAGE(STATUS "----------")
  MESSAGE(STATUS "Path to jni.h    : " ${JAVA_INCLUDE_PATH})
  MESSAGE(STATUS "Path to jni_md.h : " ${JAVA_INCLUDE_PATH2})
  ADD_SUBDIRECTORY(src/bindings/java)
ENDIF ()

INCLUDE(FindPerl)
IF (PERL_FOUND)
  FIND_PROGRAM(DZIL_EXECUTABLE dzil)
  IF (DZIL_EXECUTABLE)
    MESSAGE(STATUS "")
    MESSAGE(STATUS "PERL Packaging will be done with dzil command, located at: ${DZIL_EXECUTABLE}")
    ADD_SUBDIRECTORY(src/bindings/perl)
    # ADD_DEPENDENCIES (package marpaESLIFPerlPackage)
  ENDIF ()
ELSE ()
  MESSAGE(STATUS "")
  MESSAGE(STATUS "PERL Packaging must be done with dzil command, please run cpan Dist::Zilla")
ENDIF ()
