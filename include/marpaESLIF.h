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
  char   *grammars;         /* Grammar */
  size_t  grammarl;         /* Grammar length in bytes */
  char   *encodings;        /* Input encoding. Default: NULL */
} marpaESLIFGrammarOption_t;

/* The reader can return encoding information, giving eventual encoding of this information in encodingOfEncodingsp, starting at *encodingsp, spreaded over *encodinglp bytes */
typedef short (*marpaESLIFReader_t)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
typedef struct marpaESLIFRecognizerOption {
  void                *userDatavp;                  /* User specific context */
  marpaESLIFReader_t   marpaESLIFReaderCallbackp;   /* Reader */
  short                disableThresholdb;           /* Default: 0 */
  short                exhaustedb;                  /* Exhaustion event. Default: 0 */
  short                latmb;                       /* Longest acceptable token match mode. Default: 0 */
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
  marpaESLIFString_t   *stringp; /* Pointer to event name as per the user - NULL if exhaustion */
} marpaESLIFEvent_t;

typedef short (*marpaESLIFValueRuleCallback_t)(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
typedef short (*marpaESLIFValueSymbolCallback_t)(void *userDatavp, char *bytep, size_t bytel, int symboli, int resulti);
typedef short (*marpaESLIFValueNullingCallback_t)(void *userDatavp, int symboli, int resulti);
typedef struct marpaESLIFValueOption {
  void                             *userDatavp;            /* User specific context */
  marpaESLIFValueRuleCallback_t     ruleCallbackp;
  marpaESLIFValueSymbolCallback_t   symbolCallbackp;
  marpaESLIFValueNullingCallback_t  nullingCallbackp;
  short                             highRankOnlyb;         /* Default: 1 */
  short                             orderByRankb;          /* Default: 1 */
  short                             ambiguousb;            /* Default: 0 */
  short                             nullb;                 /* Default: 0 */
  int                               maxParsesi;            /* Default: 0 */
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

  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_startib(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_starti_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int *startip, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rulesb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rules_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int **ruleipp, size_t *rulelp, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rulenameb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledecsp, size_t *ruledesclp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_rulename_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledecsp, size_t *ruledesclp, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruledisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruledisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruledisplaysp, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruleshowformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_ruleshowform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int rulei, char **ruleshowsp, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symbolnameb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldecsp, size_t *symboldesclp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symbolname_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldecsp, size_t *symboldesclp, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symboldisplayformb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_symboldisplayform_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int symboli, char **symboldisplaysp, char *grammardescs, size_t grammardescl, int grammarleveli);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parse_by_grammarb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp, char *descs, size_t descl, int leveli);
  marpaESLIF_EXPORT void                    marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);

  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOption);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFRecognizer_grammarp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbolnamecp, size_t symbolnamel, int valuei);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *stringArraylp, marpaESLIFEvent_t **eventArraypp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int starti, int endi, genericLoggerLevel_t logleveli, void *userDatavp, marpaESLISymbolDescriptionCallback_t symbolDescriptionCallbackp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

  marpaESLIF_EXPORT marpaESLIFValue_t      *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFValue_recognizerp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT short                   marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT void                    marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep);

  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
