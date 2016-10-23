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
typedef short (*marpaESLIFGrammar_matcher_t)(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *stackp);
typedef enum    marpaESLIF_event_type      marpaESLIF_event_type_t;
typedef enum    marpaESLIF_action_type     marpaESLIF_action_type_t;
typedef enum    marpaESLIF_array_type      marpaESLIF_array_type_t;
typedef struct  marpaESLIF_readerContext   marpaESLIF_readerContext_t;
typedef struct  marpaESLIF_valueContext    marpaESLIF_valueContext_t;
typedef struct  marpaESLIF_cloneContext    marpaESLIF_cloneContext_t;

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
  MARPAESLIF_REGEX_OPTION_NA                       = 0x000,
  MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF      = 0x001, /* e */
  MARPAESLIF_REGEX_OPTION_CASELESS                 = 0x002, /* i */
  MARPAESLIF_REGEX_OPTION_JAVASCRIPT               = 0x004, /* j */
  MARPAESLIF_REGEX_OPTION_MULTILINE                = 0x008, /* m */
  MARPAESLIF_REGEX_OPTION_UCP                      = 0x010, /* n */
  MARPAESLIF_REGEX_OPTION_DOTALL                   = 0x020, /* s */
  MARPAESLIF_REGEX_OPTION_EXTENDED                 = 0x040, /* x */
  MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY           = 0x080, /* D */
  MARPAESLIF_REGEX_OPTION_DUPNAMES                 = 0x100, /* J */
  MARPAESLIF_REGEX_OPTION_UNGREEDY                 = 0x200, /* U */
  MARPAESLIF_REGEX_OPTION_NO_UTF                   = 0x400, /* a */
  MARPAESLIF_REGEX_OPTION_NO_UCP                   = 0x800  /* N */
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
  { MARPAESLIF_REGEX_OPTION_NO_UTF,                   "a", 0,                                        PCRE2_UTF },
  { MARPAESLIF_REGEX_OPTION_NO_UCP,                   "N", 0,                                        PCRE2_UCP },
};

struct marpaESLIF_regex {
  pcre2_code       *patternp;      /* Compiled pattern */
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
  marpaESLIFGrammar_matcher_t matcherbp;           /* Terminal matcher */
  union {
    marpaESLIF_regex_t        regex;
    marpaESLIF_string_t       string;
  } u;
};

struct marpaESLIF_meta {
  int                          idi;                       /* Non-terminal Id */
  char                        *descs;                     /* Non-terminal description as per the user */
  size_t                       descl;
  char                        *asciidescs;                /* Non-terminal description (ASCII) */
  marpaESLIFGrammar_matcher_t  matcherbp;                 /* Non-terminal matcher */
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
  MARPAESLIF_ACTION_TYPE_NA        = 0x00,
  MARPAESLIF_ACTION_TYPE_ARRAY     = 0x01, /* ::array */
  MARPAESLIF_ACTION_TYPE_FIRST     = 0x02, /* ::first */
  MARPAESLIF_ACTION_TYPE_UNDEF     = 0x04, /* ::undef */
  MARPAESLIF_ACTION_TYPE_COMPOSITE = 0x08  /* [] */
};

/* Array action types */
enum marpaESLIF_array_type {
  MARPAESLIF_ARRAY_TYPE_NA       = 0x00,
  MARPAESLIF_ARRAY_TYPE_NAME     = 0x01, /* name */
  MARPAESLIF_ARRAY_TYPE_RULE     = 0x02, /* rule */
  MARPAESLIF_ARRAY_TYPE_SYMBOL   = 0x04, /* symbol */
  MARPAESLIF_ARRAY_TYPE_VALUE    = 0x08, /* value */
  MARPAESLIF_ARRAY_TYPE_VALUES   = 0x08, /* values: a synonym for value */
};

