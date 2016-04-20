#ifndef TCONV_CONVERT_ICONV_H
#define TCONV_CONVERT_ICONV_H

#include <stddef.h>

typedef struct tconv_convert_iconv_option {
  short translitb;                 /* Default:    0 */
  short ignoreb;                   /* Default:    0 */
} tconv_convert_iconv_option_t;

#include "tconv.h"
#include "tconv/export.h"

TCONV_NO_EXPORT void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *optionp);
TCONV_NO_EXPORT size_t tconv_convert_iconv_run(tconv_t tconvp, void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp);
TCONV_NO_EXPORT int    tconv_convert_iconv_free(tconv_t tconvp, void *contextp);

#endif /*  TCONV_CONVERT_ICONV_H */
