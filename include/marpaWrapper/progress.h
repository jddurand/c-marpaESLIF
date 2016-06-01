#ifndef MARPAWRAPPER_PROGRESS
#define MARPAWRAPPER_PROGRESS

#include "marpaWrapper/recognizer.h"

typedef struct marpaWrapperProgress {
  int                 earleySetIdi;
  int                 earleySetOriginIdi;
  marpaWrapperGrammarRule_t *marpaWrapperGrammarRulep;
  int                 positioni;
} marpaWrapperProgress_t;

#endif /* MARPAWRAPPER_PROGRESS */
