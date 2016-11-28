#ifndef MARPAESLIF_INTERNAL_STRUCTURES_H
#define MARPAESLIF_INTERNAL_STRUCTURES_H

#include <genericStack.h>
#include <pcre2.h>
#include <tconv.h>

#define INTERNAL_ANYCHAR_PATTERN "."                    /* This ASCII string is UTF-8 compatible */
#define INTERNAL_UTF8BOM_PATTERN "\\x{FEFF}"            /* FEFF Unicode code point i.e. EFBBBF in UTF-8 encoding */
#define INTERNAL_NEWLINE_PATTERN "(*BSR_UNICODE).*?\\R" /* newline as per unicode - we do .*? because our regexps are always anchored */
#define INTERNAL_STRINGMODIFIERS_PATTERN "i$"
#define INTERNAL_CHARACTERCLASSMODIFIERS_PATTERN "[eijmnsxDJUuaNubcA]+$"
#define INTERNAL_REGEXMODIFIERS_PATTERN "[eijmnsxDJUuaNubcA]*$"

typedef struct  marpaESLIF_regex           marpaESLIF_regex_t;
typedef         marpaESLIFString_t         marpaESLIF_string_t;
typedef enum    marpaESLIF_symbol_type     marpaESLIF_symbol_type_t;
typedef enum    marpaESLIF_terminal_type   marpaESLIF_terminal_type_t;
typedef struct  marpaESLIF_terminal        marpaESLIF_terminal_t;
typedef struct  marpaESLIF_meta            marpaESLIF_meta_t;
typedef         marpaESLIFSymbol_t         marpaESLIF_symbol_t;
typedef struct  marpaESLIF_rule            marpaESLIF_rule_t;
typedef struct  marpaESLIF_grammar         marpaESLIF_grammar_t;
typedef enum    marpaESLIF_matcher_value   marpaESLIF_matcher_value_t;
typedef enum    marpaESLIF_event_type      marpaESLIF_event_type_t;
typedef enum    marpaESLIF_array_type      marpaESLIF_array_type_t;
typedef struct  marpaESLIF_readerContext   marpaESLIF_readerContext_t;
typedef struct  marpaESLIF_cloneContext    marpaESLIF_cloneContext_t;
typedef         marpaESLIFStackType_t      marpaESLIF_stack_type_t;

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
struct marpaESLIF_regex_option_map {
  char                       modifierc;
  char                      *pcre2Options;
  marpaESLIF_uint32_t        pcre2Optioni;
  char                      *pcre2OptionNots;
  marpaESLIF_uint32_t        pcre2OptionNoti;
} marpaESLIF_regex_option_map[] = {
  { 'e', "PCRE2_MATCH_UNSET_BACKREF",                PCRE2_MATCH_UNSET_BACKREF,                NULL,              0 },
  { 'i', "PCRE2_CASELESS",                           PCRE2_CASELESS,                           NULL,              0 },
  { 'j', "PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF", PCRE2_ALT_BSUX|PCRE2_MATCH_UNSET_BACKREF, NULL,              0 },
  { 'm', "PCRE2_MULTILINE",                          PCRE2_MULTILINE,                          NULL,              0 },
  { 'n', "PCRE2_UCP",                                PCRE2_UCP,                                NULL,              0 },
  { 's', "PCRE2_DOTALL",                             PCRE2_DOTALL,                             NULL,              0 },
  { 'x', "PCRE2_EXTENDED",                           PCRE2_EXTENDED,                           NULL,              0 },
  { 'D', "PCRE2_DOLLAR_ENDONLY",                     PCRE2_DOLLAR_ENDONLY,                     NULL,              0 },
  { 'J', "PCRE2_DUPNAMES",                           PCRE2_DUPNAMES,                           NULL,              0 },
  { 'U', "PCRE2_UNGREEDY",                           PCRE2_UNGREEDY,                           NULL,              0 },
  { 'a', NULL,                                       0,                                        "PCRE2_UTF",       PCRE2_UTF },
  { 'N', NULL,                                       0,                                        "PCRE2_UCP",       PCRE2_UCP },
  { 'u', "PCRE2_UTF",                                PCRE2_UTF,                                NULL,              0 },
  { 'b', "PCRE2_NEVER_UTF",                          PCRE2_NEVER_UTF,                          "PCRE2_UTF",       PCRE2_UTF },
  { 'c', "PCRE2_UTF",                                PCRE2_UTF,                                "PCRE2_NEVER_UTF", PCRE2_NEVER_UTF },
  { 'A', NULL,                                       0,                                        "PCRE2_ANCHORED",  PCRE2_ANCHORED }
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
  char                       *modifiers;           /* Modifiers */
  char                       *patterns;            /* This is what is sent to PCRE2 and what defines exactly the terminal */
  size_t                      patternl;
  marpaESLIF_uint32_t         patterni;            /* ... this includes pattern options */
  marpaESLIF_terminal_type_t  type;                /* Original type. Used for description. When origin is STRING we know that patterns if ASCII safe */
  marpaESLIF_regex_t          regex;
};

