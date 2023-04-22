MACRO (MYPACKAGEPACK VENDOR SUMMARY)
  #
  # This should be executed only at the top-level
  #
  IF (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    #
    # Set common CPack variables
    #
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Configuration CPack")
    ENDIF ()
    SET (CPACK_PACKAGE_NAME                ${PROJECT_NAME})
    SET (CPACK_PACKAGE_VENDOR              ${VENDOR})
    SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY ${SUMMARY})
    SET (CPACK_PACKAGE_VERSION             ${${PROJECT_NAME}_VERSION})
    SET (CPACK_PACKAGE_VERSION_MAJOR       ${${PROJECT_NAME}_VERSION_MAJOR})
    SET (CPACK_PACKAGE_VERSION_MINOR       ${${PROJECT_NAME}_VERSION_MINOR})
    SET (CPACK_PACKAGE_VERSION_PATCH       ${${PROJECT_NAME}_VERSION_PATCH})
    IF (EXISTS ${PROJECT_SOURCE_DIR}/LICENSE)
      CONFIGURE_FILE (${PROJECT_SOURCE_DIR}/LICENSE ${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt)
      SET (CPACK_RESOURCE_FILE_LICENSE     ${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt)
    ELSE ()
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] No LICENSE")
      ENDIF ()
      SET (CPACK_RESOURCE_FILE_LICENSE)
    ENDIF ()
    #
    # Get all components in one package
    #
    SET (CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
    #
    # And explicit show them
    #
    SET (CPACK_MONOLITHIC_INSTALL FALSE)
    #
    # Always enable archive
    #
    SET (CPACK_ARCHIVE_COMPONENT_INSTALL ON)
    #
    # We need a way to know if make install is running under CPACK or not
    #
    SET (CPACK_PROJECT_CONFIG_FILE_PATH ${CMAKE_CURRENT_BINARY_DIR}/cpack_project_config_file.cmake)
    FILE (WRITE ${CPACK_PROJECT_CONFIG_FILE_PATH} "message(STATUS \"Setting ENV{CPACK_IS_RUNNING}\")\n")
    FILE (APPEND ${CPACK_PROJECT_CONFIG_FILE_PATH} "set(ENV{CPACK_IS_RUNNING} TRUE)\n")
    SET (CPACK_PROJECT_CONFIG_FILE ${CPACK_PROJECT_CONFIG_FILE_PATH})
    #
    # Include CPack - from now on we will have access to CPACK own macros
    #
    INCLUDE (CPack)
    #
    # Groups
    #
    IF (${PROJECT_NAME}_HAVE_HEADERCOMPONENT)
      SET (_CAN_DEVELOPMENTGROUP TRUE)
    ELSE ()
      SET (_CAN_DEVELOPMENTGROUP FALSE)
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_LIBRARYCOMPONENT)
      SET (_CAN_LIBRARYGROUP TRUE)
    ELSE ()
      SET (_CAN_LIBRARYGROUP FALSE)
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_MANPAGECOMPONENT)
      SET (_CAN_DOCUMENTGROUP TRUE)
    ELSE ()
      SET (_CAN_DOCUMENTGROUP FALSE)
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_APPLICATIONCOMPONENT)
      SET (_CAN_RUNTIMEGROUP TRUE)
    ELSE ()
      SET (_CAN_RUNTIMEGROUP FALSE)
    ENDIF ()
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Development group : ${_CAN_DEVELOPMENTGROUP}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Library group     : ${_CAN_LIBRARYGROUP}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Document group    : ${_CAN_DOCUMENTGROUP}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Runtime group     : ${_CAN_RUNTIMEGROUP}")
    ENDIF ()
    IF (_CAN_DEVELOPMENTGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DevelopmentGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (DevelopmentGroup        DISPLAY_NAME "Development" DESCRIPTION "Development\n\nContains header and CMake components"  EXPANDED)
    ENDIF ()
    IF (_CAN_LIBRARYGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add LibraryGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (LibraryGroup DISPLAY_NAME "Libraries"   DESCRIPTION "Libraries\n\nContains dynamic and static components" EXPANDED)
    ENDIF ()
    IF (_CAN_DOCUMENTGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DocumentGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (DocumentGroup           DISPLAY_NAME "Documents"   DESCRIPTION "Documents\n\nContains manpages component" EXPANDED)
    ENDIF ()
    IF (_CAN_RUNTIMEGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add RuntimeGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (RuntimeGroup            DISPLAY_NAME "Runtime"     DESCRIPTION "Runtime\n\nContains application component"  EXPANDED)
    ENDIF ()
    #
    # Components
    #
    SET (CPACK_COMPONENTS_ALL)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Manpage component        : ${${PROJECT_NAME}_HAVE_MANPAGECOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Library component        : ${${PROJECT_NAME}_HAVE_LIBRARYCOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Header component         : ${${PROJECT_NAME}_HAVE_HEADERCOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Application component    : ${${PROJECT_NAME}_HAVE_APPLICATIONCOMPONENT}")
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_MANPAGECOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add ManpageComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(ManpageComponent
                          DISPLAY_NAME "Man pages"
                          DESCRIPTION "Documentation in the man format"
                          GROUP DocumentGroup
                          )
      LIST (APPEND CPACK_COMPONENTS_ALL ManpageComponent)
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_LIBRARYCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add LibraryComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(LibraryComponent
                          DISPLAY_NAME "Libraries"
                          DESCRIPTION "Dynamic and Static Libraries"
                          GROUP LibraryGroup
                          )
      LIST (APPEND CPACK_COMPONENTS_ALL LibraryComponent)
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_HEADERCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add HeaderComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(HeaderComponent
                          DISPLAY_NAME "Headers"
                          DESCRIPTION "C/C++ Headers"
                          GROUP DevelopmentGroup
                          )
      LIST (APPEND CPACK_COMPONENTS_ALL HeaderComponent)
    ENDIF ()
    IF (${PROJECT_NAME}_HAVE_APPLICATIONCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add ApplicationComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(ApplicationComponent
                          DISPLAY_NAME "Applications"
                          DESCRIPTION "Executables"
                          GROUP RuntimeGroup
                          DEPENDS LibraryComponent)
      LIST (APPEND CPACK_COMPONENTS_ALL ApplicationComponent)
    ENDIF ()
  ENDIF ()		    
ENDMACRO()
