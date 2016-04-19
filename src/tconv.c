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
  tconv_opt_t             *optp;
  char                    *tocodes;
  char                    *fromcodes;
  /* At the end we always end-up in an "external" configuration */
  void                    *sharedLibraryHandlep;
  tconv_charset_external_t charset;
  tconv_convert_external_t convert;
};

/* All our functions have an err label if necessary */
#define TCONV_MALLOC(ptr, type, size) do {                              \
    ptr = (type) malloc(size);						\
    if (ptr == NULL) {							\
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
int tconv_close(tconv_t tconvp)
/****************************************************************************/
{
  if (tconvp != NULL) {
    if (tconvp->optp                 != NULL) { free(tconvp->optp); }
    if (tconvp->tocodes              != NULL) { free(tconvp->tocodes); }
    if (tconvp->fromcodes            != NULL) { free(tconvp->fromcodes); }
    if (tconvp->sharedLibraryHandlep != NULL) { dlclose(tconvp->sharedLibraryHandlep); }
    free(tconvp);
  }

  return 0;
}

/****************************************************************************/
tconv_opt_t *tconv_opt_default()
/****************************************************************************/
{
  tconv_opt_t *optp;

  TCONV_MALLOC(optp, tconv_opt_t *, sizeof(tconv_opt_t));

  /* C89 style */

#if defined(TCONV_HAVE_ICU)
  optp->charset.detectori                     = TCONV_CHARSET_ICU;
  optp->charset.u.ICU.confidencei             = 10;
#else
  optp->charset.detectori                     = TCONV_CHARSET_CCHARDET;
  optp->charset.u.cchardet.confidencef        = 0.1f;
#endif

#if defined(TCONV_HAVE_ICU)
  optp->convert.converteri                     = TCONV_CONVERT_ICU;
  optp->convert.u.ICU.bufSizel                 = 0;
  optp->convert.u.ICU.fromCallbackp            = NULL;
  optp->convert.u.ICU.fromFallbackp            = NULL;
  optp->convert.u.ICU.toCallbackp              = NULL;
  optp->convert.u.ICU.toFallbackp              = NULL;
#else
  #if defined(TCONV_HAVE_ICONV)
  optp->convert.converteri                     = TCONV_CONVERT_ICONV;
  optp->convert.u.iconv.translitb              = 0;
  optp->convert.u.iconv.ignoreb                = 0;
  #else
  optp->convert.converteri                     = -1;
  #endif
#endif
  optp->charset.maxl                           = 1024;

  return optp;

 err:
  return NULL;
}

/****************************************************************************/
tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_opt_t *optp)
/****************************************************************************/
{
  void            *sharedLibraryHandlep = NULL;
  tconv_t          tconvp;

  TCONV_MALLOC(tconvp, tconv_t, sizeof(struct tconv));

  tconvp->optp           = NULL;
  tconvp->fromcodes      = NULL;
  tconvp->tocodes        = NULL;

  /* We always duplicate the options */
  if (optp != NULL) {
    TCONV_MALLOC(tconvp->optp, tconv_opt_t *, sizeof(tconv_opt_t));
    *(tconvp->optp) = *optp;
  } else {
    tconvp->optp = tconv_opt_default();
    if (tconvp->optp == NULL) {
      goto err;
    }
  }

  /* Look to the options - at the end we always end up as if it was an */
  /* "external" thingy.                                                */
  switch (tconvp->optp->convert.converteri) {
  case TCONV_CHARSET_ICU:
    tconvp->charset.optionp             = &(tconvp->optp->charset.u.ICU);
    tconvp->charset.tconv_charset_newp  = tconv_charset_ICU_new;
    tconvp->charset.tconv_charset_runp  = tconv_charset_ICU_run;
    tconvp->charset.tconv_charset_freep = tconv_charset_ICU_free;
    tconvp->sharedLibraryHandlep        = NULL;
    break;
  case TCONV_CHARSET_CCHARDET:
    tconvp->charset.optionp             = &(tconvp->optp->charset.u.cchardet);
    tconvp->charset.tconv_charset_newp  = tconv_charset_cchardet_new;
    tconvp->charset.tconv_charset_runp  = tconv_charset_cchardet_run;
    tconvp->charset.tconv_charset_freep = tconv_charset_cchardet_free;
    tconvp->sharedLibraryHandlep        = NULL;
    break;
  case TCONV_CHARSET_EXTERNAL:
    tconvp->charset                     = tconvp->optp->charset.u.external;
    tconvp->sharedLibraryHandlep        = NULL;
    break;
  case TCONV_CHARSET_PLUGIN:
    /* ------------------------------------------------------------------ */
    /* If the application is calling us with a plugin configuration       */
    /* Up to the application to make this call serialized (mutex)         */
    /* Because in general dlopen/dlclose at least many not be thread-safe */
    /* ------------------------------------------------------------------ */
    sharedLibraryHandlep = dlopen(tconvp->optp->charset.u.plugin.filenames, RTLD_LAZY);
    if (sharedLibraryHandlep == NULL) {
      goto err;
    }
    tconvp->charset.tconv_charset_newp  = dlsym(sharedLibraryHandlep, "tconv_charset_new");
    if (tconvp->charset.tconv_charset_newp == NULL) {
      goto err;
    }
    tconvp->charset.tconv_charset_runp  = dlsym(sharedLibraryHandlep, "tconv_charset_run");
    if (tconvp->charset.tconv_charset_runp == NULL) {
      goto err;
    }
    tconvp->charset.tconv_charset_freep  = dlsym(sharedLibraryHandlep, "tconv_charset_free");
    if (tconvp->charset.tconv_charset_freep == NULL) {
      goto err;
    }
    tconvp->charset.optionp             = tconvp->optp->charset.u.plugin.optionp;
    tconvp->sharedLibraryHandlep        = sharedLibraryHandlep;
    break;
 default:
   errno = EINVAL;
   goto err;
  }

  /* We always duplicate the charsets */
  if (tocodes != NULL) {
    TCONV_MALLOC(tconvp->tocodes, char *, strlen(tocodes) + 1);
  }
  if (fromcodes != NULL) {
    TCONV_MALLOC(tconvp->fromcodes, char *, strlen(fromcodes) + 1);
  }

  return tconvp;
  
 err:
  {
    int errnol = errno;
    tconv_close(tconvp);
    if (sharedLibraryHandlep != NULL) {
      dlclose(sharedLibraryHandlep);
    }
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

