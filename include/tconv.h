#ifndef TCONV_H
#define TCONV_H

typedef struct tconv *tconv_t;

/************************************************************************************/
/* If fromcode == NULL, charset detection will happen                               */
/* This library does NOT try to restrict tocode to only a charset name              */
/* This mean that if the backend is iconv, something like ASCII//TRANSLIT will work */
/* while this will FAIL if the backend is not iconv.                                */
/* This is why it is recommended to only use charsets in tconv_open(), and to use   */
/* tconv_open_ext() if you want to have further configuration.                      */
/************************************************************************************/
tconv_t  tconv_open(const char *tocode, const char *fromcode);
size_t   tconv(tconv_t cd, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
int      tconv_close(tconv_t cd);

/****************************************************/
/* Extended version to handle backend specificities */
/****************************************************/

/* ------------------- */
/* Cconversion options */
/* ------------------- */

typedef struct tconv_convert_iconv {    /* Built-in */
  short translitb;                 /* Default:    0 */
  short ignoreb;                   /* Default:    0 */
  size_t bufsizel;                 /* Default:    0 (no memory limit) */
} tconv_convert_iconv_t;

typedef struct tconv_convert_ICU {   /* Built-in */
  void *fromCallackp;           /* Default: NULL */
  void *fromFallbackp;          /* Default: NULL */
  void *toCallackp;             /* Default: NULL */
  void *toFallbackp;            /* Default: NULL */
  size_t bufsizel;              /* Default:    0 (no memory limit) */
} tconv_convert_ICU_t;

typedef struct tconv_convert_external {     /* External */
  void    *optionp;
  void    *(*tconv_convert_open)(const char *tocode, const char *fromcode, void *optionp);
  size_t   (*tconv_convert)(void *contextp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
  int      (*tconv_convert_close)(void *contextp);
} tconv_convert_external_t;

typedef struct tconv_convert_plugin {     /* plugin */
  void    *optionp;
  char    *filenames;
} tconv_convert_plugin_t;

typedef struct tconv_convert {
  enum {
    TCONV_CONVERT_ICONV,
    TCONV_CONVERT_ICU,
    TCONV_CONVERT_EXTERNAL,
    TCONV_CONVERT_PLUGIN
  } converteri;
  union {
    tconv_convert_iconv_t    *iconvp;
    tconv_convert_ICU        *ICUp;
    tconv_convert_external_t *externalp;
  } u;
} tconv_convert_t;

/* ------------------------- */
/* Charset detection options */
/* ------------------------- */

typedef struct tconv_charset_cchardet { /*      Built-in */
  float confidencef;                    /* Default: 0.1f */
} tconv_charset_cchardet_t;

typedef struct tconv_charset_ICU { /*    Built-in */
  int confidencei;                 /* Default: 10 */
} tconv_charset_ICU_t;

typedef struct tconv_charset_external {     /* External */
  void *optionp;
  void *(*tconv_charset_new)(void *optionp);
  char *(*tconv_charset)(void *contextp, char *bytep, size_t bytel);
  void  (*tconv_charset_free)(void *contextp);
} tconv_charset_external_t;

typedef struct tconv_charset {
  enum { TCONV_CHARSET_CCHARDET, TCONV_CHARSET_ICU, TCONV_CHARSET_X } detectori;
  union {
    tconv_charset_cchardet_t  *cchardetp;
    tconv_charset_ICU_t       *ICUp;
    tconv_charset_external_t  *externalp;
  } u;
} tconv_convert_t;

/* -------------- */
/* Global options */
/* -------------- */
typedef struct tconv_opt {
  tconv_convert_t *convertp; /* Default: NULL */
  tconv_charset_t *charsetp; /* Default: NULL */
} tconv_opt_t;

tconv_t  tconv_open_ext(const char *tocode, const char *fromcode, tconv_opt_t *opt);

#endif /* TCONV_H */
