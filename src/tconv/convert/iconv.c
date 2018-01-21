#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <iconv.h>
#include <assert.h>

/* iconv interface is a direct proxy to iconv */

/* ==================================================== */
/* Some iconv implementations are buggy when source and */
/* charset families are the same.                       */
/* In such a case our implementation is doing much more */
/* work by forcing an intermediate representation,      */
/* using an intermediate encoding that can encode all   */
/* characters. Namely: UTF-8 or UTF-32.                 */
/*                                                      */
/* Also some iconv returns EINVAL in this case. We try  */
/* with the intermediate representation as well in this */
/* case.                                                */
/*                                                      */
/* When source and destination charset families are the */
/* same:                                                */
/* - UTF-8 intermediate encoding is used if destination */
/*   charset is not UTF-8, or                           */
/* - UTF-32 intermediate encoding                       */
/* The implementation is simply calling recursively     */
/* tconv.                                               */
/*                                                      */
/* The difficulty is about the families...              */
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
  iconv_t  iconvp;        /* iconv itself */
  short    intermediateb; /* Use an intermerdiate charset */
  short    toutf8b;       /* destination is UTF-8 */

  /* --------------------------------------------- */
  /* When intermediateb is a true value            */
  /* --------------------------------------------- */
  /* input   ->  internal  -> output               */
  /*     iconvfromp       iconvtop                 */
  /* --------------------------------------------- */
  iconv_t  iconvfromp;    /* iconv "from" instance */
  char    *internals;     /* Internal buffers */
  short    internalutf8b; /* Internal buffer is UTF-8 */
  size_t   internall;     /* Internal buffers length */
  char    *internalp;     /* Internal buffers current position */
  iconv_t  iconvtop;      /* iconv "to" instance */
} tconv_convert_iconv_context_t;

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
#define ICONV_SECOND_ARGUMENT const
#else
#define ICONV_SECOND_ARGUMENT
#endif

