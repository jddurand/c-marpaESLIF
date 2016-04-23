#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <genericLogger.h>
#ifndef TCONV_NTRACE
#include <stdarg.h>
#endif
#include "tconv.h"
#include "tconv_config.h"

/* For logging */
static char *files = "tconv.c";
#define TCONV_ENV_TRACE "TCONV_ENV_TRACE"
#ifndef TCONV_NTRACE
/* Inlined for performance */
#define TCONV_TRACE(tconvp, msgs)       tconv_trace(tconvp, msgs)
#define TCONV_TRACEF(tconvp, fmts, ...) tconv_trace(tconvp, fmts, __VA_ARGS__)
static TCONV_C_INLINE void _tconvTraceCallbackProxy(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
#else
#define TCONV_TRACE(tconvp, msgs)
#define TCONV_TRACEF(tconvp, fmts, ...)
#endif

/* Default options for built-ins */
static tconv_charset_ICU_option_t tconv_charset_ICU_option_default = { 10 };
static tconv_charset_ICU_option_t tconv_charset_cchardet_option_default = { 10 };


/* Internal structure */
struct tconv {
  void                    *charsetContextp;
  void                    *convertContextp;
  char                    *tocodes;
  char                    *fromcodes;
  char                    *charsetBytep;
  size_t                   charsetBytel;
  genericLogger_t         *genericLoggerp;
  /* At the end, we always end up in an "external"-like configuration */
  tconv_charset_external_t charsetExternal;
  tconv_convert_external_t convertExternal;
  /* For cleanup */
  void                    *sharedLibraryHandlep;
  /* For trace */
  short                    traceb;
  tconvTraceCallback_t     traceCallbackp;
  void                    *traceUserDatavp;
};

/* All our functions have an err label if necessary */
#define TCONV_MALLOC(funcs, ptr, type, size) do {                       \
    ptr = (type) malloc(size);						\
    if (ptr == NULL) {							\
      TCONV_TRACEF(tconvp, "%s - malloc(%lld) failure, %s", funcs, (unsigned long long) size, strerror(errno)); \
      goto err;								\
    }									\
  } while (0)

static TCONV_C_INLINE void _tconvDefaultOption(tconv_t tconvp);
static TCONV_C_INLINE void _tconvDefaultCharsetOption(tconv_t tconvp, tconv_charset_external_t *tconvCharsetExternalp);
static TCONV_C_INLINE void _tconvDefaultConvertOption(tconv_t tconvp, tconv_convert_external_t *tconvConvertExternalp);

/****************************************************************************/
tconv_t tconv_open(const char *tocodes, const char *fromcodes)
/****************************************************************************/
{
  return tconv_open_ext(tocodes, fromcodes, NULL);
}

/****************************************************************************/
void tconv_trace_on(tconv_t tconvp)
/****************************************************************************/
{
  if (tconvp != NULL) {
    tconvp->traceb = 1;
  }
}

/****************************************************************************/
void tconv_trace_off(tconv_t tconvp)
/****************************************************************************/
{
  if (tconvp != NULL) {
    tconvp->traceb = 0;
  }
}

/****************************************************************************/
void tconv_trace(tconv_t tconvp, const char *fmts, ...)
/****************************************************************************/
{
#ifndef TCONV_NTRACE
  va_list ap;
  
  if ((tconvp != NULL) && (tconvp->traceb != 0) && (tconvp->genericLoggerp != NULL)) {
    va_start(ap, fmts);
    GENERICLOGGER_LOGAP(tconvp->genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE, fmts, ap);
    va_end(ap);
  }
#endif
}

