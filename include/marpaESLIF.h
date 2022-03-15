#ifndef MARPAESLIF_H
#define MARPAESLIF_H

#include <stddef.h>                 /* For size_t */
#include <stdio.h>                  /* For __int64, eventually */
#include <limits.h>                 /* For long long, eventually */
#include <genericLogger.h>
#include <marpaESLIF/export.h>

/* ===================================== */
/* long long detection using only macros */
/* ===================================== */
/* It is recommended to use the macro    */
/* MARPAESLIF_LONG_LONG if               */
/* MARPAESLIF_HAVE_LONG_LONG is defined. */
/* ===================================== */
#if defined(LLONG_MAX) && defined(LLONG_MIN)
#  define MARPAESLIF_HAVE_LONG_LONG
#  define MARPAESLIF_LONG_LONG long long
#  define MARPAESLIF_LLONG_MAX LLONG_MAX
#  define MARPAESLIF_LLONG_MIN LLONG_MIN
#  define MARPAESLIF_LONG_LONG_FMT "%lld"
#else
#  if defined(LONGLONG_MAX) && defined(LONGLONG_MIN)
#    define MARPAESLIF_HAVE_LONG_LONG
#    define MARPAESLIF_LONG_LONG long long
#    define MARPAESLIF_LLONG_MAX LONGLONG_MAX
#    define MARPAESLIF_LLONG_MIN LONGLONG_MIN
#    define MARPAESLIF_LONG_LONG_FMT "%lld"
#  else
#    if defined(_MSC_VER) || defined(__BORLANDC__)
/*     Just because of versions of these compilers might not have long long, but they */
/*     always had __int64. Note that on Windows short is always 2, int is always 4,   */
/*     long is always 4, __int64 is always 8 */
#      define MARPAESLIF_HAVE_LONG_LONG
#      define MARPAESLIF_LONG_LONG __int64
#      if defined(_I64_MAX) && defined(_I64_MIN)
#        define MARPAESLIF_LLONG_MAX _I64_MAX
#        define MARPAESLIF_LLONG_MIN _I64_MIN
#      else
#        define MARPAESLIF_LLONG_MAX 9223372036854775807i64
#        define MARPAESLIF_LLONG_MIN -9223372036854775808i64
#      endif
#      define MARPAESLIF_LONG_LONG_FMT "%I64d"
#    else
#      undef MARPAESLIF_HAVE_LONG_LONG
#    endif
#  endif
#endif

/* ============================================= */
/* Forward declarations needed for the callbacks */
/* ============================================= */
typedef struct marpaESLIFRecognizer        marpaESLIFRecognizer_t;
typedef struct marpaESLIFValue             marpaESLIFValue_t;
typedef struct marpaESLIFValueResult       marpaESLIFValueResult_t;
typedef enum   marpaESLIFValueResultBool   marpaESLIFValueResultBool_t;
typedef int                                marpaESLIFValueResultInt_t;
typedef struct marpaESLIFValueResultString marpaESLIFValueResultString_t;
typedef struct marpaESLIFEvent             marpaESLIFEvent_t;
typedef struct marpaESLIFSymbol            marpaESLIFSymbol_t;

/* ========= */
/* Callbacks */
/* ========= */
/* marpaESLIFValueResult free callback */
typedef void  (*marpaESLIFValueResultFreeCallback_t)(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

/* Reader recognizer callback. It returns encoding information, giving eventual encoding in *encodingsp, spreaded over *encodinglp bytes. Encoding of encoding is free. */
/* The content of inputcpp and encodingsp might be volatile data that the end-user want to dispose immediately after the reader callback, therefore there is a */
/* disposal argument that is guaranteed to be called if it is not NULL. */
/* A typical situation is e.g. the JNI environment: inputcp may be the result of a call to GetByteArrayElements(). This implies a call to ReleaseByteArrayElements(). */
typedef void  (*marpaESLIFReaderDispose_t)(void *userDatavp, char *inputcp, size_t inputl, short eofb, short characterStreamb, char *encodings, size_t encodingl);
typedef short (*marpaESLIFReader_t)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);

