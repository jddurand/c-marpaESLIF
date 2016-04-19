#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "tconv_runtime.h"

/* For logging */
static char *files = "tconv.c";

/* Internal structure */
struct tconv {
  tconv_opt_t     *optp;
  char            *tocodes;
  char            *fromcodes;
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
    if (tconvp->optp      != NULL) { free(tconvp->optp); }
    if (tconvp->tocodes   != NULL) { free(tconvp->tocodes); }
    if (tconvp->fromcodes != NULL) { free(tconvp->fromcodes); }
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
  optp->convert.u.iconv.bufSizel               = 0;
  optp->convert.u.iconv.translitb              = 0;
  optp->convert.u.iconv.ignoreb                = 0;
  #else
  optp->convert.converteri                     = -1;
  #endif
#endif

  return optp;

 err:
  return NULL;
}

/****************************************************************************/
tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_opt_t *optp)
/****************************************************************************/
{
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
    errno = errnol;
  }
  return NULL;
}

/****************************************************************************/
size_t tconv(tconv_t tconvp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp)
/****************************************************************************/
{
}

