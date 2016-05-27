#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper/grammar.h"

int main(int argc, char **argv) {
  marpaWrapperGrammar_t *marpaWrapperGrammarp;

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(NULL);

  fprintf(stderr, "OK: %p\n", marpaWrapperGrammarp);

  return 0;
}
