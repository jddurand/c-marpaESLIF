#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <iconv.h>
#include <assert.h>

/* iconv interface is a direct proxy to iconv */

/* ==================================================== */
/* Some iconv implementations are buggy when source and */
/* charset categories are the same.                     */
/* In such a case our implementation is doing much more */
/* work by forcing an intermediate representation,      */
/* using an intermediate encoding that can encode all   */
/* characters. Namely: UTF-8.                           */
/*                                                      */
/* Also some iconv returns EINVAL in this case. We try  */
/* with the intermediate representation as well in this */
/* case.                                                */
/*                                                      */
/* The difficulty is about the families.                */
/* We take the list from ICU to derive a list of        */
/* "categories". This is particularly usefull when the  */
/* input is an alias. If the category lookup fails, we  */
/* fallback to simple name comparison.                  */
/* ==================================================== */

#include "tconv/convert/iconv.h"
#include "tconv_config.h"
#include "alias.c"

/* Context */
#define TCONV_ICONV_INITIAL_SIZE 4096
typedef struct tconv_convert_iconv_context {
  iconv_t     iconvp;           /* iconv itself */
  short       toutf8b;          /* 1 when destination charset is UTF-8 */
  short       intermediateb;    /* Use an intermerdiate UTF-8 charset */
  /* ----------------------------------------------- */
  /* When intermediateb is a true value it is        */
  /* guaranteed that fromutf8b and toutf8b are == 0  */
  /* ----------------------------------------------- */
  /* input   ->     UTF-8     ->    output           */
  /*               buffer                            */
  /*     iconvfromp       iconvtop                   */
  /* ----------------------------------------------- */
  iconv_t     iconvfromp;       /* "from" -> "UTF-8" iconv instance */
  char       *internals;        /* Internal buffers */
  size_t      internall;        /* Internal buffers length */
  char       *internalp;        /* Internal buffers current position */
  iconv_t     iconvtop;         /* "UTF-8" -> "to" iconv instance */
} tconv_convert_iconv_context_t;

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
#define ICONV_SECOND_ARGUMENT const
#else
#define ICONV_SECOND_ARGUMENT
#endif

#undef  ICONV_IS_NOT_TRUSTABLE
#define ICONV_IS_NOT_TRUSTABLE   /* Sorry guys, but what don't you use ICU instead of iconv... ? */

