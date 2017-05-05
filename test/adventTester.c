#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <genericStack.h>
#include <marpaESLIF.h>

static short inputReaderb(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp, short *characterStreambp, char **encodingOfEncodingsp, char **encodingsp, size_t *encodinglp);
static int   card2inti(char *inputs, size_t inputl);
static char *int2cards(genericLogger_t *genericLoggerp, int cardi);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
  char            *inputs;
  size_t           inputl;
} marpaESLIFTester_context_t;

const static char *base_dsl = "\n"
  ":start ::= deal\n"
  "deal ::= hands\n"
  "hands ::= hand | hands ';' hand\n"
  "hand ::= card card card card card\n"
  "card ~ face suit\n"
  "face ~ [2-9jqka] | '10'\n"
  "WS ~ [\\s]\n"
  ":discard ::= WS\n"
  "\n"
  ":lexeme ::= <card> pause => after event => card\n"
  ;

#define BLACK_HEART_SUIT_UTF8s   "\xE2\x99\xA5"
#define BLACK_DIAMOND_SUIT_UTF8s "\xE2\x99\xA6"
#define BLACK_CLUB_SUIT_UTF8s    "\xE2\x99\xA3"
#define BLACK_SPADE_SUIT_UTF8s   "\xE2\x99\xA0"
const static char *suit_line[] = {
  "suit ~ [\\x{2665}\\x{2666}\\x{2663}\\x{2660}]:u", /* Code points - the u flag is NEEDED */
  "suit ~ [" BLACK_HEART_SUIT_UTF8s BLACK_DIAMOND_SUIT_UTF8s BLACK_CLUB_SUIT_UTF8s BLACK_SPADE_SUIT_UTF8s "]", /* Character class */
  "suit ~ '" BLACK_HEART_SUIT_UTF8s "' | '" BLACK_DIAMOND_SUIT_UTF8s "' | '" BLACK_CLUB_SUIT_UTF8s "'| '" BLACK_SPADE_SUIT_UTF8s "'" /* Characters */
};

const static char *tests_input[] = {
  "2" BLACK_HEART_SUIT_UTF8s " 5" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " 9" BLACK_SPADE_SUIT_UTF8s "",
  "2" BLACK_HEART_SUIT_UTF8s " a" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s "",
  "a" BLACK_HEART_SUIT_UTF8s " a" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s "",
  "a" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s "; 10" BLACK_HEART_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s " q" BLACK_HEART_SUIT_UTF8s " k" BLACK_HEART_SUIT_UTF8s " a" BLACK_HEART_SUIT_UTF8s "",
  "2" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 2" BLACK_DIAMOND_SUIT_UTF8s " 3" BLACK_CLUB_SUIT_UTF8s " 3" BLACK_DIAMOND_SUIT_UTF8s "",
  "2" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 2" BLACK_DIAMOND_SUIT_UTF8s " 3" BLACK_CLUB_SUIT_UTF8s "",
  "2" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 2" BLACK_DIAMOND_SUIT_UTF8s " 3" BLACK_CLUB_SUIT_UTF8s " 3" BLACK_DIAMOND_SUIT_UTF8s " 1" BLACK_DIAMOND_SUIT_UTF8s "",
  "2" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 2" BLACK_DIAMOND_SUIT_UTF8s " 3" BLACK_CLUB_SUIT_UTF8s "",
  "a" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s "; 10" BLACK_HEART_SUIT_UTF8s " j" BLACK_CLUB_SUIT_UTF8s " q" BLACK_HEART_SUIT_UTF8s " k" BLACK_HEART_SUIT_UTF8s "",
};

typedef enum test_parse_result_type {
  PARSE_OK,
  PARSE_STOPPED_BY_APPLICATION,
  PARSE_REACHED_END_OF_INPUT_BUT_FAILED,
  PARSE_FAILED_BEFORE_END,
  PARSE_FAILED_AFTER_FINDING_HANDS
} test_parse_result_type_t;

