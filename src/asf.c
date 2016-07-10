#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/asf.h"
#include "marpaWrapper/internal/recognizer.h"
#include "marpaWrapper/internal/grammar.h"
#include "marpaWrapper/internal/logging.h"

#ifndef MARPAWRAPPERASF_INTSET_MAXROWS
#define MARPAWRAPPERASF_INTSET_MAXROWS 65536
#endif

#undef MARPAWRAPPERASF_INTSET_MODULO
#define MARPAWRAPPERASF_INTSET_MODULO (MARPAWRAPPERASF_INTSET_MAXROWS - 1)

#ifndef MARPAWRAPPERASF_FACTORING_MAX
#define MARPAWRAPPERASF_FACTORING_MAX 42
#endif

#ifndef MARPAWRAPPERASF_NID_LEAF_BASE
#define MARPAWRAPPERASF_NID_LEAF_BASE (-MARPAWRAPPERASF_FACTORING_MAX - 1)
#endif

#ifndef MARPAWRAPPERASF_NODE_HASH_SIZE
#define MARPAWRAPPERASF_NODE_HASH_SIZE 2048
#endif

#ifdef C_VA_COPY
/* Used when passing a va_list on the stack to another function taking a va_list */
#define REAL_AP ap2
#else
#define REAL_AP ap
#endif

static marpaWrapperAsfOption_t marpaWrapperAsfOptionDefault = {
  NULL,   /* genericLoggerp */
   1,     /* highRankOnlyb */
   1,     /* orderByRankb */
};

static char *marpaWrapperAsfIdsets[_MARPAWRAPPERASFIDSET_IDSETE_MAX] = {
  "nidset",
  "powerset"
};

static char *marpaWrapperAsfIdsetUpfirsts[_MARPAWRAPPERASFIDSET_IDSETE_MAX] = {
  "Nidset",
  "Powerset"
};

typedef struct marpaWrapperAfsAndNodeIdAndPredecessorId {
  Marpa_And_Node_ID andNodeIdi;
  Marpa_And_Node_ID andNodePredecessorIdi;
} marpaWrapperAfsAndNodeIdAndPredecessorId_t;

static inline void                       _marpaWrapperAsf_lastAllChoices_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_orNodeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_gladeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *symchesStackp);
static inline void                       _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *factoringsStackp);
static inline short                      _marpaWrapperAsf_factoringStackp_resetb(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline short                      _marpaWrapperAsf_peakb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip);
static inline int                        _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi(const void *p1, const void *p2);
static inline short                      _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, size_t countl, int *idip);
static inline char                      *_marpaWrapperAsf_stringBuilders(char *fmts, ...);
static inline char                      *_marpaWrapperAsf_stringBuilder_aps(char *fmts, va_list ap);
static inline short                      _marpaWrapperAsf_cmpIntsetFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);
static inline unsigned long              _marpaWrapperAsf_djb2(unsigned char *str);
static inline int                        _marpaWrapperAsf_idCmpi(const void *p1, const void *p2);

static inline marpaWrapperAsfIdset_t    *_marpaWrapperAsf_idset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, size_t countl, int *idip);
static inline int                       *_marpaWrapperAsf_idset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline short                      _marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip);
static inline size_t                     _marpaWrapperAsf_idset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_idset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline char                      *_marpaWrapperAsf_idset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_idset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete);

static inline marpaWrapperAsfNidset_t   *_marpaWrapperAsf_nidset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t countl, int *idip);
static inline int                       *_marpaWrapperAsf_nidset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline short                      _marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip);
static inline size_t                     _marpaWrapperAsf_nidset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_nidset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline char                      *_marpaWrapperAsf_nidset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_nidset_freev(marpaWrapperAsf_t *marpaWrapperAsfp);

/* Generic nidset/powerset methods */

static inline marpaWrapperAsfPowerset_t *_marpaWrapperAsf_powerset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t countl, int *idip);
static inline int                       *_marpaWrapperAsf_powerset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline short                      _marpaWrapperAsf_powerset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip);
static inline size_t                     _marpaWrapperAsf_powerset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_powerset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline char                      *_marpaWrapperAsf_powerset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_powerset_freev(marpaWrapperAsf_t *marpaWrapperAsfp);

/* Specific to nid */
static inline int                        _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
static inline int                        _marpaWrapperAsf_and_node_to_nidi(int idi);
static inline int                        _marpaWrapperAsf_nid_to_and_nodei(int idi);
static inline int                        _marpaWrapperAsf_sourceDataCmpi(const void *p1, const void *p2);
static inline int                        _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);

/* Specific to powerset */
static inline marpaWrapperAsfNidset_t   *_marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfPowerset_t *powersetp, int ixi);
static inline marpaWrapperAsfGlade_t    *_marpaWrapperAsf_glade_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, size_t gladei);
static inline short                      _marpaWrapperAsf_first_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, int nidOfChoicePointi, short *firstFactoringbp);
static inline short                      _marpaWrapperAsf_factoring_finishb(marpaWrapperAsf_t *marpaWrapperAsfp, int nidOfChoicePointi, short *factoringFinishbp);
static inline short                      _marpaWrapperAsf_factoring_iterateb(marpaWrapperAsf_t *marpaWrapperAsfp, short *factoringIteratebp);
static inline short _marpaWrapperAsf_next_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, int nidOfChoicePointi, short *factoringbp);

/* Specific to glade */
static inline short                      _marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t **stackpp);
size_t                                   _marpaWrapperAsf_indAndNodesl(void *userDatavp, genericStackItemType_t itemType, void *p);
static inline short                      _marpaWrapperAsf_cmpAndNodesb(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);
static inline short                      _marpaWrapperAsf_and_nodes_to_cause_nidsp(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *andNodeIdStackp, genericStack_t **causeNidsStackpp);
static inline short                      _marpaWrapperAsf_glade_is_visitedb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi);
static inline void                       _marpaWrapperAsf_glade_visited_clearb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip);
static inline short                      _marpaWrapperAsf_glade_symch_countb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, size_t *countlp);
static inline short                      _marpaWrapperAsf_glade_symbol_idb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, int *symbolIdip);

/* Specific to nook */
static inline marpaWrapperAsfNook_t     *_marpaWrapperAsf_nook_newp(marpaWrapperAsf_t *marpaWrapperAsfp, int orNodeIdi, int parentOrNodeIdi);
static inline short                      _marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp);
static inline short                      _marpaWrapperAsf_setLastChoiceb(marpaWrapperAsf_t *marpaWrapperAsfp, int **choiceipp, marpaWrapperAsfNook_t *nookp);
static inline short                      _marpaWrapperAsf_nook_incrementb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp);


