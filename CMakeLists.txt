#
# Usage: With nmake: cmake -G "NMake Makefiles", then nmake install
#        With  make: cmake -G "UNIX Makefiles", then make install
#
CMAKE_MINIMUM_REQUIRED (VERSION 2.4)
PROJECT (genericStack)

# -------
# Version
# -------
SET (GENERICSTACK_VERSION_MAJOR 1)
SET (GENERICSTACK_VERSION_MINOR 0)
SET (GENERICSTACK_VERSION_PATCH 0)
SET (GENERICSTACK_VERSION ${GENERICSTACK_VERSION_MAJOR}.${GENERICSTACK_VERSION_MINOR}.${GENERICSTACK_VERSION_PATCH})

# ---------
# Set paths
# ---------
INCLUDE_DIRECTORIES (${PROJECT_SOURCE_DIR}/include)

# -----
# Check
# -----
ENABLE_TESTING()
ADD_EXECUTABLE        (genericStackTester       EXCLUDE_FROM_ALL test/genericStack.c)
ADD_TEST              (NAME genericStackTest    COMMAND genericStackTester)
ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS genericStackTester)

# -------
# Install
# -------
INSTALL (FILES include/genericStack.h DESTINATION include)

IF (UNIX)
  INCLUDE (Pod2Man)
  ADD_MANPAGE_TARGET()
  POD2MAN(README.pod "GENERICSTACK" "3" "${GENERICSTACK_VERSION}")
ENDIF (UNIX)
