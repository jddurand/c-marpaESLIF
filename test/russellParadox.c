#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *exceptions = "\n"
  "xx ::= [ABC] - yy\n"
  "yy ::= [BCD] - zz\n"
  "zz ::= [CDE] - xx\n"
;

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          grammari;
  genericLogger_t             *genericLoggerp;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFValue_t           *marpaESLIFValuep = NULL;
  short                        continueb;
  short                        exhaustedb;
  const static char           *inputs = "C";
  short                        rcValueb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.bytep               = (void *) exceptions;
  marpaESLIFGrammarOption.bytel               = strlen(exceptions);
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = (marpaESLIFGrammarOption.encodings != NULL) ? strlen(marpaESLIFGrammarOption.encodings) : 0;
  marpaESLIFGrammarOption.encodingOfEncodings = "ASCII";
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammari(marpaESLIFGrammarp, &ngrammari)) {
    for (grammari = 0; grammari < ngrammari; grammari++) {
      if (marpaESLIFGrammar_grammarshowform_by_grammarb(marpaESLIFGrammarp, &grammarshows, grammari, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", grammari);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }

  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = (char *) inputs;
  marpaESLIFTester_context.inputl         = strlen(inputs);

  marpaESLIFRecognizerOption.userDatavp                = &marpaESLIFTester_context;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = inputReaderb;
  marpaESLIFRecognizerOption.disableThresholdb         = 0;
  marpaESLIFRecognizerOption.exhaustedb                = 0;
  marpaESLIFRecognizerOption.newlineb                  = 1;
  marpaESLIFRecognizerOption.bufsizl                   = 0;
  marpaESLIFRecognizerOption.buftriggerperci           = 50;
  marpaESLIFRecognizerOption.bufaddperci               = 50;

  marpaESLIFValueOption.userDatavp             = NULL;
  marpaESLIFValueOption.ruleActionResolverp    = NULL;
  marpaESLIFValueOption.symbolActionResolverp  = NULL;
  marpaESLIFValueOption.freeActionResolverp    = NULL;
  marpaESLIFValueOption.highRankOnlyb          = 1;
  marpaESLIFValueOption.orderByRankb           = 1;
  marpaESLIFValueOption.ambiguousb             = 0;
  marpaESLIFValueOption.nullb                  = 0;
  marpaESLIFValueOption.maxParsesi             = 0;

  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, NULL /* exhaustedbp */, NULL /* marpaESLIFValueResultp */)) {
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);

  GENERICLOGGER_FREE(genericLoggerp);
  return exiti;
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  marpaESLIFTester_context_t *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;

  *inputsp              = marpaESLIFTester_contextp->inputs;
  *inputlp              = marpaESLIFTester_contextp->inputl;
  *eofbp                = 1;
  *characterStreambp    = 0; /* We say this is not a stream of characters - regexp will adapt and to UTF correctness if needed */
  *encodingOfEncodingsp = NULL;
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}

