#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tconv_runtime.h"

/* For logging */
static char *files = "tconv.c";

/* Internal structure */
struct tconv {
  tconv_opt_t     *optp;
  genericLogger_t *genericLoggerp;   /* When not NULL, it is an --internal-- logger */
  char            *tocodes;
  char            *fromcodes;
};

/* Internal functions */
static TCONV_C_INLINE genericLogger_t *_tconvp2genericLoggerp(struct tconv *tconvp);

#ifndef TCONV_NTRACE
#define TCONV_TRACE(tconvp, msgs)       GENERICLOGGER_TRACE(_tconvp2genericLoggerp(tconvp), msgs)
#define TCONV_TRACEF(tconvp, fmts, ...) GENERICLOGGER_TRACEF(_tconvp2genericLoggerp(tconvp), fmts, __VA_ARGS__)
#define TCONV_ERROR(tconvp, msgs)       GENERICLOGGER_ERROR(_tconvp2genericLoggerp(tconvp), msgs)
#define TCONV_ERRORF(tconvp, fmts, ...) GENERICLOGGER_ERRORF(_tconvp2genericLoggerp(tconvp), fmts, __VA_ARGS__)
#else
#define TCONV_TRACE(tconvp, msgs)
#define TCONV_TRACEF(tconvp, fmts, ...)
#define TCONV_ERROR(tconvp, msgs)
#define TCONV_ERRORF(tconvp, fmts, ...)
#endif

/* All our functions have an err label if necessary */
#define TCONV_MALLOC(tconvp, funcs, ptr, type, size) do {		\
    ptr = (type) malloc(size);						\
    TCONV_TRACEF((tconvp), "[%s/v.%s] malloc(%lld) at %s:%d", funcs, TCONV_VERSION, (unsigned long long) (size), files, __LINE__); \
    if (ptr == NULL) {							\
      TCONV_ERRORF((tconvp), "[%s/v.%s] malloc(%lld) error: %s at %s:%d", funcs, TCONV_VERSION, (unsigned long long) (size), strerror(errno), files, __LINE__); \
      goto err;								\
    }									\
  } while (0)

/****************************************************************************/
tconv_t tconv_open(const char *tocodes, const char *fromcodes)
/****************************************************************************/
{
  return tconv_open_ext(tocodes, fromcodes, NULL);
}

/****************************************************************************/
tconv_opt_t *tconv_opt_default()
/****************************************************************************/
{
  static char funcs[] = "tconv_opt_default";
  tconv_opt_t *optp;

  TCONV_MALLOC(NULL, funcs, optp, tconv_opt_t *, sizeof(tconv_opt_t));

  /* C89 style */

  optp->genericLoggerp                        = NULL;
#if defined(TCONV_HAVE_ICU)
  optp->charset.detectori                     = TCONV_CHARSET_ICU;
#else
  optp->charset.detectori                     = TCONV_CHARSET_CCHARDET;
#endif
  optp->charset.u.ICU.confidencei             = 10;
  optp->charset.u.cchardet.confidencef        = 0.1f;
  optp->charset.u.external.optionp            = NULL;
  optp->charset.u.external.tconv_charset_new  = NULL;
  optp->charset.u.external.tconv_charset      = NULL;
  optp->charset.u.external.tconv_charset_free = NULL;
  optp->charset.u.plugin.optionp              = NULL;
  optp->charset.u.plugin.filenames            = NULL;

#if defined(TCONV_HAVE_ICU)
  optp->convert.converteri                     = TCONV_CONVERT_ICU;
#elsif defined(TCONV_HAVE_ICONV)
  optp->convert.converteri                     = TCONV_CONVERT_ICONV;
#else
  optp->convert.converteri                     = -1;
#endif
  optp->convert.u.ICU.bufSizel                 = 0;
  optp->convert.u.ICU.fromCallbackp            = NULL;
  optp->convert.u.ICU.fromFallbackp            = NULL;
  optp->convert.u.ICU.toCallbackp              = NULL;
  optp->convert.u.ICU.toFallbackp              = NULL;
  optp->convert.u.iconv.bufSizel               = 0;
  optp->convert.u.iconv.translitb              = 0;
  optp->convert.u.iconv.ignoreb                = 0;
  optp->convert.u.external.optionp             = NULL;
  optp->convert.u.external.tconv_convert_open  = 0;
  optp->convert.u.external.tconv_convert       = 0;
  optp->convert.u.external.tconv_convert_close = 0;
  optp->convert.u.plugin.optionp               = NULL;
  optp->convert.u.plugin.filenames             = NULL;

  return optp;

 err:
  return NULL;
}

