#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <ctype.h>
#include <marpaESLIF.h>

#include "jsonTesterData.c"

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);
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
  char                        *names;
  short                        foundb = 0;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarJsonp = marpaESLIFJSON_decode_newp(marpaESLIFp, MARPAESLIF_JSONTESTER_STRICT);
  if (marpaESLIFGrammarJsonp == NULL) {
    goto err;
  }

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);

  marpaESLIFJSONDecodeOption.disallowDupkeysb                = 0;
  marpaESLIFJSONDecodeOption.maxDepthl                       = 0;
  marpaESLIFJSONDecodeOption.noReplacementCharacterb         = 0;
  marpaESLIFJSONDecodeOption.positiveInfinityActionp         = NULL; /* +Infinity action */
  marpaESLIFJSONDecodeOption.negativeInfinityActionp         = NULL; /* -Infinity action */
  marpaESLIFJSONDecodeOption.positiveNanActionp              = NULL; /* +Nan action */
  marpaESLIFJSONDecodeOption.negativeNanActionp              = NULL; /* -Nan action */
  marpaESLIFJSONDecodeOption.numberActionp                   = NULL; /* Number action */

  test_elementp = &(tests[0]);
  
  while (test_elementp->names != NULL) {

    if (argc == 2) {
      if (strcmp(argv[1], "-") == 0) {
        /* stdin */
        char stdins[1024];
        size_t stdinl;

        marpaESLIFTester_context.inputl = 0;
        while (1) {
          clearerr(stdin);
          stdinl = fread(stdins, 1, sizeof(stdins), stdin);
          if (stdinl > sizeof(stdins)) {
            fprintf(stderr, "fread returned more bytes than expected\n");
            goto err;
          } else {
            if (ferror(stdin)) {
              fprintf(stderr, "stdin stream error, %s\n", strerror(errno));
              goto err;
            }
            if (stdinl > 0) {
              if (marpaESLIFTester_context.inputs == NULL) {
                marpaESLIFTester_context.inputs = (char *) malloc(stdinl);
                if (marpaESLIFTester_context.inputs == NULL) {
                  GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
                  goto err;
                }
              } else {
                /* Tant pis for the memory leak if realloc failed */
                marpaESLIFTester_context.inputs = (char *) realloc(marpaESLIFTester_context.inputs, marpaESLIFTester_context.inputl + stdinl);
                if (marpaESLIFTester_context.inputs == NULL) {
                  GENERICLOGGER_ERRORF(genericLoggerp, "realloc failure, %s", strerror(errno));
                  goto err;
                }
              }
              memcpy(marpaESLIFTester_context.inputs + marpaESLIFTester_context.inputl, stdins, stdinl);
              marpaESLIFTester_context.inputl += stdinl;
            }
          }
          if (feof(stdin)) {
            goto go;
          }
        }
        foundb = 1;
      } else if (strcmp(argv[1], test_elementp->names) != 0) {
        goto next;
      } else {
        foundb = 1;
      }
    } else {
      foundb = 1;
    }

    if (! foundb) {
      if (argc == 2) {
        GENERICLOGGER_ERRORF(genericLoggerp, "Test %s not found", argv[1]);
      } else {
        GENERICLOGGER_ERROR(genericLoggerp, "No test ?");
      }
      goto err;
    }

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

  go:
    marpaESLIFRecognizerOption.userDatavp               = &marpaESLIFTester_context;
    marpaESLIFRecognizerOption.readerCallbackp          = inputReaderb;
    marpaESLIFRecognizerOption.disableThresholdb        = 0;
    marpaESLIFRecognizerOption.exhaustedb               = 0;
    marpaESLIFRecognizerOption.newlineb                 = 1;
    marpaESLIFRecognizerOption.trackb                   = 0;
    marpaESLIFRecognizerOption.bufsizl                  = 0;
    marpaESLIFRecognizerOption.buftriggerperci          = 50;
    marpaESLIFRecognizerOption.bufaddperci              = 50;
    marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
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

    names = (argc == 2) ? argv[1] : test_elementp->names;
    GENERICLOGGER_INFOF(genericLoggerp, "Scanning JSON %s", names);

    jsonb = marpaESLIFJSON_decodeb(marpaESLIFGrammarJsonp, &marpaESLIFJSONDecodeOption, &marpaESLIFRecognizerOption, &marpaESLIFValueOption);
    if (names[0] == 'i') {
      /* Implementation defined */
      if (jsonb) {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK (implementation defined)", names);
      } else {
        GENERICLOGGER_NOTICEF(genericLoggerp, "%s => KO (implementation defined)", names);
      }
    } else if (names[0] == 'n') {
      if (jsonb) {
        GENERICLOGGER_ERRORF(genericLoggerp, "%s => KO (success when it should have failed)", names);
        ++nberrori;
        exiti = 1;
      } else {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK", names);
      }
    } else if (names[0] == 'y') {
      if (jsonb) {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK", names);
      } else {
        GENERICLOGGER_ERRORF(genericLoggerp, "%s => KO (failure when it should have succeeded)", names);
        ++nberrori;
        exiti = 1;
      }
    } else {
      if (jsonb) {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => OK", names);
      } else {
        GENERICLOGGER_INFOF(genericLoggerp, "%s => KO", names);
        ++nberrori;
        exiti = 1;
      }
    }

    free(marpaESLIFTester_context.inputs);
    marpaESLIFTester_context.inputs = NULL;

  next:
    if ((argc == 2) && (strcmp(argv[1], "-") == 0)) {
      break;
    }
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
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 1; /* We say this is a stream of characters */
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;

  return 1;
}
