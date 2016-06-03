#ifndef MARPAWRAPPER_RECOGNIZER_H
#define MARPAWRAPPER_RECOGNIZER_H

#include "marpaWrapper/grammar.h"
#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperRecognizer marpaWrapperRecognizer_t;

/* --------------- */
/* General options */
/* --------------- */
typedef struct marpaWrapperRecognizerOption {
  genericLogger_t *genericLoggerp;             /* Default: NULL. */
  short            latmb;                      /* Default: 1.    */
} marpaWrapperRecognizerOption_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperRecognizer_t    *marpaWrapperRecognizer_newp(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperRecognizerOption_t *marpaWrapperRecognizerOptionp);
  marpaWrapper_EXPORT short                        marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, int valuei, int lengthi);
  marpaWrapper_EXPORT short                        marpaWrapperRecognizer_completeb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp);
  marpaWrapper_EXPORT short                        marpaWrapperRecognizer_readb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, int valuei, int lengthi);
  marpaWrapper_EXPORT void                         marpaWrapperRecognizer_freev(marpaWrapperRecognizer_t *marpaWrapperRecognizerp);
  marpaWrapper_EXPORT short                        marpaWrapperRecognizer_event_onoffb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, marpaWrapperGrammarEventType_t eventSeti, int onoffb);
  marpaWrapper_EXPORT size_t                       marpaWrapperRecognizer_expectedb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int **symbolArraypp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_RECOGNIZER_H */
