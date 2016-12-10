#include <marpaESLIF.h>
#include <genericLogger.h>
#include <string.h>

const static char *grammars =
  ":default ::= action => do_first_arg\n"
    ":discard ::= [\\s]\n"
  ":discard ::= /^#.*$/u           # Perl Comments - unicode support\n"
  "\n"
  "Script   ::= Expression+ separator => comma action => do_script\n"
  "comma    ::= ','\n"
  "Number   ::= /[\\d]+/\n"
  "Expression ::=\n"
  "    Number\n"
  "    | '(' Expression ')'              action => do_parens    assoc => group\n"
  "   ||     Expression '**' Expression  action => do_pow       assoc => right\n"
  "   ||     Expression  '*' Expression  action => do_multiply\n"
  "    |     Expression  '/' Expression  action => do_divide\n"
  "   ||     Expression  '+' Expression  action => do_add\n"
  "    |     Expression  '-' Expression  action => do_subtract\n"
  "\n";

int main() {
  marpaESLIF_t                *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  marpaESLIFOption_t           marpaESLIFOption;
  marpaESLIFGrammarOption_t    marpaESLIFGrammarOption;
  int                          leveli;
  genericLogger_t             *genericLoggerp;
  int                          exiti;
  int                          ngrammari;
  char                        *grammarshows;

  genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);

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

  exiti = 0;
  goto done;

 err:
  exiti = 1;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);

  GENERICLOGGER_FREE(genericLoggerp);
  return exiti;
}
