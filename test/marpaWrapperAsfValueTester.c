#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "marpaWrapper.h"
#include "genericLogger.h"
#include "genericStack.h"

typedef struct traverseContext {
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  int                      *symbolip;
  int                      *ruleip;
  genericStack_t           *inputStackp;
  genericStack_t           *outputStackp;
  genericLogger_t          *genericLoggerp;
} traverseContext_t;

static char *penn_tag_symbols(traverseContext_t *traverseContextp, int symbolIdi);
static char *penn_tag_rules(traverseContext_t *traverseContextp, int ruleIdi);
static short full_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip);
static short okSymbolCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi);
static short okNullingCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli);
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, int arg0i, int argni);
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static short valueNullingCallback(void *userDatavp, int symboli, int resulti);
static void  freeStringStackv(genericStack_t *stringStackp);
static void  freeStringArrayStackv(genericStack_t *stringStackp);

/*
  S ::= firsts seconds

  firsts ::= first+
  seconds ::= second+
  first  ::= A | B
  second ::= B | C

  A ~ 'a'
  B ~ 'b'
  C ~ 'c'
*/
enum { START = 0, S, FIRSTS, FIRST, SECONDS, SECOND, A, B, C, MAX_SYMBOL };
enum { S_RULE,
       FIRSTS_RULE,
       FIRST_RULE01,
       FIRST_RULE02,
       SECONDS_RULE,
       SECOND_RULE01,
       SECOND_RULE02,
       START_RULE,
       MAX_RULE };

