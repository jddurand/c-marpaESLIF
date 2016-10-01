#ifndef MARPAESLIF_INTERNAL_GRAMMAR_H
#define MARPAESLIF_INTERNAL_GRAMMAR_H

#include <genericStack.h>

typedef enum   marpaESLIF_symbol_type marpaESLIF_symbol_type_t;
typedef struct marpaESLIF_terminal    marpaESLIF_terminal_t;
typedef struct marpaESLIF_symbol      marpaESLIF_symbol_t;
typedef struct marpaESLIF_rule        marpaESLIF_rule_t;
typedef struct marpaESLIF_grammar     marpaESLIF_grammar_t;

/* Symbol types */
enum marpaESLIF_symbol_type {
  MARPAESLIF_SYMBOL_TYPE_NA = 0,
  MARPAESLIF_SYMBOL_TYPE_TERMINAL,
  MARPAESLIF_SYMBOL_TYPE_META
};

/* A terminal */
struct marpaESLIF_terminal {
  int                      idi;   /* Terminal Id */
  char                    *descs; /* Terminal description */
};

/* A symbol */
struct marpaESLIF_symbol {
  int                      idi;   /* Symbol Id */
  char                    *descs; /* Symbol description */
  marpaESLIF_symbol_type_t type;  /* Symbol type */
  union {
    marpaESLIF_terminal_t *terminalp; /* Symbol is a terminal */
    marpaESLIF_rule_t     *rulep;     /* Symbol is a meta identifier, i.e. a rule */
  } u;
  short shallowb;                 /* If true, pointer above is a shallow copy */
};

/* A rule */
struct marpaESLIF_rule {
  int                  idi;             /* Rule Id */
  char                *descs;           /* Rule description */
  marpaESLIF_symbol_t *lhsp;            /* LHS symbol */
  genericStack_t      *rhsStackp;       /* Stack of RHS symbols */
  genericStack_t      *exceptionStackp; /* Stack of Exceptions symbols */
};

/* A grammar */
struct marpaESLIF_grammar {
  int                   idi;          /* Grammar Id */
  char                 *descs;        /* Grammar description */
  genericStack_t       *symbolStackp; /* Stack of symbols */
  genericStack_t       *ruleStackp;   /* Stack of rules */
  marpaESLIF_grammar_t *previousp;    /* Previous eventual grammar */
  marpaESLIF_grammar_t *nextp;        /* Lexer is a sub-grammar */
};

#endif /* MARPAESLIF_INTERNAL_GRAMMAR_H */
