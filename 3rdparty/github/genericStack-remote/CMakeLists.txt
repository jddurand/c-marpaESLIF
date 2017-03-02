CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
INCLUDE ("3rdparty/github/cmake-utils-remote/cmake/MyPackageStart.cmake")
MYPACKAGESTART (genericStack 1 0 28)

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)

# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (genericStackTesterDefaultInitialStack       EXCLUDE_FROM_ALL test/genericStack_defaultinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterDefaultInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
ADD_TEST              (NAME genericStackTestDefaultInitialStack    COMMAND genericStackTesterDefaultInitialStack WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

ADD_EXECUTABLE        (genericStackTesterSmallInitialStack       EXCLUDE_FROM_ALL test/genericStack_smallinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterSmallInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
ADD_TEST              (NAME genericStackTestSmallInitialStack    COMMAND genericStackTesterSmallInitialStack WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

ADD_EXECUTABLE        (genericStackTesterNoInitialStack       EXCLUDE_FROM_ALL test/genericStack_noinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterNoInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_PATH})
ADD_TEST              (NAME genericStackTestNoInitialStack    COMMAND genericStackTesterNoInitialStack WORKING_DIRECTORY ${BINARY_OUTPUT_PATH})

ADD_DEPENDENCIES      (check genericStackTesterDefaultInitialStack genericStackTesterSmallInitialStack genericStackTesterNoInitialStack)

# -------
# Install
# -------
INSTALL (FILES include/genericStack.h DESTINATION include)

INCLUDE (MyPackageMan)
MYPACKAGEMAN("${PROJECT_SOURCE_DIR}/README.pod" "GENERICSTACK" "3" "${GENERICSTACK_VERSION}")

#############
# Packaging #
#############
SET (CPACK_PACKAGE_VERSION_MAJOR "${GENERICSTACK_VERSION_MAJOR}")
SET (CPACK_PACKAGE_VERSION_MINOR "${GENERICSTACK_VERSION_MINOR}")
SET (CPACK_PACKAGE_VERSION_PATCH "${GENERICSTACK_VERSION_PATCH}")
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/LICENSE"  "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
INCLUDE (CPack)
