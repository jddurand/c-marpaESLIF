#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <genericLogger.h>
#include <marpaESLIF.h>

static marpaESLIFValueRuleCallback_t ruleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static marpaESLIFValueSymbolCallback_t symbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions);
static short default_meta_action(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb);
static short default_lexeme_action(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti);

typedef struct marpaESLIFTester_context {
  genericLogger_t *genericLoggerp;
} marpaESLIFTester_context_t;

const static char *metags = "\n"
  "inaccessible is warn by default\n"
  "autorank is on by default\n"
  ":start ::= Script\n"
  ":desc ::= 'This is Grammar Top Level with UTF-8 characters: r\xc3\xa9sum\xc3\xa9'\n"
  "Script ::= Expression+ separator => comma action => do_script\n"
  "<empty rule> ::= \n"
  "; # null statement\n"
  "Expression ::= \n"
  "            Number\n"
  "            | '(' Expression ')' action => do_parens assoc => group\n"
  "           || Expression '**' Expression action => do_pow assoc => right\n"
  "           || Expression '*' Expression action => do_multiply\n"
  "            | Expression '/' Expression action => do_divide\n"
  "           || Expression '+' Expression action => do_add\n"
  "            | Expression '-' Expression action => do_subtract\n"
  "comma ::= [,]\n"
  "Number ::= [\\d]+"
;

int main() {
  marpaESLIF_t             *marpaESLIFp        = NULL;
  marpaESLIFGrammar_t      *marpaESLIFGrammarp = NULL;
  char                     *helpers            = NULL;
  marpaESLIFOption_t        marpaESLIFOption;
  marpaESLIFGrammarOption_t marpaESLIFGrammarOption;
  int                       exiti;
  int                       ngrammari;
  char                     *grammarshows;
  int                      *ruleip;
  size_t                    rulel;
  int                       grammari;
  size_t                    l;

  marpaESLIFOption.genericLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG);
  marpaESLIFp = marpaESLIF_newp(&marpaESLIFOption);
  if (marpaESLIFp == NULL) {
    goto err;
  }

  marpaESLIFGrammarOption.grammars            = (char *) metags;
  marpaESLIFGrammarOption.grammarl            = strlen(metags);
  marpaESLIFGrammarOption.encodings           = "UTF-8";
  marpaESLIFGrammarOption.encodingl           = (marpaESLIFGrammarOption.encodings != NULL) ? strlen(marpaESLIFGrammarOption.encodings) : 0;
  marpaESLIFGrammarOption.encodingOfEncodings = "ASCII";
  marpaESLIFGrammarp = marpaESLIFGrammar_newp(marpaESLIFp, &marpaESLIFGrammarOption);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  if (marpaESLIFGrammar_ngrammari(marpaESLIFGrammarp, &ngrammari)) {
    for (grammari = 0; grammari < ngrammari; grammari++) {
      if (marpaESLIFGrammar_grammarshowform_by_grammarb(marpaESLIFGrammarp, &grammarshows, grammari, NULL)) {
        GENERICLOGGER_INFO (marpaESLIFOption.genericLoggerp, "-------------------------");
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "ESLIF grammar at level %d:", grammari);
        GENERICLOGGER_INFOF(marpaESLIFOption.genericLoggerp, "-------------------------\n\n%s", grammarshows);
      }
    }
  }

  exiti = 0;
  goto done;
  
 err:
  exiti = 1;

 done:
  if (helpers != NULL) {
    free(helpers);
  }
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIF_freev(marpaESLIFp);

  GENERICLOGGER_FREE(marpaESLIFOption.genericLoggerp);
  return exiti;
}

/****************************************************************************/
static marpaESLIFValueRuleCallback_t ruleActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
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
  if (strcmp(actions, "default_meta_action") == 0) {
    marpaESLIFValueRuleCallbackp = default_meta_action;
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
static marpaESLIFValueSymbolCallback_t symbolActionResolver(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *actions)
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
  if (strcmp(actions, "default_lexeme_action") == 0) {
    marpaESLIFValueSymbolCallbackp = default_lexeme_action;
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
static short default_meta_action(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, int arg0i, int argni, int resulti, short nullableb)
/****************************************************************************/
{
  return 0;
}

/****************************************************************************/
static short default_lexeme_action(void *userDatavp, marpaESLIFValue_t *marpaESLIFValuep, char *bytep, size_t bytel, int resulti)
/****************************************************************************/
{
  return 0;
}
