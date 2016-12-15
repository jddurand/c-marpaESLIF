ENABLE_TESTING()

ADD_EXECUTABLE        (marpaESLIFTester EXCLUDE_FROM_ALL test/marpaESLIFTester.c)
ADD_EXECUTABLE        (adventTester EXCLUDE_FROM_ALL test/adventTester.c)
ADD_EXECUTABLE        (russellParadoxTester EXCLUDE_FROM_ALL test/russellParadox.c)
ADD_EXECUTABLE        (jsonTester EXCLUDE_FROM_ALL test/jsonTester.c)
ADD_EXECUTABLE        (selfTester EXCLUDE_FROM_ALL test/selfTester.c)
#
# Painful but putting the binaries produced with dynamic linking in the same directory as the
# dynamic libraries is the only way to make sure the OS will always see the dynamic library
# at runtime, REGARDLESS of the OS (we are thinking to Windows and Visual Studio for instance).
# and since we must do that for the dynamic test, we do it as well for any runtime thingy, just for coherency.
#
SET_TARGET_PROPERTIES (marpaESLIFTester       PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (marpaESLIFTester       marpaESLIF)
SET_TARGET_PROPERTIES (adventTester           PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (adventTester           marpaESLIF)
SET_TARGET_PROPERTIES (russellParadoxTester   PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (russellParadoxTester   marpaESLIF)
SET_TARGET_PROPERTIES (jsonTester             PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (jsonTester             marpaESLIF)
SET_TARGET_PROPERTIES (selfTester             PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (selfTester             marpaESLIF)

ADD_TEST              (NAME marpaESLIFTest
  COMMAND marpaESLIFTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_TEST              (NAME adventTest
  COMMAND adventTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_TEST              (NAME russellParadoxTest
  COMMAND russellParadoxTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_TEST              (NAME jsonTest
  COMMAND jsonTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_TEST              (NAME selfTest
  COMMAND selfTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND}
                       DEPENDS marpaESLIFTester adventTester russellParadoxTester jsonTester selfTester)
