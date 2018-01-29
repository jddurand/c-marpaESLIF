#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <iconv.h>
#include <assert.h>

/* iconv interface is a direct proxy to iconv */

#include "tconv/convert/iconv.h"
#include "tconv_config.h"

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_new";
  iconv_t           iconvp;

  TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, tocodes, fromcodes);
  iconvp = iconv_open(tocodes, fromcodes);
  TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\") returns %p", funcs, tocodes, fromcodes, iconvp);

  return (iconvp == (iconv_t)-1) ? NULL : iconvp;
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *voidp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_run";
  iconv_t           iconvp  = (iconv_t) voidp;
  size_t            rcl;

  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  rcl = iconv(iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p) returns %ld", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp, (long) rcl);

  return rcl;
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_free";
  iconv_t           iconvp  = (iconv_t) voidp;
  int               rci;

  TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, iconvp);
  rci = iconv_close(iconvp);
  TCONV_TRACE(tconvp, "%s - iconv_close(%p) returns %d", funcs, iconvp, rci);

  return rci;
}
