#ifndef MARPAESLIF_INTERNAL_STRUCTURES_H
#define MARPAESLIF_INTERNAL_STRUCTURES_H

#include <genericStack.h>
#include <pcre2.h>

typedef struct  marpaESLIF_regex           marpaESLIF_regex_t;
typedef struct  marpaESLIF_string          marpaESLIF_string_t;
typedef enum    marpaESLIF_symbol_type     marpaESLIF_symbol_type_t;
typedef enum    marpaESLIF_terminal_type   marpaESLIF_terminal_type_t;
typedef struct  marpaESLIF_terminal        marpaESLIF_terminal_t;
typedef struct  marpaESLIF_meta            marpaESLIF_meta_t;
typedef struct  marpaESLIF_symbol          marpaESLIF_symbol_t;
typedef struct  marpaESLIF_rule            marpaESLIF_rule_t;
typedef struct  marpaESLIF_grammar         marpaESLIF_grammar_t;
typedef enum    marpaESLIF_matcher_value   marpaESLIF_matcher_value_t;
typedef short (*marpaESLIF_matcher_t)(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp);
typedef enum    marpaESLIF_event_type      marpaESLIF_event_type_t;
typedef struct  marpaESLIF_readerContext   marpaESLIF_readerContext_t;
typedef struct  marpaESLIF_alternative     marpaESLIF_alternative_t;

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
  MARPAESLIF_REGEX_OPTION_NA                       = 0x0000,
  MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF      = 0x0001, /* e */
  MARPAESLIF_REGEX_OPTION_CASELESS                 = 0x0002, /* i */
  MARPAESLIF_REGEX_OPTION_JAVASCRIPT               = 0x0004, /* j */
  MARPAESLIF_REGEX_OPTION_MULTILINE                = 0x0008, /* m */
  MARPAESLIF_REGEX_OPTION_UCP                      = 0x0010, /* n */
  MARPAESLIF_REGEX_OPTION_DOTALL                   = 0x0020, /* s */
  MARPAESLIF_REGEX_OPTION_EXTENDED                 = 0x0040, /* x */
  MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY           = 0x0080, /* D */
  MARPAESLIF_REGEX_OPTION_DUPNAMES                 = 0x0100, /* J */
  MARPAESLIF_REGEX_OPTION_UNGREEDY                 = 0x0200, /* U */
  MARPAESLIF_REGEX_OPTION_SUBSTITUTE_GLOBAL        = 0x0400, /* g */
  MARPAESLIF_REGEX_OPTION_SUBSTITUTE_UNKNOWN_UNSET = 0x0800, /* z */
  MARPAESLIF_REGEX_OPTION_SUBSTITUTE_UNSET_EMPTY   = 0x1000, /* Z */
  MARPAESLIF_REGEX_OPTION_NO_UTF                   = 0x2000, /* a */
  MARPAESLIF_REGEX_OPTION_NO_UCP                   = 0x4000  /* N */
} marpaESLIF_regex_option_t;

typedef enum marpaESLIF_regex_option_id {
  MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF_ID = 0,
  MARPAESLIF_REGEX_OPTION_CASELESS_ID,
  MARPAESLIF_REGEX_OPTION_JAVASCRIPT_ID,
  MARPAESLIF_REGEX_OPTION_MULTILINE_ID,
  MARPAESLIF_REGEX_OPTION_UCP_ID,
  MARPAESLIF_REGEX_OPTION_DOTALL_ID,
  MARPAESLIF_REGEX_OPTION_EXTENDED_ID,
  MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY_ID,
  MARPAESLIF_REGEX_OPTION_DUPNAMES_ID,
  MARPAESLIF_REGEX_OPTION_UNGREEDY_ID,
  MARPAESLIF_REGEX_OPTION_SUBSTITUTE_GLOBAL_ID,
  MARPAESLIF_REGEX_OPTION_SUBSTITUTE_UNKNOWN_UNSET_ID,
  MARPAESLIF_REGEX_OPTION_SUBSTITUTE_UNSET_EMPTY_ID,
  MARPAESLIF_REGEX_OPTION_NO_UTF_ID,
  MARPAESLIF_REGEX_OPTION_NO_UCP_ID,
  _MARPAESLIF_REGEX_OPTION_ID_MAX
} marpaESLIF_regex_option_id_t;

