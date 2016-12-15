#
# Taken from MaxScale
#
# This CMake file tries to find the Perl regular expression libraries
# The following variables are set:
# PCRE2_FOUND - System has the PCRE library
# PCRE2_LIBRARIES - The PCRE library file
# PCRE2_INCLUDE_DIRS - The folder with the PCRE headers

find_library(PCRE2_LIBRARIES NAMES pcre2 pcre2-8)
find_path(PCRE2_INCLUDE_DIRS pcre2.h)
if(PCRE2_LIBRARIES AND PCRE2_INCLUDE_DIRS)
  MESSAGE(STATUS "")
  MESSAGE(STATUS "PCRE2 Setup:")
  MESSAGE(STATUS "------------")
  message(STATUS "     LIBRARIES: ${PCRE2_LIBRARIES}")
  message(STATUS "  INCLUDE_DIRS: ${PCRE2_INCLUDE_DIRS}")
  set(PCRE2_FOUND TRUE CACHE BOOL "Found PCRE2 libraries" FORCE)
  add_custom_target(pcre2)
else()
  MESSAGE(STATUS "")
  MESSAGE(STATUS "PCRE2 Setup: buit-in")
  set(PCRE2_FOUND FALSE CACHE BOOL "Found PCRE2 libraries" FORCE)
endif()
