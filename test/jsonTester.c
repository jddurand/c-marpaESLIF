#include <stdlib.h>
#include <errno.h>
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

const static char *dsl = "\n"
":default ::= action => ::shift\n"
":start       ::= json\n"
"json         ::= object\n"
"               | array\n"
"object       ::= '{' members '}'       action => do_object\n"
"# comma is provided as a char class here, to ensure that char classes\n"
"# as separators are in the test suite.\n"
"members      ::= pair*                 action => do_array separator => [,]\n"
"pair         ::= string ':' value action => do_array\n"
"value        ::= string\n"
"               | object\n"
"               | number\n"
"               | array\n"
"               | 'true'                action => do_true\n"
"               | 'false'               action => do_true\n"
"               | 'null'                action => ::undef\n"
"array        ::= '[' ']'               action => do_empty_array\n"
"               | '[' elements ']' \n"
"# comma is provided as a char class here, to ensure that char classes\n"
"# as separators are in the test suite.\n"
"elements     ::= value+                action => do_array separator => [,]\n"
"number         ~ int\n"
"               | int frac\n"
"               | int exp\n"
"               | int frac exp\n"
"int            ~ digits\n"
"               | '-' digits\n"
"digits         ~ [\\d]+\n"
"frac           ~ '.' digits\n"
"exp            ~ e digits\n"
"e              ~ 'e'\n"
"               | 'e+'\n"
"               | 'e-'\n"
"               | 'E'\n"
"               | 'E+'\n"
"               | 'E-'\n"
"string       ::= lstring\n"
":lexeme ::= lstring pause => before event => ^lstring\n"
"lstring        ~ quote in_string quote\n"
"quote          ~ [\"]\n"
"in_string      ~ in_string_char*\n"
"in_string_char  ~ [^\"] | '\\\"'\n"
":discard       ::= whitespace\n"
"whitespace     ~ [\\s]+\n"
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
  const static char           *inputs = "abc 123:de@ @f";
  short                        rcValueb;
  char                        *pauses;
  size_t                       pausel;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.bytep               = (void *) dsl;
  marpaESLIFGrammarOption.bytel               = strlen(dsl);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
  marpaESLIFGrammarOption.encodingOfEncodings = NULL;

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

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }

  /* Scan the input */
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }

  while (continueb) {
    /* We have a single event, no need to ask what it is */
    marpaESLIFRecognizer_pausev(marpaESLIFRecognizerp, &pauses, &pausel, NULL /* eofbp */);

    /* Resume */
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
      goto err;
    }
  }

  /* Get value */
  marpaESLIFValueOption.userDatavp             = NULL;
  marpaESLIFValueOption.ruleActionResolverp    = NULL;
  marpaESLIFValueOption.symbolActionResolverp  = NULL;
  marpaESLIFValueOption.freeActionResolverp    = NULL;
  marpaESLIFValueOption.highRankOnlyb          = 1;
  marpaESLIFValueOption.orderByRankb           = 1;
  marpaESLIFValueOption.ambiguousb             = 0;
  marpaESLIFValueOption.nullb                  = 0;
  marpaESLIFValueOption.maxParsesi             = 0;

  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    goto err;
  }

  rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep, NULL);
  if (rcValueb < 0) {
    goto err;
  }

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
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
  *characterStreambp    = 1; /* We say this is a stream of characters */
  *encodingOfEncodingsp = NULL;
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}
