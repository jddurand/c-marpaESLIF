#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/value.h"
#include "marpaWrapper/internal/recognizer.h"
#include "marpaWrapper/internal/grammar.h"
#include "marpaWrapper/internal/logging.h"

static marpaWrapperValueOption_t marpaWrapperValueOptionDefault = {
  NULL,                           /* genericLoggerp */
  1,                              /* highRankOnlyb */
  1,                              /* orderByRankb */
  0,                              /* ambiguousb */
  0,                              /* nullb */
  NULL,                           /* valueRuleCallbackDefault */
  NULL,                           /* valueSymbolCallbackDefault */
  NULL                            /* valueNullingCallback */
};

/****************************************************************************/
marpaWrapperValue_t *marpaWrapperValue_newp(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, marpaWrapperValueOption_t *marpaWrapperValueOptionp)
/****************************************************************************/
{
  const static char     funcs[] = "marpaWrapperValue_newp";
  marpaWrapperValue_t  *marpaWrapperValuep;
  genericLogger_t      *genericLoggerp;
  Marpa_Earley_Set_ID   marpaLatestEarleySetIdi;
  int                   highRankOnlyFlagi;
  int                   ambiguousi;
  int                   nulli;

  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  if (marpaWrapperValueOptionp == NULL) {
    marpaWrapperValueOptionp = &marpaWrapperValueOptionDefault;
  }
  genericLoggerp = marpaWrapperValueOptionp->genericLoggerp;

  /* Create a value instance */
  marpaWrapperValuep = malloc(sizeof(marpaWrapperValue_t));
  if (marpaWrapperValuep == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  /* See first instruction after this initialization block: marpaWrapperValuep->marpaValuep */
  marpaWrapperValuep->marpaWrapperRecognizerp = marpaWrapperRecognizerp;
  marpaWrapperValuep->marpaWrapperValueOption = *marpaWrapperValueOptionp;
  marpaWrapperValuep->marpaBocagep            = NULL;
  marpaWrapperValuep->marpaOrderp             = NULL;
  marpaWrapperValuep->marpaTreep              = NULL;

  /* Always succeed as per the doc */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_latest_earley_set(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
  marpaLatestEarleySetIdi = marpa_r_latest_earley_set(marpaWrapperRecognizerp->marpaRecognizerp);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_b_new(%p, %d)", marpaWrapperRecognizerp->marpaRecognizerp, (int) marpaLatestEarleySetIdi);
  marpaWrapperValuep->marpaBocagep = marpa_b_new(marpaWrapperRecognizerp->marpaRecognizerp, marpaLatestEarleySetIdi);
  if (marpaWrapperValuep->marpaBocagep == NULL) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_new(%p)", marpaWrapperValuep->marpaBocagep);
  marpaWrapperValuep->marpaOrderp = marpa_o_new(marpaWrapperValuep->marpaBocagep);
  if (marpaWrapperValuep->marpaOrderp == NULL) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  highRankOnlyFlagi = (marpaWrapperValueOptionp->highRankOnlyb != 0) ? 1 : 0;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_high_rank_only_set(%p, %d)", marpaWrapperValuep->marpaOrderp, highRankOnlyFlagi);
  if (marpa_o_high_rank_only_set(marpaWrapperValuep->marpaOrderp, highRankOnlyFlagi) != highRankOnlyFlagi) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (marpaWrapperValueOptionp->orderByRankb != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_rank(%p)", marpaWrapperValuep->marpaOrderp);
    if (marpa_o_rank(marpaWrapperValuep->marpaOrderp) < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  if (marpaWrapperValueOptionp->ambiguousb == 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_ambiguity_metric(%p)", marpaWrapperValuep->marpaOrderp);
    ambiguousi = marpa_o_ambiguity_metric(marpaWrapperValuep->marpaOrderp);
    if (ambiguousi < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    } else if (ambiguousi > 1) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Parse is ambiguous");
      goto err;
    }
  }
  
  if (marpaWrapperValueOptionp->nullb == 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_is_null(%p)", marpaWrapperValuep->marpaOrderp);
    nulli = marpa_o_is_null(marpaWrapperValuep->marpaOrderp);
    if (nulli < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    } else if (nulli >= 1) {
      MARPAWRAPPER_ERROR(genericLoggerp, "Parse is null");
      goto err;
    }
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_t_new(%p)", marpaWrapperValuep->marpaOrderp);
  marpaWrapperValuep->marpaTreep = marpa_t_new(marpaWrapperValuep->marpaOrderp);
  if (marpaWrapperValuep->marpaTreep == NULL) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_t_next(%p)", marpaWrapperValuep->marpaTreep);
  if (marpa_t_next(marpaWrapperValuep->marpaTreep) < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  if (genericLoggerp != NULL) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Cloning genericLogger");

    marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp = GENERICLOGGER_CLONE(genericLoggerp);
    if (marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Failed to clone genericLogger: %s", strerror(errno));
      goto err;
    }
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperValuep);
  return marpaWrapperValuep;

err:
  if (marpaWrapperValuep != NULL) {
    int errnoi = errno;

    if ((genericLoggerp != NULL) &&
        (marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp != NULL) &&
        (marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp != genericLoggerp)) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned genericLogger");
      GENERICLOGGER_FREE(marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp);
    }
    marpaWrapperValue_freev(marpaWrapperValuep);

    errno = errnoi;
  }

  return NULL;
}

/****************************************************************************/
void marpaWrapperValue_freev(marpaWrapperValue_t *marpaWrapperValuep)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperValue_freev";
  genericLogger_t  *genericLoggerp;

  if (marpaWrapperValuep != NULL) {
    /* Keep a copy of the generic logger. If original is not NULL, then we have a clone of it */
    genericLoggerp = marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp;

    if (marpaWrapperValuep->marpaTreep != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_t_unref(%p)", marpaWrapperValuep->marpaTreep);
      marpa_t_unref(marpaWrapperValuep->marpaTreep);
    }

    if (marpaWrapperValuep->marpaOrderp != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_o_unref(%p)", marpaWrapperValuep->marpaOrderp);
      marpa_o_unref(marpaWrapperValuep->marpaOrderp);
    }

    if (marpaWrapperValuep->marpaBocagep != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_b_unref(%p)", marpaWrapperValuep->marpaBocagep);
      marpa_b_unref(marpaWrapperValuep->marpaBocagep);
    }

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperValuep);
    free(marpaWrapperValuep);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

