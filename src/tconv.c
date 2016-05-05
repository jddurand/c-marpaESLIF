#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>
#include <genericLogger.h>
#include <stdarg.h>
#include "tconv.h"
#include "tconv_config.h"

/* Maximum size for last error. */
#define TCONV_ERROR_SIZE 1024

/* For logging */
static char *files = "tconv.c";
#define TCONV_ENV_TRACE "TCONV_ENV_TRACE"
static TCONV_C_INLINE void _tconvTraceCallbackProxy(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);

/* Default options for built-ins */
static tconv_charset_ICU_option_t tconv_charset_ICU_option_default = { 10 };
static tconv_charset_ICU_option_t tconv_charset_cchardet_option_default = { 10 };


/* Internal structure */
struct tconv {
  /* 1. trace */
  short                    traceb;
  tconvTraceCallback_t     traceCallbackp;
  void                    *traceUserDatavp;
  genericLogger_t         *genericLoggerp;
  /* 2. encodings */
  char                    *tocodes;
  char                    *fromcodes;
  /* 3. runtime */
  void                    *charsetContextp;
  void                    *convertContextp;
  /* 4. for cleanup */
  void                    *sharedLibraryHandlep;
  /* 5. options - we always end up in an "external"-like configuration */
  tconv_charset_external_t charsetExternal;
  tconv_convert_external_t convertExternal;
  /* 6. last error */
  char                     errors[TCONV_ERROR_SIZE];
};

#define TCONV_MAX(tconvp, literalA, literalB) (((literalA) > (literalB)) ? (literalA) : (literalB))
#define TCONV_MIN(tconvp, literalA, literalB) (((literalA) < (literalB)) ? (literalA) : (literalB))

/* All our functions have an err label if necessary */
#define TCONV_FREE(tconvp, funcs, ptr) do {		\
    TCONV_TRACE((tconvp), "%s - free(%p)", (funcs), (ptr));		\
    free(ptr);								\
    (ptr) = NULL;							\
  } while (0)

#define TCONV_MALLOC(tconvp, funcs, ptr, type, size) do {		\
    TCONV_TRACE((tconvp), "%s - malloc(%lld)", (funcs), (unsigned long long) (size)); \
    (ptr) = (type) malloc(size);					\
    if ((ptr) == NULL) {						\
      TCONV_TRACE((tconvp), "%s - malloc(%lld) failure, %s", (funcs), (unsigned long long) (size), strerror(errno)); \
      goto err;								\
    } else {								\
      TCONV_TRACE((tconvp), "%s - malloc(%lld) success: %p", (funcs), (unsigned long long) (size), (ptr)); \
    }									\
  } while (0)

#define TCONV_REALLOC(tconvp, funcs, ptr, type, size) do {		\
    type tmp;								\
    TCONV_TRACE((tconvp), "%s - realloc(%p, %lld)", (funcs), (ptr), (unsigned long long) (size)); \
    tmp = (type) realloc((ptr), (size));				\
    if (tmp == NULL) {							\
      TCONV_TRACE((tconvp), "%s - realloc(%p, %lld) failure, %s", (funcs), (ptr), (unsigned long long) (size), strerror(errno)); \
      goto err;								\
    } else {								\
      TCONV_TRACE((tconvp), "%s - realloc(%p, %lld) success: %p", (funcs), (ptr), (unsigned long long) (size), (ptr)); \
      (ptr) = tmp;							\
    }									\
  } while (0)

#define TCONV_STRDUP(tconvp, funcs, dst, src) do {			\
    TCONV_TRACE((tconvp), "%s - strdup(\"%s\")", (funcs), (src));	\
    (dst) = strdup(src);						\
    if ((dst) == NULL) {						\
      TCONV_TRACE((tconvp), "%s - strdup(\"%s\") failure, %s", (funcs), (src), strerror(errno)); \
      goto err;								\
    } else {								\
      TCONV_TRACE((tconvp), "%s - strdup(\"%s\") success: %p", (funcs), (src), (dst)); \
    }									\
  } while (0)

static TCONV_C_INLINE void _tconvDefaultCharsetAndConvertOptions(tconv_t tconvp);
static TCONV_C_INLINE void _tconvDefaultCharsetOption(tconv_t tconvp, tconv_charset_external_t *tconvCharsetExternalp);
static TCONV_C_INLINE void _tconvDefaultConvertOption(tconv_t tconvp, tconv_convert_external_t *tconvConvertExternalp);

