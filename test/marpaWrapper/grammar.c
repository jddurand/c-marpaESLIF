#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper/grammar.h"

enum { S = 0, E, op, number, MAX_SYMBOL };

int main(int argc, char **argv) {
  marpaWrapperGrammar_t       *marpaWrapperGrammarp;
  marpaWrapperGrammar_t      *marpaWrapperGrammarClonep;
  marpaWrapperGrammarSymbol_t *symbolp[MAX_SYMBOL];
  int                         i;

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(NULL);
  
  for (i = 0; i < MAX_SYMBOL; i++) {
    marpaWrapperGrammarSymbolOption_t option;

    option.datavp = NULL;
    option.terminalb = 0;
    option.startb = (i == 0) ? 1 : 0;
    option.eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE;
    symbolp[i] = marpaWrapperGrammar_newSymbolp(marpaWrapperGrammarp, &option);
  }

  marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(marpaWrapperGrammarp);

  marpaWrapperGrammar_freev(marpaWrapperGrammarClonep);
  marpaWrapperGrammar_freev(marpaWrapperGrammarp);

  return 0;
}
