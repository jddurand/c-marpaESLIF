#ifndef MARPAWRAPPER_INTERNAL_RECOGNIZER_H
#define MARPAWRAPPER_INTERNAL_RECOGNIZER_H

#include <stddef.h>
#include "marpaWrapper/recognizer.h"
#include "marpa.h"


struct marpaWrapperRecognizer {
  Marpa_Recognizer               marpaRecognizerp;
  marpaWrapperGrammar_t         *marpaWrapperGrammarp;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;

  /* Storage of symbols for expected terminals */
  size_t                        sizeSymboll;           /* Allocated size */
  size_t                        nSymboll;              /* Used size      */
  int                          *symbolip;
};

#endif /* MARPAWRAPPER_INTERNAL_RECOGNIZER_H */
