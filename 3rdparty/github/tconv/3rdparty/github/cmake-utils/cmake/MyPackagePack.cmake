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
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] No LICENSE")
      ENDIF ()
      SET (CPACK_RESOURCE_FILE_LICENSE)
    ENDIF ()
    #
    # Get all components in one package
    #
    SET (CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
    #
    # Always enable archive
    #
    SET (CPACK_ARCHIVE_COMPONENT_INSTALL ON)
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
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Full install type        : ${_CAN_FULLTYPE}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Development install type : ${_CAN_DEVELOPMENTTYPE}")
    ENDIF ()
    IF (_CAN_FULLTYPE)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add FullType")
      ENDIF ()
      CPACK_ADD_INSTALL_TYPE (FullType        DISPLAY_NAME "Full")
    ENDIF ()
    IF (_CAN_DEVELOPMENTTYPE)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DevelopmentType")
      ENDIF ()
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
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Development group        : ${_CAN_DEVELOPMENTGROUP}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Development Library group: ${_CAN_DEVELOPMENTLIBRARYGROUP}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Document group           : ${_CAN_DOCUMENTGROUP}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Runtime group            : ${_CAN_RUNTIMEGROUP}")
    ENDIF ()
    IF (_CAN_DEVELOPMENTGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DevelopmentGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (DevelopmentGroup        DISPLAY_NAME "Development" DESCRIPTION "Develoment files\n\nThis group contains libraries and headers"  EXPANDED)
    ENDIF ()
    IF (_CAN_DEVELOPMENTLIBRARYGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DevelopmentLibraryGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (DevelopmentLibraryGroup DISPLAY_NAME "Libraries"   DESCRIPTION "Library files\n\nBoth static and dynamic version are provided" PARENT_GROUP DevelopmentGroup)
    ENDIF ()
    IF (_CAN_DOCUMENTGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DevelopmentLibraryGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (DocumentGroup           DISPLAY_NAME "Documents"   DESCRIPTION "Document files\n\nThis group contains all the provided documentation" EXPANDED)
    ENDIF ()
    IF (_CAN_RUNTIMEGROUP)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add RuntimeGroup")
      ENDIF ()
      CPACK_ADD_COMPONENT_GROUP (RuntimeGroup            DISPLAY_NAME "Runtime"     DESCRIPTION "Runtime applications"  EXPANDED)
    ENDIF ()
    #
    # Components
    #
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Manpage component        : ${_HAVE_MANPAGECOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Dynamic Library component: ${_HAVE_DYNAMICLIBRARYCOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Static Library component : ${_HAVE_STATICLIBRARYCOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Header component         : ${_HAVE_HEADERCOMPONENT}")
      MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] Application component    : ${_HAVE_APPLICATIONCOMPONENT}")
    ENDIF ()
    IF (_HAVE_MANPAGECOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add ManpageComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(ManpageComponent
                          DISPLAY_NAME "Man pages"
                          DESCRIPTION "Documentation in the man format\n\nUseful on all platforms but Windows, in general"
                          GROUP DocumentGroup
                          INSTALL_TYPES FullType)
    ENDIF ()
    IF (_HAVE_DYNAMICLIBRARYCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add DynamicLibraryComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(DynamicLibraryComponent
                          DISPLAY_NAME "Dynamic"
                          DESCRIPTION "Dynamic Libraries\n\nNecessary almost anytime"
                          GROUP DevelopmentLibraryGroup
                          INSTALL_TYPES FullType DevelopmentType)
    ENDIF ()
    IF (_HAVE_STATICLIBRARYCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add StaticLibraryComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(StaticLibraryComponent
                          DISPLAY_NAME "Static"
                          DESCRIPTION "Static Libraries\n\nOnly programmers would eventually need that"
                          GROUP DevelopmentLibraryGroup
                          INSTALL_TYPES FullType DevelopmentType)
    ENDIF ()
    IF (_HAVE_HEADERCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add HeaderComponent")
      ENDIF ()
      CPACK_ADD_COMPONENT(HeaderComponent
                          DISPLAY_NAME "Headers"
                          DESCRIPTION "C/C++ Headers\n\nProgrammers will need these files"
                          GROUP DevelopmentGroup
                          INSTALL_TYPES FullType DevelopmentType)
    ENDIF ()
    IF (_HAVE_APPLICATIONCOMPONENT)
      IF (MYPACKAGE_DEBUG)
        MESSAGE (STATUS "[${PROJECT_NAME}-PACK-DEBUG] ... Add ApplicationComponent")
      ENDIF ()
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
    # In addition man pages is generated using the INSTALL(CODE xxx) hack.
    #
    ADD_CUSTOM_TARGET(pack
      COMMAND ${CMAKE_MAKE_PROGRAM} package
      COMMENT "Packaging ${PROJECT_NAME}"
	  DEPENDS man
      VERBATIM
    )
  ENDIF ()		    
ENDMACRO()