/* If recognizer callback: marpaESLIFValueResultp is of type ARRAY and represents the binary memory chunk of current symbol */
typedef short (*marpaESLIFRecognizerIfCallback_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
typedef marpaESLIFRecognizerIfCallback_t (*marpaESLIFRecognizerIfActionResolver_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);

/* Event recognizer callback */
typedef short (*marpaESLIFRecognizerEventCallback_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFEvent_t *eventArrayp, size_t eventArrayl, marpaESLIFValueResultBool_t *marpaESLIFValueResultBoolp);
typedef marpaESLIFRecognizerEventCallback_t (*marpaESLIFRecognizerEventActionResolver_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);

/* ------------------------------------------------------------------------------------------------ */
/* regex recognizer callback: marpaESLIFCalloutBlockp is of type TABLE:                             */
/*                                                                                                  */
/* KeyType key                => valueType       value                              key/pair indice */
/* ------- ---                   ---------       -----                                              */
/* STRING  "callout_number"   => LONG or UNDEF   Callout number                                   0 */
/* STRING  "callout_string"   => ARRAY or UNDEF  Callout string (*)                               1 */
/* STRING  "subject"          => ARRAY           Subject (*)                                      2 */
/* STRING  "pattern"          => ARRAY           Pattern (*)                                      3 */
/* STRING  "capture_top"      => LONG            Max current capture                              4 */
/* STRING  "capture_last"     => LONG            Most recently closed capture                     5 */
/* STRING  "offset_vector"    => ROW of LONGs    Offset vector                                    6 */
/* STRING  "mark"             => LONG or UNDEF   Current mark offset                              7 */
/* STRING  "start_match"      => LONG            Current match start attempt offset               8 */
/* STRING  "current_position" => LONG            Current subject offset                           9 */
/* STRING  "next_item"        => ARRAY or UNDEF  Next item in the pattern (*)                    10 */
/* STRING  "grammar_level"    => INT             The grammar level                               11 */
/* STRING  "symbol_id"        => INT             The symbol identifier                           12 */
/*                                                                                                  */
/* (*) Direct memory pointer is returned. Any attempt to change the underlying bytes will result    */
/*     to undefined behaviour.                                                                      */
/*     ESLIF guarantees that:                                                                       */
/*     - subject correspond to the output of marpaESLIFRecognizer_input()                           */
/*     - pattern can be retreived using the symbol's description using symbol_id value in input to  */
/*       marpaESLIFGrammar_symbolproperty_by_levelb().                                              */
/*                                                                                                  */
/* ESLIF guarantees that this marpaESLIFCalloutBlockp is filled in this exact order, therefore      */
/* using an indice in the marpaESLIFCalloutBlockEnum_t enum below is safe.                          */
/* ------------------------------------------------------------------------------------------------ */
typedef enum marpaESLIFCalloutBlockEnum {
                                         MARPAESLIFCALLOUTBLOCK_CALLOUT_NUMBER = 0,
                                         MARPAESLIFCALLOUTBLOCK_CALLOUT_STRING,
                                         MARPAESLIFCALLOUTBLOCK_SUBJECT,
                                         MARPAESLIFCALLOUTBLOCK_PATTERN,
                                         MARPAESLIFCALLOUTBLOCK_CAPTURE_TOP,
                                         MARPAESLIFCALLOUTBLOCK_CAPTURE_LAST,
                                         MARPAESLIFCALLOUTBLOCK_OFFSET_VECTOR,
                                         MARPAESLIFCALLOUTBLOCK_MARK,
                                         MARPAESLIFCALLOUTBLOCK_START_MATCH,
                                         MARPAESLIFCALLOUTBLOCK_CURRENT_POSITION,
                                         MARPAESLIFCALLOUTBLOCK_NEXT_ITEM,
                                         MARPAESLIFCALLOUTBLOCK_GRAMMAR_LEVEL,
                                         MARPAESLIFCALLOUTBLOCK_SYMBOL_ID,
                                         _MARPAESLIFCALLOUTBLOCK_SIZE
} marpaESLIFCalloutBlockEnum_t;

/* marpaESLIFCallout table Keys */
static const char *marpaESLIFCalloutKeysp[_MARPAESLIFCALLOUTBLOCK_SIZE] = {
  "callout_number",
  "callout_string",
  "subject",
  "pattern",
  "capture_top",
  "capture_last",
  "offset_vector",
  "mark",
  "start_match",
  "current_position",
  "next_item",
  "grammar_level",
  "symbol_id"
};


/* -------------------------------------------------------------------------------- */
/* You MUST refer to                                                                */
/* https://www.pcre.org/current/doc/html/pcre2callout.html#calloutinterface         */
/* for explanation of "capture_top", "capture_last" and "ovector".                  */
/*                                                                                  */
/* -------------------------------------------------------------------------------- */
/* If callback is successful, then *marpaESLIFValueResultOutp drives PCRE2 engine:  */
/* -   0 means ok                                                                   */
/* - > 0 means fails at current point but continue                                  */
/*   < 0 means failure and send to PCRE2 engine                                     */
/*                                                                                  */
/* If callback is not successful, PCRE2_ERROR_CALLOUT is sent to PCRE2              */
/* If callback is successful and *marpaESLIFValueResultOutp is < 0, marpaESLIF      */
/* makes sure that *marpaESLIFValueResultOutp has a meaningul value for PCRE2. In   */
/* other words, it depends on the buily-in PCRE2 version. Current possible values   */
/* are:                                                                             */
/* -1                   No Match                                                    */
/* -2                   Partial match                                               */
/* ...                                                                              */
/* -65                  Last supported value                                        */
/*                                                                                  */
/* Values -1 and -2 are not errors per-se. Any other value down to -65 is a fatal   */
/* error for the match. The meaning of all the constants can be looked in the file  */
/* 3rdparty/untar/pcre2-10.33/src/pcre2.h.in                                        */
/* -------------------------------------------------------------------------------- */
typedef short (*marpaESLIFRecognizerRegexCallback_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFCalloutBlockp, marpaESLIFValueResultInt_t *marpaESLIFValueResultOutp);
typedef marpaESLIFRecognizerRegexCallback_t (*marpaESLIFRecognizerRegexActionResolver_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);

typedef short (*marpaESLIFRecognizerGeneratorCallback_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *contextp, marpaESLIFValueResultString_t *marpaESLIFValueResultOutp);
typedef marpaESLIFRecognizerGeneratorCallback_t (*marpaESLIFRecognizerGeneratorActionResolver_t)(void *userDatavp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *actions);

/* Ask the host system to import a marpaESLIFValueResult in the recognizer namespace */
typedef short (*marpaESLIFRecognizerImport_t)(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb);

typedef struct marpaESLIFRecognizerOption {
  void                                          *userDatavp;          /* User specific context */
  marpaESLIFReader_t                             readerCallbackp;     /* Reader */
  short                                          disableThresholdb;   /* Default: 0 */
  short                                          exhaustedb;          /* Exhaustion event. Default: 0 */
  short                                          newlineb;            /* Count line/column numbers. Default: 0 */
  short                                          trackb;              /* Track absolute position. Default: 0 */
  size_t                                         bufsizl;             /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  unsigned int                                   buftriggerperci;     /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  unsigned int                                   bufaddperci;         /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
  marpaESLIFRecognizerIfActionResolver_t         ifActionResolverp;   /* Will return the function doing the wanted if action */
  marpaESLIFRecognizerEventActionResolver_t      eventActionResolverp; /* Will return the function doing the wanted event action */
  marpaESLIFRecognizerRegexActionResolver_t      regexActionResolverp; /* Will return the function doing the wanted regex callout action */
  marpaESLIFRecognizerGeneratorActionResolver_t  generatorActionResolverp; /* Will return the function doing the wanted symbol generation action */
  marpaESLIFRecognizerImport_t                   importerp;           /* If end-user want to import a marpaESLIFValueResult */
} marpaESLIFRecognizerOption_t;

typedef enum marpaESLIFEventType {
  MARPAESLIF_EVENTTYPE_NONE       = 0x00,
  MARPAESLIF_EVENTTYPE_COMPLETED  = 0x01, /* Grammar event */
  MARPAESLIF_EVENTTYPE_NULLED     = 0x02, /* Grammar event */
  MARPAESLIF_EVENTTYPE_PREDICTED  = 0x04, /* Grammar event */
  MARPAESLIF_EVENTTYPE_BEFORE     = 0x08, /* Just before symbol is commited */
  MARPAESLIF_EVENTTYPE_AFTER      = 0x10, /* Just after symbol is commited */
  MARPAESLIF_EVENTTYPE_EXHAUSTED  = 0x20, /* Exhaustion */
  MARPAESLIF_EVENTTYPE_DISCARD    = 0x40  /* Discard */
} marpaESLIFEventType_t;

struct marpaESLIFEvent {
  marpaESLIFEventType_t type;
  char                 *symbols; /* Symbol name, always NULL if exhausted event, always ':discard' if discard event */
  char                 *events;  /* Event name, always NULL if exhaustion eent */
};

typedef enum marpaESLIFValueType {
  MARPAESLIF_VALUE_TYPE_UNDEF = 0,
  MARPAESLIF_VALUE_TYPE_CHAR,
  MARPAESLIF_VALUE_TYPE_SHORT,
  MARPAESLIF_VALUE_TYPE_INT,
  MARPAESLIF_VALUE_TYPE_LONG,
  MARPAESLIF_VALUE_TYPE_FLOAT,
  MARPAESLIF_VALUE_TYPE_DOUBLE,
  MARPAESLIF_VALUE_TYPE_PTR,
  MARPAESLIF_VALUE_TYPE_ARRAY,
  MARPAESLIF_VALUE_TYPE_BOOL,
  MARPAESLIF_VALUE_TYPE_STRING,
  MARPAESLIF_VALUE_TYPE_ROW,
  MARPAESLIF_VALUE_TYPE_TABLE,
  MARPAESLIF_VALUE_TYPE_LONG_DOUBLE
#ifdef MARPAESLIF_HAVE_LONG_LONG
  ,MARPAESLIF_VALUE_TYPE_LONG_LONG
#endif
} marpaESLIFValueType_t;

/* Value callback definitions */
typedef short (*marpaESLIFValueRuleCallback_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
typedef short (*marpaESLIFValueSymbolCallback_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResult, int resulti);

typedef marpaESLIFValueRuleCallback_t (*marpaESLIFValueRuleActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
typedef marpaESLIFValueSymbolCallback_t (*marpaESLIFValueSymbolActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);

/* --------------------------------------------------------------------------------------------- */
/* The representation returns a sequence of bytes, eventually meaning a string                   */
/* It is legal to return NULL in *inputcpp or 0 in *inputlp: the callback will be like a no-op.  */
/* It is legal to return NULL in encodingmaybesp, then this is an opaque sequence of bytes, else */
/* this is considered as a string in this given encoding, /unless/ *stringbp is overwritten.     */
/* Note that it is the responsibility of the caller to make sure that *inputcpp and              */
/* **encodingmaybesp points to valid memory area when the call returns.                          */
/* The *disposeCallbackpp, if set, will be called back to release memory in user-land.           */
/*                                                                                               */
/* A specific case in when marpaESLIFValueResultp->type is MARPAESLIF_VALUE_TYPE_PTR:            */
/* It is not a hasard that ESLIF never produces PTR internally, such type can only be injected   */
/* by an external call, and usually represents something that the end-user wants to remain       */
/* opaque to ESLIF. In such a case, if there is a representation callback, the end-user can      */
/* inform ESLIF if he wanted to use marpaESLIFValueResultp as a string or not.                   */
/* This has an impact on how ESLIF will represent *inputcp depending on serialization context    */
/* (e.g. the :json action).                                                                      */
/* The default value of *stringbp is always the safe thing, i.e. a true value.                   */
/* The eventual overwrite of *stringbp is ignored when marpaESLIFValueResultp->type is not       */
/* MARPAESLIF_VALUE_TYPE_PTR.                                                                    */
/* Typical usages are:                                                                           */
/* - Host variables whose type depend on the language context, e.g. in Perl.                     */
/* - Host languages that took over number representations, e.g. Math::BigDecimal in              */
/*   Java, Math::BigFloat and Math::BigInt in Perl.                                              */
/* It is strongly advised to overwrite *stringb to 0 /ONLY/ when the intended usage is a number. */
/* --------------------------------------------------------------------------------------------- */
typedef void (*marpaESLIFRepresentationDispose_t)(void *userDatavp, char *inputcp, size_t inputl, char *encodingasciis);
typedef short (*marpaESLIFRepresentation_t)(void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, char **inputcpp, size_t *inputlp, char **encodingasciisp, marpaESLIFRepresentationDispose_t *disposeCallbackpp, short *stringbp);

/* --------------------------------------------------------------------------------------------- */
/* Valuation result                                                                              */
/* --------------------------------------------------------------------------------------------- */
typedef char marpaESLIFValueResultChar_t;
typedef short marpaESLIFValueResultShort_t;
typedef long marpaESLIFValueResultLong_t;
typedef float marpaESLIFValueResultFloat_t;
typedef double marpaESLIFValueResultDouble_t;
typedef struct marpaESLIFValueResultPtr {
  void                                *p;
  short                                shallowb;
  void                                *freeUserDatavp;
  marpaESLIFValueResultFreeCallback_t  freeCallbackp;
} marpaESLIFValueResultPtr_t;
typedef struct marpaESLIFValueResultArray {
  char                                *p;
  short                                shallowb;
  void                                *freeUserDatavp;
  marpaESLIFValueResultFreeCallback_t  freeCallbackp;
  size_t                               sizel;
} marpaESLIFValueResultArray_t;
enum marpaESLIFValueResultBool {
  MARPAESLIFVALUERESULTBOOL_FALSE = 0,
  MARPAESLIFVALUERESULTBOOL_TRUE = 1
};
struct marpaESLIFValueResultString {
  unsigned char                       *p;
  short                                shallowb;
  void                                *freeUserDatavp;
  marpaESLIFValueResultFreeCallback_t  freeCallbackp;
  size_t                               sizel;
  char                                *encodingasciis;
};
typedef struct marpaESLIFValueResultRow {
  marpaESLIFValueResult_t             *p;
  short                                shallowb;
  void                                *freeUserDatavp;
  marpaESLIFValueResultFreeCallback_t  freeCallbackp;
  size_t                               sizel;
} marpaESLIFValueResultRow_t;
typedef struct marpaESLIFValueResultPair marpaESLIFValueResultPair_t;
typedef struct marpaESLIFValueResultTable {
  marpaESLIFValueResultPair_t         *p;
  short                                shallowb;
  void                                *freeUserDatavp;
  marpaESLIFValueResultFreeCallback_t  freeCallbackp;
  size_t                               sizel;
} marpaESLIFValueResultTable_t;
typedef long double marpaESLIFValueResultLongDouble_t;
#ifdef MARPAESLIF_HAVE_LONG_LONG
typedef MARPAESLIF_LONG_LONG marpaESLIFValueResultLongLong_t;
#endif

struct marpaESLIFValueResult {
  void                      *contextp;          /* Free value meaningful only to the user */
  marpaESLIFRepresentation_t representationp;   /* How a user-land alternative is represented if it was in the input */
  marpaESLIFValueType_t      type;              /* Type for tagging the following union */
  union {
    marpaESLIFValueResultChar_t        c; /* Value is a char */
    marpaESLIFValueResultShort_t       b; /* Value is a short */
    marpaESLIFValueResultInt_t         i; /* Value is an int */
    marpaESLIFValueResultLong_t        l; /* Value is a long */
    marpaESLIFValueResultFloat_t       f; /* Value is a float */
    marpaESLIFValueResultDouble_t      d; /* Value is a double */
    marpaESLIFValueResultPtr_t         p; /* Value is a pointer */
    marpaESLIFValueResultArray_t       a; /* Value is a byte array */
    marpaESLIFValueResultBool_t        y; /* Value is a boolean */
    marpaESLIFValueResultString_t      s; /* Value is a string */
    marpaESLIFValueResultRow_t         r; /* Value is a row of values */
    marpaESLIFValueResultTable_t       t; /* Value is a row of values, where sizel is even */
    marpaESLIFValueResultLongDouble_t ld; /* Value is a long double */
#ifdef MARPAESLIF_HAVE_LONG_LONG
    marpaESLIFValueResultLongLong_t   ll; /* Value is a long long */
#endif
  } u;
};
/* Now that marpaESLIFValueResult is defined, we can define the pair */
struct marpaESLIFValueResultPair {
  marpaESLIFValueResult_t    key;
  marpaESLIFValueResult_t    value;
};

/* An alternative from external lexer point of view */
typedef struct marpaESLIFAlternative {
  char                      *names;       /* Symbol name (from automatic description or explicit name adverb) */
  marpaESLIFValueResult_t    value;       /* Value */
  size_t                     grammarLengthl; /* Length within the grammar (1 in the token-stream model) */
} marpaESLIFAlternative_t;

/* Ask the host system to import a marpaESLIFValueResult in the valuator namespace */
typedef short (*marpaESLIFValueImport_t)(marpaESLIFValue_t *marpaESLIFValuep, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb);

typedef struct marpaESLIFValueOption {
  void                                 *userDatavp;            /* User specific context */
  marpaESLIFValueRuleActionResolver_t   ruleActionResolverp;   /* Will return the function doing the wanted rule action */
  marpaESLIFValueSymbolActionResolver_t symbolActionResolverp; /* Will return the function doing the wanted symbol action */
  marpaESLIFValueImport_t               importerp;             /* Will ask end-user to import a marpaESLIFValueResult */
  short                                 highRankOnlyb;         /* Default: 1 */
  short                                 orderByRankb;          /* Default: 1 */
  short                                 ambiguousb;            /* Default: 0 */
  short                                 nullb;                 /* Default: 0 */
  int                                   maxParsesi;            /* Default: 0 */
} marpaESLIFValueOption_t;

typedef struct marpaESLIFRecognizerProgress {
  int earleySetIdi;
  int earleySetOrigIdi;
  int rulei;
  int positioni;
} marpaESLIFRecognizerProgress_t;

typedef enum marpaESLIFActionType {
  MARPAESLIF_ACTION_TYPE_NAME = 0,
  MARPAESLIF_ACTION_TYPE_STRING,
  MARPAESLIF_ACTION_TYPE_LUA,
  MARPAESLIF_ACTION_TYPE_LUA_FUNCTION
} marpaESLIFActionType_t;

/* A string */
typedef struct marpaESLIFString {
  char   *bytep;            /* pointer bytes */
  size_t  bytel;            /* number of bytes */
  char   *encodingasciis;   /* Encoding of bytes, itself being writen in ASCII encoding, NUL byte terminated */
  char   *asciis;           /* ASCII (un-translatable bytes are changed to a replacement character) translation of previous bytes, NUL byte terminated - never NULL if bytep is not NULL */
  /*
   * Remark: the encodings and asciis pointers are not NULL only when ESLIF know that the buffer is associated to a "description". I.e.
   * this is happening ONLY when parsing the grammar. Raw data never have non-NULL asciis or encodings.
   */
} marpaESLIFString_t;

typedef struct marpaESLIFLuaFunction {
  char  *luas;       /* Original action as per the grammar */
  char  *actions;    /* The action injected into lua */
  short  luacb;      /* True if action was in the form ::luac->function */
  char  *luacp;      /* Precompiled chunk. Not NULL only when luacb is true and action as been used at least once */
  size_t luacl;      /* Precompiled chunk length */
  char  *luacstripp; /* Precompiled stripped chunk - never NULL (it is done during grammar validation) */
  size_t luacstripl; /* Precompiled stripped chunk length */
} marpaESLIFLuaFunction_t;

typedef struct marpaESLIFAction {
  marpaESLIFActionType_t type;
  union {
    char                          *names;
    marpaESLIFString_t            *stringp;
    char                          *luas;
    marpaESLIFLuaFunction_t        luaFunction;
  } u;
} marpaESLIFAction_t;

typedef struct marpaESLIFGrammarDefaults {
  marpaESLIFAction_t *defaultRuleActionp;      /* Default action for rules */
  marpaESLIFAction_t *defaultSymbolActionp;    /* Default action for symbols */
  marpaESLIFAction_t *defaultEventActionp;     /* Default action for events */
  marpaESLIFAction_t *defaultRegexActionp;     /* Default action for regex */
  char               *defaultEncodings;        /* Default encoding */
  char               *fallbackEncodings;       /* Fallback encoding */
} marpaESLIFGrammarDefaults_t;

/* Rule property */

typedef enum marpaESLIFRulePropertyBit {
  MARPAESLIF_RULE_IS_ACCESSIBLE = 0x01,
  MARPAESLIF_RULE_IS_NULLABLE   = 0x02,
  MARPAESLIF_RULE_IS_NULLING    = 0x04,
  MARPAESLIF_RULE_IS_LOOP       = 0x08,
  MARPAESLIF_RULE_IS_PRODUCTIVE = 0x10
} marpaESLIFRulePropertyBit_t;

/* Symbol Property */
typedef enum marpaESLIFSymbolPropertyBit {
  MARPAESLIF_SYMBOL_IS_ACCESSIBLE = 0x01,
  MARPAESLIF_SYMBOL_IS_NULLABLE   = 0x02,
  MARPAESLIF_SYMBOL_IS_NULLING    = 0x04,
  MARPAESLIF_SYMBOL_IS_PRODUCTIVE = 0x08,
  MARPAESLIF_SYMBOL_IS_START      = 0x10,
  MARPAESLIF_SYMBOL_IS_TERMINAL   = 0x20
} marpaESLIFSymbolPropertyBit_t;

typedef enum marpaESLIFSymbolEventBit {
  MARPAESLIF_SYMBOL_EVENT_COMPLETION = 0x01,
  MARPAESLIF_SYMBOL_EVENT_NULLED     = 0x02,
  MARPAESLIF_SYMBOL_EVENT_PREDICTION = 0x04
} marpaESLIFSymbolEventBit_t;

typedef struct marpaESLIFGrammarProperty {
  int                    leveli;                  /* Grammar level */
  int                    maxLeveli;               /* Maximum grammar level */
  marpaESLIFString_t    *descp;                   /* Grammar description (auto-generated if none) */
  short                  latmb;                   /* LATM ? */
  short                  discardIsFallbackb;      /* Discard is fallback ? */
  short                  terminalsAreExclusiveb;  /* Terminals are exclusive ? */
  marpaESLIFAction_t    *defaultSymbolActionp;    /* Default action for symbols - never NULL */
  marpaESLIFAction_t    *defaultRuleActionp;      /* Default action for rules - never NULL */
  marpaESLIFAction_t    *defaultEventActionp;     /* Default action for event - can be NULL */
  marpaESLIFAction_t    *defaultRegexActionp;     /* Default action for regex - can be NULL */
  int                    starti;                  /* Start symbol Id - always >= 0 */
  int                    discardi;                /* Discard symbol Id (-1 if none) */
  size_t                 nsymboll;                /* Number of symbols - always > 0*/
  int                   *symbolip;                /* Array of symbols Ids - never NULL */
  size_t                 nrulel;                  /* Number of rules - always > 0*/
  int                   *ruleip;                  /* Array of rule Ids - never NULL */
  char                  *defaultEncodings;        /* Default encoding */
  char                  *fallbackEncodings;       /* Fallback encoding */
} marpaESLIFGrammarProperty_t;

typedef struct marpaESLIFRuleProperty {
  int                    idi;                          /* Rule Id */
  marpaESLIFString_t    *descp;                        /* Rule alternative name (auto-generated if none) */
  char                  *asciishows;                   /* Rule show (ASCII) */
  int                    lhsi;                         /* LHS symbol Id */
  int                    separatori;                   /* Eventual separator symbol Id (-1 if none) */
  size_t                 nrhsl;                        /* Number of RHS, 0 in case of a nullable */
  int                   *rhsip;                        /* Array of RHS Ids, NULL in case of a nullable */
  short                 *skipbp;                       /* Skip flags per RHS, first RHS is at indice 0 and so on, NULL in case of a nullable or a sequence */
  int                    exceptioni;                   /* Exception symbol Id (-1 if none) */
  marpaESLIFAction_t    *actionp;                      /* Action */
  char                  *discardEvents;                /* Discard event name - shallowed with its RHS */
  short                  discardEventb;                /* Discard event initial state: 0: off, 1: on */
  int                    ranki;                        /* Rank */
  short                  nullRanksHighb;               /* Null ranks high ? */
  short                  sequenceb;                    /* Sequence ? */
  short                  properb;                      /* Proper ? */
  int                    minimumi;                     /* minimum in case of sequence ? */
  int                    propertyBitSet;               /* C.f. marpaESLIFRulePropertyBit_t */
  short                  hideseparatorb;               /* Separator hiden from arguments ? */
} marpaESLIFRuleProperty_t;

typedef enum marpaESLIFSymbolType {
  MARPAESLIF_SYMBOLTYPE_TERMINAL = 0,
  MARPAESLIF_SYMBOLTYPE_META
} marpaESLIFSymbolType_t;

typedef struct marpaESLIFSymbolProperty {
  marpaESLIFSymbolType_t       type;                   /* Symbol type */
  short                        startb;                 /* Start symbol ? */
  short                        discardb;               /* Discard LHS symbol (i.e. :discard) ? */
  short                        discardRhsb;            /* Discard RHS symbol ? */
  short                        lhsb;                   /* Is an LHS somewhere in its grammar ? */
  short                        topb;                   /* Is a top-level symbol in its grammar - implies lhsb */
  int                          idi;                    /* Marpa ID */
  marpaESLIFString_t          *descp;                  /* Symbol description */
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
  char                        *discardEvents;          /* Discard event name - shallow pointer to a :discard rule's discardEvents */
  short                        discardEventb;          /* Discard event initial state: 0: off, 1: on - copy of :discard's rule value */
  int                          lookupResolvedLeveli;   /* Resolved grammar level */
  int                          priorityi;              /* Symbol priority */
  marpaESLIFAction_t          *nullableActionp;        /* Nullable semantic */
  int                          propertyBitSet;
  int                          eventBitSet;
  marpaESLIFAction_t          *symbolActionp;          /* symbol-action */
  marpaESLIFAction_t          *ifActionp;              /* if-action */
  marpaESLIFAction_t          *generatorActionp;       /* generator-action */
  short                        verboseb;               /* Symbol is verbose */
} marpaESLIFSymbolProperty_t;

/* Whenever marpaESLIF fails to parse exactly a JSON number it will call the proposal callback if defined. */
/* It will always propose either UNDEF or a LONG DOUBLE if available, else a DOUBLE.                       */
/* The user can safely overwrite the proposal that is in the stack.                                        */
/* Note that a signed integer number is always explicitly converted to a signe double 0 with no proposal.  */
/*           +/-Infinity and +/-NaN always goes to a FLOAT with no proposal.                               */
/*           marpaESLIF tries hard to use a non-floating number, calls the proposal in any other case.     */
typedef short (*marpaESLIFJSONProposalAction_t)(void *userDatavp, char *strings, size_t stringl, marpaESLIFValueResult_t *marpaESLIFValueResultp, short confidenceb);

typedef struct marpaESLIFJSONDecodeOption {
  short                                        disallowDupkeysb;                /* Do not allow duplicate key in an object */
  size_t                                       maxDepthl;                       /* Maximum depth - 0 if no maximum */
  short                                        noReplacementCharacterb;         /* No replacement character for invalid UTF-16 surrogates */
  marpaESLIFJSONProposalAction_t               positiveInfinityActionp;         /* +Infinity action */
  marpaESLIFJSONProposalAction_t               negativeInfinityActionp;         /* -Infinity action */
  marpaESLIFJSONProposalAction_t               positiveNanActionp;              /* +Nan action */
  marpaESLIFJSONProposalAction_t               negativeNanActionp;              /* -Nan action */
  marpaESLIFJSONProposalAction_t               numberActionp;                   /* Number action */
} marpaESLIFJSONDecodeOption_t;

typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;  /* Logger. Default: NULL */
} marpaESLIFOption_t;

typedef struct marpaESLIFGrammarOption {
  void   *bytep;               /* Input */
  size_t  bytel;               /* Input length in byte unit */
  char   *encodings;           /* Input encoding. Default: NULL */
  size_t  encodingl;           /* Length of encoding itself. Default: 0 */
} marpaESLIFGrammarOption_t;

/* Opaque types */
typedef struct marpaESLIF        marpaESLIF_t;
typedef struct marpaESLIFGrammar marpaESLIFGrammar_t;

/* Ask the host system to import a marpaESLIFValueResult in the symbol namespace */
typedef short (*marpaESLIFSymbolImport_t)(marpaESLIFSymbol_t *marpaESLIFSymbolp, void *userDatavp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short haveUndefb);

typedef struct marpaESLIFSymbolOption {
  void                     *userDatavp;          /* User specific context */
  marpaESLIFSymbolImport_t  importerp;           /* If end-user want to import a marpaESLIFValueResult */
} marpaESLIFSymbolOption_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIF_EXPORT marpaESLIF_t                 *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
  marpaESLIF_EXPORT short                         marpaESLIF_versionb(marpaESLIF_t *marpaESLIFp, char **versionsp);
  marpaESLIF_EXPORT short                         marpaESLIF_versionMajorb(marpaESLIF_t *marpaESLIFp, int *versionMajorip);
  marpaESLIF_EXPORT short                         marpaESLIF_versionMinorb(marpaESLIF_t *marpaESLIFp, int *versionMinorip);
  marpaESLIF_EXPORT short                         marpaESLIF_versionPatchb(marpaESLIF_t *marpaESLIFp, int *versionPatchip);
  marpaESLIF_EXPORT marpaESLIFOption_t           *marpaESLIF_optionp(marpaESLIF_t *marpaESLIFp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t          *marpaESLIF_grammarp(marpaESLIF_t *marpaESLIFp);
  /* Helper function that tells if a string could be parsed to a number using the non-strict ESLIF's JSON number formalism */
  /* restricted to not special numbers, i.e. /[+-]?(?:[0-9]+)(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?/.                          */
  /* This does NOT handle Infinity nor Nans, see the specific functions later that check on the explicit value.            */
  marpaESLIF_EXPORT short                         marpaESLIF_numberb(marpaESLIF_t *marpaESLIFp, char *s, size_t sizel, marpaESLIFValueResult_t *marpaESLIFValueResultp, short *confidencebp);

  marpaESLIF_EXPORT marpaESLIFGrammar_t          *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp);
  marpaESLIF_EXPORT marpaESLIF_t                 *marpaESLIFGrammar_eslifp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT marpaESLIFGrammarOption_t    *marpaESLIFGrammar_optionp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ngrammarib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *ngrammarip);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_defaultsb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_defaults_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_defaults_setb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_defaults_by_level_setb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFGrammarDefaults_t *marpaESLIFGrammarDefaultsp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *levelip, marpaESLIFString_t **descpp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammar_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int leveli, marpaESLIFString_t *descp, int *levelip, marpaESLIFString_t **descpp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammarproperty_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFGrammarProperty_t *grammarPropertyp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammarproperty_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFGrammarProperty_t *grammarPropertyp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_rulearray_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_rulearray_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ruleproperty_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, marpaESLIFRuleProperty_t *rulePropertyp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ruleproperty_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, marpaESLIFRuleProperty_t *rulePropertyp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ruledisplayform_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ruledisplayform_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_symbolarray_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **symbolipp, size_t *symbollp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_symbolarray_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **symbolipp, size_t *symbollp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_symbolproperty_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, marpaESLIFSymbolProperty_t *marpaESLIFSymbolPropertyp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_symbolproperty_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, marpaESLIFSymbolProperty_t *marpaESLIFSymbolPropertyp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_symboldisplayform_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_symboldisplayform_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ruleshowform_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_ruleshowform_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammarshowform_currentb(marpaESLIFGrammar_t *marpaESLIFGrammarp, char **grammarshowsp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, char **grammarshowsp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammar_t *marpaESLIFGrammarp, char **grammarscriptsp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp);
  marpaESLIF_EXPORT short                         marpaESLIFGrammar_parse_by_levelb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, int leveli, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT void                          marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);

  marpaESLIF_EXPORT marpaESLIFRecognizer_t       *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t       *marpaESLIFRecognizer_newFromp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizer_t *marpaESLIFRecognizerSharedp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_set_exhausted_flagb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short exhaustedb);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_isExhaustedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *exhaustedbp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_isCanContinueb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *isCanContinuebp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_shareb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFRecognizer_t *marpaESLIFRecognizerSharedp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_peekb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFRecognizer_t *marpaESLIFRecognizerPeekedp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t          *marpaESLIFRecognizer_grammarp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT marpaESLIFRecognizerOption_t *marpaESLIFRecognizer_optionp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t deltaLengthl, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFAlternative_t *marpaESLIFAlternativep);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_alternative_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t lengthl);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_alternative_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFAlternative_t *marpaESLIFAlternativep, size_t lengthl);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_name_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *names, short *matchbp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_discardb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *discardlp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_discard_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *matchbp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_name_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nNamelp, char ***namesArraypp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_name_last_pauseb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *names, char **pausesp, size_t *pauselp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_name_last_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *names, char **trysp, size_t *trylp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_discard_last_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char **trysp, size_t *trylp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_discard_lastb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char **lastsp, size_t *lastlp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_isEofb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *eofbp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_isStartCompleteb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completebp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbols, marpaESLIFEventType_t eventSeti, short onoffb);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *eventArraylp, marpaESLIFEvent_t **eventArraypp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int starti, int endi, genericLoggerLevel_t logleveli);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_progressb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int starti, int endi, size_t *progresslp, marpaESLIFRecognizerProgress_t **progresspp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_inputb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char **inputsp, size_t *inputlp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_errorb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_locationb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *linelp, size_t *columnlp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char **inputsp, size_t *inputlp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_last_completedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *names, char **offsetpp, size_t *lengthlp);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_hook_discardb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short discardOnOffb);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_hook_discard_switchb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  /* TAKE CARE: trying to match an external to an existing recognizer will use, in case of a match, the RECOGNIZER's import configuration, not the external symbol */
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_symbol_tryb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFSymbol_t *marpaESLIFSymbolp, short *matchbp);
  marpaESLIF_EXPORT void                          marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

  marpaESLIF_EXPORT marpaESLIFValue_t            *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t       *marpaESLIFValue_recognizerp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT marpaESLIFValueOption_t      *marpaESLIFValue_optionp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                         marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                         marpaESLIFValue_value_startb(marpaESLIFValue_t *marpaESLIFValuep, int *startip);
  marpaESLIF_EXPORT short                         marpaESLIFValue_value_lengthb(marpaESLIFValue_t *marpaESLIFValuep, int *lengthip);
  marpaESLIF_EXPORT short                         marpaESLIFValue_contextb(marpaESLIFValue_t *marpaESLIFValuep, char **symbolsp, int *symbolip, char **rulesp, int *ruleip);
  marpaESLIF_EXPORT void                          marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep);

  /* ------------------------------- */
  /* marpaESLIFValueResult_t helpers */
  /* ------------------------------- */
  marpaESLIF_EXPORT short                         marpaESLIFValue_stack_setb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT marpaESLIFValueResult_t      *marpaESLIFValue_stack_getp(marpaESLIFValue_t *marpaESLIFValuep, int indicei);
  /* marpaESLIFValue_stack_forgetb is like setting a value of type MARPAESLIFVALUE_TYPE_UNDEF saying that memory management is switched off at this specific indice */
  marpaESLIF_EXPORT short                         marpaESLIFValue_stack_forgetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei);
  /* marpaESLIFValue_stack_getAndForgetb transfers the memory management from the stack to the end-user in one call */
  /* It is nothing else but a wrapper on marpaESLIFValue_stack_getp followed by marpaESLIFValue_stack_forgetb */
  marpaESLIF_EXPORT short                         marpaESLIFValue_stack_getAndForgetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT short                         marpaESLIFValue_marpaESLIFValueResult_freeb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp, short deepb);
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_marpaESLIFValueResult_freeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short deepb);

  /* Helpers */
  
  /* -------------------------------------------------------------------------------------------------------------------------------------------------------------- */
  /* Import helpers                                                                                                                                                 */
  /* -------------------------------------------------------------------------------------------------------------------------------------------------------------- */
  /* ROW and TABLE types are flat'idified:                                                                                                                          */
  /* - For a ROW type, all members are imported first in order, then the row marpaESLIFValueResult itself (which contains the number of elements in u.r.sizel)      */
  /*   i.e. value[0], value[1], ... value[value.u.r.sizel - 1], value                                                                                               */
  /* - For a TABLE type, all members are imported first in order, then the array marpaESLIFValueResult itself (which contains the number of elements in u.t.sizel)  */
  /*   i.e. table[0], table[1], ... table[table.u.t.sizel - 1], table                                                                                               */
  /*   Since a table member is nothign else but a row with an even number of elements, it means this can be reinterpreted as:                                       */
  /*        key[0],value[0]...,key[(table.u.t.sizel/2)-1],value[(table.u.t.sizel/2)-1]                                                                              */
  /*                                                                                                                                                                */
  /*   This mean that the importer callback should maintain an internal stack of marpaESLIFValueResult_t items everytime it is called and:                          */
  /*   - push to its stack anything that is not a ROW or a TABLE                                                                                                    */
  /*   - pop from its stack value.u.r.sizel   elements when it receives a ROW callback                                                                              */
  /*   - pop from its stack value.u.t.sizel*2 elements when it receives a TABLE callback                                                                            */
  /*   At the end it must have exactly one element in its internal stack.                                                                                           */
  /* -------------------------------------------------------------------------------------------------------------------------------------------------------------- */
  /* marpaESLIFValue_importb call the end-user importerp() function callback. */
  marpaESLIF_EXPORT short                         marpaESLIFValue_importb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  /* marpaESLIFRecognizer_importb call the end-user importerp() function callback. */
  marpaESLIF_EXPORT short                         marpaESLIFRecognizer_importb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueResult_t *marpaESLIFValueResultp);

  /* Helper routine to guess the encoding of a byte array. If it returns a non-NULL value it is the responsibility of the caller to free it */
  marpaESLIF_EXPORT char                         *marpaESLIF_encodings(marpaESLIF_t *marpaESLIFp, char *bytep, size_t bytel);

  marpaESLIF_EXPORT void                          marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);

  /* --------------------- */
  /* Embedded JSON support */
  /* --------------------- */
  marpaESLIF_EXPORT marpaESLIFGrammar_t          *marpaESLIFJSON_encode_newp(marpaESLIF_t *marpaESLIFp, short strictb);
  /* When encoding, marpaESLIFValueResultp is injected to an internal grammar so, unless the caller sets the shallow flag to a true value */
  /* ESLIF will automatically frees marpaESLIFValueResultp content. */
  marpaESLIF_EXPORT short                         marpaESLIFJSON_encodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFValueResult_t *marpaESLIFValueResultp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t          *marpaESLIFJSON_decode_newp(marpaESLIF_t *marpaESLIFp, short strictb);
  /* For decoding, a marpaESLIFValueOption_t* argument is required. Only the importer callback and its context will be used */
  marpaESLIF_EXPORT short                         marpaESLIFJSON_decodeb(marpaESLIFGrammar_t *marpaESLIFGrammarJSONp, marpaESLIFJSONDecodeOption_t *marpaESLIFJSONDecodeOptionp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);

  /* ------------------------------------- */
  /* Value result helpers                  */
  /* ------------------------------------- */
  /* These mathods return a true value if the underlying float, double or long double hosts an infinity or NaN value. */
  marpaESLIF_EXPORT short                         marpaESLIFValueResult_isinfb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT short                         marpaESLIFValueResult_isnanb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  /* Not all systems represent correctly +/-Nan, *confidencebp will be a true value if marpaESLIF is sure */
  marpaESLIF_EXPORT short                         marpaESLIFValueResult_is_positive_nanb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short *confidencebp);
  marpaESLIF_EXPORT short                         marpaESLIFValueResult_is_negative_nanb(marpaESLIF_t *marpaESLIFp, marpaESLIFValueResult_t *marpaESLIFValueResultp, short *confidencebp);

  /* ------------------------------------- */
  /* Floating point method helpers         */
  /* ------------------------------------- */
  /* Note that the followings are NOT meant to be accurate, they just try to follow the "floating -> string -> floating" safe round-trip */
  /* Returns NULL in case of failure, else an ASCII NUL terminated string that the caller will have to free(). */
  marpaESLIF_EXPORT char                         *marpaESLIF_ftos(marpaESLIF_t *marpaESLIFp, float f);
  marpaESLIF_EXPORT char                         *marpaESLIF_dtos(marpaESLIF_t *marpaESLIFp, double d);
  marpaESLIF_EXPORT char                         *marpaESLIF_ldtos(marpaESLIF_t *marpaESLIFp, long double ld);

  /* ---------------------------------------------- */
  /* Symbols not not bound to any grammar           */
  /* ---------------------------------------------- */
  marpaESLIF_EXPORT marpaESLIFSymbol_t           *marpaESLIFSymbol_string_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFString_t *stringp, char *modifiers, marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);
  marpaESLIF_EXPORT marpaESLIFSymbol_t           *marpaESLIFSymbol_regex_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFString_t *stringp, char *modifiers, marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);
  /* For meta external symbol, the start symbol of the given grammar is duplicated */
  marpaESLIF_EXPORT marpaESLIFSymbol_t           *marpaESLIFSymbol_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, char *symbols, marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);
  marpaESLIF_EXPORT marpaESLIFSymbol_t           *marpaESLIFSymbol_meta_new_by_levelp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, char *symbols, int leveli, marpaESLIFString_t *descp, marpaESLIFSymbolOption_t *marpaESLIFSymbolOptionp);
  marpaESLIF_EXPORT marpaESLIF_t                 *marpaESLIFSymbol_eslifp(marpaESLIFSymbol_t *marpaESLIFSymbolp);
  /* An external symbol can be used directly inside the recognizer phase in the current input stream. The later will automatically expand if needed */
  /* as in normal recognizer lifetime. */
  /* It can be also used outside of any grammar on a free input string */
  /* If there is a match, *matchbp will contain a true value, else a false value */
  /* If there is a match and importer is set, the later is called */
  marpaESLIF_EXPORT short                        marpaESLIFSymbol_tryb(marpaESLIFSymbol_t *marpaESLIFSymbolp, char *inputs, size_t inputl, short *matchbp);
  marpaESLIF_EXPORT void                         marpaESLIFSymbol_freev(marpaESLIFSymbol_t *marpaESLIFSymbolp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H */
