#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <unicode/uconfig.h>
#include <unicode/ucnv.h>
#if !UCONFIG_NO_TRANSLITERATION
#include <unicode/utrans.h>
#endif
#include <unicode/uset.h>
#include <unicode/ustring.h>

#include "tconv/convert/ICU.h"
/* Because this is a built-in, it can take advantage of TCONV_TRACE macro */
#include "tconv_config.h"

#define TCONV_ICU_IGNORE   "//IGNORE"
#define TCONV_ICU_TRANSLIT "//TRANSLIT"

/* Default option */
#define TCONV_ENV_CONVERT_ICU_UCHARCAPACITY "TCONV_ENV_CONVERT_ICU_UCHARCAPACITY"
#define TCONV_ENV_CONVERT_ICU_FALLBACK  "TCONV_ENV_CONVERT_ICU_FALLBACK"

tconv_convert_ICU_option_t tconv_convert_icu_option_default = {
  4096, /* uCharCapacityl */
  0     /* fallbackb */
};

/* Context */
typedef struct tconv_convert_ICU_context {
  UConverter                 *uConverterFromp;  /* Input => UChar  */
  UChar                      *uCharBufp;        /* UChar buffer    */
  int32_t                     uCharCapacityl;   /* Allocated Length (not bytes) */
  size_t                      uCharSizel;       /* Allocated size (bytes) */
  UConverter                 *uConverterTop;    /* UChar => Output */
#if !UCONFIG_NO_TRANSLITERATION
  UChar                      *chunkp;
  UChar                      *chunkcopyp;
  int32_t                     chunkCapacityl;   /* Used Length (not bytes) */
  int32_t                     chunkUsedl;       /* Used Length (not bytes) */
  size_t                      chunkSizel;       /* Allocated size (bytes) */
  UChar                      *outp;
  int32_t                     outCapacityl;     /* Transformed used Length (not bytes) */
  int32_t                     outUsedl;         /* Transformed used Length (not bytes) */
  size_t                      outSizel;         /* Transformed allocated size (bytes) */
  UTransliterator            *uTransliteratorp; /* Transliteration */
#endif
} tconv_convert_ICU_context_t;

static TCONV_C_INLINE int32_t getChunkLimit(const UChar *chunk, const size_t chunklen, const UChar *u, size_t ulen);

/*****************************************************************************/
void  *tconv_convert_ICU_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char            funcs[]          = "tconv_convert_ICU_new";
  tconv_convert_ICU_option_t  *optionp          = (tconv_convert_ICU_option_t *) voidp;
  UBool                        ignoreb          = FALSE;
  UBool                        translitb        = FALSE;
  char                        *realToCodes      = NULL;
  tconv_convert_ICU_context_t *contextp         = NULL;
  char                        *ignorep          = NULL;
  char                        *endIgnorep       = NULL;
  char                        *translitp        = NULL;
  char                        *endTranslitp     = NULL;
  UConverter                  *uConverterFromp  = NULL;
  UChar                       *uCharBufp        = NULL;
  int32_t                      uCharCapacityl   = 0;
  size_t                       uCharSizel       = 0;
#if !UCONFIG_NO_TRANSLITERATION
  UTransliterator             *uTransliteratorp = NULL;
#endif
  UConverter                  *uConverterTop    = NULL;
  UConverterFromUCallback      fromUCallbackp   = NULL;
  const void                  *fromuContextp    = NULL;
  UConverterToUCallback        toUCallbackp     = NULL;
  const void                  *toUContextp      = NULL;
  UBool                        fallbackb        = FALSE;
  int32_t                      uSetPatternFroml = 0;
  UChar                       *uSetPatternFroms = NULL;
  USet                        *uSetFromp        = NULL;
  int32_t                      uSetPatternTol   = 0;
  UChar                       *uSetPatternTos   = NULL;
  USet                        *uSetTop          = NULL;
  int32_t                      uSetPatternl     = 0;
  UChar                       *uSetPatterns     = NULL;
  USet                        *uSetp            = NULL;
  UErrorCode                   uErrorCode;
  char                        *p, *q;
  UConverterUnicodeSet         whichSet;
