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

  tconv_trace(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) sizeof(tconv_convert_iconv_context_t));
  contextp = malloc(sizeof(tconv_convert_iconv_context_t));
  if (contextp == NULL) {
    tconv_trace(tconvp, "%s - malloc failure, %s", funcs, strerror(errno));
    goto err;
  } else {
    tconv_trace(tconvp, "%s - malloc success: %p", funcs, contextp);
  }

  contextp->iconvp  = NULL;

  tconv_trace(tconvp, "%s - iconv_open(%p, %p)", funcs, tocodes, fromcodes);
  iconvp = iconv_open(tocodes, fromcodes);
  if (iconvp == NULL) {
    tconv_trace(tconvp, "%s - iconv_open failure, %s", funcs, strerror(errno));
    goto err;
  } else {
    tconv_trace(tconvp, "%s - iconv_open success: %p", funcs, iconvp);
  }

  contextp->iconvp = iconvp;

  tconv_trace(tconvp, "%s - return %p", funcs, contextp);
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
  size_t                            rcl;
  
  if (contextp == NULL) {
    errno = EFAULT;
    goto err;
  }

  tconv_trace(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, contextp->iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  rcl = iconv((iconv_t) contextp->iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

  tconv_trace(tconvp, "%s - return %lld", funcs, (signed long long) rcl);
  return rcl;

 err:
  tconv_trace(tconvp, "%s - return (size_t)-1", funcs);
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

  tconv_trace(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvp);
  i = iconv_close((iconv_t) contextp);

  tconv_trace(tconvp, "%s - free(%p)", funcs, contextp);
  free(contextp);

  tconv_trace(tconvp, "%s - return %d", funcs, i);
  return i;

 err:
  tconv_trace(tconvp, "%s - return -1", funcs);
  return -1;
}
