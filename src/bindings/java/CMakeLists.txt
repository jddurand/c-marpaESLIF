INCLUDE (UseJava)
INCLUDE_DIRECTORIES (${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})

# ------------------------------------------
# JNI
# ------------------------------------------
FILE (GLOB lib_src_java marpaESLIFJava.c)
ADD_LIBRARY (marpaESLIFJava SHARED ${lib_src_java})
TARGET_LINK_LIBRARIES (marpaESLIFJava ${MARPAESLIF_LIBRARY} ${GENERICLOGGER_LIBRARIES})
SET_TARGET_PROPERTIES (marpaESLIFJava
  PROPERTIES
  VERSION "${MARPAESLIF_VERSION}"
  SOVERSION "${MARPAESLIF_VERSION_MAJOR}")
INSTALL (TARGETS marpaESLIFJava ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)

# ------------------------------------------
# JAVA
# ------------------------------------------
FILE (GLOB_RECURSE MARPAESLIFJAVA_SRC src "*.java")
SET (CMAKE_JNI_TARGET TRUE)
SET (CMAKE_JAVA_TARGET_VERSION 1.4.0)
ADD_JAR(marpaESLIFJava_jar
        SOURCES ${MARPAESLIFJAVA_SRC}
        VERSION ${MARPAESLIF_VERSION}
        OUTPUT_NAME marpaESLIF
        )
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
ENABLE_TESTING()
FILE (GLOB_RECURSE MARPAESLIFJAVA_TEST test "*.java")
SET (CMAKE_JNI_TARGET FALSE)
SET (CMAKE_JAVA_TARGET_VERSION FALSE)
ADD_JAR(marpaESLIFJavaTest_jar
        SOURCES ${MARPAESLIFJAVA_TEST}
        ENTRY_POINT org/parser/marpa/AppParse
        OUTPUT_NAME marpaESLIFJavaTest
        )
ADD_DEPENDENCIES(marpaESLIFJavaTest_jar marpaESLIFJava_jar)
      
ADD_TEST (NAME marpaESLIFJavaTest
          COMMAND "${Java_JAVA_EXECUTABLE}" -Djava.library.path=${LIBRARY_OUTPUT_PATH} -jar marpaESLIFJavaTest.jar org.parser.marpa.AppParse
          DEPENDS marpaESLIFJavaTest_jar
          )

ADD_CUSTOM_TARGET(NAME marpaESLIFJavaTester DEPENDS marpaESLIFJavaTest)
