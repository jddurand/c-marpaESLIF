#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

typedef struct reader_context {
  char            *inputs;
  size_t           inputl;
} reader_context_t;

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);

const static char *grammars =
  ":default ::= action => do_double\n"
  ":discard ::= [\\s]\n"
  "Expression ::=\n"
  "    /[\\d]+/  action => do_int\n"
  "    | '(' Expression ')'              assoc => group\n"
  "   ||     Expression '**' Expression  assoc => right\n"
  "   ||     Expression  '*' Expression\n"
  "    |     Expression  '/' Expression\n"
  "   ||     Expression  '+' Expression\n"
  "    |     Expression  '-' Expression\n"
  "\n";

int main() {
  genericLogger_t             *genericLoggerp;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  reader_context_t             reader_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp;
  char                         inputs[4096];
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFValue_t           *marpaESLIFValuep;
  int                          rcValueb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    exit(1);
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    exit(1);
  }

  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
  marpaESLIFGrammarOption.encodingOfEncodings = NULL;
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    exit(1);
  }

  marpaESLIFRecognizerOption.userDatavp                = &reader_context;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = inputReaderb;
  marpaESLIFRecognizerOption.disableThresholdb         = 0;  /* No disable of threshold warning */
  marpaESLIFRecognizerOption.exhaustedb                = 0;  /* No exhaustion event */
  marpaESLIFRecognizerOption.newlineb                  = 1;  /* Do newline counting */
  marpaESLIFRecognizerOption.bufsizl                   = 0;  /* Recommended value */
  marpaESLIFRecognizerOption.buftriggerperci           = 50; /* Recommended value */
  marpaESLIFRecognizerOption.bufaddperci               = 50; /* Recommended value */
  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    exit(1);
  }

  if (fgets(inputs, sizeof(inputs), stdin) == NULL) {
    perror("fgets");
    exit(1);
  }
  reader_context.inputs = inputs;
  reader_context.inputl = strlen(inputs);
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp,
                                   0 /* initialEventsb */,
                                   NULL /* continuebp*/,
                                   NULL /* exhaustedbp */)) {
    exit(1);
  }

  /* marpaESLIFRecognizer_progressLogb(marpaESLIFRecognizerp, -1, -1, GENERICLOGGER_LOGLEVEL_DEBUG); */

  marpaESLIFValueOption.userDatavp             = NULL; /* No context... why not */
  marpaESLIFValueOption.ruleActionResolverp    = NULL; /* No rule action resolver! */
  marpaESLIFValueOption.symbolActionResolverp  = NULL; /* No symbol action resolver... Okay we use the default */
  marpaESLIFValueOption.freeActionResolverp    = NULL; /* No free action resolver... Okay if we generate no pointer */
  marpaESLIFValueOption.highRankOnlyb          = 1;    /* Recommended value */
  marpaESLIFValueOption.orderByRankb           = 1;    /* Recommended value */
  marpaESLIFValueOption.ambiguousb             = 0;    /* Recommended value */
  marpaESLIFValueOption.nullb                  = 0;    /* Recommended value */
  marpaESLIFValueOption.maxParsesi             = 0;    /* Get all parse tree values - meaningless because ambiguousb is false */
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    exit(1);
  }
  while (1) {
    rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep, NULL);
    if (rcValueb < 0) {
      exit(1);
    } else if (rcValueb == 0) {
      break;
    }
  }

  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);

  exit(0);
}

/*****************************************************************************/
static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  reader_context_t *reader_contextp = (reader_context_t *) userDatavp;
  
  *inputsp              = reader_contextp->inputs;
  *inputlp              = reader_contextp->inputl;
  *eofbp                = 1;    /* One chunk */
  *characterStreambp    = 1;    /* We say this is a stream of characters */
  *encodingOfEncodingsp = NULL; /* let marpaESLIF deal with encoding */
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}