/****************************************************************************/
marpaWrapperAsf_t *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp)
/****************************************************************************/
{
  const static char                           funcs[]                    = "marpaWrapperAsf_newp";
  marpaWrapperAsf_t                          *marpaWrapperAsfp           = NULL;
  marpaWrapperAsfOrNode_t                    *orNodep                    = NULL;
  marpaWrapperAfsAndNodeIdAndPredecessorId_t *andNodeIdAndPredecessorIdp = NULL;
  genericLogger_t                            *genericLoggerp             = NULL;
  Marpa_Earley_Set_ID                         marpaLatestEarleySetIdi;
  int                                         highRankOnlyFlagi;
  int                                         ambiguousi;
  int                                         nulli;
  int                                         andNodeCounti;
  Marpa_Or_Node_ID                            orNodei;
  int                                         ix;
  Marpa_And_Node_ID                           andNodeIdi;
  int                                        *andNodep;
  size_t                                      sizeAndNodeIdAndPredecessorIdl = 0;
  size_t                                      nAndNodeIdAndPredecessorIdl = 0;
  Marpa_And_Node_ID                           andNodePredecessorIdi;
  
  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  /* Impossible if we are already valuating it */
  if (marpaWrapperRecognizerp->treeModeb != MARPAWRAPPERRECOGNIZERTREEMODE_NA) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Already in valuation mode");
    goto err;
  }

  if (marpaWrapperAsfOptionp == NULL) {
    marpaWrapperAsfOptionp = &marpaWrapperAsfOptionDefault;
  }
  genericLoggerp = marpaWrapperAsfOptionp->genericLoggerp;

  /* Create an asf instance */
  marpaWrapperAsfp = malloc(sizeof(marpaWrapperAsf_t));
  if (marpaWrapperAsfp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  marpaWrapperAsfp->marpaWrapperRecognizerp = marpaWrapperRecognizerp;
  marpaWrapperAsfp->marpaWrapperAsfOption   = *marpaWrapperAsfOptionp;
  marpaWrapperAsfp->marpaBocagep            = NULL;
  marpaWrapperAsfp->marpaOrderp             = NULL;
  marpaWrapperAsfp->orNodeStackp            = NULL;
  marpaWrapperAsfp->intsetHashp             = NULL;
  marpaWrapperAsfp->nidsetStackp            = NULL;
  marpaWrapperAsfp->powersetStackp          = NULL;
  marpaWrapperAsfp->lastNidsetp             = NULL;
  marpaWrapperAsfp->lastPowersetp           = NULL;
  marpaWrapperAsfp->gladeStackp             = NULL;
  marpaWrapperAsfp->factoringStackp         = NULL;
  marpaWrapperAsfp->orNodeInUseStackp       = NULL;
  marpaWrapperAsfp->nextIntseti             = 0;
  marpaWrapperAsfp->traverserCallbackp      = NULL;
  marpaWrapperAsfp->userDatavp              = NULL;
  marpaWrapperAsfp->lastValuesStackp        = NULL;
  marpaWrapperAsfp->lastAllChoicesStackp    = NULL;

  /* Always succeed as per the doc */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_latest_earley_set(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
  marpaLatestEarleySetIdi = marpa_r_latest_earley_set(marpaWrapperRecognizerp->marpaRecognizerp);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_b_new(%p, %d)", marpaWrapperRecognizerp->marpaRecognizerp, (int) marpaLatestEarleySetIdi);
  marpaWrapperAsfp->marpaBocagep = marpa_b_new(marpaWrapperRecognizerp->marpaRecognizerp, marpaLatestEarleySetIdi);
  if (marpaWrapperAsfp->marpaBocagep == NULL) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_new(%p)", marpaWrapperAsfp->marpaBocagep);
  marpaWrapperAsfp->marpaOrderp = marpa_o_new(marpaWrapperAsfp->marpaBocagep);
  if (marpaWrapperAsfp->marpaOrderp == NULL) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  highRankOnlyFlagi = (marpaWrapperAsfOptionp->highRankOnlyb != 0) ? 1 : 0;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_high_rank_only_set(%p, %d)", marpaWrapperAsfp->marpaOrderp, highRankOnlyFlagi);
  if (marpa_o_high_rank_only_set(marpaWrapperAsfp->marpaOrderp, highRankOnlyFlagi) != highRankOnlyFlagi) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (marpaWrapperAsfOptionp->orderByRankb != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_rank(%p)", marpaWrapperAsfp->marpaOrderp);
    if (marpa_o_rank(marpaWrapperAsfp->marpaOrderp) < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if (marpaWrapperAsfOptionp->ambiguousb == 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_ambiguity_metric(%p)", marpaWrapperAsfp->marpaOrderp);
    ambiguousi = marpa_o_ambiguity_metric(marpaWrapperAsfp->marpaOrderp);
    if (ambiguousi < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    } else if (ambiguousi > 1) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Parse is ambiguous");
      goto err;
    }
  }

  /* ASF is not possible for a null parse */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_is_null(%p)", marpaWrapperAsfp->marpaOrderp);
  nulli = marpa_o_is_null(marpaWrapperAsfp->marpaOrderp);
  if (nulli < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  } else if (nulli >= 1) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Parse is null");
    goto err;
  }
  
  GENERICSTACK_NEW(marpaWrapperAsfp->orNodeStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->orNodeStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "orNode stack initialization error, %s", strerror(errno));
    goto err;
  }

  GENERICHASH_NEW_SIZED(marpaWrapperAsfp->intsetHashp, NULL, _marpaWrapperAsf_cmpIntsetFunction, 1, MARPAWRAPPERASF_INTSET_MAXROWS, 0);
  if (GENERICHASH_ERROR(marpaWrapperAsfp->intsetHashp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Intset hash initialization error, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(marpaWrapperAsfp->nidsetStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->nidsetStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Nidset stack initialization error, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(marpaWrapperAsfp->powersetStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->powersetStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Powerset stack initialization error, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(marpaWrapperAsfp->gladeStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->gladeStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "glade stack initialization error, %s", strerror(errno));
    goto err;
  }

  /* marpaWrapperAsfp->factoringStackp is left DELIBIRATELY to a NULL value by default */
  /*
  GENERICSTACK_NEW(marpaWrapperAsfp->factoringStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoring stack initialization error, %s", strerror(errno));
    goto err;
  }
  */

  GENERICSTACK_NEW(marpaWrapperAsfp->orNodeInUseStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->orNodeInUseStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "orNodeInUse stack initialization error, %s", strerror(errno));
    goto err;
  }

  if (genericLoggerp != NULL) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Cloning genericLogger");

    marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp = GENERICLOGGER_CLONE(genericLoggerp);
    if (marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Failed to clone genericLogger: %s", strerror(errno));
      goto err;
    }
  }

  /* Say we are in forest mode */
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Flagging tree mode to FOREST");
  marpaWrapperRecognizerp->treeModeb = MARPAWRAPPERRECOGNIZERTREEMODE_FOREST;

  /* Get all the primary AND nodes */
  for (orNodei = 0; ; orNodei++) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_o_or_node_and_node_count(%p, %d)", marpaWrapperAsfp->marpaOrderp, (int) orNodei);
    andNodeCounti = _marpa_o_or_node_and_node_count(marpaWrapperAsfp->marpaOrderp, orNodei);
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "OR Node Id %d says there are %d AND nodes", orNodei, andNodeCounti);
    if (andNodeCounti == -1) {
      andNodeCounti = 0;
    }
    if (andNodeCounti < 0) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Invalid OR Node Id %d", orNodei);
      goto err;
    }
    if (andNodeCounti == 0) {
      break;
    }

    orNodep = malloc(sizeof(marpaWrapperAsfOrNode_t));
    if (orNodep == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }
    andNodep = malloc(andNodeCounti * sizeof(int));
    if (andNodep == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }

    orNodep->nAndNodel = (size_t) andNodeCounti;
    orNodep->andNodep = andNodep;

    /* Prepare the sort the AND Node Ids or this OR Node using predecessor Id */
    andNodeIdAndPredecessorIdp = malloc(andNodeCounti * sizeof(marpaWrapperAfsAndNodeIdAndPredecessorId_t));
    if (andNodeIdAndPredecessorIdp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }

    for (ix = 0; ix < andNodeCounti; ix++) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_o_or_node_and_node_id_by_ix(%p, %d, %d)", marpaWrapperAsfp->marpaOrderp, (int) orNodei, (int) ix);
      andNodeIdi            = _marpa_o_or_node_and_node_id_by_ix(marpaWrapperAsfp->marpaOrderp, orNodei, ix);
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_predecessor(%p, %d);", marpaWrapperAsfp->marpaBocagep, (int) andNodeIdi);
      andNodePredecessorIdi = _marpa_b_and_node_predecessor(marpaWrapperAsfp->marpaBocagep, andNodeIdi);
      if (andNodePredecessorIdi < -1) {
	MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	goto err;
      }

      andNodeIdAndPredecessorIdp[ix].andNodeIdi            = andNodeIdi;
      andNodeIdAndPredecessorIdp[ix].andNodePredecessorIdi = andNodePredecessorIdi;
    }

    qsort(andNodeIdAndPredecessorIdp,
	  (size_t) andNodeCounti,
	  sizeof(marpaWrapperAfsAndNodeIdAndPredecessorId_t),
	  _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi);
    
    for (ix = 0; ix < andNodeCounti; ix++) {
      andNodep[ix] = andNodeIdAndPredecessorIdp[ix].andNodeIdi;
    }

    free(andNodeIdAndPredecessorIdp);
    andNodeIdAndPredecessorIdp = NULL;

    GENERICSTACK_SET_PTR(marpaWrapperAsfp->orNodeStackp, orNodep, orNodei);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->orNodeStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "orNode stack failure: %s", strerror(errno));
      goto err;
    }

  }

  if (genericLoggerp != NULL) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Cloning genericLogger");

    marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp = GENERICLOGGER_CLONE(genericLoggerp);
    if (marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Failed to clone genericLogger: %s", strerror(errno));
      goto err;
    }
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperAsfp);
  return marpaWrapperAsfp;

err:
  if (marpaWrapperAsfp != NULL) {
    int errnoi = errno;

    if ((genericLoggerp != NULL) &&
        (marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp != NULL) &&
        (marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp != genericLoggerp)) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned genericLogger");
      GENERICLOGGER_FREE(marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp);
    }
    marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp = NULL;

    if (orNodep != NULL) {
      if (orNodep->andNodep != NULL) {
	free(orNodep->andNodep);
      }
      free(orNodep);
    }

    if (andNodeIdAndPredecessorIdp != NULL) {
      free(andNodeIdAndPredecessorIdp);
    }
    
    marpaWrapperAsf_freev(marpaWrapperAsfp);
    errno = errnoi;
  }

  if (marpaWrapperRecognizerp != NULL) {
    marpaWrapperRecognizerp->treeModeb = MARPAWRAPPERRECOGNIZERTREEMODE_NA;
  }

  return NULL;
}

/****************************************************************************/
void *marpaWrapperAsf_traversep(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp, void *userDatavp)
/****************************************************************************/
{
  const static char           funcs[] = "marpaWrapperAsf_traverseb";
  genericLogger_t            *genericLoggerp = NULL;
  int                         gladeIdi;
  marpaWrapperAsfGlade_t     *gladep;
  marpaWrapperAsfTraverser_t *traverserp = NULL;
  void                       *valuep;

  if (marpaWrapperAsfp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  if (_marpaWrapperAsf_peakb(marpaWrapperAsfp, &gladeIdi) == 0) {
    goto err;
  }
  gladep = _marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, (size_t) gladeIdi);
  if (gladep == NULL) {
    goto err;
  }

  marpaWrapperAsfp->traverserCallbackp = traverserCallbackp;
  marpaWrapperAsfp->userDatavp         = userDatavp;

  traverserp = malloc(sizeof(marpaWrapperAsfTraverser_t));
  if (traverserp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc error: %s", strerror(errno));
    goto err;
  }
  traverserp->marpaWrapperAsfp = marpaWrapperAsfp;
  GENERICSTACK_NEW(traverserp->valuep);
  if (GENERICSTACK_ERROR(traverserp->valuep)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "traverserp->valuep initialization failure: %s", strerror(errno));
    goto err;
  }
  traverserp->gladep       = gladep;
  traverserp->symchIxi     = 0;
  traverserp->factoringIxi = 0;
  valuep = marpaWrapperAsfp->traverserCallbackp(traverserp, marpaWrapperAsfp->userDatavp);
  GENERICSTACK_FREE(traverserp->valuep);
  free(traverserp);

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return %p");
  return valuep;

 err:
  if (traverserp != NULL) {
    GENERICSTACK_FREE(traverserp->valuep);
    free(traverserp);
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
void marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[] = "marpaWrapperAsf_freev";
  genericLogger_t         *genericLoggerp;

  if (marpaWrapperAsfp != NULL) {
    /* Keep a copy of the generic logger. If original is not NULL, then we have a clone of it */
    genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

    if (marpaWrapperAsfp->marpaOrderp != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_unref(%p)", marpaWrapperAsfp->marpaOrderp);
      marpa_o_unref(marpaWrapperAsfp->marpaOrderp);
    }

    if (marpaWrapperAsfp->marpaBocagep != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_b_unref(%p)", marpaWrapperAsfp->marpaBocagep);
      marpa_b_unref(marpaWrapperAsfp->marpaBocagep);
    }

    if (marpaWrapperAsfp->marpaWrapperRecognizerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Flagging tree mode to NA");
      marpaWrapperAsfp->marpaWrapperRecognizerp->treeModeb = MARPAWRAPPERRECOGNIZERTREEMODE_NA;
    }

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing orNode stack");
    _marpaWrapperAsf_orNodeStackp_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing intset hash");
    GENERICHASH_FREE(marpaWrapperAsfp->intsetHashp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Nidset stack");
    _marpaWrapperAsf_nidset_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Powerset stack");
    _marpaWrapperAsf_powerset_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing glade stack");
    _marpaWrapperAsf_gladeStackp_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing factoring stack");
    _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing orNodeInUse stack");
    GENERICSTACK_FREE(marpaWrapperAsfp->orNodeInUseStackp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing last values stack");
    GENERICSTACK_FREE(marpaWrapperAsfp->lastValuesStackp);

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing last all choices stack");
    _marpaWrapperAsf_lastAllChoices_freev(marpaWrapperAsfp);

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperAsfp);
    free(marpaWrapperAsfp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_lastAllChoices_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_lastAllChoices_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   i;
  genericStack_t          *stackp;

  if (marpaWrapperAsfp->lastAllChoicesStackp != NULL) {
    /* This is a stack of stack */
    for (i = 0; i < GENERICSTACK_USED(marpaWrapperAsfp->lastAllChoicesStackp); i++) {
      if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->lastAllChoicesStackp, i)) {
	stackp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->lastAllChoicesStackp, i);
	GENERICSTACK_FREE(stackp);
      }
    }
    GENERICSTACK_FREE(marpaWrapperAsfp->lastAllChoicesStackp);
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_orNodeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_orNodeStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   orNodeUsedl;
  size_t                   i;
  marpaWrapperAsfOrNode_t *orNodep;

  orNodeUsedl = GENERICSTACK_USED(marpaWrapperAsfp->orNodeStackp);
  for (i = 0; i < orNodeUsedl; i++) {
    if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, i)) {
      orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, i);
      if (orNodep->andNodep != NULL) {
	free(orNodep->andNodep);
      }
    }
  }
  GENERICSTACK_FREE(marpaWrapperAsfp->orNodeStackp);
}

/****************************************************************************/
static inline void _marpaWrapperAsf_gladeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_gladeStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   gladeUsedl;
  size_t                   i;
  marpaWrapperAsfGlade_t  *gladep;

  gladeUsedl = GENERICSTACK_USED(marpaWrapperAsfp->gladeStackp);
  for (i = 0; i < gladeUsedl; i++) {
    if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, i)) {
      gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, i);
      if (gladep->symchesStackp != NULL) {
	_marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, gladep->symchesStackp);
      }
      free(gladep);
    }
  }
  GENERICSTACK_FREE(marpaWrapperAsfp->gladeStackp);
}