/****************************************************************************/
TCONV_EXPORT tconv_t tconv_open(const char *tocodes, const char *fromcodes)
/****************************************************************************/
{
  return tconv_open_ext(tocodes, fromcodes, NULL);
}

/****************************************************************************/
TCONV_EXPORT void tconv_trace_on(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "tconv_trace_on";
  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  if (tconvp != NULL) {
    tconvp->traceb = 1;
  }
}

/****************************************************************************/
TCONV_EXPORT void tconv_trace_off(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "tconv_trace_off";
  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  if (tconvp != NULL) {
    tconvp->traceb = 0;
  }
}

/****************************************************************************/
TCONV_EXPORT void tconv_trace(tconv_t tconvp, const char *fmts, ...)
/****************************************************************************/
{
  va_list ap;
  int     errnol;
  
  if ((tconvp != NULL) && (tconvp->traceb != 0) && (tconvp->genericLoggerp != NULL)) {
    /* In any case we do not want errno to change when doing logging */
    errnol = errno;
    va_start(ap, fmts);
    GENERICLOGGER_LOGAP(tconvp->genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE, fmts, ap);
    va_end(ap);
    errno = errnol;
  }
}

/****************************************************************************/
TCONV_EXPORT int tconv_close(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "tconv_close";
  int               rci     = 0;
  genericLogger_t  *genericLoggerp;

  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  if (tconvp != NULL) {
    if (tconvp->charsetContextp != NULL) {
      if (tconvp->charsetExternal.tconv_charset_freep != NULL) {
        TCONV_TRACE(tconvp, "%s - freeing charset engine: %p(%p, %p)", funcs, tconvp->charsetExternal.tconv_charset_freep, tconvp, tconvp->charsetContextp);
        tconvp->charsetExternal.tconv_charset_freep(tconvp, tconvp->charsetContextp);
      }
    }
    if (tconvp->convertContextp != NULL) {
      if (tconvp->convertExternal.tconv_convert_freep != NULL) {
        TCONV_TRACE(tconvp, "%s - freeing convert engine: %p(%p, %p)", funcs, tconvp->convertExternal.tconv_convert_freep, tconvp, tconvp->convertContextp);
        tconvp->convertExternal.tconv_convert_freep(tconvp, tconvp->convertContextp);
      }
    }
    if (tconvp->tocodes != NULL) {
      TCONV_TRACE(tconvp, "%s - freeing copy of \"to\" charset %s", funcs, tconvp->tocodes);
      TCONV_FREE(tconvp, funcs, tconvp->tocodes);
    }
    if (tconvp->fromcodes != NULL) {
      TCONV_TRACE(tconvp, "%s - freeing copy of \"from\" charset %s", funcs, tconvp->fromcodes);
      TCONV_FREE(tconvp, funcs, tconvp->fromcodes);
    }
    if (tconvp->sharedLibraryHandlep != NULL) {
      TCONV_TRACE(tconvp, "%s - closing shared library: dlclose(%p)", funcs, tconvp->sharedLibraryHandlep);
      if (dlclose(tconvp->sharedLibraryHandlep) != 0) {
        TCONV_TRACE(tconvp, "%s - dlclose failure, %s", funcs, dlerror());
        errno = EFAULT;
        rci = -1;
      }
    }
    /* Remember the logger to log the maxiumum as possible */
    genericLoggerp = tconvp->genericLoggerp;
#ifndef TCONV_NTRACE
    if (genericLoggerp != NULL) {
      TCONV_TRACE(tconvp, "%s - prepared to free genericLogger %p", funcs, genericLoggerp);
    }
#endif
    TCONV_TRACE(tconvp, "%s - freeing tconv context", funcs);
    TCONV_FREE(tconvp, funcs, tconvp);

    if (genericLoggerp != NULL) {
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }

  return 0;
}

/****************************************************************************/
TCONV_EXPORT tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_option_t *tconvOptionp)
/****************************************************************************/
{
  static const char funcs[]              = "tconv_open_ext";
  void             *sharedLibraryHandlep = NULL;
  tconv_t           tconvp               = NULL;
  char             *traces               = NULL;

  /* The very initial malloc cannot be done with TCONV_MALLOC */
  tconvp = (tconv_t) malloc(sizeof(struct tconv));
  if (tconvp == NULL) {
    goto err;
  }
  /* Make sure everything is initialized NOW regardless of what is coded after */
  /* 1. trace */
  tconvp->traceb          = 0;
  tconvp->traceCallbackp  = NULL;
  tconvp->traceUserDatavp = NULL;
  tconvp->genericLoggerp  = NULL;
  /* 2. encodings */
  tconvp->tocodes   = NULL;
  tconvp->fromcodes = NULL;
  /* 3. runtime */
  tconvp->charsetContextp = NULL;
  tconvp->convertContextp = NULL;
  /* 4. for cleanup */
  tconvp->sharedLibraryHandlep = NULL;
  /* 5. options - we always end up in an "external"-like configuration */
  /* charsetExternal and convertExternal do not contain malloc thingies */
  /* 6. last error */
  tconvp->errors[0]                    = '\0';
  /* Last byte can never change, because we do an strncpy */
  tconvp->errors[TCONV_ERROR_SIZE - 1] = '\0';

  /* 1. trace */
  traces                       = getenv(TCONV_ENV_TRACE);
  tconvp->traceb               = (traces != NULL) ? (atoi(traces) != 0 ? 1 : 0) : 0;
  if (tconvOptionp != NULL) {
    tconvp->traceCallbackp  = tconvOptionp->traceCallbackp;
    tconvp->traceUserDatavp = tconvOptionp->traceUserDatavp;
    tconvp->genericLoggerp  = genericLogger_newp(_tconvTraceCallbackProxy, tconvp, GENERICLOGGER_LOGLEVEL_TRACE);
  } else {
    tconvp->traceCallbackp  = NULL;
    tconvp->traceUserDatavp = NULL;
    tconvp->genericLoggerp  = NULL;
  }

  /* From now on, we can log */
#ifndef TCONV_NTRACE
  if (tconvp->genericLoggerp != NULL) {
    TCONV_TRACE(tconvp, "%s - genericLogger created: %p ", funcs, tconvp->genericLoggerp);
  }
#endif
  TCONV_TRACE(tconvp, "%s - trace flag set to %d ", funcs, (int) tconvp->traceb);

  /* 2. encodings */
  if (tocodes != NULL) {
    TCONV_STRDUP(tconvp, funcs, tconvp->tocodes, tocodes);
  } else {
    tconvp->tocodes = NULL;
  }
  if (fromcodes != NULL) {
    TCONV_STRDUP(tconvp, funcs, tconvp->fromcodes, fromcodes);
  } else {
    tconvp->fromcodes = NULL;
  }

  /* 3. runtime */
  tconvp->charsetContextp      = NULL;
  tconvp->convertContextp      = NULL;

   /* 4. for cleanup */
  tconvp->sharedLibraryHandlep = NULL;

  /* 5. options - we always end up in an "external"-like configuration */
  if (tconvOptionp != NULL) {

    TCONV_TRACE(tconvp, "%s - validation user option", funcs);

    /* Charset */
    if (tconvOptionp->charsetp != NULL) {

      switch (tconvOptionp->charsetp->charseti) {
      case TCONV_CHARSET_EXTERNAL:
        TCONV_TRACE(tconvp, "%s - charset detector type is external", funcs);
        tconvp->charsetExternal = tconvOptionp->charsetp->u.external;
        break;
      case TCONV_CHARSET_PLUGIN:
        TCONV_TRACE(tconvp, "%s - charset detector type is plugin", funcs);
        if (tconvOptionp->charsetp->u.plugin.filenames == NULL) {
          TCONV_TRACE(tconvp, "%s - null charset plugin filename", funcs);
          errno = EINVAL;
          goto err;
        }
        TCONV_TRACE(tconvp, "%s - opening shared library %s", funcs, tconvOptionp->charsetp->u.plugin.filenames);
        tconvp->sharedLibraryHandlep = dlopen(tconvOptionp->charsetp->u.plugin.filenames, RTLD_LAZY);
        if (tconvp->sharedLibraryHandlep == NULL) {
          TCONV_TRACE(tconvp, "%s - dlopen failure, %s", funcs, dlerror());
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
        TCONV_TRACE(tconvp, "%s - charset detector type is built-in ICU", funcs);
        tconvp->charsetExternal.tconv_charset_newp  = tconv_charset_ICU_new;
        tconvp->charsetExternal.tconv_charset_runp  = tconv_charset_ICU_run;
        tconvp->charsetExternal.tconv_charset_freep = tconv_charset_ICU_free;
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.cchardetOptionp;
        if (tconvp->charsetExternal.optionp == NULL) {
          TCONV_TRACE(tconvp, "%s - setting default charset detector ICU option", funcs);
          tconvp->charsetExternal.optionp = &tconv_charset_ICU_option_default;
        }
#endif
        break;
      case TCONV_CHARSET_CCHARDET:
        TCONV_TRACE(tconvp, "%s - charset detector type is built-in cchardet", funcs);
        tconvp->charsetExternal.tconv_charset_newp  = tconv_charset_cchardet_new;
        tconvp->charsetExternal.tconv_charset_runp  = tconv_charset_cchardet_run;
        tconvp->charsetExternal.tconv_charset_freep = tconv_charset_cchardet_free;
        tconvp->charsetExternal.optionp             = tconvOptionp->charsetp->u.ICUOptionp;
        if (tconvp->charsetExternal.optionp == NULL) {
          TCONV_TRACE(tconvp, "%s - setting default charset detector cchardet option", funcs);
          tconvp->charsetExternal.optionp = &tconv_charset_cchardet_option_default;
        }
        break;
      default:
        TCONV_TRACE(tconvp, "%s - charset detector type is unknown", funcs);
        tconvp->charsetExternal.tconv_charset_newp  = NULL;
        tconvp->charsetExternal.tconv_charset_runp  = NULL;
        tconvp->charsetExternal.tconv_charset_freep = NULL;
        tconvp->charsetExternal.optionp             = NULL;
        break;
      }
      if (tconvp->charsetExternal.tconv_charset_runp == NULL) {
        /* Formally, only the "run" entry point is required */
        TCONV_TRACE(tconvp, "%s - tconv_charset_runp is NULL", funcs);
        errno = EINVAL;
        goto err;
      }
    } else {    
      TCONV_TRACE(tconvp, "%s - setting default charset options", funcs);
      _tconvDefaultCharsetOption(tconvp, &(tconvp->charsetExternal));
    }
    if (tconvOptionp->convertp != NULL) {
      switch (tconvOptionp->convertp->converti) {
      case TCONV_CONVERT_EXTERNAL:
        TCONV_TRACE(tconvp, "%s - converter type is external", funcs);
        tconvp->convertExternal = tconvOptionp->convertp->u.external;
        break;
      case TCONV_CONVERT_PLUGIN:
        TCONV_TRACE(tconvp, "%s - converter type is plugin", funcs);
        if (tconvOptionp->convertp->u.plugin.filenames == NULL) {
          TCONV_TRACE(tconvp, "%s - null convert filename", funcs);
          errno = EINVAL;
          goto err;
        }
        TCONV_TRACE(tconvp, "%s - opening shared library %s", funcs, tconvOptionp->convertp->u.plugin.filenames);
        tconvp->sharedLibraryHandlep = dlopen(tconvOptionp->convertp->u.plugin.filenames, RTLD_LAZY);
        if (tconvp->sharedLibraryHandlep == NULL) {
          TCONV_TRACE(tconvp, "%s - dlopen failure, %s", funcs, dlerror());
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
        TCONV_TRACE(tconvp, "%s - converter type is built-in ICU", funcs);
        tconvp->convertExternal.tconv_convert_newp  = tconv_convert_ICU_new;
        tconvp->convertExternal.tconv_convert_runp  = tconv_convert_ICU_run;
        tconvp->convertExternal.tconv_convert_freep = tconv_convert_ICU_free;
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.ICUOptionp;
#endif
        break;
      case TCONV_CONVERT_ICONV:
#ifdef TCONV_HAVE_ICONV
        TCONV_TRACE(tconvp, "%s - converter type is built-in iconv", funcs);
        tconvp->convertExternal.tconv_convert_newp  = tconv_convert_iconv_new;
        tconvp->convertExternal.tconv_convert_runp  = tconv_convert_iconv_run;
        tconvp->convertExternal.tconv_convert_freep = tconv_convert_iconv_free;
        tconvp->convertExternal.optionp             = tconvOptionp->convertp->u.iconvOptionp;
#endif
        break;
      default:
        TCONV_TRACE(tconvp, "%s - converter type is unknown", funcs);
        tconvp->convertExternal.tconv_convert_newp  = NULL;
        tconvp->convertExternal.tconv_convert_runp  = NULL;
        tconvp->convertExternal.tconv_convert_freep = NULL;
        tconvp->convertExternal.optionp             = NULL;
        break;
      }
      if (tconvp->convertExternal.tconv_convert_runp == NULL) {
        /* Formally, only the "run" entry point is required */
        TCONV_TRACE(tconvp, "%s - tconv_convert_runp is NULL", funcs);
        errno = EINVAL;
        goto err;
      }
    } else {    
      TCONV_TRACE(tconvp, "%s - setting default converter options", funcs);
      _tconvDefaultConvertOption(tconvp, &(tconvp->convertExternal));
    }
  } else {
    _tconvDefaultCharsetAndConvertOptions(tconvp);
  }

  TCONV_TRACE(tconvp, "%s - return %p", funcs, tconvp);
  return tconvp;
  
 err:
  {
    int errnol = errno;
    tconv_close(tconvp);
    errno = errnol;
  }
  return (tconv_t)-1;
}

/****************************************************************************/
TCONV_EXPORT size_t tconv(tconv_t tconvp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/****************************************************************************/
{
  static const char funcs[]         = "tconv";
  void             *charsetContextp = NULL;
  void             *charsetOptionp  = NULL;
  char             *fromcodes       = NULL;
  void             *convertContextp = NULL;
  void             *convertOptionp  = NULL;
  size_t            rcl;

  TCONV_TRACE(tconvp, "%s(%p, %p, %p, %p, %p)", funcs, tconvp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);

  if (tconvp == NULL) {
    errno = EINVAL;
    goto err;
  }

  if ((tconvp->fromcodes == NULL) &&
      (inbufsp != NULL)           &&
      (*inbufsp != NULL)          &&
      (inbytesleftlp != NULL)     &&
      (*inbytesleftlp > 0)) {
    
    /* it is legal to have no new() for charset engine, but if there is one */
    /* it must return something */
    if (tconvp->charsetExternal.tconv_charset_newp != NULL) {
      charsetOptionp = tconvp->charsetExternal.optionp;
      TCONV_TRACE(tconvp, "%s - initializing charset detection: %p(%p, %p)", funcs, tconvp->charsetExternal.tconv_charset_newp, tconvp, charsetOptionp);
      tconvp->errors[0] = '\0';
      charsetContextp = tconvp->charsetExternal.tconv_charset_newp(tconvp, charsetOptionp);
      if (charsetContextp == NULL) {
	goto err;
      }
    }
    tconvp->charsetContextp = charsetContextp;
    TCONV_TRACE(tconvp, "%s - calling charset detection: %p(%p, %p, %p, %p)", funcs, tconvp->charsetExternal.tconv_charset_runp, tconvp, charsetContextp, *inbufsp, *inbytesleftlp);
    tconvp->errors[0] = '\0';
    fromcodes = tconvp->charsetExternal.tconv_charset_runp(tconvp, charsetContextp, *inbufsp, *inbytesleftlp);
    if (fromcodes == NULL) {
      goto err;
    }
    TCONV_TRACE(tconvp, "%s - charset detection returned %s", funcs, fromcodes);
    TCONV_STRDUP(tconvp, funcs, tconvp->fromcodes, fromcodes);
    strcpy(tconvp->fromcodes, fromcodes);
    if (tconvp->charsetExternal.tconv_charset_freep != NULL) {
      TCONV_TRACE(tconvp, "%s - ending charset detection: %p(%p, %p)", funcs, tconvp->charsetExternal.tconv_charset_freep, tconvp, charsetContextp);
      tconvp->charsetExternal.tconv_charset_freep(tconvp, charsetContextp);
    }
    tconvp->charsetContextp = NULL;
  }

  if ((tconvp->tocodes == NULL) && (tconvp->fromcodes != NULL)) {
    TCONV_TRACE(tconvp, "%s - duplicating from charset \"%s\" into \"to\" charset", funcs, tconvp->fromcodes);
    TCONV_STRDUP(tconvp, funcs, tconvp->tocodes, tconvp->fromcodes);
  }

  if (tconvp->convertContextp == NULL) {
    /* Initialize converter context if not already done */
    /* it is legal to have no new() for convert engine, but if there is one */
    /* it must return something */
    if (tconvp->convertExternal.tconv_convert_newp != NULL) {
      convertOptionp = tconvp->convertExternal.optionp;
      TCONV_TRACE(tconvp, "%s - initializing convert engine: %p(%p, %p, %p, %p)", funcs, tconvp->convertExternal.tconv_convert_newp, tconvp, tconvp->tocodes, tconvp->fromcodes, convertOptionp);
      tconvp->errors[0] = '\0';
      convertContextp = tconvp->convertExternal.tconv_convert_newp(tconvp, tconvp->tocodes, tconvp->fromcodes, convertOptionp);
      if (convertContextp == (void *)-1) {
	goto err;
      }
    }
    tconvp->convertContextp = convertContextp;
  } else {
    convertContextp = tconvp->convertContextp;
  }

  TCONV_TRACE(tconvp, "%s - calling convert engine: %p(%p, %p, %p, %p, %p, %p)", funcs, tconvp->convertExternal.tconv_convert_runp, tconvp, convertContextp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  tconvp->errors[0] = '\0';
  rcl = tconvp->convertExternal.tconv_convert_runp(tconvp, convertContextp, inbufsp, inbytesleftlp, outbufsp, outbytesleftlp);
  if (rcl == (size_t)-1) {
    goto err;
  }
  
  TCONV_TRACE(tconvp, "%s - return %lld", funcs, (signed long long) rcl);
  return rcl;

 err:
  if (tconvp->errors[0] == '\0') {
    tconv_error_set(tconvp, strerror(errno));
  }
  return (size_t)-1;
}

/****************************************************************************/
static TCONV_C_INLINE void _tconvDefaultCharsetAndConvertOptions(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "_tconvDefaultCharsetAndConvertOptions";

  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  _tconvDefaultCharsetOption(tconvp, &(tconvp->charsetExternal));
  _tconvDefaultConvertOption(tconvp, &(tconvp->convertExternal));
}

/****************************************************************************/
static TCONV_C_INLINE void _tconvDefaultCharsetOption(tconv_t tconvp, tconv_charset_external_t *tconvCharsetExternalp)
/****************************************************************************/
{
  static const char funcs[] = "_tconvDefaultCharsetOption";

  TCONV_TRACE(tconvp, "%s(%p, %p)", funcs, tconvp, tconvCharsetExternalp);

  /*
#ifdef TCONV_HAVE_ICU
  TCONV_TRACE(tconvp, "%s - setting default charset detector to ICU", funcs);
  tconvCharsetExternalp->optionp             = &tconv_charset_ICU_option_default;
  tconvCharsetExternalp->tconv_charset_newp  = tconv_charset_ICU_new;
  tconvCharsetExternalp->tconv_charset_runp  = tconv_charset_ICU_run;
  tconvCharsetExternalp->tconv_charset_freep = tconv_charset_ICU_free;
#else
  */
  TCONV_TRACE(tconvp, "%s - setting default charset detector to cchardet", funcs);
  tconvCharsetExternalp->optionp             = &tconv_charset_cchardet_option_default;
  tconvCharsetExternalp->tconv_charset_newp  = tconv_charset_cchardet_new;
  tconvCharsetExternalp->tconv_charset_runp  = tconv_charset_cchardet_run;
  tconvCharsetExternalp->tconv_charset_freep = tconv_charset_cchardet_free;
  /*
#endif
  */
}

/****************************************************************************/
 static TCONV_C_INLINE void _tconvDefaultConvertOption(tconv_t tconvp, tconv_convert_external_t *tconvConvertExternalp)
/****************************************************************************/
{
  static const char funcs[] = "_tconvDefaultConvertOption";

  TCONV_TRACE(tconvp, "%s(%p, %p)", funcs, tconvp, tconvConvertExternalp);

#ifdef TCONV_HAVE_ICU
  TCONV_TRACE(tconvp, "%s - setting default converter to ICU", funcs);
  tconvConvertExternalp->optionp             = NULL;
  tconvConvertExternalp->tconv_convert_newp  = tconv_convert_ICU_new;
  tconvConvertExternalp->tconv_convert_runp  = tconv_convert_ICU_run;
  tconvConvertExternalp->tconv_convert_freep = tconv_convert_ICU_free;
#else
  #ifdef TCONV_HAVE_ICONV
  TCONV_TRACE(tconvp, "%s - setting default converter to iconv", funcs);
  tconvConvertExternalp->optionp             = NULL;
  tconvConvertExternalp->tconv_convert_newp  = tconv_convert_iconv_new;
  tconvConvertExternalp->tconv_convert_runp  = tconv_convert_iconv_run;
  tconvConvertExternalp->tconv_convert_freep = tconv_convert_iconv_free;
  #else
  TCONV_TRACE(tconvp, "%s - setting default converter to unknown", funcs);
  tconvConvertExternalp->optionp             = NULL;
  tconvConvertExternalp->tconv_convert_newp  = NULL;
  tconvConvertExternalp->tconv_convert_runp  = NULL;
  tconvConvertExternalp->tconv_convert_freep = NULL;
  #endif
#endif
}

/****************************************************************************/
static TCONV_C_INLINE void _tconvTraceCallbackProxy(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/****************************************************************************/
{
  tconv_t tconvp = (tconv_t) userDatavp;

  if (tconvp->traceCallbackp != NULL) {
    tconvp->traceCallbackp(tconvp->traceUserDatavp, msgs);
  }
}

/****************************************************************************/
TCONV_EXPORT char *tconv_error_set(tconv_t tconvp, const char *msgs)
/****************************************************************************/
{
  static const char funcs[] = "tconv_error_set";
  char             *errors  = NULL;
  int               errnol = errno;  /* Make sure errno is never changed */

  TCONV_TRACE(tconvp, "%s(%p, %p)", funcs, tconvp, msgs);

  if (tconvp != NULL) {
    /* This is making sure that errors[TCONV_ERROR_SIZE - 1] is never touched */
    strncpy(tconvp->errors, msgs, TCONV_ERROR_SIZE - 1);
    errors = tconvp->errors;
  }

#ifndef TCONV_NTRACE
  if (errors != NULL) {
    TCONV_TRACE(tconvp, "%s - return %s", funcs, errors);
  } else {
    TCONV_TRACE(tconvp, "%s - return NULL", funcs);
  }
#endif

  errno = errnol;

  return errors;
}

/****************************************************************************/
TCONV_EXPORT char *tconv_error(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[] = "tconv_error";
  char             *errors  = NULL;

  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  if (tconvp != NULL) {
    errors = tconvp->errors;
  }

#ifndef TCONV_NTRACE
  if (errors != NULL) {
    TCONV_TRACE(tconvp, "%s - return %s", funcs, errors);
  } else {
    TCONV_TRACE(tconvp, "%s - return NULL", funcs);
  }
#endif

  return errors;
}

/****************************************************************************/
TCONV_EXPORT char *tconv_fromcode(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[]   = "tconv_fromcode";
  char             *fromcodes = NULL;

  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  if (tconvp != NULL) {
    fromcodes = tconvp->fromcodes;
  }

#ifndef TCONV_NTRACE
  if (fromcodes != NULL) {
    TCONV_TRACE(tconvp, "%s - return %s", funcs, fromcodes);
  } else {
    TCONV_TRACE(tconvp, "%s - return NULL", funcs);
  }
#endif

  return fromcodes;
}
/****************************************************************************/
TCONV_EXPORT char *tconv_tocode(tconv_t tconvp)
/****************************************************************************/
{
  static const char funcs[]   = "tconv_tocode";
  char             *tocodes = NULL;

  TCONV_TRACE(tconvp, "%s(%p)", funcs, tconvp);

  if (tconvp != NULL) {
    tocodes = tconvp->tocodes;
  }

#ifndef TCONV_NTRACE
  if (tocodes != NULL) {
    TCONV_TRACE(tconvp, "%s - return %s", funcs, tocodes);
  } else {
    TCONV_TRACE(tconvp, "%s - return NULL", funcs);
  }
#endif

  return tocodes;
}