#define universalTransliteratorsLength 22
  U_STRING_DECL(universalTransliterators, "Any-Latin; Latin-ASCII", universalTransliteratorsLength);

  U_STRING_INIT(universalTransliterators, "Any-Latin; Latin-ASCII", universalTransliteratorsLength);

  if ((tocodes == NULL) || (fromcodes == NULL)) {
    errno = EINVAL;
    goto err;
  }

  /* ----------------------------------------------------------- */
  /* Duplicate tocodes and manage //IGNORE and //TRANSLIT option */
  /* ----------------------------------------------------------- */
  TCONV_TRACE(tconvp, "%s - strdup(\"%s\")", funcs, tocodes);
  realToCodes = strdup(tocodes);
  if (realToCodes == NULL) {
    goto err;
  }
  ignorep   = strstr(realToCodes, TCONV_ICU_IGNORE);
  translitp = strstr(realToCodes, TCONV_ICU_TRANSLIT);
  /* They must end the string or be followed by another (maybe) option */
  if (ignorep != NULL) {
    endIgnorep = ignorep + strlen(TCONV_ICU_IGNORE);
    if ((*endIgnorep == '\0') || (*(endIgnorep + 1) == '/')) {
      ignoreb = TRUE;
    }
  }
  if (translitp != NULL) {
    endTranslitp = translitp + strlen(TCONV_ICU_TRANSLIT);
    if ((*endTranslitp == '\0') || (*(endTranslitp + 1) == '/')) {
      translitb = TRUE;
    }
  }
  /* ... Remove options from realToCodes  */
  for (p = q = realToCodes; *p != '\0'; ++p) {    /* Note that a valid charset cannot contain \0 */
    if ((ignoreb == TRUE) && ((p >= ignorep) && (p < endIgnorep))) {
      continue;
    }
    if ((translitb == TRUE) && ((p >= translitp) && (p < endTranslitp))) {
      continue;
    }
    if (p != q) {
      *q++ = *p;
    } else {
      q++;
    }
  }
  *q = '\0';
  TCONV_TRACE(tconvp, "%s - realToCodes is now \"%s\"", funcs, realToCodes);

  /* ----------------------------------------------------------- */
  /* Get options                                                 */
  /* ----------------------------------------------------------- */
  if (optionp == NULL) {
    optionp = &tconv_convert_icu_option_default;
    /* This can be overwriten with environment variables */
    TCONV_TRACE(tconvp, "%s - getenv(\"%s\")", funcs, TCONV_ENV_CONVERT_ICU_UCHARCAPACITY);
    p = getenv(TCONV_ENV_CONVERT_ICU_UCHARCAPACITY);
    if (p != NULL) {
      TCONV_TRACE(tconvp, "%s - atoi(\"%s\")", funcs, p);
      uCharCapacityl = atoi(p);
      if (uCharCapacityl <= 0) {
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
  fallbackb = (optionp->fallbackb != 0) ? TRUE : FALSE;

  /* ----------------------------------------------------------- */
  /* Setup the from converter                                    */
  /* ----------------------------------------------------------- */
  fromUCallbackp = (ignoreb == TRUE) ? UCNV_FROM_U_CALLBACK_SKIP : UCNV_FROM_U_CALLBACK_STOP;
  fromuContextp  = NULL;

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_open(\"%s\", %p)", funcs, fromcodes, &uErrorCode);
  uConverterFromp = ucnv_open(fromcodes, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucnv_open returned %p", funcs, uConverterFromp);

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_setFromUCallBack(%p, %p, %p, %p, %p)", funcs, uConverterFromp, fromUCallbackp, fromuContextp, NULL, NULL, &uErrorCode);
  ucnv_setFromUCallBack(uConverterFromp, fromUCallbackp, fromuContextp, NULL, NULL, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    errno = ENOSYS;
    goto err;
  }

  TCONV_TRACE(tconvp, "%s - ucnv_setFallback(%p, %d)", funcs, uConverterFromp, (int) fallbackb);
  ucnv_setFallback(uConverterFromp, fallbackb);

  /* ----------------------------------------------------------- */
  /* Setup the proxy unicode buffer                              */
  /* ----------------------------------------------------------- */
  uCharSizel = uCharCapacityl * sizeof(UChar);
  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) uCharSizel);
  uCharBufp = (UChar *) malloc(uCharSizel);
  if (uCharBufp == NULL) {
    goto err;
  }
  
  /* ----------------------------------------------------------- */
  /* Setup the to converter                                      */
  /* ----------------------------------------------------------- */
  toUCallbackp   = (ignoreb == TRUE) ? UCNV_TO_U_CALLBACK_SKIP   : UCNV_TO_U_CALLBACK_STOP;
  toUContextp    = NULL;

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_open(\"%s\", %p)", funcs, realToCodes, &uErrorCode);
  uConverterTop = ucnv_open(realToCodes, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    errno = ENOSYS;
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - ucnv_open returned %p", funcs, uConverterTop);

  uErrorCode = U_ZERO_ERROR;
  TCONV_TRACE(tconvp, "%s - ucnv_setToUCallBack(%p, %p, %p, %p, %p)", funcs, uConverterTop, toUCallbackp, toUContextp, NULL, NULL, &uErrorCode);
  ucnv_setToUCallBack(uConverterTop, toUCallbackp, toUContextp, NULL, NULL, &uErrorCode);
  if (U_FAILURE(uErrorCode)) {
    errno = ENOSYS;
    goto err;
  }

  TCONV_TRACE(tconvp, "%s - ucnv_setFallback(%p, %d)", funcs, uConverterTop, (int) fallbackb);
  ucnv_setFallback(uConverterTop, fallbackb);

  /* ----------------------------------------------------------- */
  /* Setup the transliterator                                    */
  /* ----------------------------------------------------------- */
  if (translitb == TRUE) {
#if UCONFIG_NO_TRANSLITERATION
    TCONV_TRACE(tconvp, "%s - UCONFIG_NO_TRANSLITERATION", funcs);
    errno = ENOSYS;
    goto err;
#else
    whichSet = (fallbackb == TRUE) ? UCNV_ROUNDTRIP_AND_FALLBACK_SET : UCNV_ROUNDTRIP_SET;

    /* Transliterator is generated on-the-fly using the unicode */
    /* sets from the two converters.                            */
    
    /* --------------------------- */
    /* Uset for the from converter */
    /* --------------------------- */
    TCONV_TRACE(tconvp, "%s - getting \"from\" USet", funcs);
    TCONV_TRACE(tconvp, "%s - uset_openEmpty()", funcs);
    uSetFromp = uset_openEmpty();
    if (uSetFromp == NULL) { /* errno ? */
      errno = ENOSYS;
      goto err;
    }
    TCONV_TRACE(tconvp, "%s - uset_openEmpty returned %p", funcs, uSetFromp);

    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - ucnv_getUnicodeSet(%p, %p, %s, %p)", funcs, uConverterFromp, uSetFromp, (fallbackb == TRUE) ? "UCNV_ROUNDTRIP_AND_FALLBACK_SET" : "UCNV_ROUNDTRIP_SET", &uErrorCode);
    ucnv_getUnicodeSet(uConverterFromp, uSetFromp, whichSet, &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }

#ifndef TCONV_NTRACE
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uset_toPattern(%p, NULL, 0, TRUE, %p)", funcs, uSetFromp, &uErrorCode);
    uSetPatternFroml = uset_toPattern(uSetFromp, NULL, 0, TRUE, &uErrorCode);
    if (uErrorCode != U_BUFFER_OVERFLOW_ERROR) {
      errno = ENOSYS;
      goto err;
    }
    uSetPatternFroms = malloc((uSetPatternFroml + 1) * sizeof(UChar));
    if (uSetPatternFroms == NULL) {
      goto err;
    }
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uset_toPattern(%p, %p, %lld, TRUE, %p)", funcs, uSetFromp, uSetPatternFroms, (unsigned long long) (uSetPatternFroml + 1), &uErrorCode);
    uset_toPattern(uSetFromp, uSetPatternFroms, uSetPatternFroml, TRUE, &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }
    /* Make sure uSetPatternFroms is NULL terminated (a-la UTF-16) */
    p = (char *) (uSetPatternFroms + uSetPatternFroml);
    *p++ = '\0';
    *p   = '\0';
#endif

    /* ------------------------- */
    /* Uset for the to converter */
    /* ------------------------- */
    TCONV_TRACE(tconvp, "%s - getting \"to\" USet", funcs);
    TCONV_TRACE(tconvp, "%s - uset_openEmpty()", funcs);
    uSetTop = uset_openEmpty();
    if (uSetTop == NULL) { /* errno ? */
      errno = ENOSYS;
      goto err;
    }
    TCONV_TRACE(tconvp, "%s - uset_openEmpty returned %p", funcs, uSetTop);

    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - ucnv_getUnicodeSet(%p, %p, %s, %p)", funcs, uConverterTop, uSetTop, (fallbackb == TRUE) ? "UCNV_ROUNDTRIP_AND_FALLBACK_SET" : "UCNV_ROUNDTRIP_SET", &uErrorCode);
    ucnv_getUnicodeSet(uConverterTop, uSetTop, whichSet, &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }

#ifndef TCONV_NTRACE
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uset_toPattern(%p, NULL, 0, TRUE, %p)", funcs, uSetTop, &uErrorCode);
    uSetPatternTol = uset_toPattern(uSetTop, NULL, 0, TRUE, &uErrorCode);
    if (uErrorCode != U_BUFFER_OVERFLOW_ERROR) {
      errno = ENOSYS;
      goto err;
    }
    uSetPatternTos = malloc((uSetPatternTol + 1) * sizeof(UChar));
    if (uSetPatternTos == NULL) {
      goto err;
    }
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uset_toPattern(%p, %p, %lld, TRUE, %p)", funcs, uSetTop, uSetPatternTos, (unsigned long long) (uSetPatternTol + 1), &uErrorCode);
    uset_toPattern(uSetTop, uSetPatternTos, uSetPatternTol, TRUE, &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }
    /* Make sure uSetPatternTos is NULL terminated (a-la UTF-16) */
    p = (char *) (uSetPatternTos + uSetPatternTol);
    *p++ = '\0';
    *p   = '\0';
#endif

    /* ---------------------------------------------------------------------------- */
    /* Interset the two usets (I could have used uset_retainAll(uSetFromp, uSetTop) */
    /* ---------------------------------------------------------------------------- */
    TCONV_TRACE(tconvp, "%s - intersecting \"from\" and \"to\" USet's", funcs);
    TCONV_TRACE(tconvp, "%s - uset_openEmpty()", funcs);
    uSetp = uset_openEmpty();
    if (uSetp == NULL) { /* errno ? */
      errno = ENOSYS;
      goto err;
    }
    TCONV_TRACE(tconvp, "%s - uset_openEmpty returned %p", funcs, uSetp);

    uset_addAll(uSetp, uSetFromp);
    uset_retainAll(uSetp, uSetTop);

    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uset_toPattern(%p, NULL, 0, TRUE, %p)", funcs, uSetp, &uErrorCode);
    uSetPatternl = uset_toPattern(uSetp, NULL, 0, TRUE, &uErrorCode);
    if (uErrorCode != U_BUFFER_OVERFLOW_ERROR) {
      errno = ENOSYS;
      goto err;
    }
    uSetPatterns = malloc((uSetPatternl + 1) * sizeof(UChar));
    if (uSetPatterns == NULL) {
      goto err;
    }
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uset_toPattern(%p, %p, %lld, TRUE, %p)", funcs, uSetp, uSetPatterns, (unsigned long long) (uSetPatternl + 1), &uErrorCode);
    uset_toPattern(uSetp, uSetPatterns, uSetPatternl, TRUE, &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }
    /* Make sure uSetPatterns is NULL terminated (a-la UTF-16) */
    p = (char *) (uSetPatterns + uSetPatternl);
    *p++ = '\0';
    *p   = '\0';

    /* ---------------------------------------------------------------------------- */
    /* Create transliterator                                                        */
    /* ---------------------------------------------------------------------------- */
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - uTransliteratorp(%p, %d, UTRANS_FORWARD, NULL, 0, NULL, %p)", funcs, universalTransliterators, universalTransliteratorsLength, &uErrorCode);
    uTransliteratorp = utrans_openU(universalTransliterators,
                                    universalTransliteratorsLength,
                                    UTRANS_FORWARD,
                                    NULL,
                                    0,
                                    NULL,
                                    &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }

    /* Add a filter to this transliterator using the intersected USet's */
    uErrorCode = U_ZERO_ERROR;
    TCONV_TRACE(tconvp, "%s - utrans_setFilter(%p, %p, %lld, %p)", funcs, uSetPatterns, (unsigned long long) uSetPatternl, &uErrorCode);
    utrans_setFilter(uTransliteratorp,
                     uSetPatterns,
                     uSetPatternl,
                     &uErrorCode);
    if (U_FAILURE(uErrorCode)) {
      errno = ENOSYS;
      goto err;
    }

    /* Cleanup */
    uset_close(uSetFromp);
    uset_close(uSetTop);
    uset_close(uSetp);
    uSetFromp = NULL;
    uSetTop   = NULL;
    uSetp     = NULL;

#ifndef TCONV_NTRACE
    free(uSetPatternFroms);
    free(uSetPatternTos);
    uSetPatternFroms = NULL;
    uSetPatternTos   = NULL;
#endif
    free(uSetPatterns);
    uSetPatterns     = NULL;
#endif /* UCONFIG_NO_TRANSLITERATION */
  }

  /* ----------------------------------------------------------- */
  /* Setup the context                                           /
  /* ----------------------------------------------------------- */
  TCONV_TRACE(tconvp, "%s - malloc(%lld)", funcs, (unsigned long long) sizeof(tconv_convert_ICU_context_t));
  contextp = (tconv_convert_ICU_context_t *) malloc(sizeof(tconv_convert_ICU_context_t));
  if (contextp == NULL) {
    goto err;
  }

  contextp->uConverterFromp  = uConverterFromp;
  contextp->uCharBufp        = uCharBufp;
  contextp->uCharCapacityl   = uCharCapacityl;
  contextp->uCharSizel       = uCharSizel;
  contextp->uConverterTop    = uConverterTop;
#if !UCONFIG_NO_TRANSLITERATION
  contextp->chunkp           = NULL;
  contextp->chunkcopyp       = NULL;
  contextp->chunkCapacityl   = 0;
  contextp->chunkUsedl       = 0;
  contextp->chunkSizel       = 0;
  contextp->outp             = NULL;
  contextp->outCapacityl     = 0;
  contextp->outUsedl         = 0;
  contextp->outSizel         = 0;
  contextp->uTransliteratorp = uTransliteratorp;
#endif

  TCONV_TRACE(tconvp, "%s - return %p", funcs, contextp);
  return contextp;

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
    if (uCharBufp != NULL) {
      free(uCharBufp);
    }
    if (uConverterTop != NULL) {
      ucnv_close (uConverterTop);
    }
    if (uSetPatternFroms == NULL) {
      free(uSetPatternFroms);
    }
    if (uSetFromp != NULL) {
      uset_close(uSetFromp);
    }
    if (uSetPatternTos == NULL) {
      free(uSetPatternTos);
    }
    if (uSetTop != NULL) {
      uset_close(uSetTop);
    }
    if (uSetp != NULL) {
      uset_close(uSetp);
    }
#if !UCONFIG_NO_TRANSLITERATION
    if (uTransliteratorp != NULL) {
      utrans_close(uTransliteratorp);
    }
#endif
    if (contextp != NULL) {
      free(contextp);
    }
    errno = errnol;
  }
  TCONV_TRACE(tconvp, "%s - return -1", funcs);
  return (void *)-1;
}

/*****************************************************************************/
size_t tconv_convert_ICU_run(tconv_t tconvp, void *voidp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char            funcs[]      = "tconv_convert_ICU_run";
  tconv_convert_ICU_context_t *contextp     = (tconv_convert_ICU_context_t *) voidp;
  /* The following is nothing else but uconv.cpp adapted to buffer and in C */
  /* so the credits go to authors of uconv.cpp                              */
  size_t                       bufsz;
  UConverter                  *convfrom;
  UConverter                  *convto;
  UBool                        flush;
  char                        *bufp;
  const UChar                 *unibuf;
  const UChar                 *unibufbp;
  UChar                       *unibufp;
  UChar                       *u;            /* String to do the transliteration */
  int32_t                      ulen;
  UBool                        fromSawEndOfBytes;
  UBool                        toSawEndOfUnicode;
  int8_t                       sig;
  UErrorCode                   uErrorCode;
#if !UCONFIG_NO_TRANSLITERATION
  UTransliterator             *t;            /* Transliterator acting on Unicode data. */
  UChar                       *chunk;        /* One chunk of the text being collected for transformation */
  UChar                       *chunkcopy;    /* Because prefighting with utrans_transUChars does not exist */
  int32_t                      chunkcapacity;
  int32_t                      chunkused;
  size_t                       chunksize;
  UChar                       *out;         /* Transformed chunk */
  int32_t                      outcapacity;
  int32_t                      outused;
  size_t                       outsize;
#endif

  /* In any case output must not be NULL */
  if ((outbufpp = NULL) || (outbytesleftlp == NULL)) {
    errno = EINVAL;
    goto err;
  }

  bufsz     = contextp->uCharSizel;
  convfrom  = contextp->uConverterFromp;
  convto    = contextp->uConverterTop;
  flush     = ((inbufpp == NULL) || (*inbufpp == NULL)) ? TRUE : FALSE;
  u         = contextp->uCharBufp;
#if !UCONFIG_NO_TRANSLITERATION
  chunk         = contextp->chunkp;
  chunkcopy     = contextp->chunkcopyp;
  chunkcapacity = contextp->chunkCapacityl;
  chunkused     = contextp->chunkUsedl;
  chunksize     = contextp->chunkSizel;
  out           = contextp->outp;
  outcapacity   = contextp->outCapacityl;
  outused       = contextp->outUsedl;
  outsize       = contextp->outSizel;
  t             = contextp->uTransliteratorp;
#endif

  /* convert until the input is consumed */
  do {
    /* remember the start of the current byte-to-Unicode conversion */
    unibuf = unibufp = u;

    if ((inbufpp != NULL) && (inbytesleftlp != NULL)) {
      TCONV_TRACE(tconvp, "%s - ucnv_toUnicode(%p, %p, %p, %p, %p, NULL, %d, NULL)", funcs, convfrom, &unibufp, unibuf + bufsz, inbufpp, *inbufpp + *inbytesleftlp, (int) flush);
      ucnv_toUnicode(convfrom,
		     &unibufp,
		     unibuf + bufsz,
		     (const char **) inbufpp,
		     *inbufpp + *inbytesleftlp,
		     NULL,
		     flush,
		     NULL);
      ulen = (int32_t)(unibufp - unibuf);
      /*
      // fromSawEndOfBytes indicates that ucnv_toUnicode() is done
      // converting all of the input bytes.
      // It works like this because ucnv_toUnicode() returns only under the
      // following conditions:
      // - an error occurred during conversion (an error code is set)
      // - the target buffer is filled (the error code indicates an overflow)
      // - the source is consumed
      // That is, if the error code does not indicate a failure,
      // not even an overflow, then the source must be consumed entirely.
      */
      fromSawEndOfBytes = (UBool)U_SUCCESS(uErrorCode);
      /*
      // Replaced a check for whether the input was consumed by
      // looping until it is; message key "premEndInput" now obsolete.
      */
      if (ulen == 0) {
	continue;
      }
#if !UCONFIG_NO_TRANSLITERATION
      /*
      // Transliterate/transform if needed.

      // For transformation, we use chunking code -
      // collect Unicode input until, for example, an end-of-line,
      // then transform and output-convert that and continue collecting.
      // This makes the transformation result independent of the buffer size
      // while avoiding the slower keyboard mode.
      // The end-of-chunk characters are completely included in the
      // transformed string in case they are to be transformed themselves.
      */
      {
	UChar  *out;
	int32_t chunkLimit;

	do {
	  chunkLimit = getChunkLimit(chunk, chunkused, u, ulen);
	  if (chunkLimit < 0 && flush && fromSawEndOfBytes) {
	    /* use all of the rest at the end of the text */
	    chunkLimit = ulen;
	  }
	  if (chunkLimit >= 0) {
	    int32_t  textLength;
	    int32_t  textCapacity;
	    int32_t  newchunkused;
	    int32_t  limit;

	    newchunkused = chunkused + chunkLimit;
	    /* complete the chunk and transform it */
	    if (newchunkused > chunkcapacity) {
	      int32_t newchunkcapacity = newchunkused;
	      size_t  newchunksize     = newchunkcapacity * sizeof(UChar);

	      chunk     = (chunk     == NULL) ? (UChar *) malloc(newchunksize) : (UChar *) realloc(chunk,     newchunksize);
	      chunkcopy = (chunkcopy == NULL) ? (UChar *) malloc(newchunksize) : (UChar *) realloc(chunkcopy, newchunksize);
	      if ((chunk == NULL) ||  (chunkcopy == NULL)) {
		goto err;
	      }
	      contextp->chunkSizel     = chunksize     = newchunksize;
	      contextp->chunkCapacityl = chunkcapacity = newchunkcapacity;
	      contextp->chunkp         = chunk;
	      contextp->chunkcopyp     = chunkcopy;
	    }
	    memcpy(chunk + chunkused, u, chunkLimit * sizeof(UChar));
	    contextp->chunkUsedl = chunkused = newchunkused;
	    /* utrans_transUChars() is not very user-friendly, in the sense that prefighting is not possible */
	    textLength   = chunkused;
	    textCapacity = chunkcapacity;
	    limit        = chunkused;
	    do {
	      uErrorCode   = U_ZERO_ERROR;
	      /* Copy of original chunk if we have to retry */
	      memcpy(chunkcopy, chunk, chunksize);
	      utrans_transUChars(t, chunk, &textLength, textCapacity, 0, &limit, &uErrorCode);
	      if (uErrorCode == U_BUFFER_OVERFLOW_ERROR) {
		/* Voila... Increase chunk allocated size and retry */
		int32_t newchunkcapacity = chunkcapacity * 2;
		size_t  newchunksize     = newchunkcapacity * sizeof(UChar);

		chunk     = (UChar *) realloc(chunk, newchunksize);
		chunkcopy = (UChar *) realloc(chunkcopy, newchunksize);
		if ((chunk == NULL) ||  (chunkcopy == NULL)) {
		  goto err;
		}
		contextp->chunkSizel     = chunksize     = newchunksize;
		contextp->chunkCapacityl = chunkcapacity = newchunkcapacity;
		contextp->chunkp         = chunk;
		contextp->chunkcopyp     = chunkcopy;
	      }
	    } while (uErrorCode == U_BUFFER_OVERFLOW_ERROR);  
	    /* append the transformation result to the result and empty the chunk */
	    {
	      int32_t  newoutused = outused + chunkused;
	      if (newoutused > outcapacity) {
		int32_t newoutcapacity = newoutused;
		size_t  newoutsize     = newoutcapacity * sizeof(UChar);

		out = (out == NULL) ? (UChar *) malloc(newoutsize) : (UChar *) realloc(out, newoutsize);
		if (out == NULL) {
		  goto err;
		}
		contextp->outSizel     = outsize     = newoutsize;
		contextp->outCapacityl = outcapacity = newoutcapacity;
		contextp->outp         = out;
	      }
	    }
	    memcpy(out + outused, chunk, chunkused * sizeof(UChar));
	    chunkused = 0;
	  } else {
	    /* continue collecting the chunk */
	    int32_t newchunkused = chunkused + ulen;

	    if (newchunkused > chunkcapacity) {
	      int32_t newchunkcapacity = newchunkused;
	      size_t  newchunksize     = newchunkcapacity * sizeof(UChar);

	      chunk     = (chunk     == NULL) ? (UChar *) malloc(newchunksize) : (UChar *) realloc(chunk,     newchunksize);
	      chunkcopy = (chunkcopy == NULL) ? (UChar *) malloc(newchunksize) : (UChar *) realloc(chunkcopy, newchunksize);
	      if ((chunk == NULL) ||  (chunkcopy == NULL)) {
		goto err;
	      }
	      contextp->chunkSizel     = chunksize     = newchunksize;
	      contextp->chunkCapacityl = chunkcapacity = newchunkcapacity;
	      contextp->chunkp         = chunk;
	      contextp->chunkcopyp     = chunkcopy;
	    }
	    memcpy(chunk + chunkused, u, ulen * sizeof(UChar));
	    chunkused = newchunkused;
	    break;
	  }
	} while (ulen > 0);

	u = out;
	ulen = outused;
      }
#endif
    } else {
      fromSawEndOfBytes = TRUE;
      ulen = 0;
    }
    /*
    // Convert the Unicode buffer into the destination codepage
    // Again 'bufp' will be placed behind the last converted character
    // And 'unibufp' will be placed behind the last converted unicode character
    // At the last conversion flush should be set to true to ensure that
    // all characters left get converted
    */
    unibuf = unibufbp = u;

    do {
      /*
      // Use fromSawEndOfBytes in addition to the flush flag -
      // it indicates whether the intermediate Unicode string
      // contains the very last UChars for the very last input bytes.
      */
      uErrorCode = U_ZERO_ERROR;
      ucnv_fromUnicode(convto, outbufpp, *outbufpp + *outbytesleftlp,
		       &unibufbp,
		       unibuf + ulen,
		       NULL,
		       ((flush == TRUE) && (fromSawEndOfBytes == TRUE)) ? TRUE : FALSE,
		       &uErrorCode);

      /*
      // toSawEndOfUnicode indicates that ucnv_fromUnicode() is done
      // converting all of the intermediate UChars.
      // See comment for fromSawEndOfBytes.
      */
      toSawEndOfUnicode = (UBool) U_SUCCESS(uErrorCode);
      if (U_FAILURE(uErrorCode)) {
	if (uErrorCode == U_BUFFER_OVERFLOW_ERROR) {
	  uErrorCode = U_ZERO_ERROR;
	  errno = E2BIG;
	}
	goto err;
      }
    } while (toSawEndOfUnicode == FALSE);
  } while (fromSawEndOfBytes == FALSE);

 err:
  if (U_FAILURE(uErrorCode)) {
    tconv_error_set(tconvp, u_errorName(uErrorCode));
  }
  return (size_t)-1;
}

/*****************************************************************************/
int tconv_convert_ICU_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_ICU_free";

  return 0;
}

