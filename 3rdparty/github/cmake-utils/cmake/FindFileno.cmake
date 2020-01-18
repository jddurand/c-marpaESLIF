MACRO (FINDFILENO)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check fileno, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_FILENO_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_FILENO_FOUND FALSE)
      #
      # We depend on stdio
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("stdio.h" HAVE_STDIO_H)
      IF (HAVE_STDIO_H)
        SET (_HAVE_STDIO_H 1)
      ELSE ()
        SET (_HAVE_STDIO_H 0)
      ENDIF ()
      #
      # Test
      #
      FOREACH (KEYWORD "fileno" "_fileno" "__fileno")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/fileno.c
          COMPILE_DEFINITIONS -DC_FILENO=${KEYWORD} -DHAVE_STDIO_H=${_HAVE_STDIO_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_FILENO ${KEYWORD})
          SET (_C_FILENO_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_FILENO_FOUND)
      SET (C_FILENO "${_C_FILENO}" CACHE STRING "C fileno implementation")
      MARK_AS_ADVANCED (C_FILENO)
    ENDIF ()
    SET (C_FILENO_SINGLETON TRUE CACHE BOOL "C fileno check singleton")
    MARK_AS_ADVANCED (C_FILENO_SINGLETON)
  ENDIF ()
ENDMACRO()
