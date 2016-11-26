#ifndef MARPAESLIF_H
#define MARPAESLIF_H

#include <genericLogger.h>
#include <marpaESLIF/export.h>

typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;  /* Logger. Default: NULL */
} marpaESLIFOption_t;

typedef struct marpaESLIF           marpaESLIF_t;
typedef struct marpaESLIFGrammar    marpaESLIFGrammar_t;
typedef struct marpaESLIFRecognizer marpaESLIFRecognizer_t;
typedef struct marpaESLIFValue      marpaESLIFValue_t;
typedef struct marpaESLIFSymbol     marpaESLIFSymbol_t;

/* A string */
typedef struct marpaESLIFString {
  char   *bytep;            /* pointer bytes */
  size_t  bytel;            /* number of bytes */
  char   *encodingasciis;   /* Encoding of previous bytes, itself being writen in ASCII encoding, NUL byte terminated */
  char   *asciis;           /* ASCII (un-translatable bytes are changed to a replacement character) translation of previous bytes, NUL byte terminated - never NULL if bytep is not NULL */
  /*
   * Remark: the encodings and asciis pointers are not NULL only when ESLIF know that the buffer is associated to a "description". I.e.
   * this is happening ONLY when parsing the grammar. Raw data never have non-NULL asciis or encodings.
   */
} marpaESLIFString_t;

typedef struct marpaESLIFGrammarOption {
  void   *bytep;               /* Input */
  size_t  bytel;               /* Input length in byte unit */
  char   *encodings;           /* Input encoding. Default: NULL */
  size_t  encodingl;           /* Length of encoding itself. Default: 0 */
  char   *encodingOfEncodings; /* Encoding of encoding, in ASCII encoding. Default: NULL. */
} marpaESLIFGrammarOption_t;

/* The reader can return encoding information, giving eventual encoding of this information in encodingOfEncodingsp, starting at *encodingsp, spreaded over *encodinglp bytes */
typedef short (*marpaESLIFReader_t)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
typedef struct marpaESLIFRecognizerOption {
  void                *userDatavp;                  /* User specific context */
  marpaESLIFReader_t   marpaESLIFReaderCallbackp;   /* Reader */
  short                disableThresholdb;           /* Default: 0 */
  short                exhaustedb;                  /* Exhaustion event. Default: 0 */
  short                newlineb;                    /* Count line/column numbers. Default: 0 */
  size_t               bufsizl;                     /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  unsigned int         buftriggerperci;             /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  unsigned int         bufaddperci;                 /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
} marpaESLIFRecognizerOption_t;

typedef enum marpaESLIFEventType {
  MARPAESLIF_EVENTTYPE_NONE       = 0x00,
  MARPAESLIF_EVENTTYPE_COMPLETED  = 0x01, /* Grammar event */
  MARPAESLIF_EVENTTYPE_NULLED     = 0x02, /* Grammar event */
  MARPAESLIF_EVENTTYPE_PREDICTED  = 0x04, /* Grammar event */
  MARPAESLIF_EVENTTYPE_BEFORE     = 0x08, /* Just before lexeme is commited */
  MARPAESLIF_EVENTTYPE_AFTER      = 0x10, /* Just after lexeme is commited */
  MARPAESLIF_EVENTTYPE_EXHAUSTED  = 0x20, /* Exhaustion */
  MARPAESLIF_EVENTTYPE_DISCARD    = 0x40  /* Discard */
} marpaESLIFEventType_t;

typedef struct marpaESLIFEvent {
  marpaESLIFEventType_t type;
  char                 *symbols; /* Symbol name, always NULL if exhausted event, always ':discard' if discard event */
  char                 *events;  /* Event name, always NULL if exhaustion eent */
} marpaESLIFEvent_t;

