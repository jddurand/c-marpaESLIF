#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);

#define UTF_8_STRING "UTF-8"
#define ASCII_STRING "ASCII"

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *selfs =
  "\n"
  "number ::= NUMBER - NUMBEREXCEPTION\n"
  "\n"
  "NUMBER            ~ /\\d+/\n"
  "NUMBEREXCEPTION   ~ '23'\n"
  "\n";

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  genericLogger_t             *genericLoggerp;
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }


  marpaESLIFGrammarOption.bytep               = (void *) selfs;
  marpaESLIFGrammarOption.bytel               = strlen(selfs);
  marpaESLIFGrammarOption.encodings           = UTF_8_STRING;
  marpaESLIFGrammarOption.encodingl           = strlen(UTF_8_STRING);
  marpaESLIFGrammarOption.encodingOfEncodings = ASCII_STRING;
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);

  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  marpaESLIFRecognizerOption.userDatavp                   = &marpaESLIFTester_context; /* User specific context */
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp    = inputReaderb; /* Reader */
  marpaESLIFRecognizerOption.disableThresholdb            = 0; /* Default: 0 */
  marpaESLIFRecognizerOption.exhaustedb                   = 0; /* Exhaustion event. Default: 0 */
  marpaESLIFRecognizerOption.newlineb                     = 1; /* Count line/column numbers. Default: 0 */
  marpaESLIFRecognizerOption.trackb                       = 1; /* Absolute position tracking. Default: 0 */
  marpaESLIFRecognizerOption.bufsizl                      = 0; /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  marpaESLIFRecognizerOption.buftriggerperci              = 50; /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  marpaESLIFRecognizerOption.bufaddperci                  = 50; /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need 
to augment. Recommended: 50 */

  /*
   * /\d+/ - '23'
   *
   * 2 must match
   * 23 must not match
   * 234 must match
   */
  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = "2";
  marpaESLIFTester_context.inputl         = 1;

  /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */, NULL /* marpaESLIFValueResultp */)) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "\"2\" does not match");
    goto err;
  } else {
    GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "\"2\" does match");
  }

  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = "23";
  marpaESLIFTester_context.inputl         = 2;

  /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
  if (marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */, NULL /* marpaESLIFValueResultp */)) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "\"23\" does match");
    goto err;
  } else {
    GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "\"23\" does not match");
  }

  marpaESLIFTester_context.genericLoggerp = genericLoggerp;
  marpaESLIFTester_context.inputs         = "234";
  marpaESLIFTester_context.inputl         = 3;

  /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */, NULL /* marpaESLIFValueResultp */)) {
    GENERICLOGGER_ERROR(marpaESLIFOption.genericLoggerp, "\"234\" does not match");
    goto err;
  } else {
    GENERICLOGGER_INFO(marpaESLIFOption.genericLoggerp, "\"234\" does match");
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