struct marpaESLIF_regex_option_map {
  marpaESLIF_regex_option_t opti;
  char                      *modifiers;
  marpaESLIF_uint32_t        pcre2Optioni;
  marpaESLIF_uint32_t        pcre2OptionNoti;
} marpaESLIF_regex_option_map[] = {
  { MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF,      "e", PCRE2_MATCH_UNSET_BACKREF,                0 },
  { MARPAESLIF_REGEX_OPTION_CASELESS,                 "i", PCRE2_CASELESS,                           0 },
  { MARPAESLIF_REGEX_OPTION_JAVASCRIPT,               "j", PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF, 0 },
  { MARPAESLIF_REGEX_OPTION_MULTILINE,                "m", PCRE2_MULTILINE,                          0 },
  { MARPAESLIF_REGEX_OPTION_UCP,                      "n", PCRE2_UCP,                                0 },
  { MARPAESLIF_REGEX_OPTION_DOTALL,                   "s", PCRE2_DOTALL,                             0 },
  { MARPAESLIF_REGEX_OPTION_EXTENDED,                 "x", PCRE2_EXTENDED,                           0 },
  { MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY,           "D", PCRE2_DOLLAR_ENDONLY,                     0 },
  { MARPAESLIF_REGEX_OPTION_DUPNAMES,                 "J", PCRE2_DUPNAMES,                           0 },
  { MARPAESLIF_REGEX_OPTION_UNGREEDY,                 "U", PCRE2_UNGREEDY,                           0 },
  { MARPAESLIF_REGEX_OPTION_SUBSTITUTE_GLOBAL,        "g", PCRE2_SUBSTITUTE_GLOBAL,                  0 },
  { MARPAESLIF_REGEX_OPTION_SUBSTITUTE_UNKNOWN_UNSET, "z", PCRE2_SUBSTITUTE_UNKNOWN_UNSET,           0 },
  { MARPAESLIF_REGEX_OPTION_SUBSTITUTE_UNSET_EMPTY,   "Z", PCRE2_SUBSTITUTE_UNSET_EMPTY,             0 },
  { MARPAESLIF_REGEX_OPTION_NO_UTF,                   "a", 0,                                        PCRE2_UTF },
  { MARPAESLIF_REGEX_OPTION_NO_UCP,                   "N", 0,                                        PCRE2_UCP },
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
  int                         idi;                 /* Terminal Id */
  char                       *descs;               /* Terminal description as per the user */
  size_t                      descl;
  char                       *asciidescs;          /* Terminal description (ASCII) */
  marpaESLIF_terminal_type_t  type;                /* Terminal type */
  marpaESLIF_matcher_t        matcherip;           /* Terminal matcher */
  union {
    marpaESLIF_regex_t        regex;
    marpaESLIF_string_t       string;
  } u;
};

struct marpaESLIF_meta {
  int                        idi;                       /* Non-terminal Id */
  char                      *descs;                     /* Non-terminal description as per the user */
  size_t                     descl;
  char                      *asciidescs;                /* Non-terminal description (ASCII) */
  marpaESLIF_matcher_t       matcherip;                 /* Non-terminal matcher */
  marpaWrapperGrammar_t     *marpaWrapperGrammarClonep; /* Eventual cloned grammar */
};

/* Matcher return values */
enum marpaESLIF_matcher_value {
  MARPAESLIF_MATCH_AGAIN   = -1,
  MARPAESLIF_MATCH_FAILURE =  0,
  MARPAESLIF_MATCH_OK      =  1
};

enum marpaESLIF_event_type {
  MARPAESLIF_EVENT_TYPE_NA        = 0x00,
  MARPAESLIF_EVENT_TYPE_COMPLETED = 0x01, /* Grammar event */
  MARPAESLIF_EVENT_TYPE_NULLED    = 0x02, /* Grammar event */
  MARPAESLIF_EVENT_TYPE_EXPECTED  = 0x04, /* Grammar event */
  MARPAESLIF_EVENT_TYPE_BEFORE    = 0x08, /* ESLIF lexeme event */
  MARPAESLIF_EVENT_TYPE_AFTER     = 0x10  /* ESLIF lexeme event */
};