/****************************************************************************/
static inline void _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *symchesStackp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_symchesStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   symchesUsedl;
  size_t                   i;
  genericStack_t          *factoringsStackp;

  if (symchesStackp != NULL) {
    symchesUsedl = GENERICSTACK_USED(symchesStackp);
    for (i = 0; i < symchesUsedl; i++) {
      if (GENERICSTACK_IS_PTR(symchesStackp, i)) {
	factoringsStackp = GENERICSTACK_GET_PTR(symchesStackp, i);
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
      }
    }
    GENERICSTACK_FREE(symchesStackp);
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_factoringStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   factoringUsedl;
  size_t                   i;
  marpaWrapperAsfNook_t   *nookp;

  if (marpaWrapperAsfp->factoringStackp != NULL) {
    factoringUsedl = GENERICSTACK_USED(marpaWrapperAsfp->factoringStackp);
    for (i = 0; i < factoringUsedl; i++) {
      if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->factoringStackp, i)) {
        nookp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->factoringStackp, i);
        if (nookp != NULL) {
          free(nookp);
        }
      }
    }
    /* Note: GENERICSTACK_FREE() is always resetting the pointer as well */
    GENERICSTACK_FREE(marpaWrapperAsfp->factoringStackp);
    /* I.e. here marpaWrapperAsfp->factoringStackp is NULL */
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *factoringsStackp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_factoringsStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   factoringsUsedl;
  size_t                   i;
  genericStack_t          *localStackp;

  if (factoringsStackp != NULL) {
    factoringsUsedl = GENERICSTACK_USED(factoringsStackp);
    /* At indice >= 2, factoringsStackp can contain inner generic stacks */
    for (i = 2; i < factoringsUsedl; i++) {
      if (GENERICSTACK_IS_PTR(factoringsStackp, i)) {
        localStackp = GENERICSTACK_GET_PTR(factoringsStackp, i);
	GENERICSTACK_FREE(localStackp);
      }
    }
    /* Note: GENERICSTACK_FREE() is always restting the pointer as well */
    GENERICSTACK_FREE(factoringsStackp);
  }
}

/****************************************************************************/
static inline short _marpaWrapperAsf_factoringStackp_resetb(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char            funcs[]        = "_marpaWrapperAsf_factoringStackp_resetb";
  genericLogger_t             *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp);
  GENERICSTACK_NEW(marpaWrapperAsfp->factoringStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStack reset failure: %s", strerror(errno));
    goto err;
  }

  MARPAWRAPPER_ERROR(genericLoggerp, "return 1");
  return 1;

 err:
  MARPAWRAPPER_ERROR(genericLoggerp, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_peakb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_peakb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfGlade_t  *gladep         = NULL;
  int                      augmentOrNodeIdi;
  int                      augmentAndNodeIdi;
  int                      startOrNodeIdi;
  marpaWrapperAsfNidset_t *baseNidsetp;
  marpaWrapperAsfOrNode_t *orNodep;
  int                      gladeIdi;

  augmentOrNodeIdi = (int) _marpa_b_top_or_node(marpaWrapperAsfp->marpaBocagep);
  if (augmentOrNodeIdi < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperAsfp->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, (size_t) augmentOrNodeIdi)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "augmentOrNodeIdi is not in the orNode stack");
    goto err;
  }

  orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, augmentOrNodeIdi);
  if (orNodep->nAndNodel <= 0) {
    MARPAWRAPPER_ERROR(genericLoggerp, "No AND node at this orNode stack");
    goto err;
  }
  augmentAndNodeIdi = orNodep->andNodep[0];
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_cause(%p, %d);", marpaWrapperAsfp->marpaBocagep, augmentAndNodeIdi);
  startOrNodeIdi = _marpa_b_and_node_cause(marpaWrapperAsfp->marpaBocagep, augmentAndNodeIdi);
  if (startOrNodeIdi < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperAsfp->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  baseNidsetp = _marpaWrapperAsf_nidset_obtainb(marpaWrapperAsfp, 1, &startOrNodeIdi);
  if (baseNidsetp == NULL) {
    goto err;
  }

  /* We cannot "obtain" the glade if it is not registered */
  gladeIdi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, (size_t) gladeIdi)) {
    gladep = malloc(sizeof(marpaWrapperAsfGlade_t));
    if (gladep == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }
    gladep->idi           = gladeIdi;
    gladep->symchesStackp = NULL;
    gladep->visitedb      = 0;
    gladep->registeredb   = 1;
    GENERICSTACK_SET_PTR(marpaWrapperAsfp->gladeStackp, gladep, gladeIdi);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->gladeStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "glade stack failure: %s", strerror(errno));
      goto err;
    }
  }

  *gladeIdip = gladeIdi;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *gladeIdip=%d", *gladeIdip);
  return 1;

 err:
  if (gladep != NULL) {
    _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, gladep->symchesStackp);
    free(gladep);    
  }
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 0", 0);
  return 0;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi(const void *p1, const void *p2)
/****************************************************************************/
{
  marpaWrapperAfsAndNodeIdAndPredecessorId_t *a1 = (marpaWrapperAfsAndNodeIdAndPredecessorId_t *) p1;
  marpaWrapperAfsAndNodeIdAndPredecessorId_t *a2 = (marpaWrapperAfsAndNodeIdAndPredecessorId_t *) p2;

  return (a1->andNodePredecessorIdi < a2->andNodePredecessorIdi) ? -1 : ((a1->andNodePredecessorIdi > a2->andNodePredecessorIdi) ? 1 : 0);
}

/****************************************************************************/
static inline short _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, size_t countl, int *idip)
/****************************************************************************/
{
  const static char        funcs[] = "_marpaWrapperAsf_intsetIdb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  const static char       *staticKeys = "";
  char                    *keys = NULL;
  char                    *keyNexts;
  size_t                   idl;
  short                    rcb;
  short                    findResultb;
  int                      intsetIdi;
  size_t                   keyi;

  if (countl <= 0) {
    keys = (char *) staticKeys;
  } else {
    for (idl = 0; idl < countl; idl++) {
      if (keys == NULL) {
	keys = _marpaWrapperAsf_stringBuilders("%d", idip[idl]);
	if (keys == NULL) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "_marpaWrapperAsf_stringBuilders error: %s", strerror(errno));
	  goto err;
	}
      } else {
	keyNexts = _marpaWrapperAsf_stringBuilders("%s %d", keys, idip[idl]);
	if (keyNexts == NULL) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "_marpaWrapperAsf_stringBuilders error: %s", strerror(errno));
	  goto err;
	}
	keys = keyNexts;
	free(keyNexts);
      }
    }
  }

  keyi = (size_t) (_marpaWrapperAsf_djb2((unsigned char *) keys) % MARPAWRAPPERASF_INTSET_MODULO);
  
  GENERICHASH_FIND_BY_IND(marpaWrapperAsfp->intsetHashp, marpaWrapperAsfp, INT, keyi, 0, findResultb, intsetIdi);
  if (GENERICHASH_ERROR(marpaWrapperAsfp->intsetHashp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "intset hash find failure: %s", strerror(errno));
    goto err;
  }
  if (! findResultb) {
    intsetIdi = marpaWrapperAsfp->nextIntseti++;
    GENERICHASH_SET_BY_IND(marpaWrapperAsfp->intsetHashp, marpaWrapperAsfp, INT, keys, intsetIdi);
    if (GENERICHASH_ERROR(marpaWrapperAsfp->intsetHashp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "intset hash set failure: %s", strerror(errno));
      goto err;
    }
  }

  *intsetIdip = intsetIdi;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  
  if ((keys != NULL) && (keys != staticKeys)) {
    free(keys);
  }
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", (int) rcb);
  return rcb;
}

/****************************************************************************/
static inline char *_marpaWrapperAsf_stringBuilders(char *fmts, ...)
/****************************************************************************/
{
  va_list ap;
  char   *msgs;

  va_start(ap, fmts);
  msgs = _marpaWrapperAsf_stringBuilder_aps(fmts, ap);
  va_end(ap);

  return msgs;
}

/****************************************************************************/
static inline char *_marpaWrapperAsf_stringBuilder_aps(char *fmts, va_list ap)
/****************************************************************************/
{
  /* This is more or less a copy of genericLogger's internal builder */
  int     n;
  size_t  sizel = 4096;     /* Guess we need no more than 4096 bytes */
  char   *p, *np;
#ifdef C_VA_COPY
  va_list ap2;
#endif

  /* ----------------------------------------------------------------------------------------------------------------------- */
  /* Take care: Windows's vsnprintf is not like UNIX's, i.e:                                                                 */
  /*                                                                                                                         */
  /* Output:                                                                                                                 */
  /* [Windows] -1 if the number of characters if > count. Minus trailing null character                                      */
  /* [ UNIX  ] number of chars that would have been writen. Minus trailing null character                                    */
  /*                                                                                                                         */
  /* Argument:                                                                                                               */
  /* [Windows] number of characters wanted, does not include the trailing null character                                     */
  /* [ UNIX  ] number of characters wanted + the trailing null character                                                     */
  /* ----------------------------------------------------------------------------------------------------------------------- */

  p = malloc(sizel);
  if (p == NULL) {
    return NULL;
  }

  while (1) {

    /* Try to print in the allocated space */

#ifdef C_VA_COPY
    C_VA_COPY(ap2, ap);
#endif
    n = C_VSNPRINTF(p, sizel, fmts, REAL_AP);   /* On Windows, argument does not include space for the NULL */
#ifdef C_VA_COPY
    va_end(ap2);
#endif

    /* Check error code */
#ifndef _WIN32
    /* On not-windows, if output is negative an output error is encountered */
    if (n < 0) {
      free(p);
      return NULL;
    }
#endif

    /* If that worked, return the string, unless not enough space - in which case we retry -; */

    if
#ifdef _WIN32
      ((n >= 0) && (n < (int) sizel))
#else
      (n < sizel)
#endif
        {
      return p;
    }

    /* Else try again with more space */

#ifdef _WIN32
    sizel *= 2;          /* Maybe enough ? */
#else
    sizel = n + 1;       /* Precisely what is needed */
#endif

    np = realloc(p, sizel);
    if (np == NULL) {
      free(p);
      return NULL;
    } else {
      p = np;
    }
  }

  /* Should never happen */
  return NULL;
}

/****************************************************************************/
static inline short  _marpaWrapperAsf_cmpIntsetFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2)
/****************************************************************************/
{
  return (* (int *) p1) == (* (int *) p2);
}

/****************************************************************************/
static inline short  _marpaWrapperAsf_cmpNidsetFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2)
/****************************************************************************/
{
  return (* (int *) p1) == (* (int *) p2);
}

/****************************************************************************/
static inline unsigned long _marpaWrapperAsf_djb2(unsigned char *str)
/****************************************************************************/
{
  unsigned long hash = 5381;
  int c;

  while (c = *str++) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }

  return hash;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_idCmpi(const void *p1, const void *p2)
/****************************************************************************/
{
  int *a1 = (int *) p1;
  int *a2 = (int *) p2;

  return (a1 < a2) ? -1 : ((a1 > a2) ? 1 : 0);
}

