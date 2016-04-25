#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include "tconv/convert/iconv.h"
/* Because this is a built-in, it can take advantage of TCONV_TRACE macro */
#include "tconv_config.h"

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

  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) sizeof(tconv_convert_iconv_context_t));

  if ((tocodes == NULL) || (fromcodes == NULL)) {
    TCONV_TRACE(tconvp, "%s - argument check failure, tcodes=%p fromcodes=%p", funcs, strerror(errno), tocodes, fromcodes);
    errno = EINVAL;
    goto err;
  }
  
  contextp = malloc(sizeof(tconv_convert_iconv_context_t));
  if (contextp == NULL) {
    TCONV_TRACE(tconvp, "%s - malloc failure, %s", funcs, strerror(errno));
    goto err;
  } else {
    TCONV_TRACE(tconvp, "%s - malloc success: %p", funcs, contextp);
  }

  contextp->iconvp  = NULL;

  TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, tocodes, fromcodes);
  iconvp = iconv_open(tocodes, fromcodes);
  if (iconvp == (iconv_t) -1) {
    TCONV_TRACE(tconvp, "%s - iconv_open failure, %s", funcs, strerror(errno));
    goto err;
  } else {
    TCONV_TRACE(tconvp, "%s - iconv_open success: %p", funcs, iconvp);
  }

  contextp->iconvp = iconvp;

  TCONV_TRACE(tconvp, "%s - return %p", funcs, contextp);
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
  static const char                 funcs[] = "tconv_convert_iconv_run";
  tconv_convert_iconv_context_t    *contextp = (tconv_convert_iconv_context_t *) voidp;
  size_t                            rcl;
  
  if (contextp == NULL) {
    errno = EFAULT;
    goto err;
  }

  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, contextp->iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  rcl = iconv((iconv_t) contextp->iconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

  TCONV_TRACE(tconvp, "%s - return %lld", funcs, (signed long long) rcl);
  return rcl;

 err:
  TCONV_TRACE(tconvp, "%s - return (size_t)-1", funcs);
  return (size_t)-1;
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char                 funcs[] = "tconv_convert_iconv_free";
  tconv_convert_iconv_context_t    *contextp = (tconv_convert_iconv_context_t *) voidp;
  int                               i;

  if (contextp == NULL) {
    errno = EFAULT;
    goto err;
  }

  TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvp);
  i = iconv_close((iconv_t) contextp->iconvp);

  TCONV_TRACE(tconvp, "%s - free(%p)", funcs, contextp);
  free(contextp);

  TCONV_TRACE(tconvp, "%s - return %d", funcs, i);
  return i;

 err:
  TCONV_TRACE(tconvp, "%s - return -1", funcs);
  return -1;
}