int main(int argc, char **argv) {
  marpaWrapperGrammar_t         *marpaWrapperGrammarp = NULL;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp = NULL;
  marpaWrapperAsf_t             *marpaWrapperAsfp = NULL;
  int                            symbolip[MAX_SYMBOL];
  int                            ruleip[MAX_RULE];
  int                            rci = 0;
  int                           *symbolArrayp = NULL;
  int                            valuei;
  traverseContext_t              traverseContext = { NULL, symbolip, ruleip, NULL, NULL, GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG) };
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */,
								  0 /* autorankb */,
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0, /* disableThresholdb */
								  0 /* exhaustion */
  };
  marpaWrapperAsfOption_t        marpaWrapperAsfOption        = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* highRankOnlyb */,
								  0 /* orderByRankb */,
								  1 /* ambiguousb */,
								  0 /* maxParsesi */
  };

  GENERICSTACK_NEW(traverseContext.inputStackp);
  if (traverseContext.inputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);
  if ( /* start, S, FIRSTS, FIRST, SECONDS, SECOND, A, B, C */
      ((symbolip[ START]  = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[     S]  = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[FIRSTS]  = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[ FIRST] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[SECONDS] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[ SECOND] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[      A] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[      B] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[      C] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      /* S   ::= FIRSTS SECONDS */
      ((ruleip[S_RULE]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[S], symbolip[FIRSTS], symbolip[SECONDS], -1)) < 0)
      ||
      /* FIRSTS  ::= FIRST+  */
      ((ruleip[FIRSTS_RULE] = MARPAWRAPPERGRAMMAR_NEWSEQUENCE(marpaWrapperGrammarp, symbolip[FIRSTS], symbolip[FIRST], 1)) < 0)
      ||
      /* FIRST  ::= A | B  */
      ((ruleip[FIRST_RULE01] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[FIRST], symbolip[A], -1)) < 0)
      ||
      ((ruleip[FIRST_RULE02] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[FIRST], symbolip[B], -1)) < 0)
      ||
      /* SECONDS  ::= SECOND+  */
      ((ruleip[SECONDS_RULE] = MARPAWRAPPERGRAMMAR_NEWSEQUENCE(marpaWrapperGrammarp, symbolip[SECONDS], symbolip[SECOND], 1)) < 0)
      ||
      /* SECOND  ::= B | C  */
      ((ruleip[SECOND_RULE01] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[SECOND], symbolip[B], -1)) < 0)
      ||
      ((ruleip[SECOND_RULE02] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[SECOND], symbolip[C], -1)) < 0)
      ||
      /* start  ::= S */
      ((ruleip[START_RULE]  = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
							  symbolip[START],
							  symbolip[S],
							  -1)) < 0)
        ) {
    rci = 1;
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_precomputeb(marpaWrapperGrammarp) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
    if (marpaWrapperRecognizerp == NULL) {
      rci = 1;
    }
  }

  if (rci == 0) {
    /* We use the stack indices directly into Marpa, and Marpa does not like indice 0 which means "unvalued". */
    /* Nevertheless, during the value phase, indice 0 will be used                                            */
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, NULL);
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }

  /* --- */
  /* abc */
  /* --- */
  
  /* - */
  /* a */
  /* - */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "a");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[A],
				     GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */,
				     1 /* length */) == 0) {
      rci = 1;
    }
  }

  /* - */
  /* b */
  /* - */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "b");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[B],
				     GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */,
				     1 /* length */) == 0) {
      rci = 1;
    }
  }

  /* - */
  /* c */
  /* - */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "c");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[C],
				     GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */,
				     1 /* length */) == 0) {
      rci = 1;
    }
  }

  if (rci == 0) {
    marpaWrapperAsfp = marpaWrapperAsf_newp(marpaWrapperRecognizerp, &marpaWrapperAsfOption);
    if (marpaWrapperAsfp == NULL) {
      rci = 1;
    }
  }

  /* -------------------------- */
  /* Do the parse tree traverse */
  /* -------------------------- */

  /* Full traverser */
  GENERICSTACK_NEW(traverseContext.outputStackp);
  if (traverseContext.outputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }
  if (marpaWrapperAsf_traverseb(marpaWrapperAsfp, full_traverserCallbacki, &traverseContext, &valuei)) {
    genericStack_t *stringStackp;
    int             i;
    GENERICLOGGER_INFO(traverseContext.genericLoggerp, "full traverser returns:");

    stringStackp = GENERICSTACK_GET_PTR(traverseContext.outputStackp, valuei);
    for (i = 0; i < GENERICSTACK_USED(stringStackp); i++) {
      GENERICLOGGER_INFOF(traverseContext.genericLoggerp, "Indice %d:\n%s", i, GENERICSTACK_GET_PTR(stringStackp, i));
    }    
  } else {
    GENERICLOGGER_ERROR(traverseContext.genericLoggerp, "marpaWrapperAsf_traverseb failure");
  }
  freeStringArrayStackv(traverseContext.outputStackp);

  marpaWrapperAsf_freev(marpaWrapperAsfp);
  marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  marpaWrapperGrammar_freev(marpaWrapperGrammarp);
  GENERICLOGGER_FREE(marpaWrapperAsfOption.genericLoggerp);
  GENERICLOGGER_FREE(marpaWrapperRecognizerOption.genericLoggerp);
  GENERICLOGGER_FREE(marpaWrapperGrammarOption.genericLoggerp);
  GENERICLOGGER_FREE(traverseContext.genericLoggerp);

  /* Input stack has no inner PTR */
  GENERICSTACK_FREE(traverseContext.inputStackp);
  
  return rci;
}

/********************************************************************************/
static char *penn_tag_symbols(traverseContext_t *traverseContextp, int symbolIdi)
/********************************************************************************/
{
  char *s;
  
  switch (symbolIdi) {
  case START:
    s = strdup("[:start:]");
    break;
  case S:
    s = strdup("S");
    break;
  case FIRSTS:
    s = strdup("firsts");
    break;
  case FIRST:
    s = strdup("first");
    break;
  case SECONDS:
    s = strdup("seconds");
    break;
  case SECOND:
    s = strdup("second");
    break;
  case A:
    s = strdup("A");
    break;
  case B:
    s = strdup("B");
    break;
  case C:
    s = strdup("C");
    break;
  default:
    s = NULL;
    errno = EINVAL;
    break;
  }

  if (s == NULL) {
    GENERICLOGGER_ERRORF(traverseContextp->genericLoggerp, "SymbolIdi=%d returns NULL description, %s", symbolIdi, strerror(errno));
  }

  return s;
}

