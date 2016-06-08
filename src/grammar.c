#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/manageBuf.h"
#include "marpaWrapper/internal/logging.h"
#include "marpaWrapper/internal/grammar.h"

static marpaWrapperGrammarOption_t marpaWrapperGrammarOptionDefault = {
  NULL,    /* genericLoggerp             */
  0,       /* warningIsErrorb            */
  0        /* warningIsIgnoredb          */
};

static marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOptionDefault = {
  0,                                  /* terminalb */
  0,                                  /* startb */
  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE  /* eventSeti */
};

static marpaWrapperGrammarRuleOption_t marpaWrapperGrammarRuleOptionDefault = {
  0,    /* ranki */
  0,    /* nullRanksHighb */
  0,    /* sequenceb */
  -1,   /* separatorSymboli */
  0,    /* properb */
  0     /* minimumi */
};

#define MARPAWRAPPERGRAMMAREVENT_WEIGHT(eventType) ((eventType) == MARPAWRAPPERGRAMMAR_EVENT_COMPLETED) ? -1 : (((eventType) == MARPAWRAPPERGRAMMAR_EVENT_NULLED) ? 0 : 1)
static inline int _marpaWrapperGrammar_cmpi(const void *event1p, const void *event2p);

/****************************************************************************/
marpaWrapperGrammar_t *marpaWrapperGrammar_newp(marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp)
/****************************************************************************/
{
  const static char      funcs[] = "marpaWrapperGrammar_newp";
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
  genericLogger_t       *genericLoggerp = NULL;
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
      MARPAWRAPPER_ERROR(genericLoggerp, "marpa_version failure");
      goto err;
    }
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_version: %d.%d.%d", marpaVersionip[0], marpaVersionip[1], marpaVersionip[2]);
  }
