#ifndef TCONV_EXT_H
#define TCONV_EXT_H

#include <stddef.h>
#include "genericLogger.h"
#include "tconv/export.h"

typedef struct tconv *tconv_t;

/*****************************/
/* Charset detection options */
/*****************************/
typedef void *(*tconv_charset_new_t) (tconv_t tconvp, void *optionp);
typedef char *(*tconv_charset_run_t) (tconv_t tconvp, void *contextp, char *bytep, size_t bytel);
typedef void  (*tconv_charset_free_t)(tconv_t tconvp, void *contextp);

/* ------------------------- */
/* External charset          */
/* ------------------------- */
typedef struct tconv_charset_external {
  void                *optionp;
  tconv_charset_new_t  tconv_charset_newp;
  tconv_charset_run_t  tconv_charset_runp;
  tconv_charset_free_t tconv_charset_freep;
} tconv_charset_external_t;

/* ------------------------- */
/* Plugin charset            */
/* ------------------------- */
typedef struct tconv_charset_plugin {
  void *optionp;
  char *filenames;
} tconv_charset_plugin_t;

/* ------------------------- */
/* Buit-in charset           */
/* ------------------------- */
#include "tconv/charset/cchardet.h"
#include "tconv/charset/ICU.h"

/* ------------------------------- */
/* Global charset option structure */
/* ------------------------------- */
typedef struct tconv_charset {
  enum {
    /* Default:
       TCONV_CHARSET_ICU if found, else
       TCONV_CHARSET_CCHARDET
    */
    TCONV_CHARSET_EXTERNAL = 0,
    TCONV_CHARSET_PLUGIN,
    TCONV_CHARSET_ICU,
    TCONV_CHARSET_CCHARDET,
  } charseti;
  union {
    tconv_charset_external_t         external;
    tconv_charset_plugin_t           plugin;
    tconv_charset_cchardet_option_t *cchardetOptionp;
    tconv_charset_ICU_option_t      *ICUOptionp;
  } u;
} tconv_charset_t;

/**********************/
/* Conversion options */
/**********************/

typedef void   *(*tconv_convert_new_t) (tconv_t tconvp, const char *tocodes, const char *fromcodes, void *optionp);
typedef size_t  (*tconv_convert_run_t)  (tconv_t tconvp, void *contextp, char **inbufsp, size_t *inbytesleftlp, char **outbufsp, size_t *outbytesleftlp);
typedef int     (*tconv_convert_free_t)(tconv_t tconvp, void *contextp);

/* ------------------ */
/* External converter */
/* ------------------ */
typedef struct tconv_convert_external {
  void                 *optionp;
  tconv_convert_new_t  tconv_convert_newp;
  tconv_convert_run_t  tconv_convert_runp;
  tconv_convert_free_t tconv_convert_freep;
} tconv_convert_external_t;

/* ------------------ */
/* Plugin converter   */
/* ------------------ */
typedef struct tconv_convert_plugin {
  void *optionp;
  char *filenames;
} tconv_convert_plugin_t;

/* ------------------ */
/* Built-in converter */
/* ------------------ */
#include "tconv/convert/iconv.h"
#include "tconv/convert/ICU.h"

/* --------------------------------- */
/* Global converter option structure */
/* --------------------------------- */
typedef struct tconv_convert {
  enum { /* Default: ICU if found, else ICONV if found, else -1 */
    TCONV_CONVERT_EXTERNAL = 0,
    TCONV_CONVERT_PLUGIN,
    TCONV_CONVERT_ICU,
    TCONV_CONVERT_ICONV
  } converti;
  union {
    tconv_convert_external_t      external;
    tconv_convert_plugin_t        plugin;
    tconv_convert_ICU_option_t   *ICUOptionp;
    tconv_convert_iconv_option_t *iconvOptionp;
  } u;
} tconv_convert_t;

/* -------------- */
/* Global options */
/* -------------- */
typedef struct tconv_option {
  tconv_charset_t *charsetp;
  tconv_convert_t *convertp;
  genericLogger_t *genericLoggerp;
} tconv_option_t;

TCONV_EXPORT tconv_t tconv_open_ext(const char *tocodes, const char *fromcodes, tconv_option_t *tconvOptionp);

#endif /* TCONV_EXT_H */
