#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "unicode/ucsdet.h"
#include "genericLogger.h"
#include "tconv/charset/ICU.h"
#include "tconv/trace.h"

typedef struct tconv_charset_ICU_context {
  genericLogger_t  *genericLoggerp;
  int               confidencei;
  UCharsetDetector *uCharsetDetectorp;
} tconv_charset_ICU_context_t;

/*****************************************************************************/
void *tconv_charset_ICU_new(genericLogger_t *genericLoggerp, void *voidp)
/*****************************************************************************/
{
  static const char            funcs[] = "tconv_charset_ICU_new";
  tconv_charset_ICU_option_t  *optionp  = (tconv_charset_ICU_option_t *) voidp;
  tconv_charset_ICU_context_t *contextp;
  UCharsetDetector            *uCharsetDetectorp;
  UErrorCode                   uErrorCode;

  /* Get logger asap */
  contextp = malloc(sizeof(tconv_charset_ICU_context_t));
  if (contextp == NULL) {
    goto err;
  }
  contextp->genericLoggerp    = genericLoggerp;
  contextp->confidencei       = 0;
  contextp->uCharsetDetectorp = NULL;

  if (optionp == NULL) {
    errno = EINVAL;
    goto err;
  }

  TCONV_TRACEF(contextp, "%s - ucsdet_open", funcs);
  uErrorCode = U_ZERO_ERROR;
  uCharsetDetectorp = ucsdet_open(&uErrorCode);
  if (U_FAILURE(uErrorCode)) {
      TCONV_ERRORF(contextp, "%s - ucsdet_open - %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
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
  static const char            funcs[] = "tconv_charset_ICU_run";
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

  TCONV_TRACEF(contextp, "%s - ucsdet_setText", funcs);
  uErrorCode = U_ZERO_ERROR;
  ucsdet_setText(uCharsetDetectorp, bytep, bytel, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_ERRORF(contextp, "%s - ucsdet_setText - %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  TCONV_TRACEF(contextp, "%s - ucsdet_detect", funcs);
  uErrorCode = U_ZERO_ERROR;
  uCharsetMatchp = ucsdet_detect(uCharsetDetectorp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_ERRORF(contextp, "%s - ucsdet_detect - %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  TCONV_TRACEF(contextp, "%s - ucsdet_getName", funcs);
  uErrorCode = U_ZERO_ERROR;
  charsets = ucsdet_getName(uCharsetMatchp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_ERRORF(contextp, "%s - ucsdet_getName - %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  TCONV_TRACEF(contextp, "%s - ucsdet_getConfidence", funcs);
  uErrorCode = U_ZERO_ERROR;
  confidencei = ucsdet_getConfidence(uCharsetMatchp, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    TCONV_ERRORF(contextp, "%s - ucsdet_getConfidence - %s", funcs, u_errorName(uErrorCode));
    /* errno ? */
    errno = ENOSYS;
    goto err;
  }

  TCONV_TRACEF(contextp, "%s - confidenci = %d < %d ?", funcs, confidencei, (int32_t) contextp->confidencei);
  if (confidencei < (int32_t) contextp->confidencei) {
    TCONV_TRACEF(contextp, "%s - too low confidence", funcs);
    errno = EAGAIN;
    return NULL;
  }

  TCONV_TRACEF(contextp, "%s - return %s", funcs, charsets);
  return (char *) charsets;

 err:
  return NULL;
}

/*****************************************************************************/
void  tconv_charset_ICU_free(void *voidp)
/*****************************************************************************/
{
  static const char            funcs[] = "tconv_charset_ICU_free";
  tconv_charset_ICU_context_t *contextp = (tconv_charset_ICU_context_t *) voidp;
  UCharsetDetector            *uCharsetDetectorp;

  if (contextp != NULL) {
    uCharsetDetectorp = contextp->uCharsetDetectorp;
    if (uCharsetDetectorp != NULL) {
      TCONV_TRACEF(contextp, "%s - ucsdet_close", funcs);
      ucsdet_close(uCharsetDetectorp);
    }
    free(contextp);
  }
}

