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

static inline void                       _marpaWrapperAsf_orNode_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_glade_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline short                      _marpaWrapperAsf_peak(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline int                        _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi(const void *p1, const void *p2);
static inline short                      _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, size_t countl, int *idip);
static inline char                      *_marpaWrapperAsf_stringBuilders(char *fmts, ...);
static inline char                      *_marpaWrapperAsf_stringBuilder_aps(char *fmts, va_list ap);
static inline short                      _marpaWrapperAsf_cmpIntsetFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);
static inline unsigned long              _marpaWrapperAsf_djb2(unsigned char *str);
static inline int                        _marpaWrapperAsf_idCmpi(const void *p1, const void *p2);

static inline marpaWrapperAsfIdset_t    *_marpaWrapperAsf_idset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, size_t countl, int *idip);
static inline int                       *_marpaWrapperAsf_idset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_idset_idi_by_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp, int ixi);
static inline size_t                     _marpaWrapperAsf_idset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_idset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline char                      *_marpaWrapperAsf_idset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_idset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete);

static inline marpaWrapperAsfNidset_t   *_marpaWrapperAsf_nidset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t countl, int *idip);
static inline int                       *_marpaWrapperAsf_nidset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_nidset_idi_by_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi);
static inline size_t                     _marpaWrapperAsf_nidset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_nidset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline char                      *_marpaWrapperAsf_nidset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_nidset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *stackp);

/* Generic nidset/powerset methods */

static inline marpaWrapperAsfPowerset_t *_marpaWrapperAsf_powerset_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t countl, int *idip);
static inline int                       *_marpaWrapperAsf_powerset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_powerset_idi_by_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi);
static inline size_t                     _marpaWrapperAsf_powerset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_powerset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline char                      *_marpaWrapperAsf_powerset_showb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_powerset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *stackp);

static inline marpaWrapperAsfNook_t     *_marpaWrapperAsf_nook_newp(marpaWrapperAsf_t *marpaWrapperAsfp, int orNodeIdi, int parentOrNodeIdi);
static inline void                       _marpaWrapperAsf_nook_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline short                      _marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp);
static inline short                      _marpaWrapperAsf_setLastChoiceb(marpaWrapperAsf_t *marpaWrapperAsfp, int **choiceipp, marpaWrapperAsfNook_t *nookp);
static inline short                      _marpaWrapperAsf_nook_incrementb(marpaWrapperAsf_t *marpaWrapperAsfp, int **choiceipp, marpaWrapperAsfNook_t *nookp);

