#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/manageBuf.h"
#include "marpaWrapper/internal/asf.h"
#include "marpaWrapper/internal/recognizer.h"
#include "marpaWrapper/internal/grammar.h"
#include "marpaWrapper/internal/logging.h"

#ifndef MARPAWRAPPERASF_INTSET_MAXROWS
#define MARPAWRAPPERASF_INTSET_MAXROWS 65536
#endif

#undef MARPAWRAPPERASF_INTSET_MODULO
#define MARPAWRAPPERASF_INTSET_MODULO (MARPAWRAPPERASF_INTSET_MAXROWS - 1)

#ifdef C_VA_COPY
/* Used when passing a va_list on the stack to another function taking a va_list */
#define REAL_AP ap2
#else
#define REAL_AP ap
#endif

static marpaWrapperAsfOption_t marpaWrapperAsfOptionDefault = {
  NULL,   /* genericLoggerp */
  42,     /* factoringMaxi  */
   1,     /* highRankOnlyb */
   1,     /* orderByRankb */
};

typedef struct marpaWrapperAfsAndNodeIdAndPredecessorId {
  Marpa_And_Node_ID andNodeIdi;
  Marpa_And_Node_ID andNodePredecessorIdi;
} marpaWrapperAfsAndNodeIdAndPredecessorId_t;

static inline short                    _marpaWrapperAsf_peak(marpaWrapperAsf_t *marpaWrapperAsfp);
static inline int                      _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi(const void *p1, const void *p2);
static inline marpaWrapperAsfNidset_t *_marpaWrapperAsf_nidsetObtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t nIdl, int *idip);
static inline short                    _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, size_t nIdl, int *idip);
static inline char                    *_marpaWrapperAsf_stringBuilders(char *fmts, ...);
static inline char                    *_marpaWrapperAsf_stringBuilder_aps(char *fmts, va_list ap);
static inline short                    _marpaWrapperAsf_cmpIntsetFunction(void *userDatavp, genericStackItemType_t itemType, void *p1, void *p2);
static inline unsigned long            _marpaWrapperAsf_djb2(unsigned char *str);
static inline int                      _marpaWrapperAsf_idCmpi(const void *p1, const void *p2);