static inline size_t tconv_convert_iconv_directl(tconv_t tconvp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
static inline size_t tconv_convert_iconv_internalflushl(tconv_t tconvp, tconv_convert_iconv_context_t *contextp, char **outbufpp, size_t *outbytesleftp);
static inline size_t tconv_convert_iconv_intermediatel(tconv_t tconvp, tconv_convert_iconv_context_t *contextp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
static inline char  *tconv_convert_iconv_charset_normalizeds(tconv_t tconvp, const char *charsets);
static inline char  *tconv_convert_iconv_charset_lookupp(tconv_t tconvp, const char *charsets, char ***categoriespp);
#ifdef ICONV_IS_NOT_TRUSTABLE
static inline size_t tconv_convert_iconv_check(tconv_t tconvp, iconv_t iconvp, short toutf8b, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp);
static inline int    tconv_utf82ordi(tconv_uint8_t *utf8bytes, tconv_uint32_t *uint32p);
static inline short  tconv_utf8_ok(tconv_t tconvp, tconv_uint8_t *utf8bytes, size_t lengthl);
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

/*****************************************************************************/
void  *tconv_convert_iconv_new(tconv_t tconvp, const char *tocodes, const char *fromcodes, void *voidp)
/*****************************************************************************/
{
  static const char               funcs[]            = "tconv_convert_iconv_new";
  short                           cancheckb          = ((tocodes != NULL) && (fromcodes != NULL));
  char                           *tonormaliseds      = NULL;
  char                           *fromnormaliseds    = NULL;
  iconv_t                         iconvp             = NULL;
  short                           samecategoryb      = 0;
  short                           forceintermediateb = 0;
  iconv_t                         iconvfromp         = NULL;
  iconv_t                         iconvtop           = NULL;
  char                           *internals          = NULL;
  size_t                          internall          = TCONV_ICONV_INITIAL_SIZE;
  tconv_convert_iconv_context_t  *contextp           = NULL;
  short                           toutf8b;
  short                           internalutf8b;
  char                          **tocategoriespp;
  char                          **fromcategoriespp;
  char                           *intermediatecharsets;
  int                             i;
  int                             j;
  
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

    /* Normalized version should never be NULL - though categories can be */
    tonormaliseds = tconv_convert_iconv_charset_lookupp(tconvp, tocodes, &tocategoriespp);
    if (tonormaliseds == NULL) {
      goto err;
    }
    fromnormaliseds = tconv_convert_iconv_charset_lookupp(tconvp, fromcodes, &fromcategoriespp);
    if (fromnormaliseds == NULL) {
      goto err;
    }
    if (tocategoriespp != NULL) {
      for (i = 0; i < TCONV_ICONV_MAX_CATEGORY; i++) {
        if (tocategoriespp[i] == NULL) {
          break;
        }
        if (strcmp(tocategoriespp[i], "UTF-8") == 0) {
          toutf8b = 1;
          break;
        }
      }
      if ((fromcategoriespp != NULL)) {
        for (i = 0; i < TCONV_ICONV_MAX_CATEGORY; i++) {
          if (tocategoriespp[i] == NULL) {
            break;
          }
          for (j = 0; j < TCONV_ICONV_MAX_CATEGORY; j++) {
            if (fromcategoriespp[j] == NULL) {
              break;
            }
            if (strcmp(tocategoriespp[i], fromcategoriespp[j]) == 0) {
              samecategoryb = 1;
              break;
            }
          }
        }
        TCONV_TRACE(tconvp, "%s - same category mode is %s", funcs, samecategoryb ? "on" : "off");
      }
    }
    if (samecategoryb) {
    forceintermediate:
      /* Determine the intermediate encoding: UTF-8 or UTF-32 */
      TCONV_TRACE(tconvp, "%s - %s category is %sUTF-8", funcs, forceintermediateb ? "destination" : "same", toutf8b ? "" : "not ");
      if (toutf8b) {
        intermediatecharsets = "UTF-32";
        internalutf8b        = 0;
      } else {
        intermediatecharsets = "UTF-8";
        internalutf8b        = 1;
      }

      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, intermediatecharsets, fromcodes);
      iconvfromp = iconv_open(intermediatecharsets, fromcodes);
      if (iconvfromp == NULL) {
        goto err;
      }
      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\") returned %p", funcs, intermediatecharsets, fromcodes, iconvfromp);

      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, tocodes, intermediatecharsets);
      iconvtop = iconv_open(tocodes, intermediatecharsets);
      if (iconvtop == NULL) {
        goto err;
      }
      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\") returned %p", funcs, tocodes, intermediatecharsets, iconvtop);

      TCONV_TRACE(tconvp, "%s - malloc(%ld)", funcs, (unsigned long) TCONV_ICONV_INITIAL_SIZE);
      internals = (char *) malloc(internall);
      if (internals == NULL) {
        goto err;
      }
    } else {
      /* Categories lookup failed, or succeeded but say the charsets are not in the same category: normal iconv */
      TCONV_TRACE(tconvp, "%s - iconv_open(\"%s\", \"%s\")", funcs, tocodes, fromcodes);
      iconvp = iconv_open(tocodes, fromcodes);
      if (iconvp == NULL) {
        /* On some implementation, for example solaris8, this can fail when we fall */
        /* into categories that have the same number of bytes. We can be sure of what */
        /* we do only when the destination category is found (i.e. toutf8b is set) */
        if (tocategoriespp != NULL) {
          forceintermediateb = 1;
          goto forceintermediate;
        } else {
          goto err;
        }
      }
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
  contextp->intermediateb      = samecategoryb || forceintermediateb;
  contextp->toutf8b            = toutf8b;
  contextp->iconvfromp         = iconvfromp;
  contextp->internals          = internals;
  contextp->internalutf8b      = internalutf8b;
  contextp->internall          = internall;
  contextp->internalp          = internals;
  contextp->iconvtop           = iconvtop;

  goto done;

 err:
  if (iconvp != NULL) {
    TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, iconvp);
    iconv_close(iconvp);
  }
  if (iconvfromp != NULL) {
    TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvfromp);
    iconv_close(iconvfromp);
  }
  if (iconvtop != NULL) {
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
    TCONV_TRACE(tconvp, "%s - tconv_convert_iconv_check(%p, %p, %d, %p, %p, %p, %p)", funcs, tconvp, contextp->iconvp, (int) contextp->toutf8b, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = tconv_convert_iconv_check(tconvp, contextp->iconvp, contextp->toutf8b, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
#else
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, contextp->iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = iconv(contextp->iconvp, (ICONV_SECOND_ARGUMENT char **) inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
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
    if (contextp->iconvp != NULL) {
      TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvp);
      if (iconv_close(contextp->iconvp) != 0) {
        errb = 1;
      }
    }
    if (contextp->iconvfromp != NULL) {
      TCONV_TRACE(tconvp, "%s - iconv_close(%p)", funcs, contextp->iconvfromp);
      if (iconv_close(contextp->iconvfromp) != 0) {
        errb = 1;
      }
    }
    if (contextp->iconvtop != NULL) {
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

    TCONV_TRACE(tconvp, "%s - remains %ld bytes to flush", funcs, (unsigned long) tmpinbytesleftl);
#ifdef ICONV_IS_NOT_TRUSTABLE
    TCONV_TRACE(tconvp, "%s - tconv_convert_iconv_check(%p, %p, %d, %p, %p, %p, %p)", funcs, tconvp, contextp->iconvtop, (int) contextp->toutf8b, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
    rcl = tconv_convert_iconv_check(tconvp, contextp->iconvtop, !contextp->internalutf8b, &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
#else
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, contextp->iconvtop, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = iconv(contextp->iconvtop, (ICONV_SECOND_ARGUMENT char **) &tmpinbufp, &tmpinbytesleftl, outbufpp, outbytesleftlp);
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
      TCONV_TRACE(tconvp, "%s - iconv on output returned -1, errno %d", funcs, errno);
    } else {
      TCONV_TRACE(tconvp, "%s - iconv on output returned %ld", funcs, (unsigned long) rcl);
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
    TCONV_TRACE(tconvp, "%s - iconv(%p, NULL, NULL, NULL, NULL)", funcs, contextp->iconvfromp);
    iconv(contextp->iconvfromp, NULL, NULL, NULL, NULL);
    TCONV_TRACE(tconvp, "%s - iconv(%p, NULL, NULL, NULL, NULL)", funcs, contextp->iconvtop);
    iconv(contextp->iconvtop, NULL, NULL, NULL, NULL);
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
    TCONV_TRACE(tconvp, "%s - tconv_convert_iconv_check(%p, %p, %d, %p, %p, %p, %p)", funcs, tconvp, contextp->iconvfromp, (int) contextp->internalutf8b, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl);
    rcl = tconv_convert_iconv_check(tconvp, contextp->iconvfromp, contextp->internalutf8b, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl);
#else
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, contextp->iconvfromp, inbufpp, inbytesleftlp, &(contextp->internalp), &internalleftl);
    rcl = iconv(contextp->iconvfromp,
                (ICONV_SECOND_ARGUMENT char **) inbufpp,
                inbytesleftlp,
                &(contextp->internalp),
                &internalleftl);
#endif
    errnoi = errno;
#ifndef TCONV_NTRACE
    /* Note that TCONV_TRACE is guaranteed to not alter errno */
    if (rcl == (size_t)-1) {
      TCONV_TRACE(tconvp, "%s - iconv on input returned -1, errno %d (%s)", funcs, errnoi, strerror(errno));
    } else {
      TCONV_TRACE(tconvp, "%s - iconv on input returned %ld", funcs, (unsigned long) rcl);
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
    TCONV_TRACE(tconvp, "%s - EINVAL - return NULL", funcs);
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
static inline size_t tconv_convert_iconv_check(tconv_t tconvp, iconv_t iconvp, short toutf8b, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char    funcs[] = "tconv_convert_iconv_check";
  char   **orig_inbufpp        = inbufpp;
  size_t  *orig_inbytesleftlp  = inbytesleftlp;
  char   **orig_outbufpp       = outbufpp;
  size_t  *orig_outbytesleftlp = outbytesleftlp;

  char    *inbufp              = (inbufpp        != NULL) ? *inbufpp        : NULL;
  size_t   inbytesleftl        = (inbytesleftlp  != NULL) ? *inbytesleftlp  : 0;
  char    *outbufp             = (outbufpp       != NULL) ? *outbufpp       : NULL;
  size_t   outbytesleftl       = (outbytesleftlp != NULL) ? *outbytesleftlp : 0;

  size_t   rcl;
  size_t   producedl;

  /* We are ONLY concerned by the case of normal character conversion to UTF-8 - not flush nor state initialization */
  if ((! toutf8b) || (inbufp == NULL) || (outbufp == NULL)) {
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = iconv(iconvp, (ICONV_SECOND_ARGUMENT char **) inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  } else {
    inbufpp        = &inbufp;
    inbytesleftlp  = &inbytesleftl;
    outbufpp       = &outbufp;
    outbytesleftlp = &outbytesleftl;

    /* Okay, so we are producing characters to UTF-8 charset and the configure says this is not reliable */
    TCONV_TRACE(tconvp, "%s - iconv(%p, %p, %p, %p, %p)", funcs, iconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    rcl = iconv(iconvp, (ICONV_SECOND_ARGUMENT char **) inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);

    producedl = (size_t) (outbufp - *orig_outbufpp);
    TCONV_TRACE(tconvp, "%s - checking UTF-8 correctness on %ld bytes", funcs, (unsigned long) producedl);
    if (producedl > 0) {
      if (! tconv_utf8_ok(tconvp, (tconv_uint8_t *) *orig_outbufpp, producedl)) {
        rcl = (size_t)-1;
        errno = EILSEQ;
      }
    }

    if (orig_inbufpp != NULL) {
      *orig_inbufpp = inbufp;
    }
    if (orig_inbytesleftlp != NULL) {
      *orig_inbytesleftlp = inbytesleftl;
    }
    if (orig_outbufpp != NULL) {
      *orig_outbufpp = outbufp;
    }
    if (orig_outbytesleftlp != NULL) {
      *orig_outbytesleftlp = outbytesleftl;
    }
  }

  return rcl;
}

/*****************************************************************************/
static inline short tconv_utf8_ok(tconv_t tconvp, tconv_uint8_t *utf8bytes, size_t lengthl)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_utf8_ok";
  tconv_uint8_t    *p       = utf8bytes;
  tconv_uint8_t    *maxp    = utf8bytes + lengthl;
  short             rcb     = 1;
  int               rci;
  tconv_uint32_t    uint32;

  while (p < maxp) {
    rci = tconv_utf82ordi(p, &uint32);
    if (rci <= 0) {
      TCONV_TRACE(tconvp, "%s - failure at offset %ld", funcs, (unsigned long) (p - utf8bytes));
      rcb = 0;
      break;
    }
    p += rci;
  }

  return rcb;
}

/*****************************************************************************/
static inline int tconv_utf82ordi(tconv_uint8_t *utf8bytes, tconv_uint32_t *uint32p)
/*****************************************************************************/
/* This is a copy of utf2ord from pcre2test.c
-----------------------------------------------------------------------------
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the University of Cambridge nor the names of its
      contributors may be used to endorse or promote products derived from
      this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/
/* This function reads one or more bytes that represent a UTF-8 character,
and returns the codepoint of that character. Note that the function supports
the original UTF-8 definition of RFC 2279, allowing for values in the range 0
to 0x7fffffff, up to 6 bytes long. This makes it possible to generate
codepoints greater than 0x10ffff which are useful for testing PCRE2's error
checking, and also for generating 32-bit non-UTF data values above the UTF
limit.

Argument:
  utf8bytes   a pointer to the byte vector
  vptr        a pointer to an int to receive the value

Returns:      >  0 => the number of bytes consumed
              -6 to 0 => malformed UTF-8 character at offset = (-return)
*/
/*****************************************************************************/
{
  tconv_uint32_t c = *utf8bytes++;
  tconv_uint32_t d = c;
  int i, j, s;
  const int utf8_table1[] = { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff};
  const int utf8_table3[] = { 0xff, 0x1f, 0x0f, 0x07, 0x03, 0x01};
  const int utf8_table1_size = sizeof(utf8_table1) / sizeof(int);

  for (i = -1; i < 6; i++) {               /* i is number of additional bytes */
    if ((d & 0x80) == 0) break;
    d <<= 1;
  }

  if (i == -1) {
    /* ascii character */
    *uint32p = c;
    return 1;
  }
  if (i == 0 || i == 6) {
    return 0;
  } /* invalid UTF-8 */

  /* i now has a value in the range 1-5 */

  s = 6*i;
  d = (c & utf8_table3[i]) << s;

  for (j = 0; j < i; j++) {
    c = *utf8bytes++;
    if ((c & 0xc0) != 0x80) {
      return -(j+1);
    }
    s -= 6;
    d |= (c & 0x3f) << s;
  }

  /* Check that encoding was the correct unique one */

  for (j = 0; j < utf8_table1_size; j++) {
    if (d <= (uint32_t)utf8_table1[j]) {
      break;
    }
  }
  if (j != i) {
    return -(i+1);
  }

  /* Valid value */

  *uint32p = d;
  return i+1;
}

#endif /* ICONV_IS_NOT_TRUSTABLE */
