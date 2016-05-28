#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "marpa.h"
#include "marpa_codes.h"

#include "config.h"
#include "marpaWrapper/grammar.h"
#include "internal/manageBuf.h"

extern const struct marpa_error_description_s marpa_error_description[];
extern const struct marpa_event_description_s marpa_event_description[];

static marpaWrapperGrammarOption_t marpaWrapperGrammarOptionDefault = {
  NULL,    /* genericLoggerp      */
  0,       /* warningIsErrorb     */
  0,       /* warningIsIgnoredb   */
  NULL,    /* eventCallbackp      */
  NULL     /* eventCallbackDatavp */
};

static marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOptionDefault = {
  NULL,                              /* datavp */
  0,                                 /* terminalb */
  0,                                 /* startb */
  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE /* eventSeti */
};

static marpaWrapperGrammarRuleOption_t marpaWrapperGrammarRuleOptionDefault = {
  NULL, /* datavp */
  0,    /* ranki */
  0,    /* nullRanksHighb */
  0,    /* sequenceb */
  NULL, /* separatorSymbolp */
  0,    /* properb */
  0     /* minimumi */
};

struct marpaWrapperGrammarSymbol {
  Marpa_Symbol_ID                   marpaSymbolIdi;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;
  /* Internal work area */
  short                             isLexemeb;
  size_t                            lengthl;
};

struct marpaWrapperGrammarRule {
  Marpa_Rule_ID                   marpaRuleIdi;
  marpaWrapperGrammarRuleOption_t marpaWrapperGrammarRuleOption;
};

struct marpaWrapperGrammar {
  short                         precomputedb; /* Flag saying it is has be precomputed */
  short                         haveStartb;   /* Flag saying it a start symbol was explicitely declare */

  marpaWrapperGrammarOption_t   marpaWrapperGrammarOption;
  Marpa_Grammar                 marpaGrammarp;
  Marpa_Config                  marpaConfig;

  /* Storage of symbols */
  size_t                        sizeSymboli;           /* Allocated size */
  size_t                        nSymboli;              /* Used size      */
  marpaWrapperGrammarSymbol_t **symbolpp;

  /* Storage of rules */
  size_t                        sizeRulei;           /* Allocated size */
  size_t                        nRulei;              /* Used size      */
  marpaWrapperGrammarRule_t   **rulepp;
};

#ifndef MARPAWRAPPER_NTRACE
#define MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, fmts, ...) GENERICLOGGER_TRACEF(genericLoggerp, "[%s] " fmts, funcs, __VA_ARGS__)
#define MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, msgs) GENERICLOGGER_TRACEF(genericLoggerp, "[%s] %s", funcs, msgs)
#else
#define MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, fmts, ...)
#define MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, msgs)
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

static short _marpaWrapperGrammar_eventb(marpaWrapperGrammar_t *marpaWrapperGrammarp);
#define MARPAWRAPPERGRAMMAREVENT_WEIGHT(eventType) ((eventType) == MARPAWRAPPERGRAMMAR_EVENT_COMPLETED) ? -1 : (((eventType) == MARPAWRAPPERGRAMMAR_EVENT_NULLED) ? 0 : 1)
static int _marpaWrapperGrammar_cmpi(const void *event1p, const void *event2p);

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

  marpaWrapperGrammarp->precomputedb              = 0;
  marpaWrapperGrammarp->haveStartb                = 0;
  marpaWrapperGrammarp->marpaWrapperGrammarOption = *marpaWrapperGrammarOptionp;
  marpaWrapperGrammarp->sizeSymboli               = 0;
  marpaWrapperGrammarp->nSymboli                  = 0;
  marpaWrapperGrammarp->symbolpp                  = NULL;
  marpaWrapperGrammarp->sizeRulei                 = 0;
  marpaWrapperGrammarp->nRulei                    = 0;
  marpaWrapperGrammarp->rulepp                    = NULL;

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
void marpaWrapperGrammar_freev(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperGrammar_freev";
  genericLogger_t  *genericLoggerp;

  if (marpaWrapperGrammarp != NULL) {
    genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_unref(%p)", marpaWrapperGrammarp->marpaGrammarp);
    marpa_g_unref(marpaWrapperGrammarp->marpaGrammarp);
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Freeing symbol table", marpaWrapperGrammarp->marpaGrammarp);
    manageBuf_freev(genericLoggerp, (void ***) &(marpaWrapperGrammarp->symbolpp), &(marpaWrapperGrammarp->nSymboli));
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Freeing rule table", marpaWrapperGrammarp->marpaGrammarp);
    manageBuf_freev(genericLoggerp, (void ***) &(marpaWrapperGrammarp->rulepp), &(marpaWrapperGrammarp->nRulei));
    if (genericLoggerp != NULL) {
      MARPAWRAPPERGRAMMAR_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp);
    }
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperGrammarp);
    free(marpaWrapperGrammarp);
  }
}

