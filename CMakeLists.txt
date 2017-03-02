CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
INCLUDE ("3rdparty/github/cmake-utils-remote/cmake/MyPackageStart.cmake")
MYPACKAGESTART (genericStack 1 0 29)

INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)

# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (genericStackTesterDefaultInitialStack       EXCLUDE_FROM_ALL test/genericStack_defaultinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterDefaultInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

ADD_EXECUTABLE        (genericStackTesterSmallInitialStack       EXCLUDE_FROM_ALL test/genericStack_smallinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterSmallInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

ADD_EXECUTABLE        (genericStackTesterNoInitialStack       EXCLUDE_FROM_ALL test/genericStack_noinitialstack.c)
SET_TARGET_PROPERTIES (genericStackTesterNoInitialStack       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

INCLUDE (MyPackageCheck)
MYPACKAGECHECK (genericStackTestDefaultInitialStack "${LIBRARY_OUTPUT_PATH}/genericStackTesterDefaultInitialStack")
MYPACKAGECHECK (genericStackTestSmallInitialStack "${LIBRARY_OUTPUT_PATH}/genericStackTesterSmallInitialStack")
MYPACKAGECHECK (genericStackTestNoInitialStack "${LIBRARY_OUTPUT_PATH}/genericStackTesterNoInitialStack")
ADD_DEPENDENCIES (check genericStackTesterDefaultInitialStack genericStackTesterSmallInitialStack genericStackTesterNoInitialStack)

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
