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

  TCONV_TRACE(tconvp, "%s - iconv_open(%p, %p", funcs, tocodes, fromcodes);
  return iconv_open(tocodes, fromcodes);
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *voidp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_run";
  iconv_t           iconvp  = (iconv_t) voidp;

  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  return iconv(iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_free";
  iconv_t           iconvp  = (iconv_t) voidp;

  TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, iconvp);
  return iconv_close(iconvp);
}
