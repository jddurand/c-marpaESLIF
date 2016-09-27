#ifndef MARPAWRAPPER_INTERNAL_ASF_H
#define MARPAWRAPPER_INTERNAL_ASF_H

#include <stddef.h>
#include <genericStack.h>
#include <genericHash.h>
#include "marpaWrapper/asf.h"
#include "marpa.h"

typedef struct marpaWrapperAsfIdset {
  int    idi;
  int    counti;
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
  int             idi;
  genericStack_t *symchesStackp;
  short           visitedb;
  short           registeredb;
} marpaWrapperAsfGlade_t;

typedef int *andNodeStack_t;

typedef struct marpaWrapperAsfOrNode {
  int     nAndNodei;
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

  /* For optimizations, internal generic stacks of methods */
  /* that do not recurse are setted once */
  genericStack_t             *worklistStackp;
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

/* Internal structure used for valuation using the ASF */
typedef struct marpaWrapperAsfValueContext {
  /* Copy of the marpaWrapperAsf_valueb() parameters */
  void                                 *userDatavp;
  marpaWrapperAsfOkSymbolCallback_t     okSymbolCallbackp;
  marpaWrapperAsfOkRuleCallback_t       okRuleCallbackp;
  marpaWrapperValueRuleCallback_t       valueRuleCallbackp;
  marpaWrapperValueSymbolCallback_t     valueSymbolCallbackp;
  marpaWrapperValueNullingCallback_t    valueNullingCallbackp;
  /* Parent rule ID stack */
  genericStack_t                       *parentRuleiStackp;
  /* Current free indice in the output stack */
  int                                   freeOutputStacki;
  /* Current wanted indice in the output stack */
  int                                   wantedOutputStacki;
  /* For logging, keep track of recursivity level */
  int                                   leveli;
} marpaWrapperAsfValueContext_t;

#endif /* MARPAWRAPPER_INTERNAL_ASF_H */
