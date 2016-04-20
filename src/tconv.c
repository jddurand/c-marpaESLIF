#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#include "tconv.h"
#include "tconv_config.h"

/* For logging */
static char *files = "tconv.c";

/* Internal structure */
struct tconv {
  void                    *charsetContextp;
  void                    *convertContextp;
  genericLogger_t         *genericLoggerp;
  /* At the end, we always end up in an "external"-like configuration */
  tconv_charset_external_t charsetExternal;
  tconv_convert_external_t convertExternal;
  /* For cleanup */
  void                    *sharedLibraryHandlep;
};

#ifndef TCONV_NTRACE
#define TCONV_TRACE(tconvp, msgs) do {                        \
  if ((tconvp != NULL) && (tconvp->genericLoggerp != NULL)) { \
    GENERICLOGGER_TRACE(tconvp->genericLoggerp, msgs);        \
  }                                                           \
} while (0)
#define TCONV_TRACEF(tconvp, fmts, ...) do {                         \
    if ((tconvp != NULL) && (tconvp->genericLoggerp != NULL)) {      \
    GENERICLOGGER_TRACEF(tconvp->genericLoggerp, fmts, __VA_ARGS__); \
  }                                                                  \
} while (0)
#else
#define TCONV_TRACE(tconvp, msgs)
#define TCONV_TRACEF(tconvp, fmts, ...)
#endif

/* All our functions have an err label if necessary */
#define TCONV_MALLOC(ptr, type, size) do {                              \
    ptr = (type) malloc(size);						\
    if (ptr == NULL) {							\
      TCONV_TRACEF(tconvp, "malloc: %s", strerror(errno));              \
      goto err;								\
    }									\
  } while (0)

static TCONV_C_INLINE _tconvDefaultOption(tconv_t tconvp);
static TCONV_C_INLINE _tconvDefaultCharsetOption(tconv_charset_external_t *tconvCharsetExternalp);
static TCONV_C_INLINE _tconvDefaultConvertOption(tconv_convert_external_t *tconvConvertExternalp);

/****************************************************************************/
tconv_t tconv_open(const char *tocodes, const char *fromcodes)
/****************************************************************************/
{
  return tconv_open_ext(tocodes, fromcodes, NULL);
}

/****************************************************************************/
int tconv_close(tconv_t tconvp)
/****************************************************************************/
{
  if (tconvp != NULL) {
    if (tconvp->charsetContextp != NULL) {
      if (tconvp->charsetExternal.tconv_charset_freep != NULL) {
        tconvp->charsetExternal.tconv_charset_freep(tconvp, tconvp->charsetContextp);
      }
    }
    if (tconvp->convertContextp != NULL) {
      if (tconvp->convertExternal.tconv_convert_freep != NULL) {
        (*(tconvp->convertExternal.tconv_convert_freep))(tconvp, tconvp->convertContextp);
      }
    }
    if (tconvp->sharedLibraryHandlep != NULL) {
      if (dlclose(tconvp->sharedLibraryHandlep) != 0) {
        TCONV_TRACEF(tconvp, "dlclose: %s", strerror(errno));
      }
    }
    free(tconvp);
  }

  return 0;
}

