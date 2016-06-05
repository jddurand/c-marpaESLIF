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
  0                               /* nullb */
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
    marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp = NULL;
    marpaWrapperValue_freev(marpaWrapperValuep);

    errno = errnoi;
  }

  return NULL;
}

/****************************************************************************/
short marpaWrapperValue_valueb(marpaWrapperValue_t               *marpaWrapperValuep,
			       void                              *userDatavp,
			       marpaWrapperValueRuleCallback_t    ruleCallbackp,
			       marpaWrapperValueSymbolCallback_t  symbolCallbackp,
			       marpaWrapperValueNullingCallback_t nullingCallbackp)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperValue_valueb";
  genericLogger_t  *genericLoggerp = NULL;
  int               tnexti;
  Marpa_Value       marpaValuep = NULL;
  int               nexti;
  Marpa_Step_Type   stepi;
  Marpa_Rule_ID     marpaRuleIdi;
  Marpa_Symbol_ID   marpaSymbolIdi;
  int               argFirsti;
  int               argLasti;
  int               argResulti;
  int               tokenValuei;
  size_t            nValueInputi;

  if (marpaWrapperValuep == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperValuep->marpaWrapperValueOption.genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_t_next(%p)", marpaWrapperValuep->marpaTreep);
  tnexti = marpa_t_next(marpaWrapperValuep->marpaTreep);
  if (tnexti < -1) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperValuep->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  } else if (tnexti == -1) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Tree iterator exhausted");
    goto done;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_new(%p)", marpaWrapperValuep->marpaTreep);
  marpaValuep = marpa_v_new(marpaWrapperValuep->marpaTreep);

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_valued_force(%p)", marpaValuep);
  if (marpa_v_valued_force(marpaValuep) < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperValuep->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  nexti = 1;
  while (nexti != 0) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_step(%p)", marpaValuep);
    stepi = marpa_v_step(marpaValuep);
    if (stepi < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperValuep->marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }

    MARPAWRAPPER_MARPA_STEP_TRACE(genericLoggerp, funcs, stepi);

    switch (stepi) {
    case MARPA_STEP_RULE:

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_rule(%p)", marpaValuep);
      marpaRuleIdi = marpa_v_rule(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_arg_0(%p)", marpaValuep);
      argFirsti = marpa_v_arg_0(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_arg_n(%p)", marpaValuep);
      argLasti = marpa_v_arg_n(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_result(%p)", marpaValuep);
      argResulti = marpa_v_result(marpaValuep);

      nValueInputi = argLasti - argFirsti + 1;
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Rule %d: Stack [%d..%d] -> Stack %d", (int) marpaRuleIdi, argFirsti, argLasti, argResulti);

      if (ruleCallbackp == NULL) {
	MARPAWRAPPER_ERROR(genericLoggerp, "A rule value callback is required");
	goto err;
      }

      if (ruleCallbackp(userDatavp, (int) marpaRuleIdi, argFirsti, argLasti, argResulti) == 0) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Rule No %d value callback failure", (int) marpaRuleIdi);
	goto err;
      }

      break;
    case MARPA_STEP_TOKEN:

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_token(%p)", marpaValuep);
      marpaSymbolIdi = marpa_v_token(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_token_value(%p)", marpaValuep);
      tokenValuei = marpa_v_token_value(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_result(%p)", marpaValuep);
      argResulti = marpa_v_result(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Symbol %d: Stack %d -> Stack %d", (int) marpaSymbolIdi, tokenValuei, argResulti);

      if (symbolCallbackp == NULL) {
	MARPAWRAPPER_ERROR(genericLoggerp, "A symbol value callback is required");
	goto err;
      }

      if (symbolCallbackp(userDatavp, (int) marpaSymbolIdi, tokenValuei, argResulti) == 0) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Symbol No %d value callback failure", (int) marpaSymbolIdi);
	goto err;
      }

      break;
    case MARPA_STEP_NULLING_SYMBOL:

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_symbol(%p)", marpaValuep);
      marpaSymbolIdi = marpa_v_symbol(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_result(%p)", marpaValuep);
      argResulti = marpa_v_result(marpaValuep);

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Nulling symbol %d-> Stack %d", (int) marpaSymbolIdi, argResulti);

      if (nullingCallbackp == NULL) {
	MARPAWRAPPER_ERROR(genericLoggerp, "A nulling symbol value callback is required");
	goto err;
      }

      if (nullingCallbackp(userDatavp, (int) marpaSymbolIdi, argResulti) == 0) {
	MARPAWRAPPER_ERRORF(genericLoggerp, "Nulling symbol No %d value callback failure", (int) marpaSymbolIdi);
	goto err;
      }

      break;
    case MARPA_STEP_INACTIVE:
      nexti = 0;
      break;
    case MARPA_STEP_INITIAL:
      break;
    default:
      break;
    }
  }
  
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_unref(%p)", marpaValuep);
  marpa_v_unref(marpaValuep);

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;
  
 done:
  if (marpaValuep != NULL) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_unref(%p)", marpaValuep);
    marpa_v_unref(marpaValuep);
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
  
 err:
  if (marpaValuep != NULL) {
    int errnoi = errno;
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_v_unref(%p)", marpaValuep);
    marpa_v_unref(marpaValuep);
    errno = errnoi;
  }
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return -1");
  return -1;
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

