MACRO (FINDEBCDIC)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check EBCDIC, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_EBCDIC_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_EBCDIC_FOUND FALSE)
      #
      # We depend on stdlib.h
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
      MESSAGE(STATUS "Looking for EBCDIC")
      TRY_RUN(C_EBCDIC_RUN_RESULT
        C_EBCDIC_COMPILE_RESULT
        ${CMAKE_CURRENT_BINARY_DIR}
        ${source_dir}/EBCDIC.c
        COMPILE_DEFINITIONS -DHAVE_STDLIB_H=${_HAVE_STDLIB_H}
        RUN_OUTPUT_VARIABLE C_EBCDIC_VAL_OUTPUT)
      IF (C_EBCDIC_COMPILE_RESULT AND (C_EBCDIC_RUN_RESULT EQUAL 0))
        SET(_EBCDIC TRUE)
        MESSAGE(STATUS "Looking for EBCDIC - yes")
      ELSE ()
        SET(_EBCDIC FALSE)
        MESSAGE(STATUS "Looking for EBCDIC - no")
      ENDIF ()
    ENDIF ()
    SET (EBCDIC ${_EBCDIC} CACHE BOOL "System coding is EBCDIC")
    MARK_AS_ADVANCED (EBCDIC)
    SET (C_EBCDIC_SINGLETON TRUE CACHE BOOL "C EBCDIC check singleton")
    MARK_AS_ADVANCED (C_EBCDIC_SINGLETON)
  ENDIF ()
ENDMACRO()
