#ifndef MARPAESLIF_INTERNAL_STRUCTURES_H
#define MARPAESLIF_INTERNAL_STRUCTURES_H

#include <genericStack.h>
#include <pcre2.h>
#include <tconv.h>

#define INTERNAL_ANYCHAR_PATTERN "."                    /* This ASCII string is UTF-8 compatible */
#define INTERNAL_UTF8BOM_PATTERN "\\x{FEFF}"            /* FEFF Unicode code point i.e. EFBBBF in UTF-8 encoding */
#define INTERNAL_NEWLINE_PATTERN "(*BSR_UNICODE).*?\\R" /* newline as per unicode - we do .*? because our regexps are always anchored */

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
typedef enum    marpaESLIF_array_type      marpaESLIF_array_type_t;
typedef struct  marpaESLIF_readerContext   marpaESLIF_readerContext_t;
typedef struct  marpaESLIF_lexemeContext   marpaESLIF_lexemeContext_t;
typedef struct  marpaESLIF_cloneContext    marpaESLIF_cloneContext_t;
typedef enum    marpaESLIF_valueMode       marpaESLIF_valueMode_t;

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
  MARPAESLIF_REGEX_OPTION_NA                       = 0x00000,
  MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF      = 0x00001, /* e */
  MARPAESLIF_REGEX_OPTION_CASELESS                 = 0x00002, /* i */
  MARPAESLIF_REGEX_OPTION_JAVASCRIPT               = 0x00004, /* j */
  MARPAESLIF_REGEX_OPTION_MULTILINE                = 0x00008, /* m */
  MARPAESLIF_REGEX_OPTION_UCP                      = 0x00010, /* n */
  MARPAESLIF_REGEX_OPTION_DOTALL                   = 0x00020, /* s */
  MARPAESLIF_REGEX_OPTION_EXTENDED                 = 0x00040, /* x */
  MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY           = 0x00080, /* D */
  MARPAESLIF_REGEX_OPTION_DUPNAMES                 = 0x00100, /* J */
  MARPAESLIF_REGEX_OPTION_UNGREEDY                 = 0x00200, /* U */
  MARPAESLIF_REGEX_OPTION_NO_UTF                   = 0x00400, /* a */
  MARPAESLIF_REGEX_OPTION_NO_UCP                   = 0x00800, /* N */
  MARPAESLIF_REGEX_OPTION_UTF                      = 0x01000, /* u */
  MARPAESLIF_REGEX_OPTION_BINARY                   = 0x02000, /* b */
  MARPAESLIF_REGEX_OPTION_CHARACTER                = 0x04000, /* c */
  MARPAESLIF_REGEX_OPTION_NO_ANCHOR                = 0x10000  /* A */
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
  MARPAESLIF_REGEX_OPTION_BINARY_ID,
  MARPAESLIF_REGEX_OPTION_CHARACTER_ID,
  MARPAESLIF_REGEX_OPTION_NO_ANCHOR_ID,
  _MARPAESLIF_REGEX_OPTION_ID_MAX
} marpaESLIF_regex_option_id_t;

