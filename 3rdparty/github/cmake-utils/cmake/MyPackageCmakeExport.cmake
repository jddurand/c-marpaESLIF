MACRO (MYPACKAGECMAKEEXPORT)
  IF (NOT MYPACKAGECMAKEEXPORT_${PROJECT_NAME}_DONE)
    SET (MYPACKAGECMAKEEXPORT_${PROJECT_NAME}_DONE TRUE)

    IF (NOT CMAKE_VERSION VERSION_LESS "3.26")
      SET (_export_targets ${PROJECT_NAME}-targets)
      SET (_namespace ${PROJECT_NAME}::)
      IF (MYPACKAGE_DEBUG)
        MESSAGE(STATUS "[${PROJECT_NAME}-CMAKEEXPORT-DEBUG] Exporting ${_export_targets}")
      ENDIF ()
      SET (TARGET_CMAKE_IN ${CMAKE_CURRENT_BINARY_DIR}/${_export_targets}.cmake.in)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-CMAKEEXPORT-DEBUG] Generating ${TARGET_CMAKE_IN}")
      ENDIF ()
      SET (_target_cmake_in [[
@PACKAGE_INIT@

]])
      #
      # Explicit public dependencies
      #
      FOREACH (_public_dependency ${${PROJECT_NAME}_public_dependencies})
        STRING (APPEND _target_cmake_in "find_package(${_public_dependency} REQUIRED)")
        STRING (APPEND _target_cmake_in [[

]])
      ENDFOREACH ()
      STRING (APPEND _target_cmake_in "include(\"\${CMAKE_CURRENT_LIST_DIR}/${_export_targets}.cmake\"\)")
      STRING (APPEND _target_cmake_in [[

]])
      FILE (WRITE ${TARGET_CMAKE_IN} ${_target_cmake_in})
      INSTALL (EXPORT ${_export_targets}
        NAMESPACE ${_namespace}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT LibraryComponent)
      INCLUDE (CMakePackageConfigHelpers)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Generating ${PROJECT_NAME}Config.cmake")
      ENDIF ()
      CONFIGURE_PACKAGE_CONFIG_FILE(${TARGET_CMAKE_IN}
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
      )
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] Generating ${PROJECT_NAME}ConfigVersion.cmake")
      ENDIF ()
      WRITE_BASIC_PACKAGE_VERSION_FILE (
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
        COMPATIBILITY SameMinorVersion
      )
      INSTALL (FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	COMPONENT LibraryComponent
      )
      FILE (REMOVE ${TARGET_CMAKE_IN} ${_target_cmake_in})
    ELSE ()
      MESSAGE (AUTHOR_WARNING "CMake export requires version >= 3.26")
    ENDIF ()
  ENDIF ()
ENDMACRO()