static inline size_t tconv_convert_iconv_directl(tconv_t tconvp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
static inline size_t tconv_convert_iconv_internalflushl(tconv_t tconvp, tconv_convert_iconv_context_t *contextp, char **outbufpp, size_t *outbytesleftp);
static inline size_t tconv_convert_iconv_intermediatel(tconv_t tconvp, tconv_convert_iconv_context_t *contextp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
static inline char  *tconv_convert_iconv_charset_normalizeds(tconv_t tconvp, const char *charsets);
static inline char  *tconv_convert_iconv_charset_lookupp(tconv_t tconvp, const char *charsets, char ***categoriespp);
#ifdef ICONV_IS_NOT_TRUSTABLE
/* When ICONV_IS_NOT_TRUSTABLE is defined, we always want to check the production:
   If output is UTF-8 we check the output, else we create an intermediate UTF-8 buffer and check it
*/
static inline size_t tconv_convert_iconv_wrapperl(tconv_t tconvp, iconv_t iconvp, short utf8b, ICONV_SECOND_ARGUMENT char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp);
static inline int    tconv_valid_utf(unsigned char *string, size_t length, size_t *erroroffset);
#endif

#ifndef ICONV_CAN_IGNORE
#define TCONV_ICONV_IGNORE   "//IGNORE"
#endif

#ifndef ICONV_CAN_TRANSLIT
#define TCONV_ICONV_TRANSLIT   "//TRANSLIT"
#endif

/* C.f. https://www.ibm.com/developerworks/aix/library/au-endianc/ */
static const int i_for_is_bigendian = 1;
short is_bigendian;
#define TCONV_SYSTEM_IS_BIGENDIAN() ( (*(char*)&i_for_is_bigendian) == 0 )

#define TCONV_UPPERCASE(p) do {			\
    char *s = (char *) p;			\
  						\
    while (*s != '\0') {			\
      if ((*s >= 'a' && *s <= 'z')) {		\
	*s = (char) toupper((int) *s);		\
      }						\
      s++;					\
    }						\
  } while (0)

/* The two possible values for internal charset */
static char *UTF8s  = "UTF-8";
static char *UTF32s = "UTF-32";

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char               funcs[]         = "tconv_convert_iconv_new";
  short                           cancheckb       = ((tocodes != NULL) && (fromcodes != NULL));
  char                           *tonormaliseds   = NULL;
  char                           *fromnormaliseds = NULL;
  iconv_t                         iconvp          = (iconv_t)-1;
  short                           intermediateb   = 0;
  iconv_t                         iconvfromp      = (iconv_t)-1;
  iconv_t                         iconvtop        = (iconv_t)-1;
  char                           *internals       = NULL;
  size_t                          internall       = TCONV_ICONV_INITIAL_SIZE;
  tconv_convert_iconv_context_t  *contextp        = NULL;
  short                           fromutf8b       = -1; /* Default value -1 means unknown */
  short                           toutf8b         = -1; /* Default value -1 means unknown */
  char                          **tocategoriespp;
  char                          **fromcategoriespp;
  int                             i;
  
  if (cancheckb) {
#ifndef ICONV_CAN_IGNORE
    {
      short  ignoreb = 0;
      char  *ignorep;
      char  *endIgnorep;

      if (tocodes != NULL) {
        ignorep = strstr(tocodes, TCONV_ICONV_IGNORE);
        if (ignorep != NULL) {
          endIgnorep = ignorep + strlen(TCONV_ICONV_IGNORE);
          if ((*endIgnorep == '\0') || (*(endIgnorep + 1) == '/')) {
            ignoreb = 1;
          }
        }
      }

      if (ignoreb) {
        /* If true, it is guaranteed that tocodes is != NULL. */
        /* It is ALSO guaranteed that tocodes is on the heap: we can modify it */
        TCONV_TRACE(tconvp, "%s - iconv implementation does not accept //IGNORE: removing it", funcs);
        *ignorep = '\0';
        if (*endIgnorep != '\0') {
          memmove(ignorep, endIgnorep, strlen(endIgnorep) + 1); /* Copy the NUL byte */
        }
        TCONV_TRACE(tconvp, "%s - destination encoding is now %s", funcs, tocodes);
      }
    }
#endif
#ifndef ICONV_CAN_TRANSLIT
    {
      short  translitb = 0;
      char  *translitp;
      char  *endTranslitp;

      if (tocodes != NULL) {
        translitp = strstr(tocodes, TCONV_ICONV_TRANSLIT);
        if (translitp != NULL) {
          endTranslitp = translitp + strlen(TCONV_ICONV_TRANSLIT);
          if ((*endTranslitp == '\0') || (*(endTranslitp + 1) == '/')) {
            translitb = 1;
          }
        }
      }

      if (translitb) {
        /* If true, it is guaranteed that tocodes is != NULL. */
        /* It is ALSO guaranteed that tocodes is on the heap: we can modify it */
        TCONV_TRACE(tconvp, "%s - iconv implementation does not accept //TRANSLIT: removing it", funcs);
        *translitp = '\0';
        if (*endTranslitp != '\0') {
          memmove(translitp, endTranslitp, strlen(endTranslitp) + 1); /* Copy the NUL byte */
        }
        TCONV_TRACE(tconvp, "%s - destination encoding is now %s", funcs, tocodes);
      }
    }
#endif
  }

  /* Normalized version should never be NULL - though categories can be */
  tonormaliseds = tconv_convert_iconv_charset_lookupp(tconvp, tocodes, &tocategoriespp);
  if (tonormaliseds == NULL) {
    goto err;
  }
  fromnormaliseds = tconv_convert_iconv_charset_lookupp(tconvp, fromcodes, &fromcategoriespp);
  if (fromnormaliseds == NULL) {
    goto err;
  }

  /* Check if source category is UTF-8 */
  if (fromcategoriespp != NULL) {
    fromutf8b = 0;
    for (i = 0; i < TCONV_ICONV_MAX_CATEGORY; i++) {
      if (fromcategoriespp[i] == NULL) {
        break;
      }
      if (strcmp(fromcategoriespp[i], "UTF-8") == 0) {
        fromutf8b = 1;
        break;
      }
    }
  }
  TCONV_TRACE(tconvp, "%s - %s category mode is %s", funcs, fromcodes, (fromutf8b == 1) ? "UTF-8" : ((fromutf8b == 0) ? "not UTF-8" : "unknown"));

  /* Check if destination category is UTF-8 */
  if (tocategoriespp != NULL) {
    toutf8b = 0;
    for (i = 0; i < TCONV_ICONV_MAX_CATEGORY; i++) {
      if (tocategoriespp[i] == NULL) {
        break;
      }
      if (strcmp(tocategoriespp[i], "UTF-8") == 0) {
        toutf8b = 1;
        break;
      }
    }
  }
  TCONV_TRACE(tconvp, "%s - %s category mode is %s", funcs, tocodes, (toutf8b == 1) ? "UTF-8" : ((toutf8b == 0) ? "not UTF-8" : "unknown"));

  /* Upper case charsets - on some OSes this is needed - remember charsets on the heap (and they are not a const char * const ptr) */
  TCONV_UPPERCASE(tocodes);
  TCONV_UPPERCASE(fromcodes);

#ifdef ICONV_IS_NOT_TRUSTABLE
  if (toutf8b != 1) {
    TCONV_TRACE(tconvp, "%s - forcing an UTF-8 intermediate buffer", funcs);
    goto force_intermediate;
  }
#endif

  TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, tocodes, fromcodes);
  iconvp = iconv_open(tocodes, fromcodes);
  TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\") returns %p", funcs, tocodes, fromcodes, iconvp);
  if (iconvp == (iconv_t)-1) {
    /* On some implementation, for example solaris8, this can fail when we fall */
    /* into categories that have the same number of bytes. We can be sure of what */
    /* we do only when both destination and source categories are determined */
    /* and none of them is UTF-8 */
    if ((fromutf8b == 0) && (toutf8b == 0)) {
      TCONV_TRACE(tconvp, "%s - iconv_open() failure - trying with an UTF-8 intermediate buffer", funcs);
#ifdef ICONV_IS_NOT_TRUSTABLE
    force_intermediate:
#endif
      intermediateb = 1;

      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, tocodes, "UTF-8");
      iconvtop = iconv_open(tocodes, "UTF-8");
      if (iconvtop == NULL) {
	goto err;
      }
      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\") returned %p", funcs, tocodes, "UTF-8", iconvtop);

      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, "UTF-8", fromcodes);
      iconvfromp = iconv_open("UTF-8", fromcodes);
      if (iconvfromp == NULL) {
	goto err;
      }
      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\") returned %p", funcs, "UTF-8", fromcodes, iconvfromp);

      TCONV_TRACE(tconvp, "%s - malloc(%ld)", funcs, (unsigned long) TCONV_ICONV_INITIAL_SIZE);
      internals = (char *) malloc(internall);
      if (internals == NULL) {
	goto err;
      }
    } else {
      goto err;
    }
  }

  /* Setup the context */
  TCONV_TRACE(tconvp, "%s - malloc(%ld)", funcs, (unsigned long) sizeof(tconv_convert_iconv_context_t));
  contextp = (tconv_convert_iconv_context_t *) malloc(sizeof(tconv_convert_iconv_context_t));
  if (contextp == NULL) {
    goto err;
  }
  TCONV_TRACE(tconvp, "%s - contextp is %p", funcs, contextp);

  contextp->iconvp             = iconvp;
  contextp->toutf8b            = toutf8b;
  contextp->intermediateb      = intermediateb;
  contextp->iconvfromp         = iconvfromp;
  contextp->internals          = internals;
  contextp->internall          = internall;
  contextp->internalp          = internals;
  contextp->iconvtop           = iconvtop;

  goto done;

 err:
  if (iconvp != (iconv_t)-1) {
    TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, iconvp);
    iconv_close(iconvp);
  }
  if (iconvfromp != (iconv_t)-1) {
    TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvfromp);
    iconv_close(iconvfromp);
  }
  if (iconvtop != (iconv_t)-1) {
    TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, iconvtop);
    iconv_close(iconvtop);
  }
  if (internals != NULL) {
    TCONV_TRACE(tconvp, "%s - free(%p)", funcs, internals);
    free(internals);
  }

 done:
  if (tonormaliseds != NULL) {
    free(tonormaliseds);
  }
  if (fromnormaliseds != NULL) {
    free(fromnormaliseds);
  }

  TCONV_TRACE(tconvp, "%s - return %p", funcs, contextp);
  return contextp;
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *voidp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char               funcs[]             = "tconv_convert_iconv_run";
  tconv_convert_iconv_context_t  *contextp            = (tconv_convert_iconv_context_t *) voidp;
  size_t                          rcl;

  if (contextp->intermediateb) {
    TCONV_TRACE(tconvp, "%s - tconv_convert_iconv_intermediatel(%p, %p, %p, %p, %p, %p)", funcs, tconvp, contextp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = tconv_convert_iconv_intermediatel(tconvp, contextp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  } else {
#ifdef ICONV_IS_NOT_TRUSTABLE
    TCONV_TRACE(tconvp, "%s - tconv_convert_iconv_wrapperl(%p, %p, %d, %p, %p, %p, %p)", funcs, tconvp, contextp->iconvp, (int) contextp->toutf8b, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = tconv_convert_iconv_wrapperl(tconvp, contextp->iconvp, contextp->toutf8b, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    TCONV_TRACE(tconvp, "%s - tconv_convert_iconv_wrapperl(%p, %p, %d, %p, %p, %p, %p) returns %ld", funcs, tconvp, contextp->iconvp, contextp->toutf8b, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp, (long) rcl);
#else
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, contextp->iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = iconv(contextp->iconvp, (ICONV_SECOND_ARGUMENT char **) inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p) returns %ld", funcs, contextp->iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp, (long) rcl);
#endif
  }

  return rcl;
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char              funcs[] = "tconv_convert_iconv_free";
  tconv_convert_iconv_context_t *contextp = (tconv_convert_iconv_context_t *) voidp;
  short                          errb;

  if (contextp == NULL) {
    errno = EINVAL;
    goto err;
  } else {
    errb = 0;
    if (contextp->iconvp != (iconv_t)-1) {
      TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvp);
      if (iconv_close(contextp->iconvp) != 0) {
        errb = 1;
      }
    }
    if (contextp->iconvfromp != (iconv_t)-1) {
      TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvfromp);
      if (iconv_close(contextp->iconvfromp) != 0) {
        errb = 1;
      }
    }
    if (contextp->iconvtop != (iconv_t)-1) {
      TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvtop);
      if (iconv_close(contextp->iconvtop) != 0) {
        errb = 1;
      }
    }
    if (contextp->internals != NULL) {
      TCONV_TRACE(tconvp, "%s - free(%p)", funcs, contextp->internals);
      free(contextp->internals);
    }
    TCONV_TRACE(tconvp, "%s - free(contextp = %p)", funcs, contextp);
    free(contextp);
  }

  return errb ? -1 : 0;

 err:
  return -1;
}