/****************************************************************************/
marpaWrapperGrammarSymbol_t *marpaWrapperGrammar_newSymbolp(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/****************************************************************************/
{
  const static char            funcs[] = "marpaWrapperGrammar_newSymbolp";
  Marpa_Symbol_ID              marpaSymbolIdi;
  genericLogger_t             *genericLoggerp;
  size_t                       nSymboli;
  marpaWrapperGrammarSymbol_t *marpaWrapperSymbolp;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  /* Create symbol */
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_new(%p)", marpaWrapperGrammarp->marpaGrammarp);
  marpaSymbolIdi = marpa_g_symbol_new(marpaWrapperGrammarp->marpaGrammarp);
  if (marpaSymbolIdi == -2) {
    MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Allocate room for the new symbol */
  nSymboli = marpaSymbolIdi + 1;
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Creating room in symbol table at index %d", (int) marpaSymbolIdi);
  if (manageBuf_createp(genericLoggerp,
			(void ***) &(marpaWrapperGrammarp->symbolpp),
			&(marpaWrapperGrammarp->sizeSymboli),
			nSymboli,
			sizeof(marpaWrapperGrammarSymbol_t *)) == NULL) {
    goto err;
  }

  marpaWrapperSymbolp = (marpaWrapperGrammarSymbol_t *) malloc(sizeof(marpaWrapperGrammarSymbol_t));
  if (marpaWrapperSymbolp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }
  marpaWrapperGrammarp->nSymboli = nSymboli;
  marpaWrapperGrammarp->symbolpp[marpaSymbolIdi] = marpaWrapperSymbolp;

  /* Fill the symbol structure */
  marpaWrapperSymbolp->marpaSymbolIdi = marpaSymbolIdi;
  if (marpaWrapperGrammarSymbolOptionp == NULL) {
    marpaWrapperGrammarSymbolOptionp = &marpaWrapperGrammarSymbolOptionDefault;
  }
  marpaWrapperSymbolp->marpaWrapperGrammarSymbolOption = *marpaWrapperGrammarSymbolOptionp;

  /* Apply options */
  if (marpaWrapperGrammarSymbolOptionp->terminalb != 0) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_terminal_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_terminal_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if (marpaWrapperGrammarSymbolOptionp->startb != 0) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_start_symbol_set(%p, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi);
    if (marpa_g_start_symbol_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi) < 0) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
    marpaWrapperGrammarp->haveStartb = 1;
  }

  if ((marpaWrapperGrammarSymbolOptionp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_completion_event_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_completion_event_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if ((marpaWrapperGrammarSymbolOptionp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) == MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_nulled_event_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_nulled_event_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if ((marpaWrapperGrammarSymbolOptionp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_prediction_event_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_prediction_event_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  return marpaWrapperSymbolp;
  
 err:
  return NULL;
}

/****************************************************************************/
marpaWrapperGrammarRule_t *marpaWrapperGrammar_newRulep(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperGrammarRuleOption_t *marpaWrapperGrammarRuleOptionp,
							marpaWrapperGrammarSymbol_t *lhsSymbolp,
							size_t rhsSymboll, marpaWrapperGrammarSymbol_t **rhsSymbolpp
							)
/****************************************************************************/
{
  const static char          funcs[] = "marpaWrapperGrammar_newRulep";
  Marpa_Rule_ID              marpaRuleIdi;
  genericLogger_t           *genericLoggerp;
  size_t                     nSymboli;
  marpaWrapperGrammarRule_t *marpaWrapperRulep;
  Marpa_Symbol_ID           *rhsIdp = NULL;
  size_t                     i;
  int                        sequenceFlagsi;
  Marpa_Symbol_ID            separatorSymbolIdi;
  size_t                     nRulei;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  /* Check parameters - Some depend on marpaWrapperGrammarRuleOptionp */
  if (marpaWrapperGrammarRuleOptionp == NULL) {
    marpaWrapperGrammarRuleOptionp = &marpaWrapperGrammarRuleOptionDefault;
  }

  if (lhsSymbolp == NULL) {
    GENERICLOGGER_ERROR(genericLoggerp, "No LHS symbol");
    goto err;
  }
  if (marpaWrapperGrammarRuleOptionp->sequenceb != 0) {
    if (rhsSymboll != 1) {
      GENERICLOGGER_ERROR(genericLoggerp, "A sequence must have exactly one RHS");
      goto err;
    }
    if ((marpaWrapperGrammarRuleOptionp->minimumi != 0) && (marpaWrapperGrammarRuleOptionp->minimumi != 1)) {
      GENERICLOGGER_ERROR(genericLoggerp, "A sequence must have a minimum of exactly 0 or 1");
      goto err;
    }
  }

  /* Temporary room for the RHS identifiers */
  if (rhsSymboll > 0) {
    rhsIdp = (Marpa_Symbol_ID *) malloc(rhsSymboll * sizeof(Marpa_Symbol_ID));
    if (rhsIdp == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }
    for (i = 0; i < rhsSymboll; i++) {
      rhsIdp[i] = rhsSymbolpp[i]->marpaSymbolIdi;
    }
  }

  /* Create rule; it either a sequence, either a list of RHS */
  if (marpaWrapperGrammarRuleOptionp->sequenceb != 0) {
    sequenceFlagsi = 0;
    separatorSymbolIdi = (marpaWrapperGrammarRuleOptionp->separatorSymbolp != NULL) ? marpaWrapperGrammarRuleOptionp->separatorSymbolp->marpaSymbolIdi : -1;
    if (marpaWrapperGrammarRuleOptionp->properb != 0) {
      sequenceFlagsi |= MARPA_PROPER_SEPARATION;
    }
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_sequence_new(%p, %d, %d, %d, %d, %s)", marpaWrapperGrammarp->marpaGrammarp, (int) lhsSymbolp->marpaSymbolIdi, (int) rhsIdp[0], (int) separatorSymbolIdi, marpaWrapperGrammarRuleOptionp->minimumi, ((sequenceFlagsi & MARPA_PROPER_SEPARATION) == MARPA_PROPER_SEPARATION) ? "MARPA_PROPER_SEPARATION" : "");
    marpaRuleIdi = marpa_g_sequence_new(marpaWrapperGrammarp->marpaGrammarp,
					lhsSymbolp->marpaSymbolIdi,
					rhsIdp[0],
					separatorSymbolIdi,
					marpaWrapperGrammarRuleOptionp->minimumi,
					sequenceFlagsi);
  } else {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_rule_new(%p, %d, %p, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) lhsSymbolp->marpaSymbolIdi, rhsIdp, (int) rhsSymboll);
    marpaRuleIdi = marpa_g_rule_new(marpaWrapperGrammarp->marpaGrammarp,
				    lhsSymbolp->marpaSymbolIdi,
				    rhsIdp,
				    rhsSymboll);
  }

  if (rhsIdp != NULL) {
    free(rhsIdp);
    rhsIdp = NULL;
  }

  if (marpaRuleIdi == -2) {
    MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Apply options */
  if (marpaWrapperGrammarRuleOptionp->ranki != 0) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_rule_rank_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaRuleIdi, marpaWrapperGrammarRuleOptionp->ranki);
    marpa_g_rule_rank_set(marpaWrapperGrammarp->marpaGrammarp, marpaRuleIdi, marpaWrapperGrammarRuleOptionp->ranki);
    if (marpa_g_error(marpaWrapperGrammarp->marpaGrammarp, NULL) != MARPA_ERR_NONE) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if (marpaWrapperGrammarRuleOptionp->nullRanksHighb != 0) {
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_rule_null_high_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaRuleIdi, 1);
    if (marpa_g_rule_null_high_set(marpaWrapperGrammarp->marpaGrammarp, marpaRuleIdi, 1) < 0) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  /* Allocate room for the new symbol */
  nRulei = marpaRuleIdi + 1;
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Creating room in rule table at index %d", (int) marpaRuleIdi);
  if (manageBuf_createp(genericLoggerp,
			(void ***) &(marpaWrapperGrammarp->rulepp),
			&(marpaWrapperGrammarp->sizeRulei),
			nRulei,
			sizeof(marpaWrapperGrammarRule_t *)) == NULL) {
    goto err;
  }

  marpaWrapperRulep = (marpaWrapperGrammarRule_t *) malloc(sizeof(marpaWrapperGrammarRule_t));
  if (marpaWrapperRulep == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }
  marpaWrapperRulep->marpaRuleIdi                  = marpaRuleIdi;
  marpaWrapperRulep->marpaWrapperGrammarRuleOption = *marpaWrapperGrammarRuleOptionp;

  marpaWrapperGrammarp->nRulei = nRulei;
  marpaWrapperGrammarp->rulepp[marpaRuleIdi] = marpaWrapperRulep;

 err:
  if (rhsIdp != NULL) {
    int errnoi = errno;
    free(rhsIdp);
    errno = errnoi;
  }
  
  return NULL;
}

/****************************************************************************/
short marpaWrapperGrammar_precomputeb(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperGrammar_precomputeb";
  genericLogger_t  *genericLoggerp;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  if ((marpaWrapperGrammarp->haveStartb == 0) && (marpaWrapperGrammarp->nSymboli > 0)) {
    /* Use arbitrarily first symbol as start symbol */
    marpaWrapperGrammarSymbol_t *marpaWrapperSymbolp = marpaWrapperGrammarp->symbolpp[0];

    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_start_symbol_set(%p, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi);
    if (marpa_g_start_symbol_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi) < 0) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
    marpaWrapperGrammarp->haveStartb = 1;
  }

  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_precompute(%p)", marpaWrapperGrammarp->marpaGrammarp);
  if (marpa_g_precompute(marpaWrapperGrammarp->marpaGrammarp) < 0) {
    MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Events can happen */
  return _marpaWrapperGrammar_eventb(marpaWrapperGrammarp);

 err:
  return 0;
}

/****************************************************************************/
static short _marpaWrapperGrammar_eventb(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char                 funcs[] = "_marpaWrapperGrammar_eventb";
  genericLogger_t                  *genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;
  int                               nbEventi;
  int                               i;
  int                               subscribedEventi;
  const char                       *msgs;
  const char                       *warningMsgs;
  const char                       *fatalMsgs;
  const char                       *infoMsgs;
  Marpa_Event_Type                  eventType;
  Marpa_Event                       event;
  int                               eventValuei;
  marpaWrapperGrammarEvent_t       *eventpp;

  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_event_count(%p)", marpaWrapperGrammarp->marpaGrammarp);
  nbEventi = marpa_g_event_count(marpaWrapperGrammarp->marpaGrammarp);
  MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "Number of events: %d", nbEventi);

  if (nbEventi < 0) {
    /* No event - this is not an error */
    return 1;
  }

  eventpp = (marpaWrapperGrammarEvent_t *) malloc(nbEventi * sizeof(marpaWrapperGrammarEvent_t));
  if (eventpp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  /* Get all events, with a distinction between warnings, and the subscriptions */
  for (i = 0, subscribedEventi = 0; i < nbEventi; i++) {
    
    MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_event(%p, %p, %d)", marpaWrapperGrammarp->marpaGrammarp, &event, i);
    eventType = marpa_g_event(marpaWrapperGrammarp->marpaGrammarp, &event, i);
    if (eventType < 0) {
      MARPAWRAPPERGRAMMAR_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }

    msgs = (eventType < MARPA_EVENT_COUNT) ? marpa_event_description[eventType].suggested : NULL;
    if (msgs == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "Unknown event type %d", (int) eventType);
      goto err;
    }
    GENERICLOGGER_TRACEF(genericLoggerp, "Event %d: %s", (int) eventType, msgs);

    warningMsgs = NULL;
    fatalMsgs   = NULL;
    infoMsgs    = NULL;

    switch (eventType) {
    case MARPA_EVENT_NONE:
      break;
    case MARPA_EVENT_COUNTED_NULLABLE:
      fatalMsgs = msgs;
      break;
    case MARPA_EVENT_EARLEY_ITEM_THRESHOLD:
      warningMsgs = msgs;
      break;
    case MARPA_EVENT_EXHAUSTED:
      infoMsgs = msgs;
      break;
    case MARPA_EVENT_LOOP_RULES:
      warningMsgs = msgs;
      break;
    case MARPA_EVENT_NULLING_TERMINAL:
      fatalMsgs = msgs;
      break;
    case MARPA_EVENT_SYMBOL_COMPLETED:
    case MARPA_EVENT_SYMBOL_NULLED:
    case MARPA_EVENT_SYMBOL_EXPECTED: /* Only if marpa_r_expected_symbol_event_set */
    case MARPA_EVENT_SYMBOL_PREDICTED:
      /* Event value is the id of the symbol */
      MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "marpa_g_event_value(%p)", &event);
      eventValuei = marpa_g_event_value(&event);
      eventpp[subscribedEventi].eventType = MARPAWRAPPERGRAMMAR_EVENT_COMPLETED;
      eventpp[subscribedEventi].symbolp   = marpaWrapperGrammarp->symbolpp[eventValuei];
      subscribedEventi++;
      break;
    default:
      /* These are all the events as per this version of marpa */
      GENERICLOGGER_NOTICEF(genericLoggerp, "Unsupported event type %d", (int) eventType);
      break;
    }
    if (warningMsgs != NULL) {
      if (marpaWrapperGrammarp->marpaWrapperGrammarOption.warningIsErrorb != 0) {
	GENERICLOGGER_ERROR(genericLoggerp, warningMsgs);
	goto err;
      } else {
	GENERICLOGGER_WARN(genericLoggerp, warningMsgs);
      }
    } else if (fatalMsgs != NULL) {
      GENERICLOGGER_ERROR(genericLoggerp, fatalMsgs);
      goto err;
    } else if (infoMsgs != NULL) {
      GENERICLOGGER_INFO(genericLoggerp, infoMsgs);
    }
  }


  if (subscribedEventi > 0) {
    if (subscribedEventi > 1) {
      /* Sort the events */
      qsort(eventpp, subscribedEventi, sizeof(marpaWrapperGrammarEvent_t), &_marpaWrapperGrammar_cmpi);
    }
    if (marpaWrapperGrammarp->marpaWrapperGrammarOption.eventCallbackp != NULL) {
      MARPAWRAPPERGRAMMAR_TRACEF(genericLoggerp, funcs, "eventCallbackp(%p, %p, %d, %p)", marpaWrapperGrammarp->marpaWrapperGrammarOption.eventCallbackDatavp, marpaWrapperGrammarp, subscribedEventi, eventpp);
      (*marpaWrapperGrammarp->marpaWrapperGrammarOption.eventCallbackp)(marpaWrapperGrammarp->marpaWrapperGrammarOption.eventCallbackDatavp, marpaWrapperGrammarp, subscribedEventi, eventpp);
    }
  }

  free(eventpp);

  return 1;

 err:

  if (eventpp != NULL) {
    int errnoi = errno;
    free(eventpp);
    errno = errnoi;
  }
  return 0;
}

/****************************************************************************/
static int _marpaWrapperGrammar_cmpi(const void *event1p, const void *event2p)
/****************************************************************************/
{
  int w1i = MARPAWRAPPERGRAMMAREVENT_WEIGHT(((marpaWrapperGrammarEvent_t *) event1p)->eventType);
  int w2i = MARPAWRAPPERGRAMMAREVENT_WEIGHT(((marpaWrapperGrammarEvent_t *) event2p)->eventType);

  return (w1i < w2i) ? -1 : ((w1i > w2i) ? 1 : 0);
}

