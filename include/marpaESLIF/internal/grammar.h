#ifndef MARPAESLIF_INTERNAL_GRAMMAR_H
#define MARPAESLIF_INTERNAL_GRAMMAR_H

#include <genericStack.h>

/* A symbol */
typedef struct marpaESLIF_symbol {
  int idi;                          /* Symbol Id */
  char *descs;                      /* Symbol description */
} marpaESLIF_symbol_t;

/* A rule */
typedef struct marpaESLIF_rule {
  int idi;                                /* Rule Id */
  char *descs;                            /* Rule description */
  marpaESLIF_symbol_t lhsSymbol;          /* LHS */
  genericStack_t     *rhsSymbolStackp;    /* Stack of RHS */
  genericStack_t     *rhsExceptionStackp; /* Stack of Exceptions */
} marpaESLIF_rule_t;

/* A grammar */
typedef struct marpaESLIF_grammar {
  int                        idi;        /* Grammar Id */
  char                      *descs;      /* Grammar description */
  genericStack_t            *ruleStackp; /* Stack of rules */
  struct marpaESLIF_grammar *subGrammar; /* Lexer is a sub-grammar */
} marpaESLIF_rule_t;

#endif /* MARPAESLIF_INTERNAL_GRAMMAR_H */