#endif

  marpaWrapperGrammarp = malloc(sizeof(marpaWrapperGrammar_t));
  if (marpaWrapperGrammarp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  marpaWrapperGrammarp->precomputedb              = 0;
  marpaWrapperGrammarp->haveStartb                = 0;
  marpaWrapperGrammarp->marpaWrapperGrammarOption = *marpaWrapperGrammarOptionp;
  marpaWrapperGrammarp->marpaGrammarp             = NULL;
  /* See first instruction after this initialization block: marpaWrapperGrammarp->marpaConfig */
  marpaWrapperGrammarp->sizeSymboll               = 0;
  marpaWrapperGrammarp->nSymboll                  = 0;
  marpaWrapperGrammarp->symbolArrayp              = NULL;
  marpaWrapperGrammarp->sizeRulel                 = 0;
  marpaWrapperGrammarp->nRulel                    = 0;
  marpaWrapperGrammarp->ruleArrayp                = NULL;
  marpaWrapperGrammarp->sizeEventl                = 0;
  marpaWrapperGrammarp->nEventl                   = 0;
  marpaWrapperGrammarp->eventArrayp               = NULL;

  /* Initialize Marpa - always succeed as per the doc */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_c_init(%p)", &(marpaWrapperGrammarp->marpaConfig));
  marpa_c_init(&(marpaWrapperGrammarp->marpaConfig));

  /* Create a grammar instance */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_new(%p)", &(marpaWrapperGrammarp->marpaConfig));
  marpaWrapperGrammarp->marpaGrammarp = marpa_g_new(&(marpaWrapperGrammarp->marpaConfig));
  if (marpaWrapperGrammarp->marpaGrammarp == NULL) {
    MARPAWRAPPER_MARPA_C_ERROR(genericLoggerp, &(marpaWrapperGrammarp->marpaConfig));
    goto err;
  }

  /* Turn off obsolete features as per the doc */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_force_valued(%p)", marpaWrapperGrammarp->marpaGrammarp);
  if (marpa_g_force_valued(marpaWrapperGrammarp->marpaGrammarp) < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (genericLoggerp != NULL) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Cloning genericLogger");

    marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp = GENERICLOGGER_CLONE(genericLoggerp);
    if (marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Failed to clone genericLogger: %s", strerror(errno));
      goto err;
    }
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperGrammarp);
  return marpaWrapperGrammarp;

 err:
  if (marpaWrapperGrammarp != NULL) {
    int errnoi = errno;

    if ((genericLoggerp != NULL) &&
        (marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp != NULL) &&
        (marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp != genericLoggerp)) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned genericLogger");
      GENERICLOGGER_FREE(marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp);
    }
    marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp = NULL;
    marpaWrapperGrammar_freev(marpaWrapperGrammarp);

    errno = errnoi;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
void marpaWrapperGrammar_freev(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperGrammar_freev";
  genericLogger_t  *genericLoggerp;

  if (marpaWrapperGrammarp != NULL) {
    /* Keep a copy of the generic logger. If original is not NULL, then we have a clone of it */
    genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

    if (marpaWrapperGrammarp->marpaGrammarp != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_unref(%p)", marpaWrapperGrammarp->marpaGrammarp);
      marpa_g_unref(marpaWrapperGrammarp->marpaGrammarp);
    }

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing symbol table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperGrammarp->symbolArrayp));

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing rule table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperGrammarp->ruleArrayp));

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing last list of events");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperGrammarp->eventArrayp));

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperGrammarp);
    free(marpaWrapperGrammarp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

/****************************************************************************/
int marpaWrapperGrammar_newSymboli(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/****************************************************************************/
{
  const static char            funcs[] = "marpaWrapperGrammar_newSymboli";
  Marpa_Symbol_ID              marpaSymbolIdi;
  genericLogger_t             *genericLoggerp = NULL;
  size_t                       nSymboll;
  marpaWrapperGrammarSymbol_t *marpaWrapperSymbolp;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  /* Create symbol */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_new(%p)", marpaWrapperGrammarp->marpaGrammarp);
  marpaSymbolIdi = marpa_g_symbol_new(marpaWrapperGrammarp->marpaGrammarp);
  if (marpaSymbolIdi == -2) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Allocate room for the new symbol */
  nSymboll = marpaSymbolIdi + 1;
  if (manageBuf_createp(genericLoggerp,
			(void **) &(marpaWrapperGrammarp->symbolArrayp),
			&(marpaWrapperGrammarp->sizeSymboll),
			nSymboll,
			sizeof(marpaWrapperGrammarSymbol_t)) == NULL) {
    goto err;
  }

  marpaWrapperSymbolp = &(marpaWrapperGrammarp->symbolArrayp[marpaSymbolIdi]);
  marpaWrapperGrammarp->nSymboll = nSymboll;

  /* Fill the symbol structure */
  marpaWrapperSymbolp->marpaSymbolIdi = marpaSymbolIdi;
  if (marpaWrapperGrammarSymbolOptionp == NULL) {
    marpaWrapperGrammarSymbolOptionp = &marpaWrapperGrammarSymbolOptionDefault;
  }
  marpaWrapperSymbolp->marpaWrapperGrammarSymbolOption = *marpaWrapperGrammarSymbolOptionp;

  /* Apply options */
  if (marpaWrapperGrammarSymbolOptionp->terminalb != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_terminal_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_terminal_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if (marpaWrapperGrammarSymbolOptionp->startb != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_start_symbol_set(%p, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi);
    if (marpa_g_start_symbol_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi) < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
    marpaWrapperGrammarp->haveStartb = 1;
  }

  if ((marpaWrapperGrammarSymbolOptionp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_completion_event_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_completion_event_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if ((marpaWrapperGrammarSymbolOptionp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) == MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_nulled_event_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_nulled_event_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if ((marpaWrapperGrammarSymbolOptionp->eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_symbol_is_prediction_event_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi, (int) 1);
    if (marpa_g_symbol_is_prediction_event_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi, 1) != 1) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  marpaWrapperSymbolp->isLexemeb = 0;
  marpaWrapperSymbolp->lengthl   = 0;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", (int) marpaSymbolIdi);
  return (int) marpaSymbolIdi;
  
 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
}

/****************************************************************************/
int marpaWrapperGrammar_newSymbolExti(marpaWrapperGrammar_t *marpaWrapperGrammarp, short terminalb, short startb, int eventSeti)
/****************************************************************************/
{
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;

  marpaWrapperGrammarSymbolOption.terminalb           = terminalb;
  marpaWrapperGrammarSymbolOption.startb              = startb;
  marpaWrapperGrammarSymbolOption.eventSeti           = eventSeti;

  return marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolOption);
}

/****************************************************************************/
int marpaWrapperGrammar_newRulei(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperGrammarRuleOption_t *marpaWrapperGrammarRuleOptionp,
                                 int lhsSymboli,
                                 size_t rhsSymboll, int *rhsSymbolip
                                 )
/****************************************************************************/
{
  const static char          funcs[] = "marpaWrapperGrammar_newRulei";
  Marpa_Rule_ID              marpaRuleIdi;
  genericLogger_t           *genericLoggerp = NULL;
  marpaWrapperGrammarRule_t *marpaWrapperRulep;
  int                        sequenceFlagsi;
  size_t                     nRulel;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  /* Check parameters - Some depend on marpaWrapperGrammarRuleOptionp */
  if (marpaWrapperGrammarRuleOptionp == NULL) {
    marpaWrapperGrammarRuleOptionp = &marpaWrapperGrammarRuleOptionDefault;
  }

  if (lhsSymboli < 0) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Bad LHS symbol Id");
    goto err;
  }
  if (marpaWrapperGrammarRuleOptionp->sequenceb != 0) {
    if (rhsSymboll != 1) {
      MARPAWRAPPER_ERROR(genericLoggerp, "A sequence must have exactly one RHS");
      goto err;
    }
    if ((marpaWrapperGrammarRuleOptionp->minimumi != 0) && (marpaWrapperGrammarRuleOptionp->minimumi != 1)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "A sequence must have a minimum of exactly 0 or 1");
      goto err;
    }
  }

  /* Create rule; it either a sequence, either a list of RHS */
  if (marpaWrapperGrammarRuleOptionp->sequenceb != 0) {
    sequenceFlagsi = 0;
    if (marpaWrapperGrammarRuleOptionp->properb != 0) {
      sequenceFlagsi |= MARPA_PROPER_SEPARATION;
    }
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_sequence_new(%p, %d, %d, %d, %d, %s)", marpaWrapperGrammarp->marpaGrammarp, lhsSymboli, (int) rhsSymbolip[0], (int) marpaWrapperGrammarRuleOptionp->separatorSymboli, marpaWrapperGrammarRuleOptionp->minimumi, ((sequenceFlagsi & MARPA_PROPER_SEPARATION) == MARPA_PROPER_SEPARATION) ? "MARPA_PROPER_SEPARATION" : "0");
    marpaRuleIdi = marpa_g_sequence_new(marpaWrapperGrammarp->marpaGrammarp,
					lhsSymboli,
					(Marpa_Symbol_ID) rhsSymbolip[0],
					(Marpa_Symbol_ID) marpaWrapperGrammarRuleOptionp->separatorSymboli,
					marpaWrapperGrammarRuleOptionp->minimumi,
					sequenceFlagsi);
  } else {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_new(%p, %d, %p, %d)", marpaWrapperGrammarp->marpaGrammarp, lhsSymboli, rhsSymbolip, (int) rhsSymboll);
    marpaRuleIdi = marpa_g_rule_new(marpaWrapperGrammarp->marpaGrammarp,
				    lhsSymboli,
				    (Marpa_Symbol_ID *) rhsSymbolip,
				    rhsSymboll);
  }

  if (marpaRuleIdi == -2) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Apply options */
  if (marpaWrapperGrammarRuleOptionp->ranki != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_rank_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaRuleIdi, marpaWrapperGrammarRuleOptionp->ranki);
    marpa_g_rule_rank_set(marpaWrapperGrammarp->marpaGrammarp, marpaRuleIdi, marpaWrapperGrammarRuleOptionp->ranki);
    if (marpa_g_error(marpaWrapperGrammarp->marpaGrammarp, NULL) != MARPA_ERR_NONE) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if (marpaWrapperGrammarRuleOptionp->nullRanksHighb != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_null_high_set(%p, %d, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaRuleIdi, 1);
    if (marpa_g_rule_null_high_set(marpaWrapperGrammarp->marpaGrammarp, marpaRuleIdi, 1) < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  /* Allocate room for the new symbol */
  nRulel = marpaRuleIdi + 1;
  if (manageBuf_createp(genericLoggerp,
			(void **) &(marpaWrapperGrammarp->ruleArrayp),
			&(marpaWrapperGrammarp->sizeRulel),
			nRulel,
			sizeof(marpaWrapperGrammarRule_t)) == NULL) {
    goto err;
  }

  marpaWrapperRulep = &(marpaWrapperGrammarp->ruleArrayp[marpaRuleIdi]);
  marpaWrapperRulep->marpaRuleIdi                  = marpaRuleIdi;
  marpaWrapperRulep->marpaWrapperGrammarRuleOption = *marpaWrapperGrammarRuleOptionp;

  marpaWrapperGrammarp->nRulel = nRulel;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", (int) marpaRuleIdi);
  return (int) marpaRuleIdi;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
}

/****************************************************************************/
int marpaWrapperGrammar_newRuleExti(marpaWrapperGrammar_t *marpaWrapperGrammarp, int ranki, short nullRanksHighb, int lhsSymboli, ...)
/****************************************************************************/
{
  const static char               funcs[] = "marpaWrapperGrammar_newRuleExti";
  marpaWrapperGrammarRuleOption_t marpaWrapperGrammarRuleOption;
  genericLogger_t                *genericLoggerp = NULL;
  size_t                          sizeSymboll = 0;
  size_t                          nSymboll = 0;
  int                            *rhsSymbolip = NULL;
  int                             rhsSymboli;
  int                             rulei;
  va_list                         ap;
  
  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  marpaWrapperGrammarRuleOption.ranki            = ranki;
  marpaWrapperGrammarRuleOption.nullRanksHighb   = nullRanksHighb;
  marpaWrapperGrammarRuleOption.sequenceb        = 0;
  marpaWrapperGrammarRuleOption.separatorSymboli = -1;
  marpaWrapperGrammarRuleOption.properb          = 0;
  marpaWrapperGrammarRuleOption.minimumi         = 0;

  va_start(ap, lhsSymboli);
  while ((rhsSymboli = va_arg(ap, int)) >= 0) {
    if (manageBuf_createp(genericLoggerp, (void **) &rhsSymbolip, &sizeSymboll, nSymboll + 1, sizeof(marpaWrapperGrammarSymbol_t)) == NULL) {
      goto err;
    }
    rhsSymbolip[nSymboll++] = rhsSymboli;
  }
  va_end(ap);

  rulei = marpaWrapperGrammar_newRulei(marpaWrapperGrammarp, &marpaWrapperGrammarRuleOption, lhsSymboli, nSymboll, rhsSymbolip);
  manageBuf_freev(genericLoggerp, (void **) &rhsSymbolip);

  return rulei;

 err:
  if (rhsSymbolip != NULL) {
    int errnoi = errno;
    manageBuf_freev(genericLoggerp, (void **) &rhsSymbolip);
    errno = errnoi;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
}

/****************************************************************************/
int marpaWrapperGrammar_newSequenceExti(marpaWrapperGrammar_t *marpaWrapperGrammarp, int ranki, short nullRanksHighb,
                                        int lhsSymboli,
                                        int rhsSymboli, int minimumi, int separatorSymboli, short properb)
/****************************************************************************/
{
  marpaWrapperGrammarRuleOption_t marpaWrapperGrammarRuleOption;
  int                             rhsSymbolip[1] = { rhsSymboli };

  switch (minimumi) {
  case '*':
    minimumi = 0;
    break;
  case '+':
    minimumi = 1;
    break;
  default:
    break;
  }
  
  marpaWrapperGrammarRuleOption.ranki            = ranki;
  marpaWrapperGrammarRuleOption.nullRanksHighb   = nullRanksHighb;
  marpaWrapperGrammarRuleOption.sequenceb        = 1;
  marpaWrapperGrammarRuleOption.separatorSymboli = separatorSymboli;
  marpaWrapperGrammarRuleOption.properb          = properb;
  marpaWrapperGrammarRuleOption.minimumi         = minimumi;

  return marpaWrapperGrammar_newRulei(marpaWrapperGrammarp, &marpaWrapperGrammarRuleOption, lhsSymboli, 1, rhsSymbolip);
}

/****************************************************************************/
short marpaWrapperGrammar_precomputeb(marpaWrapperGrammar_t *marpaWrapperGrammarp)
/****************************************************************************/
{
  const static char           funcs[] = "marpaWrapperGrammar_precomputeb";
  genericLogger_t            *genericLoggerp = NULL;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  if ((marpaWrapperGrammarp->haveStartb == 0) && (marpaWrapperGrammarp->nSymboll > 0)) {
    /* Use arbitrarily first symbol as start symbol */
    marpaWrapperGrammarSymbol_t *marpaWrapperSymbolp = &(marpaWrapperGrammarp->symbolArrayp[0]);

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_start_symbol_set(%p, %d)", marpaWrapperGrammarp->marpaGrammarp, (int) marpaWrapperSymbolp->marpaSymbolIdi);
    if (marpa_g_start_symbol_set(marpaWrapperGrammarp->marpaGrammarp, marpaWrapperSymbolp->marpaSymbolIdi) < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
    marpaWrapperGrammarp->haveStartb = 1;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_precompute(%p)", marpaWrapperGrammarp->marpaGrammarp);
  if (marpa_g_precompute(marpaWrapperGrammarp->marpaGrammarp) < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Prefetch events */
  if (marpaWrapperGrammar_eventb(marpaWrapperGrammarp, NULL, NULL, 1) == 0) {
    goto err;
  }
  
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperGrammar_eventb(marpaWrapperGrammar_t *marpaWrapperGrammarp, size_t *eventlp, marpaWrapperGrammarEvent_t **eventpp, short forceReloadb)
/****************************************************************************/
{
  const static char                 funcs[] = "marpaWrapperGrammar_eventb";
  genericLogger_t                  *genericLoggerp = NULL;
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
  marpaWrapperGrammarEvent_t       *eventp;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperGrammarp->marpaWrapperGrammarOption.genericLoggerp;

  /* Events are always fetched when necessary internally. External API can nevertheless */
  /* force the refresh on demand.                                                       */
  if (forceReloadb != 0) {

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Updating cached event list");
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_event_count(%p)", marpaWrapperGrammarp->marpaGrammarp);
    nbEventi = marpa_g_event_count(marpaWrapperGrammarp->marpaGrammarp);
    if (nbEventi < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Number of events: %d", nbEventi);

    /* This variable is the number of subscribed events */
    marpaWrapperGrammarp->nEventl = 0;

    if (nbEventi > 0) {
      /* Get all events, with a distinction between warnings, and the subscriptions */
      for (i = 0, subscribedEventi = 0; i < nbEventi; i++) {
    
        MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_event(%p, %p, %d)", marpaWrapperGrammarp->marpaGrammarp, &event, i);
        eventType = marpa_g_event(marpaWrapperGrammarp->marpaGrammarp, &event, i);
        if (eventType < 0) {
          MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
          goto err;
        }

        msgs = (eventType < MARPA_EVENT_COUNT) ? marpa_event_description[eventType].suggested : NULL;
        if (msgs == NULL) {
          MARPAWRAPPER_ERRORF(genericLoggerp, "Unknown event type %d", (int) eventType);
          goto err;
        }
        MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Event %d: %s", (int) eventType, msgs);

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
          eventValuei = marpa_g_event_value(&event);
          MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_event_value(%p) returns %d", &event, eventValuei);
          if (manageBuf_createp(genericLoggerp, (void **) &(marpaWrapperGrammarp->eventArrayp), &(marpaWrapperGrammarp->sizeEventl), subscribedEventi + 1, sizeof(marpaWrapperGrammarEvent_t)) == NULL) {
            goto err;
          }
          eventp = &(marpaWrapperGrammarp->eventArrayp[subscribedEventi]);

          eventp->eventType = MARPAWRAPPERGRAMMAR_EVENT_COMPLETED;
          eventp->symboli   = marpaWrapperGrammarp->symbolArrayp[eventValuei].marpaSymbolIdi;

          marpaWrapperGrammarp->nEventl = ++subscribedEventi;
          break;
        default:
          /* These are all the events as per this version of marpa */
          MARPAWRAPPER_NOTICEF(genericLoggerp, "Unsupported event type %d", (int) eventType);
          break;
        }
        if (warningMsgs != NULL) {
          if (marpaWrapperGrammarp->marpaWrapperGrammarOption.warningIsErrorb != 0) {
            MARPAWRAPPER_ERROR(genericLoggerp, warningMsgs);
            goto err;
          } else {
            MARPAWRAPPER_WARN(genericLoggerp, warningMsgs);
          }
        } else if (fatalMsgs != NULL) {
          MARPAWRAPPER_ERROR(genericLoggerp, fatalMsgs);
          goto err;
        } else if (infoMsgs != NULL) {
          MARPAWRAPPER_INFO(genericLoggerp, infoMsgs);
        }
      }

      if (subscribedEventi > 0) {
        if (subscribedEventi > 1) {
          /* Sort the events */
          qsort(marpaWrapperGrammarp->eventArrayp, subscribedEventi, sizeof(marpaWrapperGrammarEvent_t), &_marpaWrapperGrammar_cmpi);
        }
      }

    }
  } else {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Using cached event list");
  }

  if (eventlp != NULL) {
    *eventlp = marpaWrapperGrammarp->nEventl;
  }
  if (eventpp != NULL) {
    *eventpp = (marpaWrapperGrammarp->nEventl > 0) ? marpaWrapperGrammarp->eventArrayp : NULL;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  if (eventp != NULL) {
    int errnoi = errno;
    free(eventp);
    errno = errnoi;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline int _marpaWrapperGrammar_cmpi(const void *event1p, const void *event2p)
/****************************************************************************/
{
  int w1i = MARPAWRAPPERGRAMMAREVENT_WEIGHT(((marpaWrapperGrammarEvent_t *) event1p)->eventType);
  int w2i = MARPAWRAPPERGRAMMAREVENT_WEIGHT(((marpaWrapperGrammarEvent_t *) event2p)->eventType);

  return (w1i < w2i) ? -1 : ((w1i > w2i) ? 1 : 0);
}

