MACRO (FINDSTRTOLL)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check strtoll, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_STRTOLL_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_STRTOLL_FOUND FALSE)
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
      FOREACH (KEYWORD "strtoll" "_strtoll" "__strtoll" "strtoi64" "_strtoi64" "__strtoi64")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/strtoll.c
          COMPILE_DEFINITIONS -DC_STRTOLL=${KEYWORD} -DHAVE_STDLIB_H=${_HAVE_STDLIB_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_STRTOLL ${KEYWORD})
          SET (_C_STRTOLL_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_STRTOLL_FOUND)
      SET (C_STRTOLL "${_C_STRTOLL}" CACHE STRING "C strtoll implementation")
      MARK_AS_ADVANCED (C_STRTOLL)
    ENDIF ()
    SET (C_STRTOLL_SINGLETON TRUE CACHE BOOL "C strtoll check singleton")
    MARK_AS_ADVANCED (C_STRTOLL_SINGLETON)
  ENDIF ()
ENDMACRO()
