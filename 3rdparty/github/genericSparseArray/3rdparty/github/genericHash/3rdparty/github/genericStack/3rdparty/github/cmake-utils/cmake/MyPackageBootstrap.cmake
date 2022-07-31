#
# Paths - ensure MyPackage* are the ones distributed with THIS version of MyPackageBootstrap
#
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  SET (MYPACKAGEBOOTSTRAP_DONE TRUE)
  SET (source_dir "${CMAKE_CURRENT_LIST_DIR}")
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Setting property MYPACKAGE_SOURCE_DIR to ${source_dir}")
  ENDIF ()
  SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR ${source_dir})
  #
  # Transversal things: link and math libraries, based on
  # https://cmake.org/cmake/help/latest/guide/tutorial/index.html?highlight=cmake_required_librarie
  #
  INCLUDE (CheckSymbolExists)
  CHECK_SYMBOL_EXISTS (log "math.h" HAVE_LOG)
  CHECK_SYMBOL_EXISTS (exp "math.h" HAVE_EXP)
  IF (NOT (HAVE_LOG AND HAVE_EXP))
    UNSET (HAVE_LOG CACHE)
    UNSET (HAVE_EXP CACHE)
    SET (CMAKE_REQUIRED_LIBRARIES "m")
    CHECK_SYMBOL_EXISTS (log "math.h" HAVE_LOG)
    CHECK_SYMBOL_EXISTS (exp "math.h" HAVE_EXP)
    IF (HAVE_LOG AND HAVE_EXP)
      SET (CMAKE_MATH_LIBS "m" CACHE STRING "Math library")
      MARK_AS_ADVANCED (CMAKE_MATH_LIBS)
    ENDIF ()
  ENDIF ()
  #
  # Policies common to all our files
  #
  FOREACH (_policy CMP0018 CMP0063 CMP0075)
    IF (POLICY ${_policy})
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Setting policy ${_policy} to NEW")
      ENDIF ()
      CMAKE_POLICY (SET ${_policy} NEW)
    ENDIF ()
  ENDFOREACH ()
  #
  # Load all macros in this directory
  #
  FILE(GLOB _cmake_glob ${source_dir}/*.cmake)
  FOREACH (_cmake IN LISTS _cmake_glob)
    GET_FILENAME_COMPONENT(_name ${_cmake} NAME)
    STRING (TOUPPER ${_name} _NAME)
    IF (${_NAME} STREQUAL "MYPACKAGEBOOTSTRAP")
      CONTINUE()
    ENDIF ()
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Including ${_cmake}")
    ENDIF ()
    #
    # We want our include to inherits all the policies we setted here
    #
    INCLUDE (${_cmake} NO_POLICY_SCOPE)  # No pb if we re-include ourself -;
  ENDFOREACH ()
  #
  # Enable Testing
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Enable tests")
  ENDIF ()
  ENABLE_TESTING()
  #
  # Create check and man targets
  #
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Creating target check")
  ENDIF ()
  ADD_CUSTOM_TARGET(check 
            ${CMAKE_COMMAND} -E echo ----------------------------------
    COMMAND ${CMAKE_COMMAND} -E echo Test command:
    COMMAND ${CMAKE_COMMAND} -E echo ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    COMMAND ${CMAKE_COMMAND} -E echo ----------------------------------
    COMMAND ${CMAKE_COMMAND} -E env CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND} -C $<CONFIG>
    )
  IF (MYPACKAGE_DEBUG)
    MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Creating target man")
  ENDIF ()
  INSTALL (CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_MAKE_PROGRAM} man)")
  ADD_CUSTOM_TARGET (man)
ENDIF ()
