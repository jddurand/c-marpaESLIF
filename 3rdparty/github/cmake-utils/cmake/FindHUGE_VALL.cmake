MACRO (FINDHUGE_VALL)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check inline, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_HUGE_VALL_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_HUGE_VALL_FOUND FALSE)
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
      MESSAGE(STATUS "Looking for working HUGE_VALL")
      TRY_COMPILE (C_HAS_HUGE_VALL ${CMAKE_CURRENT_BINARY_DIR}
        ${source_dir}/HUGE_VALL.c
        COMPILE_DEFINITIONS -DC_HUGE_VALL=HUGE_VALL -DHAVE_MATH_H=${_HAVE_MATH_H})
      IF (C_HAS_HUGE_VALL)
        MESSAGE(STATUS "Looking for working HUGE_VALL - found")
        SET (_C_HUGE_VALL HUGE_VALL)
        SET (_C_HUGE_VALL_FOUND TRUE)
      ELSE ()
        MESSAGE(STATUS "Looking for HUGE_VALL replacement")
        TRY_COMPILE (C_HAS_HUGE_VALL_REPLACEMENT ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/HUGE_VALL.c
          COMPILE_DEFINITIONS -DC_HUGE_VALL=HUGE_VALL -DHAVE_HUGE_VALL_REPLACEMENT=1 -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_HUGE_VALL_REPLACEMENT)
          MESSAGE(STATUS "Looking for HUGE_VALL replacement - found")
          SET (_C_HUGE_VALL HUGE_VALL)
          SET (_C_HUGE_VALL_FOUND TRUE)
          SET (_C_HUGE_VALL_REPLACEMENT TRUE)
      ELSE ()
      ENDIF ()
      ENDIF ()
    ENDIF ()
    IF (_C_HUGE_VALL_FOUND)
      SET (C_HUGE_VALL "${_C_HUGE_VALL}" CACHE STRING "C HUGE_VALL implementation")
      MARK_AS_ADVANCED (C_HUGE_VALL)
    ENDIF ()
    IF (_C_HUGE_VALL_REPLACEMENT)
      SET (C_HUGE_VALL_REPLACEMENT "(__builtin_huge_vall())" CACHE STRING "C HUGE_VALL_REPLACEMENT implementation")
      MARK_AS_ADVANCED (C_HUGE_VALL_REPLACEMENT)
    ENDIF ()
    SET (C_HUGE_VALL_SINGLETON TRUE CACHE BOOL "C HUGE_VALL check singleton")
    MARK_AS_ADVANCED (C_HUGE_VALL_SINGLETON)
  ENDIF ()
ENDMACRO()
