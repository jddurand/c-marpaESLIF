#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "charsetdetect.h"
#include "tconv/charset/cchardet.h"

typedef struct tconv_charset_cchardet_context {
  float            confidencef;
  csd_t            csdp;
} tconv_charset_cchardet_context_t;

/*****************************************************************************/
void *tconv_charset_cchardet_new(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char            funcs[] = "tconv_charset_cchardet_new";
  tconv_charset_cchardet_option_t  *optionp  = (tconv_charset_cchardet_option_t *) voidp;
  tconv_charset_cchardet_context_t *contextp;
  csd_t                             csdp;

  contextp = malloc(sizeof(tconv_charset_cchardet_context_t));
  if (contextp == NULL) {
    goto err;
  }
  contextp->confidencef    = 0.0f;
  contextp->csdp           = NULL;

  if (optionp == NULL) {
    errno = EINVAL;
    goto err;
  }

  tconv_trace(tconvp, "%s - csd_open", funcs);
  csdp = csd_open();
  if (csdp == NULL) {
    tconv_trace(tconvp, "%s - csd_open - %s", funcs, strerror(errno));
    goto err;
  }

  contextp->confidencef    = optionp->confidencef;
  contextp->csdp           = csdp;

  return contextp;

 err:
  {
    int errnol = errno;
    if (csdp != NULL) { csd_close(csdp); }
    if (contextp != NULL) { free(contextp); }
    errno = errnol;
  }
  return NULL;
}

/*****************************************************************************/
char *tconv_charset_cchardet_run(tconv_t tconvp, void *voidp, char *bytep, size_t bytel)
/*****************************************************************************/
{
  static const char                 funcs[] = "tconv_charset_cchardet_run";
  tconv_charset_cchardet_context_t *contextp = (tconv_charset_cchardet_context_t *) voidp;
  int                               csdi;
  csd_t                             csdp;
  float                             confidencef;
  const char                       *charsets;

  if ((contextp == NULL) || (bytep == NULL) || (bytel <= 0)) {
    errno = EFAULT;
    goto err;
  }

  csdp = contextp->csdp;

  tconv_trace(tconvp, "%s - csd_consider", funcs);
  csdi = csd_consider(csdp, bytep, (unsigned long) bytel);
  if (csdi < 0) {
    tconv_trace(tconvp, "%s - csd_consider return value is < 0", funcs);
    errno = ENOENT;
    goto err;
  } else if (csdi == 0) {
    tconv_trace(tconvp, "%s - csd_consider return value is == 0", funcs);
    errno = EAGAIN;
    goto err;
  }

  tconv_trace(tconvp, "%s - csd_close2", funcs);
  charsets = csd_close2(csdp, &confidencef);
  contextp->csdp = NULL;
  if (charsets == NULL) {
    tconv_trace(tconvp, "%s - csd_close2 return value is NULL", funcs);
    errno = EFAULT;
    return NULL;
  }

  if ((strcmp(charsets, "ASCII") != 0) && (strcmp(charsets, "ibm850") != 0)) {
    tconv_trace(tconvp, "%s - confidencef %f < %f ?", funcs, confidencef, contextp->confidencef);
    if (confidencef < contextp->confidencef) {
      tconv_trace(tconvp, "%s - too low confidence", funcs);
      errno = ENOENT;
      return NULL;
    }
  } else {
    tconv_trace(tconvp, "%s - csd_close2 returns %s, known to not set confidence", funcs, charsets);
  }

  tconv_trace(tconvp, "%s - return %s", funcs, charsets);
  return (char *) charsets;

 err:
  return NULL;
}

/*****************************************************************************/
void  tconv_charset_cchardet_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char                 funcs[] = "tconv_charset_cchardet_free";
  tconv_charset_cchardet_context_t *contextp = (tconv_charset_cchardet_context_t *) voidp;
  csd_t                             csdp;

  if (contextp != NULL) {
    csdp = contextp->csdp;
    if (csdp != NULL) {
      tconv_trace(tconvp, "%s - csd_close", funcs);
      csd_close(csdp);
    }
    free(contextp);
  }
}

