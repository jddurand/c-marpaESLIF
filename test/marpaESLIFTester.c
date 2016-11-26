#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static marpaESLIFValueRuleCallback_t   ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t symbolActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short                           default_meta_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short                           default_lexeme_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);
static short                           inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static short                           eventManagerb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericLogger_t *genericLoggerp);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *exceptions = "\n"
  ":start ::= start\n"
  ":discard ::= whitespace event => discard$\n"
  "event ^start = predicted start\n"
  "event start[] = nulled start\n"
  "event start$ = completed start\n"
  "start ::= thisstart - startException\n"
  "\n"
  "event ^thisstart = predicted thisstart\n"
  "event thisstart[] = nulled thisstart\n"
  "event thisstart$ = completed thisstart\n"
  "thisstart ::= chars hereiam\n"
  "\n"
  "event ^hereiam = predicted hereiam\n"
  "event hereiam[] = nulled hereiam\n"
  "event hereiam = completed hereiam\n"
  "hereiam ::= \n"
  "\n"
  "event ^chars = predicted chars\n"
  "event chars[] = nulled chars\n"
  "event chars$ = completed chars\n"
  "chars ::= char*\n"
  "\n"
  "event ^char = predicted start\n"
  "event char[] = nulled start\n"
  "event char$ = completed start\n"
  ":lexeme ::= <char> pause => before event => ^[a-zA-Z0-9_:]\n"
  ":lexeme ::= <char> pause => after event => [a-zA-Z0-9_:]$\n"
  "char ~ [a-zA-Z0-9_:]\n"
  "\n"
  "event ^start_exception = predicted startException\n"
  "event start_exception[] = nulled startException\n"
  "event start_exception$ = completed startException\n"
  "startException ::= chars ':' chars\n"
  "\n"
  "event ^whitespace = predicted whitespace\n"
  "event whitespace[] = nulled whitespace\n"
  "event whitespace$ = completed whitespace\n"
  "whitespace ::= [\\s]\n"
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
  const static char           *inputs = "abc 123:def";
  short                        rcValueb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

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

  /* Try recognizer */
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
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }
  if (! eventManagerb(marpaESLIFRecognizerp, genericLoggerp)) {
    goto err;
  }
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
      goto err;
    }
    if (! eventManagerb(marpaESLIFRecognizerp, genericLoggerp)) {
      goto err;
    }
  }

  /* Try value */
  
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

  while (1) {
    rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep, NULL);
    if (rcValueb < 0) {
      goto err;
    } else if (rcValueb == 0) {
      break;
    }
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

/****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/****************************************************************************/
{
  marpaESLIFTester_context_t    *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  marpaESLIFGrammar_t           *marpaESLIFGrammarp;
  marpaESLIFValueRuleCallback_t  marpaESLIFValueRuleCallbackp;
  int                            grammari;
  int                            leveli;

  if (marpaESLIFTester_contextp == NULL) {
    goto err;
  }
  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFValue_grammarib failure");
    goto err;
  }
  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
  if (marpaESLIFGrammarp == NULL) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFValue_grammarp failure");
    goto err;
  }
  if (marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL /* descp */)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFGrammar_leveli_by_grammarb failure");
    goto err;
  }
  /* We have only one level here */
  if (leveli != 0) {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "leveli = %d", leveli);
    goto err;
  }
  if (strcmp(actions, "default_meta_actionb") == 0) {
    marpaESLIFValueRuleCallbackp = default_meta_actionb;
  } else {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueRuleCallbackp = NULL;
 done:
  return marpaESLIFValueRuleCallbackp;
}