/*****************************************************************************/
static inline size_t tconv_convert_iconv_directl(tconv_t tconvp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_directl";

  /* C.f. https://dev.openwrt.org/browser/packages/libs/libiconv/src/iconv.c?rev=24777&order=name */
  size_t len = 0;

  if ((inbufpp == NULL) || (*inbufpp == NULL)) {
    if ((outbufpp != NULL) && (*outbufpp != NULL)) {
      TCONV_TRACE(tconvp, "%s - Flush: no shift sequence - return 0", funcs);
      return 0;
    } else {
      TCONV_TRACE(tconvp, "%s - Back to initial state - return 0", funcs);
      return 0;
    }
  }

  /* From there it is illegal that outbufpp is NULL or *outbufpp is NULL */
  assert((outbufpp != NULL) && (*outbufpp != NULL));

  if ((inbytesleftlp  == NULL) || (*inbytesleftlp  <= 0) ||
      (outbytesleftlp == NULL) || (*outbytesleftlp <= 0)) {
    TCONV_TRACE(tconvp, "%s - Nothing to do - return 0", funcs);
    return 0;
  }

  len = (*inbytesleftlp > *outbytesleftlp) ? *outbytesleftlp : *inbytesleftlp;
  memcpy(*outbufpp, *inbufpp, len);

  *inbufpp        += len;
  *inbytesleftlp  -= len;
  *outbufpp       += len;
  *outbytesleftlp -= len;

  if (*inbytesleftlp > 0) {
    TCONV_TRACE(tconvp, "%s - E2BIG - return -1", funcs);
    errno = E2BIG;
    return (size_t)(-1);
  }

  TCONV_TRACE(tconvp, "%s - All is well - return 0", funcs);
  return (size_t)(0);
}

