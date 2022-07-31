MACRO (FINDCOPYSIGNL)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check copysignl, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_COPYSIGNL_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_COPYSIGNL_FOUND FALSE)
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
      FOREACH (KEYWORD "copysignl" "_copysignl" "__copysignl")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/copysignl.c
          COMPILE_DEFINITIONS -DC_COPYSIGNL=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_COPYSIGNL ${KEYWORD})
          SET (_C_COPYSIGNL_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_COPYSIGNL_FOUND)
      SET (C_COPYSIGNL "${_C_COPYSIGNL}" CACHE STRING "C copysignl implementation")
      MARK_AS_ADVANCED (C_COPYSIGNL)
    ENDIF ()
    SET (C_COPYSIGNL_SINGLETON TRUE CACHE BOOL "C copysignl check singleton")
    MARK_AS_ADVANCED (C_COPYSIGNL_SINGLETON)
  ENDIF ()
ENDMACRO()
