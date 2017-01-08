INCLUDE_DIRECTORIES (${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
FILE (GLOB lib_src_java marpaESLIFJava.c)
ADD_LIBRARY (marpaESLIFJava SHARED ${lib_src_java})
TARGET_LINK_LIBRARIES (marpaESLIFJava ${MARPAESLIF_LIBRARY} ${GENERICLOGGER_LIBRARIES})
SET_TARGET_PROPERTIES (marpaESLIFJava
  PROPERTIES
  VERSION "${MARPAESLIF_VERSION}"
  SOVERSION "${MARPAESLIF_VERSION_MAJOR}")
INSTALL (TARGETS marpaESLIFJava ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)

#
# Inspired by https://github.com/SimpleITK/SimpleITK/blob/master/Wrapping/Java/CMakeLists.txt
#
SET (JAR_FILE "marpaESLIF-${MARPAESLIF_VERSION}.jar")
SET (JAVADOC_FILE "marpaESLIF-javadoc-${MARPAESLIF_VERSION}.jar")
SET (JAVASRC_FILE "marpaESLIF-source-${MARPAESLIF_VERSION}.jar")

FIND_PROGRAM (Java_JAVADOC_EXECUTABLE
  NAMES javadoc
  HINTS ${_JAVA_HINTS}
  PATHS ${_JAVA_PATHS}
  )

# Make sure the nested directory structure exists
SET (JAVA_SOURCE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src)
SET (JAVA_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmake-build)
FILE (MAKE_DIRECTORY ${JAVA_SOURCE_DIRECTORY})
FILE (MAKE_DIRECTORY ${JAVA_BINARY_DIRECTORY})

SET (JAVA_SOURCE_CODE "${JAVA_SOURCE_DIRECTORY}/org/parser/marpa/*.java")

# Oracle Java 1.8 javadoc treats linting errors as build errors by default
# so linting should be opt-in for this version. OpenJDK is not as
# consistent with the 1.8.0 versions, so we execute at look for a
# invalid flag error.
EXECUTE_PROCESS (COMMAND "${Java_JAVADOC_EXECUTABLE}" "-Xdoclint:none" ERROR_VARIABLE err OUTPUT_VARIABLE err)
STRING (REGEX MATCH "javadoc.*invalid flag" invalid_flag_match "${err}")
IF (invalid_flag_match STREQUAL "")
  MARK_AS_ADVANCED ( Java_JAVADOC_LINTING )
  OPTION ( Java_JAVADOC_LINTING "Enable javadoc linting for Java 1.8" OFF )
  IF ( Java_JAVADOC_LINTING )
    SET ( Java_JAVADOC_LINTING_CMD "-Xdoclint:all" )
  ELSE ()
    SET ( Java_JAVADOC_LINTING_CMD "-Xdoclint:none" )
  ENDIF ()
ENDIF ()

# Add custom command and target to compile the generated files and put them in a jar file
# Make sure the commands depend on the output library from marpaESLIFJava
ADD_CUSTOM_COMMAND (
  OUTPUT ${JAR_FILE}
  COMMENT "Creating jar..."
  WORKING_DIRECTORY src
  COMMAND ${Java_JAVAC_EXECUTABLE} -d ${JAVA_BINARY_DIRECTORY} ${JAVA_SOURCE_CODE}
  COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${JAR_FILE} -C ${JAVA_BINARY_DIRECTORY} org
  DEPENDS marpaESLIFJava
  )
ADD_CUSTOM_TARGET (marpaESLIFJava_jar ALL DEPENDS ${JAR_FILE})
INSTALL (FILES ${JAR_FILE} DESTINATION bindings/java)

ADD_CUSTOM_COMMAND (
  OUTPUT ${JAVADOC_FILE}
  COMMENT "Creating javadoc..."
  WORKING_DIRECTORY src
  COMMAND ${Java_JAVADOC_EXECUTABLE} ${Java_JAVADOC_LINTING_CMD} -quiet -d ${JAVA_BINARY_DIRECTORY}/javadoc -sourcepath ${JAVA_SOURCE_DIRECTORY} org.parser.marpa
  COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${JAVADOC_FILE} -C ${JAVA_BINARY_DIRECTORY}/javadoc org
  DEPENDS marpaESLIFJava
  )
ADD_CUSTOM_TARGET (marpaESLIFJava_javadoc ALL DEPENDS ${JAVADOC_FILE})
INSTALL (FILES ${JAVADOC_FILE} DESTINATION bindings/java)

ADD_CUSTOM_COMMAND (
  OUTPUT ${JAVASRC_FILE}
  COMMENT "Creating source jar..."
  WORKING_DIRECTORY src
  COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${JAVASRC_FILE} org
  DEPENDS marpaESLIFJava
  )
ADD_CUSTOM_TARGET (marpaESLIFJava_source ALL DEPENDS ${JAVASRC_FILE})
INSTALL (FILES ${JAVASRC_FILE} DESTINATION bindings/java)
