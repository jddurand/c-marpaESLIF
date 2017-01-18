#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

#define EXTEND_ALTERNATIVE() {                                          \
    if (marpaESLIFAlternativel == 0) {                                  \
      marpaESLIFAlternativep = (marpaESLIFAlternative_t *) malloc(sizeof(marpaESLIFAlternative_t)); \
      if (marpaESLIFAlternativep == NULL) {                             \
        GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno)); \
        goto err;                                                       \
      }                                                                 \
    } else {                                                            \
      marpaESLIFAlternative_t *tmp = (marpaESLIFAlternative_t *) realloc(marpaESLIFAlternativep, marpaESLIFAlternativel + 1); \
      if (tmp == NULL) {                                                \
        GENERICLOGGER_ERRORF(genericLoggerp, "realloc failure, %s", strerror(errno)); \
        goto err;                                                       \
      }                                                                 \
      marpaESLIFAlternativep = tmp;                                     \
    }                                                                   \
    ++marpaESLIFAlternativel;                                           \
  }

const static char *grammars =
  "    Start           ::= <Init many> <Sentence any>\n"
  "    <Sentence any>  ::= Sentence*\n"
  "    <Init many>     ::= Init+\n"
  "    Init            ::= Symbol Set Boolean\n"
  "    Set             ::= SET\n"
  "    Boolean         ::= TRUE | FALSE\n"
  "    Sentence        ::= Atomicsentence | ComplexSentence\n"
  "    Atomicsentence  ::= Boolean | Symbol\n"
  "    Symbol          ::= SYMBOL /* Anything that is not a space or special characters */\n"
  "    ComplexSentence ::=                        NOT Sentence\n"
  "                     || LPAREN Sentence        AND Sentence RPAREN\n"
  "                     || LPAREN Sentence         OR Sentence RPAREN\n"
  "                     || LPAREN Sentence    IMPLIES Sentence RPAREN\n"
  "                     || LPAREN Sentence EQUIVALENT Sentence RPAREN\n"
  "    SET         ~ '='\n"
  "    TRUE        ~ 'TRUE':i | '1'\n"
  "    FALSE       ~ 'FALSE':i | '0'\n"
  "    LPAREN      ~ '('\n"
  "    RPAREN      ~ ')'\n"
  "    NOT         ~ 'NOT':i | '!'\n"
  "    AND         ~ 'AND':i | '&'\n"
  "    OR          ~ 'OR':i  | '|'\n"
  "    IMPLIES     ~ 'IMPLIES':i  | '=>'\n"
  "    EQUIVALENT  ~ 'EQUIVALENT':i  | '<=>'\n"
  "    SYMBOL      ~ /[^\\s]+/n\n"
  "\n"
  "    :discard    ::= whitespace event => discard_whitespace$\n"
  "    whitespace  ::= [\\s]\n"
  "\n"
  "event ^Symbol  = predicted Symbol\n"
  "event ^Set     = predicted Set\n"
  "event ^Boolean = predicted Boolean\n"
  "event Init$ = completed Init\n"
  ;

static short stdinReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);

typedef enum alternativeContext {
  ALTERNATIVE_SYMBOL,
  ALTERNATIVE_BOOLEAN
} alternativeContext_t;

typedef struct readerContext {
  genericLogger_t *genericLoggerp;
  char             c;
} readerContext_t;