/****************************************************************************/
marpaWrapperAsf_t *marpaWrapperAsf_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperAsfOption_t *marpaWrapperAsfOptionp)
/****************************************************************************/
{
  const static char                           funcs[] = "marpaWrapperAsf_newp";
  marpaWrapperAsf_t                          *marpaWrapperAsfp;
  genericLogger_t                            *genericLoggerp;
  Marpa_Earley_Set_ID                         marpaLatestEarleySetIdi;
  int                                         highRankOnlyFlagi;
  int                                         ambiguousi;
  int                                         nulli;
  int                                         orNodeAndNodeCounti;
  Marpa_Or_Node_ID                            orNodei;
  int                                         ix;
  Marpa_And_Node_ID                           andNodeIdi;
  size_t                                      nAndNodeIdl;
  size_t                                      sizeAndNodeIdAndPredecessorIdl = 0;
  size_t                                      nAndNodeIdAndPredecessorIdl = 0;
  marpaWrapperAfsAndNodeIdAndPredecessorId_t *andNodeIdAndPredecessorIdp = NULL;
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
  marpaWrapperAsfp->sizeAndNodeIdl          = 0;
  marpaWrapperAsfp->nAndNodeIdl             = 0;
  marpaWrapperAsfp->andNodeIdip             = NULL;
  marpaWrapperAsfp->sizeOrNodeAndNodeIdl    = 0;
  marpaWrapperAsfp->nOrNodeAndNodeIdl       = 0;
  marpaWrapperAsfp->orNodeAndNodeIdipp      = NULL;
  marpaWrapperAsfp->intsetHashp             = NULL;
  marpaWrapperAsfp->sizeNidsetl             = 0;
  marpaWrapperAsfp->nNidsetl                = 0;
  marpaWrapperAsfp->nidsetp                 = 0;
  marpaWrapperAsfp->nextIntseti             = 0;

  GENERICHASH_NEW_SIZED(marpaWrapperAsfp->intsetHashp, NULL, _marpaWrapperAsf_cmpIntsetFunction, 1, MARPAWRAPPERASF_INTSET_MAXROWS, 0);
  if (GENERICHASH_ERROR(marpaWrapperAsfp->intsetHashp)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Intset hash initialization error, %s", strerror(errno));
    goto err;
  }

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
  nAndNodeIdl = 0;
  for (orNodei = 0; ; orNodei++) {
    orNodeAndNodeCounti = _marpa_o_or_node_and_node_count(marpaWrapperAsfp->marpaOrderp, orNodei);
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "OR Node Id %d says there are %d AND nodes", orNodei, orNodeAndNodeCounti);
    if (orNodeAndNodeCounti == -1) {
      orNodeAndNodeCounti = 0;
    }
    if (orNodeAndNodeCounti < 0) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Invalid OR Node Id %d", orNodei);
      goto err;
    }
    if (orNodeAndNodeCounti == 0) {
      break;
    }
    if (manageBuf_createp(genericLoggerp,
			  (void **) &(marpaWrapperAsfp->orNodeAndNodeIdipp),
			  &(marpaWrapperAsfp->sizeOrNodeAndNodeIdl),
			  orNodei + 1,
			  sizeof(int *)) == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Internal error on orNodeAndNodeIdipp, %s", strerror(errno));
      goto err;
    }
    marpaWrapperAsfp->orNodeAndNodeIdipp[orNodei] = NULL;
    marpaWrapperAsfp->nOrNodeAndNodeIdl = orNodei + 1;

    nAndNodeIdl = 0;
    for (ix = 0; ix < orNodeAndNodeCounti; ix++) {
      andNodeIdi = _marpa_o_or_node_and_node_id_by_ix(marpaWrapperAsfp->marpaOrderp, orNodei, ix);

      if (manageBuf_createp(genericLoggerp,
			    (void **) &(marpaWrapperAsfp->andNodeIdip),
			    &(marpaWrapperAsfp->sizeAndNodeIdl),
			    nAndNodeIdl + 1,
			    sizeof(int)) == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Internal error on andNodeIdip, %s", strerror(errno));
	goto err;
      }
      marpaWrapperAsfp->andNodeIdip[nAndNodeIdl] = (int) andNodeIdi;
      marpaWrapperAsfp->nAndNodeIdl = ++nAndNodeIdl;
    }
    if (nAndNodeIdl > 0) {
      /* Sort the AND Node Ids or this OR Node */
      if (manageBuf_createp(genericLoggerp,
			    (void **) &(andNodeIdAndPredecessorIdp),
			    &(sizeAndNodeIdAndPredecessorIdl),
			    nAndNodeIdl,
			    sizeof(int *)) == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Internal error on andNodeIdAndPredecessorIdp, %s", strerror(errno));
	goto err;
      }
      nAndNodeIdAndPredecessorIdl = nAndNodeIdl;

      for (ix = 0; ix < nAndNodeIdl; ix++) {
	andNodeIdAndPredecessorIdp[ix].andNodeIdi = marpaWrapperAsfp->andNodeIdip[ix];
	andNodePredecessorIdi = _marpa_b_and_node_predecessor(marpaWrapperAsfp->marpaBocagep, marpaWrapperAsfp->andNodeIdip[ix]);
	if (andNodePredecessorIdi < -1) {
	  MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	  goto err;
	}
	andNodeIdAndPredecessorIdp[ix].andNodePredecessorIdi = andNodePredecessorIdi;
      }
      qsort(andNodeIdAndPredecessorIdp, nAndNodeIdl, sizeof(marpaWrapperAfsAndNodeIdAndPredecessorId_t), _marpaWrapperAsf_andNodeIdAndPredecessorIdCmpi);
    }
    
    /* Commit the sorted array into orNodeAndNodeIdipp */
    orNodeAndNodeIdip = malloc((nAndNodeIdl + 1) * sizeof(int));
    if (orNodeAndNodeIdip == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
      goto err;
    }

    marpaWrapperAsfp->orNodeAndNodeIdipp[orNodei] = orNodeAndNodeIdip;
    if (nAndNodeIdl > 0) {
      for (ix = 0; ix < nAndNodeIdl; ix++) {
	orNodeAndNodeIdip[ix] = andNodeIdAndPredecessorIdp[ix].andNodeIdi;
      }
    }
    /* Last element */
    orNodeAndNodeIdip[ix] = -1;

    free(andNodeIdAndPredecessorIdp);
    andNodeIdAndPredecessorIdp = NULL;
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

    if (andNodeIdAndPredecessorIdp != NULL) {
      free(andNodeIdAndPredecessorIdp);
    }
    
    marpaWrapperAsf_freev(marpaWrapperAsfp);
    errno = errnoi;
  }

  manageBuf_freev(genericLoggerp, (void **) &(andNodeIdAndPredecessorIdp));

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
  const static char funcs[] = "marpaWrapperAsf_freev";
  genericLogger_t  *genericLoggerp;
  size_t            orNodeAndNodeIdi;
  size_t            i;

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

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing AND Node Ids table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperAsfp->andNodeIdip));
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing OR Node Ids table");
    /* Every OR Node is itself an array allocated on the heap */
    if (marpaWrapperAsfp->nOrNodeAndNodeIdl > 0) {
      for (orNodeAndNodeIdi = 0; orNodeAndNodeIdi < marpaWrapperAsfp->nOrNodeAndNodeIdl; orNodeAndNodeIdi++) {
	if (marpaWrapperAsfp->orNodeAndNodeIdipp[orNodeAndNodeIdi] != NULL) {
	  free(marpaWrapperAsfp->orNodeAndNodeIdipp[orNodeAndNodeIdi]);
	}
      }
    }
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing OR and AND Node Ids table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperAsfp->orNodeAndNodeIdipp));

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing intset hash");
    GENERICHASH_FREE(marpaWrapperAsfp->intsetHashp);
    
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing Nidset array");
    /* Every node also have an array inside */
    if (marpaWrapperAsfp->nNidsetl > 0) {
      for (i = 0; i < marpaWrapperAsfp->nNidsetl; i++) {
	if (marpaWrapperAsfp->nidsetp[i].idip != NULL) {
	  free(marpaWrapperAsfp->nidsetp[i].idip);
	}
      }
    }
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperAsfp->nidsetp));
    
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperAsfp);
    free(marpaWrapperAsfp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

/****************************************************************************/
static inline short _marpaWrapperAsf_peak(marpaWrapperAsf_t *marpaWrapperAsfp)
/****************************************************************************/
{
  const static char        funcs[] = "_marpaWrapperAsf_peak";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  int                      augmentOrNodeIdi;
  int                      augmentAndNodeIdi;
  int                      startOrNodeIdi;
  marpaWrapperAsfNidset_t *baseNidseti;

  augmentOrNodeIdi = (int) _marpa_b_top_or_node(marpaWrapperAsfp->marpaBocagep);
  if (augmentOrNodeIdi < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperAsfp->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  augmentAndNodeIdi = marpaWrapperAsfp->orNodeAndNodeIdipp[augmentOrNodeIdi][0];
  startOrNodeIdi = _marpa_b_and_node_cause(marpaWrapperAsfp->marpaBocagep, augmentAndNodeIdi);
  if (startOrNodeIdi < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperAsfp->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  baseNidseti = _marpaWrapperAsf_nidsetObtainb(marpaWrapperAsfp, 1, &startOrNodeIdi);
  if (baseNidseti == NULL) {
    goto err;
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return 1", 0);
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

  return (a1->andNodePredecessorIdi < a2->andNodePredecessorIdi) ? -1 : ((a1->andNodePredecessorIdi > a2->andNodePredecessorIdi) ? 1 : 0);
}

/****************************************************************************/
static inline marpaWrapperAsfNidset_t *_marpaWrapperAsf_nidsetObtainb(marpaWrapperAsf_t *marpaWrapperAsfp, size_t nIdl, int *idip)
/****************************************************************************/
{
  const static char        funcs[] = "_marpaWrapperAsf_nidsetObtainb";
  genericLogger_t         *genericLoggerp = marpaWrapperAsfp->genericLoggerp;
  int                      intsetIdi;
  int                      nidseti;
  size_t                   nNidsetl;
  marpaWrapperAsfNidset_t *nidsetp;
  size_t                   i;

  if (_marpaWrapperAsf_intsetIdb(marpaWrapperAsfp, &intsetIdi, nIdl, idip) == 0) {
    goto err;
  }
  
  nNidsetl = (size_t) (intsetIdi + 1);
  /* Per construction this is always ok, but who knows */
  if (marpaWrapperAsfp->nNidsetl >= nNidsetl) {
    nidsetp = &(marpaWrapperAsfp->nidsetp[intsetIdi]);
  } else {
    if (manageBuf_createp(genericLoggerp,
			  (void **) &(marpaWrapperAsfp->nidsetp),
			  &(marpaWrapperAsfp->sizeNidsetl),
			  nNidsetl,
			  sizeof(marpaWrapperAsfNidset_t)) == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Internal error on nidsetp, %s", strerror(errno));
      goto err;
    }
    marpaWrapperAsfp->nNidsetl = nNidsetl;
    nidsetp = &(marpaWrapperAsfp->nidsetp[intsetIdi]);
    nidsetp->id = intsetIdi;
    if (nIdl <= 0) {
      nidsetp->nIdl = 0;
      nidsetp->idip = NULL;
    } else {
      nidsetp->nIdl = nIdl;
      nidsetp->idip = malloc(sizeof(int) * nIdl);
      if (nidsetp->idip == NULL) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
	goto err;
      }
      memcpy(nidsetp->idip, idip, nIdl);
      qsort(nidsetp->idip, nIdl, sizeof(int), _marpaWrapperAsf_idCmpi);
    }
  }

  goto done;

 err:
  nidsetp = NULL;

 done:
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", nidsetp);
  return nidsetp;
}

/****************************************************************************/
static inline short _marpaWrapperAsf_intsetIdb(marpaWrapperAsf_t *marpaWrapperAsfp, int *intsetIdip, size_t nIdl, int *idip)
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

  if (nIdl <= 0) {
    keys = (char *) staticKeys;
  } else {
    for (idl = 0; idl < nIdl; idl++) {
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

