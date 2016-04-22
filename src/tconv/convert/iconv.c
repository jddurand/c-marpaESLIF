#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include "tconv/convert/iconv.h"
#include "tconv/trace.h"

typedef struct tconv_convert_iconv_context {
  genericLogger_t *genericLoggerp;
  iconv_t          iconvp;
} tconv_convert_iconv_context_t;

/*****************************************************************************/
void  *tconv_convert_iconv_new(genericLogger_t *genericLoggerp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char              funcs[] = "tconv_convert_iconv_new";
  iconv_t                        iconvp;
  tconv_convert_iconv_context_t *contextp;

 /* Get logger asap */
  contextp = malloc(sizeof(tconv_convert_iconv_context_t));
  if (contextp == NULL) {
    goto err;
  }
  contextp->genericLoggerp = genericLoggerp;
  contextp->iconvp         = NULL;

  TCONV_TRACEF(contextp, "%s - iconv_open", funcs);

  iconvp = iconv_open(tocodes, fromcodes);
  if (iconvp == NULL) {
    TCONV_ERRORF(contextp, "%s - iconv_open - %s", funcs, strerror(errno));
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
size_t tconv_convert_iconv_run(void *voidp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char                 funcs[] = "t tconv_convert_iconv_run";
  tconv_convert_iconv_context_t    *contextp = (tconv_convert_iconv_context_t *) voidp;
  size_t                            n;
  
  if (contextp == NULL) {
    errno = EFAULT;
    goto err;
  }

  TCONV_TRACEF(contextp, "%s - iconv", funcs);
  n = iconv((iconv_t) contextp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

  return n;

 err:
  return (size_t)-1;
}

/*****************************************************************************/
int tconv_convert_iconv_free(void *voidp)
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