struct marpaESLIF_meta {
  int                          idi;                       /* Non-terminal Id */
  char                        *asciinames;
  marpaESLIF_string_t         *descp;                           /* Non-terminal description */
  marpaWrapperGrammar_t       *marpaWrapperGrammarLexemeClonep; /* Cloned grammar in lexeme search mode (no event) */
  short                       *prioritizedb;                    /* Internal flag to prevent a prioritized symbol to appear more than once as an LHS */
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
struct marpaESLIFSymbol {
  marpaESLIF_symbol_type_t     type;  /* Symbol type */
  union {
    marpaESLIF_terminal_t     *terminalp; /* Symbol is a terminal */
    marpaESLIF_meta_t         *metap;     /* Symbol is a meta identifier, i.e. a rule */
  } u;
  short                        startb;                 /* Start symbol ? */
  short                        discardb;               /* Discard LHS symbol (i.e. :discard) ? */
  short                        discardRhsb;            /* Discard RHS symbol ? */
  short                        lhsb;                   /* Is an LHS somewhere in its grammar ? */
  short                        topb;                   /* Is a top-level symbol in its grammar - implies lhsb */
  int                          idi;                    /* Marpa ID */
  marpaESLIF_string_t         *descp;                  /* Symbol description */
  char                        *eventBefores;           /* Pause before */
  short                        eventBeforeb;           /* Pause before initial state: 0: off, 1: on */
  char                        *eventAfters;            /* Pause after */
  short                        eventAfterb;            /* Pause after initial state: 0: off, 1: on */
  char                        *eventPredicteds;        /* Event name for prediction */
  short                        eventPredictedb;        /* Prediction initial state: 0: off, 1: on */
  char                        *eventNulleds;           /* Event name for nulled */
  short                        eventNulledb;           /* Nulled initial state: 0: off, 1: on */
  char                        *eventCompleteds;        /* Event name for completion */
  short                        eventCompletedb;        /* Completion initial state: 0: off, 1: on */
  char                        *discardEvents;          /* Discard event name - shallow pointer to a :discard rule */
  short                        discardEventb;          /* Discard event initial state: 0: off, 1: on - copy of :discard's rule value */
  int                          lookupLevelDeltai;      /* Referenced grammar delta level */
  char                        *lookupMetas;            /* Referenced lookup meta name - shallow pointer */
  int                          lookupResolvedLeveli;   /* Referenced grammar level */
  int                          priorityi;              /* Symbol priority */
  char                        *actions;                /* Action */
  unsigned int                 nbupdatei;              /* Number of updates - used in grammar ESLIF actions */
  genericStack_t              *nullableRuleStackp;     /* Used during validation, to determine nullable semantics */
  char                        *nullableActions;        /* Nullable semantic */
  int                          propertyBitSet;
  genericStack_t              *lhsRuleStackp;          /* Stack of rules having this symbol as LHS */
};

/* A rule */
struct marpaESLIF_rule {
  int                    idi;                             /* Rule Id */
  marpaESLIF_string_t   *descp;                           /* Rule alternative name */
  short                  descautob;                       /* True if alternative name was autogenerated */
  char                  *asciishows;                      /* Rule show (ASCII) */
  marpaESLIF_symbol_t   *lhsp;                            /* LHS symbol */
  marpaESLIF_symbol_t   *separatorp;                      /* Eventual separator symbol */
  genericStack_t        *rhsStackp;                       /* Stack of RHS symbols */
  marpaESLIF_symbol_t   *exceptionp;                      /* Exception symbol */
  marpaWrapperGrammar_t *marpaWrapperGrammarExceptionp; /* Grammar implementation at exception */
  char                  *actions;                         /* Action */
  char                  *discardEvents;                   /* Discard event name - shallowed with its RHS */
  short                  discardEventb;                   /* Discard event initial state: 0: off, 1: on - copied to its RHS */
  int                    ranki;
  short                  nullRanksHighb;
  short                  sequenceb;
  short                  properb;
  int                    minimumi;
  short                  passthroughb;                    /* This rule is a passthrough */
  int                    propertyBitSet;
};

/* A grammar */
struct marpaESLIF_grammar {
  marpaESLIF_t          *marpaESLIFp;                 /* Shallow pointer to top marpaESLIFp */
  marpaESLIF_grammar_t  *selfp;                       /* This is vicious: internally the grammar is duplicated on the stack. This pointer give the original allocated memory. */
  int                    leveli;                      /* Grammar level */
  marpaESLIF_string_t   *descp;                       /* Grammar description */
  short                  descautob;                   /* True if alternative name was autogenerated */
  short                  latmb;                       /* Longest acceptable token match mode */
  marpaWrapperGrammar_t *marpaWrapperGrammarStartp;   /* Grammar implementation at :start */
  marpaWrapperGrammar_t *marpaWrapperGrammarDiscardp; /* Grammar implementation at :discard */
  marpaESLIF_symbol_t   *discardp;                    /* Discard symbol, used at grammar validation */
  genericStack_t        *symbolStackp;                /* Stack of symbols */
  genericStack_t        *ruleStackp;                  /* Stack of rules */
  char                  *defaultSymbolActions;        /* Default action for symbols */
  char                  *defaultRuleActions;          /* Default action for rules */
  char                  *defaultFreeActions;          /* Default action for free */
  int                    starti;                      /* Default start symbol ID - filled during grammar validation */
  char                  *starts;                      /* Default start symbol name - filled during grammar validation - shallow pointer */
  int                   *ruleip;                      /* Array of rule IDs - filled by grammar validation */
  size_t                 rulel;                       /* Size of the rule IDs array - filled by grammar validation */
  short                  haveRejectionb;              /* When true, valuation go to the ASF mode - filled by grammar validation */
  unsigned int           nbupdatei;                   /* Number of updates - used in grammar ESLIF actions */
  char                  *asciishows;                  /* Grammar show (ASCII) */
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
  marpaESLIF_terminal_t *stringModifiersp;            /* Internal regex for match string modifiers */
  marpaESLIF_terminal_t *characterClassModifiersp;    /* Internal regex for match character class modifiers */
  marpaESLIF_terminal_t *regexModifiersp;             /* Internal regex for match regex modifiers */
};

struct marpaESLIFGrammar {
  marpaESLIF_t             *marpaESLIFp;
  genericStack_t           *grammarStackp;
  marpaESLIF_grammar_t     *grammarp;          /* This is a SHALLOW copy of current grammar in grammarStackp, defaulting to the top grammar */
  short                     warningIsErrorb;   /* Current warningIsErrorb setting (used when parsing grammars ) */
  short                     warningIsIgnoredb; /* Current warningIsErrorb setting (used when parsing grammars ) */
  short                     autorankb;         /* Current autorank setting */
};

struct marpaESLIFValue {
  marpaESLIF_t             *marpaESLIFp;
  marpaESLIFRecognizer_t   *marpaESLIFRecognizerp;
  marpaESLIFValueOption_t   marpaESLIFValueOption;
  marpaWrapperValue_t      *marpaWrapperValuep;
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  short                     previousPassWasPassthroughb;
  int                       previousArg0i;
  int                       previousArgni;
  genericStack_t           *valueStackp;
  genericStack_t           *typeStackp;
  genericStack_t           *contextStackp;
  short                     inValuationb;
  int                       symboli;
  int                       rulei;
  int                       grammari;
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
  marpaESLIFEvent_t           *eventArrayp;        /* For the events */
  size_t                       eventArrayl;        /* Current number of events */
  size_t                       eventArraySizel;    /* Real allocated size (to avoid constant free/deletes) */
  marpaESLIFRecognizer_t      *parentRecognizerp;
  char                        *lastCompletionEvents; /* A trick to avoid having to fetch the array event when a discard subgrammar succeed */
  char                        *discardEvents;     /* Set by a child discard recognizer that reaches a completion event */
  int                          resumeCounteri;    /* Internal counter for tracing - no functional impact */
  int                          callstackCounteri; /* Internal counter for tracing - no functional impact */

