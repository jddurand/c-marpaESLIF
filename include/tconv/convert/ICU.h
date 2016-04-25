#ifndef TCONV_CONVERT_ICU_H
#define TCONV_CONVERT_ICU_H

#include <stddef.h>

typedef struct tconv_convert_icu_option {
  size_t uCharSizel;           /* Size of intermediary buffer. Default: 1024*1024 */
} tconv_convert_icu_option_t;

#include "tconv.h"
#include "tconv/export.h"

TCONV_NO_EXPORT void  *tconv_convert_ICU_new (tconv_t tconv, const char *tocodes, const char *fromcodes, void *optionp);
TCONV_NO_EXPORT size_t tconv_convert_ICU_run (tconv_t tconv, void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp);
TCONV_NO_EXPORT int    tconv_convert_ICU_free(tconv_t tconv, void *contextp);

#endif /*  TCONV_CONVERT_ICU_H */
