#ifndef MARPAESLIF_INTERNAL_STRUCTURES_H
#define MARPAESLIF_INTERNAL_STRUCTURES_H

#include <genericStack.h>
#include <pcre2.h>

#define INTERNAL_ANYCHAR_PATTERN "." /* This ASCII string is UTF-8 compatible */

typedef struct  marpaESLIF_regex           marpaESLIF_regex_t;
typedef         marpaESLIFString_t         marpaESLIF_string_t;
typedef enum    marpaESLIF_symbol_type     marpaESLIF_symbol_type_t;
typedef enum    marpaESLIF_terminal_type   marpaESLIF_terminal_type_t;
typedef struct  marpaESLIF_terminal        marpaESLIF_terminal_t;
typedef struct  marpaESLIF_meta            marpaESLIF_meta_t;
typedef struct  marpaESLIF_symbol          marpaESLIF_symbol_t;
typedef struct  marpaESLIF_rule            marpaESLIF_rule_t;
typedef struct  marpaESLIF_grammar         marpaESLIF_grammar_t;
typedef enum    marpaESLIF_matcher_value   marpaESLIF_matcher_value_t;
typedef enum    marpaESLIF_event_type      marpaESLIF_event_type_t;
typedef enum    marpaESLIF_action_type     marpaESLIF_action_type_t;
typedef enum    marpaESLIF_array_type      marpaESLIF_array_type_t;
typedef struct  marpaESLIF_readerContext   marpaESLIF_readerContext_t;
typedef struct  marpaESLIF_valueContext    marpaESLIF_valueContext_t;
typedef struct  marpaESLIF_cloneContext    marpaESLIF_cloneContext_t;
typedef struct  marpaESLIF_action          marpaESLIF_action_t;

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
  MARPAESLIF_REGEX_OPTION_NO_UTF                   = 0x0400, /* a */
  MARPAESLIF_REGEX_OPTION_NO_UCP                   = 0x0800, /* N */
  MARPAESLIF_REGEX_OPTION_UTF                      = 0x1000  /* u */
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
  MARPAESLIF_REGEX_OPTION_NO_UTF_ID,
  MARPAESLIF_REGEX_OPTION_NO_UCP_ID,
  MARPAESLIF_REGEX_OPTION_UTF_ID,
  _MARPAESLIF_REGEX_OPTION_ID_MAX
} marpaESLIF_regex_option_id_t;

struct marpaESLIF_regex_option_map {
  marpaESLIF_regex_option_t opti;
  char                      *modifiers;
  char                      *pcre2modifiers;
  marpaESLIF_uint32_t        pcre2Optioni;
  marpaESLIF_uint32_t        pcre2OptionNoti;
} marpaESLIF_regex_option_map[] = {
  { MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF,      "e", "PCRE2_MATCH_UNSET_BACKREF",                PCRE2_MATCH_UNSET_BACKREF,                0 },
  { MARPAESLIF_REGEX_OPTION_CASELESS,                 "i", "PCRE2_CASELESS",                           PCRE2_CASELESS,                           0 },
  { MARPAESLIF_REGEX_OPTION_JAVASCRIPT,               "j", "PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF", PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF, 0 },
  { MARPAESLIF_REGEX_OPTION_MULTILINE,                "m", "PCRE2_MULTILINE",                          PCRE2_MULTILINE,                          0 },
  { MARPAESLIF_REGEX_OPTION_UCP,                      "n", "PCRE2_UCP",                                PCRE2_UCP,                                0 },
  { MARPAESLIF_REGEX_OPTION_DOTALL,                   "s", "PCRE2_DOTALL",                             PCRE2_DOTALL,                             0 },
  { MARPAESLIF_REGEX_OPTION_EXTENDED,                 "x", "PCRE2_EXTENDED",                           PCRE2_EXTENDED,                           0 },
  { MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY,           "D", "PCRE2_DOLLAR_ENDONLY",                     PCRE2_DOLLAR_ENDONLY,                     0 },
  { MARPAESLIF_REGEX_OPTION_DUPNAMES,                 "J", "PCRE2_DUPNAMES",                           PCRE2_DUPNAMES,                           0 },
  { MARPAESLIF_REGEX_OPTION_UNGREEDY,                 "U", "PCRE2_UNGREEDY",                           PCRE2_UNGREEDY,                           0 },
  { MARPAESLIF_REGEX_OPTION_NO_UTF,                   "a", "PCRE2_UTF",                                0,                                        PCRE2_UTF },
  { MARPAESLIF_REGEX_OPTION_NO_UCP,                   "N", "PCRE2_UCP",                                0,                                        PCRE2_UCP },
  { MARPAESLIF_REGEX_OPTION_UTF,                      "u", "PCRE2_UTF",                                PCRE2_UTF, 0 }
};

