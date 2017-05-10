# - Check iconv capabilities:
# 
# ICONV_CAN_TRANSLIT      - iconv supports //TRANSLIT option
# ICONV_CAN_IGNORE        - iconv supports //IGNORE option
# 
# Just to be sure, we do not limit the calls to iconv_open, but do the full thing
# Input is FORCED with a valid ASCII and UTF-8 compliant thingy.
# Caller have the choice of destination charset and have to provide TWO TRAILING BYTES.
#
MACRO(_FINDICONVCAPABILITY tocharset fromcharset outputvariable twobytes)
  include(CheckCSourceRuns)
  SET (CMAKE_REQUIRED_INCLUDES_PREVIOUS ${CMAKE_REQUIRED_INCLUDES})
  SET (CMAKE_REQUIRED_LIBRARIES_PREVIOUS ${CMAKE_REQUIRED_LIBRARIES})
  SET (CMAKE_REQUIRED_DEFINITIONS_PREVIOUS ${CMAKE_REQUIRED_DEFINITIONS})
  SET (CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_PREVIOUS} ${ICONV_INCLUDE_DIR})
  SET (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_PREVIOUS} ${ICONV_LINK_FLAGS})
  IF (WINICONV)
    SET (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS_PREVIOUS} -DWINICONV=1)
  ELSE ()
    SET (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS_PREVIOUS} -DWINICONV=0)
  ENDIF ()
  CHECK_C_SOURCE_RUNS ("
  #include <stdlib.h>
  #include <stdio.h>
  #include <stddef.h>
  #include <string.h>
  #if WINICONV
  #include <win_iconv.c>
  #endif
  #include <iconv.h>
  int main(){
    iconv_t  conv;
    char    *src = \"123456\" ${twobytes};
    char     dst[1024];
    char    *inp = src;                             /* Input moving pointer */
    char    *outp = dst;                            /* Output moving pointer */
    size_t   inl = 8;                               /* 6 + 2 */
    size_t   outl = 1024;                           /* 1024 bytes available in output buffer */

    memset((void *) dst, '\\\\0', 1024);
    conv = iconv_open(\"${tocharset}\", \"${fromcharset}\");
    if (conv == (iconv_t)-1) { perror(\"iconv_open\"); return 1; }
    if (iconv(conv, &inp, &inl, &outp, &outl) == (size_t)-1)  { perror(\"iconv\"); return 1; }
    /* Flush */
    if (iconv(conv, NULL, NULL, &outp, &outl) == (size_t)-1)  { perror(\"iconv\"); return 1; }
    if (iconv_close(conv) == -1)  { perror(\"iconv_close\"); return 1; }
    fprintf(stdout, \"%s\\\\n\", dst);
    return 0;
  }
" ${outputvariable} )
  SET (CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_PREVIOUS})
  SET (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_PREVIOUS})
  SET (CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS_PREVIOUS})
  #
  # Just make sure this is a boolean
  #
  IF (${outputvariable})
    SET (${outputvariable} TRUE)
  ELSE ()
    SET (${outputvariable} FALSE)
  ENDIF ()
ENDMACRO()


SET (TWO_UTF8_BYTES_OK "\"!!\"")
SET (TWO_UTF8_BYTES_KO "\"\\\\\\\\xa0\\\\\\\\xa1\"")

_FINDICONVCAPABILITY("ASCII//TRANSLIT" "UTF-8" ICONV_CAN_TRANSLIT           ${TWO_UTF8_BYTES_OK})
_FINDICONVCAPABILITY("ASCII//IGNORE"   "UTF-8" ICONV_CAN_IGNORE             ${TWO_UTF8_BYTES_OK})

MESSAGE (WARNING "In the iconv plugin, when input and destination charsets are equivalent, they will NOT be checked and fuzzy conversion will happen")

MESSAGE(STATUS "-----------------------------------------")
MESSAGE(STATUS "Iconv capabilities:")
MESSAGE(STATUS "")
MESSAGE(STATUS "           ICONV_CAN_TRANSLIT: ${ICONV_CAN_TRANSLIT}")
MESSAGE(STATUS "             ICONV_CAN_IGNORE: ${ICONV_CAN_IGNORE}")
MESSAGE(STATUS "-----------------------------------------")

MARK_AS_ADVANCED(
  ICONV_CAN_TRANSLIT
  ICONV_CAN_IGNORE
  )
