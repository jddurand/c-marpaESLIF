CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
INCLUDE ("3rdparty/github/cmake-utils-remote/cmake/MyPackageStart.cmake")
MYPACKAGESTART (genericHash 1 0 23)

#############################
# Dependencies
#############################
INCLUDE (MyPackageDependency)

# ----------------------
# genericStack: required
# ----------------------
MYPACKAGEDEPENDENCY(genericStack "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericStack-remote" GENERICSTACK_REAL_INCLUDE_DIRS)

# -----------------------------------
# genericLogger: required (for tests)
# -----------------------------------
MYPACKAGEDEPENDENCY(genericLogger "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/github/genericLogger-remote" GENERICLOGGER_REAL_INCLUDE_DIRS)

INCLUDE_DIRECTORIES (
  "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
  ${GENERICLOGGER_REAL_INCLUDE_DIRS}                   # Include path for genericLogger (external - required)
  ${GENERICSTACK_REAL_INCLUDE_DIRS}                    # Include path for genericStack (external - required)
  )

# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (genericHashTester       EXCLUDE_FROM_ALL test/genericHash.c)
ADD_EXECUTABLE        (genericHashTesterStatic EXCLUDE_FROM_ALL test/genericHash.c)

IF (ALL_IN_ONE)
  TARGET_LINK_LIBRARIES (genericHashTester       genericLogger)
  TARGET_LINK_LIBRARIES (genericHashTesterStatic genericLogger)
ELSE ()
  TARGET_LINK_LIBRARIES (genericHashTester       ${GENERICLOGGER_LIBRARIES})
  TARGET_LINK_LIBRARIES (genericHashTesterStatic ${GENERICLOGGER_LIBRARIES})
ENDIF ()

SET_TARGET_PROPERTIES (genericHashTester       PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
SET_TARGET_PROPERTIES (genericHashTesterStatic PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})

INCLUDE (MyPackageCheck)
MYPACKAGECHECK (genericHashTest "${LIBRARY_OUTPUT_PATH}/genericHashTester")
MYPACKAGECHECK (genericHashTestStatic "${LIBRARY_OUTPUT_PATH}/genericHashTesterStatic")
ADD_DEPENDENCIES  (check genericHashTester genericHashTesterStatic)

# -------
# Install
# -------
INSTALL (FILES include/genericHash.h DESTINATION include)
INSTALL (FILES include/genericHash/cloak.h DESTINATION include/genericHash)

INCLUDE (MyPackageMan)
MYPACKAGEMAN("${PROJECT_SOURCE_DIR}/README.pod" "GENERICHASH" "3" "${GENERICHASH_VERSION}")

#############
# Packaging #
#############
SET (CPACK_PACKAGE_VERSION_MAJOR "${GENERICHASH_VERSION_MAJOR}")
SET (CPACK_PACKAGE_VERSION_MINOR "${GENERICHASH_VERSION_MINOR}")
SET (CPACK_PACKAGE_VERSION_PATCH "${GENERICHASH_VERSION_PATCH}")
CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/LICENSE"  "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
SET (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
INCLUDE (CPack)
