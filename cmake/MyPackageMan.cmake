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
    FIND_PROGRAM (POD2MAN pod2man)
    IF (POD2MAN)
      FIND_PROGRAM (GZIP gzip)
      IF (GZIP)
        SET (MANPAGE_TARGET "man-${MANFILE}")
        IF (NOT TARGET ${MANPAGE_TARGET})
          ADD_CUSTOM_COMMAND(
	    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}"
	    DEPENDS "${PODFILE}"
	    COMMAND "${POD2MAN}"
	    ARGS --section "${SECTION}" --center "${CMAKE_PROJECT_NAME}" -r "${VERSION}" --stderr --name "${MANFILE}" "${PODFILE}" > "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}"
	    )
          ADD_CUSTOM_COMMAND(
	    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz"
	    COMMAND "${GZIP}" --best -c "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}" > "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz"
	    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}"
	    )
          ADD_CUSTOM_TARGET(${MANPAGE_TARGET} DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz")
          ADD_DEPENDENCIES(man ${MANPAGE_TARGET})
          INSTALL(
	    FILES "${CMAKE_CURRENT_BINARY_DIR}/${MANFILE}.${SECTION}.gz"
	    DESTINATION "share/man/man${SECTION}"
	    )
        ENDIF ()
      ENDIF ()
    ENDIF ()
  ENDIF ()
ENDMACRO ()
