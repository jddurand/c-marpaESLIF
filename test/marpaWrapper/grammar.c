#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper/grammar.h"

int main(int argc, char **argv) {
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
  marpaWrapperGrammar_t *marpaWrapperGrammarClonep;

  marpaWrapperGrammarp      = marpaWrapperGrammar_newp(NULL);
  marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(marpaWrapperGrammarp);

  marpaWrapperGrammar_freev(marpaWrapperGrammarClonep);
  marpaWrapperGrammar_freev(marpaWrapperGrammarp);

  return 0;
}
