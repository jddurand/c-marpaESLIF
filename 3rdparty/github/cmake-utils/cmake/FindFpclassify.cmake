MACRO (FINDFPCLASSIFY)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check inline, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_FPCLASSIFY_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_FPCLASSIFY_FOUND FALSE)
      #
      # We depend on math and float
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("math.h" HAVE_MATH_H)
      IF (HAVE_MATH_H)
        SET (_HAVE_MATH_H 1)
      ELSE ()
        SET (_HAVE_MATH_H 0)
      ENDIF ()
      CHECK_INCLUDE_FILE ("float.h" HAVE_FLOAT_H)
      IF (HAVE_FLOAT_H)
        SET (_HAVE_FLOAT_H 1)
      ELSE ()
        SET (_HAVE_FLOAT_H 0)
      ENDIF ()
      #
      # Test
      #
      FOREACH (KEYWORD "fpclassify" "_fpclassify" "__fpclassify" "fpclass" "_fpclass" "__fpclass")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/fpclassify.c
          COMPILE_DEFINITIONS -DC_FPCLASSIFY=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H} -DHAVE_FLOAT_H=${_HAVE_FLOAT_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_FPCLASSIFY ${KEYWORD})
          SET (_C_FPCLASSIFY_FOUND TRUE)
          FOREACH (KEYWORD "FP_NAN" "_FPCLASS_SNAN" "_FPCLASS_QNAN" "FP_INFINITE" "_FPCLASS_NINF" "_FPCLASS_PINF")
            MESSAGE(STATUS "Looking for ${KEYWORD}")
            TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
              ${source_dir}/FP_CONSTANT.c
              COMPILE_DEFINITIONS -DC_${KEYWORD}=${KEYWORD} -DHAVE_MATH_H=${_HAVE_MATH_H} -DHAVE_FLOAT_H=${_HAVE_FLOAT_H})
            IF (C_HAS_${KEYWORD})
              MESSAGE(STATUS "Looking for ${KEYWORD} - found")
              SET (C_${KEYWORD} "${_C_${KEYWORD}}" CACHE STRING "C ${KEYWORD} implementation")
              MARK_AS_ADVANCED (C_${KEYWORD})
            ENDIF ()
          ENDFOREACH ()
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_FPCLASSIFY_FOUND)
      SET (C_FPCLASSIFY "${_C_FPCLASSIFY}" CACHE STRING "C fpclassify implementation")
      MARK_AS_ADVANCED (C_FPCLASSIFY)
    ENDIF ()
    SET (C_FPCLASSIFY_SINGLETON TRUE CACHE BOOL "C fpclassify check singleton")
    MARK_AS_ADVANCED (C_FPCLASSIFY_SINGLETON)
  ENDIF ()
ENDMACRO()