int main() {
  genericLogger_t             *genericLoggerp        = NULL;
  marpaESLIF_t                *marpaESLIFp           = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp    = NULL;
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  int                          rci;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          leveli;
  int                          ngrammari;
  char                        *grammarshows;
  readerContext_t              readerContext;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  size_t                       nEventl;
  size_t                       eventl;
  marpaESLIFEvent_t           *eventArrayp;
  short                        eofb;
  short                        symbolb;
  char                        *inputs;
  size_t                       inputl;
  char                        *symbols;
  size_t                       symboll;
  short                        setb;
  short                        trueb;
  short                        falseb;
  marpaESLIFAlternative_t     *marpaESLIFAlternativep;
  marpaESLIFAlternative_t     *thismarpaESLIFAlternativep;
  size_t                       marpaESLIFAlternativel = 0;
  size_t                       l;
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

  marpaESLIFGrammarOption.bytep               = (void *) grammars;
  marpaESLIFGrammarOption.bytel               = strlen(grammars);
  marpaESLIFGrammarOption.encodings           = NULL;
  marpaESLIFGrammarOption.encodingl           = 0;
  marpaESLIFGrammarOption.encodingOfEncodings = NULL;
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

#if 0
  /* Dump grammar */
  if (marpaESLIFGrammar_ngrammarib(marpaESLIFGrammarp, &ngrammari)) {
    for (leveli = 0; leveli < ngrammari; leveli++) {
      if (marpaESLIFGrammar_grammarshowform_by_levelb(marpaESLIFGrammarp, &grammarshows, leveli, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "Grammar at level %d:", leveli);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }
#endif

  /* Start a recognizer */
  readerContext.genericLoggerp = genericLoggerp;

  marpaESLIFRecognizerOption.userDatavp                = &readerContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = stdinReaderb;
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

  /* Loop on events */
  while (marpaESLIFRecognizer_eventb(marpaESLIFRecognizerp, &nEventl, &eventArrayp)) {
    if (! marpaESLIFRecognizer_inputb(marpaESLIFRecognizerp, &inputs, &inputl)) {
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "Current input length is %ld", (unsigned long) inputl);
    for (eventl = 0; eventl < nEventl; eventl++) {
      /* Note that events are always sorted in order: predictions, nullables, completions, discard, exhaustion */
      GENERICLOGGER_INFOF(genericLoggerp, "Event %s on symbol %s", eventArrayp[eventl].events, eventArrayp[eventl].symbols);
      if (strcmp(eventArrayp[eventl].events, "^Symbol") == 0) {
        /* By putting the rule Symbol ::= SYMBOL, we know that ^Symbol */
        /* is equivalent to "I need the SYMBOL lexeme" */
        if (! marpaESLIFRecognizer_lexeme_tryb(marpaESLIFRecognizerp, "SYMBOL", &symbolb)) {
          goto err;
        }
        GENERICLOGGER_INFOF(genericLoggerp, "... SYMBOL try is %s", symbolb ? "ok" : "ko");
        if (symbolb) {
          /* Get the symbol as the recognizer saw it */
          if (! marpaESLIFRecognizer_lexeme_last_tryb(marpaESLIFRecognizerp, "SYMBOL", &symbols, &symboll)) {
            goto err;
          }
          GENERICLOGGER_INFOF(genericLoggerp, "... SYMBOL match on \"%s\"", symbols);
          /* Feed symbol */
          EXTEND_ALTERNATIVE();
          thismarpaESLIFAlternativep = &(marpaESLIFAlternativep[marpaESLIFAlternativel - 1]);
          thismarpaESLIFAlternativep->lexemes = "SYMBOL";
          thismarpaESLIFAlternativep->value.type = MARPAESLIF_VALUE_TYPE_ARRAY;
          thismarpaESLIFAlternativep->value.u.p = malloc(symboll);
          if (thismarpaESLIFAlternativep->value.u.p == NULL) {
            GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
            goto err;
          }
          memcpy(thismarpaESLIFAlternativep->value.u.p, symbols, symboll);
          thismarpaESLIFAlternativep->value.contexti = ALTERNATIVE_SYMBOL;
          thismarpaESLIFAlternativep->value.sizel = symboll;
          thismarpaESLIFAlternativep->grammarLengthl = 1;
          /* Push it */
          if (! marpaESLIFRecognizer_lexeme_readb(marpaESLIFRecognizerp, thismarpaESLIFAlternativep, symboll)) {
            goto err;
          }
        }
      }
      else if (strcmp(eventArrayp[eventl].events, "^Set") == 0) {
        if (! marpaESLIFRecognizer_lexeme_tryb(marpaESLIFRecognizerp, "SET", &setb)) {
          goto err;
        }
        GENERICLOGGER_INFOF(genericLoggerp, "... SET try is %s", setb ? "ok" : "ko");
        if (setb) {
        }
      }
      else if (strcmp(eventArrayp[eventl].events, "^Boolean") == 0) {
        /* No proof that recognizer saw TRUE or FALSE - we have to try */
        if ((! marpaESLIFRecognizer_lexeme_tryb(marpaESLIFRecognizerp, "TRUE", &trueb)) ||
            (! marpaESLIFRecognizer_lexeme_tryb(marpaESLIFRecognizerp, "FALSE", &falseb))) {
          goto err;
        }
        if (trueb) {
          GENERICLOGGER_INFO(genericLoggerp, "... TRUE will match");
          goto done;
        }
        if (falseb) {
          GENERICLOGGER_INFO(genericLoggerp, "... FALSE will match");
          goto done;
        }
      }
      if (strcmp(eventArrayp[eventl].events, "Init$") == 0) {
        /* Get last symbol and boolean */
        if (! marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFRecognizerp, "SYMBOL", &pauses, &pausel)) {
          goto err;
        }
        GENERICLOGGER_INFOF(genericLoggerp, "... Last SYMBOL is %s", pauses);
      }
    }
    /* Stop if eof */
    if (! marpaESLIFRecognizer_isEofb(marpaESLIFRecognizerp, &eofb)) {
      goto err;
    }
    if (eofb) {
      GENERICLOGGER_INFO(genericLoggerp, "EOF");
      break;
    }
  }

  rci = 0;
  goto done;

 err:
  rci = 1;

 done:
  /* All these calls are NULL protected */
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);
  for (l = 0; l < marpaESLIFAlternativel; l++) {
    thismarpaESLIFAlternativep = &(marpaESLIFAlternativep[l]);
    if (thismarpaESLIFAlternativep->value.u.p != NULL) {
      free(thismarpaESLIFAlternativep->value.u.p);
    }
  }

  exit(rci);
}

/*****************************************************************************/
static short stdinReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp)
/*****************************************************************************/
{
  readerContext_t *readerContextp = (readerContext_t *) userDatavp;
  genericLogger_t *genericLoggerp = readerContextp->genericLoggerp;
  int              i;

  i = fgetc(stdin);
  if (i != EOF) {
    readerContextp->c = (unsigned char) i;

    *inputsp              = (char *) &(readerContextp->c);
    *inputlp              = sizeof(char);
    *eofbp                = 0;
    if (iscntrl(i)) {
      GENERICLOGGER_DEBUGF(genericLoggerp, "... fgetc(stdin) returned 0x%x", readerContextp->c);
    } else {
      GENERICLOGGER_DEBUGF(genericLoggerp, "... fgetc(stdin) returned '%c' (0x%x)", readerContextp->c, readerContextp->c);
    }
  } else {
    *inputsp              = NULL;
    *inputlp              = 0;
    *eofbp                = 1;    /* One chunk */
    GENERICLOGGER_DEBUG(genericLoggerp, "... fgetc(stdin) returned EOF");
  }


  *characterStreambp    = 1;    /* We say this is a stream of characters */
  *encodingOfEncodingsp = NULL; /* let marpaESLIF deal with encoding */
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}