/****************************************************************************/
static marpaESLIFValueSymbolCallback_t symbolActionResolverp(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
/****************************************************************************/
{
  marpaESLIFTester_context_t     *marpaESLIFTester_contextp = (marpaESLIFTester_context_t *) userDatavp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp;
  marpaESLIFValueSymbolCallback_t marpaESLIFValueSymbolCallbackp;
  int                             grammari;
  int                             leveli;

  if (marpaESLIFTester_contextp == NULL) {
    goto err;
  }
  if (! marpaESLIFValue_grammarib(marpaESLIFValuep, &grammari)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFValue_grammarib failure");
    goto err;
  }
  marpaESLIFGrammarp = marpaESLIFValue_grammarp(marpaESLIFValuep);
  if (marpaESLIFGrammarp == NULL) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFValue_grammarp failure");
    goto err;
  }
  if (marpaESLIFGrammar_leveli_by_grammarb(marpaESLIFGrammarp, &leveli, grammari, NULL /* descp */)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFGrammar_leveli_by_grammarb failure");
    goto err;
  }
  /* We have only one level here */
  if (leveli != 0) {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "leveli = %d", leveli);
    goto err;
  }
  if (strcmp(actions, "default_lexeme_actionb") == 0) {
    marpaESLIFValueSymbolCallbackp = default_lexeme_actionb;
  } else {
    GENERICLOGGER_ERRORF(marpaESLIFTester_contextp->genericLoggerp, "Unsupported action \"%s\"", actions);
    goto err;
  }

  goto done;

 err:
  marpaESLIFValueSymbolCallbackp = NULL;
 done:
  return marpaESLIFValueSymbolCallbackp;
}

/****************************************************************************/
static short default_meta_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/****************************************************************************/
{
  return 0;
}

/****************************************************************************/
static short default_lexeme_actionb(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/****************************************************************************/
{
  return 0;
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

/*****************************************************************************/
static short eventManagerb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericLogger_t *genericLoggerp)
/*****************************************************************************/
{
  marpaESLIFEvent_t *eventArrayp;
  size_t             eventArrayl;
  size_t             eventArrayIteratorl;
  short              rcb;
  char              *inputs;
  size_t             inputl;
  short              eofb;

  marpaESLIFRecognizer_eventb(marpaESLIFRecognizerp, &eventArrayl, &eventArrayp);

  for (eventArrayIteratorl = 0; eventArrayIteratorl < eventArrayl; eventArrayIteratorl++) {
    switch (eventArrayp[eventArrayIteratorl].type) {
    case MARPAESLIF_EVENTTYPE_COMPLETED:
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s for symbol <%s>", eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_NULLED:
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s for symbol <%s>", eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_PREDICTED:
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s for symbol <%s>", eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_BEFORE:
      marpaESLIFRecognizer_inputv(marpaESLIFRecognizerp, &inputs, &inputl, &eofb);
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s for symbol <%s> (character is %c (0x%lx), eofb is %d)", eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols, *inputs, (unsigned long) *inputs, (int) eofb);
      if (strcmp(eventArrayp[eventArrayIteratorl].events, "^[a-zA-Z0-9_:]") == 0) {
        if (! marpaESLIFRecognizer_alternative_lengthb(marpaESLIFRecognizerp, 1)) {
          goto err;
        }
        GENERICLOGGER_INFOF(genericLoggerp, "... Pushing alternative <%s>", eventArrayp[eventArrayIteratorl].symbols);
        if (! marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizerp, eventArrayp[eventArrayIteratorl].symbols)) {
          goto err;
        }
        if (! marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp)) {
          goto err;
        }
        /* Complete can generate again events! */
        /* We have no risk because a given symbol instance can never generate two events: if it has been completed */
        /* and it will be predicted again, then the completion event wins. */
        if (! eventManagerb(marpaESLIFRecognizerp, genericLoggerp)) {
          goto err;
        }
      }
      break;
    case MARPAESLIF_EVENTTYPE_AFTER:
      marpaESLIFRecognizer_inputv(marpaESLIFRecognizerp, &inputs, &inputl, &eofb);
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s for symbol <%s> (inputl=%ld, eofbp is %d)", eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols, (unsigned long) inputl, (int) eofb);
      break;
    case MARPAESLIF_EVENTTYPE_EXHAUSTED:
      GENERICLOGGER_INFO (genericLoggerp, ">>> Exhausted event");
      break;
    case MARPAESLIF_EVENTTYPE_DISCARD:
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s for symbol <%s>", eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    default:
      if (eventArrayp[eventArrayIteratorl].type != MARPAESLIF_EVENTTYPE_NONE) {
        /* Should NEVER happen */
        GENERICLOGGER_WARNF(genericLoggerp, "Unsupported event type %d", eventArrayp[eventArrayIteratorl].type);
      }
      break;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}
