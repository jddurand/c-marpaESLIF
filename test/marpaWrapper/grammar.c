#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper.h"
#include "genericLogger.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE, MAX_RULE };
static char *desc[] = { "S", "E", "op", "number", "?" };

static char *symbolDescription(void *userDatavp, int symboli);

int main(int argc, char **argv) {
  marpaWrapperGrammar_t         *marpaWrapperGrammarp;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp;
  marpaWrapperValue_t           *marpaWrapperValuep;
  int                            symbolip[MAX_SYMBOL];
  int                            ruleip[MAX_RULE];
  int                            rci = 0;
  int                           *symbolArrayp = NULL;
  size_t                         neventl;
  size_t                         nsymboll;
  int                            i;
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  1 /* latm */
  };
  marpaWrapperValueOption_t      marpaWrapperValueOption      = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  1 /* highRankOnlyb */,
								  1 /* orderByRankb */,
								  0 /* ambiguousb */,
								  0 /* nullb */,
								  NULL /* valueRuleCallbackDefault */,
								  NULL /* valueSymbolCallbackDefault */,
								  NULL /* valueNullingCallback */
  };

  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolTerminalOption    = { 1 /* terminal */, 0 /* start */, MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION };
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolNonTerminalOption = { 0 /* terminal */, 0 /* start */, MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION };

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);
  if ( /* S (start symbol automatically), E, op, number */
      ((symbolip[     S] = marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolNonTerminalOption)) < 0) ||
      ((symbolip[     E] = marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolNonTerminalOption)) < 0) ||
      ((symbolip[    op] = marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolNonTerminalOption)) < 0) ||
      ((symbolip[number] = marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolNonTerminalOption)) < 0) ||
      /* S ::= E      */
      /* E ::= E op E */
      /* E ::= number */
      ((ruleip[ START_RULE] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[S], symbolip[E],                            -1)) < 0) ||
      ((ruleip[    OP_RULE] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[E], symbolip[E], symbolip[op], symbolip[E], -1)) < 0) ||
      ((ruleip[NUMBER_RULE] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[E], symbolip[number],                       -1)) < 0)
      ) {
    rci = 1;
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_precomputeb(marpaWrapperGrammarp) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_eventb(marpaWrapperGrammarp, &neventl, NULL, 0) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
    if (marpaWrapperRecognizerp == NULL) {
      rci = 1;
    }
  }

  /* ------ */
  /* number */
  /* ------ */
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, 1, GENERICLOGGER_LOGLEVEL_INFO, (void *) symbolip, symbolDescription) == 0) {
      rci = 1;
    }
  }
  /* -- */
  /* op */
  /* -- */
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], 2 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, 1, GENERICLOGGER_LOGLEVEL_INFO, (void *) symbolip, symbolDescription) == 0) {
      rci = 1;
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], 3 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, 1, GENERICLOGGER_LOGLEVEL_INFO, (void *) symbolip, symbolDescription) == 0) {
      rci = 1;
    }
  }

  if (rci == 0) {
    if (marpaWrapperRecognizer_event_onoffb(marpaWrapperRecognizerp, symbolip[S], MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION, 0) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nsymboll, &symbolArrayp) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "Number of expected symbols: %ld", (unsigned long) nsymboll);
      if (nsymboll > 0) {
	for (i = 0; i < nsymboll; i++) {
	  GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: %d", i, symbolArrayp[i]);
	}
      }
    }
  }

  if (rci == 0) {
    marpaWrapperValuep = marpaWrapperValue_newp(marpaWrapperRecognizerp, &marpaWrapperValueOption);
    if (marpaWrapperValuep == NULL) {
      rci = 1;
    }
  }

  marpaWrapperValue_freev(marpaWrapperValuep);
  GENERICLOGGER_FREE(marpaWrapperValueOption.genericLoggerp);

  marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  GENERICLOGGER_FREE(marpaWrapperRecognizerOption.genericLoggerp);

  marpaWrapperGrammar_freev(marpaWrapperGrammarp);
  GENERICLOGGER_FREE(marpaWrapperGrammarOption.genericLoggerp);

  return rci;
}

static char *symbolDescription(void *userDatavp, int symboli)
{
  int *symbolip = (int *) userDatavp;
  int  i;

  for (i = 0; i < MAX_SYMBOL; i++) {
    if (symboli == symbolip[i]) {
      return desc[i];
    }
  }

  return desc[MAX_SYMBOL];
}

