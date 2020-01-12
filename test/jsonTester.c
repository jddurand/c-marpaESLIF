#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <ctype.h>
#include <marpaESLIF.h>

#include "jsonTesterData.c"

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp);
static short dumpb(size_t indentl, short commab, marpaESLIFValueResult_t *marpaESLIFValueResultp);

typedef struct marpaESLIFTester_context {
  char    *inputs;
  size_t   inputl;
} marpaESLIFTester_context_t;

int main(int argc, char **argv) {
  marpaESLIF_t                *marpaESLIFp            = NULL;
  genericLogger_t             *genericLoggerp         = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarJsonp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFJSONDecodeOption_t marpaESLIFJSONDecodeOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFTester_context_t   marpaESLIFTester_context = { NULL, 0 };
  int                          exiti = 0;
  test_element_t              *test_elementp;
  short                        jsonb;
  int                          nberrori = 0;
  test_element_chunk_t        *chunkp;
  char                        *p;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarJsonp = marpaESLIFJSON_decode_newp(marpaESLIFp, 1 /* strictb */);
  if (marpaESLIFGrammarJsonp == NULL) {
    goto err;
  }

  test_elementp = &(tests[0]);
  
  while (test_elementp->names != NULL) {

    if ((argc == 2) && (strcmp(argv[1], test_elementp->names) != 0)) {
      goto next;
    }

    marpaESLIFJSONDecodeOption.disallowDupkeysb                = 0;
    marpaESLIFJSONDecodeOption.maxDepthl                       = 0;
    marpaESLIFJSONDecodeOption.noReplacementCharacterb         = 0;
    marpaESLIFJSONDecodeOption.positiveInfinityActionp         = NULL; /* +Infinity action */
    marpaESLIFJSONDecodeOption.negativeInfinityActionp         = NULL; /* -Infinity action */
    marpaESLIFJSONDecodeOption.positiveNanActionp              = NULL; /* +Nan action */
    marpaESLIFJSONDecodeOption.negativeNanActionp              = NULL; /* -Nan action */
    marpaESLIFJSONDecodeOption.numberActionp                   = NULL; /* Number action */

    marpaESLIFTester_context.inputl = 0;

    chunkp = &(test_elementp->chunks[0]);
    while (chunkp->contentl > 0) {
      marpaESLIFTester_context.inputl += chunkp->contentl;
      chunkp++;
    }

    marpaESLIFTester_context.inputs = (char *) malloc(marpaESLIFTester_context.inputl);
    if (marpaESLIFTester_context.inputs == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
      goto err;
    }

    chunkp = &(test_elementp->chunks[0]);
    p = marpaESLIFTester_context.inputs;
    while (chunkp->contentl > 0) {
      memcpy(p, chunkp->contents, chunkp->contentl);
      p += chunkp->contentl;
      chunkp++;
    }

    marpaESLIFRecognizerOption.userDatavp        = &marpaESLIFTester_context;
    marpaESLIFRecognizerOption.readerCallbackp   = inputReaderb;
    marpaESLIFRecognizerOption.disableThresholdb = 0;
    marpaESLIFRecognizerOption.exhaustedb        = 0;
    marpaESLIFRecognizerOption.newlineb          = 1;
    marpaESLIFRecognizerOption.trackb            = 0;
    marpaESLIFRecognizerOption.bufsizl           = 0;
    marpaESLIFRecognizerOption.buftriggerperci   = 50;
    marpaESLIFRecognizerOption.bufaddperci       = 50;
    marpaESLIFRecognizerOption.ifActionResolverp = NULL;

    marpaESLIFValueOption.userDatavp            = NULL; /* User specific context */
    marpaESLIFValueOption.ruleActionResolverp   = NULL; /* Will return the function doing the wanted rule action */
    marpaESLIFValueOption.symbolActionResolverp = NULL; /* Will return the function doing the wanted symbol action */
    marpaESLIFValueOption.importerp             = NULL;
    marpaESLIFValueOption.highRankOnlyb         = 1;    /* Default: 1 */
    marpaESLIFValueOption.orderByRankb          = 1;    /* Default: 1 */
    marpaESLIFValueOption.ambiguousb            = 0;    /* Default: 0 */
    marpaESLIFValueOption.nullb                 = 0;    /* Default: 0 */
    marpaESLIFValueOption.maxParsesi            = 0;    /* Default: 0 */

    GENERICLOGGER_INFOF(genericLoggerp, "Scanning JSON %s", test_elementp->names);

    jsonb = marpaESLIFJSON_decodeb(marpaESLIFGrammarJsonp, &marpaESLIFJSONDecodeOption, &marpaESLIFRecognizerOption, &marpaESLIFValueOption);
    if (test_elementp->names[0] == 'i') {
      /* Implementation defined */
      if (jsonb) {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK (implementation defined)", test_elementp->names);
      } else {
        GENERICLOGGER_NOTICEF(genericLoggerp, "%s => KO (implementation defined)", test_elementp->names);
      }
    } else if (test_elementp->names[0] == 'n') {
      if (jsonb) {
        GENERICLOGGER_ERRORF(genericLoggerp, "%s => KO (success when it should have failed)", test_elementp->names);
        ++nberrori;
        exiti = 1;
      } else {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK", test_elementp->names);
      }
    } else if (test_elementp->names[0] == 'y') {
      if (jsonb) {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK", test_elementp->names);
      } else {
        GENERICLOGGER_ERRORF(genericLoggerp, "%s => KO (failure when it should have succeeded)", test_elementp->names);
        ++nberrori;
        exiti = 1;
      }
    }

    free(marpaESLIFTester_context.inputs);
    marpaESLIFTester_context.inputs = NULL;

  next:
    test_elementp++;
  }

  goto done;

 err:
  exiti = 1;

 done:
  if (marpaESLIFTester_context.inputs != NULL) {
    free(marpaESLIFTester_context.inputs);
  }
  GENERICLOGGER_INFOF(genericLoggerp, "Number of of tests in error: %d", nberrori);
  marpaESLIFGrammar_freev(marpaESLIFGrammarJsonp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 1; /* We say this is a stream of characters */
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}
