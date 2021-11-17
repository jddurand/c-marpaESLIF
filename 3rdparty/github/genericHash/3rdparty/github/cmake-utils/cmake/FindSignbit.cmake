MACRO (FINDSIGNBIT)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check signbit, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_SIGNBIT_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_SIGNBIT_FOUND FALSE)
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
      FOREACH (KEYWORD "signbit" "_signbit" "__signbit")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/signbit.c
          COMPILE_DEFINITIONS -DC_SIGNBIT=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_SIGNBIT ${KEYWORD})
          SET (_C_SIGNBIT_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_SIGNBIT_FOUND)
      SET (C_SIGNBIT "${_C_SIGNBIT}" CACHE STRING "C signbit implementation")
      MARK_AS_ADVANCED (C_SIGNBIT)
    ENDIF ()
    SET (C_SIGNBIT_SINGLETON TRUE CACHE BOOL "C signbit check singleton")
    MARK_AS_ADVANCED (C_SIGNBIT_SINGLETON)
  ENDIF ()
ENDMACRO()