struct marpaESLIF_regex {
  pcre2_code          *patternp;      /* Compiled pattern */
  pcre2_match_data    *match_datap;   /* Match data */
#ifdef PCRE2_CONFIG_JIT
  short       jitCompleteb;   /* Eventual optimized JIT */
  short       jitPartialb;
#endif
  short       utfb; /* Is UTF mode enabled in that pattern ? */
};

struct marpaESLIF_terminal {
  int                         idi;                 /* Terminal Id */
  marpaESLIF_string_t        *descp;               /* Terminal description */
  marpaESLIF_regex_t          regex;
};

struct marpaESLIF_meta {
  int                          idi;                       /* Non-terminal Id */
  marpaESLIF_string_t         *descp;                     /* Non-terminal description */
  marpaWrapperGrammar_t       *marpaWrapperGrammarClonep; /* Eventual cloned grammar */
};

/* Matcher return values */
enum marpaESLIF_matcher_value {
  MARPAESLIF_MATCH_AGAIN   = -1,
  MARPAESLIF_MATCH_FAILURE =  0,
  MARPAESLIF_MATCH_OK      =  1
};

/* Event types */
enum marpaESLIF_event_type {
  MARPAESLIF_EVENT_TYPE_NA        = 0x00,
  MARPAESLIF_EVENT_TYPE_COMPLETED = 0x01, /* Grammar event */
  MARPAESLIF_EVENT_TYPE_NULLED    = 0x02, /* Grammar event */
  MARPAESLIF_EVENT_TYPE_EXPECTED  = 0x04, /* Grammar event */
  MARPAESLIF_EVENT_TYPE_BEFORE    = 0x08, /* ESLIF lexeme event */
  MARPAESLIF_EVENT_TYPE_AFTER     = 0x10  /* ESLIF lexeme event */
};

/* Action types */
enum marpaESLIF_action_type {
  MARPAESLIF_ACTION_TYPE_NA = 0,
  MARPAESLIF_ACTION_TYPE_ARRAY,      /* ::array */
  MARPAESLIF_ACTION_TYPE_FIRST,      /* ::first */
  MARPAESLIF_ACTION_TYPE_UNDEF,      /* ::undef */
  MARPAESLIF_ACTION_TYPE_COMPOSITE,  /* [] */
  MARPAESLIF_ACTION_TYPE_NAME        /* c.f. action callback */
};

/* Array action types */
enum marpaESLIF_array_type {
  MARPAESLIF_ARRAY_TYPE_NA = 0,
  MARPAESLIF_ARRAY_TYPE_START,    /* start */
  MARPAESLIF_ARRAY_TYPE_LENGTH,   /* length */
  MARPAESLIF_ARRAY_TYPE_G1START,  /* g1start */
  MARPAESLIF_ARRAY_TYPE_G1LENGTH, /* g1length */
  MARPAESLIF_ARRAY_TYPE_NAME,     /* name */
  MARPAESLIF_ARRAY_TYPE_LHS,      /* rule */
  MARPAESLIF_ARRAY_TYPE_RULE,     /* rule */
  MARPAESLIF_ARRAY_TYPE_SYMBOL,   /* symbol */
  MARPAESLIF_ARRAY_TYPE_VALUE,    /* value */
  MARPAESLIF_ARRAY_TYPE_VALUES,   /* values: a synonym for value */
};

