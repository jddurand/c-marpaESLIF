#ifndef TCONV_CONVERT_ICONV_H
#define TCONV_CONVERT_ICONV_H

#include <stddef.h>

#include "tconv_export.h"

typedef struct tconv_convert_iconv {    /* Built-in */
  short translitb;                 /* Default:    0 */
  short ignoreb;                   /* Default:    0 */
} tconv_convert_iconv_t;

TCONV_NO_EXPORT void  *tconv_convert_iconv_open(const char *tocodes, const char *fromcodes, void *optionp);
TCONV_NO_EXPORT size_t tconv_convert_iconv_run(void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp);
TCONV_NO_EXPORT int    tconv_convert_iconv_close(void *contextp);

#endif /*  TCONV_CONVERT_ICONV_H */