struct marpaESLIF_regex_option_map {
  marpaESLIF_regex_option_t opti;
  char                      *modifiers;
  char                      *pcre2Options;
  marpaESLIF_uint32_t        pcre2Optioni;
  char                      *pcre2OptionNots;
  marpaESLIF_uint32_t        pcre2OptionNoti;
} marpaESLIF_regex_option_map[] = {
  { MARPAESLIF_REGEX_OPTION_MATCH_UNSET_BACKREF,      "e", "PCRE2_MATCH_UNSET_BACKREF",                PCRE2_MATCH_UNSET_BACKREF,                NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_CASELESS,                 "i", "PCRE2_CASELESS",                           PCRE2_CASELESS,                           NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_JAVASCRIPT,               "j", "PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF", PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF, NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_MULTILINE,                "m", "PCRE2_MULTILINE",                          PCRE2_MULTILINE,                          NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_UCP,                      "n", "PCRE2_UCP",                                PCRE2_UCP,                                NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_DOTALL,                   "s", "PCRE2_DOTALL",                             PCRE2_DOTALL,                             NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_EXTENDED,                 "x", "PCRE2_EXTENDED",                           PCRE2_EXTENDED,                           NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_DOLLAR_ENDONLY,           "D", "PCRE2_DOLLAR_ENDONLY",                     PCRE2_DOLLAR_ENDONLY,                     NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_DUPNAMES,                 "J", "PCRE2_DUPNAMES",                           PCRE2_DUPNAMES,                           NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_UNGREEDY,                 "U", "PCRE2_UNGREEDY",                           PCRE2_UNGREEDY,                           NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_NO_UTF,                   "a", NULL,                                       0,                                        "PCRE2_UTF",       PCRE2_UTF },
  { MARPAESLIF_REGEX_OPTION_NO_UCP,                   "N", NULL,                                       0,                                        "PCRE2_UCP",       PCRE2_UCP },
  { MARPAESLIF_REGEX_OPTION_UTF,                      "u", "PCRE2_UTF",                                PCRE2_UTF,                                NULL,              0 },
  { MARPAESLIF_REGEX_OPTION_BINARY,                   "b", "PCRE2_NEVER_UTF",                          PCRE2_NEVER_UTF,                          "PCRE2_UTF",       PCRE2_UTF },
  { MARPAESLIF_REGEX_OPTION_CHARACTER,                "c", "PCRE2_UTF",                                PCRE2_UTF,                                "PCRE2_NEVER_UTF", PCRE2_NEVER_UTF },
  { MARPAESLIF_REGEX_OPTION_NO_ANCHOR,                "A", NULL,                                       0,                                        "PCRE2_ANCHORED",  PCRE2_ANCHORED }
};

struct marpaESLIF_regex {
  pcre2_code          *patternp;      /* Compiled pattern */
  pcre2_match_data    *match_datap;   /* Match data */
#ifdef PCRE2_CONFIG_JIT
  short       jitCompleteb;   /* Eventual optimized JIT */
  short       jitPartialb;
#endif
  short       isAnchoredb; /* Remember if pattern was allocated with PCRE2_ANCHORED (set automatically or not) */
  short       utfb; /* Is UTF mode enabled in that pattern ? */
};

struct marpaESLIF_terminal {
  int                         idi;                 /* Terminal Id */
  marpaESLIF_string_t        *descp;               /* Terminal description */
  marpaESLIF_regex_t          regex;
};

struct marpaESLIF_meta {
  int                          idi;                       /* Non-terminal Id */
  char                        *asciinames;
  marpaESLIF_string_t         *descp;                     /* Non-terminal description */
  marpaWrapperGrammar_t       *marpaWrapperGrammarClonep; /* Eventual cloned grammar */
  short                       *prioritizedb;              /* Internal flag to prevent a prioritized symbol to appear more than once as an LHS */
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
  marpaESLIF_string_t         *actionp;             /* Action */
};

/* A rule */
struct marpaESLIF_rule {
  int                  idi;             /* Rule Id */
  char                *asciinames;      /* Rule name (ASCII as per the grammar) */
  marpaESLIF_string_t *descp;           /* Rule alternative name */
  char                *asciishows;      /* Rule show (ASCII) */
  marpaESLIF_symbol_t *lhsp;            /* LHS symbol */
  marpaESLIF_symbol_t *separatorp;      /* Eventual separator symbol */
  genericStack_t      *rhsStackp;       /* Stack of RHS symbols */
  genericStack_t      *exceptionStackp; /* Stack of Exceptions symbols */
  marpaESLIF_string_t *actionp;         /* Action */
  int                  ranki;
  short                nullRanksHighb;
  short                sequenceb;
  short                properb;
  int                  minimumi;
  short                passthroughb; /* Internal rule with a single RHS that is doing nothing but is used for grouping and associativity */
};

