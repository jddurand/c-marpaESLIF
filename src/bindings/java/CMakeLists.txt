cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
#
# We inherit project version, it is marpaESLIF's version
#
# project(marpaESLIFJava VERSION ${marpaESLIFJava_VERSION} LANGUAGES C)
#
# Create library
#
cmake_helpers_library(marpaESLIFJava
  CONFIG_ARGS                           include/marpaESLIFJava/internal/config.h.in include/marpaESLIFJava/internal/config.h
  TYPE_AUTO                             FALSE              # We want a single library
  TYPE_SHARED                           TRUE               # that is the the shared version
  EXPORT_HEADER                         FALSE              # JNICALL takes care of the symbols's import/export
  TARGET_NAME_SHARED                    marpaESLIFJava     # then cmake_helpers default is not what we want.
  DEPENDS                               PRIVATE marpaESLIF # We depend totally on marpaESLIF shared library, but internally
  TARGETS_OUTVAR                        targets
)
#
# Java dependency
#
find_package(Java) # For Java_JAVA_EXECUTABLE
include(UseJava)   # To create jars, doc
#
# Add internal definitions
#
foreach(_target IN LISTS targets)
  target_compile_definitions(marpaESLIFJava PRIVATE
    -DMARPAESLIFJAVA_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
    -DMARPAESLIFJAVA_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
    -DMARPAESLIFJAVA_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
    -DMARPAESLIFJAVA_VERSION="${${PROJECT_NAME}_VERSION}"
  )
  target_include_directories(marpaESLIFJava PUBLIC
    ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2}
  )
endforeach()
#
# Create jars
#
file(GLOB_RECURSE _java_sources src "*.java")
add_jar(marpaESLIFJavaJar SOURCES ${_java_sources} OUTPUT_NAME marpaESLIF)
add_dependencies(marpaESLIFJavaJar marpaESLIFJava)
install_jar(marpaESLIFJavaJar ${CMAKE_HELPERS_INSTALL_JAVADIR})
export_jars(TARGETS marpaESLIFJavaJar NAMESPACE marpaESLIFJava:: FILE marpaESLIFJavaTargets.cmake)
install_jar_exports(TARGETS marpaESLIFJavaJar NAMESPACE marpaESLIFJava:: FILE marpaESLIFJavaTargets.cmake DESTINATION ${CMAKE_HELPERS_INSTALL_CMAKEDIR})
#
# Create documentation
#
create_javadoc(marpaESLIFJava
  PACKAGES    org.parser.marpa
  SOURCEPATH  src
  INSTALLPATH ${CMAKE_HELPERS_INSTALL_JAVADOCDIR}
  DOCTITLE    "marpaESLIF Java Bindings"
  WINDOWTITLE marpaESLIFJava
  AUTHOR      TRUE
  USE         TRUE
  VERSION     TRUE
)
add_dependencies(marpaESLIFJavaJar marpaESLIFJava_javadoc)
#
# Tests
#
include(CTest)
file(GLOB_RECURSE _test_sources test "*.java")
set(CMAKE_JNI_TARGET FALSE)
set(CMAKE_JAVA_TARGET_VERSION FALSE)
add_jar(marpaESLIFJavaTestJar
  SOURCES ${_test_sources}
  ENTRY_POINT org/parser/marpa/AppParse
  OUTPUT_NAME marpaESLIFJavaTest
)
add_dependencies(marpaESLIFJavaTestJar marpaESLIFJavaJar)
#
# Recuperate our two jar files locations. It is hard to get rid of ";" interpretation if we play
# with the -cp option, fortunately CLASSPATH environment variable exists.
#
get_target_property(marpaESLIFJavaJarFile marpaESLIFJavaJar JAR_FILE)
get_target_property(marpaESLIFJavaTestJarFile marpaESLIFJavaTestJar JAR_FILE)
#
# Recuperate path separator: if separator is ";" then we have to escape otherwise CTest will not understand
#
cmake_path(CONVERT "x;y" TO_NATIVE_PATH_LIST _xy)
string(SUBSTRING "${_xy}" 1 1 _sep)
if(_sep STREQUAL ";")
  set(_sep "\\\\\\\\\;")
endif()
#
# Declare the test. cmake_helpers_exe() has the logic to do path modifications. So we build
# a fake binary and run test with a custom COMMAND
#
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/marpaESLIFJavaTesterDummy.c [[
#include <stdlib.h>

int main(int argc, char **argv) {
  exit(0);
}
]])
include(CTest)
cmake_helpers_exe(marpaESLIFJavaTester
  INSTALL FALSE
  TEST TRUE
  SOURCES marpaESLIFJavaTesterDummy.c
  DEPENDS
    PUBLIC marpaESLIF
    PUBLIC marpaESLIFJava
  #
  # Note how we play with ${_sep}
  #
  ENVIRONMENTS "CLASSPATH=set:${marpaESLIFJavaJarFile}${_sep}${marpaESLIFJavaTestJarFile}"
  COMMAND ${CMAKE_COMMAND} -E env ${Java_JAVA_EXECUTABLE} org.parser.marpa.AppParse
)
