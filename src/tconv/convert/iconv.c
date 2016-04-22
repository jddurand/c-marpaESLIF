#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include "tconv/convert/iconv.h"

#define TCONV_CONVERT_ICONV_TRANSLIT "//TRANSLIT"
#define TCONV_CONVERT_ICONV_IGNORE "//IGNORE"

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  iconv_t                       iconvp;
  
  if ((tocodes == NULL) || (fromcodes == NULL)) {
    goto err;
  }

  iconvp = iconv_open(tocodes, fromcodes);
  if (iconvp == NULL) {
    goto err;
  }

  return iconvp;
  
 err:
  errno = EINVAL;
  return NULL;
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  iconv_t iconvp = (iconv_t) contextp;

  if (iconvp == NULL) {
    goto err;
  }

  return iconv(iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

 err:
  errno = EINVAL;
  return (size_t)-1;
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *contextp)
/*****************************************************************************/
{
  iconv_t iconvp = (iconv_t) contextp;

  if (iconvp == NULL) {
    goto err;
  }

  return iconv_close(iconvp);

 err:
  errno = EINVAL;
  return -1;
}