/****************************************************************************/
static inline marpaWrapperAsfIdset_t *_marpaWrapperAsf_idset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, size_t countl, int *idip)
/****************************************************************************/
{
  const static char        funcs[] = "_marpaWrapperAsf_idset_obtainb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  genericStack_t         **stackpp = (idsete == MARPAWRAPPERASFIDSET_NIDSET) ? &(marpaWrapperAsfp->nidsetStackp) : &(marpaWrapperAsfp->powersetStackp);
  char                    *idsets  = marpaWrapperAsfIdsets[idsete];
  marpaWrapperAsfIdset_t  *idsetp   = NULL;
  int                      intsetIdi;

  if (_marpaWrapperAsf_intsetIdb(marpaWrapperAsfp, &intsetIdi, countl, idip) == 0) {
    goto err;
  }

  if (GENERICSTACK_IS_PTR((*stackpp), (size_t) intsetIdi)) {
    idsetp = GENERICSTACK_GET_PTR((*stackpp), intsetIdi);
  } else {
    idsetp = malloc(sizeof(marpaWrapperAsfIdset_t));
    if (idsetp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }
    idsetp->idi = intsetIdi;
    if (countl <= 0) {
      idsetp->countl = 0;
      idsetp->idip   = NULL;
    } else {
      idsetp->countl = countl;
      idsetp->idip   = malloc(sizeof(int) * countl);
      if (idsetp->idip == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
	goto err;
      }
      memcpy(idsetp->idip, idip, countl);
      qsort(idsetp->idip, countl, sizeof(int), _marpaWrapperAsf_idCmpi);
      GENERICSTACK_SET_PTR((*stackpp), idsetp, intsetIdi);
      if (GENERICSTACK_ERROR((*stackpp))) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "%s genericStack error: %s", idsets, strerror(errno));
	goto err;
      }
    }
  }

  goto done;

 err:
  if (idsetp != NULL) {
    if (idsetp->idip != NULL) {
      free(idsetp->idip);
    }
    free(idsetp);
    idsetp = NULL;
  }

 done:

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", idsetp);
  return idsetp;
}

/****************************************************************************/
static inline int *_marpaWrapperAsf_idset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idipb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %p", marpaWrapperAsfIdsets[idsete], idsetp->idip);
  return idsetp->idip;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi_by_ixib";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  if ((size_t) ixi >= idsetp->countl) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "%s: indice %d out of range [0..%d[", marpaWrapperAsfIdsets[idsete], ixi, (int) idsetp->countl);
    goto err;
  }

  *idip = idsetp->idip[ixi];
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return 1, *idip=%d", marpaWrapperAsfIdsets[idsete], idsetp->idip[ixi]);
  return 1;

 err:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return 0", marpaWrapperAsfIdsets[idsete]);
  return 0;
  
}

/****************************************************************************/
static inline size_t _marpaWrapperAsf_idset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_countl";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %d", marpaWrapperAsfIdsets[idsete], (int) idsetp->countl);
  return idsetp->countl;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_idset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %d", marpaWrapperAsfIdsets[idsete], (int) idsetp->idi);
  return idsetp->idi;
}

/****************************************************************************/
static inline char *_marpaWrapperAsf_idset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_showb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  char                    *idsets         = marpaWrapperAsfIdsets[idsete];
  char                    *Idsets         = marpaWrapperAsfIdsetUpfirsts[idsete];
  int                      idi            = _marpaWrapperAsf_idset_idi(marpaWrapperAsfp, idsete, idsetp);
  int                     *idip           = _marpaWrapperAsf_idset_idip(marpaWrapperAsfp, idsete, idsetp);
  size_t                   countl         = _marpaWrapperAsf_idset_countl(marpaWrapperAsfp, idsete, idsetp);
  char                    *shows;
  size_t                   idl;
  char                    *showNexts;

  shows = _marpaWrapperAsf_stringBuilders("%s #%d:", Idsets, idi);
  if (shows == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "%s: _marpaWrapperAsf_stringBuilders error: %s", idsets, strerror(errno));
    goto err;
  }
  for (idl = 0; idl < countl; idl++) {
    int idiByIxi;

    if (_marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsfp, idsete, idsetp, (int) idl, &idiByIxi) == 0) {
      goto err;
    }
    showNexts = _marpaWrapperAsf_stringBuilders("%s %d", shows, idiByIxi);
    if (showNexts == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "%s: _marpaWrapperAsf_stringBuilders error: %s", idsets, strerror(errno));
      goto err;
    }
    shows = showNexts;
    free(showNexts);
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return \"%s\"", idsets, shows);
  return shows;

 err:
  if (shows != NULL) {
    free(shows);
  }
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return NULL", idsets);
  return NULL;
}

/****************************************************************************/
static inline void _marpaWrapperAsf_idset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_freeb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  genericStack_t         **stackpp        = (idsete == MARPAWRAPPERASFIDSET_NIDSET) ? &(marpaWrapperAsfp->nidsetStackp) : &(marpaWrapperAsfp->powersetStackp);
  char                    *idsets         = marpaWrapperAsfIdsets[idsete];
  size_t                   idsetUsedl;
  size_t                   i;
  marpaWrapperAsfIdset_t  *idsetp;

  idsetUsedl = GENERICSTACK_USED((*stackpp));
  /* Every idset may have an array inside */
  for (i = 0; i < idsetUsedl; i++) {
    if (GENERICSTACK_IS_PTR((*stackpp), i)) {
      idsetp = GENERICSTACK_GET_PTR((*stackpp), i);
      if (idsetp->idip != NULL) {
	free(idsetp->idip);
      }
      free(idsetp);
    }
  }
  GENERICSTACK_FREE((*stackpp));
}

/****************************************************************************/
static inline marpaWrapperAsfNidset_t *_marpaWrapperAsf_nidset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t countl, int *idip)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_obtainb(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, countl, idip);
}

/****************************************************************************/
static inline int *_marpaWrapperAsf_nidset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idip(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp);
}

/****************************************************************************/
static inline short _marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp, ixi, idip);
}

/****************************************************************************/
static inline size_t _marpaWrapperAsf_nidset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_countl(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp);
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nidset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp);
}

/****************************************************************************/
static inline char *_marpaWrapperAsf_nidset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_showb(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp);
}

/****************************************************************************/
static inline void _marpaWrapperAsf_nidset_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  _marpaWrapperAsf_idset_freev(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET);
}

/****************************************************************************/
static inline marpaWrapperAsfPowerset_t *_marpaWrapperAsf_powerset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t countl, int *idip)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_obtainb(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, countl, idip);
}

/****************************************************************************/
static inline int *_marpaWrapperAsf_powerset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idip(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp);
}

/****************************************************************************/
static inline short _marpaWrapperAsf_powerset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp, ixi, idip);
}

/****************************************************************************/
static inline size_t _marpaWrapperAsf_powerset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_countl(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp);
}

/****************************************************************************/
static inline int _marpaWrapperAsf_powerset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp);
}

/****************************************************************************/
static inline char *_marpaWrapperAsf_powerset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_showb(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp);
}

/****************************************************************************/
static inline void _marpaWrapperAsf_powerset_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  _marpaWrapperAsf_idset_freev(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET);
}

/****************************************************************************/
static inline marpaWrapperAsfNook_t *_marpaWrapperAsf_nook_newp(marpaWrapperAsf_t *marpaWrapperAsfp, int orNodeIdi, int parentOrNodeIdi)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_nook_newp";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfNook_t   *nookp;

  nookp = malloc(sizeof(marpaWrapperAsfNook_t));
  if (nookp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  nookp->orNodeIdi              = orNodeIdi;
  nookp->firstChoicei           = 0;
  nookp->lastChoicei            = 0;
  nookp->parentOrNodeIdi        = parentOrNodeIdi;
  nookp->isCauseb               = 0;
  nookp->isPredecessorb         = 0;
  nookp->causeIsExpandedb       = 0;
  nookp->predecessorIsExpandedb = 0;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nookp);
  return NULL;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]                  = "_marpaWrapperAsf_nook_has_semantic_causeb";
  genericLogger_t         *genericLoggerp           = marpaWrapperAsfp->genericLoggerp;
  int                       orNodeIdi               = nookp->orNodeIdi;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  Marpa_IRL_ID              irlIdi;
  int                       predotPositioni;
  Marpa_Symbol_ID           predotIsyIdi;
  short                    rcb;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_irl(%p, %d)", marpaBocagep, orNodeIdi);
  irlIdi = _marpa_b_or_node_irl(marpaBocagep, orNodeIdi);
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_position(%p, %d)", marpaBocagep, orNodeIdi);
  predotPositioni = _marpa_b_or_node_position(marpaBocagep, orNodeIdi) - 1;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_irl_rhs(%p, %d, %d)", marpaGrammarp, (int) irlIdi, predotPositioni);
  predotIsyIdi = _marpa_g_irl_rhs(marpaGrammarp, irlIdi, predotPositioni);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_nsy_is_semantic(%p, %d)", marpaGrammarp, (int) predotIsyIdi);
  rcb = (_marpa_g_nsy_is_semantic(marpaGrammarp, predotIsyIdi) != 0) ? 1 : 0;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", (int) rcb);
  return rcb;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_setLastChoiceb(marpaWrapperAsf_t *marpaWrapperAsfp, int **choiceipp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_setLastChoiceb";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->genericLoggerp;
  int                      orNodeIdi          = nookp->orNodeIdi;
  int                      choicei            = nookp->firstChoicei;
  short                    rcb                = 1;
  marpaWrapperAsfOrNode_t *orNodep;

  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, (size_t) orNodeIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No entry in orNode stack at indice %d", orNodeIdi);
    goto err;
  }
  orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, orNodeIdi);

  if (choicei >= (int) orNodep->nAndNodel) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "No last choice available");
    *choiceipp = NULL;
  } else {
    if (_marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsfp, nookp)) {
      marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
      marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
      Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
      int                       andNodeIdi              = orNodep->andNodep[choicei];
      int                       currentPredecessori;
      int                       nextPredecessori;

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_predecessor(%p, %d)", marpaBocagep, andNodeIdi);
      currentPredecessori = _marpa_b_and_node_predecessor(marpaBocagep, andNodeIdi);
      if (currentPredecessori < 0) {
	currentPredecessori = -1;
      }
      while (1) {
	if (++choicei >= (int) orNodep->nAndNodel) {
	  break;
	}
	andNodeIdi = orNodep->andNodep[choicei];
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_predecessor(%p, %d)", marpaBocagep, andNodeIdi);
	nextPredecessori = _marpa_b_and_node_predecessor(marpaBocagep, andNodeIdi);
	if (nextPredecessori < 0) {
	  nextPredecessori = -1;
	}
	if (currentPredecessori != nextPredecessori) {
	  break;
	}
      }
      --choicei;
    }
    nookp->lastChoicei = choicei;
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Last choice set to %d", choicei);
    *choiceipp = &(nookp->lastChoicei);
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_nook_incrementb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_setLastChoiceb";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->genericLoggerp;
  int                     *choiceip;
  short                    rcb;

  nookp->firstChoicei = nookp->lastChoicei + 1;

  rcb = _marpaWrapperAsf_setLastChoiceb(marpaWrapperAsfp, &choiceip, nookp);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", (int) rcb);
  return rcb;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_nidset_sort_ixi";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  int                       rci;
  
  if (nidi >= 0) {
    int irlIdi = _marpa_b_or_node_irl(marpaBocagep, nidi);
    
    rci = _marpa_g_source_xrl(marpaGrammarp, irlIdi);
  } else {
    int andNodeIdi = _marpaWrapperAsf_nid_to_and_nodei(nidi);
    int tokenNsyIdi = _marpa_b_and_node_symbol(marpaBocagep, andNodeIdi);
    int tokenIdi = _marpa_g_source_xsy(marpaGrammarp, tokenNsyIdi);

    /* -2 is reserved to 'end of data' */
    rci = -tokenIdi - 3;
  }

  return rci;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_and_node_to_nidi(int idi)
