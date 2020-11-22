MACRO (FINDLOCALTIME_R)
  GET_PROPERTY(source_dir_set GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR SET)
  IF (NOT ${source_dir_set})
    MESSAGE (WARNING "Cannot check localtime_r, property MYPACKAGE_SOURCE_DIR is not set")
  ELSE ()
    IF (NOT C_LOCALTIME_R_SINGLETON)
      GET_PROPERTY(source_dir GLOBAL PROPERTY MYPACKAGE_SOURCE_DIR)
      SET (_C_LOCALTIME_R_FOUND FALSE)
      #
      # We depend on time
      #
      INCLUDE (CheckIncludeFile)
      CHECK_INCLUDE_FILE ("time.h" HAVE_TIME_H)
      IF (HAVE_TIME_H)
        SET (_HAVE_TIME_H 1)
      ELSE ()
        SET (_HAVE_TIME_H 0)
      ENDIF ()
      #
      # Test
      #
      FOREACH (KEYWORD "localtime_r" "_localtime_r" "__localtime_r")
        MESSAGE(STATUS "Looking for ${KEYWORD}")
        TRY_COMPILE (C_HAS_${KEYWORD} ${CMAKE_CURRENT_BINARY_DIR}
          ${source_dir}/localtime_r.c
          COMPILE_DEFINITIONS -DC_LOCALTIME_R=${KEYWORD} -DHAVE_TIME_H=${_HAVE_TIME_H})
        IF (C_HAS_${KEYWORD})
          MESSAGE(STATUS "Looking for ${KEYWORD} - found")
          SET (_C_LOCALTIME_R ${KEYWORD})
          SET (_C_LOCALTIME_R_FOUND TRUE)
          BREAK ()
        ENDIF ()
      ENDFOREACH ()
    ENDIF ()
    IF (_C_LOCALTIME_R_FOUND)
      SET (C_LOCALTIME_R "${_C_LOCALTIME_R}" CACHE STRING "C localtime_r implementation")
      MARK_AS_ADVANCED (C_LOCALTIME_R)
    ENDIF ()
    SET (C_LOCALTIME_R_SINGLETON TRUE CACHE BOOL "C localtime_r check singleton")
    MARK_AS_ADVANCED (C_LOCALTIME_R_SINGLETON)
  ENDIF ()
ENDMACRO()