  char                        *_buffers;       /* Pointer to allocated buffer containing input */
  size_t                       _bufferl;       /* Number of valid bytes in this buffer (!= allocated size) */
  size_t                       _bufferallocl;  /* Number of allocated bytes in this buffer (!= valid bytes) */
  char                        *_globalOffsetp; /* The offset between the original start of input, and current buffer */
  short                        _eofb;          /* EOF flag */
  short                        _utfb;          /* A flag to say if input is UTF-8 correct. Automatically true if _charconv is true. Can set be regex engine as well. */
  short                        _charconvb;     /* A flag to say if latest stream chunk was converted to UTF-8 */

  int                          leveli;         /* Recognizer level (!= grammar level) */

  char                       **buffersp;       /* Pointer to allocated buffer - for sharing with eventual parent recognizers */
  size_t                      *bufferlp;       /* Ditto for the size */
  size_t                      *bufferalloclp;  /* Ditto for the allocated size */
  char                       **globalOffsetpp; /* Pointer to offset between the original start of input, and current buffer */
  short                       *eofbp;          /* Ditto for the EOF flag */
  short                       *utfbp;          /* Ditto for the UTF-8 correctness flag */
  short                       *charconvbp;     /* Ditto for the character conversion flag */

  size_t                       parentDeltal;   /* Parent original delta - used to recovert parent current pointer at our free */
  char                        *inputs;         /* Current pointer in input - specific to every recognizer */
  size_t                       inputl;         /* Current remaining bytes - specific to every recognizer */
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

