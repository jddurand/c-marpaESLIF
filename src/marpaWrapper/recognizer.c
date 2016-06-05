#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "marpa.h"
#include "config.h"
#include "marpaWrapper/internal/manageBuf.h"
#include "marpaWrapper/internal/recognizer.h"
#include "marpaWrapper/internal/grammar.h"
#include "marpaWrapper/internal/logging.h"

static marpaWrapperRecognizerOption_t marpaWrapperRecognizerOptionDefault = {
  NULL,    /* genericLoggerp   */
  0        /* latmb            */
};

static int alternativeCmpByLengthi(const void *p1, const void *p2);

/****************************************************************************/
marpaWrapperRecognizer_t *marpaWrapperRecognizer_newp(marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaWrapperRecognizerOption_t *marpaWrapperRecognizerOptionp)
/****************************************************************************/
{
  const static char         funcs[] = "marpaWrapperRecognizer_newp";
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp;
  genericLogger_t          *genericLoggerp;
  int                       highestSymbolIdi;
  size_t                    nSymboll;

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  if (marpaWrapperRecognizerOptionp == NULL) {
    marpaWrapperRecognizerOptionp = &marpaWrapperRecognizerOptionDefault;
  }
  genericLoggerp = marpaWrapperRecognizerOptionp->genericLoggerp;

  /* Create a recognizer instance */
  marpaWrapperRecognizerp = malloc(sizeof(marpaWrapperRecognizer_t));
  if (marpaWrapperRecognizerp == NULL) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

  /* See first instruction after this initialization block: marpaWrapperRecognizerp->marpaRecognizerp */
  marpaWrapperRecognizerp->marpaWrapperGrammarp         = marpaWrapperGrammarp;
  marpaWrapperRecognizerp->marpaWrapperRecognizerOption = *marpaWrapperRecognizerOptionp;
  marpaWrapperRecognizerp->sizeSymboll                  = 0;
  marpaWrapperRecognizerp->nSymboll                     = 0;
  marpaWrapperRecognizerp->symbolip                     = NULL;
  marpaWrapperRecognizerp->sizeAlternativel             = 0;
  marpaWrapperRecognizerp->nAlternativel                = 0;
  marpaWrapperRecognizerp->alternativeip                = NULL;
  marpaWrapperRecognizerp->sizeAlternativeokl           = 0;
  marpaWrapperRecognizerp->nAlternativeokl              = 0;
  marpaWrapperRecognizerp->alternativeokip              = NULL;
  marpaWrapperRecognizerp->sizeProgressl                = 0;
  marpaWrapperRecognizerp->nProgressl                   = 0;
  marpaWrapperRecognizerp->progressp                    = NULL;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_new(%p)", marpaWrapperGrammarp->marpaGrammarp);
  marpaWrapperRecognizerp->marpaRecognizerp = marpa_r_new(marpaWrapperGrammarp->marpaGrammarp);
  if (marpaWrapperRecognizerp->marpaRecognizerp == NULL) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_start_input(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
  if (marpa_r_start_input(marpaWrapperRecognizerp->marpaRecognizerp) < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Allocate room for the terminals expected output */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_highest_symbol_id(%p)", marpaWrapperGrammarp->marpaGrammarp);
  highestSymbolIdi = marpa_g_highest_symbol_id(marpaWrapperGrammarp->marpaGrammarp);
  if (highestSymbolIdi < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  nSymboll = highestSymbolIdi + 1;
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Pre-allocating room for %d symbols", nSymboll);
  if (manageBuf_createp(genericLoggerp,
			(void **) &(marpaWrapperRecognizerp->symbolip),
			&(marpaWrapperRecognizerp->sizeSymboll),
			nSymboll,
			sizeof(int)) == NULL) {
    goto err;
  }

  /* Events can happen */
  if (marpaWrapperGrammar_eventb(marpaWrapperGrammarp, NULL, NULL, 1) == 0) {
    goto err;
  }

  if (genericLoggerp != NULL) {
    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Cloning genericLogger");

    marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp = GENERICLOGGER_CLONE(genericLoggerp);
    if (marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp == NULL) {
      MARPAWRAPPER_ERRORF(genericLoggerp, "Failed to clone genericLogger: %s", strerror(errno));
      goto err;
    }
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "return %p", marpaWrapperRecognizerp);
  return marpaWrapperRecognizerp;

err:
  if (marpaWrapperRecognizerp != NULL) {
    int errnoi = errno;

    if ((genericLoggerp != NULL) &&
        (marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp != NULL) &&
        (marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp != genericLoggerp)) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned genericLogger");
      GENERICLOGGER_FREE(marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp);
    }
    marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp = NULL;
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);

    errno = errnoi;
  }

  return NULL;
}

/****************************************************************************/
short marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, int valuei, int lengthi)
/****************************************************************************/
{
  const static char                   funcs[] = "marpaWrapperRecognizer_alternativeb";
  genericLogger_t                    *genericLoggerp = NULL;
  size_t                              nAlternativel;
  marpaWrapperRecognizerAlternative_t alternative;
  
  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

  if (symboli < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "Invalid symbol number %d", symboli);
    errno = EINVAL;
    goto err;
  }

  if (marpaWrapperRecognizerp->marpaWrapperRecognizerOption.latmb != 0) {
    /* Cache this alternative */
    nAlternativel = marpaWrapperRecognizerp->nAlternativel;
    if (manageBuf_createp(genericLoggerp,
			  (void **) &(marpaWrapperRecognizerp->alternativeip),
			  &(marpaWrapperRecognizerp->sizeAlternativel),
			  nAlternativel + 1,
			  sizeof(marpaWrapperRecognizerAlternative_t)) == NULL) {
      goto err;
    }
    alternative.symboli = symboli;
    alternative.valuei  = valuei;
    alternative.lengthi = lengthi;
    marpaWrapperRecognizerp->alternativeip[nAlternativel] = alternative;
    marpaWrapperRecognizerp->nAlternativel = ++nAlternativel;
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Remembering alternative No %d", (int) nAlternativel);
  } else {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_alternative(%p, %d, %d, %d)", marpaWrapperRecognizerp->marpaRecognizerp, symboli, valuei, lengthi);
    if (marpa_r_alternative(marpaWrapperRecognizerp->marpaRecognizerp, (Marpa_Symbol_ID) symboli, valuei, lengthi) != MARPA_ERR_NONE) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperRecognizer_completeb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperRecognizer_completeb";
  genericLogger_t  *genericLoggerp = NULL;
  size_t            nAlternativel, nSymboll, nAlternativeokl, nAlternativeok2l;
  size_t            i, j;
  short             latmb;
  size_t            lengthi;
  
  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

  if ((latmb = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.latmb) != 0) {
    nAlternativel = marpaWrapperRecognizerp->nAlternativel;
    /* If there are alternatives */
    if (nAlternativel > 0) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Using %d cached alternatives", (int) nAlternativel);
      /* We want to make sure we get the correct list of expected symbols */
      if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, NULL, NULL) == 0) {
	goto err;
      }
      nSymboll = marpaWrapperRecognizerp->nSymboll;
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Number of expected symbols is %d", (int) nSymboll);
      if (nSymboll > 0) {
	/* Get only accepted alternatives */
	nAlternativeokl = 0;
	for (i = 0; i < nSymboll; i++) {
	  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "... Looking if symbol No %d is an alternative", marpaWrapperRecognizerp->symbolip[i]);
	  for (j = 0; j < nAlternativel; j++) {
	    if (marpaWrapperRecognizerp->alternativeip[j].symboli == marpaWrapperRecognizerp->symbolip[i]) {
	      if (manageBuf_createp(genericLoggerp,
				    (void **) &(marpaWrapperRecognizerp->alternativeokip),
				    &(marpaWrapperRecognizerp->sizeAlternativeokl),
				    nAlternativeokl + 1,
				    sizeof(marpaWrapperRecognizerAlternative_t)) == NULL) {
		goto err;
	      }
	      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Alternative No %d is symbol No %d, length %d", (int) j, marpaWrapperRecognizerp->alternativeip[j].symboli, marpaWrapperRecognizerp->alternativeip[j].lengthi);
	      marpaWrapperRecognizerp->alternativeokip[nAlternativeokl] = marpaWrapperRecognizerp->alternativeip[j];
	      marpaWrapperRecognizerp->nAlternativeokl = ++nAlternativeokl;
	    }
	  }
	}
	if (nAlternativeokl > 0) {
	  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "%d alternatives accepted", (int) nAlternativeokl);
	  /* Sort them by length */
	  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Sorting %d alternatives by length", (int) nAlternativeokl);
	  qsort(marpaWrapperRecognizerp->alternativeokip, nAlternativeokl, sizeof(marpaWrapperRecognizerAlternative_t), alternativeCmpByLengthi);
	  lengthi = marpaWrapperRecognizerp->alternativeokip[0].lengthi;
	  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Filtering %d alternatives using max length %d", (int) nAlternativeokl, lengthi);
	  /* And push only the longest alternatives, we temporarly set off latmb to false */
	  marpaWrapperRecognizerp->marpaWrapperRecognizerOption.latmb = 0;
	  for (i = 0, nAlternativeok2l = 0; i < nAlternativeokl; i++) {
	    if (marpaWrapperRecognizerp->alternativeokip[i].lengthi < lengthi) {
	      break;
	    }
	    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Pushing symbol %d, value %d, length %d",
				marpaWrapperRecognizerp->alternativeokip[i].symboli,
				marpaWrapperRecognizerp->alternativeokip[i].valuei,
				marpaWrapperRecognizerp->alternativeokip[i].lengthi);
	    if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp,
						    marpaWrapperRecognizerp->alternativeokip[i].symboli,
						    marpaWrapperRecognizerp->alternativeokip[i].valuei,
						    marpaWrapperRecognizerp->alternativeokip[i].lengthi) == 0) {
	      marpaWrapperRecognizerp->marpaWrapperRecognizerOption.latmb = latmb;
	      goto err;
	    }
	    nAlternativeok2l++;
	  }
	  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Pushed %d alternatives", (int) nAlternativeok2l);
	} else {
	  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "No symbol accepted");
	}
	marpaWrapperRecognizerp->marpaWrapperRecognizerOption.latmb = latmb;
      }
    } else {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "No cached alternative");
    }
    /* Get ready for next round */
    marpaWrapperRecognizerp->nAlternativel = 0;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_earleme_complete(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
  if (marpa_r_earleme_complete(marpaWrapperRecognizerp->marpaRecognizerp) < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  /* Events can happen */
  if (marpaWrapperGrammar_eventb(marpaWrapperRecognizerp->marpaWrapperGrammarp, NULL, NULL, 1) == 0) {
    goto err;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperRecognizer_readb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, int valuei, int lengthi)
/****************************************************************************/
{
  if (marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symboli, valuei, lengthi) == 0) {
    return 0;
  }
  
  return marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp);
}

