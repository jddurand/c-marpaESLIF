#
# Adaptation from:
# Copyright (C) 2013	Emmanuel Roullit <emmanuel.roullit@gmail.com>
#

# Generate man pages of the project by using the
# POD header written in the tool source code.
# To use it, include this file in CMakeLists.txt and
# invoke POD2MAN(<podfile> <manfile> <section>)

MACRO(MyPackageMan PODFILE MANFILE SECTION VERSION)

  IF (EXISTS ${PODFILE})
    GET_FILENAME_COMPONENT(PODFILE_ABSOLUTE ${PODFILE} ABSOLUTE)
    FIND_PROGRAM (POD2MAN pod2man)
    IF (POD2MAN)
      FIND_PROGRAM (GZIP gzip)
      IF (GZIP)
        IF (NOT TARGET man)
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-BOOTSTRAP-DEBUG] Creating target man")
          ENDIF ()
          INSTALL (CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_MAKE_PROGRAM} man)")
          ADD_CUSTOM_TARGET (man)
        ENDIF ()
        SET (MANPAGE_TARGET "man_${MANFILE}")
        IF (NOT TARGET ${MANPAGE_TARGET})
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-MAN-DEBUG] Creating target ${MANPAGE_TARGET}")
          ENDIF ()
          ADD_CUSTOM_COMMAND(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}"
			DEPENDS "${PODFILE_ABSOLUTE}"
			COMMAND "${POD2MAN}"
			ARGS --section "${SECTION}" --center "${PROJECT_NAME}" -r "${VERSION}" --stderr --name "${MANFILE}" "${PODFILE_ABSOLUTE}" > "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}"
	      )
          ADD_CUSTOM_COMMAND(
			OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz"
			COMMAND "${GZIP}" --best -c "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}" > "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz"
			DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}"
	      )
          ADD_CUSTOM_TARGET(${MANPAGE_TARGET} DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz")
          ADD_DEPENDENCIES(man ${MANPAGE_TARGET})
          INSTALL(
			FILES ${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz
			DESTINATION ${CMAKE_INSTALL_MANDIR}/man${SECTION}
            COMPONENT ManpageComponent
	      )
          SET (${PROJECT_NAME}_HAVE_MANPAGECOMPONENT TRUE CACHE INTERNAL "Have ManpageComponent" FORCE)
          SET (CPACK_INSTALL_SCRIPT_MAN_PATH ${CMAKE_CURRENT_BINARY_DIR}/cpack_install_script_man.cmake)
          FILE (WRITE ${CPACK_INSTALL_SCRIPT_MAN_PATH} "execute_process(COMMAND \"${CMAKE_COMMAND}\" --build \"${CMAKE_CURRENT_BINARY_DIR}\" -t man WORKING_DIRECTORY \"${CMAKE_CURRENT_BINARY_DIR}\")\n")
          LIST (APPEND CPACK_INSTALL_SCRIPTS ${CPACK_INSTALL_SCRIPT_MAN_PATH})
        ELSE ()
          IF (MYPACKAGE_DEBUG)
            MESSAGE (STATUS "[${PROJECT_NAME}-MAN-DEBUG] Target ${MANPAGE_TARGET} already exist")
          ENDIF ()
        ENDIF ()
      ENDIF ()
    ENDIF ()
  ENDIF ()
ENDMACRO ()