/* 
   Note from http://userguide.icu-project.org/strings :
   Endianness is not an issue on this level because the interpretation of an integer is fixed within any given platform.
*/
enum {
    uSP  = 0x20,         // space
    uCR  = 0xd,          // carriage return
    uLF  = 0xa,          // line feed
    uNL  = 0x85,         // newline
    uLS  = 0x2028,       // line separator
    uPS  = 0x2029,       // paragraph separator
    uSig = 0xfeff       // signature/BOM character
};

static const UChar paraEnds[] = {
  0xd, 0xa, 0x85, 0x2028, 0x2029
};
enum {
  iCR = 0, iLF, iNL, iLS, iPS, iCount
};
  
/*****************************************************************************/
static TCONV_C_INLINE int32_t getChunkLimit(const UChar *prev, const size_t prevlen, const UChar *s, size_t slen)
/*****************************************************************************/
{
  const UChar *u     = s;
  const UChar *limit = u + slen;
    UChar c;
  /*
  // find one of
  // CR, LF, CRLF, NL, LS, PS
  // for paragraph ends (see UAX #13/Unicode 4)
  // and include it in the chunk
  // all of these characters are on the BMP
  // do not include FF or VT in case they are part of a paragraph
  // (important for bidi contexts)
  */
  /* first, see if there is a CRLF split between prev and s */
  if ((prevlen > 0) && (prev[prevlen - 1] == paraEnds[iCR])) {
    if ((slen > 0) && (s[0] == paraEnds[iLF])) {
      return 1; // split CRLF, include the LF
    } else if (slen > 0) {
      return 0; // complete the last chunk
    } else {
      return -1; // wait for actual further contents to arrive
    }
  }

  while (u < limit) {
    c = *u++;
    if (
        ((c < uSP) && (c == uCR || c == uLF)) ||
        (c == uNL) ||
        ((c & uLS) == uLS)
        ) {
      if (c == uCR) {
        // check for CRLF
        if (u == limit) {
          return -1; // LF may be in the next chunk
        } else if (*u == uLF) {
          ++u; // include the LF in this chunk
        }
      }
      return (int32_t)((u - s) / sizeof(UChar));
    }
  }

  return -1; // continue collecting the chunk
}
