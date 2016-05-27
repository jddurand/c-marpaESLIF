#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "marpa.h"
#include "marpa_codes.h"

extern const struct marpa_error_description_s marpa_error_description[];

#include "marpaWrapper/grammar.h"

static marpaWrapperGrammarOption_t marpaWrapperGrammarOptionDefault = {
  NULL,    /* genericLoggerp    */
  0,       /* warningIsErrorb   */
  0,       /* warningIsIgnoredb */
};

/* Our context is exactly marpa's structure */
typedef struct marpaWrapperGrammar {
  int                         refi;                              /* Number of references used when cloning */
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  Marpa_Grammar               marpaGrammarp;
  Marpa_Config                marpaConfig;
};

#ifndef MARPAWRAPPER_NTRACE
#define MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, fmts, ...) GENERICLOGGER_TRACEF(genericLoggerp, "[%s] " fmts, funcs, __VA_ARGS__)
#define MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, msgs) GENERICLOGGER_TRACEF(genericLoggerp, "[%s] %s", funcs, msgs)
#else
#define MARPAWRAPPERGRAMMAR_TRACEF
#define MARPAWRAPPERGRAMMAR_TRACE
#endif

#define MARPAWRAPPERGRAMMAR_C_ERROR(genericLoggerp, marpaConfigp) do {  \
    Marpa_Error_Code marpaErrorCodei;                                   \
                                                                        \
    marpaErrorCodei = marpa_c_error((marpaConfigp), NULL);              \
    if (marpaErrorCodei < MARPA_ERROR_COUNT) {                          \
      struct marpa_error_description_s s = marpa_error_description[marpaErrorCodei]; \
      GENERICLOGGER_ERRORF(genericLoggerp, "%s: %s", s.name, s.suggested); \
    } else {                                                            \
      GENERICLOGGER_ERRORF(genericLoggerp, "Unknown marpa error code %d", marpaErrorCodei); \
    }                                                                   \
} while (0)

#define MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaGrammarp) do { \
    Marpa_Error_Code marpaErrorCodei;                                   \
                                                                        \
    marpaErrorCodei = marpa_g_error((marpaGrammarp), NULL);             \
    if (marpaErrorCodei < MARPA_ERROR_COUNT) {                          \
      struct marpa_error_description_s s = marpa_error_description[marpaErrorCodei]; \
      GENERICLOGGER_ERRORF(genericLoggerp, "%s: %s", s.name, s.suggested); \
    } else {                                                            \
      GENERICLOGGER_ERRORF(genericLoggerp, "Unknown marpa error code %d", marpaErrorCodei); \
    }                                                                   \
} while (0)


/****************************************************************************/
marpaWrapperGrammar_t *marpaWrapperGrammar_newp(marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp)
/****************************************************************************/
{
  const static char      funcs[] = "marpaWrapperGrammar_newp";
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
  genericLogger_t       *genericLoggerp;
#ifndef MARPAWRAPPER_NTRACE
  int                    marpaVersionip[3];
#endif

  if (marpaWrapperGrammarOptionp == NULL) {
    marpaWrapperGrammarOptionp = &marpaWrapperGrammarOptionDefault;
  }
  genericLoggerp = marpaWrapperGrammarOptionp->genericLoggerp;

#ifndef MARPAWRAPPER_NTRACE
  {
    /* Get marpa version */
    Marpa_Error_Code marpaErrorCodei = marpa_version(&(marpaVersionip[0]));
    if (marpaErrorCodei < 0) {
      GENERICLOGGER_ERROR(genericLoggerp, "marpa_version failure");
      goto err;
    }
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_version: %d.%d.%d", marpaVersionip[0], marpaVersionip[1], marpaVersionip[2]);
  }
#endif

  marpaWrapperGrammarp = malloc(sizeof(marpaWrapperGrammar_t));
  if (marpaWrapperGrammarp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  /* Initialize Marpa - always succeed as per the doc */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_c_init(%p)", &(marpaWrapperGrammarp->marpaConfig));
  marpa_c_init(&(marpaWrapperGrammarp->marpaConfig));

  /* Create a grammar instance */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_new(%p)", &(marpaWrapperGrammarp->marpaConfig));
  marpaWrapperGrammarp->marpaGrammarp = marpa_g_new(&(marpaWrapperGrammarp->marpaConfig));
  if (marpaWrapperGrammarp->marpaGrammarp == NULL) {
    MARPAWRAPPERGRAMMAR_C_ERROR(genericLoggerp, &(marpaWrapperGrammarp->marpaConfig));
    goto err;
  }

  /* Turn off obsolete features as per the doc */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_force_valued(%p)", marpaWrapperGrammarp->marpaGrammarp);
  if (marpa_g_force_valued(marpaWrapperGrammarp->marpaGrammarp) < 0) {
    MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (genericLoggerp != NULL) {
    MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, "Cloning genericLogger");

    marpaWrapperGrammarOptionp->genericLoggerp = GENERICLOGGER_CLONE(genericLoggerp);
    if (marpaWrapperGrammarOptionp->genericLoggerp == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "Failed to clone genericLogger: %s", strerror(errno));
      goto err;
    }
    /* We INTENTIONNALY do not change genericLoggerp */
  }

  marpaWrapperGrammarp->refi                      = 1;
  marpaWrapperGrammarp->marpaWrapperGrammarOption = *marpaWrapperGrammarOptionp;

  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperGrammarp);
  return marpaWrapperGrammarp;

 err:
  if (marpaWrapperGrammarp != NULL) {
    int errnoi = errno;

    if ((genericLoggerp != NULL) &&
        (marpaWrapperGrammarOptionp->genericLoggerp != NULL) &&
        (marpaWrapperGrammarOptionp->genericLoggerp != genericLoggerp)) {
      MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, "Freeing cloned genericLogger");
      GENERICLOGGER_FREE(marpaWrapperGrammarOptionp->genericLoggerp);
    }

    free(marpaWrapperGrammarp);
    errno = errnoi;
  }

  return NULL;
}

/****************************************************************************/
marpaWrapperGrammar_t *marpaWrapperGrammar_clonep(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperGrammar_clonep";
#ifndef MARPAWRAPPER_NTRACE
  genericLogger_t   *genericLoggerp;
#endif

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  /* Instead of grammar built-in reference count, we use our own counter */
#ifndef MARPAWRAPPER_NTRACE
  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;
#endif
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Increasing reference count from %d to %d", marpaWrapperGrammarp->refi, marpaWrapperGrammarp->refi + 1);
  marpaWrapperGrammarp->refi++;

  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperGrammarp);
  return marpaWrapperGrammarp;

 err:
  return NULL;
}

/****************************************************************************/
void marpaWrapperGrammar_freev(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperGrammar_freev";
#ifndef MARPAWRAPPER_NTRACE
  genericLogger_t  *genericLoggerp;
#endif

  if (marpaWrapperGrammarp != NULL) {
#ifndef MARPAWRAPPER_NTRACE
    genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;
#endif
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Decreasing reference count from %d to %d", marpaWrapperGrammarp->refi, marpaWrapperGrammarp->refi - 1);
    if (--marpaWrapperGrammarp->refi <= 0) {
      MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_unref(%p)", marpaWrapperGrammarp->marpaGrammarp);
      marpa_g_unref(marpaWrapperGrammarp->marpaGrammarp);
      if (genericLoggerp != NULL) {
        MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
        GENERICLOGGER_FREE(marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp);
      }
      MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperGrammarp);
      free(marpaWrapperGrammarp);
    }
  }
}
