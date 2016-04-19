#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unicode/ucsdet.h>
#include "charset/ICU.h"

typedef struct tconv_charset_ICU_context {
  int               confidencei;
  UCharsetDetector *uCharsetDetectorp;
} tconv_charset_ICU_context_t;

/*****************************************************************************/
void *tconv_charset_ICU_new(void *voidp)
/*****************************************************************************/
{
  tconv_charset_ICU_t         *optionp  = (tconv_charset_ICU_t *) voidp;
  tconv_charset_ICU_context_t *contextp = NULL;
  UCharsetDetector            *uCharsetDetectorp = NULL;
  UErrorCode                   uErrorCode;

  if (optionp == NULL) {
    errno = EINVAL;
    goto err;
  }

  uErrorCode = U_ZERO_ERROR;
  uCharsetDetectorp = ucsdet_open(&uErrorCode);
  if ((uCharsetDetectorp == NULL) || U_FAILURE(uErrorCode)) {
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  contextp = malloc(sizeof(tconv_charset_ICU_context_t));
  if (contextp == NULL) {
    goto err;
  }

  contextp->confidencei       = optionp->confidencei;
  contextp->uCharsetDetectorp = uCharsetDetectorp;

  return contextp;

 err:
  {
    int errnol = errno;
    if (uCharsetDetectorp != NULL) { ucsdet_close(uCharsetDetectorp); }
    if (contextp          != NULL) { free(contextp); }
    errno = errnol;
  }
  return NULL;
}

/*****************************************************************************/
char *tconv_charset_ICU_run(void *voidp, char *bytep, size_t bytel)
/*****************************************************************************/
{
  tconv_charset_ICU_context_t *contextp = (tconv_charset_ICU_context_t *) voidp;
  UCharsetDetector            *uCharsetDetectorp;
  UErrorCode                   uErrorCode;
  const UCharsetMatch         *uCharsetMatchp;
  int32_t                      confidencei; 
  const char                  *charsets;

  if ((contextp == NULL) || (bytep == NULL) || (bytel <= 0)) {
    errno = EFAULT;
    goto err;
  }

  uCharsetDetectorp = contextp->uCharsetDetectorp;
  if (uCharsetDetectorp == NULL) {
    errno = EFAULT;
    goto err;
  }

  uErrorCode = U_ZERO_ERROR;
  ucsdet_setText(uCharsetDetectorp, bytep, bytel, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  uErrorCode = U_ZERO_ERROR;
  uCharsetMatchp = ucsdet_detect(uCharsetDetectorp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  if (uCharsetMatchp == NULL) {
    errno = EFAULT;
    goto err;
  }

  uErrorCode = U_ZERO_ERROR;
  charsets = ucsdet_getName(uCharsetMatchp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  if (charsets == NULL) {
    errno = EFAULT;
    goto err;
  }

  uErrorCode = U_ZERO_ERROR;
  confidencei = ucsdet_getConfidence(uCharsetMatchp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  if (confidencei < (int32_t) contextp->confidencei) {
    errno = EAGAIN;
    return NULL;
  }

  return (char *) charsets;

 err:
  return NULL;
}

/*****************************************************************************/
void  tconv_charset_ICU_free(void *voidp)
/*****************************************************************************/
{
  tconv_charset_ICU_context_t *contextp = (tconv_charset_ICU_context_t *) voidp;
  UCharsetDetector            *uCharsetDetectorp;

  if (contextp != NULL) {
    uCharsetDetectorp = contextp->uCharsetDetectorp;
    if (uCharsetDetectorp != NULL) {
      ucsdet_close(uCharsetDetectorp);
    }
    free(contextp);
  }
}