/* A symbol */
struct marpaESLIF_symbol {
  marpaESLIF_symbol_type_t     type;  /* Symbol type */
  union {
    marpaESLIF_terminal_t     *terminalp; /* Symbol is a terminal */
    marpaESLIF_meta_t         *metap;     /* Symbol is a meta identifier, i.e. a rule */
  } u;
  short                        startb;             /* Start symbol ? */
  short                        discardb;           /* Discard symbol ? */
  short                        lhsb;               /* Is an LHS somewhere in its grammar ? */
  int                          idi;                /* Marpa ID */
  char                        *descs;              /* Terminal description in user's encoding */
  size_t                       descl;              /* Length in bytesd of this terminal description */
  char                        *asciidescs;         /* Shallow pointer to the asciidecs from the union members */
  marpaESLIFGrammar_matcher_t  matcherbp;          /* Matcher function pointer */
  short                        pauseb;             /* -1: before, 0: NA, 1: after */
  short                        pauseIsOnb;         /* 0: off, 1: on */
  char                        *pauses;             /* Pause event name in native encoding */
  size_t                       pausel;             /* Pause event name length in bytes */
  char                        *asciipauses;        /* Pause event name in ASCII encoding */
  char                        *events;             /* Grammar event name in native encoding */
  size_t                       eventl;             /* Grammar event name length in bytes */
  char                        *asciievents;        /* Grammar event name in ASCII encoding */
  int                          lookupLevelDeltai;  /* Referenced grammar delta level */
  char                        *lookupGrammarNamep; /* Referenced grammar (string in user's encoding) */
  size_t                       lookupGrammarNamel; /* Referenced grammar (number of bytes of string in user's encoding) */
  int                          resolvedLeveli;     /* Referenced grammar level */
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
  marpaESLIF_t          *marpaESLIFp;                 /* Shallow pointer to top marpaESLIFp */
  int                    leveli;                      /* Grammar level */
  char                  *descs;                       /* Grammar description */
  size_t                 descl;
  char                  *asciidescs;                  /* Terminal description (ASCII) */
  marpaWrapperGrammar_t *marpaWrapperGrammarStartp;   /* Grammar implementation at :start */
  marpaWrapperGrammar_t *marpaWrapperGrammarDiscardp; /* Grammar implementation at :discard */
  marpaESLIF_symbol_t   *discardSymbolp;              /* Discard symbol, used at grammar validation */
  genericStack_t        *symbolStackp;                /* Stack of symbols */
  genericStack_t        *ruleStackp;                  /* Stack of rules */
};

/* Internal reader context when parsing a grammar. Everything is in utf8s so the reader can say ok to any stream callback */
struct marpaESLIF_readerContext {
  marpaESLIF_t *marpaESLIFp;
  char         *utf8s;
  size_t        utf8l;
  char         *p;      /* Current position */
  char         *markp;  /* Marked position - no notion of limit here */
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
  marpaESLIFGrammar_t *marpaESLIFGrammarp;  /* ESLIF has its own grammar -; */
  marpaESLIFOption_t   marpaESLIFOption;
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
  char                        *inputs;
  size_t                       inputl;
  short                        eofb;
  short                        scanb;
  short                        discardb; /* discard mode */
  short                        haveLexemeb;
};

/* ------------------------------------------- */
/* Definition of the default option structures */
/* ------------------------------------------- */
marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /* genericLoggerp */
};

marpaESLIFGrammarOption_t marpaESLIFGrammarOption_default = {
  NULL, /* grammars */
  0,    /* grammarl */
  NULL  /* encodings */
};

marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption_default = {
  NULL,              /* userDatavp */
  NULL,              /* marpaESLIFReaderCallbackp */
  NULL,              /* marpaESLIFSkipperCallbackp */
  NULL,              /* marpaESLIFMarkerCallbackp */
  NULL,              /* marpaESLIFResetterCallbackp */
  NULL,              /* marpaESLIFCloserCallbackp */
  0,                 /* disableThresholdb */
  0,                 /* exhaustedb */
  0,                 /* latmb */
  0                  /* rejectionb */
};

marpaESLIFValueOption_t marpaESLIFValueOption_default_template = {
  NULL, /* userDatavp - filled at run-time */
  NULL, /* valueCallbackp - filled at run-time */
  1, /* highRankOnlyb */
  1, /* orderByRankb */
  0, /* ambiguousb */
  0, /* nullb */
  0, /* maxParsesi */
  NULL /* outputStackp */
};

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_STRUCTURES_H */
