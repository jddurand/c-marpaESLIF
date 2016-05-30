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
  symbolp[     S] = marpaWrapperGrammar_newSymbolExtp(marpaWrapperGrammarp, NULL, 0, 1, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE);
  symbolp[     E] = marpaWrapperGrammar_newSymbolExtp(marpaWrapperGrammarp, NULL, 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE);
  symbolp[    op] = marpaWrapperGrammar_newSymbolExtp(marpaWrapperGrammarp, NULL, 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE);
  symbolp[number] = marpaWrapperGrammar_newSymbolExtp(marpaWrapperGrammarp, NULL, 0, 0, MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE);
  
  /* S ::= E */
  marpaWrapperGrammar_newRuleExtp(marpaWrapperGrammarp, (void *) START_RULE,  0, 0, symbolp[S], symbolp[E], NULL);
  /* E ::= E op E */
  marpaWrapperGrammar_newRuleExtp(marpaWrapperGrammarp, (void *) OP_RULE,     0, 0, symbolp[E], symbolp[E], symbolp[op], symbolp[E], NULL);
  /* E ::= number */
  marpaWrapperGrammar_newRuleExtp(marpaWrapperGrammarp, (void *) NUMBER_RULE, 0, 0, symbolp[E], symbolp[number], NULL);

  marpaWrapperGrammar_precomputeb(marpaWrapperGrammarp);
  marpaWrapperGrammar_freev(marpaWrapperGrammarp);

  return 0;
}
