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
  "CDSect          ::= CDStart CData CDEnd\n"
  "CDStart         ::= '<![CDATA['\n"
  "CData           ::= CHARDATA - CHARDATAEXCEPTION\n"
  "CData           ::= # Using CHARDATA removed CData nullable aspect\n"
  "CDEnd           ::= ']]>'\n"
  "_CHARDATA         ~ [\\x{9}\\x{A}\\x{D}\\x{20}-\\x{D7FF}\\x{E000}-\\x{FFFD}\\x{10000}-\\x{10FFFF}]:u\n"
  "CHARDATA          ~ _CHARDATA*\n"
  "CHARDATAEXCEPTION ~ CHARDATA ']]>' CHARDATA\n"
#ifdef TEST_STACKOVERFLOW
  "CHARDATAEXCEPTION ~ CData@-1 - CData@-1\n"
#endif
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
  struct testdata {
    char *inputs;
    short wantedstatusb;
  } testdata[] = { { "<![CDATA[2]]>",                                                                            1 },
                   { "<![CDATA[]]>",                                                                             1 },
                   { "12<![CDATA[]]>",                                                                           0 },
                   { "<![CDATA[<script>...</script>]]>",                                                         1 },
                   { "<![CDATA[]]]]>",                                                                           1 },
                   { "<![CDATA[<data>some embedded xml</data>]]>",                                               1 },
                   { "<![CDATA[ They're saying \"x < y\" & that \"z > y\" so I guess that means that z > x ]]>", 1 },
                   { "<![CDATA[An in-depth look at creating applications with XML, using <, >,]]>",              1 },
                   { "<![CDATA[</this is malformed!</malformed</malformed & worse>]]>",                          1 },
                   { "<![CDATA[if (c<10)]]>",                                                                    1 },
                   { "<![CDATA[ something ]]",                                                                   0 }
  };
  int i;

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

  for (i = 0; i < sizeof(testdata)/sizeof(testdata[0]); i++) {
    marpaESLIFTester_context.genericLoggerp = genericLoggerp;
    marpaESLIFTester_context.inputs         = testdata[i].inputs;
    marpaESLIFTester_context.inputl         = strlen(testdata[i].inputs);

    /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
    if (marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */, NULL /* marpaESLIFValueResultp */)) {
      if (testdata[i].wantedstatusb) {
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "\"%s\" does match", testdata[i].inputs);
      } else {
        GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "\"%s\" does match", testdata[i].inputs);
        goto err;
      }
    } else {
      if (testdata[i].wantedstatusb) {
        GENERICLOGGER_ERRORF(marpaESLIFOption.genericLoggerp, "\"%s\" does not match", testdata[i].inputs);
        goto err;
      } else {
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "\"%s\" does not match", testdata[i].inputs);
      }
    }
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
