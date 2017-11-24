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
    SET (CPACK_PACKAGE_NAME                "${PROJECT_NAME}")
    SET (CPACK_PACKAGE_VENDOR              "${VENDOR}")
    SET (CPACK_PACKAGE_DESCRIPTION_SUMMARY "${SUMMARY}")
    SET (CPACK_PACKAGE_VERSION             "${${PROJECT_NAME}_VERSION}")
    SET (CPACK_PACKAGE_VERSION_MAJOR       "${${PROJECT_NAME}_VERSION_MAJOR}")
    SET (CPACK_PACKAGE_VERSION_MINOR       "${${PROJECT_NAME}_VERSION_MINOR}")
    SET (CPACK_PACKAGE_VERSION_PATCH       "${${PROJECT_NAME}_VERSION_PATCH}")
    IF (EXISTS "${PROJECT_SOURCE_DIR}/LICENSE")
      CONFIGURE_FILE ("${PROJECT_SOURCE_DIR}/LICENSE"  "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
      SET (CPACK_RESOURCE_FILE_LICENSE     "${CMAKE_CURRENT_BINARY_DIR}/LICENSE.txt")
    ELSE ()
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-START-DEBUG] No LICENSE")
      ENDIF ()
      SET (CPACK_RESOURCE_FILE_LICENSE)
    ENDIF ()
    #
    # Enable component to all archive packaging types
    #
    SET (CPACK_ARCHIVE_COMPONENT_INSTALL TRUE)
    #
    # Get all components in one package
    #
    SET (CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE 1)
    #
    # Do not get fooled by components out of our control
    #
    SET (CPACK_COMPONENTS_ALL ManpageComponent DynamicLibraryComponent StaticLibraryComponent HeaderComponent ApplicationComponent)
    #
    # Include CPack - from now on we will have access to CPACK own macros
    #
    INCLUDE (CPack)
    #
    # Install types
    #
    IF (_HAVE_HEADERCOMPONENT OR _HAVE_DYNAMICLIBRARYCOMPONENT OR _HAVE_STATICLIBRARYCOMPONENT OR _HAVE_APPLICATIONCOMPONENT OR _HAVE_MANPAGECOMPONENT)
      SET (_CAN_FULLTYPE TRUE)
    ELSE ()
      SET (_CAN_FULLTYPE FALSE)
    ENDIF ()
    IF (_HAVE_HEADERCOMPONENT OR _HAVE_DYNAMICLIBRARYCOMPONENT OR _HAVE_STATICLIBRARYCOMPONENT)
      SET (_CAN_DEVELOPMENTTYPE TRUE)
    ELSE ()
      SET (_CAN_DEVELOPMENTTYPE FALSE)
    ENDIF ()
    IF (_CAN_FULLTYPE)
      CPACK_ADD_INSTALL_TYPE (FullType        DISPLAY_NAME "Full")
    ENDIF ()
    IF (_CAN_DEVELOPMENTTYPE)
      CPACK_ADD_INSTALL_TYPE (DevelopmentType DISPLAY_NAME "Development")
    ENDIF ()
    #
    # Groups
    #
    IF (_HAVE_HEADERCOMPONENT)
      SET (_CAN_DEVELOPMENTGROUP TRUE)
    ELSE ()
      SET (_CAN_DEVELOPMENTGROUP FALSE)
    ENDIF ()
    IF (_HAVE_DYNAMICLIBRARYCOMPONENT OR _HAVE_STATICLIBRARYCOMPONENT)
      SET (_CAN_DEVELOPMENTLIBRARYGROUP TRUE)
    ELSE ()
      SET (_CAN_DEVELOPMENTLIBRARYGROUP FALSE)
    ENDIF ()
    IF (_HAVE_MANPAGECOMPONENT)
      SET (_CAN_DOCUMENTGROUP TRUE)
    ELSE ()
      SET (_CAN_DOCUMENTGROUP FALSE)
    ENDIF ()
    IF (_HAVE_APPLICATIONCOMPONENT)
      SET (_CAN_RUNTIMEGROUP TRUE)
    ELSE ()
      SET (_CAN_RUNTIMEGROUP FALSE)
    ENDIF ()
    IF (_CAN_DEVELOPMENTGROUP)
      CPACK_ADD_COMPONENT_GROUP (DevelopmentGroup        DISPLAY_NAME "Development" DESCRIPTION "Develoment files\n\nThis group contains libraries and headers"  EXPANDED)
    ENDIF ()
    IF (_CAN_DEVELOPMENTLIBRARYGROUP)
      CPACK_ADD_COMPONENT_GROUP (DevelopmentLibraryGroup DISPLAY_NAME "Libraries"   DESCRIPTION "Library files\n\nBoth static and dynamic version are provided" PARENT_GROUP DevelopmentGroup)
    ENDIF ()
    IF (_CAN_DOCUMENTGROUP)
      CPACK_ADD_COMPONENT_GROUP (DocumentGroup           DISPLAY_NAME "Documents"   DESCRIPTION "Document files\n\nThis group contains all the provided documentation" EXPANDED)
    ENDIF ()
    IF (_CAN_RUNTIMEGROUP)
      CPACK_ADD_COMPONENT_GROUP (RuntimeGroup            DISPLAY_NAME "Runtime"     DESCRIPTION "Runtime applications"  EXPANDED)
    ENDIF ()
    #
    # Components
    #
    IF (_HAVE_MANPAGECOMPONENT)
      CPACK_ADD_COMPONENT(ManpageComponent
                          DISPLAY_NAME "Man pages"
                          DESCRIPTION "Documentation in the man format\n\nUseful on all platforms but Windows, in general"
                          GROUP DocumentGroup
                          INSTALL_TYPES FullType)
    ENDIF ()
    IF (_HAVE_DYNAMICLIBRARYCOMPONENT)
      CPACK_ADD_COMPONENT(DynamicLibraryComponent
                          DISPLAY_NAME "Dynamic"
                          DESCRIPTION "Dynamic Libraries\n\nNecessary almost anytime"
                          GROUP DevelopmentLibraryGroup
                          INSTALL_TYPES FullType DevelopmentType)
    ENDIF ()
    IF (_HAVE_STATICLIBRARYCOMPONENT)
      CPACK_ADD_COMPONENT(StaticLibraryComponent
                          DISPLAY_NAME "Static"
                          DESCRIPTION "Static Libraries\n\nOnly programmers would eventually need that"
                          GROUP DevelopmentLibraryGroup
                          INSTALL_TYPES FullType DevelopmentType)
    ENDIF ()
    IF (_HAVE_HEADERCOMPONENT)
      CPACK_ADD_COMPONENT(HeaderComponent
                          DISPLAY_NAME "Headers"
                          DESCRIPTION "C/C++ Headers\n\nProgrammers will need these files"
                          GROUP DevelopmentGroup
                          INSTALL_TYPES FullType DevelopmentType)
    ENDIF ()
    IF (_HAVE_APPLICATIONCOMPONENT)
      CPACK_ADD_COMPONENT(ApplicationComponent
                          DISPLAY_NAME "Applications"
                          DESCRIPTION "Executables"
                          GROUP RuntimeGroup
                          INSTALL_TYPES FullType
                          DEPENDS DynamicLibraryComponent)
    ENDIF ()
    #
    # Quite subtil, but the "package" target is not visible at this time. There is a old standing bug
    # in CMake about this.
    # We use a custom target for package generation.
    # In addition documentation is generated using the INSTALL(CODE xxx) hack.
    #
    ADD_CUSTOM_TARGET(pack
      COMMAND ${CMAKE_MAKE_PROGRAM} package
      COMMENT "Packaging ${PROJECT_NAME}"
      DEPENDS man
      VERBATIM
    )
  ENDIF ()		    
ENDMACRO()
