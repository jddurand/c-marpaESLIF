#ifndef MARPAWRAPPER_INTERNAL_ASF_H
#define MARPAWRAPPER_INTERNAL_ASF_H

#include <stddef.h>
#include <genericStack.h>
#include <genericHash.h>
#include "marpaWrapper/asf.h"
#include "marpa.h"

typedef struct marpaWrapperAsfNidset {
  int    id;
  size_t nIdl;
  int   *idip;
} marpaWrapperAsfNidset_t;

struct marpaWrapperAsf {
  genericLogger_t          *genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp;
  marpaWrapperAsfOption_t   marpaWrapperAsfOption;
  Marpa_Bocage              marpaBocagep;
  Marpa_Order               marpaOrderp;

  /* Temporary Storage of AND Node Ids */
  size_t                        sizeAndNodeIdl;     /* Allocated size */
  size_t                        nAndNodeIdl;        /* Used size      */
  int                          *andNodeIdip;

  /* Storage of OR Node Ids */
  size_t                        sizeOrNodeAndNodeIdl;     /* Allocated size */
  size_t                        nOrNodeAndNodeIdl;        /* Used size      */
  int                         **orNodeAndNodeIdipp;

  /* Memoization of visited glades */
  genericHash_t                *intsetHashp;
  genericStack_t               *nidsetStackp;

  int                           nextIntseti;

};

#endif /* MARPAWRAPPER_INTERNAL_ASF_H */
