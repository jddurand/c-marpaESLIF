MACRO (MYPACKAGEEXECUTABLE name)
  IF (MYPACKAGE_DEBUG)
    FOREACH (_source ${ARGN})
      MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Source: ${_source}")
    ENDFOREACH ()
  ENDIF ()

  FOREACH (_name ${name} ${name}_static)
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Adding ${_name}")
    ENDIF ()
    LIST (APPEND ${PROJECT_NAME}_EXECUTABLE ${_name})
    ADD_EXECUTABLE (${_name} ${ARGN})
    IF (MYPACKAGE_DEBUG)
      MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Set runtime output directory of ${_name} to ${LIBRARY_OUTPUT_PATH}")
    ENDIF ()
    SET_TARGET_PROPERTIES (${_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
    IF (NOT CMAKE_VERSION VERSION_LESS "3.26")
      INSTALL (TARGETS ${_name}
	EXPORT ${PROJECT_NAME}-targets
  	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT ApplicationComponent
      )
    ELSE ()
      INSTALL (TARGETS ${_name}
  	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	COMPONENT ApplicationComponent
      )
    ENDIF ()
    SET (${PROJECT_NAME}_HAVE_APPLICATIONCOMPONENT TRUE CACHE INTERNAL "Have ApplicationComponent" FORCE)
 
    IF (${_name} STREQUAL ${name})
      IF (TARGET ${PROJECT_NAME})
        IF (MYPACKAGE_DEBUG)
          MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Adding ${PROJECT_NAME} link library to ${_name}")
        ENDIF ()
        TARGET_LINK_LIBRARIES(${_name} PUBLIC ${PROJECT_NAME})
      ELSE ()
        #
        # Current project does not define a library
        #
        FOREACH (_include_directory ${CMAKE_CURRENT_BINARY_DIR}/output/include ${PROJECT_SOURCE_DIR}/include)
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Adding ${_include_directory} include dependency to ${_name}")
          ENDIF ()
		  TARGET_INCLUDE_DIRECTORIES(${_name} PUBLIC $<${build_local_interface}:${_include_directory}>)
        ENDFOREACH ()
      ENDIF ()
    ENDIF ()

    IF (${_name} STREQUAL ${name}_static)
      IF (TARGET ${PROJECT_NAME}_static)
        IF (MYPACKAGE_DEBUG)
          MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Adding ${PROJECT_NAME}_static link library to ${_name}")
        ENDIF ()
        TARGET_LINK_LIBRARIES(${_name} PUBLIC ${PROJECT_NAME}_static)
      ELSE ()
        IF (MYPACKAGE_DEBUG)
          MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Setting -D${PROJECT_NAME}_STATIC to ${_name}")
        ENDIF ()
        TARGET_COMPILE_DEFINITIONS(${_name} PUBLIC -D${PROJECT_NAME}_STATIC)
        #
        # Current project does not define a static library
        #
        FOREACH (_include_directory ${CMAKE_CURRENT_BINARY_DIR}/output/include ${PROJECT_SOURCE_DIR}/include)
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-EXECUTABLE-DEBUG] Adding ${_include_directory} include dependency to ${_name}")
          ENDIF ()
		  TARGET_INCLUDE_DIRECTORIES(${_name} PUBLIC $<${build_local_interface}:${_include_directory}>)
        ENDFOREACH ()
      ENDIF ()
    ENDIF ()
  ENDFOREACH ()
ENDMACRO()