/********************************************************************************/
static char *penn_tag_rules(traverseContext_t *traverseContextp, int ruleIdi)
/********************************************************************************/
{
  char *s;
  
  switch (ruleIdi) {
  case START_RULE:
    s = strdup("[Internal Start Rule]");
    break;
  case S_RULE:
    s = strdup("S_RULE");
    break;
  case FIRSTS_RULE:
    s = strdup("FIRSTS_RULE");
    break;
  case FIRST_RULE01:
    s = strdup("FIRST_RULE01");
    break;
  case FIRST_RULE02:
    s = strdup("FIRST_RULE02");
    break;
  case SECONDS_RULE:
    s = strdup("SECONDS_RULE");
    break;
  case SECOND_RULE01:
    s = strdup("SECOND_RULE01");
    break;
  case SECOND_RULE02:
    s = strdup("SECOND_RULE02");
    break;
  default:
    s = NULL;
    errno = EINVAL;
    break;
  }

  if (s == NULL) {
    GENERICLOGGER_ERRORF(traverseContextp->genericLoggerp, "ruleIdi=%d returns NULL description, %s", ruleIdi, strerror(errno));
  }

  return s;
}

/********************************************************************************/
static short okSymbolCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  short              rcb;
  char              *descs;

  descs = penn_tag_symbols(traverseContextp, symboli);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "okSymbolCallback description failure for symbol %d", symboli);
    goto err;
  }
  
  /* We accept any symbol */
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  GENERICLOGGER_DEBUGF(genericLoggerp, "[okSymbolCallback] Symbol %d %s: returns %d", symboli, descs != NULL ? descs : "???", (int) rcb);
  if (descs != NULL) {
    free(descs);
  }
  return rcb;
}

/********************************************************************************/
static short okNullingCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  short              rcb;
  char              *descs;

  descs = penn_tag_symbols(traverseContextp, symboli);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "okNullingCallback description failure for symbol %d", symboli);
    goto err;
  }
  
  /* We reject any nullable in our grammar */
  rcb = -1;
  goto done;

 err:
  rcb = 0;

 done:
  GENERICLOGGER_DEBUGF(genericLoggerp, "[okSymbolCallback] Symbol %d %s: returns %d", symboli, descs != NULL ? descs : "???", (int) rcb);
  if (descs != NULL) {
    free(descs);
  }
  return rcb;
}

/********************************************************************************/
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, int arg0i, int argni)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  short              rcb;
  char              *descs;

  descs = penn_tag_rules(traverseContextp, rulei);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "okRuleCallback description failure for rule %d", rulei);
    goto err;
  }
  
  /* We want to rejected first ::= 'B' */
  rcb = (rulei == FIRST_RULE02) ? -1 : 1;
  goto done;

 err:
  rcb = 0;

 done:
  GENERICLOGGER_DEBUGF(genericLoggerp, "[okRuleCallback] Rule %d %s: returns %d", rulei, descs != NULL ? descs : "???", (int) rcb);
  if (descs != NULL) {
    free(descs);
  }
  return rcb;
}