  /* Current recognizer states */
  short                        scanb;          /* Prevent resume before a call to scan */
  short                        discardb;       /* Discard mode */
  short                        exceptionb;     /* Exception mode */
  short                        silentb;        /* Silent mode */
  short                        haveLexemeb;    /* Remember if this recognizer have at least one lexeme */
  size_t                       linel;          /* Line number */
  size_t                       columnl;        /* Column number */
  short                        exhaustedb;     /* Internally, every recognizer need to know if parsing is exhausted */
  short                        completedb;     /* Ditto for completion (used in case od discard events) */
  short                        continueb;
  size_t                       alternativeLengthl;
  genericStack_t              *alternativeStackp;       /* Current alternative stack */
  genericStack_t              *alternativeStackWorkp;   /* Work alternative stack */
  genericStack_t              *commitedAlternativeStackp; /* Commited alternative stack (internal + external) */
  char                        *pauses;        /* Last pause data */
  size_t                       pausel;        /* Last pause length */
  size_t                       pauseSizel;    /* Last pause allocate size */
  genericStack_t              *set2InputStackp;
};

/* ------------------------------- */
/* Definition of internal contexts */
/* ------------------------------- */

/* Internal reader context when parsing a grammar. Everything is in utf8s so the reader can say ok to any stream callback */
struct marpaESLIF_readerContext {
  marpaESLIF_t              *marpaESLIFp;
  marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp;
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
  0,                 /* newlineb */
  MARPAESLIF_BUFSIZ, /* bufsizl */
  50,                /* buftriggerperci */
  50                 /* bufaddperci */
};

marpaESLIFValueOption_t marpaESLIFValueOption_default_template = {
  NULL, /* userDatavp - filled at run-time */
  NULL, /* ruleActionResolverp */
  NULL, /* symbolActionResolverp */
  NULL, /* freeActionResolverp */
  1,    /* highRankOnlyb */
  1,    /* orderByRankb */
  0,    /* ambiguousb */
  0,    /* nullb */
  0     /* maxParsesi */
};

#include "marpaESLIF/internal/eslif.h"

#endif /* MARPAESLIF_INTERNAL_STRUCTURES_H */