/* Action description */
struct marpaESLIF_action {
  marpaESLIF_action_type_t type;
  union {
    genericStack_t      *compositeStackp; /* Only when type is MARPAESLIF_ACTION_TYPE_COMPOSITE */
    marpaESLIF_string_t *namep;           /* Only when type is MARPAESLIF_ACTION_TYPE_NAME */
  } u;
};

/* A symbol */
struct marpaESLIF_symbol {
  marpaESLIF_symbol_type_t     type;  /* Symbol type */
  union {
    marpaESLIF_terminal_t     *terminalp; /* Symbol is a terminal */
    marpaESLIF_meta_t         *metap;     /* Symbol is a meta identifier, i.e. a rule */
  } u;
  short                        startb;              /* Start symbol ? */
  short                        discardb;            /* Discard symbol ? */
  short                        lhsb;                /* Is an LHS somewhere in its grammar ? */
  int                          idi;                 /* Marpa ID */
  marpaESLIF_string_t         *descp;               /* Symbol description */
  short                        pauseb;              /* -1: before, 0: NA, 1: after */
  short                        pauseIsOnb;          /* 0: off, 1: on */
  marpaESLIF_string_t         *pausep;              /* Pause event name in native encoding */
  char                        *asciipauses;         /* Pause event name in ASCII encoding */
  marpaESLIF_string_t         *eventp;              /* Grammar event name in native encoding */
  char                        *asciievents;         /* Grammar event name in ASCII encoding */
  int                          lookupLevelDeltai;   /* Referenced grammar delta level */
  marpaESLIF_string_t         *lookupGrammarStringp; /* Referenced grammar (string in user's encoding) */
  int                          resolvedLeveli;      /* Referenced grammar level */
  int                          priorityi;           /* Symbol priority */
  marpaESLIF_action_t         *actionp;             /* Action */
};

/* A rule */
struct marpaESLIF_rule {
  int                  idi;             /* Rule Id */
  marpaESLIF_string_t *descp;           /* Rule description */
  char                *asciishows;      /* Rule show (ASCII) */
  marpaESLIF_symbol_t *lhsp;            /* LHS symbol */
  marpaESLIF_symbol_t *separatorp;      /* Eventual separator symbol */
  genericStack_t      *rhsStackp;       /* Stack of RHS symbols */
  genericStack_t      *maskStackp;      /* Stack of RHS mask */
  genericStack_t      *exceptionStackp; /* Stack of Exceptions symbols */
  marpaESLIF_action_t *actionp;         /* Associated action */
  int                  ranki;
  short                nullRanksHighb;
  short                sequenceb;
  short                properb;
  int                  minimumi;
};

/* A grammar */
struct marpaESLIF_grammar {
  marpaESLIF_t          *marpaESLIFp;                 /* Shallow pointer to top marpaESLIFp */
  int                    leveli;                      /* Grammar level */
  marpaESLIF_string_t   *descp;                       /* Grammar description */
  marpaWrapperGrammar_t *marpaWrapperGrammarStartp;   /* Grammar implementation at :start */
  marpaWrapperGrammar_t *marpaWrapperGrammarDiscardp; /* Grammar implementation at :discard */
  marpaESLIF_symbol_t   *discardSymbolp;              /* Discard symbol, used at grammar validation */
  genericStack_t        *symbolStackp;                /* Stack of symbols */
  genericStack_t        *ruleStackp;                  /* Stack of rules */
  marpaESLIF_action_t   *actionp;                     /* Default action */
  int                    starti;                      /* Default start symbol ID - filled during grammar validation */
  int                   *ruleip;                      /* Array of rule IDs - filled by grammar validation */
  size_t                 rulel;                       /* Size of the rule IDs array - filled by grammar validation */
};

/* Internal reader context when parsing a grammar. Everything is in utf8s so the reader can say ok to any stream callback */
struct marpaESLIF_readerContext {
  marpaESLIF_t              *marpaESLIFp;
  marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp;
};

/* Internal structure to have value context information */
struct marpaESLIF_valueContext {
  int ruleIdi;
};

