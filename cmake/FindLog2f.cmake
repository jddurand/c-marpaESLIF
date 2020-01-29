MACRO (FINDLOG2F)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check log2f, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_LOG2F_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_LOG2F_FOUND FALSE)
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
      FOREACH (KEYWORD "log2f")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/log2f.c
          COMPILE_DEFINITIONS -DC_LOG2F=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_LOG2F ${KEYWORD})
          SET (_C_LOG2F_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_LOG2F_FOUND)
      SET (C_LOG2F "${_C_LOG2F}" CACHE STRING "C log2f implementation")
      MARK_AS_ADVANCED (C_LOG2F)
    ENDIF ()
    SET (C_LOG2F_SINGLETON TRUE CACHE BOOL "C log2f check singleton")
    MARK_AS_ADVANCED (C_LOG2F_SINGLETON)
  ENDIF ()
ENDMACRO()
