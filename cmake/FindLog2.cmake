MACRO (FINDLOG2)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check log2, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_LOG2_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_LOG2_FOUND FALSE)
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
      FOREACH (KEYWORD "log2")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/log2.c
          COMPILE_DEFINITIONS -DC_LOG2=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_LOG2 ${KEYWORD})
          SET (_C_LOG2_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_LOG2_FOUND)
      SET (C_LOG2 "${_C_LOG2}" CACHE STRING "C log2 implementation")
      MARK_AS_ADVANCED (C_LOG2)
    ENDIF ()
    SET (C_LOG2_SINGLETON TRUE CACHE BOOL "C log2 check singleton")
    MARK_AS_ADVANCED (C_LOG2_SINGLETON)
  ENDIF ()
ENDMACRO()
