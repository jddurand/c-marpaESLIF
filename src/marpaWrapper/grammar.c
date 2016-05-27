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
  Marpa_Grammar marpaGrammarp;
  Marpa_Config  marpaConfig;
};

#ifndef MARPAWRAPPER_NTRACE
#define MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, fmts, ...) GENERICLOGGER_TRACEF(genericLoggerp, fmts " at %s:%d", __VA_ARGS__, __FILE__, __LINE__)
#define MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, msgs) GENERICLOGGER_TRACEF(genericLoggerp, "%s at %s:%d", msgs, __FILE__, __LINE__)
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
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, "marpa_version: %d.%d.%d", marpaVersionip[0], marpaVersionip[1], marpaVersionip[2]);
  }
#endif

  marpaWrapperGrammarp = malloc(sizeof(marpaWrapperGrammar_t));
  if (marpaWrapperGrammarp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  /* Initialize Marpa - always succeed as per the doc */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, "marpa_c_init(%p)", &(marpaWrapperGrammarp->marpaConfig));
  marpa_c_init(&(marpaWrapperGrammarp->marpaConfig));

  /* Create a grammar instance */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, "marpa_g_new(%p)", &(marpaWrapperGrammarp->marpaConfig));
  marpaWrapperGrammarp->marpaGrammarp = marpa_g_new(&(marpaWrapperGrammarp->marpaConfig));
  if (marpaWrapperGrammarp->marpaGrammarp == NULL) {
    MARPAWRAPPERGRAMMAR_C_ERROR(genericLoggerp, &(marpaWrapperGrammarp->marpaConfig));
    goto err;
  }

  /* Turn off obsolete features as per the doc */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, "marpa_g_force_valued(%p)", marpaWrapperGrammarp->marpaGrammarp);
  if (marpa_g_force_valued(marpaWrapperGrammarp->marpaGrammarp) < 0) {
    MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  return marpaWrapperGrammarp;

 err:
  if (marpaWrapperGrammarp != NULL) {
    free(marpaWrapperGrammarp);
  }

  return NULL;
}

