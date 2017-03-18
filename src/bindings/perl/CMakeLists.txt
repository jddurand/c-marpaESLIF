PROJECT (marpaESLIFPerl)

FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_PM src "lib/*.pm")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_XS src "*.xs")
FILE (GLOB_RECURSE MARPAESLIFPERL_SRC_T src "t/*.t")
SET  (ETC_COPYDIRS_PL etc/copyDirs.PL)

ADD_CUSTOM_TARGET (marpaESLIFPerlCopy ${CMAKE_COMMAND} -E echo "${PERL_EXECUTABLE} etc/copyDirs.PL"
  COMMAND ${PERL_EXECUTABLE} etc/copyDirs.PL
  DEPENDS ${ETC_COPYDIRS_PL} ${MARPAESLIFPERL_SRC_PM} ${MARPAESLIFPERL_SRC_XS} ${MARPAESLIFPERL_SRC_T} dist.ini
  )

# -----
# Clean
# -----
ADD_CUSTOM_TARGET (marpaESLIFPerlClean ${CMAKE_COMMAND} -E echo "... dzil clean"
  COMMAND dzil clean
  DEPENDS marpaESLIFPerlCopy
  )

# -------
# Package
# -------
ADD_CUSTOM_TARGET (marpaESLIFPerlPackage ALL ${CMAKE_COMMAND} -E echo "... dzil build"
  COMMAND dzil build
  DEPENDS marpaESLIFPerlClean
  )

# -----
# Check
# -----
ADD_TEST (NAME marpaESLIFPerlTest COMMAND dzil test)
ADD_DEPENDENCIES (check marpaESLIFPerlCopy)