/* Internal structure to have clone context information */
struct marpaESLIF_cloneContext {
  marpaESLIF_t         *marpaESLIFp;
  marpaESLIF_grammar_t *grammarp;
};

/* ----------------------------------- */
/* Definition of the opaque structures */
/* ----------------------------------- */
struct marpaESLIF {
  marpaESLIFGrammar_t   *marpaESLIFGrammarp;  /* ESLIF has its own grammar -; */
  marpaESLIFOption_t     marpaESLIFOption;
  marpaESLIF_terminal_t *anycharp;     /* internal regex */
};

struct marpaESLIFGrammar {
  marpaESLIF_t             *marpaESLIFp;
  genericStack_t           *grammarStackp;
  marpaESLIF_grammar_t     *grammarp;         /* This is a SHALLOW copy of current grammar in grammarStackp, defaulting to the top grammar */
};

struct marpaESLIFValue {
  marpaESLIF_t             *marpaESLIFp;
  marpaESLIFRecognizer_t   *marpaESLIFRecognizerp;
  marpaESLIFValueOption_t   marpaESLIFValueOption;
  marpaWrapperValue_t      *marpaWrapperValuep;
  marpaESLIF_valueContext_t context;
};

struct marpaESLIFRecognizer {
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaWrapperRecognizer_t    *marpaWrapperRecognizerp;
  genericStack_t              *lexemeInputStackp;  /* Internal input stack of lexemes */
  marpaESLIFEvent_t           *eventArrayp;      /* For the events */
  size_t                       eventArrayl;
  marpaESLIFRecognizer_t      *parentRecognizerp;
  unsigned long                resumeCounterl; /* Internal counter for tracing - no functional impact */

  char                        *buffers;        /* Pointer to allocated buffer containing input */
  size_t                       bufferl;        /* Number of valid bytes in this buffer (!= allocated size in the exceptional case of a realloc failure) */
  short                        eofb;           /* EOF flag */
  short                        convertedb;     /* A flag to say if input was converted. If any regex requires characters and this flag is off, this will be an error */
  char                       **buffersp;       /* Pointer to allocated buffer - for sharing with eventual parent recognizers */
  size_t                      *bufferlp;       /* Ditto for the size */
  short                       *eofbp;          /* Ditto for the EOF flag */
  short                       *convertedbp;    /* Ditto for the conversion flag */

  char                        *remembers;      /* Same logic as with the buffer */
  size_t                       rememberl;
  char                       **remembersp;
  size_t                      *rememberlp;

  size_t                       parentDeltal;   /* Parent original delta - used to recovert parent current pointer at our free */
  char                        *inputs;         /* Current pointer in input - specific to every recognizer */
  size_t                       inputl;         /* Current remaining bytes - specific to every recognizer */
  short                        scanb;          /* Prevent resume before a call to scan */
  short                        discardb;       /* discard mode */
  short                        haveLexemeb;    /* Remember if this recognizer have at least one lexeme */
};

/* ------------------------------------------- */
/* Definition of the default option structures */
/* ------------------------------------------- */
marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /*  __ */
};

marpaESLIFGrammarOption_t marpaESLIFGrammarOption_default = {
  NULL, /* grammars */
  0,    /* grammarl */
  NULL  /* encodings */
};

marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption_default = {
  NULL,              /* userDatavp */
  NULL,              /* marpaESLIFReaderCallbackp */
  0,                 /* disableThresholdb */
  0,                 /* exhaustedb */
  0,                 /* latmb */
  0,                 /* rejectionb */
  0                  /* rememberInputb */
};

marpaESLIFValueOption_t marpaESLIFValueOption_default_template = {
  NULL, /* userDatavp - filled at run-time */
  NULL, /* valueCallbackp - filled at run-time */
  1,    /* highRankOnlyb */
  1,    /* orderByRankb */
  0,    /* ambiguousb */
  0,    /* nullb */
  0,    /* maxParsesi */
  NULL  /* outputStackp */
};

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_STRUCTURES_H */
