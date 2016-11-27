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
  ":discard ~ WS\n"
  ;

#define BLACK_HEART_SUIT_UTF8s   "\xE2\x99\xA5"
#define BLACK_DIAMOND_SUIT_UTF8s "\xE2\x99\xA6"
#define BLACK_CLUB_SUIT_UTF8s    "\xE2\x99\xA3"
#define BLACK_SPADE_SUIT_UTF8s   "\xE2\x99\xA0"
const static char *suit_line[] = {
  "suit ~ [\\x{2665}\\x{2666}\\x{2663}\\x{2660}]:u", /* Code points */
  "suit ~ [" BLACK_HEART_SUIT_UTF8s BLACK_DIAMOND_SUIT_UTF8s BLACK_CLUB_SUIT_UTF8s BLACK_SPADE_SUIT_UTF8s "]", /* Character class */
  "suit ~ '" BLACK_HEART_SUIT_UTF8s "' | '" BLACK_DIAMOND_SUIT_UTF8s "' | '" BLACK_CLUB_SUIT_UTF8s "'| '" BLACK_SPADE_SUIT_UTF8s "'" /* Characters */
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
  size_t                       dsll;
  char                        *dsls = NULL;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

  marpaESLIFOption.genericLoggerp = genericLoggerp;
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  for (suit_linei = 0; suit_linei < 3; suit_linei++) {
    GENERICLOGGER_INFO (genericLoggerp, "*********************************************");
    GENERICLOGGER_INFOF(genericLoggerp, "Suit line No %d", suit_linei);
    GENERICLOGGER_INFO (genericLoggerp, "*********************************************");

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

    /* Dump grammar */
    if (marpaESLIFGrammar_ngrammari(marpaESLIFGrammarp, &ngrammari)) {
      for (grammari = 0; grammari < ngrammari; grammari++) {
        if (marpaESLIFGrammar_grammarshowform_by_grammarb(marpaESLIFGrammarp, &grammarshows, grammari, NULL)) {
          GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
          GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ADVENT grammar at level %d:", grammari);
          GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
        }
      }
    }
  }

  exiti = 0;
  goto done;

 err:
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
  *characterStreambp    = 0; /* We say this is not a stream of characters - regexp will adapt and to UTF correctness if needed */
  *encodingOfEncodingsp = NULL;
  *encodingsp           = NULL;
  *encodinglp           = 0;

  return 1;
}
