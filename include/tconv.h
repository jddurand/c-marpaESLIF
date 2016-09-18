#ifndef TCONV_H
#define TCONV_H

#include <stddef.h>
#include <tconv/export.h>
#include <tconv/ext.h>

TCONV_EXPORT tconv_t  tconv_open(const char *tocodes, const char *fromcodes);
TCONV_EXPORT size_t   tconv(tconv_t tconvp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp);
TCONV_EXPORT int      tconv_close(tconv_t tconvp);

#endif /* TCONV_H */