/****************************************************************************/
int tconv_close(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "tconv_close";
  int rci = 0;

  if (tconvp != NULL) {
    if (tconvp->charsetContextp != NULL) {
      if (tconvp->charsetExternal.tconv_charset_freep != NULL) {
        TCONV_TRACEF(tconvp, "%s - freeing charset engine", funcs);
        tconvp->charsetExternal.tconv_charset_freep(tconvp, tconvp->charsetContextp);
      }
    }
    if (tconvp->convertContextp != NULL) {
      if (tconvp->convertExternal.tconv_convert_freep != NULL) {
        TCONV_TRACEF(tconvp, "%s - freeing convert engine", funcs);
        tconvp->convertExternal.tconv_convert_freep(tconvp, tconvp->convertContextp);
      }
    }
    if (tconvp->tocodes != NULL) {
      TCONV_TRACEF(tconvp, "%s - freeing copy of \"to\" code %s", funcs, tconvp->tocodes);
      free(tconvp->tocodes);
    }
    if (tconvp->fromcodes != NULL) {
      TCONV_TRACEF(tconvp, "%s - freeing copy of \"from\" code %s", funcs, tconvp->fromcodes);
      free(tconvp->fromcodes);
    }
    if (tconvp->sharedLibraryHandlep != NULL) {
      TCONV_TRACEF(tconvp, "%s - closing shared library", funcs);
      if (dlclose(tconvp->sharedLibraryHandlep) != 0) {
        TCONV_TRACEF(tconvp, "%s - dlclose failure, %s", funcs, dlerror());
        errno = EFAULT;
        rci = -1;
      }
    }
    if (tconvp->charsetBytep != NULL) {
      TCONV_TRACEF(tconvp, "%s - freeing copy of first bytes", funcs);
      free(tconvp->charsetBytep);
    }
    free(tconvp);
  }

  return 0;
}

