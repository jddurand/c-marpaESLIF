cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
#
# We inherit project version, it is marpaESLIF's version
#
project(marpaESLIFJava VERSION ${marpaESLIFJava_VERSION} LANGUAGES C)
#
# Create library
#
cmake_helpers_library(${PROJECT_NAME}
  CONFIG_ARGS                           include/marpaESLIFJava/internal/config.h.in include/marpaESLIFJava/internal/config.h
  TYPE_AUTO                             FALSE              # We want a single library
  TYPE_SHARED                           TRUE               # that is the the shared version
  EXPORT_HEADER                         FALSE              # JNICALL takes care of the symbols's import/export
  TARGET_NAME_SHARED                    ${PROJECT_NAME}    # then cmake_helpers default is not what we want.
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
  target_compile_definitions(${PROJECT_NAME} PRIVATE
    -DMARPAESLIFJAVA_VERSION_MAJOR=${${PROJECT_NAME}_VERSION_MAJOR}
    -DMARPAESLIFJAVA_VERSION_MINOR=${${PROJECT_NAME}_VERSION_MINOR}
    -DMARPAESLIFJAVA_VERSION_PATCH=${${PROJECT_NAME}_VERSION_PATCH}
    -DMARPAESLIFJAVA_VERSION="${${PROJECT_NAME}_VERSION}"
  )
  target_include_directories(${PROJECT_NAME} PUBLIC
    ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2}
  )
endforeach()
#
# Create jars
#
file(GLOB_RECURSE _java_sources src "*.java")
set(CMAKE_JNI_TARGET TRUE)
set(CMAKE_JAVA_TARGET_VERSION 1.4.0)
set(CMAKE_JAVA_TARGET_OUTPUT_NAME marpaESLIF)
add_jar(${PROJECT_NAME}Jar SOURCES ${_java_sources} VERSION ${${PROJECT_NAME}_VERSION} OUTPUT_NAME marpaESLIF)
add_dependencies(${PROJECT_NAME}Jar ${PROJECT_NAME})
install_jar(${PROJECT_NAME}Jar ${CMAKE_HELPERS_INSTALL_JAVADIR})
install_jni_symlink(${PROJECT_NAME}Jar ${CMAKE_HELPERS_INSTALL_LIBDIR})
export_jars(TARGETS ${PROJECT_NAME}Jar NAMESPACE ${PROJECT_NAME}:: FILE ${PROJECT_NAME}Targets.cmake)
install_jar_exports(TARGETS ${PROJECT_NAME}Jar NAMESPACE ${PROJECT_NAME}:: FILE ${PROJECT_NAME}Targets.cmake DESTINATION ${CMAKE_HELPERS_INSTALL_CMAKEDIR})
#
# Create documentation
#
create_javadoc(${PROJECT_NAME}
  PACKAGES    org.parser.marpa
  SOURCEPATH  src
  INSTALLPATH ${CMAKE_HELPERS_INSTALL_JAVADOCDIR}
  DOCTITLE    "marpaESLIF Java Bindings"
  WINDOWTITLE ${PROJECT_NAME}
  AUTHOR      TRUE
  USE         TRUE
  VERSION     TRUE
)
add_dependencies(${PROJECT_NAME}Jar ${PROJECT_NAME}_javadoc)
#
# Tests
#
include(CTest)
file(GLOB_RECURSE _test_sources test "*.java")
set(CMAKE_JNI_TARGET FALSE)
set(CMAKE_JAVA_TARGET_VERSION FALSE)
add_jar(${PROJECT_NAME}TestJar
  SOURCES ${_test_sources}
  ENTRY_POINT org/parser/marpa/AppParse
  OUTPUT_NAME marpaESLIFJavaTest
)
add_dependencies(${PROJECT_NAME}TestJar ${PROJECT_NAME}Jar)
#
# Recuperate our two jar files locations. It is hard to get rid of ";" interpretation if we play
# with the -cp option, fortunately CLASSPATH environment variable exists.
#
get_target_property(${PROJECT_NAME}JarFile ${PROJECT_NAME}Jar JAR_FILE)
get_target_property(${PROJECT_NAME}TestJarFile ${PROJECT_NAME}TestJar JAR_FILE)
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
  ENVIRONMENTS "CLASSPATH=set:${${PROJECT_NAME}JarFile}${_sep}${${PROJECT_NAME}TestJarFile}"
  COMMAND ${CMAKE_COMMAND} -E env ${Java_JAVA_EXECUTABLE} org.parser.marpa.AppParse
)
