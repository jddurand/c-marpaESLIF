#
# Inspired by https://github.com/javagg/ctp-swig
#
INCLUDE(UseSWIG)
INCLUDE_DIRECTORIES (${JAVA_INCLUDE_PATH} ${JNI_INCLUDE_DIRS})
SET (_package_name "org.parser.marpaESLIF")
STRING (REPLACE "." "/" _package_dir ${_package_name})
SET (_package_top_dir "org")
SET (CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/${_package_dir})
SET (CMAKE_SWIG_FLAGS "-package" ${_package_name} "-v")
SET (WRAPPER_FILES ../marpaESLIF.i)
SWIG_ADD_MODULE(marpaESLIF_java java ${WRAPPER_FILES})
MESSAGE(STATUS "Using ${MARPAESLIF_LIBRARY}")
SWIG_LINK_LIBRARIES(marpaESLIF_java ${MARPAESLIF_LIBRARY} ${GENERICLOGGER_LIBRARIES})
SET (SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR})
SET (BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
FILE (MAKE_DIRECTORY ${SOURCE_DIR})
FILE (MAKE_DIRECTORY ${BINARY_DIR})
ADD_DEPENDENCIES(marpaESLIF_java marpaESLIF)

SET (JAVA_SOURCES ${CMAKE_SWIG_OUTDIR}/*.java)
SET (BIN_FILE "marpaESLIF.jar")
SET (DOC_FILE "marpaESLIF-doc.jar")
SET (SRC_FILE "marpaESLIF-src.jar")

# FILE(COPY Runmd.java DESTINATION ${CMAKE_SWIG_OUTDIR})

ADD_CUSTOM_COMMAND(OUTPUT _build_jar
    COMMENT "Creating jar file..."
    COMMAND ${Java_JAVAC_EXECUTABLE} -d ${BINARY_DIR} ${JAVA_SOURCES}
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${BIN_FILE} -C ${BINARY_DIR} ${_package_top_dir}
    COMMAND ${Java_JAR_EXECUTABLE} uf ${CMAKE_CURRENT_BINARY_DIR}/${BIN_FILE} ${_marpaESLIFfiles} marpaESLIF_java
  #  COMMAND ${Java_JAVADOC_EXECUTABLE} -quiet -d ${BINARY_DIR}/javadoc -sourcepath ${SOURCE_DIR} ${_package_name}
  #  COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${DOC_FILE} -C ${BINARY_DIR}/javadoc ${_package_top_dir}
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE} ${_package_top_dir}
    DEPENDS marpaESLIF
)

add_custom_target(run_build_jar ALL DEPENDS _build_jar)
