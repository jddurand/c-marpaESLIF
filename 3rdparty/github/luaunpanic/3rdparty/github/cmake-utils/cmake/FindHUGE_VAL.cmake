MACRO (FINDHUGE_VAL)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check HUGE_VAL, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_HUGE_VAL_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_HUGE_VAL_FOUND FALSE)
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
      MESSAGE(STATUS "Looking for working HUGE_VAL")
      TRY_COMPILE (C_HAS_HUGE_VAL ${CMAKE_CURRENT_BINARY_DIR}
        ${source_dir}/HUGE_VAL.c
        COMPILE_DEFINITIONS -DC_HUGE_VAL=HUGE_VAL -DHAVE_MATH_H=${_HAVE_MATH_H})
      IF (C_HAS_HUGE_VAL)
        MESSAGE(STATUS "Looking for working HUGE_VAL - found")
        SET (_C_HUGE_VAL HUGE_VAL)
        SET (_C_HUGE_VAL_FOUND TRUE)
      ELSE ()
        MESSAGE(STATUS "Looking for HUGE_VAL replacement")
        TRY_COMPILE (C_HAS_HUGE_VAL_REPLACEMENT ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/HUGE_VAL.c
          COMPILE_DEFINITIONS -DC_HUGE_VAL=HUGE_VAL -DHAVE_HUGE_VAL_REPLACEMENT=1 -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_HUGE_VAL_REPLACEMENT)
          MESSAGE(STATUS "Looking for HUGE_VAL replacement - found")
          SET (_C_HUGE_VAL_REPLACEMENT TRUE)
      ELSE ()
      ENDIF ()
      ENDIF ()
    ENDIF ()
    IF (_C_HUGE_VAL_FOUND)
      SET (C_HUGE_VAL "${_C_HUGE_VAL}" CACHE STRING "C HUGE_VAL implementation")
      MARK_AS_ADVANCED (C_HUGE_VAL)
    ENDIF ()
    IF (_C_HUGE_VAL_REPLACEMENT)
      SET (C_HUGE_VAL_REPLACEMENT "(__builtin_huge_val())" CACHE STRING "C HUGE_VAL_REPLACEMENT implementation")
      MARK_AS_ADVANCED (C_HUGE_VAL_REPLACEMENT)
    ENDIF ()
    SET (C_HUGE_VAL_SINGLETON TRUE CACHE BOOL "C HUGE_VAL check singleton")
    MARK_AS_ADVANCED (C_HUGE_VAL_SINGLETON)
  ENDIF ()
ENDMACRO()