const static test_parse_result_type_t tests_parse_result[] = {
  PARSE_OK,
  PARSE_OK,
  PARSE_STOPPED_BY_APPLICATION,
  PARSE_STOPPED_BY_APPLICATION,
  PARSE_OK,
  PARSE_REACHED_END_OF_INPUT_BUT_FAILED,
  PARSE_FAILED_BEFORE_END,
  PARSE_REACHED_END_OF_INPUT_BUT_FAILED,
  PARSE_FAILED_AFTER_FINDING_HANDS
};

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
  short                        rcValueb;
  int                          suit_linei;
  int                          test_datai;
  size_t                       dsll;
  char                        *dsls = NULL;
  test_parse_result_type_t     test_parse_result_type;
  char                        *pauses;
  size_t                       pausel;
  int                          cardi;
  char                        *cards = NULL;
  genericStack_t              *cardStackp = NULL;
  char                        *offsetp;
  size_t                       lengthl;
  char                        *handsp = NULL;
  short                        isWSb;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  if (genericLoggerp == NULL) {
    perror("GENERICLOGGER_NEW");
    goto err;
  }

  GENERICSTACK_NEW(cardStackp);
  if (GENERICSTACK_ERROR(cardStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "cardStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  for (test_datai = 0; test_datai < sizeof(tests_input)/sizeof(tests_input[0]); test_datai++) {
    for (suit_linei = 0; suit_linei < sizeof(suit_line)/sizeof(suit_line[0]); suit_linei++) {

      /* Free in order - marpaESLIF interface will do nothing if input is NULL */
      marpaESLIFValue_freev(marpaESLIFValuep);
      marpaESLIFValuep = NULL;
      marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
      marpaESLIFRecognizerp = NULL;
      marpaESLIFGrammar_freev(marpaESLIFGrammarp);
      marpaESLIFGrammarp = NULL;

      GENERICLOGGER_INFOF(genericLoggerp, "Test data No %d, suite line No %d", test_datai, suit_linei);

      if (dsls != NULL) {
        free(dsls);
      }
      dsll = strlen(base_dsl) + strlen(suit_line[suit_linei]);
      dsls = (char *) malloc(dsll + 1);
      if (dsls == NULL) {
        GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      strcpy(dsls, base_dsl);
      strcat(dsls, suit_line[suit_linei]);

      marpaESLIFGrammarOption.bytep               = (void *) dsls;
      marpaESLIFGrammarOption.bytel               = dsll;
      marpaESLIFGrammarOption.encodings           = NULL;
      marpaESLIFGrammarOption.encodingl           = 0;
      marpaESLIFGrammarOption.encodingOfEncodings = NULL;

      marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
      if (marpaESLIFGrammarp == NULL) {
        goto err;
      }

      marpaESLIFTester_context.genericLoggerp = genericLoggerp;
      marpaESLIFTester_context.inputs         = (char *) tests_input[test_datai];
      marpaESLIFTester_context.inputl         = strlen(tests_input[test_datai]);

      marpaESLIFRecognizerOption.userDatavp                = &marpaESLIFTester_context;
      marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = inputReaderb;
      marpaESLIFRecognizerOption.disableThresholdb         = 0;
      marpaESLIFRecognizerOption.exhaustedb                = 0;
      marpaESLIFRecognizerOption.newlineb                  = 1;
      marpaESLIFRecognizerOption.trackb                    = 1;
      marpaESLIFRecognizerOption.bufsizl                   = 0;
      marpaESLIFRecognizerOption.buftriggerperci           = 50;
      marpaESLIFRecognizerOption.bufaddperci               = 50;

      marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
      if (marpaESLIFRecognizerp == NULL) {
        goto check;
      }

      /* Scan the input */
      if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
        test_parse_result_type = PARSE_FAILED_BEFORE_END;
        goto check;
      }

      GENERICSTACK_FREE(cardStackp);
      GENERICSTACK_NEW(cardStackp);
      if (GENERICSTACK_ERROR(cardStackp)) {
        GENERICLOGGER_ERRORF(genericLoggerp, "cardStackp initialization failure, %s", strerror(errno));
        goto err;
      }
      while (continueb) {
        /* We have a single event, no need to ask what it is */
        if (! marpaESLIFRecognizer_lexeme_last_pauseb(marpaESLIFRecognizerp, "card", &pauses, &pausel)) {
          goto err;
        }

        /* We arbitrarily transform card data into a number to uniquely identify it */
        cardi = card2inti(pauses, pausel);
        if (cards != NULL) {
          free(cards);
        }
        cards = int2cards(genericLoggerp, cardi);
        GENERICLOGGER_DEBUGF(genericLoggerp, "Got card %s", cards);

        /* Try to see if there is whitespace after */
        if (! marpaESLIFRecognizer_lexeme_tryb(marpaESLIFRecognizerp, "WS", &isWSb)) {
          goto err;
        }
        if (isWSb) {
          GENERICLOGGER_DEBUGF(genericLoggerp, "Card %s is followed by whitespace", cards);
        } else {
          GENERICLOGGER_DEBUGF(genericLoggerp, "Card %s is not followed by whitespace", cards);
        }

        /* Check for duplicate card */
        if (GENERICSTACK_IS_SHORT(cardStackp, cardi)) {
          GENERICLOGGER_DEBUGF(genericLoggerp, "Duplicate card %s", cards);
          test_parse_result_type = PARSE_STOPPED_BY_APPLICATION;
          goto check;
        }
        GENERICSTACK_SET_SHORT(cardStackp, 1, cardi);

        /* Resume */
        if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0, &continueb, &exhaustedb)) {
          test_parse_result_type = PARSE_FAILED_BEFORE_END;
          goto check;
        }
      }

      /* Get eventual value */
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
        rcValueb = 0;
      } else {
        rcValueb = marpaESLIFValue_valueb(marpaESLIFValuep, NULL);
      }

      /* Get eventual last hand */
      if (handsp != NULL) {
        free(handsp);
        handsp = NULL;
      }
      if (marpaESLIFRecognizer_last_completedb(marpaESLIFRecognizerp, "hand", &offsetp, &lengthl)) {
        GENERICLOGGER_NOTICE(genericLoggerp, "Got hands");
        handsp = (char *) malloc(lengthl + 1); /* +1 for the debugger - hiden */
        if (handsp == NULL) {
          GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        memcpy(handsp, marpaESLIFTester_context.inputs, lengthl);
      }

      if (rcValueb > 0) {
        test_parse_result_type = PARSE_OK;
        goto check;
      }
      if (handsp != NULL) {
        test_parse_result_type = PARSE_FAILED_AFTER_FINDING_HANDS;
        goto check;
      }

      test_parse_result_type = PARSE_REACHED_END_OF_INPUT_BUT_FAILED;

    check:
      if (test_parse_result_type != tests_parse_result[test_datai]) {
        GENERICLOGGER_ERRORF(genericLoggerp, "Got test parse result %d, excepted %d", test_parse_result_type, tests_parse_result[test_datai]);
        goto err;
      }
    }
  }

  exiti = 0;
  goto done;

 err:
  if (marpaESLIFGrammarp != NULL) {
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
  }
  exiti = 1;

 done:
  if (dsls != NULL) {
    free(dsls);
  }
  if (cards != NULL) {
    free(cards);
  }
  if (handsp != NULL) {
    free(handsp);
  }
  GENERICSTACK_FREE(cardStackp);
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

