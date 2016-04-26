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
#define TCONV_ENV_CONVERT_ICU_UCHARSIZE "TCONV_ENV_CONVERT_ICU_UCHARSIZE"
#define TCONV_ENV_CONVERT_ICU_FALLBACK  "TCONV_ENV_CONVERT_ICU_FALLBACK"

tconv_convert_ICU_option_t tconv_convert_icu_option_default = {
  1024*1024, /* uCharSizel */
  0          /* fallbackb */
};

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
  static const char            funcs[]          = "tconv_convert_ICU_new";
  tconv_convert_ICU_option_t  *optionp          = (tconv_convert_ICU_option_t *) voidp;
  UBool                        ignoreb          = FALSE;
  UBool                        translitb        = FALSE;
  char                        *realToCodes      = NULL;
  tconv_convert_icu_context_t *contextp         = NULL;
  char                        *ignorep          = NULL;
  char                        *endIgnorep       = NULL;
  char                        *translitp        = NULL;
  char                        *endTranslitp     = NULL;
  UConverter                  *uConverterFromp  = NULL;
  UChar                       *ucharBufp        = NULL;
  UTransliterator             *uTransliteratorp = NULL;
  UConverter                  *uConverterTop    = NULL;
  UConverterFromUCallback      fromUCallbackp   = NULL;
  const void                  *fromuContextp    = NULL;
  UConverterToUCallback        toUCallbackp     = NULL;
  const void                  *toUContextp      = NULL;
  UBool                        fallbackb        = FALSE;
  UErrorCode                   uErrorCode;
  char                        *p, *q;

  if ((tocodes == NULL) || (fromcodes == NULL)) {
    errno = EINVAL;
    goto err;
  }

  /* ----------------------------------------------------------- */
  /* Duplicate tocodes and manage //IGNORE and //TRANSLIT option */
  /* ----------------------------------------------------------- */
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
    endTranslitp = translitp + strlen(translitp) - 1;
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
      *q++ = *p;
    }
  }
  TCONV_TRACE(tconvp, "%s - realToCodes is now \"%s\"", funcs, realToCodes);

  /* ----------------------------------------------------------- */
  /* Get options                                                 */
  /* ----------------------------------------------------------- */
  if (optionp == NULL) {
    optionp = &tconv_convert_icu_option_default;
    /* This can be overwriten with environment variables */
    TCONV_TRACE(tconvp, "%s - getenv(\"%s\")", funcs, TCONV_ENV_CONVERT_ICU_UCHARSIZE);
    p = getenv(TCONV_ENV_CONVERT_ICU_UCHARSIZE);
    if (p != NULL) {
      TCONV_TRACE(tconvp, "%s - atoi(\"%s\")", funcs, p);
      optionp->uCharSizel = atoi(p);
      if (optionp->uCharSizel <= 0) {
	errno = EINVAL;
	goto err;
      }
    }
    TCONV_TRACE(tconvp, "%s - getenv(\"%s\")", funcs, TCONV_ENV_CONVERT_ICU_FALLBACK);
    p = getenv(TCONV_ENV_CONVERT_ICU_FALLBACK);
    if (p != NULL) {
      TCONV_TRACE(tconvp, "%s - atoi(\"%s\")", funcs, p);
      optionp->fallbackb = (atoi(p) != 0) ? 1 : 0;
    }
  }

  /* ----------------------------------------------------------- */
  /* Setup the from converter                                    */
  /* ----------------------------------------------------------- */
  fromUCallbackp = (ignoreb == TRUE) ? UCNV_FROM_U_CALLBACK_SKIP : UCNV_FROM_U_CALLBACK_STOP;
  fromuContextp  = NULL;

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_open(%p, %p)", funcs, fromcodes, &uErrorCode);
  uConverterFromp = ucnv_open(fromcodes, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucnv_open returned %p", funcs, uConverterFromp);

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_setFromUCallBack(%p, %p, %p, %p, %p)", funcs, uConverterFromp, fromUCallbackp, fromuContextp, NULL, NULL, &uErrorCode);
  ucnv_setFromUCallBack(uConverterFromp, fromUCallbackp, fromuContextp, NULL, NULL, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    goto err;
  }

  /* ----------------------------------------------------------- */
  /* Setup the proxy unicode buffer                              */
  /* ----------------------------------------------------------- */
  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) (optionp->uCharSizel * sizeof(UChar)));
  ucharBufp = (UChar *) malloc(optionp->uCharSizel * sizeof(UChar));
  if (ucharBufp == NULL) {
    goto err;
  }
  
  /* ----------------------------------------------------------- */
  /* Setup the to converter (there is the fallback in it)        */
  /* ----------------------------------------------------------- */
  toUCallbackp   = (ignoreb == TRUE) ? UCNV_TO_U_CALLBACK_SKIP   : UCNV_TO_U_CALLBACK_STOP;
  toUContextp    = NULL;
  fallbackb      = (optionp->fallbackb != 0) ? TRUE : FALSE;

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_open(%p, %p)", funcs, tocodes, &uErrorCode);
  uConverterTop = ucnv_open(tocodes, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucnv_open returned %p", funcs, uConverterTop);

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_setToUCallBack(%p, %p, %p, %p, %p)", funcs, uConverterTop, toUCallbackp, toUContextp, NULL, NULL, &uErrorCode);
  ucnv_setToUCallBack(uConverterTop, toUCallbackp, toUContextp, NULL, NULL, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    goto err;
  }

  TCONV_TRACE(tconvp, "%s - ucnv_setFallback(%p, %d)", funcs, uConverterTop, (int) fallbackb);
  ucnv_setFallback(uConverterTop, fallbackb);

  /* Create context */
  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) sizeof(tconv_convert_icu_context_t));
  contextp = (tconv_convert_icu_context_t *) malloc(sizeof(tconv_convert_icu_context_t));
  if (contextp == NULL) {
    goto err;
  }

  contextp->uConverterFromp             = NULL;
  contextp->ucharBufp                   = NULL;
  contextp->uTransliteratorp            = NULL;
  contextp->uConverterTop               = NULL;

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
    if (uConverterFromp != NULL) {
      ucnv_close (uConverterFromp);
    }
    if (uConverterTop != NULL) {
      ucnv_close (uConverterTop);
    }
    if (contextp != NULL) {
      free(contextp);
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
