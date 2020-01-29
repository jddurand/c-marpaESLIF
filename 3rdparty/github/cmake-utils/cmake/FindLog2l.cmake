MACRO (FINDLOG2L)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check log2l, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_LOG2L_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_LOG2L_FOUND FALSE)
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
      FOREACH (KEYWORD "log2l")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/log2l.c
          COMPILE_DEFINITIONS -DC_LOG2L=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_LOG2L ${KEYWORD})
          SET (_C_LOG2L_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_LOG2L_FOUND)
      SET (C_LOG2L "${_C_LOG2L}" CACHE STRING "C log2l implementation")
      MARK_AS_ADVANCED (C_LOG2L)
    ENDIF ()
    SET (C_LOG2L_SINGLETON TRUE CACHE BOOL "C log2l check singleton")
    MARK_AS_ADVANCED (C_LOG2L_SINGLETON)
  ENDIF ()
ENDMACRO()