/*****************************************************************************/
static int card2inti(char *inputs, size_t inputl)
/*****************************************************************************/
{
  /* card data is:

  "card ~ face suit\n"
  "face ~ [2-9jqka] | '10'\n"

  where suit is is always a 3-bytes UTF8 thingy
  */
  int   i100;
  int   i;
  char *suits;
  
  switch ((unsigned char) *inputs) {
  case '2': i100 = 100; suits = inputs + 1; break;
  case '3': i100 = 110; suits = inputs + 1; break;
  case '4': i100 = 120; suits = inputs + 1; break;
  case '5': i100 = 130; suits = inputs + 1; break;
  case '6': i100 = 140; suits = inputs + 1; break;
  case '7': i100 = 150; suits = inputs + 1; break;
  case '8': i100 = 160; suits = inputs + 1; break;
  case '9': i100 = 170; suits = inputs + 1; break;
  case 'j': i100 = 180; suits = inputs + 1; break;
  case 'q': i100 = 190; suits = inputs + 1; break;
  case 'k': i100 = 200; suits = inputs + 1; break;
  case 'a': i100 = 210; suits = inputs + 1; break;
  default:  i100 = 220; suits = inputs + 2; break;
  }

  switch (((unsigned char) suits[2]) % 10) {
  case 5:  i = 1; break;
  case 6:  i = 2; break;
  case 3:  i = 3; break;
  default: i = 4; break;
  }

  return i100 + i;
}

/*****************************************************************************/
static char *int2cards(genericLogger_t *genericLoggerp, int cardi)
/*****************************************************************************/
{
  /* card data is:

  "card ~ face suit\n"
  "face ~ [2-9jqka] | '10'\n"

  where suit is is always a 3-bytes UTF8 thingy
  */
  int   i100 = (cardi / 10) * 10;
  int   i = cardi % 10;
  char *faces;
  char *suits;
  char *cards;

  switch (i100) {
  case 100: faces = "2"; break;
  case 110: faces = "3"; break;
  case 120: faces = "4"; break;
  case 130: faces = "5"; break;
  case 140: faces = "6"; break;
  case 150: faces = "7"; break;
  case 160: faces = "8"; break;
  case 170: faces = "9"; break;
  case 180: faces = "j"; break;
  case 190: faces = "q"; break;
  case 200: faces = "k"; break;
  case 210: faces = "a"; break;
  default:  faces = "10"; break;
  }

  switch (i) {
  case 1:  suits = "BLACK_HEART_SUIT"; break;
  case 2:  suits = "BLACK_DIAMOND_SUIT"; break;
  case 3:  suits = "BLACK_CLUB_SUIT"; break;
  default: suits = "BLACK_SPADE_SUIT"; break;
  }

  cards = (char *) malloc(strlen(faces) + 1 + strlen(suits) + 1);
  if (cards == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc failure, %s", strerror(errno));
    return NULL;
  }

  strcpy(cards, faces);
  strcat(cards, "/");
  strcat(cards, suits);

  return cards;
}
 
