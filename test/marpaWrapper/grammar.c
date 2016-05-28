#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper/grammar.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE };

int main(int argc, char **argv) {
  marpaWrapperGrammar_t       *marpaWrapperGrammarp;
  marpaWrapperGrammarSymbol_t *symbolp[MAX_SYMBOL];
  marpaWrapperGrammarSymbol_t *rhsSymbolp[3]; /* Just to have enough room */
  int                         i;

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(NULL);
  
  for (i = 0; i < MAX_SYMBOL; i++) {
    /* First symbol, i.e. E, will automatically become the start symbol */
    symbolp[i] = marpaWrapperGrammar_newSymbolp(marpaWrapperGrammarp, NULL);
  }

  /* S ::= E */
  {
    marpaWrapperGrammarRuleOption_t option;

    option.datavp           = (void *) START_RULE;
    option.ranki            = 0;
    option.nullRanksHighb   = 0;
    option.sequenceb        = 0;
    option.separatorSymbolp = NULL;
    option.properb          = 0;
    option.minimumi         = 0;

    rhsSymbolp[0]         = symbolp[E];
    marpaWrapperGrammar_newRulep(marpaWrapperGrammarp, &option, symbolp[S], 1, rhsSymbolp);
  }

  /* E ::= E op E */
  {
    marpaWrapperGrammarRuleOption_t option;

    option.datavp           = (void *) OP_RULE;
    option.ranki            = 0;
    option.nullRanksHighb   = 0;
    option.sequenceb        = 0;
    option.separatorSymbolp = NULL;
    option.properb          = 0;
    option.minimumi         = 0;

    rhsSymbolp[0]         = symbolp[ E];
    rhsSymbolp[1]         = symbolp[op];
    rhsSymbolp[2]         = symbolp[ E];
    marpaWrapperGrammar_newRulep(marpaWrapperGrammarp, &option, symbolp[E], 3, rhsSymbolp);
  }

  /* E ::= number */
  {
    marpaWrapperGrammarRuleOption_t option;

    option.datavp           = (void *) NUMBER_RULE;
    option.ranki            = 0;
    option.nullRanksHighb   = 0;
    option.sequenceb        = 0;
    option.separatorSymbolp = NULL;
    option.properb          = 0;
    option.minimumi         = 0;

    rhsSymbolp[0]         = symbolp[number];
    marpaWrapperGrammar_newRulep(marpaWrapperGrammarp, &option, symbolp[S], 1, rhsSymbolp);
  }

  marpaWrapperGrammar_precomputeb(marpaWrapperGrammarp);
  marpaWrapperGrammar_freev(marpaWrapperGrammarp);

  return 0;
}
