MACRO (MYPACKAGEDEPENDENCY packageDepend packageDependSourceDir)
  #
  # Optional argument: TESTS, LIBS, EXES
  #
  SET (_TESTS TRUE)
  SET (_LIBS TRUE)
  SET (_EXES TRUE)
  FOREACH (_var ${ARGN})
    IF ("\"${_var}\"" STREQUAL TESTS)
      SET (_TESTS TRUE)
    ELSE ()
      IF ("\"${_var}\"" STREQUAL LIBS)
        SET (_LIBS TRUE)
      ELSE ()
        IF ("\"${_var}\"" STREQUAL EXES)
          SET (_EXES TRUE)
        ENDIF ()
      ENDIF ()
    ENDIF ()
  ENDFOREACH ()
  #
  # Check if inclusion was already done - via us or another mechanism... guessed with TARGET check
  #
  GET_PROPERTY(_packageDepend_set GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend} SET)
  IF (${_packageDepend_set})
    GET_PROPERTY(_packageDepend GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend})
  ELSE ()
    SET (_packageDepend "")
  ENDIF ()
  IF ((NOT ("${_packageDepend}" STREQUAL "")) OR (TARGET ${packageDepend}))
    IF (${_packageDepend_set})
      IF (${_packageDepend} STREQUAL "PENDING")
        MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-STATUS] ${packageDepend} is already being processed")
      ELSE ()
        IF (${_packageDepend} STREQUAL "DONE")
          MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-STATUS] ${packageDepend} is already available")
        ELSE ()
          MESSAGE (FATAL_ERROR "[${PROJECT_NAME}-DEPEND-STATUS] ${packageDepend} state is ${_packageDepend}, should be DONE or PENDING")
        ENDIF ()
      ENDIF ()
    ELSE ()
      MESSAGE (WARNING "[${PROJECT_NAME}-DEPEND-WARNING] Target ${packageDepend} already exist - use MyPackageDependency to avoid this warning")
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Setting property MYPACKAGE_DEPENDENCY_${packageDepend} to DONE")
      ENDIF ()
      SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend} "DONE")
    ENDIF ()
  ELSE ()
    MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-STATUS] ${packageDepend} is not yet available")
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Setting property MYPACKAGE_DEPENDENCY_${packageDepend} to PENDING")
    ENDIF ()
    SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend} "PENDING")
    #
    # ===================================================
    # Do the dependency: ADD_SUBDIRECTORY or FIND_PACKAGE
    # ===================================================
    #
    STRING (TOUPPER ${packageDepend} _PACKAGEDEPEND)
    IF (ALL_IN_ONE)
      GET_FILENAME_COMPONENT(packageDependSourceDirAbsolute ${packageDependSourceDir} ABSOLUTE)
      MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-STATUS] Adding subdirectory ${packageDependSourceDirAbsolute}")
      ADD_SUBDIRECTORY(${packageDependSourceDirAbsolute})
    ELSE ()
      MESSAGE(STATUS "[${PROJECT_NAME}-DEPEND-STATUS] Looking for ${packageDepend}")
      FIND_PACKAGE (${packageDepend})
      IF (NOT ${_PACKAGEDEPEND}_FOUND)
        MESSAGE (FATAL_ERROR "[${PROJECT_NAME}-DEPEND-STATUS] Find ${packageDepend} failed")
      ENDIF ()
    ENDIF ()
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Setting property MYPACKAGE_DEPENDENCY_${packageDepend} to DONE")
    ENDIF ()
    SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend} "DONE")
    #
    # Remember all eventual packageDepend variables we depend upon
    #
    FOREACH (_what "INCLUDE_DIRS" "LIBRARIES" "C_FLAGS_SHARED" "LINK_FLAGS")
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Setting property MYPACKAGE_DEPENDENCY_${packageDepend}_${_what} to ${${_PACKAGEDEPEND}_${_what}}")
      ENDIF ()
      SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend}_${_what} ${${_PACKAGEDEPEND}_${_what}})
    ENDFOREACH ()
  ENDIF ()

  #
  # Manage dependencies
  #
  SET (_test_candidates ${${PROJECT_NAME}_TEST_EXECUTABLE})
  SET (_lib_candidates  ${PROJECT_NAME} ${PROJECT_NAME}_static)
  SET (_exe_candidates  ${${PROJECT_NAME}_EXECUTABLE})
  SET (_candidates)
  IF (_TESTS)
    LIST (APPEND _candidates ${_test_candidates})
  ENDIF ()
  IF (_LIBS)
    LIST (APPEND _candidates ${_lib_candidates})
  ENDIF ()
  IF (_EXES)
    LIST (APPEND _candidates ${_exe_candidates})
  ENDIF ()
  FOREACH (_target ${_candidates})
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Inspecting target candidate ${_target}")
    ENDIF ()
    IF (TARGET ${_target})
      IF (ALL_IN_ONE)
        #
        # Dependency by target
        #
        IF (TARGET ${packageDepend})
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Adding ${packageDepend} dependency to ${_target}")
          ENDIF ()
          TARGET_LINK_LIBRARIES(${_target} ${packageDepend})
        ELSE ()
          #
          # Bad luck, this target does not generate a library
          #
          FOREACH (_include_directory ${packageDependSourceDir}/output/include ${packageDependSourceDir}/include)
            IF (MYPACKAGE_DEBUG)
              MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Adding -I${_include_directory} include to ${_target}")
            ENDIF ()
            TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC ${_include_directory})
          ENDFOREACH ()
        ENDIF ()
      ELSE ()
        #
        # Include dependency
        #
        GET_PROPERTY(_property GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend}_INCLUDE_DIRS)
        FOREACH (_include_directory ${_property})
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Adding -I${_include_directory} include to ${_target}")
          ENDIF ()
          TARGET_INCLUDE_DIRECTORIES(${_target} PUBLIC ${_include_directory})
        ENDFOREACH ()
        #
        # Library dependency
        #
        GET_PROPERTY(_property GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend}_LIBRARIES)
        FOREACH (_library ${_property})
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Adding ${_library} library to ${_target}")
          ENDIF ()
          TARGET_LINK_LIBRARIES(${_target} PUBLIC ${_library})
        ENDFOREACH ()
        #
        # Compile definitions
        #
        GET_PROPERTY(_property GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend}_C_FLAGS_SHARED)
        FOREACH (_flag ${_property})
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Adding ${_flag} compile flag to ${_target}")
          ENDIF ()
          TARGET_COMPILE_DEFINITIONS(${_target} PUBLIC ${_library})
        ENDFOREACH ()
        #
        # Link flags
        #
        GET_PROPERTY(_property GLOBAL PROPERTY MYPACKAGE_DEPENDENCY_${packageDepend}_LINK_FLAGS)
        FOREACH (_flag ${_property})
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-DEBUG] Adding ${_flag} link flag to ${_target}")
          ENDIF ()
          SET_TARGET_PROPERTIES(${_target}
            PROPERTIES
            LINK_FLAGS ${_flag}
            )
        ENDFOREACH ()
      ENDIF ()
    ENDIF ()
  ENDFOREACH ()
  #
  # Test path management
  #
  GET_PROPERTY(_test_path_set GLOBAL PROPERTY MYPACKAGE_TEST_PATH SET)
  IF (${_test_path_set})
    GET_PROPERTY(_test_path GLOBAL PROPERTY MYPACKAGE_TEST_PATH)
  ELSE ()
    SET (_test_path "")
  ENDIF ()

  IF (ALL_IN_ONE)
    SET (_dependLibraryRuntimeDirectory "${packageDependSourceDirAbsolute}/output/lib")
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      STRING(REGEX REPLACE "/" "\\\\"  _dependLibraryRuntimeDirectory "${_dependLibraryRuntimeDirectory}")
    ELSE ()
      STRING(REGEX REPLACE " " "\\\\ "  _dependLibraryRuntimeDirectory "${_dependLibraryRuntimeDirectory}")
    ENDIF ()
    SET(${_PACKAGEDEPEND}_RUNTIME_DIRECTORY "${_dependLibraryRuntimeDirectory}")
    IF ("${_test_path}" STREQUAL "")
      MESSAGE(STATUS "[${PROJECT_NAME}-DEPEND-STATUS] Initializing TEST_PATH with PATH")
      SET (_test_path "$ENV{PATH}" )
    ENDIF ()
    IF ("${CMAKE_HOST_SYSTEM}" MATCHES ".*Windows.*")
      SET (SEP "\\;")
    ELSE ()
      SET (SEP ":")
    ENDIF ()
    SET (_test_path "${${_PACKAGEDEPEND}_RUNTIME_DIRECTORY}${SEP}${_test_path}")
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-DEPEND-STATUS] Prepended ${${_PACKAGEDEPEND}_RUNTIME_DIRECTORY} to TEST_PATH")
    ENDIF ()
    SET_PROPERTY(GLOBAL PROPERTY MYPACKAGE_TEST_PATH "${_test_path}")
  ENDIF ()
ENDMACRO()
