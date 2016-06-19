#ifndef MARPAWRAPPER_INTERNAL_ASF_H
#define MARPAWRAPPER_INTERNAL_ASF_H

#include <stddef.h>
#include "marpaWrapper/asf.h"
#include "marpa.h"

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

};

#endif /* MARPAWRAPPER_INTERNAL_ASF_H */