#define TCONV_ICONV_E2BIG_MANAGER(workbufp, workbytesleftl, bufs, bufl) do { \
    char   *tmps;                                                       \
    size_t  tmpl;                                                       \
    size_t  incl;                                                       \
    size_t  deltal;                                                     \
                                                                        \
    TCONV_TRACE(tconvp, "%s - E2BIG manager - %s=%p, %s=%ld, %s=%p, %s=%ld", funcs, #workbufp, workbufp, #workbytesleftl, (unsigned long) workbytesleftl, #bufs, bufs, #bufl, (unsigned long) bufl); \
    if (bufs == NULL) {                                                 \
      tmpl = TCONV_ICONV_INITIAL_SIZE;                                  \
      incl = TCONV_ICONV_INITIAL_SIZE;                                  \
      TCONV_TRACE(tconvp, "%s - malloc(%ld)", funcs, (unsigned long) TCONV_ICONV_INITIAL_SIZE); \
      tmps = (char *) malloc(TCONV_ICONV_INITIAL_SIZE);                 \
    } else {                                                            \
      tmpl = bufl * 2;                                                  \
      incl = bufl;                                                      \
      if (tmpl < bufl) {                                                \
        TCONV_TRACE(tconvp, "%s - size_t flip", funcs);                 \
        errno = ERANGE;                                                 \
        rcl = (size_t)-1;                                               \
        goto err;                                                       \
      }                                                                 \
      TCONV_TRACE(tconvp, "%s - realloc(%p, %ld)", funcs, bufs, (unsigned long) tmpl); \
      tmps = (char *) realloc(bufs, tmpl);                              \
    }                                                                   \
    if (tmps == NULL) {                                                 \
      rcl = (size_t)-1;                                                 \
      goto err;                                                         \
    }                                                                   \
    deltal = bufl - workbytesleftl;                                     \
    bufs = tmps;                                                        \
    bufl = tmpl;                                                        \
    workbufp = bufs + deltal;                                           \
    workbytesleftl += incl;                                             \
    TCONV_TRACE(tconvp, "%s - %s is now %p, length %ld", funcs, #bufs, bufs, (unsigned long) bufl); \
  } while (0)

/*****************************************************************************/
static inline size_t tconv_convert_iconv_internalflushl(tconv_t tconvp, tconv_convert_iconv_context_t *contextp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char  funcs[]             = "tconv_convert_iconv_internalflushl";
  char              *tmpinbufp;
  size_t             tmpinbytesleftl;
  size_t             origtmpinbytesleftl;
  size_t             consumedl;
  size_t             rcl;

  if (contextp->internalp > contextp->internals) {
    tmpinbufp           = contextp->internals;
    tmpinbytesleftl     = contextp->internalp - contextp->internals;
    origtmpinbytesleftl = tmpinbytesleftl;

#ifdef ICONV_IS_NOT_TRUSTABLE
    TCONV_TRACE(tconvp, "%s - remains %ld bytes to flush - tconv_convert_iconv_wrapperl(%p, %p, %d, %p, %p, %p, %p)", funcs, (unsigned long) tmpinbytesleftl, tconvp, contextp->iconvtop, (int) contextp->toutf8b, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
    rcl = tconv_convert_iconv_wrapperl(tconvp, contextp->iconvtop, contextp->toutf8b, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
    TCONV_TRACE(tconvp, "%s - remains %ld bytes to flush - tconv_convert_iconv_wrapperl(%p, %p, %d, %p, %p, %p, %p) returns %ld", funcs, (unsigned long) tmpinbytesleftl, tconvp, contextp->iconvtop, (int) contextp->toutf8b, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp, (long) rcl);
#else
    TCONV_TRACE(tconvp, "%s - remains %ld bytes to flush - iconv(%p, %p, %p, %p, %p)", funcs, (unsigned long) tmpinbytesleftl, contextp->iconvtop, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
    rcl = iconv(contextp->iconvtop, (ICONV_SECOND_ARGUMENT char **) &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
    TCONV_TRACE(tconvp, "%s - remains %ld bytes to flush - iconv(%p, %p, %p, %p, %p) returns %d", funcs, (unsigned long) tmpinbytesleftl, contextp->iconvtop, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp, (long) rcl);
#endif

    /* Whatever happened, forget the bytes that were converted into user's output buffer */
    consumedl = origtmpinbytesleftl - tmpinbytesleftl;
    if (consumedl < origtmpinbytesleftl) {
      /* Some bytes were consumed, but not all */
      memmove(contextp->internals, contextp->internals + consumedl, tmpinbytesleftl);
      contextp->internalp -= consumedl;
    } else {
      /* All bytes were consumed */
      contextp->internalp = contextp->internals;
    }
#ifndef TCONV_NTRACE
    if (rcl == (size_t)-1) {
      TCONV_TRACE(tconvp, "%s - errno %d (%s)", funcs, errno, strerror(errno));
    }
#endif
  } else {
    rcl = 0;
  }

  return rcl;
}

/*****************************************************************************/
static inline size_t tconv_convert_iconv_intermediatel(tconv_t tconvp, tconv_convert_iconv_context_t *contextp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char  funcs[]  = "tconv_convert_iconv_intermediatel";
  size_t             rcl;
  int                errnoi;
  size_t             internalleftl;

  /* A special case is then the user is asking for a reset to initial state without flush. */
  if (((inbufpp == NULL) || (*inbufpp == NULL)) && ((outbufpp == NULL) || (*outbufpp == NULL))) {
    TCONV_TRACE(tconvp, "%s - ...->%s - iconv(%p, NULL, NULL, NULL, NULL)", funcs, "UTF-8", contextp->iconvfromp);
    rcl = iconv(contextp->iconvfromp, NULL, NULL, NULL, NULL);
    TCONV_TRACE(tconvp, "%s - ...->%s - iconv(%p, NULL, NULL, NULL, NULL) returns %ld", funcs, "UTF-8", contextp->iconvfromp, (long) rcl);

    TCONV_TRACE(tconvp, "%s - %s->... - iconv(%p, NULL, NULL, NULL, NULL)", funcs, "UTF-8", contextp->iconvtop);
    rcl = iconv(contextp->iconvtop, NULL, NULL, NULL, NULL);
    TCONV_TRACE(tconvp, "%s - %s->... - iconv(%p, NULL, NULL, NULL, NULL) returns %ld", funcs, "UTF-8", contextp->iconvtop, (long) rcl);

    TCONV_TRACE(tconvp, "%s - reset internal buffer backlog", funcs);
    contextp->internalp = contextp->internals;
    return 0;
  }

  /* We always want to flush previous state */
  if (tconv_convert_iconv_internalflushl(tconvp, contextp, outbufpp, outbytesleftlp) == (size_t)-1) {
    return (size_t)-1;
  }

  /* Here it is guaranteed that the internal buffer can be used entirely */
  contextp->internalp = contextp->internals;
  internalleftl       = contextp->internall;
  while (1) {
#ifdef ICONV_IS_NOT_TRUSTABLE
    TCONV_TRACE(tconvp, "%s - ...->%s - tconv_convert_iconv_wrapperl(%p, %p, %d, %p, %p, %p, %p)", funcs, "UTF-8", tconvp, contextp->iconvfromp, 1, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl);
    rcl = tconv_convert_iconv_wrapperl(tconvp, contextp->iconvfromp, 1, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl);
    TCONV_TRACE(tconvp, "%s - ...->%s - tconv_convert_iconv_wrapperl(%p, %p, %d, %p, %p, %p, %p) returns %ld", funcs, "UTF-8", tconvp, contextp->iconvfromp, 1, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl, (long) rcl);
#else
    TCONV_TRACE(tconvp, "%s - ...->%s - iconv(%p, %p, %p, %p, %p)", funcs, "UTF-8", contextp->iconvfromp, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl);
    rcl = iconv(contextp->iconvfromp,
                (ICONV_SECOND_ARGUMENT char **) inbufpp,
                inbytesleftlp,
                &(contextp->internalp),
                &internalleftl);
    TCONV_TRACE(tconvp, "%s - ...->%s - iconv(%p, %p, %p, %p, %p) returns %ld", funcs, "UTF-8", contextp->iconvfromp, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl, (long) rcl);
#endif
    errnoi = errno;
#ifndef TCONV_NTRACE
    /* Note that TCONV_TRACE is guaranteed to not alter errno */
    if (rcl == (size_t)-1) {
      TCONV_TRACE(tconvp, "%s - errno %d (%s)", funcs, errnoi, strerror(errno));
    }
#endif
    if ((rcl == (size_t)-1) && (errno == E2BIG)) {
      TCONV_ICONV_E2BIG_MANAGER(contextp->internalp, internalleftl, contextp->internals, contextp->internall);
    } else {
      break;
    }
  }

  /* We always want to flush current state */
  if (tconv_convert_iconv_internalflushl(tconvp, contextp, outbufpp, outbytesleftlp) == (size_t)-1) {
    return (size_t)-1;
  }

  if (rcl == (size_t)-1) {
    errno = errnoi;
  }
  return rcl;

 err:
  return (size_t)-1;
}

/*****************************************************************************/
static inline char *tconv_convert_iconv_charset_normalizeds(tconv_t tconvp, const char *charsets)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_charset_normalizeds";
  char             *normalizeds;
  char             *p;
  char             *q;
  unsigned char     c;

  if (charsets == NULL) {
    TCONV_TRACE(tconvp, "%s - null charset - return NULL", funcs);
    errno = EINVAL;
    return NULL;
  }

  normalizeds = malloc(strlen(charsets) + 1);
  TCONV_TRACE(tconvp, "%s - malloc(\"%s\") returns %p", funcs, charsets, normalizeds);
  if (normalizeds == NULL) {
    TCONV_TRACE(tconvp, "%s - return NULL", funcs);
    return NULL;
  }

  p = (char *) charsets;
  q = normalizeds;
  
  while ((c = (unsigned char) *p++) != '\0') {
    /*
     *
     * An alias is used for name matching:
     * - case-insensitive
     - '-', '_' and ' ' are ignored
     *
     */
         if ((c >= 'A' && c<= 'Z')) { *q++ = (char) tolower((int) c); }
    else if ((c >= 'a' && c<= 'z')) { *q++ = c; }
    else if ((c >= '0' && c<= '9')) { *q++ = c; }
    else if  (c == '+')             { *q++ = c; }
    else if  (c == '.')             { *q++ = c; }
    else if  (c == ':')             { *q++ = c; }
    else if  (c == '/')             { break;    } /* iconv specific extensions, means end of the charset in any case */
  };

  *q = '\0';

  TCONV_TRACE(tconvp, "%s - return \"%s\"", funcs, normalizeds);
  return normalizeds;
}

/*****************************************************************************/
static inline char *tconv_convert_iconv_charset_lookupp(tconv_t tconvp, const char *charsets, char ***categoriespp)
/*****************************************************************************/
{
  static const char             funcs[] = "tconv_convert_iconv_charset_lookupp";
  tconv_iconv_alias2category_t *alias2categoryp = alias2category;
  char                         *normalizeds;
  int                           i;

  normalizeds = tconv_convert_iconv_charset_normalizeds(tconvp, charsets);
  if (normalizeds != NULL) {
    for (i = 0; i < TCONV_ICONV_NB_ALIAS; alias2categoryp++, i++) {
      if (strcmp(alias2categoryp->alias, normalizeds) == 0) {
        *categoriespp = alias2categoryp->categoriesp;
#ifndef TCONV_NTRACE
        for (i = 0; i < TCONV_ICONV_MAX_CATEGORY; i++) {
          if  (alias2categoryp->categoriesp[i] == NULL) {
            break;
          }
          TCONV_TRACE(tconvp, "%s - normalized charset %s associated to category %s", funcs, normalizeds, alias2categoryp->categoriesp[i]);
        }
#endif
        TCONV_TRACE(tconvp, "%s - return \"%s\" (*categoriespp = %p)", funcs, normalizeds, *categoriespp);
        return normalizeds;
      }
    }
    *categoriespp = NULL;
    TCONV_TRACE(tconvp, "%s - return \"%s\" (*categoriespp = NULL)", funcs, normalizeds);
    return normalizeds;
  } else {
    TCONV_TRACE(tconvp, "%s - return NULL", funcs);
    return NULL;
  }
}

#ifdef ICONV_IS_NOT_TRUSTABLE
/*****************************************************************************/
static inline size_t tconv_convert_iconv_wrapperl(tconv_t tconvp, iconv_t iconvp, short utf8b, ICONV_SECOND_ARGUMENT char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
/* This method should be called ONLY when destination charset is UTF-8.      */
/*****************************************************************************/
{
  static const char  funcs[] = "tconv_convert_iconv_wrapperl";
  char              *orig_outbufp;
  char              *max_outbufp;
  size_t             rcl;
  int                rci;
  unsigned long      uint32;
  size_t             erroroffset;

  /* We are only interested by the case of real production, not flush, nor reset, nor when then is no output buffer, nor when destination is not UTF-8 */
  if ((utf8b != 1) || (inbufpp == NULL) || (*inbufpp == NULL) || (outbufpp == NULL) || (*outbufpp == NULL)) {
    TCONV_TRACE(tconvp, "%s - return iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    return iconv(iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  }

  orig_outbufp = *outbufpp;

  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  rcl = iconv(iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p) returns %ld", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp, (long) rcl);

  if (rcl == (size_t)-1) {
    return rcl;
  }

  max_outbufp = *outbufpp;

  if (tconv_valid_utf((unsigned char *) orig_outbufp, (size_t) (max_outbufp - orig_outbufp), &erroroffset) != 0) {
    TCONV_TRACE(tconvp, "%s - output has a malformed UTF-8 character at offset %lld", funcs, (unsigned long long) erroroffset);
    rcl = (size_t)-1;
    errno = EILSEQ;
  }

  TCONV_TRACE(tconvp, "%s - return %ld", funcs, (long) rcl);
  return rcl;
}

/*************************************************
*           Validate a UTF string                *
*************************************************/
/* This is a copy of PCRE2's valid_utf source */

/* This function is called (optionally) at the start of compile or match, to
check that a supposed UTF string is actually valid. The early check means
that subsequent code can assume it is dealing with a valid string. The check
can be turned off for maximum performance, but the consequences of supplying an
invalid string are then undefined.

Arguments:
  string       points to the string
  length       length of string
  errp         pointer to an error position offset variable

Returns:       == 0    if the string is a valid UTF string
               != 0    otherwise, setting the offset of the bad character
*/

/* Error codes for UTF-8 validity checks */

#define PCRE2_ERROR_UTF8_ERR1        (-3)
#define PCRE2_ERROR_UTF8_ERR2        (-4)
#define PCRE2_ERROR_UTF8_ERR3        (-5)
#define PCRE2_ERROR_UTF8_ERR4        (-6)
#define PCRE2_ERROR_UTF8_ERR5        (-7)
#define PCRE2_ERROR_UTF8_ERR6        (-8)
#define PCRE2_ERROR_UTF8_ERR7        (-9)
#define PCRE2_ERROR_UTF8_ERR8       (-10)
#define PCRE2_ERROR_UTF8_ERR9       (-11)
#define PCRE2_ERROR_UTF8_ERR10      (-12)
#define PCRE2_ERROR_UTF8_ERR11      (-13)
#define PCRE2_ERROR_UTF8_ERR12      (-14)
#define PCRE2_ERROR_UTF8_ERR13      (-15)
#define PCRE2_ERROR_UTF8_ERR14      (-16)
#define PCRE2_ERROR_UTF8_ERR15      (-17)
#define PCRE2_ERROR_UTF8_ERR16      (-18)
#define PCRE2_ERROR_UTF8_ERR17      (-19)
#define PCRE2_ERROR_UTF8_ERR18      (-20)
#define PCRE2_ERROR_UTF8_ERR19      (-21)
#define PCRE2_ERROR_UTF8_ERR20      (-22)
#define PCRE2_ERROR_UTF8_ERR21      (-23)

static const unsigned char utf8_table4[] = {
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5 };


static inline int tconv_valid_utf(unsigned char *string, size_t length, size_t *erroroffset)
{
  unsigned char * p;
  unsigned long c;

  /* ----------------- Check a UTF-8 string ----------------- */

  /* Originally, this function checked according to RFC 2279, allowing for values
     in the range 0 to 0x7fffffff, up to 6 bytes long, but ensuring that they were
     in the canonical format. Once somebody had pointed out RFC 3629 to me (it
     obsoletes 2279), additional restrictions were applied. The values are now
     limited to be between 0 and 0x0010ffff, no more than 4 bytes long, and the
     subrange 0xd000 to 0xdfff is excluded. However, the format of 5-byte and 6-byte
     characters is still checked. Error returns are as follows:

     PCRE2_ERROR_UTF8_ERR1   Missing 1 byte at the end of the string
     PCRE2_ERROR_UTF8_ERR2   Missing 2 bytes at the end of the string
     PCRE2_ERROR_UTF8_ERR3   Missing 3 bytes at the end of the string
     PCRE2_ERROR_UTF8_ERR4   Missing 4 bytes at the end of the string
     PCRE2_ERROR_UTF8_ERR5   Missing 5 bytes at the end of the string
     PCRE2_ERROR_UTF8_ERR6   2nd-byte's two top bits are not 0x80
     PCRE2_ERROR_UTF8_ERR7   3rd-byte's two top bits are not 0x80
     PCRE2_ERROR_UTF8_ERR8   4th-byte's two top bits are not 0x80
     PCRE2_ERROR_UTF8_ERR9   5th-byte's two top bits are not 0x80
     PCRE2_ERROR_UTF8_ERR10  6th-byte's two top bits are not 0x80
     PCRE2_ERROR_UTF8_ERR11  5-byte character is not permitted by RFC 3629
     PCRE2_ERROR_UTF8_ERR12  6-byte character is not permitted by RFC 3629
     PCRE2_ERROR_UTF8_ERR13  4-byte character with value > 0x10ffff is not permitted
     PCRE2_ERROR_UTF8_ERR14  3-byte character with value 0xd800-0xdfff is not permitted
     PCRE2_ERROR_UTF8_ERR15  Overlong 2-byte sequence
     PCRE2_ERROR_UTF8_ERR16  Overlong 3-byte sequence
     PCRE2_ERROR_UTF8_ERR17  Overlong 4-byte sequence
     PCRE2_ERROR_UTF8_ERR18  Overlong 5-byte sequence (won't ever occur)
     PCRE2_ERROR_UTF8_ERR19  Overlong 6-byte sequence (won't ever occur)
     PCRE2_ERROR_UTF8_ERR20  Isolated 0x80 byte (not within UTF-8 character)
     PCRE2_ERROR_UTF8_ERR21  Byte with the illegal value 0xfe or 0xff
  */

  for (p = string; length > 0; p++)
    {
      unsigned long ab, d;

      c = *p;
      length--;

      if (c < 128) continue;                /* ASCII character */

      if (c < 0xc0)                         /* Isolated 10xx xxxx byte */
        {
          *erroroffset = (size_t)(p - string);
          return PCRE2_ERROR_UTF8_ERR20;
        }

      if (c >= 0xfe)                        /* Invalid 0xfe or 0xff bytes */
        {
          *erroroffset = (size_t)(p - string);
          return PCRE2_ERROR_UTF8_ERR21;
        }

      ab = utf8_table4[c & 0x3f];     /* Number of additional bytes (1-5) */
      if (length < ab)                      /* Missing bytes */
        {
          *erroroffset = (size_t)(p - string);
          switch(ab - length)
            {
            case 1: return PCRE2_ERROR_UTF8_ERR1;
            case 2: return PCRE2_ERROR_UTF8_ERR2;
            case 3: return PCRE2_ERROR_UTF8_ERR3;
            case 4: return PCRE2_ERROR_UTF8_ERR4;
            case 5: return PCRE2_ERROR_UTF8_ERR5;
            }
        }
      length -= ab;                         /* Length remaining */

      /* Check top bits in the second byte */

      if (((d = *(++p)) & 0xc0) != 0x80)
        {
          *erroroffset = (int)(p - string) - 1;
          return PCRE2_ERROR_UTF8_ERR6;
        }

      /* For each length, check that the remaining bytes start with the 0x80 bit
         set and not the 0x40 bit. Then check for an overlong sequence, and for the
         excluded range 0xd800 to 0xdfff. */

      switch (ab)
        {
          /* 2-byte character. No further bytes to check for 0x80. Check first byte
             for for xx00 000x (overlong sequence). */

        case 1: if ((c & 0x3e) == 0)
            {
              *erroroffset = (int)(p - string) - 1;
              return PCRE2_ERROR_UTF8_ERR15;
            }
          break;

          /* 3-byte character. Check third byte for 0x80. Then check first 2 bytes
             for 1110 0000, xx0x xxxx (overlong sequence) or
             1110 1101, 1010 xxxx (0xd800 - 0xdfff) */

        case 2:
          if ((*(++p) & 0xc0) != 0x80)     /* Third byte */
            {
              *erroroffset = (int)(p - string) - 2;
              return PCRE2_ERROR_UTF8_ERR7;
            }
          if (c == 0xe0 && (d & 0x20) == 0)
            {
              *erroroffset = (int)(p - string) - 2;
              return PCRE2_ERROR_UTF8_ERR16;
            }
          if (c == 0xed && d >= 0xa0)
            {
              *erroroffset = (int)(p - string) - 2;
              return PCRE2_ERROR_UTF8_ERR14;
            }
          break;

          /* 4-byte character. Check 3rd and 4th bytes for 0x80. Then check first 2
             bytes for for 1111 0000, xx00 xxxx (overlong sequence), then check for a
             character greater than 0x0010ffff (f4 8f bf bf) */

        case 3:
          if ((*(++p) & 0xc0) != 0x80)     /* Third byte */
            {
              *erroroffset = (int)(p - string) - 2;
              return PCRE2_ERROR_UTF8_ERR7;
            }
          if ((*(++p) & 0xc0) != 0x80)     /* Fourth byte */
            {
              *erroroffset = (int)(p - string) - 3;
              return PCRE2_ERROR_UTF8_ERR8;
            }
          if (c == 0xf0 && (d & 0x30) == 0)
            {
              *erroroffset = (int)(p - string) - 3;
              return PCRE2_ERROR_UTF8_ERR17;
            }
          if (c > 0xf4 || (c == 0xf4 && d > 0x8f))
            {
              *erroroffset = (int)(p - string) - 3;
              return PCRE2_ERROR_UTF8_ERR13;
            }
          break;

          /* 5-byte and 6-byte characters are not allowed by RFC 3629, and will be
             rejected by the length test below. However, we do the appropriate tests
             here so that overlong sequences get diagnosed, and also in case there is
             ever an option for handling these larger code points. */

          /* 5-byte character. Check 3rd, 4th, and 5th bytes for 0x80. Then check for
             1111 1000, xx00 0xxx */

        case 4:
          if ((*(++p) & 0xc0) != 0x80)     /* Third byte */
            {
              *erroroffset = (int)(p - string) - 2;
              return PCRE2_ERROR_UTF8_ERR7;
            }
          if ((*(++p) & 0xc0) != 0x80)     /* Fourth byte */
            {
              *erroroffset = (int)(p - string) - 3;
              return PCRE2_ERROR_UTF8_ERR8;
            }
          if ((*(++p) & 0xc0) != 0x80)     /* Fifth byte */
            {
              *erroroffset = (int)(p - string) - 4;
              return PCRE2_ERROR_UTF8_ERR9;
            }
          if (c == 0xf8 && (d & 0x38) == 0)
            {
              *erroroffset = (int)(p - string) - 4;
              return PCRE2_ERROR_UTF8_ERR18;
            }
          break;

          /* 6-byte character. Check 3rd-6th bytes for 0x80. Then check for
             1111 1100, xx00 00xx. */

        case 5:
          if ((*(++p) & 0xc0) != 0x80)     /* Third byte */
            {
              *erroroffset = (int)(p - string) - 2;
              return PCRE2_ERROR_UTF8_ERR7;
            }
          if ((*(++p) & 0xc0) != 0x80)     /* Fourth byte */
            {
              *erroroffset = (int)(p - string) - 3;
              return PCRE2_ERROR_UTF8_ERR8;
            }
          if ((*(++p) & 0xc0) != 0x80)     /* Fifth byte */
            {
              *erroroffset = (int)(p - string) - 4;
              return PCRE2_ERROR_UTF8_ERR9;
            }
          if ((*(++p) & 0xc0) != 0x80)     /* Sixth byte */
            {
              *erroroffset = (int)(p - string) - 5;
              return PCRE2_ERROR_UTF8_ERR10;
            }
          if (c == 0xfc && (d & 0x3c) == 0)
            {
              *erroroffset = (int)(p - string) - 5;
              return PCRE2_ERROR_UTF8_ERR19;
            }
          break;
        }

      /* Character is valid under RFC 2279, but 4-byte and 5-byte characters are
         excluded by RFC 3629. The pointer p is currently at the last byte of the
         character. */

      if (ab > 3)
        {
          *erroroffset = (int)(p - string) - ab;
          return (ab == 4)? PCRE2_ERROR_UTF8_ERR11 : PCRE2_ERROR_UTF8_ERR12;
        }
    }
  return 0;
}
#endif
