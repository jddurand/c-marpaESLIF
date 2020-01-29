MACRO (FINDVACOPY)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check va_copy, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_VA_COPY_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_VA_COPY_FOUND FALSE)
      #
      # We depend on stdarg
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("stdarg.h"     HAVE_STDARG_H)
      IF (HAVE_STDARG_H)
        SET (_HAVE_STDARG_H 1)
      ELSE ()
        SET (_HAVE_STDARG_H 0)
      ENDIF ()
      #
      # Test
      #
      FOREACH (KEYWORD "va_copy" "_va_copy" "__va_copy")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/va_copy.c
          COMPILE_DEFINITIONS -DC_VA_COPY=${KEYWORD} -DHAVE_STDARG_H=${_HAVE_STDARG_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_VA_COPY ${KEYWORD})
          SET (_C_VA_COPY_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_VA_COPY_FOUND)
      SET (C_VA_COPY "${_C_VA_COPY}" CACHE STRING "C va_copy implementation")
      MARK_AS_ADVANCED (C_VA_COPY)
    ENDIF ()
    SET (C_VA_COPY_SINGLETON TRUE CACHE BOOL "C va_copy check singleton")
    MARK_AS_ADVANCED (C_VA_COPY_SINGLETON)
  ENDIF ()
ENDMACRO()