/****************************************************************************/
{
  return -idi + MARPAWRAPPERASF_NID_LEAF_BASE;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nid_to_and_nodei(int idi)
/****************************************************************************/
{
  return -idi + MARPAWRAPPERASF_NID_LEAF_BASE;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_sourceDataCmpi(const void *p1, const void *p2)
/****************************************************************************/
{
  marpaWrapperAsfSourceData_t *d1 = (marpaWrapperAsfSourceData_t *) p1;
  marpaWrapperAsfSourceData_t *d2 = (marpaWrapperAsfSourceData_t *) p2;

  return (d1->sortIxi < d2->sortIxi) ? -1 : ((d1->sortIxi > d2->sortIxi) ? 1 : 0);
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_powerset_nidsetp";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  int                       idi                     = -1;

  if (nidi >= 0) {
    marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
    marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
    Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
    Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
    Marpa_IRL_ID              irlIdi                  = _marpa_b_or_node_irl(marpaBocagep, nidi);
    idi = (int) _marpa_g_source_xrl(marpaGrammarp, irlIdi);
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", idi);
  return idi;
}

/****************************************************************************/
static inline marpaWrapperAsfNidset_t *_marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfPowerset_t *powersetp, int ixi)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_powerset_nidsetp";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfNidset_t *nidsetp            = NULL;
  size_t                   countl;
  int                      idi;

  countl = _marpaWrapperAsf_powerset_countl(marpaWrapperAsfp, powersetp);
  if (ixi < (int) countl) {
    if (_marpaWrapperAsf_powerset_idi_by_ixib(marpaWrapperAsfp, powersetp, ixi, &idi) == 0) {
      goto err;
    }
    if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->nidsetStackp, (size_t) idi)) {
      nidsetp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->nidsetStackp, idi);
    }
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nidsetp);
  return nidsetp;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline marpaWrapperAsfGlade_t *_marpaWrapperAsf_glade_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, size_t gladei)
