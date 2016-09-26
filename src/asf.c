#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <limits.h> /* For INT_MAX */

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/asf.h"
#include "marpaWrapper/internal/recognizer.h"
#include "marpaWrapper/internal/grammar.h"
#include "marpaWrapper/internal/logging.h"

#ifndef MARPAWRAPPERASF_INTSET_MAXROWS
#define MARPAWRAPPERASF_INTSET_MAXROWS 65536   /* x &0xFFFF */
#endif

#undef MARPAWRAPPERASF_INTSET_MODULO
/* x mod 65536 is x & 0xffff when x is unsigned */
/* #define MARPAWRAPPERASF_INTSET_MODULO (MARPAWRAPPERASF_INTSET_MAXROWS - 1) */
#define MARPAWRAPPERASF_INTSET_MODULO(x) ((x) & 0xFFFF)

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

const static int half_int_max = INT_MAX / 2;
#undef MARPAWRAPPER_MIN
#define MARPAWRAPPER_MIN(a,b) ((a) < half_int_max ? (a) : (b))

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

static inline void                       _marpaWrapperAsf_orNodeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_gladeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                       _marpaWrapperAsf_glade_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfGlade_t *gladep);
static inline void                       _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *symchesStackp);
static inline void                       _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t **factoringStackpp);
static inline void                       _marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *factoringsStackp);
static inline genericStack_t            *_marpaWrapperAsf_factoringStackp_resetb(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t **factoringStackpp);
static inline short                      _marpaWrapperAsf_peakb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip);
static inline int                        _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi(const void *p1, const void *p2);
static inline short                      _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, int counti, int *idip);
static inline char                      *_marpaWrapperAsf_stringBuilders(char *fmts, ...);
static inline char                      *_marpaWrapperAsf_stringBuilder_aps(char *fmts, va_list ap);
static inline int                        _marpaWrapperAsf_idCmpi(const void *p1, const void *p2);

static inline marpaWrapperAsfIdset_t    *_marpaWrapperAsf_idset_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, int counti, int *idip);
static inline int                       *_marpaWrapperAsf_idset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline short                      _marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip);
static inline int                        _marpaWrapperAsf_idset_counti(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_idset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_idset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete);

static inline marpaWrapperAsfNidset_t   *_marpaWrapperAsf_nidset_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, int counti, int *idip);
static inline int                       *_marpaWrapperAsf_nidset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline short                      _marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip);
static inline int                        _marpaWrapperAsf_nidset_counti(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_nidset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_nidset_freev(marpaWrapperAsf_t *marpaWrapperAsfp);

/* Generic nidset/powerset methods */

static inline marpaWrapperAsfPowerset_t *_marpaWrapperAsf_powerset_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, int counti, int *idip);
static inline int                       *_marpaWrapperAsf_powerset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline short                      _marpaWrapperAsf_powerset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip);
static inline int                        _marpaWrapperAsf_powerset_counti(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline int                        _marpaWrapperAsf_powerset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp);
static inline void                       _marpaWrapperAsf_powerset_freev(marpaWrapperAsf_t *marpaWrapperAsfp);

/* Specific to nid */
static inline int                        _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
static inline int                        _marpaWrapperAsf_and_node_to_nidi(int idi);
static inline int                        _marpaWrapperAsf_nid_to_and_nodei(int idi);
static inline int                        _marpaWrapperAsf_sourceDataCmpi(const void *p1, const void *p2);
static inline int                        _marpaWrapperAsf_nid_token_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
static inline int                        _marpaWrapperAsf_nid_symbol_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
static inline int                        _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);
static inline int                        _marpaWrapperAsf_nid_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi);

/* Specific to powerset */
static inline marpaWrapperAsfNidset_t   *_marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfPowerset_t *powersetp, int ixi);
static inline marpaWrapperAsfGlade_t    *_marpaWrapperAsf_glade_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, int gladei);
static inline short                      _marpaWrapperAsf_first_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, int nidOfChoicePointi, short *firstFactoringbp);
static inline short                      _marpaWrapperAsf_factoring_finishb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, int nidOfChoicePointi, short *factoringFinishbp);
static inline short                      _marpaWrapperAsf_factoring_iterateb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, short *factoringIteratebp);
static inline short                      _marpaWrapperAsf_next_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, int nidOfChoicePointi, short *factoringbp);

/* Specific to glade */
static inline short                      _marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, genericStack_t **stackpp);
int                                      _marpaWrapperAsf_indAndNodesi(void *userDatavp, genericStackItemType_t itemType, void **pp);
static inline short                      _marpaWrapperAsf_and_nodes_to_cause_nidsp(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *andNodeIdStackp, genericStack_t **causeNidsStackpp);
static inline short                      _marpaWrapperAsf_glade_is_visitedb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi);
static inline void                       _marpaWrapperAsf_glade_visited_clearb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip);
static inline short                      _marpaWrapperAsf_glade_symch_countb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, int *countip);
static inline int                        _marpaWrapperAsf_glade_symbol_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi);
static inline int                        _marpaWrapperAsf_glade_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi);

/* Specific to nook */
static inline marpaWrapperAsfNook_t     *_marpaWrapperAsf_nook_newp(marpaWrapperAsf_t *marpaWrapperAsfp, int orNodeIdi, int parentOrNodeIdi);
static inline short                      _marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp);
static inline short                      _marpaWrapperAsf_setLastChoiceb(marpaWrapperAsf_t *marpaWrapperAsfp, short *haveLastChoicebp, marpaWrapperAsfNook_t *nookp);
static inline short                      _marpaWrapperAsf_nook_incrementb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp, short *haveLastChoicebp);

/* Specific to symch */
static inline short                      _marpaWrapperAsf_symch_factoring_countb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, int symchIxi, int *factoringCountip);

/* Specific to intset */
int                                      _marpaWrapperAsf_intset_keyIndFunctioni(void *userDatavp, genericStackItemType_t itemType, void **pp);
short                                    _marpaWrapperAsf_intset_keyCmpFunctionb(void *userDatavp, void **pp1, void **pp2);
void                                    *_marpaWrapperAsf_intset_keyCopyFunctionv(void *userDatavp, void **pp);
void                                     _marpaWrapperAsf_intset_keyFreeFunctionv(void *userDatavp, void **pp);

/* General */
static inline unsigned long              _marpaWrapperAsf_djb2(unsigned char *str);
static inline int                        _marpaWrapperAsf_token_es_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int andNodeIdi, int *lengthip);
static inline int                        _marpaWrapperAsf_or_node_es_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int choicepointi, int *lengthip);
static inline void                       _marpaWrapperAsf_dump_stack(marpaWrapperAsf_t *marpaWrapperAsfp, char *what, genericStack_t *stackp);
static inline short                      _marpaWrapperAsf_traverse_nextFactoringb(marpaWrapperAsfTraverser_t *traverserp, int *factoringIxip);
static inline short                      _marpaWrapperAsf_traverse_nextSymchb(marpaWrapperAsfTraverser_t *traverserp, int *symchIxip);

/* Specific to choicepoint */
static inline marpaWrapperAsfChoicePoint_t *_marpaWrapperAsf_choicepoint_newp(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline void                          _marpaWrapperAsf_choicepoint_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp);

/* Specific to value using the ASF */
static inline short                       _marpaWrapperAsf_valueTraverserb(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip);

