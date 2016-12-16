INCLUDE(${SWIG_USE_FILE})
#
# Inspired by https://github.com/javagg/ctp-swig
#             http://stackoverflow.com/questions/14542872/what-is-the-correct-way-to-use-add-jar-with-java-bindings-produced-by-swig
#
SET (PACKAGE_NAME "org.parser.marpaESLIFWrapperJava")

INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2})
#
# Prepare src and bin directories
#
SET (SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/main/java)
SET (TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/test/java)
SET (BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
FILE (MAKE_DIRECTORY ${SOURCE_DIR})
FILE (MAKE_DIRECTORY ${TEST_DIR})
FILE (MAKE_DIRECTORY ${BINARY_DIR})
#
# Set SWIG variables before creating the target
#
SET (CMAKE_SWIG_OUTDIR "${SOURCE_DIR}/org/parser/marpaESLIFWrapperJava")
FILE(MAKE_DIRECTORY "${CMAKE_SWIG_OUTDIR}")
SET (CMAKE_SWIG_FLAGS "-package" ${PACKAGE_NAME})
SET (SWIG_MODULE_marpaESLIFWrapperJava_EXTRA_DEPS ${MARPAESLIF_LIBRARY})
SWIG_ADD_MODULE(marpaESLIFWrapperJava java ../marpaESLIFWrapper.i)
SWIG_LINK_LIBRARIES(marpaESLIFWrapperJava ${MARPAESLIF_LIBRARY} ${GENERICLOGGER_LIBRARIES})

FILE(COPY marpaESLIF.java DESTINATION "${SOURCE_DIR}/org/parser")
SET (JAVA_SWIG_SOURCES "${CMAKE_SWIG_OUTDIR}/*.java")
SET (JAVA_OUR_SOURCES "${SOURCE_DIR}/org/parser/marpaESLIF.java")
SET (BIN_FILE "marpaESLIF.jar")
SET (DOC_FILE "marpaESLIF-doc.jar")
SET (SRC_FILE "marpaESLIF-src.jar")

#
# Our additional stuff on top of the native wrapper
#
ADD_CUSTOM_COMMAND(OUTPUT _build_jar
    COMMENT "Creating jar file..."
    COMMAND ${Java_JAVAC_EXECUTABLE} -d ${BINARY_DIR} ${JAVA_SWIG_SOURCES} ${JAVA_OUR_SOURCES}
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${BIN_FILE} -C ${BINARY_DIR} org
    # COMMAND ${Java_JAR_EXECUTABLE} uf ${CMAKE_CURRENT_BINARY_DIR}/${BIN_FILE} ${JAVA_SWIG_SOURCES} ${JAVA_OUR_SOURCES}
    COMMAND ${Java_JAVADOC_EXECUTABLE} -quiet -d ${BINARY_DIR}/javadoc -sourcepath ${SOURCE_DIR} ${JAVA_SWIG_SOURCES} ${JAVA_OUR_SOURCES}
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${DOC_FILE} -C ${BINARY_DIR}/javadoc org
    COMMAND ${Java_JAR_EXECUTABLE} cf ${CMAKE_CURRENT_BINARY_DIR}/${SRC_FILE} -C ${SOURCE_DIR} org
    DEPENDS marpaESLIFWrapperJava
)

ADD_CUSTOM_TARGET(run_build_jar ALL DEPENDS _build_jar)
