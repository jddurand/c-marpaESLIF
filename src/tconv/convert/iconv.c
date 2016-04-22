#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include "tconv/convert/iconv.h"

/***********************************************************************/
/* Our semantics are the iconv ones - so naturally the iconv interface */
/* should do nothing else but call iconv -;                            */
/***********************************************************************/

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  return iconv_open(tocodes, fromcodes);
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  return iconv((iconv_t) contextp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *contextp)
/*****************************************************************************/
{
  return iconv_close((iconv_t) contextp);
}