/****************************************************************************/
tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_opt_t *optp)
/****************************************************************************/
{
  static char      funcs[]            = "tconv_open_ext";
  genericLogger_t *genericLoggerp     = NULL;
  struct tconv    *fakeTconvp         = NULL;
  tconv_t          tconvp;
  struct tconv     fakeTconv;
  tconv_opt_t      fakeOpt;

#ifndef TCONV_NTRACE
  /* We want to be able to log asap: the alloc macros uses tconvp to get */
  /* the logger. If the user provide no logger, fake a tconvp to get one */
  /* with the compile defaults                                           */
  if (optp != NULL) {
    genericLoggerp = optp->genericLoggerp;
  }
  if (genericLoggerp == NULL) {
    genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp != NULL) {
      /* This is dangerous, because we will use a fake thingy, with a lot of */
      /* things unitialized - but we know what we are doing -;               */
      fakeTconvp = &fakeTconv;
      fakeTconvp->optp = &fakeOpt;
      fakeTconvp->optp->genericLoggerp = genericLoggerp;
    }
  }
#endif

  TCONV_MALLOC(fakeTconvp, funcs, tconvp, tconv_t, sizeof(struct tconv));

  tconvp->genericLoggerp = genericLoggerp;
  tconvp->optp           = NULL;
  tconvp->fromcodes      = NULL;
  tconvp->tocodes        = NULL;

  /* We always duplicate the options */
  if (optp != NULL) {
    TCONV_MALLOC(fakeTconvp, funcs, tconvp->optp, tconv_opt_t *, sizeof(tconv_opt_t));
    *(tconvp->optp) = *optp;
  } else {
    tconvp->optp = tconv_opt_default();
    if (tconvp->optp == NULL) {
      goto err;
    }
  }

  /* We always duplicate the charsets */
  if (tocodes != NULL) {
    TCONV_MALLOC(tconvp, funcs, tconvp->tocodes, char *, strlen(tocodes) + 1);
  }
  if (fromcodes != NULL) {
    TCONV_MALLOC(tconvp, funcs, tconvp->fromcodes, char *, strlen(fromcodes) + 1);
  }

  return tconvp;
  
 err:
  {
    int errnol = errno;

    if (genericLoggerp != NULL) {
      GENERICLOGGER_FREE(genericLoggerp);
    }
    if (tconvp != NULL) {
      if (tconvp->optp      != NULL) { free(tconvp->optp); }
      if (tconvp->tocodes   != NULL) { free(tconvp->tocodes); }
      if (tconvp->fromcodes != NULL) { free(tconvp->fromcodes); }
      free(tconvp);
    }

    errno = errnol;
  }
  return NULL;
}

static TCONV_C_INLINE genericLogger_t *_tconvp2genericLoggerp(struct tconv *tconvp)
{
  tconv_opt_t     *optp;
  genericLogger_t *genericLoggerp;

  if (tconvp == NULL) {
    return NULL;
  } else {
    if ((genericLoggerp = tconvp->genericLoggerp) != NULL) {
      return genericLoggerp;
    } else {
      if ((optp = tconvp->optp) != NULL) {
	return optp->genericLoggerp;
      } else {
	return NULL;
      }
    }
  }
}
 