/****************************************************************************/
short marpaWrapperRecognizer_event_onoffb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, marpaWrapperGrammarEventType_t eventSeti, int onoffb)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperRecognizer_event_onoffb";
  genericLogger_t  *genericLoggerp = NULL;

  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

  if (onoffb != 0) {
    onoffb = 1;
  }

  if ((eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_completion_symbol_activate(%p, %d, %d)", marpaWrapperRecognizerp->marpaRecognizerp, symboli, onoffb);
    if (marpa_r_completion_symbol_activate(marpaWrapperRecognizerp->marpaRecognizerp, symboli, onoffb) != onoffb) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }
  if ((eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) == MARPAWRAPPERGRAMMAR_EVENTTYPE_NULLED) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_nulled_symbol_activate(%p, %d, %d)", marpaWrapperRecognizerp->marpaRecognizerp, symboli, onoffb);
    if (marpa_r_nulled_symbol_activate(marpaWrapperRecognizerp->marpaRecognizerp, symboli, onoffb) != onoffb) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }
  if ((eventSeti & MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) == MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION) {
    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_prediction_symbol_activate(%p, %d, %d)", marpaWrapperRecognizerp->marpaRecognizerp, symboli, onoffb);
    if (marpa_r_prediction_symbol_activate(marpaWrapperRecognizerp->marpaRecognizerp, symboli, onoffb) != onoffb) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperRecognizer_expectedb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, size_t *nSymbollp, int **symbolArraypp)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperRecognizer_expectedb";
  genericLogger_t  *genericLoggerp = NULL;
  int               nSymbolIdi;
  int               i;

  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_terminals_expected(%p, %p)", marpaWrapperRecognizerp->marpaRecognizerp, marpaWrapperRecognizerp->symbolip);
  nSymbolIdi = marpa_r_terminals_expected(marpaWrapperRecognizerp->marpaRecognizerp, marpaWrapperRecognizerp->symbolip);
  if (nSymbolIdi < 0) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
  }

  marpaWrapperRecognizerp->nSymboll = (size_t) nSymbolIdi;

  if (nSymbollp != NULL) {
    *nSymbollp = (size_t) nSymbolIdi;
  }
  if (symbolArraypp != NULL) {
    *symbolArraypp = marpaWrapperRecognizerp->symbolip;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperRecognizer_progressb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int starti, int lengthi, size_t *nProgresslp, marpaWrapperRecognizerProgress_t **progresspp)
