#include <iconv.h>

#include "tconv/convert/iconv.h"
/* Because this is a built-in, it can take advantage of TCONV_TRACE macro */
#include "tconv_config.h"

/* iconv interface is a direct proxy to iconv: context is iconv_t itself */

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_new";
  iconv_t           iconvp;

  TCONV_TRACE(tconvp, "%s - iconv_open(%p, %p)", funcs, tocodes, fromcodes);
  iconvp = iconv_open(tocodes, fromcodes);

  TCONV_TRACE(tconvp, "%s - return %p", funcs, iconvp);
  return iconvp;
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *voidp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_run";
  iconv_t           iconvp  = (iconv_t) voidp;
  size_t            rcl;

  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  rcl = iconv(iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

#ifndef TCONV_NTRACE
  if (rcl == (size_t)-1) {
    TCONV_TRACE(tconvp, "%s - return -1", funcs);
  } else {
    TCONV_TRACE(tconvp, "%s - return %lld", funcs, (signed long long) rcl);
  }
#endif
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

  TCONV_TRACE(tconvp, "%s - return %d", funcs, rci);
  return rci;
}