/* For my very internal purpose */
#ifndef MARPAWRAPPER_NTRACE
static void _marpaWrapperAsf_dumpintsetHashpv(marpaWrapperAsf_t *marpaWrapperAsfp);
#endif

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
  int                                         ixi;
  Marpa_And_Node_ID                           andNodeIdi;
  int                                        *andNodep;
  Marpa_And_Node_ID                           andNodePredecessorIdi;
  
  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  if (marpaWrapperAsfOptionp == NULL) {
    marpaWrapperAsfOptionp = &marpaWrapperAsfOptionDefault;
  }
  genericLoggerp = marpaWrapperAsfOptionp->genericLoggerp;

  /* Impossible if we are already valuating it */
  if (marpaWrapperRecognizerp->treeModeb != MARPAWRAPPERRECOGNIZERTREEMODE_NA) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Already in valuation mode");
    goto err;
  }

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
  marpaWrapperAsfp->gladeStackp             = NULL;
  marpaWrapperAsfp->nextIntseti             = 0;
  marpaWrapperAsfp->traverserCallbackp      = NULL;
  marpaWrapperAsfp->userDatavp              = NULL;

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

  GENERICHASH_NEW_ALL(marpaWrapperAsfp->intsetHashp,
                      _marpaWrapperAsf_intset_keyIndFunctioni,
                      _marpaWrapperAsf_intset_keyCmpFunctionb,
                      NULL, /* We already allocate key on the heap: no need to copy it */
                      _marpaWrapperAsf_intset_keyFreeFunctionv,
                      NULL,  /* The value type will always be INT: no need */
                      NULL,  /* for a copy not a free functions for the value */
                      MARPAWRAPPERASF_INTSET_MAXROWS,
                      0);
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

  /* Say we are in forest mode */
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Flagging tree mode to FOREST");
  marpaWrapperRecognizerp->treeModeb = MARPAWRAPPERRECOGNIZERTREEMODE_FOREST;

  /* Get all the primary AND nodes */
  for (orNodei = 0; ; orNodei++) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_o_or_node_and_node_count(%p, %d)", marpaWrapperAsfp->marpaOrderp, orNodei);
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

    orNodep->nAndNodei = andNodeCounti;
    orNodep->andNodep = andNodep;

    /* Prepare the sort the AND Node Ids or this OR Node using predecessor Id */
    andNodeIdAndPredecessorIdp = malloc(andNodeCounti * sizeof(marpaWrapperAfsAndNodeIdAndPredecessorId_t));
    if (andNodeIdAndPredecessorIdp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }

    for (ixi = 0; ixi < andNodeCounti; ixi++) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_o_or_node_and_node_id_by_ix(%p, %d, %d)", marpaWrapperAsfp->marpaOrderp, orNodei, ixi);
      andNodeIdi = _marpa_o_or_node_and_node_id_by_ix(marpaWrapperAsfp->marpaOrderp, orNodei, ixi);
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_predecessor(%p, %d);", marpaWrapperAsfp->marpaBocagep, andNodeIdi);
      andNodePredecessorIdi = _marpa_b_and_node_predecessor(marpaWrapperAsfp->marpaBocagep, andNodeIdi);
      if (andNodePredecessorIdi < 0) {
	andNodePredecessorIdi = -1;
      }

      andNodeIdAndPredecessorIdp[ixi].andNodeIdi            = andNodeIdi;
      andNodeIdAndPredecessorIdp[ixi].andNodePredecessorIdi = andNodePredecessorIdi;
    }

    qsort(andNodeIdAndPredecessorIdp,
	  (size_t) andNodeCounti,
	  sizeof(marpaWrapperAfsAndNodeIdAndPredecessorId_t),
	  _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi);
    
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "OR_NODE[%3d]", orNodei);
    for (ixi = 0; ixi < andNodeCounti; ixi++) {
      andNodep[ixi] = andNodeIdAndPredecessorIdp[ixi].andNodeIdi;
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "\tAND_NODE[%3d]: %3d", ixi, andNodep[ixi]);
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
short marpaWrapperAsf_traverseb(marpaWrapperAsf_t *marpaWrapperAsfp, traverserCallback_t traverserCallbackp, void *userDatavp, int *valueip)
/****************************************************************************/
{
  const static char           funcs[] = "marpaWrapperAsf_traverseb";
  genericLogger_t            *genericLoggerp = NULL;
  int                         gladeIdi;
  marpaWrapperAsfGlade_t     *gladep;
  marpaWrapperAsfTraverser_t *traverserp = NULL;
  int                         valuei;

  if (marpaWrapperAsfp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  if (traverserCallbackp == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "traverserCallbackp is NULL");
    goto err;
  }

  if (_marpaWrapperAsf_peakb(marpaWrapperAsfp, &gladeIdi) == 0) {
    goto err;
  }
  gladep = _marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, gladeIdi);
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
  GENERICSTACK_NEW(traverserp->valueStackp);
  if (GENERICSTACK_ERROR(traverserp->valueStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "traverserp->valueStackp initialization failure: %s", strerror(errno));
    goto err;
  }
  traverserp->gladep       = gladep;
  traverserp->symchIxi     = 0;
  traverserp->factoringIxi = 0;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Calling traverser for glade %d", gladep->idi);
  if (! marpaWrapperAsfp->traverserCallbackp(traverserp, marpaWrapperAsfp->userDatavp, &valuei)) {
    goto err;
  }
  GENERICSTACK_FREE(traverserp->valueStackp);
  free(traverserp);

  if (valueip != NULL) {
    *valueip = valuei;
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  if (traverserp != NULL) {
    GENERICSTACK_FREE(traverserp->valueStackp);
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
#ifndef MARPAWRAPPER_NTRACE
    _marpaWrapperAsf_dumpintsetHashpv(marpaWrapperAsfp);
#endif
    GENERICHASH_FREE(marpaWrapperAsfp->intsetHashp, marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Nidset stack");
    _marpaWrapperAsf_nidset_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Powerset stack");
    _marpaWrapperAsf_powerset_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing glade stack");
    _marpaWrapperAsf_gladeStackp_freev(marpaWrapperAsfp);
    
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperAsfp);
    free(marpaWrapperAsfp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_orNodeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_orNodeStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                      orNodeUsedi;
  int                      i;
  marpaWrapperAsfOrNode_t *orNodep;

  if (marpaWrapperAsfp->orNodeStackp != NULL) {
    orNodeUsedi = GENERICSTACK_USED(marpaWrapperAsfp->orNodeStackp);
    for (i = 0; i < orNodeUsedi; i++) {
      orNodep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->orNodeStackp, i);
      if (GENERICSTACK_ERROR(marpaWrapperAsfp->orNodeStackp)) {
	GENERICSTACK_ERROR_RESET(marpaWrapperAsfp->orNodeStackp);
	continue;
      }
      if (orNodep->andNodep != NULL) {
	free(orNodep->andNodep);
      }
      free(orNodep);
    }
    GENERICSTACK_FREE(marpaWrapperAsfp->orNodeStackp);
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_gladeStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_gladeStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                      gladeUsedi;
  int                      i;
  marpaWrapperAsfGlade_t  *gladep;

  if (marpaWrapperAsfp->gladeStackp != NULL) {
    gladeUsedi = GENERICSTACK_USED(marpaWrapperAsfp->gladeStackp);
    for (i = 0; i < gladeUsedi; i++) {
      if (GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, i)) {
	gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp, i);
        _marpaWrapperAsf_glade_freev(marpaWrapperAsfp, gladep);
      }
    }
    GENERICSTACK_FREE(marpaWrapperAsfp->gladeStackp);
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_glade_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfGlade_t *gladep)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  if (gladep != NULL) {
    _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, gladep->symchesStackp);
    free(gladep);
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *symchesStackp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_symchesStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                      symchesUsedi;
  int                      i;
  genericStack_t          *factoringsStackp;

  if (symchesStackp != NULL) {
    symchesUsedi = GENERICSTACK_USED(symchesStackp);
    for (i = 0; i < symchesUsedi; i++) {
      /* symchesStackp is a stack of factoringsStackp */
      if (GENERICSTACK_IS_PTR(symchesStackp, i)) {
	factoringsStackp = GENERICSTACK_GET_PTR(symchesStackp, i);
	_marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
      }
    }
    GENERICSTACK_FREE(symchesStackp);
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t **factoringStackpp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_factoringStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                      factoringUsedi;
  int                      i;
  marpaWrapperAsfNook_t   *nookp;

  if (factoringStackpp != NULL) {
    genericStack_t *factoringStackp = *factoringStackpp;
    if (factoringStackp != NULL) {
      factoringUsedi = GENERICSTACK_USED(factoringStackp);
      for (i = 0; i < factoringUsedi; i++) {
	if (GENERICSTACK_IS_PTR(factoringStackp, i)) {
	  nookp = GENERICSTACK_GET_PTR(factoringStackp, i);
	  if (nookp != NULL) {
	    free(nookp);
	  }
	}
      }
      GENERICSTACK_FREE(factoringStackp);
    }
    *factoringStackpp = NULL;
  }
}

/****************************************************************************/
static inline void _marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *factoringsStackp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_factoringsStackp_freev";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                      factoringsUsedi;
  int                      i;
  genericStack_t          *localStackp;

  if (factoringsStackp != NULL) {
    factoringsUsedi = GENERICSTACK_USED(factoringsStackp);
    /* At indice >= 2, factoringsStackp can contain inner generic stacks */
    /* that are an array of gladeIdi */
    for (i = 2; i < factoringsUsedi; i++) {
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
static inline genericStack_t *_marpaWrapperAsf_factoringStackp_resetb(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t **factoringStackpp)
/****************************************************************************/
{
  const static char            funcs[]        = "_marpaWrapperAsf_factoringStackp_resetb";
  genericLogger_t             *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t              *factoringStackp;

  if (factoringStackpp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStackpp is NULL", strerror(errno));
    goto err;
  }
  _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp, factoringStackpp);
  GENERICSTACK_NEW(factoringStackp);
  if (GENERICSTACK_ERROR(factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStack reset failure: %s", strerror(errno));
    goto err;
  }

  *factoringStackpp = factoringStackp;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", factoringStackp);
  return factoringStackp;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_peakb(marpaWrapperAsf_t *marpaWrapperAsfp, int *gladeIdip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_peakb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *orNodeStackp   = marpaWrapperAsfp->orNodeStackp;
  int                      augmentOrNodeIdi;
  int                      augmentAndNodeIdi;
  int                      startOrNodeIdi;
  marpaWrapperAsfNidset_t *baseNidsetp;
  marpaWrapperAsfOrNode_t *orNodep;
  int                      gladeIdi;
  marpaWrapperAsfGlade_t  *gladep;

  augmentOrNodeIdi = (int) _marpa_b_top_or_node(marpaWrapperAsfp->marpaBocagep);
  if (augmentOrNodeIdi < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperAsfp->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  orNodep = GENERICSTACK_GET_PTR(orNodeStackp, augmentOrNodeIdi);
  if (GENERICSTACK_ERROR(orNodeStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "marpaWrapperAsfp->orNodeStackp get failure, %s", strerror(errno));
    goto err;
  }

  if (orNodep->nAndNodei <= 0) {
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

  baseNidsetp = _marpaWrapperAsf_nidset_obtainp(marpaWrapperAsfp, 1, &startOrNodeIdi);
  if (baseNidsetp == NULL) {
    goto err;
  }

  /* We cannot "obtain" the glade if it is not registered */
  gladeIdi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
  if (! GENERICSTACK_IS_PTR(marpaWrapperAsfp->gladeStackp, gladeIdi)) {

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Generating glade at indice %d", gladeIdi);
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
      free(gladep);
      goto err;
    }
  } else {

    gladep = GENERICSTACK_GET_PTR(marpaWrapperAsfp->gladeStackp,  gladeIdi);
    gladep->registeredb   = 1;
  }

  if (_marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, gladeIdi) == NULL) {
    goto err;
  }

  *gladeIdip = gladeIdi;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *gladeIdip=%d", *gladeIdip);
  return 1;

 err:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 0", 0);
  return 0;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi(const void *p1, const void *p2)
/****************************************************************************/
{
  marpaWrapperAfsAndNodeIdAndPredecessorId_t *a1 = (marpaWrapperAfsAndNodeIdAndPredecessorId_t *) p1;
  marpaWrapperAfsAndNodeIdAndPredecessorId_t *a2 = (marpaWrapperAfsAndNodeIdAndPredecessorId_t *) p2;

  return
    (a1->andNodePredecessorIdi < a2->andNodePredecessorIdi)
    ?
    -1
    :
    (
     (a1->andNodePredecessorIdi > a2->andNodePredecessorIdi)
     ?
     1
     :
     (
      (a1->andNodeIdi < a2->andNodeIdi)
      ?
      -1
      :
      (
       (a1->andNodeIdi > a2->andNodeIdi)
       ?
       1
       :
       0
       )
      )
     )
    ;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, int counti, int *idip)
/****************************************************************************/
{
  const static char        funcs[] = "_marpaWrapperAsf_intsetIdb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  short                    rcb;
  short                    findResultb;
  int                      intsetIdi;
  int                     *localIdip = NULL;

  /* Note: look to the callers, you will see that it can never happen than counti is <= 0 or idip is NULL */
  /* This method is responsible of memoization and is called very often. We create a local array of ints  */
  /* of size counti+1, and store counti at indice 0.                                                      */

  localIdip = malloc(sizeof(int) * (counti + 1));
  if (localIdip == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }
  *localIdip++ = counti;
  memcpy(localIdip, idip, sizeof(int) * counti);
  qsort(localIdip--, (size_t) counti, sizeof(int), _marpaWrapperAsf_idCmpi);

#ifndef MARPAWRAPPER_NTRACE
  {
    int idi;
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Looking for this context:");
    for (idi = 1; idi <= counti; idi++) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "   idi[0]=%d", localIdip[idi]);
    }
  }
#endif
  GENERICHASH_FIND(marpaWrapperAsfp->intsetHashp,
                   marpaWrapperAsfp,
                   PTR,
                   localIdip,
                   INT,
                   &intsetIdi,
                   findResultb);
  if (GENERICHASH_ERROR(marpaWrapperAsfp->intsetHashp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "intset hash find failure: %s", strerror(errno));
    goto err;
  }
  if (! findResultb) {

    intsetIdi = marpaWrapperAsfp->nextIntseti++;
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Creating next intset id %d", intsetIdi);
    GENERICHASH_SET(marpaWrapperAsfp->intsetHashp,
                    marpaWrapperAsfp,
                    PTR,
                    localIdip,
                    INT,
                    intsetIdi);
    if (GENERICHASH_ERROR(marpaWrapperAsfp->intsetHashp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "intset hash set failure: %s", strerror(errno));
      goto err;
    } else {
      /* The hash is keeping key pointer: we must not free it */
      localIdip = NULL;
    }
  } else {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Found intset id %d", intsetIdi);
  }

  *intsetIdip = intsetIdi;

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  if (localIdip != NULL) {
    free(localIdip);
  }
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d, *intsetIdip=%d", (int) rcb, *intsetIdip);
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
static inline int _marpaWrapperAsf_idCmpi(const void *p1, const void *p2)
/****************************************************************************/
{
  int i1 = * ((int *) p1);
  int i2 = * ((int *) p2);

  return (i1 < i2) ? -1 : ((i1 > i2) ? 1 : 0);
}

/****************************************************************************/
static inline marpaWrapperAsfIdset_t *_marpaWrapperAsf_idset_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, int counti, int *idip)
/****************************************************************************/
{
  const static char        funcs[] = "_marpaWrapperAsf_idset_obtainp";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *stackp = (idsete == MARPAWRAPPERASFIDSET_NIDSET) ? marpaWrapperAsfp->nidsetStackp : marpaWrapperAsfp->powersetStackp;
  char                    *idsets  = marpaWrapperAsfIdsets[idsete];
  marpaWrapperAsfIdset_t  *idsetp   = NULL;
  int                      intsetIdi;

  if (_marpaWrapperAsf_intsetIdb(marpaWrapperAsfp, &intsetIdi, counti, idip) == 0) {
    goto err;
  }

  if (GENERICSTACK_IS_PTR(stackp, intsetIdi)) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: indice %d already generated", idsets, intsetIdi);
    idsetp = GENERICSTACK_GET_PTR(stackp, intsetIdi);
  } else {
    idsetp = malloc(sizeof(marpaWrapperAsfIdset_t));
    if (idsetp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }
    idsetp->idi = intsetIdi;
    if (counti <= 0) {
      idsetp->counti = 0;
      idsetp->idip   = NULL;
    } else {
      idsetp->counti = counti;
      idsetp->idip   = malloc(sizeof(int) * counti);
      if (idsetp->idip == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
	goto err;
      }
      memcpy(idsetp->idip, idip, (size_t) (counti * sizeof(int)));
      qsort(idsetp->idip, (size_t) counti, sizeof(int), _marpaWrapperAsf_idCmpi);
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

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %p", idsets, idsetp);
  return idsetp;
}

/****************************************************************************/
static inline int *_marpaWrapperAsf_idset_idip(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idipb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %p", marpaWrapperAsfIdsets[idsete], idsetp->idip);
  return idsetp->idip;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_idset_idi_by_ixib(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp, int ixi, int *idip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi_by_ixib";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  if (ixi >= idsetp->counti) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "%s: indice %d out of range [0..%d[", marpaWrapperAsfIdsets[idsete], ixi, idsetp->counti);
    goto err;
  }

  *idip = idsetp->idip[ixi];

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return 1, *idip=%d", marpaWrapperAsfIdsets[idsete], *idip);
  return 1;

 err:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return 0", marpaWrapperAsfIdsets[idsete]);
  return 0;
  
}

/****************************************************************************/
static inline int _marpaWrapperAsf_idset_counti(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_counti";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %d", marpaWrapperAsfIdsets[idsete], idsetp->counti);
  return idsetp->counti;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_idset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_idi";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s: return %d", marpaWrapperAsfIdsets[idsete], (int) idsetp->idi);
  return idsetp->idi;
}

/****************************************************************************/
static inline void _marpaWrapperAsf_idset_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdsete_t idsete)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_idset_freeb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t         **stackpp        = (idsete == MARPAWRAPPERASFIDSET_NIDSET) ? &(marpaWrapperAsfp->nidsetStackp) : &(marpaWrapperAsfp->powersetStackp);
  char                    *idsets         = marpaWrapperAsfIdsets[idsete];
  genericStack_t          *stackp;
  int                      idsetUsedi;
  int                      i;
  marpaWrapperAsfIdset_t  *idsetp;

  if (*stackpp != NULL) {
    stackp = *stackpp;
    idsetUsedi = GENERICSTACK_USED(stackp);
    /* Every idset may have an array inside */
    for (i = 0; i < idsetUsedi; i++) {
      if (GENERICSTACK_IS_PTR(stackp, i)) {
	idsetp = GENERICSTACK_GET_PTR(stackp, i);
	if (idsetp->idip != NULL) {
	  free(idsetp->idip);
	}
	free(idsetp);
      }
    }
    GENERICSTACK_FREE(stackp);
    *stackpp = NULL;
  }
}

/****************************************************************************/
static inline marpaWrapperAsfNidset_t *_marpaWrapperAsf_nidset_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, int counti, int *idip)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_obtainp(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, counti, idip);
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
static inline int _marpaWrapperAsf_nidset_counti(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_counti(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp);
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nidset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET, idsetp);
}

