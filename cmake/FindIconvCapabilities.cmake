# - Check iconv capabilities:
# 
# ICONV_CAN_SAME_CHARSETS - iconv supports identical charsets in input
# ICONV_CAN_TRANSLIT      - iconv supports //TRANSLIT option
# ICONV_CAN_IGNORE        - iconv supports //IGNORE option
# 
include(CheckCSourceCompiles)

SET (CMAKE_REQUIRED_INCLUDES_PREVIOUS ${CMAKE_REQUIRED_INCLUDES})
SET (CMAKE_REQUIRED_LIBRARIES_PREVIOUS ${CMAKE_REQUIRED_LIBRARIES})
SET (CMAKE_REQUIRED_INCLUDES ${ICONV_INCLUDE_DIR})
SET (CMAKE_REQUIRED_LIBRARIES ${ICONV_LINK_FLAGS})

MESSAGE(STATUS "Checking if iconv supports identical charsets in input")
CHECK_C_SOURCE_COMPILES ("
  #include <iconv.h>
  int main(){
    iconv_t conv;

    conv = iconv_open(\"ASCII\", \"ASCII\");
    return (conv == (iconv_t)-1) ? 1 : 0;
  }
" ICONV_CAN_SAME_CHARSETS )
IF (NOT ICONV_CAN_SAME_CHARSETS)
  MESSAGE (WARNING "When input and destination charsets are equivalent, they will NOT be checked and fuzzy conversion will happen")
ENDIF ()

MESSAGE(STATUS "Checking if iconv supports //TRANSLIT option")
CHECK_C_SOURCE_COMPILES ("
  #include <iconv.h>
  int main(){
    iconv_t conv;

    conv = iconv_open(\"ASCII//TRANSLIT\", \"UTF-8\");
    return (conv == (iconv_t)-1) ? 1 : 0;
  }
" ICONV_CAN_TRANSLIT )
IF (NOT ICONV_CAN_TRANSLIT)
  MESSAGE(STATUS "Checking if iconv //TRANSLIT option can be ignored")
  #
  # We expect a replacement character to have happened automatically, then
  #
  CHECK_C_SOURCE_COMPILES ("
  #include <stdlib.h>
  #include <stdio.h>
  #include <stddef.h>
  #include <iconv.h>
  int main(){
    iconv_t  conv;
    char    *src = \"123\\\\xE2\\\\x82\\\\xAC\" \"456\"; /* In UTF8: 123, euro sign, 456 */
    char     dst[1024];
    char    *inp = src;                             /* Input moving pointer */
    char    *outp = dst;                            /* Output moving pointer */
    size_t   inl = 9;                               /* In UTF8: 9 bytes remaining */
    size_t   outl = 1024;                           /* 1024 bytes available in output buffer */
    int      i;

    conv = iconv_open(\"ASCII\", \"UTF-8\");
    if (conv == (iconv_t)-1) { perror(\"iconv_open\"); return 1; }
    if (iconv(conv, &inp, &inl, &outp, &outl) == (size_t)-1)  { perror(\"iconv\"); return 1; }
    /* Flush */
    if (iconv(conv, NULL, NULL, &outp, &outl) == (size_t)-1)  { perror(\"iconv\"); return 1; }
    if (iconv_close(conv) == -1)  { perror(\"iconv_close\"); return 1; }
    outp = &(dst[3]);
    while (*++outp != '4');
    *outp = '\\\\0';
    outp = &(dst[3]);
    fprintf(stdout, \"Euro character \\\\\\\\xE2\\\\\\\\x82\\\\\\\\xAC has been transformed to %s\\\\n\", outp);
    return 0;
  }
" ICONV_TRANSLIT_CAN_BE_IGNORED )
ELSE ()
  SET (ICONV_TRANSLIT_CAN_BE_IGNORED FALSE)
ENDIF ()

MESSAGE(STATUS "Checking if iconv supports //IGNORE option")
CHECK_C_SOURCE_COMPILES ("
  #include <iconv.h>
  int main(){
    iconv_t conv;

    conv = iconv_open(\"ASCII//IGNORE\", \"UTF-8\");
    return (conv == (iconv_t)-1) ? 1 : 0;
  }
" ICONV_CAN_IGNORE )

SET (CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES_PREVIOUS})
SET (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_PREVIOUS})

MESSAGE(STATUS "-----------------------------------------")
MESSAGE(STATUS "Iconv capabilities:")
MESSAGE(STATUS "")
MESSAGE(STATUS "      ICONV_CAN_SAME_CHARSETS: ${ICONV_CAN_SAME_CHARSETS}")
MESSAGE(STATUS "           ICONV_CAN_TRANSLIT: ${ICONV_CAN_TRANSLIT}")
MESSAGE(STATUS "ICONV_TRANSLIT_CAN_BE_IGNORED: ${ICONV_TRANSLIT_CAN_BE_IGNORED}")
MESSAGE(STATUS "             ICONV_CAN_IGNORE: ${ICONV_CAN_IGNORE}")
MESSAGE(STATUS "-----------------------------------------")

MARK_AS_ADVANCED(
  ICONV_CAN_SAME_CHARSETS
  ICONV_CAN_TRANSLIT
  ICONV_TRANSLIT_CAN_BE_IGNORED
  ICONV_CAN_IGNORE
  )
