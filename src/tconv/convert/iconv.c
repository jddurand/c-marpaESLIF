#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include "tconv/convert/iconv.h"

typedef struct tconv_convert_iconv_context {
  iconv_t          iconvp;
} tconv_convert_iconv_context_t;

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char              funcs[] = "tconv_convert_iconv_new";
  iconv_t                        iconvp;
  tconv_convert_iconv_context_t *contextp;

  contextp = malloc(sizeof(tconv_convert_iconv_context_t));
  if (contextp == NULL) {
    goto err;
  }
  contextp->iconvp         = NULL;

  tconv_trace(tconvp, "%s - iconv_open", funcs);

  iconvp = iconv_open(tocodes, fromcodes);
  if (iconvp == NULL) {
    tconv_trace(tconvp, "%s - iconv_open - %s", funcs, strerror(errno));
    goto err;
  }

  contextp->iconvp         = iconvp;

  return contextp;

 err:
  {
    int errnol = errno;
    if (iconvp != NULL) { iconv_close(iconvp); }
    if (contextp != NULL) { free(contextp); }
    errno = errnol;
  }
  return NULL;
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *voidp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char                 funcs[] = "t tconv_convert_iconv_run";
  tconv_convert_iconv_context_t    *contextp = (tconv_convert_iconv_context_t *) voidp;
  size_t                            n;
  
  if (contextp == NULL) {
    errno = EFAULT;
    goto err;
  }

  tconv_trace(tconvp, "%s - iconv", funcs);
  n = iconv((iconv_t) contextp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

  return n;

 err:
  return (size_t)-1;
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char                 funcs[] = "t tconv_convert_iconv_free";
  tconv_convert_iconv_context_t    *contextp = (tconv_convert_iconv_context_t *) voidp;
  int                               i;

  if (contextp == NULL) {
    errno = EFAULT;
    goto err;
  }

  i = iconv_close((iconv_t) contextp);
  free(contextp);

  return i;

 err:
  return -1;
}
