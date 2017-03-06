# Inspired from /usr/share/autoconf/autoconf/c.m4
#
# Note: __forceinline also exist, but is not recommended if the C compiler
#       think it is not worth inlining
#
MACRO (FINDINLINE)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check inline, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_INLINE_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_INLINE_FOUND FALSE)
      FOREACH (KEYWORD "inline" "__inline__" "inline__" "__inline")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/inline.c
          COMPILE_DEFINITIONS "-DC_INLINE=${KEYWORD}")
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_INLINE ${KEYWORD})
          SET (_C_INLINE_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_INLINE_FOUND)
      SET (C_INLINE "${_C_INLINE}" CACHE STRING "C inline keyword")
      MARK_AS_ADVANCED (C_INLINE)
    ENDIF ()
    SET (C_INLINE_SINGLETON TRUE CACHE BOOL "C inline check singleton")
    MARK_AS_ADVANCED (C_INLINE_SINGLETON)
  ENDIF ()
ENDMACRO()