/****************************************************************************/
{
  const static char            funcs[]                = "_marpaWrapperAsf_glade_obtainp";
  genericLogger_t             *genericLoggerp         = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfSourceData_t *sourceDatap            = NULL;
  size_t                       nidWithCurrentSortIxil = 0;
  int                         *nidWithCurrentSortIxip = NULL;
  size_t                       symchIdil              = 0;
  int                         *symchIdip              = NULL;
  short                        thisNidEndb            = 0;
  size_t                       nSymboll               = 0;
  int                         *symbolip               = NULL;
  genericStack_t              *symchesStackp = NULL;
  size_t                       nSourceDatal;
  marpaWrapperAsfGlade_t      *gladep;
  marpaWrapperAsfNidset_t     *baseNidsetp;
  marpaWrapperAsfPowerset_t   *choicepointPowsersetp;
  int                          sourceNidi;
  int                          nidIxi;
  size_t                       ixi;
  int                          sortIxOfThisNidi;
  int                          thisNidi;
  int                          currentSortIxi;
  marpaWrapperAsfNidset_t     *nidsetForSortIxp;
  size_t                       symchCountl;
  size_t                       symchIxl;
  int                          choicepointNidi;
  int                          symchRuleIdi;

  if ((! GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, gladei)) || ((gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, gladei))->registeredb == 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Attempt to use an invalid glade, one whose ID is %d", gladei);
    goto err;
  }

  /* Return the glade if it is already set up */
  if (gladep->symchesStackp != NULL) {
    goto done;
  }

  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->nidsetStackp, gladei)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No nidset at glade id %d", gladei);
    goto err;
  }
  baseNidsetp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->nidsetStackp, gladei);
  nSourceDatal = _marpaWrapperAsf_nidset_countl(marpaWrapperAsfp, baseNidsetp);
  if (nSourceDatal <= 0) {
    MARPAWRAPPER_ERROR(genericLoggerp, "No nidset");
    goto err;
  }
  sourceDatap = malloc(nSourceDatal * sizeof(marpaWrapperAsfSourceData_t));
  if (sourceDatap == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }
  for (ixi = 0; ixi < nSourceDatal; ixi++) {
    if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, baseNidsetp, ixi, &sourceNidi) == 0) {
      goto err;
    }
    sourceDatap[ixi].sortIxi    = _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsfp, sourceNidi);
    if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, baseNidsetp, ixi, &(sourceDatap[ixi].sourceNidi)) == 0) {
      goto err;
    }
  }
  qsort(sourceDatap, nSourceDatal, sizeof(marpaWrapperAsfSourceData_t), _marpaWrapperAsf_sourceDataCmpi);

  sortIxOfThisNidi = sourceDatap[0].sortIxi;
  thisNidi         = sourceDatap[0].sourceNidi;
  nidIxi           = 1;
  currentSortIxi   = sortIxOfThisNidi;
  while (1) {
    if (sortIxOfThisNidi != currentSortIxi) {
      /* Current only whole id break logic */
      nidsetForSortIxp = _marpaWrapperAsf_nidset_obtainb(marpaWrapperAsfp, nidWithCurrentSortIxil, nidWithCurrentSortIxip);
      if (nidsetForSortIxp == NULL) {
	goto err;
      }
      /* Create or extend symchIdip */
      if (symchIdip == NULL) {
	symchIdip = malloc(sizeof(int));
	if (symchIdip == NULL) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
	  goto err;
	}
      } else {
	int *tmpip = realloc(symchIdip, (symchIdil + 1) * sizeof(int));
	if (tmpip == NULL) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "realloc failure: %s", strerror(errno));
	  goto err;
	}
	symchIdip = tmpip;
      }
      symchIdip[symchIdil++] = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, nidsetForSortIxp);
      free(nidWithCurrentSortIxip);
      nidWithCurrentSortIxip = NULL;
      nidWithCurrentSortIxil = 0;

      currentSortIxi = sortIxOfThisNidi;
    }
    if (thisNidEndb != 0) {
      break;
    }
    /* Create or extend nidWithCurrentSortIxip */
    if (nidWithCurrentSortIxip == NULL) {
      nidWithCurrentSortIxip = malloc(sizeof(int));
      if (nidWithCurrentSortIxip == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
	goto err;
      }
    } else {
      int *tmpip = realloc(nidWithCurrentSortIxip, (nidWithCurrentSortIxil + 1) * sizeof(int));
      if (tmpip == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "realloc failure: %s", strerror(errno));
	goto err;
      }
      nidWithCurrentSortIxip = tmpip;
    }
    nidWithCurrentSortIxip[nidWithCurrentSortIxil++] = thisNidi;
    if (nidIxi < (int) nSourceDatal) {
      sortIxOfThisNidi = sourceDatap[nidIxi].sortIxi;
      thisNidi         = sourceDatap[nidIxi].sourceNidi;
      nidIxi++;
      continue;
    }
    thisNidEndb = 1;
    sortIxOfThisNidi = -2;
  }
  choicepointPowsersetp = _marpaWrapperAsf_powerset_obtainb(marpaWrapperAsfp, symchIdil, symchIdip);
  if (choicepointPowsersetp == NULL) {
    goto err;
  }
  /* Free factoring stack */
  _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp);

  /* Check if choicepoint already seen ? */
  GENERICSTACK_NEW(symchesStackp);
  if (GENERICSTACK_ERROR(symchesStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to initalize symchesStackp, %s", strerror(errno));
    goto err;
  }
  symchCountl = _marpaWrapperAsf_powerset_countl(marpaWrapperAsfp, choicepointPowsersetp);
  for (symchIxl = 0; symchIxl < symchCountl; symchIxl++) {
    genericStack_t          *factoringsStackp;
    genericStack_t          *factoring;
    marpaWrapperAsfNidset_t *symchNidsetp;
    size_t                   nidcountl;
    int                      nidixi;
    
    /* Free factoring stack */
    _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp);

    symchNidsetp = _marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsfp, choicepointPowsersetp, symchIxl);
    if (symchNidsetp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "symchNidsetp is NULL", strerror(errno));
      goto err;
    }
    if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, symchNidsetp, 0, &choicepointNidi) == 0) {
      goto err;
    }
    symchRuleIdi = _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsfp, choicepointNidi);

    /* Initial NULL indicates no factorings omitted */
    GENERICSTACK_NEW(factoringsStackp);
    if (GENERICSTACK_ERROR(factoringsStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "factoringsStackp initialization failure: %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_PUSH_INT(factoringsStackp, symchRuleIdi);
    if (GENERICSTACK_ERROR(factoringsStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "factoringsStackp set at indice 0 failure: %s", strerror(errno));
      GENERICSTACK_FREE(factoringsStackp);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(factoringsStackp, NULL);
    if (GENERICSTACK_ERROR(factoringsStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "factoringsStackp set at indice 1 failure: %s", strerror(errno));
      GENERICSTACK_FREE(factoringsStackp);
      goto err;
    }

    if (symchRuleIdi < 0) {
      marpaWrapperAsfNidset_t *baseNidsetp = _marpaWrapperAsf_nidset_obtainb(marpaWrapperAsfp, 1, &choicepointNidi);
      int                      gladeidi;
      marpaWrapperAsfGlade_t  *gladep;
      genericStack_t          *localStackp;

      if (baseNidsetp == NULL) {
	GENERICSTACK_FREE(factoringsStackp);
	goto err;
      }
      gladeidi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
      if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, (size_t) gladeidi)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "No glade at indice %d of gladeStackp", gladeidi);
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
	goto err;
      }
      gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, gladeidi);
      gladep->registeredb = 1;

      GENERICSTACK_NEW(localStackp);
      if (GENERICSTACK_ERROR(localStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "localStackp initialization failure, %s", strerror(errno));
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
	goto err;
      }
      GENERICSTACK_PUSH_INT(localStackp, gladeidi);
      if (GENERICSTACK_ERROR(localStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "localStackp push failure, %s", strerror(errno));
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
	goto err;
      }
      GENERICSTACK_PUSH_PTR(factoringsStackp, localStackp);
      if (GENERICSTACK_ERROR(factoringsStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to push factoringsStackp, %s", strerror(errno));
	GENERICSTACK_FREE(localStackp);
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
	goto err;
      }
      /* localStackp is now in factoringsStackp */
      GENERICSTACK_PUSH_PTR(symchesStackp, factoringsStackp);
      if (GENERICSTACK_ERROR(symchesStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to push symchesStack, %s", strerror(errno));
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
	goto err;
      }
      /* factoringsStackp is now in symchesStackp */
      continue;
    }

    symchNidsetp = _marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsfp, choicepointPowsersetp, (int) symchIxl);
    if (symchNidsetp == NULL) {
      MARPAWRAPPER_ERROR(genericLoggerp, "symchNidsetp is NULL");
      goto err;
    }
    nidcountl = _marpaWrapperAsf_nidset_countl(marpaWrapperAsfp, symchNidsetp);
    for (nidixi = 0; nidixi < (int) nidcountl; nidixi++) {
      short breakFactoringsLoop = 0;
      short firstFactoringb;
      short factoringb;
      
      if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, symchNidsetp, nidixi, &choicepointNidi) == 0) {
	goto err;
      }
      if (_marpaWrapperAsf_first_factoringb(marpaWrapperAsfp, choicepointNidi, &firstFactoringb) == 0) {
	goto err;
      }

      if (_marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsfp, &factoring) == 0) {
	goto err;
      }

      while (factoring != NULL) {
	if (GENERICSTACK_USED(factoringsStackp) > MARPAWRAPPERASF_FACTORING_MAX) {
	  /* Update factorings omitted flag */
	  GENERICSTACK_SET_INT(factoringsStackp, 1, 1);
	  if (GENERICSTACK_ERROR(factoringsStackp)) {
	    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to set omitted flag in local factoring");
	    goto err;
	  }
	  breakFactoringsLoop = 1;
	  break;
	}
	{
	  genericStack_t *localFactoringStackp;
	  size_t          itemIxl;

	  GENERICSTACK_NEW(localFactoringStackp);
	  if (GENERICSTACK_ERROR(localFactoringStackp)) {
	    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize localFactoringStackp");
	    goto err;
	  }
	  for (itemIxl = GENERICSTACK_USED(factoring) - 1; itemIxl >= 0; itemIxl--) {
	    if (! GENERICSTACK_IS_INT(factoring, itemIxl)) {
	      MARPAWRAPPER_ERRORF(genericLoggerp, "Indice %d is not an int", (int) itemIxl);
	      GENERICSTACK_FREE(localFactoringStackp);
	      goto err;
	    }
	    
	    GENERICSTACK_PUSH_INT(localFactoringStackp, GENERICSTACK_GET_INT(factoring, itemIxl));
	    if (GENERICSTACK_ERROR(localFactoringStackp)) {
	      MARPAWRAPPER_ERROR(genericLoggerp, "Failure to push in localFactoringStackp");
	      GENERICSTACK_FREE(localFactoringStackp);
	      goto err;
	    }
	  }

	  GENERICSTACK_PUSH_PTR(factoringsStackp, localFactoringStackp);
	  if (GENERICSTACK_ERROR(factoringsStackp)) {
	    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to push in factoringsStackp");
	    GENERICSTACK_FREE(localFactoringStackp);
	    goto err;
	  }

	  if (_marpaWrapperAsf_next_factoringb(marpaWrapperAsfp, choicepointNidi, &factoringb) == 0) {
	    goto err;
	  }
	  if (_marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsfp, &factoring) == 0) {
	    goto err;
	  }
	}
      }
      if (breakFactoringsLoop != 0) {
	break;
      }
    }
    
  }

  /* Replace current symches */
  _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, gladep->symchesStackp);
  gladep->symchesStackp = symchesStackp;
  gladep->idi = gladei;

  GENERICSTACK_SET_PTR(marpaWrapperAsfp->gladeStackp, gladep, gladei);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->gladeStackp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to set in gladeStackp");
    goto err;
  }

 done:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", gladep);
  return gladep;

 err:
  if (sourceDatap != NULL) {
    free(sourceDatap);
  }
  if (nidWithCurrentSortIxip != NULL) {
    free(nidWithCurrentSortIxip);
  }
  if (symchIdip != NULL) {
    free(symchIdip);
  }
  if (symbolip != NULL) {
    free(symbolip);
  }
  _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, symchesStackp);

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_first_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, int nidOfChoicePointi, short *firstFactoringbp)
/****************************************************************************/
{
  const static char            funcs[]                = "_marpaWrapperAsf_first_factoringb";
  genericLogger_t             *genericLoggerp         = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfOrNode_t     *orNodep;
  marpaWrapperAsfNook_t       *nookp;

  /* Current NID of current SYMCH */
  /* The caller should ensure that we are never called unless the current NIS is for a rule */
  if (nidOfChoicePointi < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "First factoring called for negative NID %d", nidOfChoicePointi);
      goto err;
  }

  /* Due to skipping, even the top OR node can have no valid choices */
  if ((! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, (size_t) nidOfChoicePointi)) ||
      (((orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, nidOfChoicePointi))->nAndNodel) <= 0)) {
    _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp);
    *firstFactoringbp = 0;
    goto done;
  }

  GENERICSTACK_SET_SHORT(marpaWrapperAsfp->orNodeInUseStackp, 1, nidOfChoicePointi);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->orNodeInUseStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "orNodeInUseStackp failure: %s", strerror(errno));
    goto err;
  }

  nookp = _marpaWrapperAsf_nook_newp(marpaWrapperAsfp, nidOfChoicePointi, -1);
  if (nookp == NULL) {
    goto err;
  }
  if (_marpaWrapperAsf_factoringStackp_resetb(marpaWrapperAsfp) == 0) {
    goto err;
  }
  GENERICSTACK_PUSH_PTR(marpaWrapperAsfp->factoringStackp, nookp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStackp push failure: %s", strerror(errno));
    goto err;
  }

  /* Iterate as long as we cannot finish this stack */
  while (1) {
    short factoringFinishb;
    short factoringIterateb;

    if (_marpaWrapperAsf_factoring_finishb(marpaWrapperAsfp, nidOfChoicePointi, &factoringFinishb) == 0) {
      goto err;
    }
    if (factoringFinishb != 0) {
      break;
    }
    
    if (_marpaWrapperAsf_factoring_iterateb(marpaWrapperAsfp, &factoringIterateb) == 0) {
      goto err;
    }
    if (factoringIterateb == 0) {
      goto done;
    }
  }

  *firstFactoringbp = 1;

 done:

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *firstFactoringbp=%d", (int) *firstFactoringbp);
  return 0;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_factoring_finishb(marpaWrapperAsf_t *marpaWrapperAsfp, int nidOfChoicePointi, short *factoringFinishbp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_factoring_finishb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  genericStack_t           *worklistStackp;
  size_t                    worklistUsedl;
  size_t                    worklistStackl;
  size_t                    worklistLastl;

  if (marpaWrapperAsfp->factoringStackp == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "factoringStackp is NULL");
    goto err;
  }

  GENERICSTACK_NEW_SIZED(worklistStackp, GENERICSTACK_USED(marpaWrapperAsfp->factoringStackp));
  if (GENERICSTACK_ERROR(worklistStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "worklistStackp initialization failure: %s", strerror(errno));
    goto err;
  }
  for (worklistStackl = 0; worklistStackl < GENERICSTACK_USED(marpaWrapperAsfp->factoringStackp); worklistStackl++) {
    GENERICSTACK_SET_INT(worklistStackp, worklistStackl, worklistStackl);
    if (GENERICSTACK_ERROR(worklistStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "worklistStackp initialization at indice %d failure: %s", (int) worklistStackl, strerror(errno));
      goto err;
    }
  }

  while ((worklistUsedl = GENERICSTACK_USED(worklistStackp)) > 0) {
    marpaWrapperAsfNook_t   *workNookp;
    int                      workOrNodeIdi;
    int                      workingChoicei;
    marpaWrapperAsfOrNode_t *orNodep;
    int                      workAndNodeIdi;
    int                      childOrNodei;
    short                    childIsCauseb;
    short                    childIsPredecessorb;
    marpaWrapperAsfNook_t   *newNookp;

    worklistLastl = worklistUsedl - 1;

    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->factoringStackp, worklistLastl)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No nook at indice %d", (int) worklistLastl);
      goto err;
    }
    workNookp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->factoringStackp, worklistLastl);
    workOrNodeIdi  = workNookp->orNodeIdi;
    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, (size_t) workOrNodeIdi)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No orNode at indice %d of orNodesStackp", workOrNodeIdi);
      goto err;
    }
    orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, workOrNodeIdi);

    workingChoicei = workNookp->firstChoicei;;
    if (workingChoicei >= (int) orNodep->nAndNodel) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No andNode at indice %d of andNodep", workingChoicei);
      goto err;
    }
    workAndNodeIdi = orNodep->andNodep[workingChoicei];

    if (workNookp->causeIsExpandedb == 0) {
      if (_marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsfp, workNookp) == 0) {
        childOrNodei = (int) _marpa_b_and_node_cause(marpaBocagep, workAndNodeIdi);
        childIsCauseb = 1;
        goto endFindChildOrNodeLabel;
      }
    }

    workNookp->causeIsExpandedb = 1;
    if (workNookp->predecessorIsExpandedb == 0) {
      childOrNodei = (int) _marpa_b_and_node_predecessor(marpaBocagep, workAndNodeIdi);
      if (childOrNodei >= 0) {
        childIsPredecessorb = 1;
        goto endFindChildOrNodeLabel;
      }
    }

    workNookp->predecessorIsExpandedb = 1;
    GENERICSTACK_POP_INT(worklistStackp);
    continue;

  endFindChildOrNodeLabel:
    if (GENERICSTACK_IS_SHORT(marpaWrapperAsfp->orNodeInUseStackp, (size_t) childOrNodei) &&
        GENERICSTACK_GET_SHORT(marpaWrapperAsfp->orNodeInUseStackp, (size_t) childOrNodei)) {
      *factoringFinishbp = 0;
      goto done;
    }

    if ((! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, (size_t) workOrNodeIdi)) ||
        (((marpaWrapperAsfOrNode_t *) GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, workOrNodeIdi))->nAndNodel <= 0)) {
      *factoringFinishbp = 0;
      goto done;
    }

    newNookp = _marpaWrapperAsf_nook_newp(marpaWrapperAsfp, childOrNodei, worklistLastl);
    if (childIsCauseb != 0) {
      newNookp->isCauseb = 1;
      workNookp->causeIsExpandedb = 1;
    }
    if (childIsPredecessorb != 0) {
      newNookp->isPredecessorb = 1;
      workNookp->predecessorIsExpandedb = 1;
    }

    GENERICSTACK_PUSH_PTR(marpaWrapperAsfp->factoringStackp, newNookp);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStackp push failure, %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_PUSH_INT(worklistStackp, GENERICSTACK_USED(marpaWrapperAsfp->factoringStackp) - 1);
    if (GENERICSTACK_ERROR(worklistStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "worklistStackp push failure, %s", strerror(errno));
      goto err;
    }
  }

  *factoringFinishbp = 1;

 done:
  GENERICSTACK_FREE(worklistStackp);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *factoringFinishbp=%d", (int) *factoringFinishbp);
  return 1;

 err:
  GENERICSTACK_FREE(worklistStackp);
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_factoring_iterateb(marpaWrapperAsf_t *marpaWrapperAsfp, short *factoringIteratebp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_factoring_iterateb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfNook_t    *topNookp;
  marpaWrapperAsfNook_t    *parentNookp;
  int                       stackIxOfParentNooki;
  int                       orNodei;

  while (1) {
    if (GENERICSTACK_USED(marpaWrapperAsfp->factoringStackp) <= 0) {
      _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp);
      *factoringIteratebp = 0;
      goto done;
    }

    topNookp = GENERICSTACK_POP_PTR(marpaWrapperAsfp->factoringStackp);
    if (_marpaWrapperAsf_nook_incrementb(marpaWrapperAsfp, topNookp) != 0) {
      break;
    }

    /* Could not iterate */
    /* "Dirty" the corresponding bits in the parent and pop this nook */
    stackIxOfParentNooki = topNookp->parentOrNodeIdi;
    if (stackIxOfParentNooki >= 0) {
      if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->factoringStackp, (size_t) stackIxOfParentNooki)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "No nook at indice %d of factoringStackp", stackIxOfParentNooki);
	goto err;
      }
      parentNookp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->factoringStackp, (size_t) stackIxOfParentNooki);
      if (topNookp->isCauseb != 0) {
	parentNookp->causeIsExpandedb = 0;
      }
      if (topNookp->isPredecessorb != 0) {
	parentNookp->predecessorIsExpandedb = 0;
      }
    }
    orNodei = topNookp->orNodeIdi;
    GENERICSTACK_SET_PTR(marpaWrapperAsfp->orNodeInUseStackp, NULL, orNodei);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->orNodeInUseStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "failure to set NULL at indice %d of orNodeInUseStackp", orNodei);
      goto err;
    }
    GENERICSTACK_POP_PTR(marpaWrapperAsfp->factoringStackp);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "failure to pop from factoringStackp");
      goto err;
    }
  }

  *factoringIteratebp = 1;

 done:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *factoringFinishbp=%d", (int) *factoringIteratebp);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t **stackpp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_glade_id_factorsb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  genericStack_t           *stackp                  = NULL;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  genericStack_t           *andNodeIdStackp         = NULL;
  genericStack_t           *causeNidsStackp         = NULL;
  int                      *causeNidsp              = NULL;
  size_t                    factorIxl;
  marpaWrapperAsfNook_t    *nookp;
  size_t                    orNodeIdl;
  marpaWrapperAsfOrNode_t  *orNodep;
  size_t                    nAndNodel;
  int                      *andNodep;
  int                       choicei;
  marpaWrapperAsfNidset_t  *baseNidsetp;
  int                       gladeIdi;
  marpaWrapperAsfGlade_t   *gladep;
  
  if (marpaWrapperAsfp->factoringStackp == NULL) {
    goto done;
  }

  GENERICSTACK_NEW(stackp);
  if (GENERICSTACK_ERROR(stackp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize stackp");
    goto err;
  }
  
  for (factorIxl = 0; factorIxl <= GENERICSTACK_USED(marpaWrapperAsfp->factoringStackp)-1; factorIxl++) {
    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->factoringStackp, factorIxl)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Not a pointer at indice %d of factoringStackp", factorIxl);
      goto err;
    }
    nookp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->factoringStackp, factorIxl);
    if (_marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsfp, nookp) == 0) {
      continue;
    }

    orNodeIdl = (size_t) nookp->orNodeIdi;
    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, orNodeIdl)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Not a pointer at indice %d of orNodeStackp", orNodeIdl);
      goto err;
    }
    orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, orNodeIdl);
    nAndNodel = orNodep->nAndNodel;
    andNodep = orNodep->andNodep;
    GENERICSTACK_NEW(andNodeIdStackp);
    if (GENERICSTACK_ERROR(andNodeIdStackp)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize andNodeIdStackp");
      goto err;
    }
    for (choicei = nookp->firstChoicei; choicei <= nookp->lastChoicei; choicei++) {
      if ((choicei < 0) || ((size_t) choicei > nAndNodel)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "andNode %d out of range [%d..%d[", choicei, 0, (int) nAndNodel);
	goto err;
      }
      GENERICSTACK_PUSH_INT(andNodeIdStackp, andNodep[choicei]);
    }
    if (_marpaWrapperAsf_and_nodes_to_cause_nidsp(marpaWrapperAsfp, andNodeIdStackp, &causeNidsStackp) == 0) {
      goto err;
    }
    GENERICSTACK_FREE(andNodeIdStackp);

    if (GENERICSTACK_USED(causeNidsStackp) > 0) {
      causeNidsp = NULL;
    } else {
      causeNidsp = malloc(GENERICSTACK_USED(causeNidsStackp) * sizeof(int));
      if (causeNidsp == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
	goto err;
      } else {
	size_t i;

	for (i = 0; i < GENERICSTACK_USED(causeNidsStackp); i++) {
	  causeNidsp[i] = GENERICSTACK_GET_INT(causeNidsStackp, i);
	}
      }
    }
    baseNidsetp = _marpaWrapperAsf_nidset_obtainb(marpaWrapperAsfp, GENERICSTACK_USED(causeNidsStackp), causeNidsp);
    if (causeNidsp != NULL) {
      free(causeNidsp);
      causeNidsp = NULL;
    }
    if (baseNidsetp == NULL) {
      goto err;
    }

    gladeIdi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, (size_t) gladeIdi)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No glade at indice %d of gladeStackp", (int) gladeIdi);
      goto err;
    }
    gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, (size_t) gladeIdi);
    gladep->registeredb = 1;
    GENERICSTACK_PUSH_INT(stackp, gladeIdi);
    if (GENERICSTACK_ERROR(stackp)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Failure to push in stackp");
      goto err;
    }

  }
  
 done:
  *stackpp = stackp;
  GENERICSTACK_FREE(andNodeIdStackp);
  GENERICSTACK_FREE(causeNidsStackp);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *stackpp=%p", *stackpp);
  return 1;

 err:
  GENERICSTACK_FREE(stackp);
  GENERICSTACK_FREE(andNodeIdStackp);
  GENERICSTACK_FREE(causeNidsStackp);
  if (causeNidsp != NULL) {
    free(causeNidsp);
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
size_t _marpaWrapperAsf_indAndNodesl(void *userDatavp, genericStackItemType_t itemType, void *p)
/****************************************************************************/
{
  return (* ((int *) p)) % MARPAWRAPPERASF_NODE_HASH_SIZE;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_cmpAndNodesb(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2)
/****************************************************************************/
{
  return ((* ((int *) p1)) == (* ((int *) p2))) ? 1 : 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_and_nodes_to_cause_nidsp(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *andNodeIdStackp, genericStack_t **causeNidsStackpp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_and_nodes_to_cause_nidsp";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  genericStack_t           *causeNidsStackp         = NULL;
  genericHash_t            *causesHashp             = NULL;
  size_t                    andNodeIdl;
  int                       andNodeIdi;
  int                       causeNidi;
  int                       goti;
  short                     findResultb;

  GENERICSTACK_NEW(causeNidsStackp);
  if (GENERICSTACK_ERROR(causeNidsStackp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize causeNidsStackp");
    goto err;
  }
  
  GENERICHASH_NEW(causesHashp, _marpaWrapperAsf_indAndNodesl, _marpaWrapperAsf_cmpAndNodesb, 1);
  if (GENERICHASH_ERROR(causesHashp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize causesHashp");
    goto err;
  }

  for (andNodeIdl = 0; andNodeIdl < GENERICSTACK_USED(andNodeIdStackp); andNodeIdl++) {
    if (! GENERICSTACK_IS_INT(andNodeIdStackp, andNodeIdl)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Not an int at indice %d andNodeIdStackp", (int) andNodeIdl);
      goto err;
    }
    andNodeIdi = GENERICSTACK_GET_INT(andNodeIdStackp, andNodeIdl);
    causeNidi = (int) _marpa_b_and_node_cause(marpaBocagep, andNodeIdi);
    if (causeNidi < 0) {
      causeNidi = _marpaWrapperAsf_and_node_to_nidi(andNodeIdi);
    }
    GENERICHASH_FIND(causesHashp, marpaWrapperAsfp, INT, causeNidi, findResultb, goti);
    if (GENERICHASH_ERROR(causesHashp)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Error looking into causesHashp");
      goto err;
    }
    if (! findResultb) {
      GENERICSTACK_PUSH_INT(causeNidsStackp, causeNidi);
      if (GENERICSTACK_ERROR(causeNidsStackp)) {
	MARPAWRAPPER_ERROR(genericLoggerp, "Failure to pushd to causeNidsStackp");
	goto err;
      }
      GENERICHASH_SET(causesHashp, marpaWrapperAsfp, INT, causeNidi);
    }
  }
  
  GENERICHASH_FREE(causesHashp);
  *causeNidsStackpp = causeNidsStackp;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *causeNidsStackpp=%p", *causeNidsStackpp);
  return 1;

 err:
  GENERICHASH_FREE(causesHashp);
  GENERICSTACK_FREE(causeNidsStackp);
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_next_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, int nidOfChoicePointi, short *factoringbp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_next_factoringb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  short                     factoringb              = 0;

  if (marpaWrapperAsfp->factoringStackp == NULL) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Attempt to iterate factoring of uninitialized checkpoint");
      goto err;
  }

  while (1) {
    short factoringIterateb;
    short factoringFinishb;
    
    if (_marpaWrapperAsf_factoring_iterateb(marpaWrapperAsfp, &factoringIterateb) == 0) {
      goto err;
    }
    if (factoringIterateb == 0) {
      break;
    }
    if (_marpaWrapperAsf_factoring_finishb(marpaWrapperAsfp, nidOfChoicePointi, &factoringFinishb) == 0) {
      goto err;
    }
    if (factoringFinishb != 0) {
      factoringb = 1;
      break;
    }
  }

  *factoringbp = factoringb;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *factorinbp=%d", *factoringbp);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;

}

/****************************************************************************/
static inline short _marpaWrapperAsf_glade_is_visitedb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_is_visitedb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  short                    rcb            = 0;
  marpaWrapperAsfGlade_t  *gladep;

  if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, (size_t) gladeIdi)) {
    gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, (size_t) gladeIdi);
    rcb = gladep->visitedb;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", rcb);
  return rcb;
}

/****************************************************************************/
static inline void _marpaWrapperAsf_glade_visited_clearb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_visited_clearb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  size_t                   i;
  marpaWrapperAsfGlade_t  *gladep;

  if (gladeIdip != NULL) {
    if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, (size_t) *gladeIdip)) {
      gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, (size_t) *gladeIdip);
      gladep->visitedb = 0;
    }
  } else {
    for (i = 0; i < GENERICSTACK_USED(marpaWrapperAsfp->gladeStackp); i++) {
      if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, i)) {
	gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, i);
	gladep->visitedb = 0;
      }
    }
  }
}

