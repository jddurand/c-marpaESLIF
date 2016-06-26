#ifndef MARPAWRAPPER_INTERNAL_ASF_H
#define MARPAWRAPPER_INTERNAL_ASF_H

#include <stddef.h>
#include <genericStack.h>
#include <genericHash.h>
#include "marpaWrapper/asf.h"
#include "marpa.h"

typedef struct marpaWrapperAsfIdset {
  int    idi;
  size_t countl;
  int   *idip;
} marpaWrapperAsfIdset_t;

typedef marpaWrapperAsfIdset_t marpaWrapperAsfNidset_t;
typedef marpaWrapperAsfIdset_t marpaWrapperAsfPowerset_t;

typedef enum marpaWrapperAsfIdsete {
  MARPAWRAPPERASFIDSET_NIDSET = 0,
  MARPAWRAPPERASFIDSET_POWERSET,
  _MARPAWRAPPERASFIDSET_IDSETE_MAX
} marpaWrapperAsfIdsete_t;

typedef struct marpaWrapperAsfGlade {
  int   idi;
  size_t nSymboll;
  int   *symbolip;
  short registeredb;
} marpaWrapperAsfGlade_t;

typedef int *andNodeStack_t;

typedef struct marpaWrapperAsfOrNode {
  size_t  nAndNodel;
  int    *andNodep;
} marpaWrapperAsfOrNode_t;

typedef struct marpaWrapperAsfNook {
  int orNodeIdi;
  int firstChoicei;
  int lastChoicei;
  int parentOrNodeIdi;
} marpaWrapperAsfNook_t;

typedef struct marpaWrapperAsfSourceData {
  int sortIxi;
  int sourceNidi;
} marpaWrapperAsfSourceData_t;

struct marpaWrapperAsf {
  genericLogger_t           *genericLoggerp;
  marpaWrapperRecognizer_t  *marpaWrapperRecognizerp;
  marpaWrapperAsfOption_t    marpaWrapperAsfOption;
  Marpa_Bocage               marpaBocagep;
  Marpa_Order                marpaOrderp;

  /* Memoization */
  genericStack_t            *orNodeStackp;
  genericHash_t             *intsetHashp;
  genericStack_t            *nidsetStackp;
  genericStack_t            *powersetStackp;
  genericStack_t            *nookStackp;
  marpaWrapperAsfNidset_t   *lastNidsetp;
  marpaWrapperAsfPowerset_t *lastPowersetp;
  genericStack_t            *gladeStackp;

  /* Memoization of choices */
  int                        nextIntseti;

  /* Current choice point */
  genericStack_t           *factoringStackp;
  genericStack_t           *orNodeInUseStackp;
};

#endif /* MARPAWRAPPER_INTERNAL_ASF_H */