/* A grammar */
struct marpaESLIF_grammar {
  marpaESLIF_t          *marpaESLIFp;                 /* Shallow pointer to top marpaESLIFp */
  int                    leveli;                      /* Grammar level */
  marpaESLIF_string_t   *descp;                       /* Grammar description */
  short                  latmb;                       /* Longest acceptable token match mode */
  marpaWrapperGrammar_t *marpaWrapperGrammarStartp;   /* Grammar implementation at :start */
  marpaWrapperGrammar_t *marpaWrapperGrammarDiscardp; /* Grammar implementation at :discard */
  marpaESLIF_symbol_t   *discardSymbolp;              /* Discard symbol, used at grammar validation */
  genericStack_t        *symbolStackp;                /* Stack of symbols */
  genericStack_t        *ruleStackp;                  /* Stack of rules */
  marpaESLIF_string_t   *defaultSymbolActionp;        /* Default action for symbols */
  marpaESLIF_string_t   *defaultRuleActionp;          /* Default action for rules */
  int                    starti;                      /* Default start symbol ID - filled during grammar validation */
  int                   *ruleip;                      /* Array of rule IDs - filled by grammar validation */
  size_t                 rulel;                       /* Size of the rule IDs array - filled by grammar validation */
  short                  internalb;                   /* When true, this is the internal ESLIF grammar */
  short                  haveRejectionb;              /* When true, valuation go to the ASF mode - filled by grammar validation */
};

/* ----------------------------------- */
/* Definition of the opaque structures */
/* ----------------------------------- */
struct marpaESLIF {
  marpaESLIFGrammar_t   *marpaESLIFGrammarp;          /* ESLIF has its own grammar -; */
  marpaESLIFOption_t     marpaESLIFOption;
  marpaESLIF_terminal_t *anycharp;                    /* internal regex for match any character */
  marpaESLIF_terminal_t *utf8bomp;                    /* Internal regex for match UTF-8 BOM */
  marpaESLIF_terminal_t *newlinep;                    /* Internal regex for match newline */
  marpaESLIF_string_t   *defaultLexemeActionp;        /* Default action for symbols and rules when collectin lexemes */
  marpaESLIF_string_t   *defaultRuleLexemeActionp;    /* Default action for rules when collectin lexemes */
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
  short                     afsmode;         /* Say if we are using the traditional Marpa mode or the ASF valuation mode */
};

struct marpaESLIFRecognizer {
  /* The variables starting with "_" are not supposed to ever be accessed  */
  /* except in very precise situations (typically the new()/free() or when */
  /* faking a new() method). */
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaWrapperRecognizer_t    *marpaWrapperRecognizerp;
  genericStack_t              *lexemeInputStackp;  /* Internal input stack of lexemes */
  marpaESLIFEvent_t           *eventArrayp;      /* For the events */
  size_t                       eventArrayl;
  marpaESLIFRecognizer_t      *parentRecognizerp;
  int                          resumeCounteri;    /* Internal counter for tracing - no functional impact */
  int                          callstackCounteri; /* Internal counter for tracing - no functional impact */

  char                        *_buffers;       /* Pointer to allocated buffer containing input */
  size_t                       _bufferl;       /* Number of valid bytes in this buffer (!= allocated size) */
  size_t                       _bufferallocl;  /* Number of allocated bytes in this buffer (!= valid bytes) */
  short                        _eofb;          /* EOF flag */
  short                        _utfb;          /* A flag to say if input is UTF-8 correct. Automatically true if _charconv is true. Can set be regex engine as well. */
  short                        _charconvb;     /* A flag to say if latest stream chunk was converted to UTF-8 */

  int                          leveli;         /* Recognizer level (!= grammar level) */

  char                       **buffersp;       /* Pointer to allocated buffer - for sharing with eventual parent recognizers */
  size_t                      *bufferlp;       /* Ditto for the size */
  size_t                      *bufferalloclp;  /* Ditto for the allocated size */
  short                       *eofbp;          /* Ditto for the EOF flag */
  short                       *utfbp;          /* Ditto for the UTF-8 correctness flag */
  short                       *charconvbp;     /* Ditto for the character conversion flag */

