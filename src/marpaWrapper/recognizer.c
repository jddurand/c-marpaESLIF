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
  1        /* latmb            */
};

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
  if (marpaWrapperGrammar_eventb(marpaWrapperGrammarp) == 0) {
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
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);

    errno = errnoi;
  }

  return NULL;
}

/****************************************************************************/
short marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizer_t *marpaWrapperRecognizerp, int symboli, int valuei, int lengthi)
/****************************************************************************/
{
  const static char  funcs[] = "marpaWrapperRecognizer_alternativeb";
  genericLogger_t *genericLoggerp = NULL;

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

  MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "marpa_r_alternative(%p, %d, %d, %d)", marpaWrapperRecognizerp->marpaRecognizerp, symboli, valuei, lengthi);
  if (marpa_r_alternative(marpaWrapperRecognizerp->marpaRecognizerp, (Marpa_Symbol_ID) symboli, valuei, lengthi) != MARPA_ERR_NONE) {
    MARPAWRAPPER_MARPA_G_ERROR(genericLoggerp, marpaWrapperRecognizerp->marpaWrapperGrammarp->marpaGrammarp);
    goto err;
    
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

    MARPAWRAPPER_TRACEF(genericLoggerp, funcs, "free(%p)", marpaWrapperRecognizerp);
    free(marpaWrapperRecognizerp);

    if (genericLoggerp != NULL) {
      MARPAWRAPPER_TRACE(genericLoggerp, funcs, "Freeing cloned generic logger");
      GENERICLOGGER_FREE(genericLoggerp);
    }
  }
}

