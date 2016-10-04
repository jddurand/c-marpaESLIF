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

/* Regexp modifiers - we take JPCRE2 matching semantics, c.f. https://neurobin.org/projects/softwares/libs/jpcre2/ */
typedef enum marpaESLIF_regexp_option {
  MARPAESLIF_REGEXP_OPTION_NA                  = 0x000,
  MARPAESLIF_REGEXP_OPTION_MATCH_UNSET_BACKREF = 0x001, /* e */
  MARPAESLIF_REGEXP_OPTION_CASELESS            = 0x002, /* i */
  MARPAESLIF_REGEXP_OPTION_JAVASCRIPT          = 0x004, /* j */
  MARPAESLIF_REGEXP_OPTION_MULTILINE           = 0x008, /* m */
  MARPAESLIF_REGEXP_OPTION_UCP                 = 0x010, /* n */
  MARPAESLIF_REGEXP_OPTION_DOTALL              = 0x020, /* s */
  MARPAESLIF_REGEXP_OPTION_EXTENDED            = 0x040, /* x */
  MARPAESLIF_REGEXP_OPTION_ANCHORED            = 0x080, /* A */
  MARPAESLIF_REGEXP_OPTION_DOLLAR_ENDONLY      = 0x100, /* D */
  MARPAESLIF_REGEXP_OPTION_DUPNAMES            = 0x200, /* J */
  MARPAESLIF_REGEXP_OPTION_JIT                 = 0x400, /* S - handled programmatically */
  MARPAESLIF_REGEXP_OPTION_UNGREEDY            = 0x800  /* U */
} marpaESLIF_regexp_option_t;

typedef enum marpaESLIF_regexp_option_id {
  MARPAESLIF_REGEXP_OPTION_MATCH_UNSET_BACKREF_ID = 0,
  MARPAESLIF_REGEXP_OPTION_CASELESS_ID,
  MARPAESLIF_REGEXP_OPTION_JAVASCRIPT_ID,
  MARPAESLIF_REGEXP_OPTION_MULTILINE_ID,
  MARPAESLIF_REGEXP_OPTION_UCP_ID,
  MARPAESLIF_REGEXP_OPTION_DOTALL_ID,
  MARPAESLIF_REGEXP_OPTION_EXTENDED_ID,
  MARPAESLIF_REGEXP_OPTION_ANCHORED_ID,
  MARPAESLIF_REGEXP_OPTION_DOLLAR_ENDONLY_ID,
  MARPAESLIF_REGEXP_OPTION_DUPNAMES_ID,
  MARPAESLIF_REGEXP_OPTION_JIT_ID,
  MARPAESLIF_REGEXP_OPTION_UNGREEDY_ID,
  _MARPAESLIF_REGEXP_OPTION_UNGREEDY_ID_MAX
} marpaESLIF_regexp_option_id_t;

struct marpaESLIF_regexp_option_map {
  marpaESLIF_regexp_option_t opti;
  char                      *modifiers;
  marpaESLIF_uint32_t        pcre2Optioni;
} marpaESLIF_regexp_option_map[] = {
  { MARPAESLIF_REGEXP_OPTION_MATCH_UNSET_BACKREF, "e", PCRE2_MATCH_UNSET_BACKREF },
  { MARPAESLIF_REGEXP_OPTION_CASELESS,            "i", PCRE2_CASELESS },
  { MARPAESLIF_REGEXP_OPTION_JAVASCRIPT,          "j", PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF },
  { MARPAESLIF_REGEXP_OPTION_MULTILINE,           "m", PCRE2_MULTILINE },
  { MARPAESLIF_REGEXP_OPTION_UCP,                 "n", PCRE2_UCP },
  { MARPAESLIF_REGEXP_OPTION_DOTALL,              "s", PCRE2_DOTALL },
  { MARPAESLIF_REGEXP_OPTION_EXTENDED,            "x", PCRE2_EXTENDED },
  { MARPAESLIF_REGEXP_OPTION_ANCHORED,            "A", PCRE2_ANCHORED },
  { MARPAESLIF_REGEXP_OPTION_DOLLAR_ENDONLY,      "D", PCRE2_DOLLAR_ENDONLY },
  { MARPAESLIF_REGEXP_OPTION_DUPNAMES,            "J", PCRE2_DUPNAMES },
  { MARPAESLIF_REGEXP_OPTION_UNGREEDY,            "U", PCRE2_UNGREEDY }
};

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
