#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unicode/ucsdet.h>
#include <unicode/uclean.h>
#include <unicode/ucnv.h>
#include <unicode/utext.h>
#include <unicode/ustring.h>
#include <unicode/utrans.h>

#include "tconv/convert/ICU.h"
/* Because this is a built-in, it can take advantage of TCONV_TRACE macro */
#include "tconv_config.h"

#define TCONV_ICU_IGNORE   "//IGNORE"
#define TCONV_ICU_TRANSLIT "//TRANSLIT"

/* Default option */
tconv_convert_icu_option_t tconv_convert_icu_option_default = { 1024*1024 };

/* Context */
typedef struct tconv_convert_icu_context {
  UConverter                 *uConverterFromp;  /* Input => UChar  */
  UChar                      *ucharBufp;        /* UChar buffer    */
  UTransliterator            *uTransliteratorp; /* Transliteration */
  UConverter                 *uConverterTop;    /* UChar => Output */
} tconv_convert_icu_context_t;

/*****************************************************************************/
void  *tconv_convert_ICU_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char            funcs[]        = "tconv_convert_ICU_new";
  tconv_convert_icu_option_t  *optionp        = (tconv_convert_icu_option_t *) voidp;
  UBool                        ignoreb        = FALSE;
  UBool                        translitb      = FALSE;
  char                        *realToCodes;
  char                        *ignorep, *endIgnorep = NULL;
  char                        *translitp, *endTranslitp = NULL;
  UConverterFromUCallback      fromUCallbackp;
  const void                  *fromuContextp;
  UConverterToUCallback        toUCallbackp;
  const void                  *toUContextp;
  UBool                        fallbackb;
  tconv_convert_icu_context_t *contextp;
  UErrorCode                   uErrorCode;
  char                        *p, *q;

  if ((tocodes == NULL) || (fromcodes == NULL)) {
    errno = EINVAL;
    goto err;
  }

  /* Detect //IGNORE and //TRANSLIT option */
  TCONV_TRACE(tconvp, "%s - strdup(%p)", funcs, tocodes);
  realToCodes = strdup(tocodes);
  if (realToCodes == NULL) {
    goto err;
  }
  ignorep   = strstr(realToCodes, TCONV_ICU_IGNORE);
  translitp = strstr(realToCodes, TCONV_ICU_TRANSLIT);
  /* They must end the string or be followed by another (maybe) option */
  if ((ignorep != NULL) && ((*(ignorep+1) == '\0') || (*(ignorep+1) == '/'))) {
    ignoreb = TRUE;
    endIgnorep = ignorep + strlen(ignorep) - 1;
  }
  if ((translitp != NULL) && ((*(translitp+1) == '\0') || (*(translitp+1) == '/'))) {
    translitb = TRUE;
    endTranslitp = translitb + strlen(translitb) - 1;
  }
  /* ... Remove options from realToCodes  */
  for (p = q = realToCodes; *p != '\0'; ++p) {    /* Note that a valid charset cannot contain \0 */
    if ((ignoreb == TRUE) && ((p >= ignorep) || (p <= endIgnorep))) {
      continue;
    }
    if ((translitb == TRUE) && ((p >= translitp) || (p <= endTranslitp))) {
      continue;
    }
    if (p != q) {
      *q++ = p;
    }
  }

  /* Get converter options */
  if (optionp == NULL) {
    optionp = &tconv_convert_icu_option_default;
  }

  /* Create context */
  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) sizeof(tconv_convert_icu_context_t));
  contextp = (tconv_convert_icu_context_t *) malloc(sizeof(tconv_convert_icu_context_t));
  if (contextp == NULL) {
    goto err;
  }

  contextp->uConverterFromp             = NULL;
  contextp->uConverterFromUCallbackp    = NULL;
  contextp->uConverterFromUCallbackCtxp = NULL;
  contextp->ucharBufp                   = NULL;
  contextp->uTransliteratorp            = NULL;
  contextp->uConverterTop               = NULL;
  contextp->uConverterToUCallbackp      = NULL;
  contextp->uConverterToUCallbackCtxp   = NULL;

  /* Callbacks, contexts and fallback */
  fromUCallbackp = (ignoreb == TRUE) ? UCNV_TO_U_CALLBACK_SKIP : UCNV_TO_U_CALLBACK_STOP;
  fromuContextp  = NULL;
  toUCallbackp   = (ignoreb == TRUE) ? UCNV_TO_U_CALLBACK_SKIP : UCNV_TO_U_CALLBACK_STOP;
  toUContextp    = NULL;
  fallbackb      = translitb;

  /* From converter: it always exist */
  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_open(%p, %p)", funcs, fromcodes, &uErrorCode);
  contextp->uConverterFromp = ucnv_open(fromcodes, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucnv_open returned %p", funcs, contextp->uConverterFromp);

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_setFromUCallBack(%p, %p, %p, %p, %p)", funcs, contextp->uConverterFromp, fromUCallbackp, fromuContextp, NULL, NULL, &uErrorCode);
  ucnv_setFromUCallBack(contextp->uConverterFromp, fromUCallbackp, fromuContextp, NULL, NULL, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    goto err;
  }

  TCONV_TRACE(tconvp, "%s - ucnv_setFallback(%p, %d)", funcs, contextp->uConverterFromp, (int) fallbackb);
  ucnv_setFallback(contextp->uConverterFromp, fallbackb);

  TCONV_TRACE(tconvp, "%s - icu_open(%p, %p)", funcs, tocodes, fromcodes);
  icup = icu_open(tocodes, fromcodes);

  TCONV_TRACE(tconvp, "%s - return %p", funcs, icup);
  return icup;

 err:
  {
    int errnol = errno;
    if (realToCodes != NULL) {
      free(realToCodes);
    }
    if (U_FAILURE(uErrorCode)) {
      tconv_error_set(tconvp, u_errorName(uErrorCode));
    }
    errno = errnol;
  }
  TCONV_TRACE(tconvp, "%s - return -1", funcs, icup);
  return (void *)-1;
}

/*****************************************************************************/
size_t tconv_convert_ICU_run(tconv_t tconvp, void *voidp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_ICU_run";
  icu_t           icup  = (icu_t) voidp;
  size_t            rcl;

  TCONV_TRACE(tconvp, "%s - icu(%p, %p, %p, %p, %p)", funcs, icup, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  rcl = icu(icup, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

#ifndef TCONV_NTRACE
  if (rcl == (size_t)-1) {
    TCONV_TRACE(tconvp, "%s - return -1", funcs);
  } else {
    TCONV_TRACE(tconvp, "%s - return %lld", funcs, (signed long long) rcl);
  }
#endif
  return rcl;
}

/*****************************************************************************/
int tconv_convert_ICU_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_ICU_free";
  icu_t           icup  = (icu_t) voidp;
  int               rci;

  TCONV_TRACE(tconvp, "%s - icu_close(%p)", funcs, icup);
  rci = icu_close(icup);

  TCONV_TRACE(tconvp, "%s - return %d", funcs, rci);
  return rci;
}
