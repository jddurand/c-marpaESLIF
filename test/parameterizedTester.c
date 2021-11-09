#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp);

typedef struct marpaESLIF_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIF_context_t;

const static char *grammars = "# Parameterized grammar\n"
  ":discard ::= /[\\s]+/\n"
  "\n"
  "top  ::= rhs1\n"
  "rhs1 ::= . => ::lua->rhs-->(1, nil, 'Input should be \"1\"')\n"
  "       | . => ::lua->rhs-->(2, nil, 'Input should be \"2\"')\n"
  "       | . => ::lua->rhs-->(3, nil, 'Input should be \"3\"')\n"
  "       | . => ::lua->rhs-->(4, nil, 'Input should be \"4\"')\n"
  "\n"
  "<luascript>\n"
  "function rhs(parameter, undef, explanation)\n"
  "  local output;\n"
  "  if (parameter > 4) then\n"
  "    output = \"start ::= '\"..tostring(parameter)..\"'\"\n"
  "  else\n"
  "    output = \"start ::= . => ::lua->rhs-->(5, { x = 'Value of x', y = 'Value of y' }, 'Input should be 5')\"\n"
  "  end\n"
  "\n"
  "  print('[lua] parameter='..tostring(parameter)..', undef='..tostring(undef)..', explanation='..tostring(explanation)..' => '..tostring(output))\n"
  "  output:encoding('UTF-8') -- Just to verify that setting the encoding works and is propagated\n"
  "  return output\n"
  "end\n"
  "</luascript>\n"
  ;

const static char *inputs = "5";

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  marpaESLIF_context_t         marpaESLIF_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  char                        *grammarscripts;
  marpaESLIFSymbol_t          *stringSymbolp = NULL;
  marpaESLIFSymbol_t          *stringSymbol2p = NULL;
  marpaESLIFSymbol_t          *regexSymbolp = NULL;
  marpaESLIFString_t           string;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  short                        continueb;
  short                        exhaustedb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);

  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
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
      GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "----------------------------------");
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Parameterized grammar at level %d:", leveli);
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "----------------------------------\n%s", grammarshows);
    }
  }

  marpaESLIF_context.genericLoggerp = genericLoggerp;
  marpaESLIF_context.inputs         = (char *) inputs;
  marpaESLIF_context.inputl         = strlen(inputs);

  marpaESLIFRecognizerOption.userDatavp        = &marpaESLIF_context; /* User specific context */
  marpaESLIFRecognizerOption.readerCallbackp   = inputReaderb; /* Reader */
  marpaESLIFRecognizerOption.disableThresholdb = 0; /* Default: 0 */
  marpaESLIFRecognizerOption.exhaustedb        = 0; /* Exhaustion event. Default: 0 */
  marpaESLIFRecognizerOption.newlineb          = 1; /* Count line/column numbers. Default: 0 */
  marpaESLIFRecognizerOption.trackb            = 1; /* Absolute position tracking. Default: 0 */
  marpaESLIFRecognizerOption.bufsizl           = 10; /* Minimum stream buffer size: Recommended: 0 (internally, a system default will apply) */
  marpaESLIFRecognizerOption.buftriggerperci   = 50; /* Excess number of bytes, in percentage of bufsizl, where stream buffer size is reduced. Recommended: 50 */
  marpaESLIFRecognizerOption.bufaddperci       = 50; /* Policy of minimum of bytes for increase, in percentage of current allocated size, when stream buffer size need to augment. Recommended: 50 */
  marpaESLIFRecognizerOption.ifActionResolverp        = NULL;
  marpaESLIFRecognizerOption.eventActionResolverp     = NULL;
  marpaESLIFRecognizerOption.regexActionResolverp     = NULL;
  marpaESLIFRecognizerOption.generatorActionResolverp = NULL;
  marpaESLIFRecognizerOption.importerp                = NULL;

  GENERICLOGGER_LEVEL_SET(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE);
  GENERICLOGGER_NOTICE(genericLoggerp, "Testing interactive recognizer");
  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }
  GENERICLOGGER_INFOF(genericLoggerp, "After scan: continueb=%d, exhaustedb=%d", (int) continueb, (int) exhaustedb);
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
      goto err;
    }
    GENERICLOGGER_INFOF(genericLoggerp, "After resume: continueb=%d, exhaustedb=%d", (int) continueb, (int) exhaustedb);
  }
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFRecognizerp = NULL;

  GENERICLOGGER_NOTICE(genericLoggerp, "Testing parse");
  marpaESLIF_context.inputs         = (char *) inputs;
  marpaESLIF_context.inputl         = strlen(inputs);
  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOption, NULL /* marpaESLIFValueOptionp */, NULL /* exhaustedbp */)) {
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  }
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_LEVEL_SET(marpaESLIFOption.genericLoggerp, GENERICLOGGER_LOGLEVEL_INFO);
  GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Exiting with status %d", exiti);
  GENERICLOGGER_FREE(genericLoggerp);
  exit(exiti);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingsp, size_t *encodinglp, marpaESLIFReaderDispose_t *disposeCallbackpp)
/*****************************************************************************/
{
  marpaESLIF_context_t *marpaESLIF_contextp = (marpaESLIF_context_t *) userDatavp;

  *inputsp              = marpaESLIF_contextp->inputs;
  *inputlp              = marpaESLIF_contextp->inputl;
  *characterStreambp    = 0;
  *encodingsp           = NULL;
  *encodinglp           = 0;
  *disposeCallbackpp    = NULL;

  *eofbp = 1;

  return 1;
}