/****************************************************************************/
static inline short _marpaWrapperAsf_glade_symch_countb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, size_t *countlp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_symch_countb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  short                    rcb            = 0;
  marpaWrapperAsfGlade_t  *gladep;

  gladep = _marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, gladeIdi);
  if (gladep == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No glade found for glade ID %d", gladeIdi);
    goto err;
  }

  *countlp = GENERICSTACK_USED(gladep->symchesStackp);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d, *countlp=%d", (int) *countlp);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_glade_symbol_idb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, int *symbolIdip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_symbol_idb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  short                    rcb            = 0;
  int                      nid0;
  marpaWrapperAsfNidset_t *nidsetp;

  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->nidsetStackp, (size_t) gladeIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No glade found for glade ID %d", gladeIdi);
    goto err;
  }
  nidsetp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->nidsetStackp, (size_t) gladeIdi);

  if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, nidsetp, 0, &nid0) == 0) {
    goto err;
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, symbolIdi=%d", (int) *symbolIdip);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
marpaWrapperGrammar_t *marpaWrapperAsf_grammarp(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_and_nodes_to_cause_nidsp";
  genericLogger_t          *genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp;

  if (marpaWrapperAsfp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  genericLoggerp          = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperGrammarp);
  return marpaWrapperGrammarp;
}

/****************************************************************************/
short marpaWrapperAsf_rh_lengthb(marpaWrapperAsfTraverser_t *traverserp, size_t *lengthlp)
/****************************************************************************/
{
  const static char         funcs[] = "marpaWrapperAsf_rh_lengthb";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  marpaWrapperAsfGlade_t   *gladep;
  int                       symchIxi;
  genericStack_t           *factoringsStackp;
  int                       ruleIdi;
  int                       factoringIxi;
  genericStack_t           *factoringStackp;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  symchIxi = traverserp->symchIxi;
  if (! GENERICSTACK_IS_PTR(gladep->symchesStackp, symchIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No symch at indice %d of current glade", symchIxi);
    goto err;
  }
  factoringsStackp = GENERICSTACK_GET_PTR(gladep->symchesStackp, symchIxi);
  if (! GENERICSTACK_IS_INT(factoringsStackp, 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not an integer at indice 0 of factoringsStackp", 0);
    goto err;
  }
  ruleIdi = GENERICSTACK_GET_INT(factoringsStackp, 0);
  if (ruleIdi < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "%s called for a token -- that is not allowed", funcs);
    goto err;
  }
  factoringIxi = traverserp->factoringIxi;
  if (! GENERICSTACK_IS_PTR(factoringsStackp, factoringIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not an pointer at indice %d of factoringsStackp", factoringIxi);
    goto err;
  }
  factoringStackp = GENERICSTACK_GET_PTR(factoringsStackp, factoringIxi);

  *lengthlp = GENERICSTACK_USED(factoringStackp);
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, lengthl = %d", (int) *lengthlp);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
genericStack_t *marpaWrapperAsf_rh_valuesStackp(marpaWrapperAsfTraverser_t *traverserp)
/****************************************************************************/
{
  const static char         funcs[] = "marpaWrapperAsf_rh_valuesStackp";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  size_t                    lengthl;
  size_t                    i;
  void                     *valuep;

  if (traverserp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  GENERICSTACK_FREE(marpaWrapperAsfp->lastValuesStackp);
  GENERICSTACK_INIT(marpaWrapperAsfp->lastValuesStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->lastValuesStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "lastValuesStackp initialization error, %s", strerror(errno));
    goto err;
  }

  if (marpaWrapperAsf_rh_lengthb(traverserp, &lengthl) == 0) {
    goto err;
  }

  for (i = 0; i < lengthl; i++) {
    /* Note that we impose that a value is not NULL */
    valuep = marpaWrapperAsf_rh_valuep(traverserp, i);
    if (valuep == NULL) {
      goto err;
    }
    GENERICSTACK_PUSH_PTR(marpaWrapperAsfp->lastValuesStackp, valuep);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->lastValuesStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "lastValuesStackp push error, %s", strerror(errno));
      goto err;
    }
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperAsfp->lastValuesStackp);
  return marpaWrapperAsfp->lastValuesStackp;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
void *marpaWrapperAsf_rh_valuep(marpaWrapperAsfTraverser_t *traverserp, size_t rhIxi)
/****************************************************************************/
{
  const static char           funcs[]         = "marpaWrapperAsf_rh_valueb";
  void                       *valuep          = NULL;
  marpaWrapperAsfTraverser_t *childTraverserp = NULL;
  marpaWrapperAsf_t          *marpaWrapperAsfp;
  marpaWrapperAsfGlade_t     *gladep;
  genericLogger_t            *genericLoggerp;
  int                         symchIxi;
  genericStack_t             *factoringsStackp;
  int                         ruleIdi;
  int                         factoringIxi;
  genericStack_t             *factoringStackp;
  size_t                      maxRhixl;
  int                         maxRhixi;
  marpaWrapperAsfGlade_t     *downGladep;
  int                         downGladeIdi;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  symchIxi = traverserp->symchIxi;
  if (! GENERICSTACK_IS_PTR(gladep->symchesStackp, symchIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No symch at indice %d of current glade", symchIxi);
    goto err;
  }
  factoringsStackp = GENERICSTACK_GET_PTR(gladep->symchesStackp, symchIxi);
  if (! GENERICSTACK_IS_INT(factoringsStackp, 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not an integer at indice 0 of factoringsStackp", 0);
    goto err;
  }
  ruleIdi = GENERICSTACK_GET_INT(factoringsStackp, 0);
  if (ruleIdi < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "%s called for a token -- that is not allowed", funcs);
    goto err;
  }
  factoringIxi = traverserp->factoringIxi;
  if (! GENERICSTACK_IS_PTR(factoringsStackp, factoringIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not an pointer at indice %d of factoringsStackp", factoringIxi);
    goto err;
  }
  factoringStackp = GENERICSTACK_GET_PTR(factoringsStackp, factoringIxi);

  maxRhixl = GENERICSTACK_USED(factoringStackp);
  /*
   * Nullables have no RHS
   */
  if (maxRhixl <= 0) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Nullable case");
    goto ok;
  }
  maxRhixi = (int) (maxRhixl - 1);
  if (rhIxi > maxRhixi) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "rhIxi should be in range [0..%d]", maxRhixi);
    goto err;
  }
  if (! GENERICSTACK_IS_INT(factoringStackp, rhIxi)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Not an int in factoringStackp");
    goto err;
  }
  downGladeIdi = GENERICSTACK_GET_INT(factoringStackp, rhIxi);
  if (GENERICSTACK_IS_PTR(traverserp->valuep, downGladeIdi)) {
    /* Already memoized */
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Memoized value");
    valuep = GENERICSTACK_GET_PTR(traverserp->valuep, downGladeIdi);
    goto ok;
  }
  downGladep = _marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, downGladeIdi);
  if (downGladep == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No glade found for glade ID %d", downGladeIdi);
    goto err;
  }
  /*
   * Do a shallow a clone
   */
  childTraverserp = malloc(sizeof(marpaWrapperAsfTraverser_t));
  if (childTraverserp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc error, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(childTraverserp->valuep);
  if (GENERICSTACK_ERROR(childTraverserp->valuep)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "childTraverserp->valuep initialization failure error, %s", strerror(errno));
    goto err;   
  }
  childTraverserp->marpaWrapperAsfp = marpaWrapperAsfp;
  childTraverserp->gladep           = downGladep;
  childTraverserp->symchIxi         = 0;
  childTraverserp->factoringIxi     = 0;
  valuep = marpaWrapperAsfp->traverserCallbackp(childTraverserp, marpaWrapperAsfp->userDatavp);
  GENERICSTACK_FREE(traverserp->valuep);
  free(traverserp);
  traverserp = NULL;

  if (valuep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "The ASF traversing method returned undef -- that is not allowed");
    goto err;   
  }

  GENERICSTACK_SET_PTR(traverserp->valuep, valuep, downGladeIdi);
  if (GENERICSTACK_ERROR(traverserp->valuep)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to set value in traverser", strerror(errno));
    goto err;   
  }

ok:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", valuep);
  return valuep;

 err:
  if (childTraverserp != NULL) {
    GENERICSTACK_FREE(childTraverserp->valuep);
    free(childTraverserp);
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
genericStack_t *marpaWrapperAsf_rh_allChoicesStackp(marpaWrapperAsfTraverser_t *traverserp)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_rh_allChoicesStackp";
  genericStack_t           *emptyStackp      = NULL;
  genericStack_t           *newResultsStackp = NULL;
  genericStack_t           *tmpp             = NULL;
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  genericStack_t           *valuesStackp;
  size_t                    rhIxl;
  genericStack_t           *childValuep;
  size_t                    oldResultl;
  genericStack_t           *oldResultp;
  size_t                    newValuel;
  void                     *newValuep;
  size_t                    tmpl;

  if (traverserp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  valuesStackp = marpaWrapperAsf_rh_valuesStackp(traverserp);
  if (valuesStackp == NULL) {
    goto err;
  }

  /*
   * Initialize lastAllChoicesStackp to an empty cartesian product
   */
  GENERICSTACK_FREE(marpaWrapperAsfp->lastAllChoicesStackp);
  GENERICSTACK_INIT(marpaWrapperAsfp->lastAllChoicesStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->lastAllChoicesStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "marpaWrapperAsfp->lastAllChoicesStackp initialization error, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(emptyStackp);
  if (GENERICSTACK_ERROR(emptyStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "emptyStackp initialization error, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_PUSH_PTR(marpaWrapperAsfp->lastAllChoicesStackp, emptyStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->lastAllChoicesStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "marpaWrapperAsfp->lastAllChoicesStackp push error, %s", strerror(errno));
    goto err;
  }

  for (rhIxl = 0; rhIxl < GENERICSTACK_USED(valuesStackp); rhIxl++) {
    GENERICSTACK_NEW(newResultsStackp);
    if (GENERICSTACK_ERROR(newResultsStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "newResultsStackp initialization error, %s", strerror(errno));
      goto err;
    }
    for (oldResultl = 0; oldResultl < GENERICSTACK_USED(marpaWrapperAsfp->lastAllChoicesStackp); oldResultl++) {
      if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->lastAllChoicesStackp, oldResultl)) {
	MARPAWRAPPER_ERROR(genericLoggerp, "Not a pointer in marpaWrapperAsfp->lastAllChoicesStackp");
	goto err;
      }
      oldResultp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->lastAllChoicesStackp, oldResultl);

      if (! GENERICSTACK_IS_PTR(valuesStackp, rhIxl)) {
	MARPAWRAPPER_ERROR(genericLoggerp, "Not a pointer in valuesStackp");
	GENERICSTACK_FREE(newResultsStackp);
	goto err;
      }
      childValuep = GENERICSTACK_GET_PTR(valuesStackp, rhIxl);

      GENERICSTACK_INIT(tmpp);
      if (GENERICSTACK_ERROR(tmpp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "tmpp initialization failure, %s", strerror(errno));
	goto err;
      }
      for (newValuel = 0; newValuel < GENERICSTACK_USED(childValuep); newValuel++) {
	newValuep = GENERICSTACK_GET_PTR(childValuep, newValuel);
	for (tmpl = 0; tmpl < GENERICSTACK_USED(oldResultp); tmpl++) {
	  GENERICSTACK_PUSH_PTR(tmpp, GENERICSTACK_GET_PTR(oldResultp, tmpl));
	  if (GENERICSTACK_ERROR(tmpp)) {
	    MARPAWRAPPER_ERRORF(genericLoggerp, "tmpp push failure, %s", strerror(errno));
	    goto err;
	  }
	}
	GENERICSTACK_PUSH_PTR(tmpp, newValuep);
	if (GENERICSTACK_ERROR(tmpp)) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "tmpp push failure, %s", strerror(errno));
	  goto err;
	}
	GENERICSTACK_PUSH_PTR(newResultsStackp, tmpp);
	if (GENERICSTACK_ERROR(newResultsStackp)) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "newResultsStackp push failure, %s", strerror(errno));
	  goto err;
	}
      }
    }

    _marpaWrapperAsf_lastAllChoices_freev(marpaWrapperAsfp);
    marpaWrapperAsfp->lastAllChoicesStackp = newResultsStackp;
    newResultsStackp = NULL;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperAsfp->lastAllChoicesStackp);
  return marpaWrapperAsfp->lastAllChoicesStackp;

 err:
  GENERICSTACK_FREE(emptyStackp);
  GENERICSTACK_FREE(tmpp);
  GENERICSTACK_FREE(newResultsStackp);
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