  size_t                       parentDeltal;   /* Parent original delta - used to recovert parent current pointer at our free */
  char                        *inputs;         /* Current pointer in input - specific to every recognizer */
  size_t                       inputl;         /* Current remaining bytes - specific to every recognizer */
  short                        scanb;          /* Prevent resume before a call to scan */
  short                        discardb;       /* discard mode */
  short                        haveLexemeb;    /* Remember if this recognizer have at least one lexeme */
  size_t                       linel;          /* Line number */
  size_t                       columnl;        /* Column number */
  size_t                       bufsizl;        /* Effective bufsizl */
  size_t                       buftriggerl;    /* Minimum number of bytes to trigger crunch of data */
  short                        _nextReadIsFirstReadb; /* Flag to say if this is the first read ever done */
  short                        _noAnchorIsOkb;  /* Flag to say if the "A" flag in regexp modifiers is allowed: removing PCRE2_ANCHOR is allowed ONLY is the whole stream was read once */

  char                        *_encodings;     /* Current encoding. Always != NULL when _charconvb is true. Always NULL when charconvb is false. */
  marpaESLIF_terminal_t       *_encodingp;     /* Terminal case-insensitive version of current encoding. Always != NULL when _charconvb is true. Always NULL when charconvb is false. */
  tconv_t                      _tconvp;        /* current converter. Always != NULL when _charconvb is true. Always NULL when charconvb is false. */
  char                       **encodingsp;     /* Pointer to current encoding - shared between recognizers */
  marpaESLIF_terminal_t      **encodingpp;     /* Pointer to terminal case-insensitive version of current encoding */
  tconv_t                     *tconvpp;        /* Pointer to current converted - shared between recognizers */
  short                       *nextReadIsFirstReadbp;
  short                       *noAnchorIsOkbp;  /* Flag to say if the "A" flag in regexp modifiers is allowed: removing PCRE2_ANCHOR is allowed ONLY is the whole stream was read once */

};

/* ------------------------------- */
/* Definition of internal contexts */
/* ------------------------------- */

/* Internal reader context when parsing a grammar. Everything is in utf8s so the reader can say ok to any stream callback */
struct marpaESLIF_readerContext {
  marpaESLIF_t              *marpaESLIFp;
  marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp;
};

/* Internal structure to have value context information */
/* This is used in three contexts:
   - discard grammar
   - meta symbol that appears to be a terminal

   Both are using the "lexeme" mode, in which any action set in the grammar is ignored
   in favour of concatenating everything that matched.
*/
struct marpaESLIF_lexemeContext {
  genericStack_t        outputStack;
  genericStack_t       *outputStackp;
};

/* Internal structure to have clone context information */
struct marpaESLIF_cloneContext {
  marpaESLIF_t         *marpaESLIFp;
  marpaESLIF_grammar_t *grammarp;
};

/* ------------------------------------------- */
/* Definition of the default option structures */
/* ------------------------------------------- */
marpaESLIFOption_t marpaESLIFOption_default_template = {
  NULL               /* genericLoggerp */
};

marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption_default_template = {
  NULL,              /* userDatavp */
  NULL,              /* marpaESLIFReaderCallbackp */
  0,                 /* disableThresholdb */
  0,                 /* exhaustedb */
  0,                 /* rejectionb */
  0,                 /* newlineb */
  MARPAESLIF_BUFSIZ, /* bufsizl */
  50,                /* buftriggerperci */
  50                 /* bufaddperci */
};

marpaESLIFValueOption_t marpaESLIFValueOption_default_template = {
  NULL, /* userDatavp - filled at run-time */
  NULL, /* ruleCallbackp */
  NULL, /* symbolCallbackp */
  NULL, /* nullingCallbackp */
  1,    /* highRankOnlyb */
  1,    /* orderByRankb */
  0,    /* ambiguousb */
  0,    /* nullb */
  0     /* maxParsesi */
};

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_STRUCTURES_H */