/****************************************************************************/
{
  const static char   funcs[] = "marpaWrapperRecognizer_progressb";
  genericLogger_t    *genericLoggerp = NULL;
  Marpa_Earley_Set_ID marpaLatestEarleySetIdi, marpaEarleySetIdStarti, earleySetIdi, marpaEarleySetIdEndi, earleySetOrigIdi;
  Marpa_Earleme       marpEarlemei;
  Marpa_Rule_ID       rulei;
  int                 realStarti = starti;
  int                 realEndi;
  int                 positioni;
  size_t              nProgressl;
  int                 nbItemsi;
  int                itemi;

  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

  if (lengthi < 0) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "lengthi is %d but must be > 0", lengthi);
    errno = EINVAL;
    goto err;
  }
  
  /* This function always succeed as per doc */
  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_latest_earley_set(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
  marpaLatestEarleySetIdi = marpa_r_latest_earley_set(marpaWrapperRecognizerp->marpaRecognizerp);
  if (realStarti < 0) {
    realStarti += (marpaLatestEarleySetIdi + 1);
  }
  if ((realStarti < 0) || (realStarti > marpaLatestEarleySetIdi)) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "starti must be in range [%d-%d]", (int) (-(marpaLatestEarleySetIdi+1)), (int) marpaLatestEarleySetIdi);
    errno = EINVAL;
    goto err;
  }

  realEndi = realStarti + lengthi - 1;
  if (realStarti > realEndi) {
    MARPAWRAPPER_ERRORF(genericLoggerp, "[starti,lengthi] range [%d,%d] evaluated to [%d-%d]", starti, lengthi, realStarti, realEndi);
    errno = EINVAL;
    goto err;
  }

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "Asking for progress report in early set range [%d-%d]", realStarti, realEndi);
  marpaEarleySetIdStarti = (Marpa_Earley_Set_ID) realStarti;
  marpaEarleySetIdEndi   = (Marpa_Earley_Set_ID) realEndi;
  nProgressl = 0;
  for (earleySetIdi = marpaEarleySetIdStarti; earleySetIdi <= marpaEarleySetIdEndi; earleySetIdi++) {

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_earleme(%p, %d)", marpaWrapperRecognizerp->marpaRecognizerp, (int) earleySetIdi);
    marpEarlemei = marpa_r_earleme(marpaWrapperRecognizerp->marpaRecognizerp, earleySetIdi);
    if (marpEarlemei < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_progress_report_start(%p, %d)", marpaWrapperRecognizerp->marpaRecognizerp, (int) earleySetIdi);
    nbItemsi = marpa_r_progress_report_start(marpaWrapperRecognizerp->marpaRecognizerp, earleySetIdi);
    if (nbItemsi < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }

    for (itemi = 0; itemi < nbItemsi; itemi++) {

      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_progress_item(%p, %p, %p)", marpaWrapperRecognizerp->marpaRecognizerp, &positioni, &earleySetOrigIdi);
      rulei = marpa_r_progress_item(marpaWrapperRecognizerp->marpaRecognizerp, &positioni, &earleySetOrigIdi);
      if (rulei < 0) {
	MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	goto err;
      }

      if (manageBuf_createp(genericLoggerp,
			    (void **) &(marpaWrapperRecognizerp->progressp),
			    &(marpaWrapperRecognizerp->sizeProgressl),
			    nProgressl + 1,
			    sizeof(marpaWrapperRecognizerProgress_t)) == NULL) {
	goto err;
      }

      marpaWrapperRecognizerp->progressp[nProgressl].earleySetIdi     = (int) earleySetIdi;
      marpaWrapperRecognizerp->progressp[nProgressl].earleySetOrigIdi = (int) earleySetOrigIdi;
      marpaWrapperRecognizerp->progressp[nProgressl].rulei            = rulei;
      marpaWrapperRecognizerp->progressp[nProgressl].positioni        = positioni;

      marpaWrapperRecognizerp->nProgressl = ++nProgressl;
    }

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_progress_report_finish(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
    if (marpa_r_progress_report_finish(marpaWrapperRecognizerp->marpaRecognizerp) < 0) {
      MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
      goto err;
    }

  }

  if (nProgresslp != NULL) {
    *nProgresslp = marpaWrapperRecognizerp->nProgressl;
  }
  if (progresspp != NULL) {
    *progresspp = marpaWrapperRecognizerp->progressp;
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
short marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int starti, int endi, genericLoggerLevel_t logleveli, void *userDatavp, symbolDescriptionCallback_t symbolDescriptionCallbackp)
/****************************************************************************/
{
  const static char funcs[] = "marpaWrapperRecognizer_progressLogb";
  size_t            i, ix;
  genericLogger_t  *genericLoggerp = NULL;
  int               ruleLengthi;
  Marpa_Symbol_ID   lhsi, rhsi;
  Marpa_Earleme     earlemei, earlemeorigi;     
  char             *descriptionLHSs, *descriptionRHSs;
  size_t            lengthDescriptionLHSi;
  int               positioni, rulei;
  char             *lefts;
  char             *middles;
  char              rtypec;
  
  if (marpaWrapperRecognizerp == NULL) {
    errno = EINVAL;
    goto err;
  }

  genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

  if (marpaWrapperRecognizer_progressb(marpaWrapperRecognizerp, starti, endi, NULL, NULL) == 0) {
    goto err;
  }

  if (genericLoggerp != NULL) {
    if (marpaWrapperRecognizerp->nProgressl > 0) {
      for (i = 0; i < marpaWrapperRecognizerp->nProgressl; i++) {
	if (symbolDescriptionCallbackp == NULL) {
	  genericLogger_logv(genericLoggerp, logleveli, "# earleySetIdi %4d earleySetOrigIdi %4d rulei %4d positioni %4d",
			     marpaWrapperRecognizerp->progressp[i].earleySetIdi,
			     marpaWrapperRecognizerp->progressp[i].earleySetOrigIdi,
			     marpaWrapperRecognizerp->progressp[i].rulei,
			     marpaWrapperRecognizerp->progressp[i].positioni
			     );
	} else {
	  /* MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_earleme(%p, %d)", marpaWrapperRecognizerp->marpaRecognizerp, marpaWrapperRecognizerp->progressp[i].earleySetIdi); */
	  earlemei = marpa_r_earleme(marpaWrapperRecognizerp->marpaRecognizerp, (Marpa_Earley_Set_ID) marpaWrapperRecognizerp->progressp[i].earleySetIdi);
	  if (earlemei < 0) {
	    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	    goto err;
	  }
	  earlemeorigi = marpa_r_earleme(marpaWrapperRecognizerp->marpaRecognizerp, (Marpa_Earley_Set_ID) marpaWrapperRecognizerp->progressp[i].earleySetOrigIdi);
	  if (earlemeorigi < 0) {
	    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	    goto err;
	  }

	  /* MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_lhs(%p, %d)", marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp, marpaWrapperRecognizerp->progressp[i].rulei); */
	  lhsi = marpa_g_rule_lhs(marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp, (Marpa_Rule_ID) marpaWrapperRecognizerp->progressp[i].rulei);
	  if (lhsi < 0) {
	    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	    goto err;
	  }
	  descriptionLHSs = symbolDescriptionCallbackp(userDatavp, lhsi);
	  if ((descriptionLHSs == NULL) || (strlen(descriptionLHSs) <= 0)) {
	    descriptionLHSs = "?";
	  }

	  /* MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_length(%p, %d)", marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp, marpaWrapperRecognizerp->progressp[i].rulei); */
	  ruleLengthi = marpa_g_rule_length(marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp, (Marpa_Rule_ID) marpaWrapperRecognizerp->progressp[i].rulei);
	  if (ruleLengthi < 0) {
	    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
	    goto err;
	  }

	  /* Description is split on multiple lines */
	  lengthDescriptionLHSi = strlen(descriptionLHSs);

	  positioni = marpaWrapperRecognizerp->progressp[i].positioni;
	  rulei     = marpaWrapperRecognizerp->progressp[i].rulei;

	  for (ix = 0; ix < ruleLengthi; ix++) {
	    lefts = (ix == 0) ? descriptionLHSs : " ";
	    middles = (ix == 0) ? "::=" : "   ";
	    rtypec = ((positioni < 0) || (positioni >= ruleLengthi)) ? 'F' : ((positioni > 0) ? 'R' : 'P');

	    if (ruleLengthi > 0) {
	      /* MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_g_rule_rhs(%p, %d, %d)", marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp, marpaWrapperRecognizerp->progressp[i].rulei, ix); */
	      rhsi = marpa_g_rule_rhs(marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp, (Marpa_Rule_ID) marpaWrapperRecognizerp->progressp[i].rulei, ix);
	      if (rhsi < 0) {
		MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
		goto err;
	      }
	      descriptionRHSs = symbolDescriptionCallbackp(userDatavp, rhsi);
	      if ((descriptionRHSs == NULL) || (strlen(descriptionRHSs) <= 0)) {
		descriptionRHSs = "?";
	      }
	    } else {
	      descriptionRHSs = "";
	    }
	    if (positioni == ix) {
	      genericLogger_logv(genericLoggerp, logleveli, "[%c%d@%d..%d] %*s %s . %s", rtypec, rulei, earlemeorigi, earlemei, (int) lengthDescriptionLHSi, lefts, middles, descriptionRHSs);
	    } else if (positioni < 0) {
	      if (ix == (ruleLengthi - 1)) {
		genericLogger_logv(genericLoggerp, logleveli, "[%c%d@%d..%d] %*s %s %s .", rtypec, rulei, earlemeorigi, earlemei, (int) lengthDescriptionLHSi, lefts, middles, descriptionRHSs);
	      } else {
		genericLogger_logv(genericLoggerp, logleveli, "[%c%d@%d..%d] %*s %s %s", rtypec, rulei, earlemeorigi, earlemei, (int) lengthDescriptionLHSi, lefts, middles, descriptionRHSs);
	      }
	    } else {
	      genericLogger_logv(genericLoggerp, logleveli, "[%c%d@%d..%d] %*s %s %s", rtypec, rulei, earlemeorigi, earlemei, (int) lengthDescriptionLHSi, lefts, middles, descriptionRHSs);
	    }
	  }
	}
      }
    }
  }

  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 1");
  return 1;

 err:
  MARPAWRAPPER_TRACE(genericLoggerp, funcs, "return 0");
  return 0;
}

/****************************************************************************/
void marpaWrapperRecognizer_freev(marpaWrapperRecognizer_t *marpaWrapperRecognizerp)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperRecognizer_freev";
  genericLogger_t  *genericLoggerp;

  if (marpaWrapperRecognizerp != NULL) {
    /* Keep a copy of the generic logger. If original is not NULL, then we have a clone of it */
    genericLoggerp = marpaWrapperRecognizerp->marpaWrapperRecognizerOption.genericLoggerp;

    if (marpaWrapperRecognizerp->marpaRecognizerp != NULL) {
      MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_unref(%p)", marpaWrapperRecognizerp->marpaRecognizerp);
      marpa_r_unref(marpaWrapperRecognizerp->marpaRecognizerp);
    }

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing symbol table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperRecognizerp->symbolip));

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing alternative symbol table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperRecognizerp->alternativeip));

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing ok alternative symbol table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperRecognizerp->alternativeokip));

    MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing progress table");
    manageBuf_freev(genericLoggerp, (void **) &(marpaWrapperRecognizerp->progressp));

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperRecognizerp);
    free(marpaWrapperRecognizerp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

/****************************************************************************/
static int alternativeCmpByLengthi(const void *p1, const void *p2)
/****************************************************************************/
{
  marpaWrapperRecognizerAlternative_t *a1p = (marpaWrapperRecognizerAlternative_t *) p1;
  marpaWrapperRecognizerAlternative_t *a2p = (marpaWrapperRecognizerAlternative_t *) p2;

  if (a1p->lengthi < a2p->lengthi) {
    return 1;
  } else if (a1p->lengthi > a2p->lengthi) {
    return -1;
  } else {
    return 0;
  }
}
 
