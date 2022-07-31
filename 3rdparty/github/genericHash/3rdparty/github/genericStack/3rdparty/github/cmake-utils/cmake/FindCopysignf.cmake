MACRO (FINDCOPYSIGNF)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check copysignf, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_COPYSIGNF_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_COPYSIGNF_FOUND FALSE)
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
      FOREACH (KEYWORD "copysignf" "_copysignf" "__copysignf")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/copysignf.c
          COMPILE_DEFINITIONS -DC_COPYSIGNF=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_COPYSIGNF ${KEYWORD})
          SET (_C_COPYSIGNF_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_COPYSIGNF_FOUND)
      SET (C_COPYSIGNF "${_C_COPYSIGNF}" CACHE STRING "C copysignf implementation")
      MARK_AS_ADVANCED (C_COPYSIGNF)
    ENDIF ()
    SET (C_COPYSIGNF_SINGLETON TRUE CACHE BOOL "C copysignf check singleton")
    MARK_AS_ADVANCED (C_COPYSIGNF_SINGLETON)
  ENDIF ()
ENDMACRO()