/********************************************************************************/
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = traverseContextp->genericLoggerp;
  char              *p                = NULL;
  int                i;
  char              *q;
  char              *descs;
  char              *tmp;
  short              rcb;

  descs = penn_tag_rules(traverseContextp, rulei);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "valueRuleCallback description failure for rule %d", rulei);
    goto err;
  }

  /* "(descs" */
  p = (char *) malloc(1 + strlen(descs) + 1);
  if (p == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[valueRuleCallback] malloc failure, %s", strerror(errno));
    goto err;
  }
  strcpy(p, "(");
  strcat(p, descs);
  for (i = arg0i; i <= argni; i++) {
    q = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, i);
    if (GENERICSTACK_ERROR(traverseContextp->outputStackp)) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[valueRuleCallback] outputStackp get failure, %s", strerror(errno));
      goto err;
    }
    tmp = (char *) realloc(p, strlen(p) + 1 /* space */ + strlen(q) /* value*/ + 1 /* 0 */);
    if (tmp == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "valueRuleCallback realloc failure, %s", strerror(errno));
      goto err;
    }
    p = tmp;
    strcat(p, " ");
    strcat(p, q);
  }
  /* Append ")" */
  tmp = (char *) realloc(p, strlen(p) + 2);
  if (tmp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "valueRuleCallback realloc failure, %s", strerror(errno));
    goto err;
  }
  p = tmp;
  strcat(p, ")");
  GENERICSTACK_SET_PTR(traverseContextp->outputStackp, p, resulti);
  if (GENERICSTACK_ERROR(traverseContextp->outputStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[valueRuleCallback] outputStackp set failure, %s", strerror(errno));
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;
  if (p != NULL) {
    free(p);
  }

 done:
  GENERICLOGGER_DEBUGF(genericLoggerp, "[valueRuleCallback] Rule %s, return %d, output stack [%d..%d] => output stack %d %s", descs != NULL ? descs : "???", (int) rcb, arg0i, argni, resulti, rcb ? p : "");
  if (descs != NULL) {
    free(descs);
  }
  return rcb;
}

/********************************************************************************/
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = traverseContextp->genericLoggerp;
  char              *p                = NULL;
  char              *s                = NULL;
  char              *descs;
  short              rcb;

  descs = penn_tag_symbols(traverseContextp, symboli);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "valueSymbolCallback description failure for symbol %d", symboli);
    goto err;
  }

  s = GENERICSTACK_GET_PTR(traverseContextp->inputStackp, argi);
  if (GENERICSTACK_ERROR(traverseContextp->inputStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[valueSymbolCallback] inputStackp get failure, %s", strerror(errno));
    goto err;
  }
  /* We generate "(descs s)" */
  p = (char *) malloc(1 + strlen(descs) + 1 + strlen(s) + 1 + 1);
  if (p == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[valueSymbolCallback] malloc failure, %s", strerror(errno));
    goto err;
  }
  strcpy(p, "(");
  strcat(p, descs);
  strcat(p, " ");
  strcat(p, s);
  strcat(p, ")");
  GENERICSTACK_SET_PTR(traverseContextp->outputStackp, p, resulti);
  if (GENERICSTACK_ERROR(traverseContextp->outputStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[valueSymbolCallback] outputStackp set failure, %s", strerror(errno));
    goto err;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;
  if (p != NULL) {
    free(p);
  }

 done:
  GENERICLOGGER_DEBUGF(genericLoggerp, "[valueSymbolCallback] Symbol %s, return %d, input stack %d \"%s\" => output stack %d %s", descs != NULL ? descs : "???", (int) rcb, argi, s != NULL ? s : "???", resulti, rcb ? p : "");
  if (descs != NULL) {
    free(descs);
  }
  return rcb;
}

/********************************************************************************/
static short valueNullingCallback(void *userDatavp, int symboli, int resulti)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;

  GENERICLOGGER_DEBUG(genericLoggerp, "[valueNullingCallback] return 0");
  return 0;
}

/* Copy of full traverser of marpaWrapperAsfTester.c, to show what are all the parse trees */
/********************************************************************************/
static short full_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip)
/********************************************************************************/
{
  char               funcs[] = "full_traverserCallbacki";
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  char              *symbolNames = NULL;
  char              *ruleNames = NULL;
  genericStack_t    *stringStackp = NULL;
  int                ruleIdi;
  int                symbolIdi;
  short              rcb = 0;
  int                valuei;
  short              nextb;

  /* This routine converts the glade into a list of Penn-tagged elements.  It is called recursively */

  marpaWrapperAsf_traverse_ruleIdb(traverserp, &ruleIdi);
  marpaWrapperAsf_traverse_symbolIdb(traverserp, &symbolIdi);
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s] => ruleIdi=%d, symbolIdi=%d", funcs, ruleIdi, symbolIdi);

  symbolNames = penn_tag_symbols(traverseContextp, symbolIdi);
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... symbol %s", funcs, ruleIdi, symbolIdi, symbolNames);

  GENERICSTACK_NEW(stringStackp);

  /* A token is a single choice, and we know enough to fully Penn-tag it */
  if (ruleIdi < 0) {
    int     spanIdi;
    int     indicei;
    char   *tokenValues;
    size_t  stringl;
    char   *strings;
    
    marpaWrapperAsf_traverse_rh_valueb(traverserp, 0, &spanIdi, NULL /* lengthip */);
    indicei     = spanIdi + 1; /* The spanId correspond to the inputstack indice spanId+1 */
    tokenValues = GENERICSTACK_GET_PTR(traverseContextp->inputStackp, indicei);

    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... spanIdi=%d", funcs, ruleIdi, symbolIdi, spanIdi);
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Token is \"%s\"", funcs, ruleIdi, symbolIdi, tokenValues);

    /* We want to generate the string "(penntag literal)" */
    stringl = 1 + strlen(symbolNames) + 1 + strlen(tokenValues) + 1 + 1;
    strings = malloc(stringl);
    sprintf(strings, "(%s %s)", symbolNames, tokenValues);

    /* We return a stack of string containing only one string */
    GENERICSTACK_PUSH_PTR(stringStackp, strings);
    
  } else {
    ruleNames = penn_tag_rules(traverseContextp, ruleIdi);
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... rule %s", funcs, ruleIdi, symbolIdi, ruleNames);

    /* Our result will be a list of choices */
    while (1) {
      int             lengthi;
      int             rhIxi;
      genericStack_t *resultStackp;
    
      /* The results at each position are a list of chocies, so
         to produce a new result list, we need to take a Cartesian
         printf("format string" ,a0,a1);oduct of all the choices */

      lengthi = marpaWrapperAsf_traverse_rh_lengthi(traverserp);
      GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... lengthi=%d", funcs, ruleIdi, symbolIdi, lengthi);

      GENERICSTACK_NEW(resultStackp);
      {
        genericStack_t *emptyStackp;
        GENERICSTACK_NEW(emptyStackp);
        GENERICSTACK_PUSH_PTR(resultStackp, emptyStackp);
      }

      for (rhIxi = 0; rhIxi <= lengthi - 1; rhIxi++) {
        genericStack_t *newResultStackp;
        int             i;

        GENERICSTACK_NEW(newResultStackp);

        for (i = 0; i < GENERICSTACK_USED(resultStackp); i++) {
          genericStack_t *oldResultStackp = GENERICSTACK_GET_PTR(resultStackp, i);
          int             childValuei;
          genericStack_t *childValueStackp;
          int             j;

	  marpaWrapperAsf_traverse_rh_valueb(traverserp, rhIxi, &childValuei, NULL /* lengthip */);
          childValueStackp = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, (size_t) childValuei);

          for (j = 0; j < GENERICSTACK_USED(childValueStackp); j++) {
            char           *newValues = GENERICSTACK_GET_PTR(childValueStackp, j);
            int             k;
            genericStack_t *stackp;

            GENERICSTACK_NEW(stackp);

            for (k = 0; k < GENERICSTACK_USED(oldResultStackp); k++) {
              GENERICSTACK_PUSH_PTR(stackp, GENERICSTACK_GET_PTR(oldResultStackp, k));
            }
            GENERICSTACK_PUSH_PTR(stackp, newValues);
            GENERICSTACK_PUSH_PTR(newResultStackp, stackp);
          }
        }

        while (GENERICSTACK_USED(resultStackp) > 0) {
          genericStack_t *stackp = GENERICSTACK_POP_PTR(resultStackp);
          GENERICSTACK_FREE(stackp);
        }
        GENERICSTACK_FREE(resultStackp);
        resultStackp = newResultStackp;
      }

      /* Here resultStackp is a stack of stacks of shallow pointers */

      /* Special case for the start rule */
      if (symbolIdi == START) {
        int i;

        for (i = 0; i < GENERICSTACK_USED(resultStackp); i++) {
          genericStack_t *stackp = GENERICSTACK_GET_PTR(resultStackp, i);
          int             j;
          size_t          stringl;
          char           *strings;

          /* All rh values are concatenated */
          stringl = 0;
          for (j = 0; j < GENERICSTACK_USED(stackp); j++) {
            char *values = GENERICSTACK_GET_PTR(stackp, j);
            stringl += strlen(values);
          }
          strings = malloc(++stringl); /* null character */
          strings[0] = '\0';
          for (j = 0; j < GENERICSTACK_USED(stackp); j++) {
            char *values = GENERICSTACK_GET_PTR(stackp, j);
            strcat(strings, values);
          }
          GENERICSTACK_PUSH_PTR(stringStackp, strings);
        }
        while (GENERICSTACK_USED(resultStackp) > 0) {
          genericStack_t *stackp = GENERICSTACK_POP_PTR(resultStackp);
          GENERICSTACK_FREE(stackp);
        }
        GENERICSTACK_FREE(resultStackp);
        break;

      } else {
        char *joinWs = " ";
        int   i;

        if (symbolIdi == S) {
          joinWs = "\n  ";
        }

        for (i = 0; i < GENERICSTACK_USED(resultStackp); i++) {
          genericStack_t *stackp = GENERICSTACK_GET_PTR(resultStackp, i);
          int             j;
          size_t          stringl;
          char           *strings;

          /* All rh values are concatenated and enclosed in "(penntag XXX)" */
          stringl = 1 + strlen(symbolNames) + 1;
          for (j = 0; j < GENERICSTACK_USED(stackp); j++) {
            char *values = GENERICSTACK_GET_PTR(stackp, j);
            if (j > 0) {
              stringl += strlen(joinWs);
            }
            stringl += strlen(values);
          }
          stringl++;
          strings = malloc(++stringl); /* null character */
          strings[0] = '\0';
          strcat(strings, "(");
          strcat(strings, symbolNames);
          strcat(strings, " ");
          for (j = 0; j < GENERICSTACK_USED(stackp); j++) {
            char *values = GENERICSTACK_GET_PTR(stackp, j);
            if (j > 0) {
              strcat(strings, joinWs);
              stringl += strlen(joinWs);
            }
            strcat(strings, values);
          }
          strcat(strings, ")");
          GENERICSTACK_PUSH_PTR(stringStackp, strings);
        }
        while (GENERICSTACK_USED(resultStackp) > 0) {
          genericStack_t *stackp = GENERICSTACK_POP_PTR(resultStackp);
          GENERICSTACK_FREE(stackp);
        }
        GENERICSTACK_FREE(resultStackp);

        marpaWrapperAsf_traverse_nextb(traverserp, &nextb);
	if (! nextb) {
          break;
        }
      }
    }
  }
  
  /* Return value is a list of choices */
  GENERICSTACK_PUSH_PTR(traverseContextp->outputStackp, stringStackp);
  valuei = (int) (GENERICSTACK_USED(traverseContextp->outputStackp) - 1);
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Returning list of choices pushed at indice %d of output stack", funcs, ruleIdi, symbolIdi, valuei);

  if (valueip != NULL) {
    *valueip = valuei;
  }

  rcb = 1;

  goto done;

  /*
 err:
  rcb = 0;
  */

 done:
  
  if (ruleNames != NULL) {
    free(ruleNames);
  }
  if (symbolNames != NULL) {
    free(symbolNames);
  }

  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] return %d", funcs, ruleIdi, symbolIdi, (int) rcb);
  return rcb;

}

/********************************************************************************/
static void freeStringStackv(genericStack_t *stringStackp)
/********************************************************************************/
{
  int i;
  int usedi;

  usedi = GENERICSTACK_USED(stringStackp);
  if (! GENERICSTACK_ERROR(stringStackp)) {
    for (i = 0; i < usedi; i++) {
      if (GENERICSTACK_IS_PTR(stringStackp, i)) {
	free(GENERICSTACK_GET_PTR(stringStackp, i));
      }
    }
    GENERICSTACK_FREE(stringStackp);
  }
}

/********************************************************************************/
static void freeStringArrayStackv(genericStack_t *stringArrayStackp)
/********************************************************************************/
{
  int i;
  int usedi;

  usedi = GENERICSTACK_USED(stringArrayStackp);
  if (! GENERICSTACK_ERROR(stringArrayStackp)) {
    for (i = 0; i < usedi; i++) {
      if (GENERICSTACK_IS_PTR(stringArrayStackp, i)) {
	freeStringStackv(GENERICSTACK_GET_PTR(stringArrayStackp, i));
      }
    }
    GENERICSTACK_FREE(stringArrayStackp);
  }
}
