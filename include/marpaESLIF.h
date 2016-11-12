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
  char   *grammars;            /* Grammar */
  size_t  grammarl;            /* Grammar length in bytes */
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
  short                rejectionb;                  /* Rejection event. Default: 0 */
  short                newlineb;                    /* Count line/column numbers. Default: 0 */
  size_t               bufsizl;                     /* Minimum stream buffer size: Default: MARPAESLIF_BUFSIZ */
  unsigned int         buftriggerperci;             /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Default: 50 */
  unsigned int         bufaddperci;                 /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Default: 50 */
} marpaESLIFRecognizerOption_t;

typedef enum marpaESLIFEventType {
  MARPAESLIF_EVENTTYPE_NONE       = 0x00,
  MARPAESLIF_EVENTTYPE_COMPLETED  = 0x01, /* Grammar event */
  MARPAESLIF_EVENTTYPE_NULLED     = 0x02, /* Grammar event */
  MARPAESLIF_EVENTTYPE_PREDICTED  = 0x04, /* Grammar event */
  MARPAESLIF_EVENTTYPE_BEFORE     = 0x08, /* Just before lexeme is commited */
  MARPAESLIF_EVENTTYPE_AFTER      = 0x10, /* Just after lexeme is commited */
  MARPAESLIF_EVENTTYPE_EXHAUSTED  = 0x20, /* Exhaustion */
  MARPAESLIF_EVENTTYPE_REJECTED   = 0x40  /* Rejection */
} marpaESLIFEventType_t;

typedef struct marpaESLIFEvent {
  marpaESLIFEventType_t type;
  char                 *events; /* Event name - NULL if exhaustion */
} marpaESLIFEvent_t;

typedef int   (*marpaESLIFValueRuleCallback_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti);
typedef int   (*marpaESLIFValueSymbolCallback_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
typedef void  (*marpaESLIFValueFreeCallback_t)(void *userDatavp, marpaESLIFGrammar_t *marpaESLIFGrammarp, int contexti, void *p);

typedef marpaESLIFValueRuleCallback_t (*marpaESLIFValueRuleActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
typedef marpaESLIFValueSymbolCallback_t (*marpaESLIFValueSymbolActionResolver_t)(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);

/* Stack types */
typedef enum marpaESLIFStackType {
  MARPAESLIF_STACK_TYPE_NA = 0,
  MARPAESLIF_STACK_TYPE_CHAR,
  MARPAESLIF_STACK_TYPE_SHORT,
  MARPAESLIF_STACK_TYPE_INT,
  MARPAESLIF_STACK_TYPE_LONG,
  MARPAESLIF_STACK_TYPE_FLOAT,
  MARPAESLIF_STACK_TYPE_DOUBLE,
  MARPAESLIF_STACK_TYPE_PTR,
  MARPAESLIF_STACK_TYPE_PTR_SHALLOW,
  MARPAESLIF_STACK_TYPE_ARRAY,
  MARPAESLIF_STACK_TYPE_ARRAY_SHALLOW,
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
  marpaESLIFValueFreeCallback_t         freeCallbackp;         /* Will be called when memory is to be released */
  short                                 highRankOnlyb;         /* Default: 1 */
  short                                 orderByRankb;          /* Default: 1 */
  short                                 ambiguousb;            /* Default: 0 */
  short                                 nullb;                 /* Default: 0 */
  int                                   maxParsesi;            /* Default: 0 */
} marpaESLIFValueOption_t;

typedef char *(*marpaESLISymbolDescriptionCallback_t)(void *userDatavp, int symboli);
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
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rulesb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rules_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruledisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruleshowformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symboldisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp, int grammari, marpaESLIFString_t *descp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, int leveli, marpaESLIFString_t *descp, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT void                    marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);

  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOption);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFRecognizer_grammarp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *asciis, int valuei);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_eofb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *stringArraylp, marpaESLIFEvent_t **eventArraypp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int starti, int endi, genericLoggerLevel_t logleveli, void *userDatavp, marpaESLISymbolDescriptionCallback_t symbolDescriptionCallbackp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

  marpaESLIF_EXPORT marpaESLIFValue_t      *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFValue_recognizerp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                   marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFValueResult_t *marpaESLIFValueResultp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_value_startb(marpaESLIFValue_t *marpaESLIFValuep, int *startip);
  marpaESLIF_EXPORT short                   marpaESLIFValue_value_lengthb(marpaESLIFValue_t *marpaESLIFValuep, int *lengthip);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                   marpaESLIFValue_grammarib(marpaESLIFValue_t *marpaESLIFValuep, int *grammarip);
  marpaESLIF_EXPORT void                    marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep);

  /* Stack management when doing valuation */
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_char(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, char c);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_short(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, short b);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_int(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, int i);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_long(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, long l);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_float(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, float f);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_double(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, double d);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_ptr(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, short shallowb);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_set_array(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int contexti, void *p, size_t l, short shallowb);

  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_char(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, char *cp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_short(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, short *bp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_int(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, int *ip);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_long(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, long *lp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_float(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, float *fp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_double(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, double *dp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_ptr(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, short *shallowbp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_stack_get_array(marpaESLIFValue_t *marpaESLIFValuep, int indicei, int *contextip, void **pp, size_t *lp, short *shallowbp);

  /* Stack context when doing valuation */
  marpaESLIF_EXPORT short                   marpaESLIFValue_context_symboli(marpaESLIFValue_t *marpaESLIFValuep, int *symbolip);
  marpaESLIF_EXPORT short                   marpaESLIFValue_context_rulei(marpaESLIFValue_t *marpaESLIFValuep, int *ruleip);


  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);

  marpaESLIF_EXPORT short                   marpaESLIF_generateHelperb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, char *userspaceincludes, char *freeMethodNames, char *copyMethodNames, char **helpersp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
