#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>
#include "yamlGrammar.c"

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
static short doparseb(genericLogger_t *genericLoggerp, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIFGrammar_t *marpaESLIFGrammarObjectp, char *inputs, int recursionleveli);

typedef struct yamlTester_context {
  char            *inputs;
  size_t           inputl;
} yamlTester_context_t;

static char *inputsp[] = {
  "---\n"
  " doe: \"a deer, a female deer\"\n"
  " ray: \"a drop of golden sun\"\n"
  " pi: 3.14159\n"
  " xmas: true\n"
  " french-hens: 3\n"
  " calling-birds:\n"
  "   - huey\n"
  "   - dewey\n"
  "   - louie\n"
  "   - fred\n"
  " xmas-fifth-day:\n"
  "   calling-birds: four\n"
  "   french-hens: 3\n"
  "   golden-rings: 5\n"
  "   partridges:\n"
  "     count: 1\n"
  "     location: \"a pear tree\"\n"
  "   turtle-doves: two\n"
    };

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFOption_t          *marpaESLIFOptionp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  char                        *grammarscripts;
  yamlTester_context_t         yamlTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  int                          i;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }
  marpaESLIFOptionp = marpaESLIF_optionp(marpaESLIFp);
  if (marpaESLIFOptionp == NULL) {
    goto err;
  }

  /* Compile YAML grammar */
  marpaESLIFGrammarOption.bytep               = (void *) yamls;
  marpaESLIFGrammarOption.bytel               = strlen(yamls);
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = strlen("UTF-8");
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (! marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    goto err;
  }
  for (leveli = 0; leveli < ngrammari; leveli++) {
    if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "YAML grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n%s", grammarshows);
    }
  }

  /* Grammar script */
  if (! marpaESLIFGrammar_grammarshowscriptb(marpaESLIFGrammarp, &grammarscripts)) {
    GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "marpaESLIFGrammar_grammarshowscriptb failure, %s", strerror(errno));
    goto err;
  }
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "--------------------");
  GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "YAML grammar script:");
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "--------------------\n%s", grammarscripts);

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);

  for (i = 0; i < (sizeof(inputsp)/sizeof(inputsp[0])); i++) {
    yamlTester_context.inputs = inputsp[i];
    yamlTester_context.inputl = strlen(inputsp[i]);

    marpaESLIFRecognizerOption.userDatavp               = &yamlTester_context;
    marpaESLIFRecognizerOption.readerCallbackp          = inputReaderb;
    marpaESLIFRecognizerOption.disableThresholdb        = 0;
    marpaESLIFRecognizerOption.exhaustedb               = 0;
    marpaESLIFRecognizerOption.newlineb                 = 1;
    marpaESLIFRecognizerOption.trackb                   = 0;
    marpaESLIFRecognizerOption.bufsizl                  = 0;
    marpaESLIFRecognizerOption.buftriggerperci          = 50;
    marpaESLIFRecognizerOption.bufaddperci              = 50;
    marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
    marpaESLIFRecognizerOption.eventActionResolverp     = NULL;
    marpaESLIFRecognizerOption.regexActionResolverp     = NULL;
    marpaESLIFRecognizerOption.generatorActionResolverp = NULL;

    marpaESLIFValueOption.userDatavp            = NULL; /* User specific context */
    marpaESLIFValueOption.ruleActionResolverp   = NULL; /* Will return the function doing the wanted rule action */
    marpaESLIFValueOption.symbolActionResolverp = NULL; /* Will return the function doing the wanted symbol action */
    marpaESLIFValueOption.importerp             = NULL;
    marpaESLIFValueOption.highRankOnlyb         = 1;    /* Default: 1 */
    marpaESLIFValueOption.orderByRankb          = 1;    /* Default: 1 */
    marpaESLIFValueOption.ambiguousb            = 0;    /* Default: 0 */
    marpaESLIFValueOption.nullb                 = 0;    /* Default: 0 */
    marpaESLIFValueOption.maxParsesi            = 0;    /* Default: 0 */

    marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL /* exhaustedbp */);
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Exiting with status %d", exiti);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  yamlTester_context_t *yamlTester_contextp = (yamlTester_context_t *) userDatavp;

  *inputsp              = yamlTester_contextp->inputs;
  *inputlp              = yamlTester_contextp->inputl;
  *characterStreambp    = 1;
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;
  *eofbp                = 1;

  return 1;
}
