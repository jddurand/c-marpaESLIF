#ifndef TCONV_CHARSET_CCHARDET_H
#define TCONV_CHARSET_CCHARDET_H

#include <stddef.h>

#include "tconv_export.h"

typedef struct tconv_charset_cchardet { /*      Built-in */
  float confidencef;                    /* Default: 0.1f */
} tconv_charset_cchardet_t;

TCONV_NO_EXPORT void *tconv_charset_cchardet_new(void *optionp);
TCONV_NO_EXPORT char *tconv_charset_cchardet_run(void *contextp, char *bytep, size_t bytel);
TCONV_NO_EXPORT void  tconv_charset_cchardet_free(void *contextp);

#endif /*  TCONV_CHARSET_CCHARDET_H */