/****************************************************************************/
static inline void _marpaWrapperAsf_nidset_freev(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  _marpaWrapperAsf_idset_freev(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_NIDSET);
}

/****************************************************************************/
static inline marpaWrapperAsfPowerset_t *_marpaWrapperAsf_powerset_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, int counti, int *idip)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_obtainp(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, counti, idip);
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
static inline int _marpaWrapperAsf_powerset_counti(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_counti(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp);
}

/****************************************************************************/
static inline int _marpaWrapperAsf_powerset_idi(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfIdset_t *idsetp)
/****************************************************************************/
{
  return _marpaWrapperAsf_idset_idi(marpaWrapperAsfp, MARPAWRAPPERASFIDSET_POWERSET, idsetp);
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
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperAsfNook_t   *nookp;
  short                    haveLastChoiceb;

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

  if (_marpaWrapperAsf_setLastChoiceb(marpaWrapperAsfp, &haveLastChoiceb, nookp) == 0) {
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Created nook {OR_NODE=%d, PARENT=%d, FIRST_CHOICE=%d, LAST_CHOICE=%d}", nookp->orNodeIdi, nookp->parentOrNodeIdi, nookp->firstChoicei, nookp->lastChoicei);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nookp);
  return nookp;

 err:
  if (nookp != NULL) {
    free(nookp);
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]                  = "_marpaWrapperAsf_nook_has_semantic_causeb";
  genericLogger_t         *genericLoggerp           = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
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
static inline short _marpaWrapperAsf_setLastChoiceb(marpaWrapperAsf_t *marpaWrapperAsfp, short *haveLastChoicebp, marpaWrapperAsfNook_t *nookp)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_setLastChoiceb";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *orNodeStackp       = marpaWrapperAsfp->orNodeStackp;
  int                      orNodeIdi          = nookp->orNodeIdi;
  int                      choicei            = nookp->firstChoicei;
  marpaWrapperAsfOrNode_t *orNodep;

  if (! GENERICSTACK_IS_PTR(orNodeStackp, orNodeIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No entry in orNode stack at indice %d", orNodeIdi);
    goto err;
  }
  orNodep = GENERICSTACK_GET_PTR(orNodeStackp, orNodeIdi);

  if (choicei >= orNodep->nAndNodei) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "No last choice available: choicei=%d >= orNodep->nAndNodei=%d", choicei, orNodep->nAndNodei);
    *haveLastChoicebp = 0;
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
	if (++choicei >= orNodep->nAndNodei) {
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
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Setted last choice of nook {OR_NODE=%d, PARENT=%d, FIRST_CHOICE=%d, LAST_CHOICE=%d}", nookp->orNodeIdi, nookp->parentOrNodeIdi, nookp->firstChoicei, nookp->lastChoicei);
    *haveLastChoicebp = 1;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *haveLastChoicebp=%d", (int) *haveLastChoicebp);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_nook_incrementb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfNook_t *nookp, short *haveLastChoicebp)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_nook_incrementb";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  nookp->firstChoicei = nookp->lastChoicei + 1;

  if (_marpaWrapperAsf_setLastChoiceb(marpaWrapperAsfp, haveLastChoicebp, nookp) == 0) {
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Incremented nook {OR_NODE=%d, PARENT=%d, FIRST_CHOICE=%d, LAST_CHOICE=%d}", nookp->orNodeIdi, nookp->parentOrNodeIdi, nookp->firstChoicei, nookp->lastChoicei);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *haveLastChoicebp=%d", (int) *haveLastChoicebp);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_symch_factoring_countb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, int symchIxi, int *factoringCountip)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_symch_factoring_countb";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperAsfGlade_t  *gladep;
  genericStack_t          *symchesStackp;
  genericStack_t          *factoringsStackp;

  gladep = _marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, gladeIdi);
  if (gladep == NULL) {
    goto err;
  }
  symchesStackp = gladep->symchesStackp;
  if (! GENERICSTACK_IS_PTR(symchesStackp, symchIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No symch at indice %d", symchIxi);
    goto err;
  }
  factoringsStackp = GENERICSTACK_GET_PTR(symchesStackp,  symchIxi);
  if (factoringsStackp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Null factorings stack at symch indice %d", symchIxi);
    goto err;
  }
  /* factoringsStackp is (symchRuleIdi, PTR, stack of gladeIdi) */
  /* This is length - 2 */
  *factoringCountip = GENERICSTACK_USED(factoringsStackp) - 2;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *factoringCountip=%d", *factoringCountip);
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
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  int                       rci;
  
  if (nidi >= 0) {
    Marpa_IRL_ID  irlIdi;
    Marpa_Rule_ID xrlIdi;

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_irl(%p, %d)", marpaBocagep, nidi);
    irlIdi = _marpa_b_or_node_irl(marpaBocagep, nidi);
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_source_xrl(%p, %d)", marpaGrammarp, (int) irlIdi);
    xrlIdi = _marpa_g_source_xrl(marpaGrammarp, irlIdi);

    rci = (int) xrlIdi;
  } else {
    int andNodeIdi = _marpaWrapperAsf_nid_to_and_nodei(nidi);
    int tokenNsyIdi;
    int tokenIdi;

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_symbol(%p, %d)", marpaBocagep, andNodeIdi);
    tokenNsyIdi = _marpa_b_and_node_symbol(marpaBocagep, andNodeIdi);
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_source_xsy(%p, %d)", marpaGrammarp, tokenNsyIdi);
    tokenIdi = _marpa_g_source_xsy(marpaGrammarp, tokenNsyIdi);

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
static inline int _marpaWrapperAsf_nid_token_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_nid_token_idi";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                       idi                     = -1;

  if (nidi <= MARPAWRAPPERASF_NID_LEAF_BASE) {
    int                       andNodeIdi              = _marpaWrapperAsf_nid_to_and_nodei(nidi);
    marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
    marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
    Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
    Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
    int                       tokenNsyIdi;

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_symbol(%p, %d)", marpaBocagep, andNodeIdi);
    tokenNsyIdi = _marpa_b_and_node_symbol(marpaBocagep, andNodeIdi);

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_source_xsy(%p, %d)", marpaBocagep, tokenNsyIdi);
    idi = (int) _marpa_g_source_xsy(marpaGrammarp, tokenNsyIdi);
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", idi);
  return idi;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nid_symbol_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_nid_symbol_idi";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                       idi                     = -1;
  int                       tokenIdi;

  tokenIdi = _marpaWrapperAsf_nid_token_idi(marpaWrapperAsfp, nidi);
  if (tokenIdi >= 0) {
    idi = tokenIdi;
  } else {
    if (nidi < 0) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No symbol ID for node ID: %d", nidi);
    } else {
      marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
      marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
      Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
      Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
      Marpa_IRL_ID              irlIdi;
      Marpa_Rule_ID             xrlIdi;
      Marpa_Symbol_ID           lhsIdi;

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_irl(%p, %d)", marpaBocagep, nidi);
      irlIdi = _marpa_b_or_node_irl(marpaBocagep, nidi);
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_source_xrl(%p, %d)", marpaGrammarp, (int) irlIdi);
      xrlIdi = _marpa_g_source_xrl(marpaGrammarp, irlIdi);
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_lhs(%p, %d)", marpaGrammarp, (int) xrlIdi);
      lhsIdi = marpa_g_rule_lhs(marpaGrammarp, xrlIdi);

      idi = (int) lhsIdi;
    }
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", idi);
  return idi;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_nid_rule_idi";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                       idi                     = -1;

  if (nidi >= 0) {
    marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
    marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
    Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
    Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
    Marpa_IRL_ID              irlIdi;
    
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_irl(%p, %d)", marpaBocagep, nidi);
    irlIdi = _marpa_b_or_node_irl(marpaBocagep, nidi);

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_g_source_xrl(%p, %d)", marpaGrammarp, (int) irlIdi);
    idi = (int) _marpa_g_source_xrl(marpaGrammarp, irlIdi);
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", idi);
  return idi;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_nid_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int nidi)
/****************************************************************************/
{
  const static char  funcs[]        = "_marpaWrapperAsf_nid_spani";
  genericLogger_t   *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                spanIdi        = -1;

  if (nidi <= MARPAWRAPPERASF_NID_LEAF_BASE) {
    int andNodeIdi = _marpaWrapperAsf_nid_to_and_nodei(nidi);

    spanIdi = _marpaWrapperAsf_token_es_spani(marpaWrapperAsfp, andNodeIdi, NULL);
  }
  if (nidi >= 0) {
    spanIdi = _marpaWrapperAsf_or_node_es_spani(marpaWrapperAsfp, nidi, NULL);
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", spanIdi);
  return spanIdi;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_token_es_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int andNodeIdi, int *lengthip)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_token_es_spani";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  int                       predecessorIdi;
  int                       parentOrNodeIdi;
  int                       spanIdi;
  int                       lengthi;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_predecessor(%p, %d);", marpaBocagep, andNodeIdi);
  predecessorIdi = _marpa_b_and_node_predecessor(marpaBocagep, andNodeIdi);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_and_node_parent(%p, %d);", marpaBocagep, andNodeIdi);
  parentOrNodeIdi = _marpa_b_and_node_parent(marpaBocagep, andNodeIdi);

  if (predecessorIdi >=0) {
    int originEsi;
    int currentEsi;

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_set(%p, %d);", marpaBocagep, predecessorIdi);
    originEsi = _marpa_b_or_node_set(marpaBocagep, predecessorIdi);

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_set(%p, %d);", marpaBocagep, predecessorIdi);
    currentEsi = _marpa_b_or_node_set(marpaBocagep, parentOrNodeIdi);

    spanIdi = originEsi; 
    lengthi = currentEsi - originEsi;
  } else {

    spanIdi = _marpaWrapperAsf_or_node_es_spani(marpaWrapperAsfp, parentOrNodeIdi, &lengthi);
  }

  if (lengthip != NULL) {
    *lengthip = lengthi;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return spanIdi=%d, lengthi=%d", spanIdi, lengthi);
  return spanIdi;
}

/****************************************************************************/
static inline marpaWrapperAsfNidset_t *_marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfPowerset_t *powersetp, int ixi)
/****************************************************************************/
{
  const static char        funcs[]            = "_marpaWrapperAsf_powerset_nidsetp";
  genericLogger_t         *genericLoggerp     = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *nidsetStackp       = marpaWrapperAsfp->nidsetStackp;
  marpaWrapperAsfNidset_t *nidsetp            = NULL;
  int                      counti;
  int                      idi;

  counti = _marpaWrapperAsf_powerset_counti(marpaWrapperAsfp, powersetp);
  if (ixi < counti) {
    if (_marpaWrapperAsf_powerset_idi_by_ixib(marpaWrapperAsfp, powersetp, ixi, &idi) == 0) {
      goto err;
    }
    if (GENERICSTACK_IS_PTR(nidsetStackp, idi)) {
      nidsetp = GENERICSTACK_GET_PTR(nidsetStackp, idi);
    }
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nidsetp);
  return nidsetp;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline marpaWrapperAsfGlade_t *_marpaWrapperAsf_glade_obtainp(marpaWrapperAsf_t *marpaWrapperAsfp, int gladei)
/****************************************************************************/
{
  const static char            funcs[]                = "_marpaWrapperAsf_glade_obtainp";
  genericLogger_t             *genericLoggerp         = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t              *gladeStackp            = marpaWrapperAsfp->gladeStackp;
  genericStack_t              *nidsetStackp           = marpaWrapperAsfp->nidsetStackp;
  marpaWrapperAsfSourceData_t *sourceDatap            = NULL;
  int                          nidWithCurrentSortIxii = 0;
  int                         *nidWithCurrentSortIxip = NULL;
  int                          symchIdii              = 0;
  int                         *symchIdip              = NULL;
  short                        thisNidEndb            = 0;
  int                         *symbolip               = NULL;
  genericStack_t              *factoring              = NULL;
  genericStack_t              *symchesStackp          = NULL;
  genericStack_t              *factoringsStackp       = NULL;
  marpaWrapperAsfChoicePoint_t *choicepointp = NULL;
  int                          nSourceDatai;
  marpaWrapperAsfGlade_t      *gladep;
  marpaWrapperAsfNidset_t     *baseNidsetp;
  marpaWrapperAsfPowerset_t   *choicepointPowsersetp;
  int                          sourceNidi;
  int                          nidIxi;
  int                          ixi;
  int                          sortIxOfThisNidi;
  int                          thisNidi;
  int                          currentSortIxi;
  marpaWrapperAsfNidset_t     *nidsetForSortIxp;
  int                          symchCounti;
  int                          symchIxi;
  int                          choicepointNidi;
  int                          symchRuleIdi;

  if ((! GENERICSTACK_IS_PTR(gladeStackp, gladei)) || ((gladep = GENERICSTACK_GET_PTR(gladeStackp, gladei))->registeredb == 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Attempt to use an invalid glade, one whose ID is %d", gladei);
    goto err;
  }

  /* Return the glade if it is already set up */
  if (gladep->symchesStackp != NULL) {
    goto done;
  }

  if (! GENERICSTACK_IS_PTR(nidsetStackp, gladei)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No nidset at glade id %d", gladei);
    goto err;
  }
  baseNidsetp = GENERICSTACK_GET_PTR(nidsetStackp, gladei);
  nSourceDatai = _marpaWrapperAsf_nidset_counti(marpaWrapperAsfp, baseNidsetp);
  if (nSourceDatai <= 0) {
    MARPAWRAPPER_ERROR(genericLoggerp, "No nidset");
    goto err;
  }
  sourceDatap = malloc((size_t) (nSourceDatai * sizeof(marpaWrapperAsfSourceData_t)));
  if (sourceDatap == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }
  for (ixi = 0; ixi < nSourceDatai; ixi++) {
    if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, baseNidsetp, ixi, &sourceNidi) == 0) {
      goto err;
    }
    sourceDatap[ixi].sortIxi = _marpaWrapperAsf_nidset_sort_ixi(marpaWrapperAsfp, sourceNidi);
    if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, baseNidsetp, ixi, &(sourceDatap[ixi].sourceNidi)) == 0) {
      goto err;
    }
  }
  qsort(sourceDatap, (size_t) nSourceDatai, sizeof(marpaWrapperAsfSourceData_t), _marpaWrapperAsf_sourceDataCmpi);

  sortIxOfThisNidi = sourceDatap[0].sortIxi;
  thisNidi         = sourceDatap[0].sourceNidi;
  nidIxi           = 1;
  currentSortIxi   = sortIxOfThisNidi;
  while (1) {
    if (sortIxOfThisNidi != currentSortIxi) {
      /* Current only whole id break logic */
      nidsetForSortIxp = _marpaWrapperAsf_nidset_obtainp(marpaWrapperAsfp, nidWithCurrentSortIxii, nidWithCurrentSortIxip);
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
	int *tmpip = realloc(symchIdip, (size_t) ((symchIdii + 1) * sizeof(int)));
	if (tmpip == NULL) {
	  MARPAWRAPPER_ERRORF(genericLoggerp, "realloc failure: %s", strerror(errno));
	  goto err;
	}
	symchIdip = tmpip;
      }
      symchIdip[symchIdii++] = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, nidsetForSortIxp);
      free(nidWithCurrentSortIxip);
      nidWithCurrentSortIxip = NULL;
      nidWithCurrentSortIxii = 0;

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
      int *tmpip = realloc(nidWithCurrentSortIxip, (size_t) ((nidWithCurrentSortIxii + 1) * sizeof(int)));
      if (tmpip == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "realloc failure: %s", strerror(errno));
	goto err;
      }
      nidWithCurrentSortIxip = tmpip;
    }
    nidWithCurrentSortIxip[nidWithCurrentSortIxii++] = thisNidi;
    if (nidIxi < nSourceDatai) {
      sortIxOfThisNidi = sourceDatap[nidIxi].sortIxi;
      thisNidi         = sourceDatap[nidIxi].sourceNidi;
      nidIxi++;
      continue;
    }
    thisNidEndb = 1;
    sortIxOfThisNidi = -2;
  }
  choicepointPowsersetp = _marpaWrapperAsf_powerset_obtainp(marpaWrapperAsfp, symchIdii, symchIdip);
  if (choicepointPowsersetp == NULL) {
    goto err;
  }
  choicepointp = _marpaWrapperAsf_choicepoint_newp(marpaWrapperAsfp);
  if (choicepointp == NULL) {
    goto err;
  }

  /* Check if choicepoint already seen ? */
  GENERICSTACK_NEW(symchesStackp);
  if (GENERICSTACK_ERROR(symchesStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to initalize symchesStackp, %s", strerror(errno));
    goto err;
  }
  symchCounti = _marpaWrapperAsf_powerset_counti(marpaWrapperAsfp, choicepointPowsersetp);
  for (symchIxi = 0; symchIxi < symchCounti; symchIxi++) {
    marpaWrapperAsfNidset_t *symchNidsetp;
    int                      nidcounti;
    int                      nidixi;
    
    /* Free factoring stack */
    _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp, &(choicepointp->factoringStackp));

    symchNidsetp = _marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsfp, choicepointPowsersetp, symchIxi);
    if (symchNidsetp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "symchNidsetp is NULL", strerror(errno));
      goto err;
    }
    if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, symchNidsetp, 0, &choicepointNidi) == 0) {
      goto err;
    }
    symchRuleIdi = _marpaWrapperAsf_nid_rule_idi(marpaWrapperAsfp, choicepointNidi);
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Generating factoringsStackp for rule %d", symchRuleIdi);

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

    /* For a token, there will not be multiple factorings or nids */
    if (symchRuleIdi < 0) {
      marpaWrapperAsfNidset_t *baseNidsetp = _marpaWrapperAsf_nidset_obtainp(marpaWrapperAsfp, 1, &choicepointNidi);
      int                      gladeIdi;
      genericStack_t          *localStackp = NULL;

      if (baseNidsetp == NULL) {
	goto err;
      }
      gladeIdi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
      if (! GENERICSTACK_IS_PTR(gladeStackp, gladeIdi)) {
        marpaWrapperAsfGlade_t *localGladep;

	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Generating glade at indice %d", gladeIdi);
        localGladep = malloc(sizeof(marpaWrapperAsfGlade_t));
        if (localGladep == NULL) {
          MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
          goto err;
        }
        localGladep->idi           = gladeIdi;
        localGladep->symchesStackp = NULL;
        localGladep->visitedb      = 0;
        localGladep->registeredb   = 1;
        GENERICSTACK_SET_PTR(gladeStackp, localGladep, gladeIdi);
        if (GENERICSTACK_ERROR(gladeStackp)) {
          MARPAWRAPPER_ERRORF(genericLoggerp, "glade stack failure: %s", strerror(errno));
          free(localGladep);
          goto err;
        }
      } else {
        marpaWrapperAsfGlade_t *localGladep;

        localGladep = GENERICSTACK_GET_PTR(gladeStackp, gladeIdi);
        localGladep->registeredb = 1;
      }

      GENERICSTACK_NEW(localStackp);
      if (GENERICSTACK_ERROR(localStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "localStackp initialization failure, %s", strerror(errno));
	goto err;
      }
      GENERICSTACK_PUSH_INT(localStackp, gladeIdi);
      if (GENERICSTACK_ERROR(localStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "localStackp push failure, %s", strerror(errno));
        GENERICSTACK_FREE(localStackp);
	goto err;
      }
      GENERICSTACK_PUSH_PTR(factoringsStackp, localStackp);
      if (GENERICSTACK_ERROR(factoringsStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to push factoringsStackp, %s", strerror(errno));
	GENERICSTACK_FREE(localStackp);
	goto err;
      }
      /* localStackp is now in factoringsStackp */
      localStackp = NULL;
      GENERICSTACK_PUSH_PTR(symchesStackp, factoringsStackp);
      if (GENERICSTACK_ERROR(symchesStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to push symchesStack, %s", strerror(errno));
	goto err;
      }
      /* factoringsStackp is now in symchesStackp, and we do NOT execute the rest of the loop */
      factoringsStackp = NULL;
      continue;
    }

    symchNidsetp = _marpaWrapperAsf_powerset_nidsetp(marpaWrapperAsfp, choicepointPowsersetp, symchIxi);
    if (symchNidsetp == NULL) {
      MARPAWRAPPER_ERROR(genericLoggerp, "symchNidsetp is NULL");
      goto err;
    }
    nidcounti = _marpaWrapperAsf_nidset_counti(marpaWrapperAsfp, symchNidsetp);
    for (nidixi = 0; nidixi < nidcounti; nidixi++) {
      short breakFactoringsLoop = 0;
      short firstFactoringb;
      short factoringb;
      
      if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, symchNidsetp, nidixi, &choicepointNidi) == 0) {
	goto err;
      }
      if (_marpaWrapperAsf_first_factoringb(marpaWrapperAsfp, choicepointp, choicepointNidi, &firstFactoringb) == 0) {
	goto err;
      }

      GENERICSTACK_FREE(factoring);
      if (_marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsfp, choicepointp, &factoring) == 0) {
	goto err;
      }

      while (factoring != NULL) {
	if (GENERICSTACK_USED(factoringsStackp) > MARPAWRAPPERASF_FACTORING_MAX) {
	  /* Update factorings omitted flag - this indice is already allocated: factoringsStackp cannot change*/
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
	  int             itemIxi;

	  GENERICSTACK_NEW(localFactoringStackp);
	  if (GENERICSTACK_ERROR(localFactoringStackp)) {
	    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize localFactoringStackp");
	    goto err;
	  }
          if (GENERICSTACK_USED(factoring) > 0) {
            for (itemIxi = GENERICSTACK_USED(factoring) - 1; itemIxi >= 0; itemIxi--) {
              if (! GENERICSTACK_IS_INT(factoring, itemIxi)) {
                MARPAWRAPPER_ERRORF(genericLoggerp, "Indice %d is not an int", itemIxi);
                GENERICSTACK_FREE(localFactoringStackp);
                goto err;
              }
	    
              GENERICSTACK_PUSH_INT(localFactoringStackp, GENERICSTACK_GET_INT(factoring,  itemIxi));
              if (GENERICSTACK_ERROR(localFactoringStackp)) {
                MARPAWRAPPER_ERROR(genericLoggerp, "Failure to push in localFactoringStackp");
                GENERICSTACK_FREE(localFactoringStackp);
                goto err;
              }
            }
          }

          GENERICSTACK_PUSH_PTR(factoringsStackp, localFactoringStackp);
          if (GENERICSTACK_ERROR(factoringsStackp)) {
            MARPAWRAPPER_ERROR(genericLoggerp, "Failure to push in factoringsStackp");
            GENERICSTACK_FREE(localFactoringStackp);
            goto err;
          }
          /* localFactoringStackp is now in factoringsStackp */

	  if (_marpaWrapperAsf_next_factoringb(marpaWrapperAsfp, choicepointp, choicepointNidi, &factoringb) == 0) {
	    goto err;
	  }
          GENERICSTACK_FREE(factoring);
	  if (_marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsfp, choicepointp, &factoring) == 0) {
	    goto err;
	  }
	}
      }
      if (breakFactoringsLoop != 0) {
	break;
      }
    }
    GENERICSTACK_PUSH_PTR(symchesStackp, factoringsStackp);
    if (GENERICSTACK_ERROR(symchesStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to push symchesStack, %s", strerror(errno));
      goto err;
    }
    /* factoringsStackp is now in symchesStackp */
    factoringsStackp = NULL;
  }

  /* Replace current symches */
  _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, gladep->symchesStackp);
  gladep->symchesStackp = symchesStackp;
  /* symchesStackp is now in gladep */
  symchesStackp = NULL;
  gladep->idi = gladei;

  GENERICSTACK_SET_PTR(gladeStackp, gladep, gladei);
  if (GENERICSTACK_ERROR(gladeStackp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to set in gladeStackp");
    goto err;
  }

  goto done;

 err:
  if (symchesStackp != NULL) {
    _marpaWrapperAsf_symchesStackp_freev(marpaWrapperAsfp, symchesStackp);
  }

 done:
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
  GENERICSTACK_FREE(factoring);
  _marpaWrapperAsf_factoringsStackp_freev(marpaWrapperAsfp, factoringsStackp);
  _marpaWrapperAsf_choicepoint_freev(marpaWrapperAsfp, choicepointp);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", gladep);
  return gladep;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_first_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, int nidOfChoicePointi, short *firstFactoringbp)
