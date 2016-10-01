#ifndef MARPAESLIF_H
#define MARPAESLIF_H

#include <genericLogger.h>
#include <marpaESLIF/export.h>

typedef struct marpaESLIFOption {
  genericLogger_t *genericLoggerp;
} marpaESLIFOption_t;

typedef struct marpaESLIF marpaESLIF_t;
typedef struct marpaESLIFGrammar marpaESLIFGrammar_t;
typedef struct marpaESLIFRecognizer marpaESLIFRecognizer_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaESLIF_EXPORT marpaESLIF_t           *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp);
  marpaESLIF_EXPORT marpaESLIFGrammar_t    *marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, char *grammars);
  marpaESLIF_EXPORT marpaESLIFRecognizer_t *marpaESLIF_recognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT short                  *marpaESLIF_recognizer_readb(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT void                    marpaESLIF_recognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
  marpaESLIF_EXPORT void                    marpaESLIF_grammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp);
  marpaESLIF_EXPORT void                    marpaESLIF_freev(marpaESLIF_t *marpaESLIFp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAESLIF_H*/
