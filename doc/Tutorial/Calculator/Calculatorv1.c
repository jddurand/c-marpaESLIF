#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <marpaESLIF.h>
#include <genericLogger.h>

const static char *grammars =
  "Expression ::=\n"
  "    /[\\d]+/\n"
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
  char                        *grammarshows;
  int                          exiti;

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

  if (marpaESLIFGrammar_grammarshowform_currentb(marpaESLIFGrammarp, &grammarshows)) {
    GENERICLOGGER_INFOF(genericLoggerp, "Grammar show:\n%s", grammarshows);
    exit(1);
  }

  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);
  GENERICLOGGER_FREE(genericLoggerp);

  exit(0);
}
