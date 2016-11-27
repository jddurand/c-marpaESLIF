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

const static char *tests_parse_value[] = {
  "Hand was 2" BLACK_HEART_SUIT_UTF8s " 5" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " 9" BLACK_SPADE_SUIT_UTF8s "",
  "Hand was 2" BLACK_HEART_SUIT_UTF8s " a" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s "",
  "Duplicate card a" BLACK_HEART_SUIT_UTF8s "",
  "Duplicate card j" BLACK_HEART_SUIT_UTF8s "",
  "Hand was 2" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 2" BLACK_DIAMOND_SUIT_UTF8s " 3" BLACK_CLUB_SUIT_UTF8s " 3" BLACK_DIAMOND_SUIT_UTF8s "",
  "No hands were found",
  NULL,
  "No hands were found",
  "Last hand successfully parsed was a" BLACK_HEART_SUIT_UTF8s " 7" BLACK_HEART_SUIT_UTF8s " 7" BLACK_DIAMOND_SUIT_UTF8s " 8" BLACK_CLUB_SUIT_UTF8s " j" BLACK_HEART_SUIT_UTF8s ""
};

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
  int                          eventCounti = 0;
  int                          suit_linei;
  int                          test_datai;
  size_t                       dsll;
  char                        *dsls = NULL;
  test_parse_result_type_t     test_parse_result_type;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  for (test_datai = 0; test_datai < sizeof(tests_input)/sizeof(tests_input[0]); test_datai++) {
    for (suit_linei = 0; suit_linei < sizeof(suit_line)/sizeof(suit_line[0]); suit_linei++) {

      GENERICLOGGER_INFOF(genericLoggerp, "test data No %d, suite line No %d", test_datai, suit_linei);

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

      marpaESLIFGrammar_freev(marpaESLIFGrammarp);
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
      marpaESLIFRecognizerOption.bufsizl                   = 0;
      marpaESLIFRecognizerOption.buftriggerperci           = 50;
      marpaESLIFRecognizerOption.bufaddperci               = 50;

      if (marpaESLIFRecognizerp != NULL) {
        marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
      }
      marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, &marpaESLIFRecognizerOption);
      if (marpaESLIFRecognizerp == NULL) {
        goto check;
      }
      /* genericLogger_logLevel_seti(genericLoggerp, GENERICLOGGER_LOGLEVEL_TRACE); */
      if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
        test_parse_result_type = PARSE_FAILED_BEFORE_END;
        goto check;
      }
      while (continueb) {
        if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
          test_parse_result_type = PARSE_FAILED_BEFORE_END;
          goto check;
        }
      }

      test_parse_result_type = PARSE_OK;
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
    if (marpaESLIFGrammar_ngrammari(marpaESLIFGrammarp, &ngrammari)) {
      for (grammari = 0; grammari < ngrammari; grammari++) {
        if (marpaESLIFGrammar_grammarshowform_by_grammarb(marpaESLIFGrammarp, &grammarshows, grammari, NULL)) {
          GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
          GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "TEST grammar at level %d:", grammari);
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
