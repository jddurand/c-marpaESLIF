MACRO (MYPACKAGEPACK VENDOR SUMMARY)
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
  # Finally, include CPack
  #
  INCLUDE (CPack)
  #
  # Install types
  #
  CPACK_ADD_INSTALL_TYPE (FullType        DISPLAY_NAME "Full")
  CPACK_ADD_INSTALL_TYPE (DevelopmentType DISPLAY_NAME "Development")
  #
  # Groups
  #
  CPACK_ADD_COMPONENT_GROUP (DevelopmentGroup        DISPLAY_NAME "Development" DESCRIPTION "Develoment files\n\nThis group contains libraries and headers"  EXPANDED)
  CPACK_ADD_COMPONENT_GROUP (DevelopmentLibraryGroup DISPLAY_NAME "Libraries"   DESCRIPTION "Library files\n\nBoth static and dynamic version are provided" PARENT_GROUP DevelopmentGroup)
  CPACK_ADD_COMPONENT_GROUP (DocumentGroup           DISPLAY_NAME "Documents"   DESCRIPTION "Document files\n\nThis group contains all the provided documentation" EXPANDED)
  CPACK_ADD_COMPONENT_GROUP (RuntimeGroup            DISPLAY_NAME "Runtime"     DESCRIPTION "Runtime applications"  EXPANDED)
  #
  # Components
  #
  CPACK_ADD_COMPONENT(ManpageComponent
                      DISPLAY_NAME "Man pages"
                      DESCRIPTION "Documentation in the man format\n\nUseful on all platforms but Windows, in general"
                      GROUP DocumentGroup
                      INSTALL_TYPES FullType)
  CPACK_ADD_COMPONENT(DynamicLibraryComponent
                      DISPLAY_NAME "Dynamic"
                      DESCRIPTION "Dynamic Libraries\n\nNecessary almost anytime"
                      GROUP DevelopmentLibraryGroup
                      INSTALL_TYPES FullType DevelopmentType)
  CPACK_ADD_COMPONENT(StaticLibraryComponent
                      DISPLAY_NAME "Static"
                      DESCRIPTION "Static Libraries\n\nOnly programmers would eventually need that"
                      GROUP DevelopmentLibraryGroup
                      INSTALL_TYPES FullType DevelopmentType)
  CPACK_ADD_COMPONENT(HeaderComponent
                      DISPLAY_NAME "Headers"
                      DESCRIPTION "C/C++ Headers\n\nProgrammers will need these files"
                      GROUP DevelopmentGroup
                      INSTALL_TYPES FullType DevelopmentType)
  CPACK_ADD_COMPONENT(ApplicationComponent
                      DISPLAY_NAME "Applications"
                      DESCRIPTION "Executables"
                      GROUP RuntimeGroup
                      INSTALL_TYPES FullType
                      DEPENDS DynamicLibraryComponent)
ENDMACRO()
