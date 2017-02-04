PROJECT (marpaESLIFPerl)

FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_PM src "*.pm")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_XS src "*.xs")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_PL src "*.pl")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_T src "*.t")
ADD_CUSTOM_TARGET(marpaESLIFPerlPackage
  DEPENDS ${MARPAESLIFPERL_SRC_PM} ${MARPAESLIFPERL_SRC_XS} ${MARPAESLIFPERL_SRC_PL} ${MARPAESLIFPERL_SRC_T}
  COMMAND dzil build)

# ------------------------------------------------
# TEST (executed with make check at the top level)
# ------------------------------------------------
# Since we want to test the current marpaESLIF and not the one that is eventually already installed
# we have to instruct the generated Makefile.PL of the header and library locations
#
GET_FILENAME_COMPONENT(MARPAESLIF_INCLUDE_ABSOLUTEPATH "${INCLUDE_OUTPUT_PATH}"
                       ABSOLUTE)
GET_FILENAME_COMPONENT(MARPAESLIF_LIBRARY_ABSOLUTEPATH "${LIBRARY_OUTPUT_PATH}"
                       ABSOLUTE)
ENABLE_TESTING()
ADD_TEST (marpaESLIFPerlTest ${CMAKE_COMMAND} -E env "PATH=${TEST_PATH}" "MARPAESLIF_INCLUDE_ABSOLUTEPATH=${MARPAESLIF_INCLUDE_ABSOLUTEPATH}" "MARPAESLIF_LIBRARY_ABSOLUTEPATH=${MARPAESLIF_LIBRARY_ABSOLUTEPATH}" "${DZIL}" test --all --test-verbose)
ADD_CUSTOM_TARGET(marpaESLIFPerlTester DEPENDS marpaESLIFPerlPackage)
ADD_DEPENDENCIES(check marpaESLIFPerlTester)