/* Specific to nid */
static inline int                        _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
static inline int                        _marpaWrapperAsf_and_node_to_nidi(int idi);
static inline int                        _marpaWrapperAsf_nid_to_and_nodei(int idi);
static inline int                        _marpaWrapperAsf_sourceDataCmpi(const void *p1, const void *p2);
static inline int                        _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
/* Specific to powerset */
static inline marpaWrapperAsfNidset_t   *_marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfPowerset_t *powersetp, int ixi);
static inline short                      _marpaWrapperAsf_glade_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t gladei);

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
  int                                        *orNodeAndNodeIdip;
  
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
  marpaWrapperAsfp->nookStackp              = NULL;
  marpaWrapperAsfp->lastNidsetp             = NULL;
  marpaWrapperAsfp->lastPowersetp           = NULL;
  marpaWrapperAsfp->gladeStackp             = NULL;
  marpaWrapperAsfp->factoringStackp         = NULL;
  marpaWrapperAsfp->orNodeInUseStackp       = NULL;
  marpaWrapperAsfp->nextIntseti             = 0;

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

  GENERICSTACK_NEW(marpaWrapperAsfp->nookStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->nookStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Nook stack initialization error, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(marpaWrapperAsfp->gladeStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->gladeStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "glade stack initialization error, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(marpaWrapperAsfp->factoringStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoring stack initialization error, %s", strerror(errno));
    goto err;
  }

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
short marpaWrapperAsf_traverseb(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperAsf_traverseb";
  genericLogger_t  *genericLoggerp = NULL;

  if (marpaWrapperAsfp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
void marpaWrapperAsf_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[] = "marpaWrapperAsf_freev";
  genericLogger_t         *genericLoggerp;
  size_t                   orNodeAndNodeIdi;

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
    _marpaWrapperAsf_orNode_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing intset hash");
    GENERICHASH_FREE(marpaWrapperAsfp->intsetHashp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Nidset stack");
    _marpaWrapperAsf_nidset_freev(marpaWrapperAsfp, marpaWrapperAsfp->nidsetStackp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Powerset stack");
    _marpaWrapperAsf_powerset_freev(marpaWrapperAsfp, marpaWrapperAsfp->powersetStackp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Nook stack");
    _marpaWrapperAsf_nook_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing glade stack");
    _marpaWrapperAsf_glade_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing factoring stack");
    GENERICSTACK_FREE(marpaWrapperAsfp->factoringStackp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing orNodeInUse stack");
    GENERICSTACK_FREE(marpaWrapperAsfp->orNodeInUseStackp);
    
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperAsfp);
    free(marpaWrapperAsfp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_orNode_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_orNode_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  genericStack_t          *stackp         = marpaWrapperAsfp->orNodeStackp;
  size_t                   orNodeUsedl;
  size_t                   i;
  marpaWrapperAsfOrNode_t *orNodep;

  orNodeUsedl = GENERICSTACK_USED(stackp);
  for (i = 0; i < orNodeUsedl; i++) {
    if (GENERICSTACK_IS_PTR(stackp, i)) {
      orNodep = GENERICSTACK_GET_PTR(stackp, i);
      if (orNodep->andNodep != NULL) {
	free(orNodep->andNodep);
      }
    }
  }
  GENERICSTACK_FREE(stackp);
}

/****************************************************************************/
static inline void _marpaWrapperAsf_glade_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  genericStack_t          *stackp         = marpaWrapperAsfp->gladeStackp;
  size_t                   gladeUsedl;
  size_t                   i;
  marpaWrapperAsfGlade_t  *gladep;

  gladeUsedl = GENERICSTACK_USED(stackp);
  for (i = 0; i < gladeUsedl; i++) {
    if (GENERICSTACK_IS_PTR(stackp, i)) {
      gladep = GENERICSTACK_GET_PTR(stackp, i);
      if (gladep->symbolip != NULL) {
	free(gladep->symbolip);
      }
    }
  }
  GENERICSTACK_FREE(stackp);
}

/****************************************************************************/
static inline short _marpaWrapperAsf_peak(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_peak";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  marpaWrapperAsfGlade_t  *gladep         = NULL;
  int                      augmentOrNodeIdi;
  int                      augmentAndNodeIdi;
  int                      startOrNodeIdi;
  marpaWrapperAsfNidset_t *baseNidsetp;
  marpaWrapperAsfOrNode_t *orNodep;

  augmentOrNodeIdi = (int) _marpa_b_top_or_node(marpaWrapperAsfp->marpaBocagep);
  if (augmentOrNodeIdi < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperAsfp->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->orNodeStackp, augmentOrNodeIdi)) {
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

  gladep = malloc(sizeof(marpaWrapperAsfGlade_t));
  if (gladep == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }
  gladep->idi         = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
  gladep->nSymboll    = 0;
  gladep->symbolip    = NULL;
  gladep->registeredb = 1;
  GENERICSTACK_SET_PTR(marpaWrapperAsfp->gladeStackp, gladep, 0);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->gladeStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "glade stack failure: %s", strerror(errno));
    goto err;
  }

  /* We cannot obtain the flag if it is not registered */
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1", 0);
  return 1;

 err:
  if (gladep != NULL) {
    if (gladep->symbolip != NULL) {
      free(gladep->symbolip);
    }
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
  genericStack_t          *stackp         = (idsete == MARPAWRAPPERASFIDSET_NIDSET) ? marpaWrapperAsfp->nidsetStackp : marpaWrapperAsfp->powersetStackp;
  char                    *idsets         = marpaWrapperAsfIdsets[idsete];
  marpaWrapperAsfIdset_t  *idsetp         = NULL;
  int                      intsetIdi;
  size_t                   i;

  if (_marpaWrapperAsf_intsetIdb(marpaWrapperAsfp, &intsetIdi, countl, idip) == 0) {
    goto err;
  }

  if (GENERICSTACK_IS_PTR(stackp, intsetIdi)) {
    idsetp = GENERICSTACK_GET_PTR(stackp, intsetIdi);
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
      GENERICSTACK_SET_PTR(stackp, idsetp, intsetIdi);
      if (GENERICSTACK_ERROR(stackp)) {
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
static inline int _marpaWrapperAsf_idset_idi_by_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp, int ixi)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi_by_ixi";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %d", marpaWrapperAsfIdsets[idsete], idsetp->idip[ixi]);
  return idsetp->idip[ixi];
}

/****************************************************************************/
static inline size_t _marpaWrapperAsf_idset_countl(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi_by_ixi";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %d", marpaWrapperAsfIdsets[idsete], (int) idsetp->countl);
  return idsetp->countl;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_idset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi_by_ixi";
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
    showNexts = _marpaWrapperAsf_stringBuilders("%s %d", shows, _marpaWrapperAsf_idset_idi_by_ixi(marpaWrapperAsfp, idsete, idsetp, (int) idl));
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
  genericStack_t          *stackp         = (idsete == MARPAWRAPPERASFIDSET_NIDSET) ? marpaWrapperAsfp->nidsetStackp : marpaWrapperAsfp->powersetStackp;
  char                    *idsets         = marpaWrapperAsfIdsets[idsete];
  size_t                   idsetUsedl;
  size_t                   i;
  marpaWrapperAsfIdset_t  *idsetp;

  idsetUsedl = GENERICSTACK_USED(stackp);
  /* Every idset may have an array inside */
  for (i = 0; i < idsetUsedl; i++) {
    if (GENERICSTACK_IS_PTR(stackp, i)) {
      idsetp = GENERICSTACK_GET_PTR(stackp, i);
      if (idsetp->idip != NULL) {
	free(idsetp->idip);
      }
      free(idsetp);
    }
  }
  GENERICSTACK_FREE(stackp);
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
static inline int _marpaWrapperAsf_nidset_idi_by_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi_by_ixi(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp, ixi);
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
static inline void _marpaWrapperAsf_nidset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *stackp)
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
static inline int _marpaWrapperAsf_powerset_idi_by_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi_by_ixi(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp, ixi);
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
static inline void _marpaWrapperAsf_powerset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *stackp)
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

  nookp->orNodeIdi       = orNodeIdi;
  nookp->firstChoicei    = 0;
  nookp->lastChoicei     = 0;
  nookp->parentOrNodeIdi = parentOrNodeIdi;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nookp);
  return NULL;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline void _marpaWrapperAsf_nook_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_nook_freeb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  genericStack_t          *stackp         = marpaWrapperAsfp->nookStackp;
  size_t                   nookUsedl;
  size_t                   i;
  marpaWrapperAsfIdset_t  *nookp;

  nookUsedl = GENERICSTACK_USED(stackp);
  for (i = 0; i < nookUsedl; i++) {
    if (GENERICSTACK_IS_PTR(stackp, i)) {
      nookp = GENERICSTACK_GET_PTR(stackp, i);
      free(nookp);
    }
  }
  GENERICSTACK_FREE(stackp);
}

