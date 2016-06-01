#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "marpa.h"

#include "config.h"
#include "marpaWrapper/recognizer.h"
#include "marpaWrapper/internal/logging.h"

struct marpaWrapperRecognizer {
  marpaWrapperGrammar_t *marpaWrapperGrammarp;
};

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

  if (marpaWrapperGrammarp == NULL) {
    errno = EINVAL;
    goto err;
  }

  if (marpaWrapperRecognizerOptionp == NULL) {
    marpaWrapperRecognizerOptionp = &marpaWrapperRecognizerOptionDefault;
  }
  genericLoggerp = marpaWrapperRecognizerOptionp->genericLoggerp;

  marpaWrapperRecognizerp = malloc(sizeof(marpaWrapperRecognizer_t));
  if (marpaWrapperRecognizerp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure: %s", strerror(errno));
    goto err;
  }

err:
  return NULL;
}
