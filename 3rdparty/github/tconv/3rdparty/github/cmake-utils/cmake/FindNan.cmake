MACRO (FINDNAN)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check nan, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_NAN_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_NAN_FOUND FALSE)
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
      MESSAGE(STATUS "Looking for working NAN")
      TRY_COMPILE (C_HAS_NAN ${CMAKE_CURRENT_BINARY_DIR}
        ${source_dir}/NAN.c
        COMPILE_DEFINITIONS -DC_NAN=NAN -DHAVE_MATH_H=${_HAVE_MATH_H})
      IF (C_HAS_NAN)
        MESSAGE(STATUS "Looking for working NAN - found")
        SET (_C_NAN NAN)
        SET (_C_NAN_FOUND TRUE)
      ELSE ()
        MESSAGE(STATUS "Looking for NAN replacement")
        TRY_COMPILE (C_HAS_NAN_REPLACEMENT ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/NAN.c
          COMPILE_DEFINITIONS -DHAVE_NAN_REPLACEMENT=1 -DHAVE_MATH_H=${_HAVE_MATH_H})
        IF (C_HAS_NAN_REPLACEMENT)
          MESSAGE(STATUS "Looking for NAN replacement - found")
          SET (_C_NAN_REPLACEMENT TRUE)
        ELSE ()
          MESSAGE(STATUS "Looking for NAN replacement using division")
          TRY_RUN(C_NAN_RUN_RESULT
            C_NAN_COMPILE_RESULT
            ${CMAKE_CURRENT_BINARY_DIR}
            ${source_dir}/NAN.c
            COMPILE_DEFINITIONS -DHAVE_NAN_REPLACEMENT_USING_DIVISION=1 -DHAVE_MATH_H=${_HAVE_MATH_H})
          IF (C_NAN_COMPILE_RESULT AND (C_NAN_RUN_RESULT EQUAL 0))
            MESSAGE(STATUS "Looking for NAN replacement using division - found")
            SET (_C_NAN_REPLACEMENT_USING_DIVISION TRUE)
          ENDIF ()
        ENDIF ()
      ENDIF ()
    ENDIF ()
    IF (_C_NAN_FOUND)
      SET (C_NAN "${_C_NAN}" CACHE STRING "C NAN implementation")
      MARK_AS_ADVANCED (C_NAN)
    ENDIF ()
    IF (_C_NAN_REPLACEMENT)
      SET (C_NAN_REPLACEMENT "(__builtin_nanf(\"\"))" CACHE STRING "C NAN_REPLACEMENT implementation")
      MARK_AS_ADVANCED (C_NAN_REPLACEMENT)
    ENDIF ()
    IF (_C_NAN_REPLACEMENT_USING_DIVISION)
      SET (C_NAN_REPLACEMENT_USING_DIVISION "(0.0/0.0)" CACHE STRING "C NAN_REPLACEMENT_USING_DIVISION implementation")
      MARK_AS_ADVANCED (C_NAN_REPLACEMENT_USING_DIVISION)
    ENDIF ()
    SET (C_NAN_SINGLETON TRUE CACHE BOOL "C NAN check singleton")
    MARK_AS_ADVANCED (C_NAN_SINGLETON)
  ENDIF ()
ENDMACRO()
