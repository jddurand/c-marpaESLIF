MACRO (FINDSTRTOULL)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check strtoull, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_STRTOULL_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_STRTOULL_FOUND FALSE)
      #
      # We depend on stdlib
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("stdlib.h" HAVE_STDLIB_H)
      IF (HAVE_STDLIB_H)
        SET (_HAVE_STDLIB_H 1)
      ELSE ()
        SET (_HAVE_STDLIB_H 0)
      ENDIF ()
      #
      # Test
      #
      FOREACH (KEYWORD "strtoull" "_strtoull" "__strtoull" "strtou64" "_strtou64" "__strtou64")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/strtoull.c
          COMPILE_DEFINITIONS -DC_STRTOULL=${KEYWORD} -DHAVE_STDLIB_H=${_HAVE_STDLIB_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_STRTOULL ${KEYWORD})
          SET (_C_STRTOULL_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_STRTOULL_FOUND)
      SET (C_STRTOULL "${_C_STRTOULL}" CACHE STRING "C strtoull implementation")
      MARK_AS_ADVANCED (C_STRTOULL)
    ENDIF ()
    SET (C_STRTOULL_SINGLETON TRUE CACHE BOOL "C strtoull check singleton")
    MARK_AS_ADVANCED (C_STRTOULL_SINGLETON)
  ENDIF ()
ENDMACRO()
