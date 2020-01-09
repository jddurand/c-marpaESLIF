MACRO (FINDCHAR_BIT)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check CHAR_BIT, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_CHAR_BIT_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_CHAR_BIT_FOUND FALSE)
      #
      # We depend on limits.h, stdio.h, stdlib.h
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("limits.h" HAVE_LIMITS_H)
      IF (HAVE_LIMITS_H)
        SET (_HAVE_LIMITS_H 1)
      ELSE ()
        SET (_HAVE_LIMITS_H 0)
      ENDIF ()
      CHECK_INCLUDE_FILE ("stdio.h" HAVE_STDIO_H)
      IF (HAVE_STDIO_H)
        SET (_HAVE_STDIO_H 1)
      ELSE ()
        SET (_HAVE_STDIO_H 0)
      ENDIF ()
      CHECK_INCLUDE_FILE ("stdlib.h" HAVE_STDLIB_H)
      IF (HAVE_STDLIB_H)
        SET (_HAVE_STDLIB_H 1)
      ELSE ()
        SET (_HAVE_STDLIB_H 0)
      ENDIF ()
      #
      # Test
      #
      MESSAGE(STATUS "Looking for CHAR_BIT")
      TRY_RUN(C_CHAR_BIT_RUN_RESULT
        C_CHAR_BIT_COMPILE_RESULT
        ${CMAKE_CURRENT_BINARY_DIR}
        ${source_dir}/CHAR_BIT.c
        COMPILE_DEFINITIONS -DHAVE_STDIO_H=${_HAVE_STDIO_H} -DHAVE_STDLIB_H=${_HAVE_STDLIB_H} -DHAVE_LIMITS_H=${_HAVE_LIMITS_H}
        RUN_OUTPUT_VARIABLE C_CHAR_BIT_VAL_OUTPUT)
      IF (C_CHAR_BIT_COMPILE_RESULT AND (C_CHAR_BIT_RUN_RESULT EQUAL 0))
        #
        # CHAR_BIT minimum value is 8, regardless of the C standard
        #
        IF (${C_CHAR_BIT_VAL_OUTPUT} LESS 8)
          MESSAGE(FATAL_ERROR "CHAR_BIT minimum value is 8, got ${C_CHAR_BIT_VAL_OUTPUT}")
        ENDIF ()
        MESSAGE(STATUS "Looking for CHAR_BIT - ${C_CHAR_BIT_VAL_OUTPUT}")
      ELSE ()
        MESSAGE(WARNING "Assuming 8 bit and this CAN BE WRONG - cross fingers")
        SET (C_CHAR_BIT_VAL_OUTPUT 8)
      ENDIF ()
      SET (_C_CHAR_BIT ${C_CHAR_BIT_VAL_OUTPUT})
      SET (_C_CHAR_BIT_FOUND TRUE)
    ENDIF ()
    IF (_C_CHAR_BIT_FOUND)
      SET (C_CHAR_BIT "${_C_CHAR_BIT}" CACHE STRING "C CHAR_BIT implementation")
      MARK_AS_ADVANCED (C_CHAR_BIT)
    ENDIF ()
    SET (C_CHAR_BIT_SINGLETON TRUE CACHE BOOL "C CHAR_BIT check singleton")
    MARK_AS_ADVANCED (C_CHAR_BIT_SINGLETON)
  ENDIF ()
ENDMACRO()
