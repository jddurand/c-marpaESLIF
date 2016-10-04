#ifndef MARPAESLIF_INTERNAL_GRAMMAR_H
#define MARPAESLIF_INTERNAL_GRAMMAR_H

#include <genericStack.h>
#include <pcre2.h>

typedef struct marpaESLIF_regexp        marpaESLIF_regexp_t;
typedef struct marpaESLIF_string        marpaESLIF_string_t;
typedef enum   marpaESLIF_symbol_type   marpaESLIF_symbol_type_t;
typedef enum   marpaESLIF_terminal_type marpaESLIF_terminal_type_t;
typedef struct marpaESLIF_terminal      marpaESLIF_terminal_t;
typedef struct marpaESLIF_symbol        marpaESLIF_symbol_t;
typedef struct marpaESLIF_rule          marpaESLIF_rule_t;
typedef struct marpaESLIF_grammar       marpaESLIF_grammar_t;

/* Symbol types */
enum marpaESLIF_symbol_type {
  MARPAESLIF_SYMBOL_TYPE_NA = 0,
  MARPAESLIF_SYMBOL_TYPE_TERMINAL,
  MARPAESLIF_SYMBOL_TYPE_META
};

/* Terminal types */
enum marpaESLIF_terminal_type {
  MARPAESLIF_TERMINAL_TYPE_NA = 0,
  MARPAESLIF_TERMINAL_TYPE_STRING,   /* String */
  MARPAESLIF_TERMINAL_TYPE_REGEXP    /* Regular expression */
};

/* A terminal */
typedef void *(*marpaESLIF_terminal_initializer_t)(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);
typedef PCRE2_SPTR *(*marpaESLIF_terminal_matcher_t)(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);

/* A string */
struct marpaESLIF_string {
  char   *stringp; /* UTF-8 string */
  size_t  stringl; /* UTF-8 length in bytes */
};

/* A regexp */
typedef enum marpaESLIF_regexp_option {
  MARPAESLIF_REGEXP_OPTION_NA        = 0x00,
  MARPAESLIF_REGEXP_OPTION_CASELESS  = 0x01, /* i modifier */
  MARPAESLIF_REGEXP_OPTION_MULTILINE = 0x02, /* m modifier */
  MARPAESLIF_REGEXP_OPTION_DOTALL    = 0x04  /* s modifier */
} marpaESLIF_regexp_option;
struct marpaESLIF_regexp {
  pcre2_code *regexp;         /* Compiled regexp */
#ifdef PCRE2_CONFIG_JIT
  short       jitCompleteb;   /* Eventual optimized JIT */
  short       jitPartialb;
#endif
};

struct marpaESLIF_terminal {
  int                        idi;                 /* Terminal Id */
  char                      *descs;               /* Terminal description */
  marpaESLIF_terminal_type_t type;                /* Terminal type */
  union {
    marpaESLIF_regexp_t     regex;
    marpaESLIF_string_t     string;
  } u;
  marpaESLIF_terminal_initializer_t initializerp; /* Run-time initializer */
  marpaESLIF_terminal_matcher_t     matcherp;     /* Run-time matcher */
};

/* A symbol */
struct marpaESLIF_symbol {
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
  marpaWrapperGrammar_t *marpaWrapperGrammarp; /* Grammar implementation */
  genericStack_t       *symbolStackp;          /* Stack of symbols */
  genericStack_t       *ruleStackp;            /* Stack of rules */
  marpaESLIF_grammar_t *previousp;             /* Previous eventual grammar */
  marpaESLIF_grammar_t *nextp;                 /* Lexer is a sub-grammar */
};

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_GRAMMAR_H */
