CMAKE_MINIMUM_REQUIRED (VERSION 3.0.0 FATAL_ERROR)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("../../../3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART (marpaESLIFPerl ${marpaESLIF_VERSION_MAJOR} ${marpaESLIF_VERSION_MINOR} ${marpaESLIF_VERSION_PATCH})

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
  COMMAND ${CMAKE_COMMAND} -E echo "${CMAKE_COMMAND} -E remove_directory src"
  COMMAND ${CMAKE_COMMAND} -E remove_directory src
  )

# -------
# Package
# -------
ADD_CUSTOM_TARGET (marpaESLIFPerlPackage ALL ${CMAKE_COMMAND} -E echo "... dzil build"
  COMMAND dzil build
  DEPENDS marpaESLIFPerlCopy
  )

# -------------
# Trial Release
# -------------
ADD_CUSTOM_TARGET (marpaESLIFPerlReleaseTrial ${CMAKE_COMMAND} -E echo "dzil release --trial"
  COMMAND dzil release --trial
  DEPENDS marpaESLIFPerlCopy
  )

# -------
# Release
# -------
ADD_CUSTOM_TARGET (marpaESLIFPerlRelease ${CMAKE_COMMAND} -E echo "dzil release"
  COMMAND dzil release
  DEPENDS marpaESLIFPerlCopy
  )

# -----
# Check
# -----
ADD_TEST (NAME marpaESLIFPerlTest COMMAND dzil test)
ADD_DEPENDENCIES (check marpaESLIFPerlCopy)
