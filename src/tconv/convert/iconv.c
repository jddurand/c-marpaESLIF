#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <iconv.h>

/* iconv interface is a direct proxy to iconv */

/* ==================================================== */
/* Some iconv implementations are buggy when source and */
/* destination charsets are the same. There is no way   */
/* to get around this.                                  */
/*                                                      */
/* In such a case our implementation is doing much more */
/* work by forcing an intermediate representation,      */
/* using an intermediate encoding that can encode all   */
/* characters. Namely: UTF-8 or UTF-32.                 */
/*                                                      */
/* So, when source and destination charsets are the     */
/* same:                                                */
/* - UTF-8 intermediate encoding is used if destination */
/*   charset is not UTF-8, or                           */
/* - UTF-32 intermediate encoding                       */
/* The implementation is simply calling recursively     */
/* tconv -;                                             */
/* ==================================================== */

#include "tconv/convert/iconv.h"
#include "tconv_config.h"
#include "alias.c"

/* Context */
#define TCONV_ICONV_BUFFER_SIZE 4096
typedef struct tconv_convert_iconv_context {
  iconv_t iconvp;                           /* iconv itself */
  char    buffers[TCONV_ICONV_BUFFER_SIZE]; /* Intermediate to avoid fuzzy mode */
} tconv_convert_iconv_context_t;

#ifdef ICONV_SECOND_ARGUMENT_IS_CONST
#define ICONV_SECOND_ARGUMENT const
#else
#define ICONV_SECOND_ARGUMENT
#endif

const static short fuzzyb;
static int    _tconv_strnicmp(const char *s1, const char *s2, size_t n);
static size_t _tconv_convert_iconv_directl(tconv_t tconvp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
static char  *_tconv_convert_charset_normalizeds(tconv_t tconvp, char *charsets);
static short  _tconv_convert_charset_categoriesb(tconv_t tconvp, char *charsets, char ***categoriespp);

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
  static const char funcs[] = "tconv_convert_iconv_new";
  short cancheckb           = ((tocodes != NULL) && (fromcodes != NULL));

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

    if (C_STRNICMP(tocodes, fromcodes, strlen(fromcodes)) == 0) {
      TCONV_TRACE(tconvp, "%s - same destination and source \"%s\": direct byte copy will happen", funcs, tocodes);
      TCONV_TRACE(tconvp, "%s - calling tconv_fuzzy_set(%p, 1)", funcs, tconvp);
      tconv_fuzzy_set(tconvp, 1);
      TCONV_TRACE(tconvp, "%s - return %p", funcs, &fuzzyb);
      return (iconv_t) &fuzzyb;
    }
  }

  TCONV_TRACE(tconvp, "%s - return iconv_open(%p, %p,)", funcs, tocodes, fromcodes);
  return iconv_open(tocodes, fromcodes);
}

