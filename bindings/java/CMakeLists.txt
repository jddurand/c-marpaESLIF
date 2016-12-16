INCLUDE(${SWIG_USE_FILE})
#
# Inspired by https://github.com/javagg/ctp-swig
#             http://stackoverflow.com/questions/14542872/what-is-the-correct-way-to-use-add-jar-with-java-bindings-produced-by-swig
#
SET (_package_name "org.parser.marpaESLIFWrapperJava")
SET (_package_top_dir "org")
STRING (REPLACE "." "/" _package_dir ${_package_name})

INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
#
# Set SWIG variables before creating the target
#
SET (CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/${_package_dir})
SET (CMAKE_SWIG_FLAGS "-package" ${_package_name})
SET (SWIG_MODULE_marpaESLIFWrapperJava_EXTRA_DEPS ${MARPAESLIF_LIBRARY})
SWIG_ADD_MODULE(marpaESLIFWrapperJava java ../marpaESLIFWrapper.i)
SWIG_LINK_LIBRARIES(marpaESLIFWrapperJava ${MARPAESLIF_LIBRARY} ${GENERICLOGGER_LIBRARIES})

SET (SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR})
SET (BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
FILE (MAKE_DIRECTORY ${SOURCE_DIR})
FILE (MAKE_DIRECTORY ${BINARY_DIR})

FILE(COPY marpaESLIF.java DESTINATION ${CMAKE_SWIG_OUTDIR})
SET (JAVA_SOURCES ${CMAKE_SWIG_OUTDIR}/*.java)
SET (BIN_FILE "marpaESLIF.jar")
SET (DOC_FILE "marpaESLIF-doc.jar")
SET (SRC_FILE "marpaESLIF-src.jar")

#
# Our additional stuff on top of the native wrapper
#
ADD_CUSTOM_COMMAND(OUTPUT _build_jar
    COMMENT "Creating jar file..."
    COMMAND ${Java_JAVAC_EXECUTABLE} -d ${BINARY_DIR} ${JAVA_SOURCES}
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${BIN_FILE} -C ${BINARY_DIR} ${_package_top_dir}
    # COMMAND ${Java_JAR_EXECUTABLE} uf ${CMAKE_CURRENT_BINARY_DIR}/${BIN_FILE} ${JAVA_SOURCES}
    COMMAND ${Java_JAVADOC_EXECUTABLE} -quiet -d ${BINARY_DIR}/javadoc -sourcepath ${SOURCE_DIR} ${_package_name} marpaESLIF.java
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${DOC_FILE} -C ${BINARY_DIR}/javadoc ${_package_top_dir}
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE} ${_package_top_dir} marpaESLIF.java
    DEPENDS marpaESLIFWrapperJava
)

ADD_CUSTOM_TARGET(run_build_jar ALL DEPENDS _build_jar)
