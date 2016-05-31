#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper/grammar.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE, MAX_RULE };

int main(int argc, char **argv) {
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
  int                    symbolip[MAX_SYMBOL];
  int                    ruleip[MAX_RULE];
  int                    rci = 0;

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(NULL);
  if ( /* S (start symbol automatically), E, op, number */
      ((symbolip[     S] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[     E] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    op] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[number] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
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
    if (marpaWrapperGrammar_eventl(marpaWrapperGrammarp, NULL) == (size_t)-1) {
      rci = 1;
    }
  }
  marpaWrapperGrammar_freev(marpaWrapperGrammarp);

  return rci;
}
