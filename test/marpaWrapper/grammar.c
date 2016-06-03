#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper.h"
#include "genericLogger.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE, MAX_RULE };

int main(int argc, char **argv) {
  marpaWrapperGrammar_t         *marpaWrapperGrammarp;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp;
  int                            symbolip[MAX_SYMBOL];
  int                            ruleip[MAX_RULE];
  int                            rci = 0;
  int                           *symbolArrayp = NULL;
  size_t                         nsymboll;
  int                            i;
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE), 0, 0 };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE), 0 };
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption = { 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION };

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);
  if ( /* S (start symbol automatically), E, op, number */
      ((symbolip[     S] = marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolOption)) < 0) ||
      ((symbolip[     E] = marpaWrapperGrammar_newSymboli(marpaWrapperGrammarp, &marpaWrapperGrammarSymbolOption)) < 0) ||
      ((symbolip[    op] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[number] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      /* S ::= E*     */
      /* E ::= E op E */
      /* E ::= number */
      ((ruleip[ START_RULE] = MARPAWRAPPERGRAMMAR_NEWSEQUENCE(marpaWrapperGrammarp, symbolip[S], symbolip[E], '*')) < 0) ||
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
    if (marpaWrapperGrammar_eventl(marpaWrapperGrammarp, NULL) == (size_t)-1) {
      rci = 1;
    }
  }
  
  marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
  marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[E], 0, 0);
  marpaWrapperRecognizer_event_onoffb(marpaWrapperRecognizerp, symbolip[S], MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION, 0);
  nsymboll = marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &symbolArrayp);
  GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "Number of expected symbols: %ld", (unsigned long) nsymboll);
  if (nsymboll > 0) {
    for (i = 0; i < nsymboll; i++) {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: %d", i, symbolArrayp[i]);
    }
  }
  marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  GENERICLOGGER_FREE(marpaWrapperRecognizerOption.genericLoggerp);

  marpaWrapperGrammar_freev(marpaWrapperGrammarp);
  GENERICLOGGER_FREE(marpaWrapperGrammarOption.genericLoggerp);

  return rci;
}
