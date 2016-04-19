#ifndef TCONV_CHARSET_ICU_H
#define TCONV_CHARSET_ICU_H

#include <stddef.h>

typedef struct tconv_charset_ICU { /*      Built-in */
  int confidencei;                 /* Default:  0.1 */
} tconv_charset_ICU_t;

TCONV_NO_EXPORT void *tconv_charset_ICU_new(void *optionp);
TCONV_NO_EXPORT char *tconv_charset_ICU_run(void *contextp, char *bytep, size_t bytel);
TCONV_NO_EXPORT void  tconv_charset_ICU_free(void *contextp);

#endif /*  TCONV_CHARSET_ICU_H */
