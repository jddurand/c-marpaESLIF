#ifndef MARPAESLIF_H
#define MARPAESLIF_H

#include <genericLogger.h>
#include <marpaESLIF/export.h>

typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;
} marpaESLIFOption_t;

typedef struct marpaESLIF           marpaESLIF_t;
typedef struct marpaESLIFGrammar    marpaESLIFGrammar_t;
typedef struct marpaESLIFRecognizer marpaESLIFRecognizer_t;

typedef struct marpaESLIFGrammarOption {
  char   *grammarcp;        /* Grammar */
  size_t  grammarl;         /* Grammar length in bytes */
  char   *encodings;        /* Input encoding. Default: NULL */
} marpaESLIFGrammarOption_t;

typedef short (*marpaESLIFReader_t)(void *userDatavp, char **inputcpp, size_t *inputlp, short *eofbp);
typedef struct marpaESLIFRecognizerOption {
  void                *userDatavp;                 /* User specific context */
  marpaESLIFReader_t   marpaESLIFReaderCallbackp;  /* Reader */
  short                disableThresholdb;          /* Default: 0 */
} marpaESLIFRecognizerOption_t;

typedef enum marpaESLIFEventType {
  MARPAESLIF_EVENTTYPE_NONE       = 0x00,
  MARPAESLIF_EVENTTYPE_COMPLETION = 0x01,
  MARPAESLIF_EVENTTYPE_NULLED     = 0x02,
  MARPAESLIF_EVENTTYPE_PREDICTION = 0x04
} marpaESLIFEventType_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOption);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, char *cp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb);
  marpaESLIF_EXPORT short                   marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp);
  marpaESLIF_EXPORT void                    marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT short                   marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp);
  marpaESLIF_EXPORT void                    marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
