#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

const static char *grammars =
  "    Sentence        ::= Atomicsentence | ComplexSentence\n"
  "    Atomicsentence  ::= TRUE | FALSE | Symbol\n"
  "    Symbol          ::= /[^\\s]+/; /* Anything that is not a space or special characters */\n"
  "    ComplexSentence ::=                        NOT Sentence\n"
  "                     || LPAREN Sentence        AND Sentence RPAREN\n"
  "                     || LPAREN Sentence         OR Sentence RPAREN\n"
  "                     || LPAREN Sentence    IMPLIES Sentence RPAREN\n"
  "                     || LPAREN Sentence EQUIVALENT Sentence RPAREN\n"
  "    TRUE        ~ 'TRUE':i | '1'\n"
  "    FALSE       ~ 'FALSE':i | '0'\n"
  "    LPAREN      ~ '('\n"
  "    RPAREN      ~ ')'\n"
  "    NOT         ~ 'NOT':i | '!'\n"
  "    AND         ~ 'AND':i | '&'\n"
  "    OR          ~ 'OR':i  | '|'\n"
  "    IMPLIES     ~ 'IMPLIES':i  | '=>'\n"
  "    EQUIVALENT  ~ 'EQUIVALENT':i  | '<=>'\n"
  ;

int main() {
  genericLogger_t             *genericLoggerp;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  int                          leveli;
  int                          ngrammari;
  char                        *grammarshows;
  
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

  exit(0);
}
