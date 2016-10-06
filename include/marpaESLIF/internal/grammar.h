#ifndef MARPAESLIF_INTERNAL_GRAMMAR_H
#define MARPAESLIF_INTERNAL_GRAMMAR_H

#include <genericStack.h>
#include <pcre2.h>

typedef struct marpaESLIF_regex         marpaESLIF_regex_t;
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
  MARPAESLIF_TERMINAL_TYPE_REGEX     /* Regular expression */
};

/* A string */
struct marpaESLIF_string {
  char   *stringp; /* UTF-8 string */
  size_t  stringl; /* UTF-8 length in bytes */
};

/* Regex modifiers - we take JPCRE2 matching semantics, c.f. https://neurobin.org/projects/softwares/libs/jpcre2/ */
typedef enum marpaESLIF_regex_option {
  MARPAESLIF_REGEX_OPTION_NA                  = 0x000,
  MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF = 0x001, /* e */
  MARPAESLIF_REGEX_OPTION_CASELESS            = 0x002, /* i */
  MARPAESLIF_REGEX_OPTION_JAVASCRIPT          = 0x004, /* j */
  MARPAESLIF_REGEX_OPTION_MULTILINE           = 0x008, /* m */
  MARPAESLIF_REGEX_OPTION_UCP                 = 0x010, /* n */
  MARPAESLIF_REGEX_OPTION_DOTALL              = 0x020, /* s */
  MARPAESLIF_REGEX_OPTION_EXTENDED            = 0x040, /* x */
  MARPAESLIF_REGEX_OPTION_ANCHORED            = 0x080, /* A */
  MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY      = 0x100, /* D */
  MARPAESLIF_REGEX_OPTION_DUPNAMES            = 0x200, /* J */
  MARPAESLIF_REGEX_OPTION_JIT                 = 0x400, /* S - handled programmatically */
  MARPAESLIF_REGEX_OPTION_UNGREEDY            = 0x800  /* U */
} marpaESLIF_regex_option_t;

typedef enum marpaESLIF_regex_option_id {
  MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF_ID = 0,
  MARPAESLIF_REGEX_OPTION_CASELESS_ID,
  MARPAESLIF_REGEX_OPTION_JAVASCRIPT_ID,
  MARPAESLIF_REGEX_OPTION_MULTILINE_ID,
  MARPAESLIF_REGEX_OPTION_UCP_ID,
  MARPAESLIF_REGEX_OPTION_DOTALL_ID,
  MARPAESLIF_REGEX_OPTION_EXTENDED_ID,
  MARPAESLIF_REGEX_OPTION_ANCHORED_ID,
  MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY_ID,
  MARPAESLIF_REGEX_OPTION_DUPNAMES_ID,
  MARPAESLIF_REGEX_OPTION_JIT_ID,
  MARPAESLIF_REGEX_OPTION_UNGREEDY_ID,
  _MARPAESLIF_REGEX_OPTION_UNGREEDY_ID_MAX
} marpaESLIF_regex_option_id_t;

struct marpaESLIF_regex_option_map {
  marpaESLIF_regex_option_t opti;
  char                      *modifiers;
  marpaESLIF_uint32_t        pcre2Optioni;
} marpaESLIF_regex_option_map[] = {
  { MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF, "e", PCRE2_MATCH_UNSET_BACKREF },
  { MARPAESLIF_REGEX_OPTION_CASELESS,            "i", PCRE2_CASELESS },
  { MARPAESLIF_REGEX_OPTION_JAVASCRIPT,          "j", PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF },
  { MARPAESLIF_REGEX_OPTION_MULTILINE,           "m", PCRE2_MULTILINE },
  { MARPAESLIF_REGEX_OPTION_UCP,                 "n", PCRE2_UCP },
  { MARPAESLIF_REGEX_OPTION_DOTALL,              "s", PCRE2_DOTALL },
  { MARPAESLIF_REGEX_OPTION_EXTENDED,            "x", PCRE2_EXTENDED },
  { MARPAESLIF_REGEX_OPTION_ANCHORED,            "A", PCRE2_ANCHORED },
  { MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY,      "D", PCRE2_DOLLAR_ENDONLY },
  { MARPAESLIF_REGEX_OPTION_DUPNAMES,            "J", PCRE2_DUPNAMES },
  { MARPAESLIF_REGEX_OPTION_UNGREEDY,            "U", PCRE2_UNGREEDY }
};

struct marpaESLIF_regex {
  pcre2_code       *patternp;      /* Compiled pattern */
  PCRE2_SPTR        substitutionp; /* Substitution pointer */
  PCRE2_SIZE        substitutionl; /* Substitution length */
  pcre2_match_data *match_datap;   /* Match data */
#ifdef PCRE2_CONFIG_JIT
  short       jitCompleteb;   /* Eventual optimized JIT */
  short       jitPartialb;
#endif
};

struct marpaESLIF_terminal {
  int                        idi;                 /* Terminal Id */
  char                      *descs;               /* Terminal description */
  char                      *asciidescs;          /* Terminal description (ASCII) */
  marpaESLIF_terminal_type_t type;                /* Terminal type */
  union {
    marpaESLIF_regex_t      regex;
    marpaESLIF_string_t     string;
  } u;
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
  char                *asciidescs;      /* Rule description (ASCII) */
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

/* Matcher return values */
typedef enum marpaESLIF_matcher_value {
  MARPAESLIF_MATCH_AGAIN   = -1,
  MARPAESLIF_MATCH_FAILURE =  0,
  MARPAESLIF_MATCH_OK      =  1
} marpaESLIF_matcher_value_t;

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_GRAMMAR_H */
