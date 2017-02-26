PROJECT (marpaESLIFPerl)

FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_PM src "*.pm")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_XS src "*.xs")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_PL src "*.pl")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_T src "*.t")

SET (MARPAESLIFCOPY_CMAKEOBJECTS_PL marpaESLIFCopy/CMakeObjects.PL)

ADD_CUSTOM_COMMAND(OUTPUT ${MARPAESLIFCOPY_CMAKEOBJECTS_PL}
  COMMAND ${PERL_EXECUTABLE} etc/copyDirs.PL
  DEPENDS etc/copyDirs.PL
  )

ADD_CUSTOM_TARGET(marpaESLIFPerlPackage
  DEPENDS ${MARPAESLIFCOPY_CMAKEOBJECTS_PL} dist.ini
  COMMAND dzil build)

# ------------------------------------------------
# TEST (executed with make check at the top level)
# ------------------------------------------------
ENABLE_TESTING()
ADD_TEST (marpaESLIFPerlTest ${CMAKE_COMMAND} "${DZIL}" test --all --test-verbose)
ADD_CUSTOM_TARGET(marpaESLIFPerlTester DEPENDS marpaESLIFPerlPackage)
ADD_DEPENDENCIES(check marpaESLIFPerlTester)
