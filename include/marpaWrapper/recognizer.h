#ifndef MARPAWRAPPER_RECOGNIZER_H
#define MARPAWRAPPER_RECOGNIZER_H

#include "marpaWrapper/grammar.h"
#include "marpaWrapper/export.h"

/***********************/
/* Opaque object types */
/***********************/
typedef struct marpaWrapperRecognizer marpaWrapperRecognizer_t;

#ifdef __cplusplus
extern "C" {
#endif
  marpaWrapper_EXPORT marpaWrapperRecognizer_t    *marpaWrapperRecognizer_newp(marpaWrapperGrammar_t *marpaWrapperGrammarp);
  marpaWrapper_EXPORT void                         marpaWrapperRecognizer_freev(marpaWrapperRecognizer_t *marpaWrapperRecognizerp);
#ifdef __cplusplus
}
#endif

#endif /* MARPAWRAPPER_RECOGNIZER_H */
