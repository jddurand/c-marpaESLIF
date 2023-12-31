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
# Because we overwrite the test COMMAND we want to get outself the java library paths
#
set(_java_library_path $<TARGET_FILE_DIR:marpaESLIF> $<TARGET_FILE_DIR:marpaESLIFJava>)
cmake_path(CONVERT "${_java_library_path}" TO_NATIVE_PATH_LIST _java_library_path)
#
# Set the cp argument
#
get_target_property(${PROJECT_NAME}JarFile ${PROJECT_NAME}Jar JAR_FILE)
get_target_property(${PROJECT_NAME}TestJarFile ${PROJECT_NAME}TestJar JAR_FILE)
set(cp ${${PROJECT_NAME}JarFile} ${${PROJECT_NAME}TestJarFile})
cmake_path(CONVERT "${cp}" TO_NATIVE_PATH_LIST cp)
#
# Declare the test
#
include(CTest)
add_test(
  NAME marpaESLIFJavaTester
  COMMAND ${CMAKE_COMMAND} -E env ${Java_JAVA_EXECUTABLE} -Djava.library.path=${_java_library_path} -cp ${cp} org.parser.marpa.AppParse
)
