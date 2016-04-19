#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "charsetdetect.h"
#include "charset/cchardet.h"

typedef struct tconv_charset_cchardet_context {
  float confidencef;
  csd_t csdp;
} tconv_charset_cchardet_context_t;

void *tconv_charset_cchardet_new(void *voidp)
{
  tconv_charset_cchardet_t         *optionp  = (tconv_charset_cchardet_t *) voidp;
  tconv_charset_cchardet_context_t *contextp = NULL;
  csd_t                             csdp     = NULL;

  if (optionp == NULL) {
    errno = EINVAL;
    goto err;
  }

  csdp = csd_open();
  if (csdp == NULL) {
    goto err;
  }

  contextp = malloc(sizeof(tconv_charset_cchardet_context_t));
  if (contextp == NULL) {
    goto err;
  }

  contextp->confidencef = optionp->confidencef;
  contextp->csdp        = csdp;

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

char *tconv_charset_cchardet_run(void *voidp, char *bytep, size_t bytel)
{
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
  if (csdp == NULL) {
    errno = EFAULT;
    goto err;
  }

  csdi = csd_consider(csdp, bytep, (unsigned long) bytel);
  if (csdi < 0) {
    errno = ENOENT;
    goto err;
  } else if (csdi == 0) {
    errno = EAGAIN;
    goto err;
  }

  charsets = csd_close2(csdp, &confidencef);
  contextp->csdp = NULL;
  if (charsets == NULL) {
    errno = EFAULT;
    return NULL;
  }

  if ((strcmp(charsets, "ASCII") != 0) || (strcmp(charsets, "ibm850") != 0)) {
    if (confidencef < contextp->confidencef) {
      errno = ENOENT;
      return NULL;
  }

    return (char *) charsets;
  }

 err:
  return NULL;
}

void  tconv_charset_cchardet_free(void *voidp)
{
  tconv_charset_cchardet_context_t *contextp = (tconv_charset_cchardet_context_t *) voidp;
  csd_t                             csdp;

  if (contextp != NULL) {
    csdp = contextp->csdp;
    if (csdp != NULL) {
      csd_close(csdp);
    }
    free(contextp);
  }
}