/****************************************************************************/
static inline short _marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]                  = "_marpaWrapperAsf_setLastChoiceb";
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
  genericStack_t          *orNodeStackp       = marpaWrapperAsfp->orNodeStackp;
  int                      orNodeIdi          = nookp->orNodeIdi;
  int                      choicei            = nookp->firstChoicei;
  short                    rcb                = 1;
  marpaWrapperAsfOrNode_t *orNodep;

  if (! GENERICSTACK_IS_PTR(orNodeStackp, orNodeIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No entry in orNode stack at indice %d", orNodeIdi);
    goto err;
  }
  orNodep = GENERICSTACK_GET_PTR(orNodeStackp, orNodeIdi);

  if (choicei >= orNodep->nAndNodel) {
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
	if (++choicei >= orNodep->nAndNodel) {
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
static inline short _marpaWrapperAsf_nook_incrementb(marpaWrapperAsf_t *marpaWrapperAsfp, int **choiceipp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_setLastChoiceb";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->genericLoggerp;

  nookp->firstChoicei = nookp->lastChoicei + 1;

  if (_marpaWrapperAsf_setLastChoiceb(marpaWrapperAsfp, choiceipp, nookp) == 0) {
    goto err;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
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
  if (ixi < countl) {
    idi = _marpaWrapperAsf_powerset_idi_by_ixi(marpaWrapperAsfp, powersetp, ixi);
    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->nidsetStackp, idi)) {
      MARPAWRAPPER_WARNF(genericLoggerp, funcs, "Powerset at indice %d references a non-existing nidset at indice %d", ixi, idi);
    } else {
      nidsetp = GENERICSTACK_GET_PTR(marpaWrapperAsfp->nidsetStackp, idi);
    }
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nidsetp);
  return nidsetp;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_glade_obtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t gladei)
/****************************************************************************/
{
  const static char            funcs[]                = "_marpaWrapperAsf_glade_obtainb";
  genericLogger_t             *genericLoggerp         = marpaWrapperAsfp->genericLoggerp;
  genericStack_t              *gladeStackp            = marpaWrapperAsfp->gladeStackp;
  marpaWrapperAsfSourceData_t *sourceDatap            = NULL;
  size_t                       nidWithCurrentSortIxil = 0;
  int                         *nidWithCurrentSortIxip = NULL;
  size_t                       symchIdil              = 0;
  int                         *symchIdip              = NULL;
  short                        thisNidEndb            = 0;
  size_t                       nSymboll               = 0;
  int                         *symbolip               = NULL;
  size_t                       nSourceDatal;
  marpaWrapperAsfGlade_t      *gladep;
  marpaWrapperAsfNidset_t     *baseNidsetp;
  int                          choicepointi;
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
  marpaWrapperAsfNidset_t     *symchNidsetp;
  int                          choicepointNidi;
  int                          symchRuleIdi;

  if ((! GENERICSTACK_IS_PTR(gladeStackp, gladei)) || ((gladep = GENERICSTACK_GET_PTR(gladeStackp, gladei))->registeredb == 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Attempt to use an invalid glade, one whose ID is %d", gladei);
    goto err;
  }

  /* Setup the glade if not already done */
  if (gladep->nSymboll <= 0) {
    if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->nidsetStackp, gladei)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No nidset at glade id %d", gladei);
      goto err;
    }
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
    sourceNidi                  = _marpaWrapperAsf_nidset_idi_by_ixi(marpaWrapperAsfp, baseNidsetp, ixi);
    sourceDatap[ixi].sortIxi    = _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsfp, sourceNidi);
    sourceDatap[ixi].sourceNidi = _marpaWrapperAsf_nidset_idi_by_ixi(marpaWrapperAsfp, baseNidsetp, ixi);
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
    if (nidIxi < nSourceDatal) {
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
  /* Reset factoring stack */
  GENERICSTACK_FREE(marpaWrapperAsfp->factoringStackp);
  GENERICSTACK_NEW(marpaWrapperAsfp->factoringStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStack reset failure: %s", strerror(errno));
    goto err;
  }

  /* Check if choicepoint alrady seen ? */
  symchCountl = _marpaWrapperAsf_powerset_countl(marpaWrapperAsfp, choicepointPowsersetp);
  for (symchIxl = 0; symchIxl < symchCountl; symchIxl++) {
    genericStack_t          *factoringsStackp;
    marpaWrapperAsfNidset_t *symchNidsetp;
    size_t                   nidcountl;
    int                      nidixi;
    
    /* Reset factoring stack */
    GENERICSTACK_FREE(marpaWrapperAsfp->factoringStackp);
    GENERICSTACK_NEW(marpaWrapperAsfp->factoringStackp);
    if (GENERICSTACK_ERROR(marpaWrapperAsfp->factoringStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStackp reset failure: %s", strerror(errno));
      goto err;
    }

    symchNidsetp = _marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsfp, choicepointPowsersetp, symchIxl);
    if (symchNidsetp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "symchNidsetp is NULL", strerror(errno));
      goto err;
    }
    choicepointNidi = _marpaWrapperAsf_nidset_idi_by_ixi(marpaWrapperAsfp, symchNidsetp, 0);
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

    /* For a token, rhere will not be multiple factorings or nids, it is assumed, for a token */
    /* From now on, at indice >= 2, factoringsStackp can contain pointers to array */
#define _marpaWrapperAsf_glade_obtainb_free_factoringsStackp		\
    if (GENERICSTACK_USED(factoringsStackp) > 2) {			\
      int factoringsi;							\
      for (factoringsi = 2; factoringsi < GENERICSTACK_USED(factoringsStackp); factoringsi++) {	\
	int *p = GENERICSTACK_GET_PTR(factoringsStackp, factoringsi);	\
	if (p != NULL) {						\
	  free(p);							\
	}								\
      }									\
  }									\
    GENERICSTACK_FREE(factoringsStackp)

    if (symchRuleIdi < 0) {
      marpaWrapperAsfNidset_t *baseNidsetp = _marpaWrapperAsf_nidset_obtainb(marpaWrapperAsfp, 1, &choicepointNidi);
      int                      gladeidi;
      marpaWrapperAsfGlade_t  *gladep;
      int                     *arrayp;

      if (baseNidsetp == NULL) {
	GENERICSTACK_FREE(factoringsStackp);
	goto err;
      }
      gladeidi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
      if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, gladeidi)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "No glade at indice %d of gladeStackp", gladeidi);
	_marpaWrapperAsf_glade_obtainb_free_factoringsStackp;
	goto err;
      }
      gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, gladeidi);
      gladep->registeredb = 1;

      arrayp = malloc(sizeof(int));
      if (arrayp == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc error, %s", strerror(errno));
	_marpaWrapperAsf_glade_obtainb_free_factoringsStackp;
	goto err;
      }
      arrayp[0] = gladeidi;
      GENERICSTACK_PUSH_PTR(factoringsStackp, arrayp);
      if (GENERICSTACK_ERROR(factoringsStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to push factoringsStackp, %s", strerror(errno));
	_marpaWrapperAsf_glade_obtainb_free_factoringsStackp;
	goto err;
      }
      continue;
    }

    symchNidsetp = _marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsfp, choicepointPowsersetp, (int) symchIxl);
    if (symchNidsetp == NULL) {
      MARPAWRAPPER_ERROR(genericLoggerp, "symchNidsetp is NULL");
      _marpaWrapperAsf_glade_obtainb_free_factoringsStackp;
      goto err;
    }
    nidcountl = _marpaWrapperAsf_nidset_countl(marpaWrapperAsfp, symchNidsetp);
    for (nidixi = 0; nidixi < nidcountl; nidixi++) {
      choicepointNidi = _marpaWrapperAsf_nidset_idi_by_ixi(marpaWrapperAsfp, symchNidsetp, nidixi);
    }
  }

  
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

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

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}