/****************************************************************************/
{
  const static char            funcs[]           = "_marpaWrapperAsf_first_factoringb";
  genericLogger_t             *genericLoggerp    = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t              *orNodeStackp      = marpaWrapperAsfp->orNodeStackp;
  marpaWrapperAsfNook_t       *nookp             = NULL;
  genericStack_t              *orNodeInUseStackp = choicepointp->orNodeInUseStackp;
  marpaWrapperAsfOrNode_t     *orNodep;
  genericStack_t              *factoringStackp;

  /* Current NID of current SYMCH */
  /* The caller should ensure that we are never called unless the current NIS is for a rule */
  if (nidOfChoicePointi < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "First factoring called for negative NID %d", nidOfChoicePointi);
      goto err;
  }

  /* Due to skipping, even the top OR node can have no valid choices */
  if ((! GENERICSTACK_IS_PTR(orNodeStackp, nidOfChoicePointi)) ||
      (((orNodep = GENERICSTACK_GET_PTR(orNodeStackp, nidOfChoicePointi))->nAndNodei) <= 0)) {
    _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp, &(choicepointp->factoringStackp));
    *firstFactoringbp = 0;
    goto done;
  }

  GENERICSTACK_SET_SHORT(orNodeInUseStackp, 1, nidOfChoicePointi);
  if (GENERICSTACK_ERROR(orNodeInUseStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "orNodeInUseStackp failure: %s", strerror(errno));
    goto err;
  }

  nookp = _marpaWrapperAsf_nook_newp(marpaWrapperAsfp, nidOfChoicePointi, -1);
  if (nookp == NULL) {
    goto err;
  }

  if (_marpaWrapperAsf_factoringStackp_resetb(marpaWrapperAsfp, &(choicepointp->factoringStackp)) == 0) {
    goto err;
  }

  factoringStackp = choicepointp->factoringStackp;
  GENERICSTACK_PUSH_PTR(factoringStackp, nookp);
  if (GENERICSTACK_ERROR(factoringStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStackp push failure: %s", strerror(errno));
    goto err;
  }

  /* nookp is now in marpaWrapperAsfp->factoringStackp */
  nookp = NULL;

  /* Iterate as long as we cannot finish this stack */
  while (1) {
    short factoringFinishb;
    short factoringIterateb;

    if (_marpaWrapperAsf_factoring_finishb(marpaWrapperAsfp, choicepointp, nidOfChoicePointi, &factoringFinishb) == 0) {
      goto err;
    }
    if (factoringFinishb != 0) {
      break;
    }
    
    if (_marpaWrapperAsf_factoring_iterateb(marpaWrapperAsfp, choicepointp, &factoringIterateb) == 0) {
      goto err;
    }
    if (factoringIterateb == 0) {
      goto done;
    }
  }

  *firstFactoringbp = 1;

 done:
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *firstFactoringbp=%d", (int) *firstFactoringbp);
  return 1;

 err:
  if (nookp != NULL) {
    free(nookp);
  }
  
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_factoring_finishb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, int nidOfChoicePointi, short *factoringFinishbp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_factoring_finishb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  genericStack_t           *orNodeStackp            = marpaWrapperAsfp->orNodeStackp;
  genericStack_t           *factoringStackp         = choicepointp->factoringStackp;
  genericStack_t           *orNodeInUseStackp       = choicepointp->orNodeInUseStackp;
  genericStack_t           *worklistStackp;
  int                       worklistUsedi;
  int                       worklistStacki;
  int                       worklistLasti;

  if (factoringStackp == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "factoringStackp is NULL");
    goto err;
  }

  GENERICSTACK_NEW_SIZED(worklistStackp, GENERICSTACK_USED(factoringStackp));
  if (GENERICSTACK_ERROR(worklistStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "worklistStackp initialization failure: %s", strerror(errno));
    goto err;
  }
  for (worklistStacki = 0; worklistStacki < GENERICSTACK_USED(factoringStackp); worklistStacki++) {
    GENERICSTACK_SET_INT(worklistStackp, worklistStacki, worklistStacki);
    if (GENERICSTACK_ERROR(worklistStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "worklistStackp initialization at indice %d failure: %s", worklistStacki, strerror(errno));
      goto err;
    }
  }

  while ((worklistUsedi = GENERICSTACK_USED(worklistStackp)) > 0) {
    marpaWrapperAsfNook_t   *workNookp;
    int                      workOrNodeIdi;
    int                      workingChoicei;
    marpaWrapperAsfOrNode_t *orNodep;
    int                      workAndNodeIdi;
    int                      childOrNodei;
    short                    childIsCauseb = 0;
    short                    childIsPredecessorb = 0;
    marpaWrapperAsfNook_t   *newNookp;

    worklistLasti = worklistUsedi - 1;

    if (! GENERICSTACK_IS_PTR(factoringStackp, worklistLasti)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No nook at indice %d", worklistLasti);
      goto err;
    }
    workNookp = GENERICSTACK_GET_PTR(factoringStackp, worklistLasti);
    workOrNodeIdi  = workNookp->orNodeIdi;
    if (! GENERICSTACK_IS_PTR(orNodeStackp, workOrNodeIdi)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No orNode at indice %d of orNodesStackp", workOrNodeIdi);
      goto err;
    }
    orNodep = GENERICSTACK_GET_PTR(orNodeStackp, workOrNodeIdi);

    workingChoicei = workNookp->firstChoicei;;
    if (workingChoicei >= orNodep->nAndNodei) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No andNode at indice %d of orNodep->andNodep", workingChoicei);
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
    if (GENERICSTACK_IS_SHORT(orNodeInUseStackp,  childOrNodei) &&
        GENERICSTACK_GET_SHORT(orNodeInUseStackp,  childOrNodei)) {
      *factoringFinishbp = 0;
      goto done;
    }

    if ((! GENERICSTACK_IS_PTR(orNodeStackp,  workOrNodeIdi)) ||
        (((marpaWrapperAsfOrNode_t *) GENERICSTACK_GET_PTR(orNodeStackp, workOrNodeIdi))->nAndNodei <= 0)) {
      *factoringFinishbp = 0;
      goto done;
    }

    newNookp = _marpaWrapperAsf_nook_newp(marpaWrapperAsfp, childOrNodei, worklistLasti);
    if (childIsCauseb != 0) {
      newNookp->isCauseb = 1;
      workNookp->causeIsExpandedb = 1;
    }
    if (childIsPredecessorb != 0) {
      newNookp->isPredecessorb = 1;
      workNookp->predecessorIsExpandedb = 1;
    }

    GENERICSTACK_PUSH_PTR(factoringStackp, newNookp);
    if (GENERICSTACK_ERROR(factoringStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "factoringStackp push failure, %s", strerror(errno));
      goto err;
    }
    GENERICSTACK_PUSH_INT(worklistStackp, GENERICSTACK_USED(factoringStackp) - 1);
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
static inline short _marpaWrapperAsf_factoring_iterateb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, short *factoringIteratebp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_factoring_iterateb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t           *factoringStackp         = choicepointp->factoringStackp;
  genericStack_t           *orNodeInUseStackp       = choicepointp->orNodeInUseStackp;
  marpaWrapperAsfNook_t    *topNookp;
  marpaWrapperAsfNook_t    *parentNookp;
  int                       stackIxOfParentNooki;
  int                       orNodei;
  int                       factoringStacki;
  short                     haveLastChoiceb;

  while (1) {
    factoringStacki = GENERICSTACK_USED(factoringStackp);
    if (factoringStacki <= 0) {
      _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp, &(choicepointp->factoringStackp));
      *factoringIteratebp = 0;
      goto done;
    }

    if (! GENERICSTACK_IS_PTR(factoringStackp, factoringStacki - 1)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "No nook at indice %d of factoringStackp", (factoringStacki - 1));
      goto err;
    }
    topNookp = GENERICSTACK_GET_PTR(factoringStackp, factoringStacki - 1);
    if (_marpaWrapperAsf_nook_incrementb(marpaWrapperAsfp, topNookp, &haveLastChoiceb) == 0) {
      goto err;
    }
    if (haveLastChoiceb == 1) {
      break;
    }

    /* Could not iterate */
    /* "Dirty" the corresponding bits in the parent and pop this nook */
    stackIxOfParentNooki = topNookp->parentOrNodeIdi;
    if (stackIxOfParentNooki >= 0) {
      if (! GENERICSTACK_IS_PTR(factoringStackp,  stackIxOfParentNooki)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "No nook at indice %d of factoringStackp", stackIxOfParentNooki);
	goto err;
      }
      parentNookp = GENERICSTACK_GET_PTR(factoringStackp,  stackIxOfParentNooki);
      if (topNookp->isCauseb != 0) {
	parentNookp->causeIsExpandedb = 0;
      }
      if (topNookp->isPredecessorb != 0) {
	parentNookp->predecessorIsExpandedb = 0;
      }
    }
    orNodei = topNookp->orNodeIdi;
    GENERICSTACK_SET_PTR(orNodeInUseStackp, NULL, orNodei);
    if (GENERICSTACK_ERROR(orNodeInUseStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "failure to set NULL at indice %d of orNodeInUseStackp", orNodei);
      goto err;
    }
    /* On the top of the stack, this is topNookp per definition */
    GENERICSTACK_POP_PTR(factoringStackp);
    if (GENERICSTACK_ERROR(factoringStackp)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "failure to pop from factoringStackp");
      goto err;
    }
    free(topNookp);
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
static inline short _marpaWrapperAsf_glade_id_factorsb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, genericStack_t **stackpp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_glade_id_factorsb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t           *stackp                  = NULL;
  genericStack_t           *orNodeStackp            = marpaWrapperAsfp->orNodeStackp;
  genericStack_t           *gladeStackp             = marpaWrapperAsfp->gladeStackp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  genericStack_t           *andNodeIdStackp         = NULL;
  genericStack_t           *causeNidsStackp         = NULL;
  int                      *causeNidsp              = NULL;
  int                       factorIxi;
  marpaWrapperAsfNook_t    *nookp;
  int                       orNodeIdi;
  marpaWrapperAsfOrNode_t  *orNodep;
  int                       nAndNodei;
  int                      *andNodep;
  int                       choicei;
  marpaWrapperAsfNidset_t  *baseNidsetp;
  int                       gladeIdi;
  int                       choicepointpMaxIndice;
  genericStack_t           *choicepointpFactoringStackp;
  
  if (choicepointp->factoringStackp == NULL) {
    goto done;
  }

  GENERICSTACK_NEW(stackp);
  if (GENERICSTACK_ERROR(stackp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize stackp");
    goto err;
  }

  choicepointpFactoringStackp = choicepointp->factoringStackp;
  choicepointpMaxIndice = GENERICSTACK_USED(choicepointpFactoringStackp) - 1;
  for (factorIxi = 0; factorIxi <= choicepointpMaxIndice; factorIxi++) {
    nookp = GENERICSTACK_GET_PTR(choicepointpFactoringStackp,  factorIxi);
    if (GENERICSTACK_ERROR(choicepointpFactoringStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Not a pointer at indice %d of factoringStackp", factorIxi);
      goto err;
    }
    if (_marpaWrapperAsf_nook_has_semantic_causeb(marpaWrapperAsfp, nookp) == 0) {
      continue;
    }

    orNodeIdi = nookp->orNodeIdi;
    orNodep = GENERICSTACK_GET_PTR(orNodeStackp, orNodeIdi);
    if (GENERICSTACK_ERROR(orNodeStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Not a pointer at indice %d of orNodeStackp", orNodeIdi);
      goto err;
    }
    nAndNodei = orNodep->nAndNodei;
    andNodep = orNodep->andNodep;
    GENERICSTACK_NEW(andNodeIdStackp);
    if (GENERICSTACK_ERROR(andNodeIdStackp)) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize andNodeIdStackp");
      goto err;
    }
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "nook of OR_NODE %d first choice and last choice are %d and %d", nookp->orNodeIdi, nookp->firstChoicei, nookp->lastChoicei);
    for (choicei = nookp->firstChoicei; choicei <= nookp->lastChoicei; choicei++) {
      if ((choicei < 0) || (choicei > nAndNodei)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "andNode %d out of range [%d..%d[", choicei, 0, nAndNodei);
	goto err;
      }
      GENERICSTACK_PUSH_INT(andNodeIdStackp, andNodep[choicei]);
      if (GENERICSTACK_ERROR(andNodeIdStackp)) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "generic stack push failure to andNodeIdStackp, %s", strerror(errno));
	goto err;
      }
    }
    GENERICSTACK_FREE(causeNidsStackp);
    if (_marpaWrapperAsf_and_nodes_to_cause_nidsp(marpaWrapperAsfp, andNodeIdStackp, &causeNidsStackp) == 0) {
      goto err;
    }
    GENERICSTACK_FREE(andNodeIdStackp);

    if (GENERICSTACK_USED(causeNidsStackp) <= 0) {
      causeNidsp = NULL;
    } else {
      causeNidsp = malloc(GENERICSTACK_USED(causeNidsStackp) * sizeof(int));
      if (causeNidsp == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
	goto err;
      } else {
	int i;

	for (i = 0; i < GENERICSTACK_USED(causeNidsStackp); i++) {
	  causeNidsp[i] = GENERICSTACK_GET_INT(causeNidsStackp, i);
	  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... causeNidsp[%d] = %d", i, causeNidsp[i]);
	}
      }
    }
    baseNidsetp = _marpaWrapperAsf_nidset_obtainp(marpaWrapperAsfp, GENERICSTACK_USED(causeNidsStackp), causeNidsp);
    if (causeNidsp != NULL) {
      free(causeNidsp);
      causeNidsp = NULL;
    }
    if (baseNidsetp == NULL) {
      goto err;
    }
    gladeIdi = _marpaWrapperAsf_nidset_idi(marpaWrapperAsfp, baseNidsetp);
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... gladeIdi = %d", gladeIdi);
    if (! GENERICSTACK_IS_PTR(gladeStackp,  gladeIdi)) {
      marpaWrapperAsfGlade_t *gladep;

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Generating glade at indice %d", gladeIdi);
      gladep = malloc(sizeof(marpaWrapperAsfGlade_t));
      if (gladep == NULL) {
        MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
        goto err;
      }
      gladep->idi           = gladeIdi;
      gladep->symchesStackp = NULL;
      gladep->visitedb      = 0;
      gladep->registeredb   = 1;
      GENERICSTACK_SET_PTR(gladeStackp, gladep,  gladeIdi);
      if (GENERICSTACK_ERROR(gladeStackp)) {
        MARPAWRAPPER_ERROR(genericLoggerp, "Failure to set in gladeStackp");
        free(gladep);
        goto err;
      }
    } else {
      marpaWrapperAsfGlade_t *gladep;

      gladep = GENERICSTACK_GET_PTR(gladeStackp,  gladeIdi);
      gladep->registeredb = 1;
    }
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
  _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "Glade id factors", *stackpp);
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
int _marpaWrapperAsf_indAndNodesi(void *userDatavp, genericStackItemType_t itemType, void **pp)
/****************************************************************************/
{
  return abs(* ((int *) pp)) % MARPAWRAPPERASF_NODE_HASH_SIZE;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_and_nodes_to_cause_nidsp(marpaWrapperAsf_t *marpaWrapperAsfp, genericStack_t *andNodeIdStackp, genericStack_t **causeNidsStackpp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_and_nodes_to_cause_nidsp";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  genericStack_t           *causeNidsStackp         = NULL;
  genericHash_t            *causesHashp             = NULL;
  int                       i;
  int                       andNodeIdi;
  int                       causeNidi;
  int                       goti;
  short                     findResultb;

  GENERICSTACK_NEW(causeNidsStackp);
  if (GENERICSTACK_ERROR(causeNidsStackp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize causeNidsStackp");
    goto err;
  }
  
  GENERICHASH_NEW(causesHashp, _marpaWrapperAsf_indAndNodesi);
  if (GENERICHASH_ERROR(causesHashp)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Failure to initalize causesHashp");
    goto err;
  }

  for (i = 0; i < GENERICSTACK_USED(andNodeIdStackp); i++) {
    if (! GENERICSTACK_IS_INT(andNodeIdStackp, i)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Not an int at indice %d andNodeIdStackp", i);
      goto err;
    }
    andNodeIdi = GENERICSTACK_GET_INT(andNodeIdStackp, i);
    causeNidi = (int) _marpa_b_and_node_cause(marpaBocagep, andNodeIdi);
    if (causeNidi < 0) {
      causeNidi = _marpaWrapperAsf_and_node_to_nidi(andNodeIdi);
    }
    GENERICHASH_FIND(causesHashp,
                     marpaWrapperAsfp,
                     INT,
                     causeNidi,
                     INT,
                     &goti,
                     findResultb);
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
      GENERICHASH_SET(causesHashp,
                      marpaWrapperAsfp,
                      INT,
                      causeNidi,
                      INT,
                      1);
      if (GENERICHASH_ERROR(causesHashp)) {
        MARPAWRAPPER_ERROR(genericLoggerp, "Error setting into causesHashp");
        goto err;
      }
    }
  }
  
  GENERICHASH_FREE(causesHashp, marpaWrapperAsfp);
  *causeNidsStackpp = causeNidsStackp;
  _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "<============= andNodeIdStackp", andNodeIdStackp);
  _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "=============> causeNidsStackp", causeNidsStackp);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *causeNidsStackpp=%p", *causeNidsStackpp);
  return 1;

 err:
  GENERICHASH_FREE(causesHashp, marpaWrapperAsfp);
  GENERICSTACK_FREE(causeNidsStackp);
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_next_factoringb(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp, int nidOfChoicePointi, short *factoringbp)
/****************************************************************************/
{
  const static char         funcs[]                 = "_marpaWrapperAsf_next_factoringb";
  genericLogger_t          *genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  short                     factoringb              = 0;

  if (choicepointp->factoringStackp == NULL) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Attempt to iterate factoring of uninitialized checkpoint");
      goto err;
  }

  while (1) {
    short factoringIterateb;
    short factoringFinishb;
    
    if (_marpaWrapperAsf_factoring_iterateb(marpaWrapperAsfp, choicepointp, &factoringIterateb) == 0) {
      goto err;
    }
    if (factoringIterateb == 0) {
      break;
    }
    if (_marpaWrapperAsf_factoring_finishb(marpaWrapperAsfp, choicepointp, nidOfChoicePointi, &factoringFinishb) == 0) {
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
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  short                    rcb            = 0;
  genericStack_t          *gladeStackp    = marpaWrapperAsfp->gladeStackp;
  marpaWrapperAsfGlade_t  *gladep;

  if (GENERICSTACK_IS_PTR(gladeStackp,  gladeIdi)) {

    gladep = GENERICSTACK_GET_PTR(gladeStackp,  gladeIdi);
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
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *gladeStackp    = marpaWrapperAsfp->gladeStackp;
  marpaWrapperAsfGlade_t  *gladep;
  int                      i;

  if (gladeIdip != NULL) {
    if (GENERICSTACK_IS_PTR(gladeStackp,  *gladeIdip)) {

      gladep = GENERICSTACK_GET_PTR(gladeStackp,  *gladeIdip);
      gladep->visitedb = 0;
    }
  } else {
    for (i = 0; i < GENERICSTACK_USED(gladeStackp); i++) {
      if (GENERICSTACK_IS_PTR(gladeStackp, i)) {

	gladep = GENERICSTACK_GET_PTR(gladeStackp, i);
	gladep->visitedb = 0;
      }
    }
  }
}

/****************************************************************************/
static inline short _marpaWrapperAsf_glade_symch_countb(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi, int *countip)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_symch_countb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  short                    rcb            = 0;
  marpaWrapperAsfGlade_t  *gladep;

  gladep = _marpaWrapperAsf_glade_obtainp(marpaWrapperAsfp, gladeIdi);
  if (gladep == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No glade found for glade ID %d", gladeIdi);
    goto err;
  }

  *countip = GENERICSTACK_USED(gladep->symchesStackp);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d, *countip=%d", *countip);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_glade_symbol_idi(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_symbol_idb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *nidsetStackp   = marpaWrapperAsfp->nidsetStackp;
  short                    rcb            = 0;
  int                      nid0;
  marpaWrapperAsfNidset_t *nidsetp;
  int                      symbolIdi;

  if (! GENERICSTACK_IS_PTR(nidsetStackp,  gladeIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No glade found for glade ID %d", gladeIdi);
    goto err;
  }
  nidsetp = GENERICSTACK_GET_PTR(nidsetStackp,  gladeIdi);

  if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, nidsetp, 0, &nid0) == 0) {
    goto err;
  }
  
  symbolIdi = _marpaWrapperAsf_nid_symbol_idi(marpaWrapperAsfp, nid0);
  if (symbolIdi < 0) {
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", symbolIdi);
  return symbolIdi;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
}

/****************************************************************************/
static inline int _marpaWrapperAsf_glade_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int gladeIdi)
/****************************************************************************/
{
  const static char        funcs[]        = "_marpaWrapperAsf_glade_spanb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericStack_t          *nidsetStackp   = marpaWrapperAsfp->nidsetStackp;
  short                    rcb            = 0;
  int                      spanIdi        = -1;
  int                      nid0;
  marpaWrapperAsfNidset_t *nidsetp;

  if (! GENERICSTACK_IS_PTR(nidsetStackp,  gladeIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No glade found for glade ID %d", gladeIdi);
    goto err;
  }
  nidsetp = GENERICSTACK_GET_PTR(nidsetStackp,  gladeIdi);

  if (_marpaWrapperAsf_nidset_idi_by_ixib(marpaWrapperAsfp, nidsetp, 0, &nid0) == 0) {
    goto err;
  }
  
  spanIdi = _marpaWrapperAsf_nid_spani(marpaWrapperAsfp, nid0);
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", spanIdi);
  return spanIdi;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
}

/****************************************************************************/
int marpaWrapperAsf_traverse_rh_lengthi(marpaWrapperAsfTraverser_t *traverserp)
/****************************************************************************/
{
  const static char         funcs[] = "marpaWrapperAsf_traverse_rh_lengthi";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  marpaWrapperAsfGlade_t   *gladep;
  int                       symchIxi;
  genericStack_t           *factoringsStackp;
  genericStack_t           *downFactoringsStackp;
  int                       downRuleIdi;
  int                       factoringIxi;
  int                       realFactoringIxi;
  int                       lengthi;

  if (traverserp == NULL) {
    errno = EINVAL;
    return -1;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  symchIxi = traverserp->symchIxi;
  /* symchesStackp is a stack of factoringsStack */
  if (! GENERICSTACK_IS_PTR(gladep->symchesStackp,  symchIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No symch at indice %d of current glade", symchIxi);
    goto err;
  }
  factoringsStackp = GENERICSTACK_GET_PTR(gladep->symchesStackp, symchIxi);
  /* factoringsStackp is (symchRuleIdi, PTR, downFactoringsStackp[0], downFactoringsStackp[1], ... ) */
  if (! GENERICSTACK_IS_INT(factoringsStackp, 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not an integer at indice 0 of factoringsStackp", 0);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "symchesStackp", gladep->symchesStackp);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "factoringsStackp", factoringsStackp);
    goto err;
  }

  factoringIxi = traverserp->factoringIxi;
  realFactoringIxi = 2 + factoringIxi;
  if (! GENERICSTACK_IS_PTR(factoringsStackp, realFactoringIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not a pointer at indice 2+%d of factoringsStackp", factoringIxi);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "symchesStackp", gladep->symchesStackp);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "factoringsStackp", factoringsStackp);
    goto err;
  }
  downRuleIdi          = GENERICSTACK_GET_INT(factoringsStackp, 0);
  downFactoringsStackp = GENERICSTACK_GET_PTR(factoringsStackp, realFactoringIxi);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "downRuleIdi is %d", downRuleIdi);

  if (downRuleIdi < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "%s called for a token -- that is not allowed", funcs);
    goto err;
  }

  /* downFactoringsStackp is a stack of gladeIdi */
  lengthi = GENERICSTACK_USED(downFactoringsStackp);
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, lengthl = %d", lengthi);
  return lengthi;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
}

/****************************************************************************/
short marpaWrapperAsf_traverse_rh_valueb(marpaWrapperAsfTraverser_t *traverserp, int rhIxi, int *valueip)
/****************************************************************************/
{
  const static char           funcs[]         = "marpaWrapperAsf_traverse_rh_valueb";
  marpaWrapperAsfTraverser_t *childTraverserp = NULL;
  int                         valuei;
  marpaWrapperAsf_t          *marpaWrapperAsfp;
  marpaWrapperAsfGlade_t     *gladep;
  genericLogger_t            *genericLoggerp;
  int                         symchIxi;
  genericStack_t             *factoringsStackp;
  genericStack_t             *downFactoringsStackp;
  int                         ruleIdi;
  int                         factoringIxi;
  int                         realFactoringIxi;
  int                         maxRhixi;
  marpaWrapperAsfGlade_t     *downGladep;
  int                         downGladeIdi;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current glade id is %d", gladep->idi);
  symchIxi = traverserp->symchIxi;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current symchIxi is %d", symchIxi);
  /* symchesStackp is a stack of factoringsStack */
  if (! GENERICSTACK_IS_PTR(gladep->symchesStackp,  symchIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No symch at indice %d of current glade", symchIxi);
    goto err;
  }
  factoringsStackp = GENERICSTACK_GET_PTR(gladep->symchesStackp, symchIxi);
  /* factoringsStackp is (symchRuleIdi, PTR, downFactoringsStackp[0], downFactoringsStackp[1], ... ) */
  if (! GENERICSTACK_IS_INT(factoringsStackp, 0)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not an integer at indice 0 of factoringsStackp", 0);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "symchesStackp", gladep->symchesStackp);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "factoringsStackp", factoringsStackp);
    goto err;
  }

  /* symchesStackp contains entries in the form: (rule_id, xxx, @factorings) */
  ruleIdi          = GENERICSTACK_GET_INT(factoringsStackp, 0);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current rule id is %d", ruleIdi);

  if (ruleIdi < 0) {
    marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
    int                       spanIdi;
    
    /* This is a token: its value is its span ID, and this is working only if no alternative */
    /* have a length > 1 */
    if (marpaWrapperRecognizerp->haveVariableLengthTokenb != 0) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "%s called for a token -- that is not allowed if there is at least one variablen length token", funcs);
      goto err;
    }
    spanIdi = _marpaWrapperAsf_glade_spani(marpaWrapperAsfp, gladep->idi);
    if (spanIdi < 0) {
      goto err;
    }
    /* The span ID is the indice in the input stack */
    valuei = spanIdi;

    goto ok;
  }

  factoringIxi = traverserp->factoringIxi;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current factoringIxi is %d", factoringIxi);

  realFactoringIxi = 2 + factoringIxi;
  if (! GENERICSTACK_IS_PTR(factoringsStackp, realFactoringIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Not a pointer at indice 2+%d of factoringsStackp", factoringIxi);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "symchesStackp", gladep->symchesStackp);
    _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "factoringsStackp", factoringsStackp);
    goto err;
  }
  downFactoringsStackp = GENERICSTACK_GET_PTR(factoringsStackp, realFactoringIxi);

  _marpaWrapperAsf_dump_stack(marpaWrapperAsfp, "downFactoringsStackp", downFactoringsStackp);
  maxRhixi = GENERICSTACK_USED(downFactoringsStackp);
  /*
   * Nullables have no RHS
   */
  if (maxRhixi <= 0) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Nullable case");
    goto ok;
  }
  --maxRhixi;
  if (rhIxi > maxRhixi) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "rhIxi should be in range [0..%d]", maxRhixi);
    goto err;
  }
  if (! GENERICSTACK_IS_INT(downFactoringsStackp, rhIxi)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Not an int in downFactoringsStackp");
    goto err;
  }
  downGladeIdi = GENERICSTACK_GET_INT(downFactoringsStackp, rhIxi);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current downGladeIdi is %d", downGladeIdi);
  if (GENERICSTACK_IS_INT(traverserp->valueStackp,  downGladeIdi)) {
    /* Already memoized */
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Memoized value");
    valuei = GENERICSTACK_GET_INT(traverserp->valueStackp, downGladeIdi);
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
  GENERICSTACK_NEW(childTraverserp->valueStackp);
  if (GENERICSTACK_ERROR(childTraverserp->valueStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "childTraverserp->valueStackp initialization failure error, %s", strerror(errno));
    goto err;   
  }
  childTraverserp->marpaWrapperAsfp = marpaWrapperAsfp;
  childTraverserp->gladep           = downGladep;
  childTraverserp->symchIxi         = 0;
  childTraverserp->factoringIxi     = 0;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Calling traverser for downglade %d", downGladep->idi);
  if (! marpaWrapperAsfp->traverserCallbackp(childTraverserp, marpaWrapperAsfp->userDatavp, &valuei)) {
    goto err;
  }
  GENERICSTACK_FREE(childTraverserp->valueStackp);
  free(childTraverserp);
  childTraverserp = NULL;

  GENERICSTACK_SET_INT(traverserp->valueStackp, valuei, downGladeIdi);
  if (GENERICSTACK_ERROR(traverserp->valueStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Failure to set value in traverser", strerror(errno));
    goto err;   
  }

ok:
  if (valueip != NULL) {
    *valueip = valuei;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *valueip=%d", valuei);
  return 1;

 err:
  if (childTraverserp != NULL) {
    GENERICSTACK_FREE(childTraverserp->valueStackp);
    free(childTraverserp);
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperAsf_traverse_symbolIdb(marpaWrapperAsfTraverser_t *traverserp, int *symbolIdip)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_traverse_symbolIdb";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  marpaWrapperAsfGlade_t   *gladep;
  int                       gladeIdi;
  int                       symbolIdi;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  gladeIdi = gladep->idi;

  symbolIdi = _marpaWrapperAsf_glade_symbol_idi(marpaWrapperAsfp, gladeIdi);
  if (symbolIdi < 0) {
    goto err;
  }

  if (symbolIdip != NULL) {
    *symbolIdip = symbolIdi;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *symbolIdip=%d", symbolIdi);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperAsf_traverse_ruleIdb(marpaWrapperAsfTraverser_t *traverserp, int *ruleIdip)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_traverse_ruleIdb";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericStack_t           *symchesStackp;
  genericLogger_t          *genericLoggerp;
  marpaWrapperAsfGlade_t   *gladep;
  int                       symchIxi;
  genericStack_t           *factoringsStackp;
  int                       ruleIdi;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  symchIxi = traverserp->symchIxi;
  symchesStackp = gladep->symchesStackp;
  if (! GENERICSTACK_IS_PTR(symchesStackp,  symchIxi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "No symch at indice %d", symchIxi);
    goto err;
  }
  factoringsStackp = GENERICSTACK_GET_PTR(symchesStackp,  symchIxi);
  if (factoringsStackp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Null factorings stack at symch indice %d", symchIxi);
    goto err;
  }
  ruleIdi = GENERICSTACK_GET_INT(factoringsStackp, 0);

  if (ruleIdip != NULL) {
    *ruleIdip = ruleIdi;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *ruleIdip=%d", ruleIdi);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_traverse_nextFactoringb(marpaWrapperAsfTraverser_t *traverserp, int *factoringIxip)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_traverse_nextFactoringb";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  marpaWrapperAsfGlade_t   *gladep;
  int                       gladeIdi;
  int                       symchIxi;
  int                       lastFactoringi;
  int                       countFactoringi;
  int                       factoringIxi;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  /* Never happens */
  /*
  if (factoringIxip == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "factoringIxip is NULL");
    goto err;
  }
  */

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  gladeIdi = gladep->idi;
  symchIxi = traverserp->symchIxi;

  if (_marpaWrapperAsf_symch_factoring_countb(marpaWrapperAsfp, gladeIdi, symchIxi, &countFactoringi) == 0) {
    goto err;
  }
  lastFactoringi = countFactoringi - 1;

  factoringIxi = traverserp->factoringIxi;
  if (factoringIxi >= lastFactoringi) {
    /* This is not formally an internal error: user asked for the next factoring and there is none */
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current factoringIxi %d is >= last factoring indice %d", factoringIxi, lastFactoringi);
    *factoringIxip = -1;
  } else {
    *factoringIxip = traverserp->factoringIxi = ++factoringIxi;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *factoringIxip=%d", *factoringIxip);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_traverse_nextSymchb(marpaWrapperAsfTraverser_t *traverserp, int *symchIxip)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_traverse_nextSymchb";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  marpaWrapperAsfGlade_t   *gladep;
  int                       gladeIdi;
  int                       symchIxi;
  int                       lastSymchi;
  int                       counti;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  if (symchIxip == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "symchIxip is NULL");
    goto err;
  }

  gladep = traverserp->gladep;
  if (gladep == NULL) {
    MARPAWRAPPER_ERROR(genericLoggerp, "Current glade is NULL");
    goto err;
  }
  gladeIdi = gladep->idi;
  symchIxi = traverserp->symchIxi;

  if (_marpaWrapperAsf_glade_symch_countb(marpaWrapperAsfp, gladeIdi, &counti) == 0) {
    goto err;
  }
  lastSymchi = counti - 1;

  if (symchIxi >= lastSymchi) {
    /* This is not formally an internal error: this is part of calls behind nextb() */
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Current symchIxi %d is >= last symch indice %d", symchIxi, lastSymchi);
    *symchIxip = -1;
  } else {
    *symchIxip = traverserp->symchIxi = ++symchIxi;
    traverserp->factoringIxi = 0;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *symchIxip=%d", *symchIxip);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperAsf_traverse_nextb(marpaWrapperAsfTraverser_t *traverserp, short *nextbp)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_traverse_nextb";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;
  int                       idi;
  short                     nextb = 1;

  if (traverserp == NULL) {
    errno = EINVAL;
    return 0;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  if (_marpaWrapperAsf_traverse_nextFactoringb(traverserp, &idi) == 0) {
    goto err;
  }
  if (idi < 0) {
    if (_marpaWrapperAsf_traverse_nextSymchb(traverserp, &idi) == 0) {
      goto err;
    }
    if (idi < 0) {
      nextb = 0;
    }
  }

  if (nextbp != NULL) {
    *nextbp = nextb;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1, *nextbp=%d", (int) nextb);
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
marpaWrapperAsf_t *marpaWrapperAsf_traverse_asfp(marpaWrapperAsfTraverser_t *traverserp)
/****************************************************************************/
{
  const static char         funcs[]          = "marpaWrapperAsf_traverse_asfp";
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  genericLogger_t          *genericLoggerp;

  if (traverserp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  marpaWrapperAsfp = traverserp->marpaWrapperAsfp;
  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperAsfp);
  return marpaWrapperAsfp;
}

/****************************************************************************/
marpaWrapperRecognizer_t *marpaWrapperAsf_recognizerp(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char         funcs[]                 = "marpaWrapperAsf_recognizerp";
  genericLogger_t          *genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp;

  if (marpaWrapperAsfp == NULL) {
    errno = EINVAL;
    return NULL;
  }

  genericLoggerp          = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperRecognizerp);
  return marpaWrapperRecognizerp;
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
int _marpaWrapperAsf_intset_keyIndFunctioni(void *userDatavp, genericStackItemType_t itemType, void **pp)
/****************************************************************************/
{
#ifndef MARPAWRAPPER_NTRACE
  const static char  funcs[]          = "_marpaWrapperAsf_intset_keyIndFunctioni";
  marpaWrapperAsf_t *marpaWrapperAsfp = (marpaWrapperAsf_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
#endif
  /* *pp is an array of int, with size at indice 0                                    */
  int   *idip = (int *) *pp;
  int    sizi = idip[0];
  int    sumi = 0;
  int    i;
  int    rci;

  for (i = 1; i <= sizi; i++) {
    sumi ^= idip[i];
#ifndef MARPAWRAPPER_NTRACE
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "idip[%d]=%d => sumi=%d", i, idip[i], sumi);
#endif
  }

  rci = MARPAWRAPPERASF_INTSET_MODULO(sumi);
#ifndef MARPAWRAPPER_NTRACE
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "sumi=%d => indice %d", i, sumi, rci);
#endif

  return rci;
}

/****************************************************************************/
short _marpaWrapperAsf_intset_keyCmpFunctionb(void *userDatavp, void **pp1, void **pp2)
/****************************************************************************/
{
#ifndef MARPAWRAPPER_NTRACE
  const static char  funcs[]          = "_marpaWrapperAsf_intset_keyCmpFunctionb";
  marpaWrapperAsf_t *marpaWrapperAsfp = (marpaWrapperAsf_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
#endif
  int *idi1p = (int *) *pp1;
  int *idi2p = (int *) *pp2;
  int  siz1i = idi1p[0];
  int  siz2i = idi2p[0];
  int  i;

#ifndef MARPAWRAPPER_NTRACE
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "size1 %d == size2 %d ?", siz1i, siz2i);
#endif
  if (siz1i != siz2i) {
#ifndef MARPAWRAPPER_NTRACE
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
#endif
    return 0;
  }
  /* By definition elements are ordered */
  for (i = 1; i <= siz1i; i++) {
#ifndef MARPAWRAPPER_NTRACE
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "idi1[%d] %d == idi2[%d] %d ?", i, idi1p[i], i, idi2p[i]);
#endif
    if (idi1p[i] != idi2p[i]) {
#ifndef MARPAWRAPPER_NTRACE
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
#endif
      return 0;
    }
  }

#ifndef MARPAWRAPPER_NTRACE
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
#endif
  return 1;
}

/****************************************************************************/
void _marpaWrapperAsf_intset_keyFreeFunctionv(void *userDatavp, void **pp)
/****************************************************************************/
{
  free((int *) *pp);
}

/****************************************************************************/
static inline int _marpaWrapperAsf_or_node_es_spani(marpaWrapperAsf_t *marpaWrapperAsfp, int choicepointi, int *lengthip)
/****************************************************************************/
{
  const static char        funcs[]                  = "_marpaWrapperAsf_or_node_es_spani";
  genericLogger_t         *genericLoggerp           = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp = marpaWrapperAsfp->marpaWrapperRecognizerp;
  marpaWrapperGrammar_t    *marpaWrapperGrammarp    = marpaWrapperRecognizerp->marpaWrapperGrammarp;
  Marpa_Grammar             marpaGrammarp           = marpaWrapperGrammarp->marpaGrammarp;
  Marpa_Bocage              marpaBocagep            = marpaWrapperAsfp->marpaBocagep;
  int                       originEsi;
  int                       currentEsi;
  int                       spanIdi;
  int                       lengthi;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_origin(%p, %d)", marpaBocagep, choicepointi);
  originEsi = _marpa_b_or_node_origin(marpaBocagep, choicepointi);
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "_marpa_b_or_node_set(%p, %d)", marpaBocagep, choicepointi);
  currentEsi = _marpa_b_or_node_set(marpaBocagep, choicepointi);
  
  spanIdi = originEsi;
  lengthi = currentEsi - originEsi;

  if (lengthip != NULL) {
    *lengthip = lengthi;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return spanIdi=%d, *lengthip=%d", spanIdi, lengthi);
  return spanIdi;
}

/****************************************************************************/
static inline void _marpaWrapperAsf_dump_stack(marpaWrapperAsf_t *marpaWrapperAsfp, char *what, genericStack_t *stackp)
/****************************************************************************/
{
  const static char        funcs[]                  = "_marpaWrapperAsf_dump_stack";
  genericLogger_t         *genericLoggerp           = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  int                      i;

  if (what == NULL) {
    what = "uncategorized";
  }
  
  if (stackp == NULL) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s stack is NULL", what);
  } else {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%s stack length is %d", what, GENERICSTACK_USED(stackp));
    for (i = 0; i < GENERICSTACK_USED(stackp); i++) {
      switch (GENERICSTACKITEMTYPE(stackp, i)) {
      case GENERICSTACKITEMTYPE_NA:
	MARPAWRAPPER_TRACE(genericLoggerp, funcs, "... stackp[%d] is NA");
	break;
      case GENERICSTACKITEMTYPE_CHAR:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is CHAR: '%c' (0x%d)", i, GENERICSTACK_GET_CHAR(stackp, i), (int) GENERICSTACK_GET_CHAR(stackp, i));
	break;
      case GENERICSTACKITEMTYPE_SHORT:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is SHORT: %d", i, (int) GENERICSTACK_GET_SHORT(stackp, i));
	break;
      case GENERICSTACKITEMTYPE_INT:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is INT: %d", i, GENERICSTACK_GET_INT(stackp, i));
	break;
      case GENERICSTACKITEMTYPE_LONG:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is LONG: %ld", i, GENERICSTACK_GET_LONG(stackp, i));
	break;
      case GENERICSTACKITEMTYPE_FLOAT:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is FLOAT: %f", i, (double) GENERICSTACK_GET_FLOAT(stackp, i));
	break;
      case GENERICSTACKITEMTYPE_DOUBLE:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is DOUBLE: %f", i, GENERICSTACK_GET_DOUBLE(stackp, i));
	break;
      case GENERICSTACKITEMTYPE_PTR:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is PTR: %p", i, GENERICSTACK_GET_PTR(stackp, i));
	break;
      default:
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... stackp[%d] is unknown", i);
	break;
      }
    }
  }
}

/****************************************************************************/
static inline marpaWrapperAsfChoicePoint_t *_marpaWrapperAsf_choicepoint_newp(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char             funcs[]        = "_marpaWrapperAsf_choicepoint_newp";
  genericLogger_t              *genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperAsfChoicePoint_t *choicepointp;

  choicepointp = malloc(sizeof(marpaWrapperAsfChoicePoint_t));
  if (choicepointp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  choicepointp->factoringStackp   = NULL;
  GENERICSTACK_NEW(choicepointp->orNodeInUseStackp);
  if (GENERICSTACK_ERROR(choicepointp->orNodeInUseStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "orNodeInUseStackp initialization failure: %s", strerror(errno));
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", choicepointp);
  return choicepointp;

 err:
  if (choicepointp != NULL) {
    GENERICSTACK_FREE(choicepointp->orNodeInUseStackp);
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return NULL");
  return NULL;
}

/****************************************************************************/
static inline void _marpaWrapperAsf_choicepoint_freev(marpaWrapperAsf_t *marpaWrapperAsfp, marpaWrapperAsfChoicePoint_t *choicepointp)
/****************************************************************************/
{
  if (choicepointp != NULL) {
    _marpaWrapperAsf_factoringStackp_freev(marpaWrapperAsfp, &(choicepointp->factoringStackp));
    GENERICSTACK_FREE(choicepointp->orNodeInUseStackp);
    free(choicepointp);
  }
}

/****************************************************************************/
short marpaWrapperAsf_prunedValueb(marpaWrapperAsf_t                    *marpaWrapperAsfp,
                                   void                                 *userDatavp,
                                   marpaWrapperAsfOkSymbolCallback_t     okSymbolCallbackp,
                                   marpaWrapperAsfOkRuleCallback_t       okRuleCallbackp,
                                   marpaWrapperValueRuleCallback_t       valueRuleCallbackp,
                                   marpaWrapperValueSymbolCallback_t     valueSymbolCallbackp,
                                   marpaWrapperValueNullingCallback_t    valueNullingCallbackp)
/****************************************************************************/
{
  const static char             funcs[] = "marpaWrapperAsf_prunedValueb";
  genericLogger_t              *genericLoggerp;
  short                         rcb;
  marpaWrapperAsfValueContext_t marpaWrapperAsfValueContext;
  int                           valuei;

  if (marpaWrapperAsfp == NULL) {
    errno = EINVAL;
    return 0;
  }

  genericLoggerp = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  
  if ((okSymbolCallbackp == NULL)    ||
      (okRuleCallbackp == NULL)      ||
      (valueRuleCallbackp == NULL)   ||
      (valueSymbolCallbackp == NULL) ||
      (valueNullingCallbackp == NULL)) {
    MARPAWRAPPER_ERROR(genericLoggerp, "All callback function pointers must be non-NULL");
    errno = EINVAL;
    goto err;
  }
  
  marpaWrapperAsfValueContext.userDatavp            = userDatavp;
  marpaWrapperAsfValueContext.okSymbolCallbackp     = okSymbolCallbackp;
  marpaWrapperAsfValueContext.okRuleCallbackp       = okRuleCallbackp;
  marpaWrapperAsfValueContext.valueRuleCallbackp    = valueRuleCallbackp;
  marpaWrapperAsfValueContext.valueSymbolCallbackp  = valueSymbolCallbackp;
  marpaWrapperAsfValueContext.valueNullingCallbackp = valueNullingCallbackp;
  marpaWrapperAsfValueContext.parentRuleiStackp     = NULL;
  marpaWrapperAsfValueContext.freeOutputStacki      = 0;
  marpaWrapperAsfValueContext.wantedOutputStacki    = 0;
  marpaWrapperAsfValueContext.leveli                = 0;

  GENERICSTACK_NEW(marpaWrapperAsfValueContext.parentRuleiStackp);
  if (GENERICSTACK_ERROR(marpaWrapperAsfValueContext.parentRuleiStackp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "parentRuleiStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  rcb = marpaWrapperAsf_traverseb(marpaWrapperAsfp, _marpaWrapperAsf_valueTraverserb, &marpaWrapperAsfValueContext, &valuei);
  if (rcb) {
    if (valuei < 0) {
      /* All alternatives were rejected */
      goto err;
    }
  }
  goto done;

 err:
  rcb = 0;

 done:
  GENERICSTACK_FREE(marpaWrapperAsfValueContext.parentRuleiStackp);
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %d", (int) rcb);
  return rcb;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_valueTraverserb(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip)
/****************************************************************************/
/* Our traverser has the following semantics:                               */
/* *valueip is the wanted indice in output stack.                           */
/* 
/* It returns -1 if rejection                                               */
/*             0 if failure                                                 */
/*             1 if ok                                                      */
/* It makes sure that only a single value is possible at the end.           */
/* It is possible to reject both a symbol and a rule.                       */
/****************************************************************************/
{
  const static char                  funcs[]                      = "_marpaWrapperAsf_valueTraverserb";
  marpaWrapperAsf_t                 *marpaWrapperAsfp             = marpaWrapperAsf_traverse_asfp(traverserp);
  genericLogger_t                   *genericLoggerp               = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  marpaWrapperAsfValueContext_t     *marpaWrapperAsfValueContextp = (marpaWrapperAsfValueContext_t *) userDatavp;
  genericStack_t                    *parentRuleiStackp            = marpaWrapperAsfValueContextp->parentRuleiStackp;
  marpaWrapperAsfOkSymbolCallback_t  okSymbolCallbackp            = marpaWrapperAsfValueContextp->okSymbolCallbackp;
  marpaWrapperAsfOkRuleCallback_t    okRuleCallbackp              = marpaWrapperAsfValueContextp->okRuleCallbackp;
  marpaWrapperValueRuleCallback_t    valueRuleCallbackp           = marpaWrapperAsfValueContextp->valueRuleCallbackp;
  marpaWrapperValueSymbolCallback_t  valueSymbolCallbackp         = marpaWrapperAsfValueContextp->valueSymbolCallbackp;
  marpaWrapperValueNullingCallback_t valueNullingCallbackp        = marpaWrapperAsfValueContextp->valueNullingCallbackp;
  int                                wantedOutputStacki           = marpaWrapperAsfValueContextp->wantedOutputStacki;
  short                              rcb;
  int                                marpaRuleIdi;
  int                                marpaSymbolIdi;
  int                                spani;
  int                                tokenValuei;
  int                                lengthi;
  int                                rhIxi;
  short                              nextb;
  int                                nbAlternativeOki;
  int                                arg0i;
  int                                argni;

  marpaWrapperAsfValueContextp->leveli++;
  
  if (! marpaWrapperAsf_traverse_ruleIdb(traverserp, &marpaRuleIdi)) {
    goto err;
  }
  if (! marpaWrapperAsf_traverse_symbolIdb(traverserp, &marpaSymbolIdi)) {
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "[%3d] Rule %d, symbol %d, wanted indice in the output stack: %d", marpaWrapperAsfValueContextp->leveli, marpaRuleIdi, marpaSymbolIdi, wantedOutputStacki);

  if (marpaRuleIdi < 0) {
    /* This is a token */

    /* Get its value */
    if (! marpaWrapperAsf_traverse_rh_valueb(traverserp, 0, &spani)) {
      goto err;
    }
    tokenValuei = spani + 1;

    /* Check if it is ok */
    rcb = okSymbolCallbackp(marpaWrapperAsfValueContextp->userDatavp, marpaWrapperAsfValueContextp->parentRuleiStackp, marpaSymbolIdi, tokenValuei);
    if (rcb < 0) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Symbol No %d value callback says reject", marpaSymbolIdi);
      goto reject;
    } else if (rcb == 0) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Symbol No %d value callback says failure", marpaSymbolIdi);
      goto err;
    }

    if (! valueSymbolCallbackp(marpaWrapperAsfValueContextp->userDatavp, marpaSymbolIdi, tokenValuei, wantedOutputStacki)) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Symbol No %d value callback says failure", marpaSymbolIdi);
      goto err;
    }

    /* A token act as a single alternative */
    nbAlternativeOki = 1;
  } else {
    /* This is a rule */
    
    GENERICSTACK_PUSH_INT(parentRuleiStackp, marpaRuleIdi);
    if (GENERICSTACK_ERROR(parentRuleiStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "parentRuleiStackp push failure, %s", strerror(errno));
      goto err;
    }
    
    nbAlternativeOki = 0;
    while (1) {

      /* Check if it is ok */
      rcb = okRuleCallbackp(marpaWrapperAsfValueContextp->userDatavp, marpaWrapperAsfValueContextp->parentRuleiStackp, marpaRuleIdi);
      if (rcb < 0) {
        MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Rule No %d value callback says reject", marpaRuleIdi);
	if (nbAlternativeOki == 0) {
	  nbAlternativeOki = -1;
	}
        goto reject;
      } else if (rcb == 0) {
        MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Rule No %d value callback says failure", marpaRuleIdi);
        goto err;
      }

      /* Rule length */
      lengthi = marpaWrapperAsf_traverse_rh_lengthi(traverserp);
      if (lengthi < 0) {
        goto err;
      }

      /* Rule value */
      if (lengthi == 0) {
        
        /* This is a nulling rule */
        if (! valueNullingCallbackp(marpaWrapperAsfValueContextp->userDatavp, marpaRuleIdi, wantedOutputStacki)) {
          MARPAWRAPPER_ERRORF(genericLoggerp, "Rule No %d nulling callback failure", marpaRuleIdi);
          goto err;
        }
      } else {
        int   localWantedOutputStacki = marpaWrapperAsfValueContextp->freeOutputStacki + 1;
        short localRcb;
        
        /* We "reserve" the space in the stack for subsequent recursive rules */
        marpaWrapperAsfValueContextp->freeOutputStacki += lengthi;
        for (rhIxi = 0; rhIxi < lengthi; rhIxi++) {
          
          marpaWrapperAsfValueContextp->wantedOutputStacki = localWantedOutputStacki;
          localRcb = marpaWrapperAsf_traverse_rh_valueb(traverserp, rhIxi, &localWantedOutputStacki);
          marpaWrapperAsfValueContextp->wantedOutputStacki = wantedOutputStacki;
          
          if (! localRcb) {
            goto err;
          }
          if (localWantedOutputStacki < 0) {
            /* There is rejection below: go to next alternative */
            MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Rule No %d traversal is hitting a reject", marpaRuleIdi);
            /* We do not change next free indice in the output stack so that user can free what is now a garbage */
            goto nextRule;
          }
          localWantedOutputStacki++;
        }
        argni = --localWantedOutputStacki;
        arg0i = argni - (lengthi - 1);
        if (! valueRuleCallbackp(marpaWrapperAsfValueContextp->userDatavp, marpaRuleIdi, arg0i, argni, wantedOutputStacki)) {
          MARPAWRAPPER_ERRORF(genericLoggerp, "Rule No %d value callback failure", marpaSymbolIdi);
          goto err;
        }
      }

      /* Prune the number of accepted alternatives */
      if (++nbAlternativeOki == 1) {
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Rule No %d value callback success: ignoring other alternatives", marpaSymbolIdi);
	break;
      }

    nextRule:
      /* Check for another alternative */
      if (! marpaWrapperAsf_traverse_nextb(traverserp, &nextb)) {
        goto err;
      }
      if (! nextb) {
        break;
      }
      /* There is another alternative: get the ruleId */
      if (! marpaWrapperAsf_traverse_ruleIdb(traverserp, &marpaRuleIdi)) {
        goto err;
      }
    }

    GENERICSTACK_POP_INT(parentRuleiStackp);
    if (GENERICSTACK_ERROR(parentRuleiStackp)) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "parentRuleiStackp pop failure, %s", strerror(errno));
      goto err;
    }

  }

  if (nbAlternativeOki < 0) {
    /* All alternatives were rejected */
    wantedOutputStacki = -1;
  }
  rcb = 1;
  goto done;

 reject:
  /* Formally not an error, we indicate our caller this is a rejection */
  rcb = 1;
  wantedOutputStacki = -1;
  goto done;

 err:
  rcb = 0;

 done:
  if (rcb) {
    if (valueip != NULL) {
      *valueip = wantedOutputStacki;
    }
  }

#ifndef MARPAWRAPPER_NTRACE
  if (rcb) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "[%3d] return %d, *valueip=%d", marpaWrapperAsfValueContextp->leveli, (int) rcb, wantedOutputStacki);
  } else {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "[%3d] return %d", marpaWrapperAsfValueContextp->leveli, (int) rcb);
  }
#endif
  
  marpaWrapperAsfValueContextp->leveli--;

  return rcb;
}

#ifndef MARPAWRAPPER_NTRACE
/****************************************************************************/
static void _marpaWrapperAsf_dumpintsetHashpv(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  static const char  funcs[]         = "dumpStacks";
  genericLogger_t   *genericLoggerp  = marpaWrapperAsfp->marpaWrapperAsfOption.genericLoggerp;
  genericHash_t     *intsetHashp     = marpaWrapperAsfp->intsetHashp;
  int                i;
  int                j;
  int                k;
  int               *idip;
  int                nInRowi;

  for (i = 0; i < GENERICSTACK_USED(intsetHashp->keyStackp); i++) {
    genericStack_t *subStackp;

    if (! GENERICSTACK_IS_PTR(intsetHashp->keyStackp, i)) {
      continue;
    }

    subStackp = GENERICSTACK_GET_PTR(intsetHashp->keyStackp, i);
    nInRowi = 0;
    for (j = 0; j < GENERICSTACK_USED(subStackp); j++) {

      if (! GENERICSTACK_IS_PTR(subStackp, j)) {
	continue;
      }
      ++nInRowi;
      idip = GENERICSTACK_GET_PTR(subStackp, j);
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Row %6d column %6d", i, j);
      for (k = 1; k <= idip[0]; k++) {
	MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "   %d", idip[k]);
      }
    }
    if (nInRowi > 1) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Row %6d has %d collisions", i, nInRowi);
    }
  }
}
#endif
