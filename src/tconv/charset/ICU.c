#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "unicode/ucsdet.h"
#include "tconv/charset/ICU.h"
/* Because this is a built-in, it can take advantage of TCONV_TRACE macro */
#include "tconv_config.h"

typedef struct tconv_charset_ICU_context {
  int               confidencei;
  UCharsetDetector *uCharsetDetectorp;
} tconv_charset_ICU_context_t;

/*****************************************************************************/
void *tconv_charset_ICU_new(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char            funcs[] = "tconv_charset_ICU_new";
  tconv_charset_ICU_option_t  *optionp  = (tconv_charset_ICU_option_t *) voidp;
  tconv_charset_ICU_context_t *contextp;
  UCharsetDetector            *uCharsetDetectorp;
  UErrorCode                   uErrorCode;
  const char                  *errors;

  TCONV_TRACE(tconvp, "%s(%p, %p)", funcs, tconvp, voidp);

  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) sizeof(tconv_charset_ICU_context_t));
  contextp = malloc(sizeof(tconv_charset_ICU_context_t));
  if (contextp == NULL) {
    TCONV_TRACE(tconvp, "%s - malloc(%lld) failure, %s", funcs, (unsigned long long) sizeof(tconv_charset_ICU_context_t), strerror(errno));
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - malloc(%lld) success: %p", funcs, (unsigned long long) sizeof(tconv_charset_ICU_context_t), contextp);
  contextp->confidencei       = 0;
  contextp->uCharsetDetectorp = NULL;

  if (optionp == NULL) {
    TCONV_TRACE(tconvp, "%s - option is NULL", funcs);
    errno = EINVAL;
    goto err;
  }

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucsdet_open(%p)", funcs, &uErrorCode);
  uCharsetDetectorp = ucsdet_open(&uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_TRACE(tconvp, "%s - ucsdet_open(%p) failure, %s", funcs, &uErrorCode, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucsdet_open(%p) success: %p", funcs, &uErrorCode, uCharsetDetectorp);

  TCONV_TRACE(tconvp, "%s - confidence level set to %d", funcs, optionp->confidencei);
  contextp->confidencei       = optionp->confidencei;
  contextp->uCharsetDetectorp = uCharsetDetectorp;

  TCONV_TRACE(tconvp, "%s - return %p", funcs, contextp);
  return contextp;

 err:
  {
    int errnol = errno;
    if (uCharsetDetectorp != NULL) {
      TCONV_TRACE(tconvp, "%s - ucsdet_close(%p)", funcs, uCharsetDetectorp);
      ucsdet_close(uCharsetDetectorp);
    }
    if (contextp != NULL) {
      TCONV_TRACE(tconvp, "%s - free(%p)", funcs, contextp);
      free(contextp);
    }
    if (U_FAILURE(uErrorCode)) {
      errors = u_errorName(uErrorCode);
      TCONV_TRACE(tconvp, "%s - setting last error to \"%s\"", funcs, errors);
      tconv_error_set(tconvp, errors);
    }
    TCONV_TRACE(tconvp, "%s - setting errno to %d (%s)", funcs, (int) errnol, strerror((int) errnol));
    errno = errnol;
  }
  TCONV_TRACE(tconvp, "%s - return NULL", funcs);
  return NULL;
}

/*****************************************************************************/
char *tconv_charset_ICU_run(tconv_t tconvp, void *voidp, char *bytep, size_t bytel)
/*****************************************************************************/
{
  static const char            funcs[] = "tconv_charset_ICU_run";
  tconv_charset_ICU_context_t *contextp = (tconv_charset_ICU_context_t *) voidp;
  UCharsetDetector            *uCharsetDetectorp;
  UErrorCode                   uErrorCode;
  const UCharsetMatch         *uCharsetMatchp;
  int32_t                      confidencei; 
  const char                  *charsets;
  const char                  *errors;

  TCONV_TRACE(tconvp, "%s(%p, %p, %p, %lld)", funcs, tconvp, voidp, bytep, (unsigned long long) bytel);

  if ((contextp == NULL) || (bytep == NULL) || (bytel <= 0)) {
    errno = EFAULT;
    goto err;
  }

  uCharsetDetectorp = contextp->uCharsetDetectorp;

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucsdet_setText(%p, %p, %lld, %p)", funcs, uCharsetDetectorp, bytep, (unsigned long long) bytel, &uErrorCode);
  ucsdet_setText(uCharsetDetectorp, bytep, bytel, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_TRACE(tconvp, "%s - ucsdet_setText failure, %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucsdet_setText success", funcs);

  TCONV_TRACE(tconvp, "%s - ucsdet_detect(%p, %p)", funcs, uCharsetDetectorp, &uErrorCode);
  uErrorCode = U_ZERO_ERROR;
  uCharsetMatchp = ucsdet_detect(uCharsetDetectorp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_TRACE(tconvp, "%s - ucsdet_detect failure, %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucsdet_detect success - returned %p", funcs, uCharsetMatchp);

  TCONV_TRACE(tconvp, "%s - ucsdet_getName(%p, %p)", funcs, uCharsetMatchp, &uErrorCode);
  uErrorCode = U_ZERO_ERROR;
  charsets = ucsdet_getName(uCharsetMatchp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_TRACE(tconvp, "%s - ucsdet_getName failure, %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucsdet_getName success - returned \"%s\"", funcs, charsets);

  TCONV_TRACE(tconvp, "%s - ucsdet_getConfidence(%p, %p)", funcs, uCharsetMatchp, &uErrorCode);
  uErrorCode = U_ZERO_ERROR;
  confidencei = ucsdet_getConfidence(uCharsetMatchp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_TRACE(tconvp, "%s - ucsdet_getConfidence - %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucsdet_getConfidence success - returned %d", funcs, confidencei);

  TCONV_TRACE(tconvp, "%s - confidenci = %d < %d ?", funcs, confidencei, (int32_t) contextp->confidencei);
  if (confidencei < (int32_t) contextp->confidencei) {
    TCONV_TRACE(tconvp, "%s - too low confidence", funcs);
    errno = EAGAIN;
    return NULL;
  }

  TCONV_TRACE(tconvp, "%s - return %s", funcs, charsets);
  return (char *) charsets;

 err:
  if (U_FAILURE(uErrorCode)) {
    errors = u_errorName(uErrorCode);
    TCONV_TRACE(tconvp, "%s - setting last error to \"%s\"", funcs, errors);
    tconv_error_set(tconvp, errors);
  }
  return NULL;
}

/*****************************************************************************/
void  tconv_charset_ICU_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char            funcs[] = "tconv_charset_ICU_free";
  tconv_charset_ICU_context_t *contextp = (tconv_charset_ICU_context_t *) voidp;
  UCharsetDetector            *uCharsetDetectorp;

  TCONV_TRACE(tconvp, "%s(%p, %p)", funcs, tconvp, voidp);
  
  if (contextp != NULL) {
    uCharsetDetectorp = contextp->uCharsetDetectorp;
    if (uCharsetDetectorp != NULL) {
      TCONV_TRACE(tconvp, "%s - ucsdet_close(%p)", funcs, uCharsetDetectorp);
      ucsdet_close(uCharsetDetectorp);
    }
    free(contextp);
  }
}

