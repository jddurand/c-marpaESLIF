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
    INCLUDE (${_cmake})  # No pb if we re-include ourself -;
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