/****************************************************************************/
tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_option_t *tconvOptionp)
/****************************************************************************/
{
  static const char funcs[]              = "tconv_open_ext";
  void             *sharedLibraryHandlep = NULL;
  tconv_t           tconvp               = NULL;
  char             *traces               = NULL;

  TCONV_MALLOC(funcs, tconvp, tconv_t, sizeof(struct tconv));

#ifndef TCONV_NTRACE
  /* Get the genericLogger asap */
  if (tconvOptionp != NULL) {
    tconvp->traceCallbackp  = tconvOptionp->traceCallbackp;
    tconvp->traceUserDatavp = tconvOptionp->traceUserDatavp;
    tconvp->genericLoggerp  = genericLogger_newp(_tconvTraceCallbackProxy, tconvp, GENERICLOGGER_LOGLEVEL_TRACE);
  } else {
    tconvp->traceCallbackp  = NULL;
    tconvp->traceUserDatavp = NULL;
    tconvp->genericLoggerp  = NULL;
  }
#else
  /* No trace in any case */
  tconvp->traceCallbackp  = NULL;
  tconvp->traceUserDatavp = NULL;
  tconvp->genericLoggerp  = NULL;
#endif

  traces                       = getenv(TCONV_ENV_TRACE);
  tconvp->traceb               = (traces != NULL) ? (atoi(traces) != 0 ? 1 : 0) : 0;
  tconvp->charsetContextp      = NULL;
  tconvp->convertContextp      = NULL;
  tconvp->sharedLibraryHandlep = NULL;
  tconvp->charsetBytep         = NULL;
  tconvp->charsetBytel         = 0;

  /* Always keep a copy of tocodes and from*/
  if (tocodes != NULL) {
    TCONV_TRACEF(tconvp, "%s - duplicating \"to\" code %s", funcs, tocodes);
    TCONV_MALLOC(funcs, tconvp->tocodes, char *, sizeof(char) * (strlen(tocodes) + 1));
    strcpy(tconvp->tocodes, tocodes);
  } else {
    tconvp->tocodes = NULL;
  }
  if (fromcodes != NULL) {
    TCONV_TRACEF(tconvp, "%s - duplicating \"from\" code %s", funcs, fromcodes);
    TCONV_MALLOC(funcs, tconvp->fromcodes, char *, sizeof(char) * (strlen(fromcodes) + 1));
    strcpy(tconvp->fromcodes, fromcodes);
  } else {
    tconvp->fromcodes = NULL;
  }

  /* Validate the options, if any */
  if (tconvOptionp != NULL) {

    TCONV_TRACEF(tconvp, "%s - validation user option", funcs);

    /* Charset */
    if (tconvOptionp->charsetp != NULL) {
      switch (tconvOptionp->charsetp->charseti) {
      case TCONV_CHARSET_EXTERNAL:
        TCONV_TRACEF(tconvp, "%s - charset detector type is external", funcs);
        tconvp->charsetExternal = tconvOptionp->charsetp->u.external;
        break;
      case TCONV_CHARSET_PLUGIN:
        TCONV_TRACEF(tconvp, "%s - charset detector type is plugin", funcs);
        if (tconvOptionp->charsetp->u.plugin.filenames == NULL) {
          TCONV_TRACEF(tconvp, "%s - null charset plugin filename", funcs);
          errno = EINVAL;
          goto err;
        }
        TCONV_TRACEF(tconvp, "%s - opening shared library %s", funcs, tconvOptionp->charsetp->u.plugin.filenames);
        tconvp->sharedLibraryHandlep = dlopen(tconvOptionp->charsetp->u.plugin.filenames, RTLD_LAZY);
        if (tconvp->sharedLibraryHandlep == NULL) {
          TCONV_TRACEF(tconvp, "%s - dlopen failure, %s", funcs, dlerror());
          errno = EINVAL;
          goto err;
        }
        tconvp->charsetExternal.tconv_charset_newp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_charset_newp");
        tconvp->charsetExternal.tconv_charset_runp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_charset_runp");
        tconvp->charsetExternal.tconv_charset_freep = dlsym(tconvp->sharedLibraryHandlep, "tconv_charset_freep");
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.plugin.optionp;
        break;
      case TCONV_CHARSET_ICU:
#ifdef TCONV_HAVE_ICU
        TCONV_TRACEF(tconvp, "%s - charset detector type is built-in ICU", funcs);
        tconvp->charsetExternal.tconv_charset_newp  = tconv_charset_ICU_new;
        tconvp->charsetExternal.tconv_charset_runp  = tconv_charset_ICU_run;
        tconvp->charsetExternal.tconv_charset_freep = tconv_charset_ICU_free;
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.cchardetOptionp;
        if (tconvp->charsetExternal.optionp == NULL) {
          TCONV_TRACEF(tconvp, "%s - setting default charset detector ICU option", funcs);
          tconvp->charsetExternal.optionp = &tconv_charset_ICU_option_default;
        }
#endif
        break;
      case TCONV_CHARSET_CCHARDET:
        TCONV_TRACEF(tconvp, "%s - charset detector type is built-in cchardet", funcs);
        tconvp->charsetExternal.tconv_charset_newp  = tconv_charset_cchardet_new;
        tconvp->charsetExternal.tconv_charset_runp  = tconv_charset_cchardet_run;
        tconvp->charsetExternal.tconv_charset_freep = tconv_charset_cchardet_free;
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.ICUOptionp;
        if (tconvp->charsetExternal.optionp == NULL) {
          TCONV_TRACEF(tconvp, "%s - setting default charset detector cchardet option", funcs);
          tconvp->charsetExternal.optionp = &tconv_charset_cchardet_option_default;
        }
        break;
      default:
        TCONV_TRACEF(tconvp, "%s - charset detector type is unknown", funcs);
        tconvp->charsetExternal.tconv_charset_newp  = NULL;
        tconvp->charsetExternal.tconv_charset_runp  = NULL;
        tconvp->charsetExternal.tconv_charset_freep = NULL;
        tconvp->charsetExternal.optionp             = NULL;
        break;
      }
      if (tconvp->charsetExternal.tconv_charset_runp == NULL) {
        /* Formally, only the "run" entry point is required */
        TCONV_TRACEF(tconvp, "%s - tconv_charset_runp is NULL", funcs);
        errno = EINVAL;
        goto err;
      }
    } else {    
      TCONV_TRACEF(tconvp, "%s - setting default charset options", funcs);
      _tconvDefaultCharsetOption(tconvp, &(tconvp->charsetExternal));
    }
    if (tconvOptionp->convertp != NULL) {
      switch (tconvOptionp->convertp->converti) {
      case TCONV_CONVERT_EXTERNAL:
        TCONV_TRACEF(tconvp, "%s - converter type is external", funcs);
        tconvp->convertExternal = tconvOptionp->convertp->u.external;
        break;
      case TCONV_CONVERT_PLUGIN:
        TCONV_TRACEF(tconvp, "%s - converter type is plugin", funcs);
        if (tconvOptionp->convertp->u.plugin.filenames == NULL) {
          TCONV_TRACEF(tconvp, "%s - null convert filename", funcs);
          errno = EINVAL;
          goto err;
        }
        TCONV_TRACEF(tconvp, "%s - opening shared library %s", funcs, tconvOptionp->convertp->u.plugin.filenames);
        tconvp->sharedLibraryHandlep = dlopen(tconvOptionp->convertp->u.plugin.filenames, RTLD_LAZY);
        if (tconvp->sharedLibraryHandlep == NULL) {
          TCONV_TRACEF(tconvp, "%s - dlopen failure, %s", funcs, dlerror());
          errno = EINVAL;
          goto err;
        }
        tconvp->convertExternal.tconv_convert_newp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_convert_newp");
        tconvp->convertExternal.tconv_convert_runp  = dlsym(tconvp->sharedLibraryHandlep, "tconv_convert_runp");
        tconvp->convertExternal.tconv_convert_freep = dlsym(tconvp->sharedLibraryHandlep, "tconv_convert_freep");
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.plugin.optionp;
        break;
      case TCONV_CONVERT_ICU:
#ifdef TCONV_HAVE_ICUJDD
        TCONV_TRACEF(tconvp, "%s - converter type is built-in ICU", funcs);
        tconvp->convertExternal.tconv_convert_newp  = tconv_convert_ICU_new;
        tconvp->convertExternal.tconv_convert_runp  = tconv_convert_ICU_run;
        tconvp->convertExternal.tconv_convert_freep = tconv_convert_ICU_free;
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.ICUOptionp;
#endif
        break;
      case TCONV_CONVERT_ICONV:
#ifdef TCONV_HAVE_ICONV
        TCONV_TRACEF(tconvp, "%s - converter type is built-in iconv", funcs);
        tconvp->convertExternal.tconv_convert_newp  = tconv_convert_iconv_new;
        tconvp->convertExternal.tconv_convert_runp  = tconv_convert_iconv_run;
        tconvp->convertExternal.tconv_convert_freep = tconv_convert_iconv_free;
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.iconvOptionp;
#endif
        break;
      default:
        TCONV_TRACEF(tconvp, "%s - converter type is unknown", funcs);
        tconvp->convertExternal.tconv_convert_newp  = NULL;
        tconvp->convertExternal.tconv_convert_runp  = NULL;
        tconvp->convertExternal.tconv_convert_freep = NULL;
        tconvp->convertExternal.optionp             = NULL;
        break;
      }
      if ((tconvp->convertExternal.tconv_convert_newp == NULL) ||
          (tconvp->convertExternal.tconv_convert_runp == NULL) ||
          (tconvp->convertExternal.tconv_convert_freep == NULL)) {
        TCONV_TRACEF(tconvp,
		    "%s - at least one convert entry point is invalid: tconv_convert_newp=%p, tconv_convert_runp=%p, tconv_convert_freep=%p",
		    funcs,
		    tconvp->convertExternal.tconv_convert_newp,
		    tconvp->convertExternal.tconv_convert_runp,
		    tconvp->convertExternal.tconv_convert_freep);
        errno = EINVAL;
        goto err;
      }
    } else {    
      TCONV_TRACEF(tconvp, "%s - setting default converter options", funcs);
      _tconvDefaultConvertOption(tconvp, &(tconvp->convertExternal));
    }
  } else {
    _tconvDefaultOption(tconvp);
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
  static const char funcs[]         = "tconv";
  void             *charsetContextp = NULL;
  void             *charsetOptionp  = NULL;
  char             *fromcodes       = NULL;

  if (tconvp == NULL) {
    errno = EINVAL;
    goto err;
  }

  if ((tconvp->fromcodes == NULL) &&
      (inbufsp != NULL)           &&
      (*inbufsp != NULL)          &&
      (inbytesleftlp != NULL)     &&
      (*inbytesleftlp > 0)) {
    /* Use that to detect charset */
    charsetOptionp = tconvp->charsetExternal.optionp;
    if (tconvp->charsetExternal.tconv_charset_newp != NULL) {
      TCONV_TRACEF(tconvp, "%s - initializing charset detection", funcs);
      charsetContextp = tconvp->charsetExternal.tconv_charset_newp(tconvp, charsetOptionp);
    }
    tconvp->charsetContextp = charsetContextp;
    TCONV_TRACEF(tconvp, "%s - calling charset detection", funcs);
    fromcodes = tconvp->charsetExternal.tconv_charset_runp(tconvp, charsetContextp, *inbufsp, *inbytesleftlp);
    if (fromcodes != NULL) {
      TCONV_TRACEF(tconvp, "%s - charset detection returned %s", funcs, fromcodes);
      TCONV_TRACEF(tconvp, "%s - duplicating \"to\" code %s", funcs, fromcodes);
      TCONV_MALLOC(funcs, tconvp->fromcodes, char *, sizeof(char) * (strlen(fromcodes) + 1));
      strcpy(tconvp->fromcodes, fromcodes);
      if (tconvp->charsetExternal.tconv_charset_freep != NULL) {
        /* It worked the very first time, we can remove the context right now */
        TCONV_TRACEF(tconvp, "%s - ending charset detection", funcs);
        tconvp->charsetExternal.tconv_charset_freep(tconvp, charsetContextp);
      }
      tconvp->charsetContextp = NULL;
    }
  }

 err:
  return (size_t)-1;
}

/****************************************************************************/
static TCONV_C_INLINE void _tconvDefaultOption(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "_tconvDefaultOption";

  TCONV_TRACEF(tconvp, "%s - setting a null generic logger", funcs);
  tconvp->genericLoggerp = NULL;

  _tconvDefaultCharsetOption(tconvp, &(tconvp->charsetExternal));
  _tconvDefaultConvertOption(tconvp, &(tconvp->convertExternal));
}

/****************************************************************************/
static TCONV_C_INLINE void _tconvDefaultCharsetOption(tconv_t tconvp, tconv_charset_external_t *tconvCharsetExternalp)
/****************************************************************************/
{
  static const char funcs[] = "_tconvDefaultCharsetOption";

#ifdef TCONV_HAVE_ICU
  TCONV_TRACEF(tconvp, "%s - setting default charset detector to ICU", funcs);
  tconvCharsetExternalp->optionp             = &tconv_charset_ICU_option_default;
  tconvCharsetExternalp->tconv_charset_newp  = tconv_charset_ICU_new;
  tconvCharsetExternalp->tconv_charset_runp  = tconv_charset_ICU_run;
  tconvCharsetExternalp->tconv_charset_freep = tconv_charset_ICU_free;
#else
  TCONV_TRACEF(tconvp, "%s - setting default charset detector to cchardet", funcs);
  tconvCharsetExternalp->optionp             = &tconv_charset_cchardet_option_default;
  tconvCharsetExternalp->tconv_charset_newp  = tconv_charset_cchardet_new;
  tconvCharsetExternalp->tconv_charset_runp  = tconv_charset_cchardet_run;
  tconvCharsetExternalp->tconv_charset_freep = tconv_charset_cchardet_free;
#endif
}

/****************************************************************************/
 static TCONV_C_INLINE void _tconvDefaultConvertOption(tconv_t tconvp, tconv_convert_external_t *tconvConvertExternalp)
/****************************************************************************/
{
  static const char funcs[] = "_tconvDefaultConvertOption";

#ifdef TCONV_HAVE_ICUJDD
  TCONV_TRACEF(tconvp, "%s - setting default converter to ICU", funcs);
  tconvConvertExternalp->optionp             = NULL;
  tconvConvertExternalp->tconv_convert_newp  = tconv_convert_ICU_new;
  tconvConvertExternalp->tconv_convert_runp  = tconv_convert_ICU_run;
  tconvConvertExternalp->tconv_convert_freep = tconv_convert_ICU_free;
#else
  #ifdef TCONV_HAVE_ICONV
  TCONV_TRACEF(tconvp, "%s - setting default converter to iconv", funcs);
  tconvConvertExternalp->optionp             = NULL;
  tconvConvertExternalp->tconv_convert_newp  = tconv_convert_iconv_new;
  tconvConvertExternalp->tconv_convert_runp  = tconv_convert_iconv_run;
  tconvConvertExternalp->tconv_convert_freep = tconv_convert_iconv_free;
  #else
  TCONV_TRACEF(tconvp, "%s - setting default converter to unknown", funcs);
  tconvConvertExternalp->optionp             = NULL;
  tconvConvertExternalp->tconv_convert_newp  = NULL;
  tconvConvertExternalp->tconv_convert_runp  = NULL;
  tconvConvertExternalp->tconv_convert_freep = NULL;
  #endif
#endif
}

#ifndef TCONV_NTRACE
/****************************************************************************/
static TCONV_C_INLINE void _tconvTraceCallbackProxy(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/****************************************************************************/
{
  tconv_t tconvp = (tconv_t) userDatavp;

  if (tconvp->traceCallbackp != NULL) {
    tconvp->traceCallbackp(tconvp->traceUserDatavp, msgs);
  }
}
#endif