/* A symbol */
struct marpaESLIF_symbol {
  marpaESLIF_symbol_type_t    type;  /* Symbol type */
  union {
    marpaESLIF_terminal_t    *terminalp; /* Symbol is a terminal */
    marpaESLIF_meta_t        *metap;     /* Symbol is a meta identifier, i.e. a rule */
  } u;
  short                       isLhsb;
  int                         idi;
  char                       *descs;           /* Terminal description as per the user */
  size_t                      descl;
  char                       *asciidescs;      /* Shallow pointer to the asciidecs from the union members */
  marpaESLIF_matcher_t        matcherip;
  short                       pauseb;          /* -1: before, 0: NA, 1: after */
  short                       pauseIsOnb;      /* 0: off, 1: on */
  char                       *pauses;          /* Pause event name in native encoding */
  size_t                     *pausel;          /* Pause event name length in bytes */
  char                       *asciipauses;     /* Pause event name in native encoding */
};

/* A rule */
struct marpaESLIF_rule {
  int                  idi;             /* Rule Id */
  char                *descs;           /* Rule description */
  size_t               descl;
  char                *asciidescs;      /* Rule description (ASCII) */
  marpaESLIF_symbol_t *lhsp;            /* LHS symbol */
  genericStack_t      *rhsStackp;       /* Stack of RHS symbols */
  genericStack_t      *maskStackp;      /* Stack of RHS mask */
  genericStack_t      *exceptionStackp; /* Stack of Exceptions symbols */
};

/* A grammar */
struct marpaESLIF_grammar {
  marpaESLIF_t          *marpaESLIFp;          /* Shallow pointer to top marpaESLIFp */
  unsigned int           leveli;               /* Grammar level */
  marpaWrapperGrammar_t *marpaWrapperGrammarp; /* Grammar implementation */
  genericStack_t        *symbolStackp;         /* Stack of symbols */
  genericStack_t        *ruleStackp;           /* Stack of rules */
  marpaESLIF_grammar_t  *previousp;            /* Previous eventual grammar */
  marpaESLIF_grammar_t  *nextp;                /* Next eventual sub-grammar */
  char                  *exhausteds;           /* Exhaustion event name in native encoding */
  size_t                *exhaustedl;           /* Pause event name length in bytes */
  char                  *asciiexhausteds;      /* Exhaustion event name in native encoding */
};

/* Internal reader context when parsing a grammar */
struct marpaESLIF_readerContext {
  marpaESLIF_t *marpaESLIFp;
  char         *utf8s;
  size_t        utf8l;
  char         *p;
};

/* Internal structure to remember the alternatives */
struct marpaESLIF_alternative {
  marpaESLIF_symbol_t     *symbolp;
  marpaESLIFAlternative_t *alternativep;
};

/* ----------------------------------- */
/* Definition of the opaque structures */
/* ----------------------------------- */
struct marpaESLIF {
  genericStack_t     *grammarStackp;
  marpaESLIFOption_t  option;
};

struct marpaESLIFGrammar {
  marpaESLIF_t             *marpaESLIFp;
  marpaESLIFGrammarOption_t marpaESLIFGrammarOption;
  marpaESLIF_grammar_t     *grammarp;
};

struct marpaESLIFRecognizer {
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaWrapperRecognizer_t    *marpaWrapperRecognizerp;
  genericStack_t              *valueStackp;
  char                        *inputcp;
  size_t                       inputl;
  short                        eofb;
  short                        scanb;
  marpaESLIFString_t          *stringArrayp;    /* For the events */
  size_t                       stringArrayl;
  genericStack_t              *alternativeStackp; /* Current alternatives */
};

/* ------------------------------------------- */
/* Definition of the default option structures */
/* ------------------------------------------- */
marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /* genericLoggerp */
};

marpaESLIFGrammarOption_t marpaESLIFGrammarOption_default = {
  NULL, /* grammarcp */
  0,    /* grammarl */
  NULL  /* encodings */
};

marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption_default = {
  NULL,              /* userDatavp */
  NULL,              /* marpaESLIFReaderCallbackp */
  0                  /* disableThresholdb */
};

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_STRUCTURES_H */
