#ifndef TCONV_H
#define TCONV_H

#include <stddef.h>
#include <genericLogger.h>

#include "tconv_export.h"

typedef struct tconv *tconv_t;

/************************************************************************************/
/* If fromcode == NULL, charset detection will happen                               */
/* This library does NOT try to restrict tocode to only a charset name              */
/* This mean that if the backend is iconv, something like ASCII//TRANSLIT will work */
/* while this will FAIL if the backend is not iconv.                                */
/* This is why it is recommended to only use charsets in tconv_open(), and to use   */
/* tconv_open_ext() if you want to have further configuration.                      */
/************************************************************************************/
TCONV_EXPORT tconv_t  tconv_open(const char *tocodes, const char *fromcodes);
TCONV_EXPORT size_t   tconv(tconv_t tconvp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp);
TCONV_EXPORT int      tconv_close(tconv_t tconvp);

/****************************************************/
/* Extended version to handle backend specificities */
/****************************************************/

/* ------------------------- */
/* Charset detection options */
/* ------------------------- */

typedef struct tconv_charset_ICU { /*    Built-in */
  int confidencei;                 /* Default: 10 */
} tconv_charset_ICU_t;

typedef struct tconv_charset_cchardet { /*      Built-in */
  float confidencef;                    /* Default: 0.1f */
} tconv_charset_cchardet_t;

typedef struct tconv_charset_external {     /* External */
  void *optionp; /* Default: NULL */
  void *(*tconv_charset_new)(void *optionp); /* Default: NULL */
  char *(*tconv_charset)(void *contextp, char *bytep, size_t bytel); /* Default: NULL */
  void  (*tconv_charset_free)(void *contextp); /* Default: NULL */
} tconv_charset_external_t;

typedef struct tconv_charset_plugin {     /* plugin */
  void    *optionp;   /* Default: NULL */
  char    *filenames; /* Default: NULL */
} tconv_charset_plugin_t;

typedef struct tconv_charset {
  enum { /* Default: ICU if found, else CCHARDET, else -1 */
    TCONV_CHARSET_ICU,
    TCONV_CHARSET_CCHARDET,
    TCONV_CHARSET_X
  } detectori;
  union {
    tconv_charset_ICU_t       ICU;
    tconv_charset_cchardet_t  cchardet;
    tconv_charset_external_t  external;
    tconv_charset_plugin_t    plugin;
  } u;
} tconv_charset_t;

/* ------------------ */
/* conversion options */
/* ------------------ */

typedef struct tconv_convert_ICU {   /* Built-in */
  size_t bufSizel;              /* Default:    0 (no memory limit) */
  void *fromCallbackp;          /* Default: NULL */
  void *fromFallbackp;          /* Default: NULL */
  void *toCallbackp;            /* Default: NULL */
  void *toFallbackp;            /* Default: NULL */
} tconv_convert_ICU_t;

typedef struct tconv_convert_iconv {    /* Built-in */
  size_t bufSizel;                 /* Default:    0 (no memory limit) */
  short translitb;                 /* Default:    0 */
  short ignoreb;                   /* Default:    0 */
} tconv_convert_iconv_t;

typedef struct tconv_convert_external {     /* External */
  void    *optionp;  /* Default: NULL */
  void    *(*tconv_convert_open)(const char *tocode, const char *fromcode, void *optionp); /* Default: NULL */
  size_t   (*tconv_convert)(void *contextp, char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft); /* Default: NULL */
  int      (*tconv_convert_close)(void *contextp); /* Default: NULL */
} tconv_convert_external_t;

typedef struct tconv_convert_plugin {     /* plugin */
  void    *optionp;   /* Default: NULL */
  char    *filenames; /* Default: NULL */
} tconv_convert_plugin_t;

typedef struct tconv_convert {
  enum { /* Default: ICU if found, else ICONV if found, else -1 */
    TCONV_CONVERT_ICU = 0,
    TCONV_CONVERT_ICONV,
    TCONV_CONVERT_EXTERNAL,
    TCONV_CONVERT_PLUGIN,
  } converteri;
  union {
    tconv_convert_ICU_t      ICU;
    tconv_convert_iconv_t    iconv;
    tconv_convert_external_t external;
    tconv_convert_plugin_t   plugin;
  } u;
} tconv_convert_t;

/* -------------- */
/* Global options */
/* -------------- */
typedef struct tconv_opt {
  genericLogger_t *genericLoggerp; /* Default: NULL - will always log unless -DTCONV_NTRACE */
  tconv_charset_t  charset;
  tconv_convert_t  convert;
} tconv_opt_t;

TCONV_EXPORT tconv_opt_t *tconv_opt_default();
TCONV_EXPORT tconv_t      tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_opt_t *optp);

#endif /* TCONV_H */
