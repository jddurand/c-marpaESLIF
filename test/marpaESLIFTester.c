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
static short                           eventManagerb(int *eventCountip, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericLogger_t *genericLoggerp);
static void                            genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);


typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *exceptions = "\n"
  ":start ::= start\n"
  ":discard ::= whitespace event => :symbol\n"
  ":discard ::= arobace event => discard_arobace\n"
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
  "event ^arobace = predicted arobace\n"
  "event arobace[] = nulled arobace\n"
  "event arobace$ = completed arobace\n"
  "arobace ::= '@'\n"
  ;

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
  marpaESLIFTester_context_t   marpaESLIFTester_context;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  marpaESLIFValueOption_t      marpaESLIFValueOption;
  marpaESLIFValue_t           *marpaESLIFValuep = NULL;
  short                        continueb;
  short                        exhaustedb;
  const static char           *inputs = "abc 123de@ @f";
  short                        rcValueb;
  int                          eventCounti = 0;
  size_t                       nLexemel;
  size_t                       lexemel;
  char                       **lexemesArrayp;

  genericLoggerp = genericLogger_newp(genericLoggerCallback, NULL /* userDatavp */, GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  /* Dump ESLIF grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIF_grammarp(marpaESLIFp), &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIF_grammarp(marpaESLIFp), &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ESLIF grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
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
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", leveli);
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
  /* Lexemes expected ? */
  if (! marpaESLIFRecognizer_lexeme_expectedb(marpaESLIFRecognizerp, &nLexemel, &lexemesArrayp)) {
    goto err;
  }
  for (lexemel = 0; lexemel < nLexemel; lexemel++) {
    GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Lexeme expected: %s", lexemesArrayp[lexemel]);
  }
  if (! eventManagerb(&eventCounti, marpaESLIFRecognizerp, genericLoggerp)) {
    goto err;
  }
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
      goto err;
    }
    /* Lexemes expected ? */
    if (! marpaESLIFRecognizer_lexeme_expectedb(marpaESLIFRecognizerp, &nLexemel, &lexemesArrayp)) {
      goto err;
    }
    for (lexemel = 0; lexemel < nLexemel; lexemel++) {
      GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Lexeme expected: %s", lexemesArrayp[lexemel]);
    }
    if (! eventManagerb(&eventCounti, marpaESLIFRecognizerp, genericLoggerp)) {
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

  /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
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
  marpaESLIFGrammar_t           *marpaESLIFGrammarp        = marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep));
  marpaESLIFValueRuleCallback_t  marpaESLIFValueRuleCallbackp;
  int                            leveli;

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL /* descp */)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFGrammar_grammar_currentb failure");
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
  marpaESLIFGrammar_t            *marpaESLIFGrammarp        = marpaESLIFRecognizer_grammarp(marpaESLIFValue_recognizerp(marpaESLIFValuep));
  marpaESLIFValueSymbolCallback_t marpaESLIFValueSymbolCallbackp;
  int                             leveli;

  if (! marpaESLIFGrammar_grammar_currentb(marpaESLIFGrammarp, &leveli, NULL /* descp */)) {
    GENERICLOGGER_ERROR(marpaESLIFTester_contextp->genericLoggerp, "marpaESLIFGrammar_grammar_currentb failure");
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
static short eventManagerb(int *eventCountip, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericLogger_t *genericLoggerp)
/*****************************************************************************/
{
  marpaESLIFEvent_t *eventArrayp;
  size_t             eventArrayl;
  size_t             eventArrayIteratorl;
  short              rcb;
  char              *inputs;
  size_t             inputl;
  short              eofb;

  (*eventCountip)++;

  if (! marpaESLIFRecognizer_eventb(marpaESLIFRecognizerp, &eventArrayl, &eventArrayp)) {
    goto err;
  }

  for (eventArrayIteratorl = 0; eventArrayIteratorl < eventArrayl; eventArrayIteratorl++) {
    switch (eventArrayp[eventArrayIteratorl].type) {
    case MARPAESLIF_EVENTTYPE_COMPLETED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_NULLED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_PREDICTED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    case MARPAESLIF_EVENTTYPE_BEFORE:
      if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
        goto err;
      }
      if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, &inputs, &inputl)) {
        goto err;
      }
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s> (character is %c (0x%lx), eofb is %d)", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols, *inputs, (unsigned long) *inputs, (int) eofb);
      if (strcmp(eventArrayp[eventArrayIteratorl].events, "^[a-zA-Z0-9_:]") == 0) {
        GENERICLOGGER_INFOF(genericLoggerp, "[%3d] ... Pushing single alternative <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].symbols);
        if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, eventArrayp[eventArrayIteratorl].symbols, 1)) {
          goto err;
        }
        /* Complete can generate again events! */
        /* We have no risk because a given symbol instance can never generate two events: if it has been completed */
        /* and it will be predicted again, then the completion event wins. */
        if (! eventManagerb(eventCountip, marpaESLIFRecognizerp, genericLoggerp)) {
          goto err;
        }
      }
      break;
    case MARPAESLIF_EVENTTYPE_AFTER:
      if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
        goto err;
      }
      if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, &inputs, &inputl)) {
        goto err;
      }
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s> (inputl=%ld, eofbp is %d)", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols, (unsigned long) inputl, (int) eofb);
      break;
    case MARPAESLIF_EVENTTYPE_EXHAUSTED:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] >>> Exhausted event", *eventCountip);
      break;
    case MARPAESLIF_EVENTTYPE_DISCARD:
      GENERICLOGGER_INFOF(genericLoggerp, "[%3d] Event %s for symbol <%s>", *eventCountip, eventArrayp[eventArrayIteratorl].events, eventArrayp[eventArrayIteratorl].symbols);
      break;
    default:
      if (eventArrayp[eventArrayIteratorl].type != MARPAESLIF_EVENTTYPE_NONE) {
        /* Should NEVER happen */
        GENERICLOGGER_WARNF(genericLoggerp, "[%3d] Unsupported event type %d", *eventCountip, eventArrayp[eventArrayIteratorl].type);
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

/*****************************************************************************/
static void genericLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  switch (logLeveli) {
  case GENERICLOGGER_LOGLEVEL_TRACE:
    fprintf(stdout, "<TRACE> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_DEBUG:
    fprintf(stdout, "<DEBUG> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_INFO:
    fprintf(stdout, "<INFO> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_NOTICE:
    fprintf(stdout, "<NOTICE> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_WARNING:
    fprintf(stdout, "<WARNING> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_ERROR:
    fprintf(stderr, "<ERROR> %s\n", msgs); fflush(stderr);
    break;
  case GENERICLOGGER_LOGLEVEL_CRITICAL:
    fprintf(stdout, "<CRITICAL> %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_ALERT:
    fprintf(stdout, "<ALERT %s\n", msgs); fflush(stdout);
    break;
  case GENERICLOGGER_LOGLEVEL_EMERGENCY:
    fprintf(stdout, "<EMERGENCY> %s\n", msgs); fflush(stdout);
    break;
  default:
    fprintf(stdout, "<UNKNOWN LEVEL> %s\n", msgs); fflush(stdout);
    break;
  }
}
