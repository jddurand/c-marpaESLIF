/* We do not want to depend on the floating point format representation */
/* so we use only an interation on the sprintf presision plus verification */
/* using C_STRTOx functions. This is very slow, you are warned. */
/* Note that gnulib uses the same technique. */

#ifdef HAVE_MATH_H
#  include <math.h>
#endif
#ifdef HAVE_FLOAT_H
#  include <float.h>
#endif

#if defined(MARPAESLIF_ISINF) && defined(MARPAESLIF_INFINITY)
#  define MARPAESLIF_FLOATTOS_INFINITY(x)
#else
#  define MARPAESLIF_FLOATTOS_INFINITY(x) do {                  \
    if (MARPAESLIF_ISINF(f)) {                                  \
      /* Get native inf representation - this must not fail */  \
      marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;     \
      marpaESLIF_stringGenerator.s           = NULL;            \
      marpaESLIF_stringGenerator.l           = 0;               \
      marpaESLIF_stringGenerator.okb         = 0;               \
      marpaESLIF_stringGenerator.allocl      = 0;                       \
                                                                        \
      genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
      if (genericLoggerp == NULL) {                                     \
        goto err;                                                       \
      }                                                                 \
      GENERICLOGGER_TRACEF(genericLoggerp, "%f", (double) MARPAESLIF_INFINITY); \
      if (! marpaESLIF_stringGenerator.okb) {                           \
        goto err;                                                       \
      }                                                                 \
      goto done;                                                        \
    }                                                                   \
} while (0)
#endif

#if defined(MARPAESLIF_ISNAN) && defined(MARPAESLIF_NAN)
#  define MARPAESLIF_FLOATTOS_NAN(x)
#else
#  define MARPAESLIF_FLOATTOS_NAN(x) do {                       \
    if (MARPAESLIF_ISNAN(f)) {                                  \
      /* Get native inf representation - this must not fail */  \
      marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;     \
      marpaESLIF_stringGenerator.s           = NULL;            \
      marpaESLIF_stringGenerator.l           = 0;               \
      marpaESLIF_stringGenerator.okb         = 0;                       \
      marpaESLIF_stringGenerator.allocl      = 0;                       \
                                                                        \
      genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
      if (genericLoggerp == NULL) {                                     \
        goto err;                                                       \
      }                                                                 \
      GENERICLOGGER_TRACEF(genericLoggerp, "%f", (double) MARPAESLIF_NAN); \
      if (! marpaESLIF_stringGenerator.okb) {                           \
        goto err;                                                       \
      }                                                                 \
      goto done;                                                        \
    }                                                                   \
} while (0)
#endif

#define MARPAESLIF_FLOATTOS(name, type, fmts, fmts_type, strtox, maxDigitsi) \
  char *name(marpaESLIF_t *marpaESLIFp, type x)                         \
  {                                                                     \
    type                          origx          = x;                   \
    genericLogger_t              *genericLoggerp = NULL;                \
    char                         *endptrp;                              \
    marpaESLIF_stringGenerator_t  marpaESLIF_stringGenerator;           \
    int                           i;                                    \
                                                                        \
    if (marpaESLIFp == NULL) {                                          \
      errno = EINVAL;                                                   \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    MARPAESLIF_FLOATTOS_INFINITY(x);                                    \
    MARPAESLIF_FLOATTOS_NAN(x);                                         \
                                                                        \
    marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;               \
    marpaESLIF_stringGenerator.s           = NULL;                      \
    marpaESLIF_stringGenerator.l           = 0;                         \
    marpaESLIF_stringGenerator.okb         = 0;                         \
    marpaESLIF_stringGenerator.allocl      = 0;                         \
                                                                        \
    genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE); \
    if (genericLoggerp == NULL) {                                       \
      goto err;                                                         \
    }                                                                   \
                                                                        \
    /* We subjectively estimate that nothing reasonable can have more than maxDigitsi */ \
    for (i = 0; i < maxDigitsi; i++) {                                  \
      GENERICLOGGER_TRACEF(genericLoggerp, fmts, i, (fmts_type) origx); \
      if (! marpaESLIF_stringGenerator.okb) {                           \
        goto err;                                                       \
      }                                                                 \
      x = strtox(marpaESLIF_stringGenerator.s, &endptrp);               \
      if (x == origx) {                                                 \
        break;                                                          \
      }                                                                 \
      /* Reset */                                                       \
      free(marpaESLIF_stringGenerator.s);                               \
      marpaESLIF_stringGenerator.s      = NULL;                         \
      marpaESLIF_stringGenerator.l      = 0;                            \
      marpaESLIF_stringGenerator.okb    = 0;                            \
      marpaESLIF_stringGenerator.allocl = 0;                            \
    }                                                                   \
                                                                        \
    /* Here either f == origf, either we reached 999 decimals, that is considered */ \
    /* subjectively far beyond what is expected. */                     \
                                                                        \
    goto done;                                                          \
                                                                        \
  err:                                                                  \
    if (marpaESLIF_stringGenerator.s != NULL) {                         \
      free(marpaESLIF_stringGenerator.s);                               \
      marpaESLIF_stringGenerator.s = NULL;                              \
    }                                                                   \
                                                                        \
  done:                                                                 \
    GENERICLOGGER_FREE(genericLoggerp);                                 \
    return marpaESLIF_stringGenerator.s;                                \
  }

MARPAESLIF_FLOATTOS(marpaESLIF_ftos, float, "%.*f", double, C_STRTOF, 100)
MARPAESLIF_FLOATTOS(marpaESLIF_dtos, double, "%.*f", double, C_STRTOD, 100)
MARPAESLIF_FLOATTOS(marpaESLIF_ldtos, long double, "%.*Lf", long double, C_STRTOLD, 1000)

