CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("../../../3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
# marpaESLIFJava version is synced with marpaESLIF
MYPACKAGESTART (marpaESLIFJava ${marpaESLIF_VERSION_MAJOR} ${marpaESLIF_VERSION_MINOR} ${marpaESLIF_VERSION_PATCH})

INCLUDE (UseJava)
INCLUDE_DIRECTORIES (${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})

#
# This is to setup a hook trying to shut down gcc (at least) warning when
# doing: char *x; jlong l = (jlong) x
#
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE("stdint.h" HAVE_STDINT_H)
IF (HAVE_STDINT_H)
  ADD_DEFINITIONS(-DHAVE_STDINT_H)
ENDIF ()
INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE(intptr_t INTPTR_T)
IF (HAVE_INTPTR_T)
  MESSAGE(STATUS "Using intptr_t for casting a pointer to jlong")
  ADD_DEFINITIONS(-DHAVE_INTPTR_T)
ENDIF ()

# ------------------------------------------
# JNI
# ------------------------------------------
FILE (GLOB lib_src_java marpaESLIFJava.c)
ADD_LIBRARY (marpaESLIFJava SHARED ${lib_src_java})
TARGET_LINK_LIBRARIES (marpaESLIFJava marpaESLIF ${GENERICLOGGER_LIBRARIES})
SET_TARGET_PROPERTIES (marpaESLIFJava
  PROPERTIES
  VERSION "${${PROJECT_NAME}_VERSION}"
  SOVERSION "${${PROJECT_NAME}_VERSION_MAJOR}")
TARGET_COMPILE_DEFINITIONS(marpaESLIFJava
      PRIVATE -DMARPAESLIFJAVA_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
      PRIVATE -DMARPAESLIFJAVA_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
      PRIVATE -DMARPAESLIFJAVA_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
      PRIVATE -DMARPAESLIFJAVA_VERSION="${${PROJECT_NAME}_VERSION}"
      )
INSTALL (TARGETS marpaESLIFJava ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)

################
# Dependencies #
################
MYPACKAGEDEPENDENCY (genericStack  "${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/github/genericStack"  LIBS LOCAL STATIC)

# ------------------------------------------
# JAVA
# ------------------------------------------
FILE (GLOB_RECURSE MARPAESLIFJAVA_SRC src "*.java")
SET (CMAKE_JNI_TARGET TRUE)
SET (CMAKE_JAVA_TARGET_VERSION 1.4.0)
# Fix for cmake 3.2, preventing "target marpaESLIFJava_jar is not known in this scope" message
SET (CMAKE_JAVA_TARGET_OUTPUT_NAME marpaESLIF)
ADD_JAR(marpaESLIFJava_jar
        SOURCES ${MARPAESLIFJAVA_SRC}
        VERSION ${${PROJECT_NAME}_VERSION}
        OUTPUT_NAME marpaESLIF
        )
ADD_DEPENDENCIES(marpaESLIFJava_jar marpaESLIFJava)
INSTALL_JAR(marpaESLIFJava_jar share/java)
INSTALL_JNI_SYMLINK(marpaESLIFJava_jar share/java)

# ------------------------------------------
# DOCUMENTATION
# ------------------------------------------
CREATE_JAVADOC(marpaESLIF
               PACKAGES org.parser.marpa
               SOURCEPATH src
               CLASSPATH ${CMAKE_JAVA_INCLUDE_PATH}
               DOCTITLE "marpaESLIF Java Bindings"
               WINDOWTITLE "marpaESLIF"
               AUTHOR TRUE
               USE TRUE
               VERSION TRUE
               )

# ------------------------------------------------
# TEST (executed with make check at the top level)
# ------------------------------------------------
FILE (GLOB_RECURSE MARPAESLIFJAVA_TEST test "*.java")
SET (CMAKE_JNI_TARGET FALSE)
SET (CMAKE_JAVA_TARGET_VERSION FALSE)
ADD_JAR(marpaESLIFJavaTest_jar
        SOURCES ${MARPAESLIFJAVA_TEST}
        ENTRY_POINT org/parser/marpa/AppParse
        OUTPUT_NAME marpaESLIFJavaTest
        )
ADD_DEPENDENCIES(marpaESLIFJavaTest_jar marpaESLIFJava_jar)
   
# -----
# Check
# -----
IF (${CMAKE_COMPILER_IS_GNUCC})
  #
  # Works only when compiler support rdynamic-path, I am lazy, I assume this means GNUCC compiler family
  #
  GET_PROPERTY(_test_path GLOBAL PROPERTY MYPACKAGE_TEST_PATH)
  GET_FILENAME_COMPONENT(_absolute_library_output_path ${LIBRARY_OUTPUT_PATH} ABSOLUTE)
  GET_PROPERTY(_marpaESLIFJava_jar_path TARGET marpaESLIFJava_jar PROPERTY JAR_FILE)
  GET_FILENAME_COMPONENT(_absolute_marpaESLIFJava_jar_path ${_marpaESLIFJava_jar_path} ABSOLUTE)
  GET_PROPERTY(_marpaESLIFJavaTest_jar_path TARGET marpaESLIFJavaTest_jar PROPERTY JAR_FILE)
  GET_FILENAME_COMPONENT(_absolute_marpaESLIFJavaTest_jar_path ${_marpaESLIFJavaTest_jar_path} ABSOLUTE)
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      SET (SEP "\\;")
    ELSE ()
      SET (SEP ":")
    ENDIF ()
  ADD_TEST (
    NAME marpaESLIFJavaTest
    COMMAND ${CMAKE_COMMAND} -E env ${Java_JAVA_EXECUTABLE} "-Djava.library.path=." "-cp" "${_absolute_marpaESLIFJava_jar_path}${SEP}${_absolute_marpaESLIFJavaTest_jar_path}" "org.parser.marpa.AppParse"
    WORKING_DIRECTORY ${_absolute_library_output_path})
  ADD_DEPENDENCIES(check marpaESLIFJavaTest_jar)
ENDIF ()
