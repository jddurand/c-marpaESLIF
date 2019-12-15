MACRO (FINDISINF)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check inline, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_ISINF_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_ISINF_FOUND FALSE)
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
      MESSAGE(STATUS "Looking for working isinf")
      TRY_COMPILE (C_HAS_ISINF ${CMAKE_CURRENT_BINARY_DIR}
        ${source_dir}/isinf.c
        COMPILE_DEFINITIONS -DC_ISINF=isinf -DHAVE_MATH_H=${_HAVE_MATH_H})
      IF (C_HAS_ISINF)
        MESSAGE(STATUS "Looking for working isinf - found")
        SET (_C_ISINF isinf)
        SET (_C_ISINF_FOUND TRUE)
      ENDIF ()
    ENDIF ()
    IF (_C_ISINF_FOUND)
      SET (C_ISINF "${_C_ISINF}" CACHE STRING "C isinf implementation")
      MARK_AS_ADVANCED (C_ISINF)
    ENDIF ()
    SET (C_ISINF_SINGLETON TRUE CACHE BOOL "C isinf check singleton")
    MARK_AS_ADVANCED (C_ISINF_SINGLETON)
  ENDIF ()
ENDMACRO()