typedef short (*marpaESLIFValueRuleCallback_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
typedef short (*marpaESLIFValueSymbolCallback_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
typedef void  (*marpaESLIFValueFreeCallback_t)(void *userDatavp, int contexti, void *p, size_t sizel);

typedef marpaESLIFValueRuleCallback_t   (*marpaESLIFValueRuleActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
typedef marpaESLIFValueSymbolCallback_t (*marpaESLIFValueSymbolActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
typedef marpaESLIFValueFreeCallback_t   (*marpaESLIFValueFreeActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);

/* Stack types */
typedef enum marpaESLIFStackType {
  MARPAESLIF_STACK_TYPE_UNDEF = 0,
  MARPAESLIF_STACK_TYPE_CHAR,
  MARPAESLIF_STACK_TYPE_SHORT,
  MARPAESLIF_STACK_TYPE_INT,
  MARPAESLIF_STACK_TYPE_LONG,
  MARPAESLIF_STACK_TYPE_FLOAT,
  MARPAESLIF_STACK_TYPE_DOUBLE,
  MARPAESLIF_STACK_TYPE_PTR,
  MARPAESLIF_STACK_TYPE_PTR_SHALLOW,
  MARPAESLIF_STACK_TYPE_ARRAY,
  MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW
} marpaESLIFStackType_t;

/* Valuation result */
typedef struct marpaESLIFValueResult {
  marpaESLIFStackType_t type;
  union {
    char c;          /* Value is a char */
    short b;         /* Value is a short */
    int i;           /* Value is an int */
    long l;          /* Value is a long */
    float f;         /* Value is a float */
    double d;        /* Value is a double */
    void *p;         /* Value is a pointer */
  } u;
  int contexti;      /* Free value meaninful to the user */
  size_t sizel;      /* Length of data in case value is an ARRAY - always 0 otherwise */
} marpaESLIFValueResult_t;

typedef struct marpaESLIFValueOption {
  void                                 *userDatavp;            /* User specific context */
  marpaESLIFValueRuleActionResolver_t   ruleActionResolverp;   /* Will return the function doing the wanted rule action */
  marpaESLIFValueSymbolActionResolver_t symbolActionResolverp; /* Will return the function doing the wanted symbol action */
  marpaESLIFValueFreeActionResolver_t   freeActionResolverp;   /* Will return the function doing the free */
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

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIF_grammarp(marpaESLIF_t *marpaESLIFp);

  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp);
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIFGrammar_eslifp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ngrammari(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *ngrammarip);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_startib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_starti_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_levelib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *levelip);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *levelip, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rulearrayb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rulearray_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruledisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_grammarshowformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, char **grammarshowsp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_grammarshowform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, char **grammarshowsp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruleshowformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symboldisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, int leveli, marpaESLIFString_t *descp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT void                    marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);

  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp);
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIFRecognizer_eslifp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFRecognizer_grammarp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_alternative_lengthb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t alternativeLengthl);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbols);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_eofb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *eventArraylp, marpaESLIFEvent_t **eventArraypp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int starti, int endi, genericLoggerLevel_t logleveli);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_inputv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char **inputsp, size_t *inputlp, short *eofbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_readb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char **inputsp, size_t *inputlp, short *eofbp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

  marpaESLIF_EXPORT marpaESLIFValue_t      *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIFValue_eslifp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFValue_recognizerp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                   marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_value_startb(marpaESLIFValue_t *marpaESLIFValuep, int *startip);
  marpaESLIF_EXPORT short                   marpaESLIFValue_value_lengthb(marpaESLIFValue_t *marpaESLIFValuep, int *lengthip);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                   marpaESLIFValue_grammarib(marpaESLIFValue_t *marpaESLIFValuep, int *grammarip);
  marpaESLIF_EXPORT void                    marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep);

  /* Stack management when doing valuation */
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, char c);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, short b);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, int i);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, long l);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, float f);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, double d);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, short shallowb);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, size_t l, short shallowb);

  /* forget is like undef except that memory management is switched off - a possible use for it is when a PTR in the stack is stored into another structure, the later pushed in the result stack */
  /* For example X ::= Y, where Y is a PTR with value yp, and X is a PTR to a struct with value xp and content { void *yp } : xp and yp are not the same, though xp contain yp. */
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_forgetb(marpaESLIFValue_t *marpaESLIFValuep, int indicei);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_contextb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip);

  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_undefb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *undefbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *charbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *shortbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *intbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *longbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *floatbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *doublebp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *ptrbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_is_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, short *arraybp);

  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, char *cp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, short *bp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, int *ip);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, long *lp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, float *fp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, double *dp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, short *shallowbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, size_t *lp, short *shallowbp);

  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_charb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, char *cp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_shortb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, short *bp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_intb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, int *ip);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_longb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, long *lp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_floatb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, float *fp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_doubleb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, double *dp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_ptrb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, short *shallowbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_getAndForget_arrayb(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, size_t *lp, short *shallowbp);

  /* Stack context when doing valuation */
  marpaESLIF_EXPORT short                   marpaESLIFValue_context_symbolsb(marpaESLIFValue_t *marpaESLIFValuep, char **symbolsp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_context_rulesb(marpaESLIFValue_t *marpaESLIFValuep, char **rulesp);


  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
