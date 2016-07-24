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

struct marpaWrapperAsfGlade {
  int             idi;
  genericStack_t *symchesStackp;
  short           visitedb;
  short           registeredb;
};

typedef int *andNodeStack_t;

typedef struct marpaWrapperAsfOrNode {
  size_t  nAndNodel;
  int    *andNodep;
} marpaWrapperAsfOrNode_t;

typedef struct marpaWrapperAsfNook {
  int   orNodeIdi;
  int   firstChoicei;
  int   lastChoicei;
  int   parentOrNodeIdi;
  short isCauseb;
  short isPredecessorb;
  short causeIsExpandedb;
  short predecessorIsExpandedb;
} marpaWrapperAsfNook_t;

typedef struct marpaWrapperAsfSourceData {
  int sortIxi;
  int sourceNidi;
} marpaWrapperAsfSourceData_t;

struct marpaWrapperAsf {
  marpaWrapperRecognizer_t    *marpaWrapperRecognizerp;
  marpaWrapperAsfOption_t      marpaWrapperAsfOption;
  Marpa_Bocage                 marpaBocagep;
  Marpa_Order                  marpaOrderp;

  /* Memoization */
  genericStack_t              *orNodeStackp;
  genericHash_t               *intsetHashp;
  genericStack_t              *nidsetStackp;
  genericStack_t              *powersetStackp;
  genericStack_t              *gladeStackp;

  /* Memoization of choices */
  int                         nextIntseti;

  /* Traverser callback */
  traverserCallback_t         traverserCallbackp;
  void                       *userDatavp;

  /* To avoid user to free the list of traverse values */
  genericStack_t             *lastValuesStackp;
  genericStack_t             *lastAllChoicesStackp;
};

typedef struct marpaWrapperAsfChoicePoint {
  genericStack_t         *factoringStackp;
  genericStack_t         *orNodeInUseStackp;
} marpaWrapperAsfChoicePoint_t;

struct marpaWrapperAsfTraverser {
  marpaWrapperAsf_t      *marpaWrapperAsfp;
  genericStack_t         *valueStackp;
  marpaWrapperAsfGlade_t *gladep;
  int                     symchIxi;
  int                     factoringIxi;
};

#endif /* MARPAWRAPPER_INTERNAL_ASF_H */
