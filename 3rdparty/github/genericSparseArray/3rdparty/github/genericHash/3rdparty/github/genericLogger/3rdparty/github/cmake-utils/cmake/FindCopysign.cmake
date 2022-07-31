MACRO (FINDCOPYSIGN)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check copysign, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_COPYSIGN_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_COPYSIGN_FOUND FALSE)
      #
      # We depend on math.h
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("math.h" HAVE_MATH_H)
      IF (HAVE_MATH_H)
        SET (_HAVE_MATH_H 1)
      ELSE ()
        SET (_HAVE_MATH_H 0)
      ENDIF ()
      #
      # Test
      #
      FOREACH (KEYWORD "copysign" "_copysign" "__copysign")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/copysign.c
          COMPILE_DEFINITIONS -DC_COPYSIGN=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_COPYSIGN ${KEYWORD})
          SET (_C_COPYSIGN_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_COPYSIGN_FOUND)
      SET (C_COPYSIGN "${_C_COPYSIGN}" CACHE STRING "C copysign implementation")
      MARK_AS_ADVANCED (C_COPYSIGN)
    ENDIF ()
    SET (C_COPYSIGN_SINGLETON TRUE CACHE BOOL "C copysign check singleton")
    MARK_AS_ADVANCED (C_COPYSIGN_SINGLETON)
  ENDIF ()
ENDMACRO()