/****************************************************************************/
tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_option_t *tconvOptionp)
/****************************************************************************/
{
  void            *sharedLibraryHandlep = NULL;
  tconv_t          tconvp;

  TCONV_MALLOC(tconvp, tconv_t, sizeof(struct tconv));

  tconvp->charsetContextp      = NULL;
  tconvp->convertContextp      = NULL;
  tconvp->sharedLibraryHandlep = NULL;

  /* Validate the options, if any */
  if (tconvOptionp != NULL) {

    /* Charset */
    if (tconvOptionp->charsetp != NULL) {
      switch (tconvOptionp->charsetp->charseti) {
      case TCONV_CHARSET_EXTERNAL:
        tconvp->charsetExternal = tconvOptionp->charsetp->u.external;
        break;
      case TCONV_CHARSET_PLUGIN:
        tconvp->sharedLibraryHandlep = dlopen(tconvOptionp->charsetp->u.plugin.filenames, RTLD_LAZY);
        if (tconvp->sharedLibraryHandlep == NULL) {
          TCONV_TRACEF(tconvp, "dlopen: %s", strerror(errno));
          goto err;
        }
        tconvp->charsetExternal.tconv_charset_newp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_charset_newp");
        tconvp->charsetExternal.tconv_charset_runp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_charset_runp");
        tconvp->charsetExternal.tconv_charset_freep = dlsym(tconvp->sharedLibraryHandlep, "tconv_charset_freep");
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.plugin.optionp;
        break;
      case TCONV_CHARSET_ICU:
#ifdef TCONV_HAVE_ICU
        tconvp->charsetExternal.tconv_charset_newp  = tconv_charset_ICU_new;
        tconvp->charsetExternal.tconv_charset_runp  = tconv_charset_ICU_run;
        tconvp->charsetExternal.tconv_charset_freep = tconv_charset_ICU_free;
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.cchardetOptionp;
#endif
        break;
      case TCONV_CHARSET_CCHARDET:
        tconvp->charsetExternal.tconv_charset_newp  = tconv_charset_cchardet_new;
        tconvp->charsetExternal.tconv_charset_runp  = tconv_charset_cchardet_run;
        tconvp->charsetExternal.tconv_charset_freep = tconv_charset_cchardet_free;
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.ICUOptionp;
        break;
      default:
        tconvp->charsetExternal.tconv_charset_newp  = NULL;
        tconvp->charsetExternal.tconv_charset_runp  = NULL;
        tconvp->charsetExternal.tconv_charset_freep = NULL;
        tconvp->charsetExternal.optionp             = NULL;
        break;
      }
      if ((tconvp->charsetExternal.tconv_charset_newp == NULL) ||
          (tconvp->charsetExternal.tconv_charset_runp == NULL) ||
          (tconvp->charsetExternal.tconv_charset_freep == NULL)) {
        errno = EINVAL;
        goto err;
      }
    } else {    
      _tconvDefaultCharsetOption(&(tconvp->charsetExternal));
    }
    if (tconvOptionp->convertp != NULL) {
      switch (tconvOptionp->convertp->converti) {
      case TCONV_CONVERT_EXTERNAL:
        tconvp->convertExternal = tconvOptionp->convertp->u.external;
        break;
      case TCONV_CONVERT_PLUGIN:
        tconvp->sharedLibraryHandlep = dlopen(tconvOptionp->convertp->u.plugin.filenames, RTLD_LAZY);
        if (tconvp->sharedLibraryHandlep == NULL) {
          TCONV_TRACEF(tconvp, "dlopen: %s", strerror(errno));
          goto err;
        }
        tconvp->convertExternal.tconv_convert_newp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_convert_newp");
        tconvp->convertExternal.tconv_convert_runp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_convert_runp");
        tconvp->convertExternal.tconv_convert_freep = dlsym(tconvp->sharedLibraryHandlep, "tconv_convert_freep");
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.plugin.optionp;
        break;
      case TCONV_CONVERT_ICU:
#ifdef TCONV_HAVE_ICUJDD
        tconvp->convertExternal.tconv_convert_newp  = tconv_convert_ICU_new;
        tconvp->convertExternal.tconv_convert_runp  = tconv_convert_ICU_run;
        tconvp->convertExternal.tconv_convert_freep = tconv_convert_ICU_free;
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.ICUOptionp;
#endif
        break;
      case TCONV_CONVERT_ICONV:
#ifdef TCONV_HAVE_ICONV
        tconvp->convertExternal.tconv_convert_newp  = tconv_convert_iconv_new;
        tconvp->convertExternal.tconv_convert_runp  = tconv_convert_iconv_run;
        tconvp->convertExternal.tconv_convert_freep = tconv_convert_iconv_free;
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.iconvOptionp;
#endif
        break;
      default:
        tconvp->convertExternal.tconv_convert_newp  = NULL;
        tconvp->convertExternal.tconv_convert_runp  = NULL;
        tconvp->convertExternal.tconv_convert_freep = NULL;
        tconvp->convertExternal.optionp             = NULL;
        break;
      }
      if ((tconvp->convertExternal.tconv_convert_newp == NULL) ||
          (tconvp->convertExternal.tconv_convert_runp == NULL) ||
          (tconvp->convertExternal.tconv_convert_freep == NULL)) {
        errno = EINVAL;
        goto err;
      }
    } else {    
      _tconvDefaultConvertOption(&(tconvp->convertExternal));
    }
  }

  return tconvp;
  
 err:
  {
    int errnol = errno;
    tconv_close(tconvp);
    errno = errnol;
  }
  return NULL;
}

/****************************************************************************/
size_t tconv(tconv_t tconvp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/****************************************************************************/
{
  if (tconvp == NULL) {
    errno = EINVAL;
    goto err;
  }

 err:
  return (size_t)-1;
}

/****************************************************************************/
static TCONV_C_INLINE _tconvDefaultOption(tconv_t tconvp)
/****************************************************************************/
{
  tconvp->genericLoggerp = NULL;
  _tconvDefaultCharsetOption(&(tconvp->charsetExternal));
  _tconvDefaultConvertOption(&(tconvp->convertExternal));
}

/****************************************************************************/
static TCONV_C_INLINE _tconvDefaultCharsetOption(tconv_charset_external_t *tconvCharsetExternalp)
/****************************************************************************/
{
#ifdef TCONV_HAVE_ICU
  tconvCharsetExternalp->tconv_charset_newp  = tconv_charset_ICU_new;
  tconvCharsetExternalp->tconv_charset_runp  = tconv_charset_ICU_run;
  tconvCharsetExternalp->tconv_charset_freep = tconv_charset_ICU_free;
#else
  tconvCharsetExternalp->tconv_charset_newp  = tconv_charset_cchardet_new;
  tconvCharsetExternalp->tconv_charset_runp  = tconv_charset_cchardet_run;
  tconvCharsetExternalp->tconv_charset_freep = tconv_charset_cchardet_free;
#endif
}

/****************************************************************************/
static TCONV_C_INLINE _tconvDefaultConvertOption(tconv_convert_external_t *tconvConvertExternalp)
/****************************************************************************/
{
#ifdef TCONV_HAVE_ICUJDD
  tconvConvertExternalp->tconv_convert_newp  = tconv_convert_ICU_new;
  tconvConvertExternalp->tconv_convert_runp  = tconv_convert_ICU_run;
  tconvConvertExternalp->tconv_convert_freep = tconv_convert_ICU_free;
  tconvConvertExternalp->optionp             = NULL;
#else
  #ifdef TCONV_HAVE_ICONV
  tconvConvertExternalp->tconv_convert_newp  = tconv_convert_iconv_new;
  tconvConvertExternalp->tconv_convert_runp  = tconv_convert_iconv_run;
  tconvConvertExternalp->tconv_convert_freep = tconv_convert_iconv_free;
  tconvConvertExternalp->optionp             = NULL;
  #else
  tconvConvertExternalp->tconv_convert_newp  = NULL;
  tconvConvertExternalp->tconv_convert_runp  = NULL;
  tconvConvertExternalp->tconv_convert_freep = NULL;
  tconvConvertExternalp->optionp             = NULL;
  #endif
#endif
}