/*****************************************************************************/
size_t tconv_convert_iconv_run(tconv_t tconvp, void *voidp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_run";

  if (voidp == (void *) &fuzzyb) {
    TCONV_TRACE(tconvp, "%s - return _tconv_convert_iconv_directl(%p, %p, %p, %p, %p)", funcs, tconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    return _tconv_convert_iconv_directl(tconvp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  } else {
    TCONV_TRACE(tconvp, "%s - return iconv(%p, %p, %p, %p, %p)", funcs, voidp, inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
    return iconv((iconv_t) voidp, (ICONV_SECOND_ARGUMENT char **) inbufpp, inbytesleftlp, outbufpp, outbytesleftlp);
  }
}

/*****************************************************************************/
int tconv_convert_iconv_free(tconv_t tconvp, void *voidp)
/*****************************************************************************/
{
  static const char funcs[] = "tconv_convert_iconv_free";

  if (voidp == (void *) &fuzzyb) {
    TCONV_TRACE(tconvp, "%s - return 0", funcs);
    return 0;
  } else {
    TCONV_TRACE(tconvp, "%s - return iconv_close(%p)", funcs, voidp);
    return iconv_close((iconv_t) voidp);
  }
}

/****************************************************************************/
static int _tconv_strnicmp(const char *s1, const char *s2, size_t n)
/****************************************************************************/
{
  const unsigned char *ucs1 = (const unsigned char *) s1;
  const unsigned char *ucs2 = (const unsigned char *) s2;
  int d = 0;

  for ( ; n != 0; n--) {
    const int c1 = tolower(*ucs1++);
    const int c2 = tolower(*ucs2++);
    if (((d = c1 - c2) != 0) || (c2 == '\0'))
      break;
  }
  return d;
}

/*****************************************************************************/
static size_t _tconv_convert_iconv_directl(tconv_t tconvp, char **inbufpp, size_t *inbytesleftlp, char **outbufpp, size_t *outbytesleftlp)
/*****************************************************************************/
{
  static const char funcs[] = "_tconv_convert_iconv_directl";

  /* C.f. https://dev.openwrt.org/browser/packages/libs/libiconv/src/iconv.c?rev=24777&order=name */
  size_t len = 0;

  if (((inbufpp == NULL) || (*inbufpp == NULL)) &&
      ((outbufpp != NULL) && (*outbufpp != NULL))) {
    TCONV_TRACE(tconvp, "%s - Flush: no shift sequence - return 0", funcs);
    return 0;
  }

  if (((inbufpp == NULL) || (*inbufpp == NULL)) &&
      ((outbufpp == NULL) || (*outbufpp == NULL))) {
    TCONV_TRACE(tconvp, "%s - Back to initial state - return 0", funcs);
    return 0;
  }

  if ((inbytesleftlp  == NULL) || (*inbytesleftlp  < 0) ||
      (outbytesleftlp == NULL) || (*outbytesleftlp < 0) ||
      (outbufpp       == NULL) || (*outbufpp == NULL)) {
    TCONV_TRACE(tconvp, "%s - EINVAL - return -1", funcs);
    errno = EINVAL;
    return (size_t)(-1);
  }

  if ((inbufpp != NULL) && (*inbufpp != NULL)) {
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
  }

  TCONV_TRACE(tconvp, "%s - return 0", funcs);
  return (size_t)(0);
}

/*****************************************************************************/
static char  *_tconv_convert_charset_normalizeds(tconv_t tconvp, char *charsets)
/*****************************************************************************/
{
  static const char funcs[] = "_tconv_convert_charset_normalizeds";
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

  p = charsets;
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
  };

  *q = '\0';

  TCONV_TRACE(tconvp, "%s - return \"%s\"", funcs, normalizeds);
  return normalizeds;
}

/*****************************************************************************/
static short _tconv_convert_charset_categoryb(tconv_t tconvp, char *charsets, char ***categoriespp)
/*****************************************************************************/
{
  static const char             funcs[] = "_tconv_convert_charset_categorys";
  tconv_iconv_alias2category_t *alias2categoryp = alias2category;
  char                         *normalizeds;
  char                        **categoriesp = NULL;
  short                         rcb;
  int                           i;

  normalizeds = _tconv_convert_charset_normalizeds(tconvp, charsets);
  if (normalizeds == NULL) {
    goto err;
  }

  for (i = 0; i < TCONV_ICONV_NB_ALIAS; alias2categoryp++, i++) {
    if (strcmp(alias2categoryp->alias, normalizeds) == 0) {
      categoriesp = alias2categoryp->categoriesp;
      break;
    }
  }

  *categoriespp = categoriesp;
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (normalizeds != NULL) {
    free(normalizeds);
  }

#ifndef TCONV_NTRACE
  if (rcb) {
    if (*categoriespp != NULL) {
      TCONV_TRACE(tconvp, "%s - return %d (*categoriespp = \"%s\"", funcs, (int) rcb, *categoriespp);
    } else {
      TCONV_TRACE(tconvp, "%s - return %d (*categoriespp = NULL)", funcs, (int) rcb);
    }
  } else {
    TCONV_TRACE(tconvp, "%s - return %d", funcs, (int) rcb);
  }
#endif
  return rcb;
}
