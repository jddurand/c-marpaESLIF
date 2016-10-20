#ifndef MARPAESLIF_H
#define MARPAESLIF_H

#include <genericLogger.h>
#include <genericStack.h>
#include <marpaESLIF/export.h>

typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;
} marpaESLIFOption_t;

typedef struct marpaESLIF           marpaESLIF_t;
typedef struct marpaESLIFGrammar    marpaESLIFGrammar_t;
typedef struct marpaESLIFRecognizer marpaESLIFRecognizer_t;
typedef struct marpaESLIFValue      marpaESLIFValue_t;

typedef struct marpaESLIFGrammarOption {
  char   *grammars;         /* Grammar */
  size_t  grammarl;         /* Grammar length in bytes */
  char   *encodings;        /* Input encoding. Default: NULL */
} marpaESLIFGrammarOption_t;

typedef short (*marpaESLIFReader_t)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp);
typedef struct marpaESLIFRecognizerOption {
  void                *userDatavp;                 /* User specific context */
  marpaESLIFReader_t   marpaESLIFReaderCallbackp;  /* Reader */
  short                disableThresholdb;          /* Default: 0 */
  short                exhaustedb;                 /* Exhaustion event. Default: 0 */
  short                latmb;                      /* Longest acceptable token match mode. Default: 0 */
  short                rejectionb;                 /* Rejection event. Default: 0 */
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

typedef struct marpaESLIFString {
  marpaESLIFEventType_t type;
  char                 *events;                     /* Point to event name in user's encoding. NULL if exhaustion */
  size_t                eventl;                     /* Length in bytes                        */
} marpaESLIFString_t;

/* A custom user action is getting a generic stack in input and returns a generic stack  */
/* as well, with a pointer to an eventual free method. This free method will have to     */
/* free the CONTENT of the stack an the stack itself (with GENERICSTACK_FREE()).         */
/* In the absence of a free method, marpaESLIF will call GENERICSTACK_FREE(), leaving to */
/* potential memory leaks if the stack returned by the action contains allocated things. */
typedef struct marpaESLIFActionValue {
  genericStack_t stack;                                                    /* Generic stack */
  void          *userDatavp;                                               /* User specific context, used only if freeCallbackp is != NULL */
  void         (*freeCallbackp)(void *userDatavp, genericStack_t *stackp); /* Eventual free method - stackp is a pointer on this structure's stack member  */
} marpaESLIFActionValue_t;
typedef short (*marpaESLIFActionCallback_t)(void *userDatavp, char *names, size_t namel, genericStack_t *actionValueInputStackp, genericStack_t **actionValueOutputStackpp);

typedef struct marpaESLIFValueOption {
  void                      *userDatavp;            /* User specific context */
  marpaESLIFActionCallback_t valueCallbackp;        /* User's value callback */
  short                      highRankOnlyb;         /* Default: 1 */
  short                      orderByRankb;          /* Default: 1 */
  short                      ambiguousb;            /* Default: 0 */
  short                      nullb;                 /* Default: 0 */
  int                        maxParsesi;            /* Default: 0 */
} marpaESLIFValueOption_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOption);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbolnamecp, size_t symbolnamel, marpaESLIFActionValue_t *marpaESLIFActionValuep);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *stringArraylp, marpaESLIFString_t **stringArraypp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp);
  marpaESLIF_EXPORT void                    marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT marpaESLIFValue_t      *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp);
  marpaESLIF_EXPORT short                   marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFActionValue_t *marpaESLIFActionValuep);
  marpaESLIF_EXPORT int                     marpaESLIFValue_rulei(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT void                    marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep);
  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
