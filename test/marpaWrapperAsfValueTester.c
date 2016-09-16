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
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, short *okbp);
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static short valueNullingCallback(void *userDatavp, int symboli, int resulti);

enum { START = 0, FIRSTS, FIRST, SECONDS, SECOND, A, B, C, MAX_SYMBOL };
enum { START_RULE = 0,
       FIRSTS_RULE,
       FIRST_RULE01,
       FIRST_RULE02,
       SECONDS_RULE,
       SECOND_RULE01,
       SECOND_RULE02,
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
  traverseContext_t              traverseContext = { NULL, symbolip, ruleip, NULL, NULL, GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE) };
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */,
								  0 /* autorankb */
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* disableThresholdb */
  };
  marpaWrapperAsfOption_t        marpaWrapperAsfOption        = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_INFO),
								  0 /* highRankOnlyb */,
								  0 /* orderByRankb */,
								  1 /* ambiguousb */
  };

  GENERICSTACK_NEW(traverseContext.inputStackp);
  if (traverseContext.inputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);
  if ( /* start, S, NP, VP, period, NN, NNS, DT, CC, VBZ */
      ((symbolip[ START]  = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
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
      ((ruleip[START_RULE]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[START], symbolip[FIRSTS], symbolip[SECONDS], -1)) < 0)
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
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[A], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
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
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[B], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
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
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[C], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
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

  /* Pruning traverser */
  traverseContext.marpaWrapperAsfp = marpaWrapperAsfp;
  GENERICSTACK_NEW(traverseContext.outputStackp);
  if (traverseContext.outputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }
  if (marpaWrapperAsf_valueb(marpaWrapperAsfp,
			     &traverseContext,
			     okRuleCallback,
			     valueRuleCallback,
			     valueSymbolCallback,
			     valueNullingCallback,
			     &valuei)) {
    GENERICLOGGER_INFOF(traverseContext.genericLoggerp, "Pruned value traverser returns:\n%s", GENERICSTACK_GET_PTR(traverseContext.outputStackp, (size_t) valuei));
  }
  while (GENERICSTACK_USED(traverseContext.outputStackp) > 0) {
    free(GENERICSTACK_POP_PTR(traverseContext.outputStackp));
  }
  GENERICSTACK_FREE(traverseContext.outputStackp);
 
  if (marpaWrapperAsfp != NULL) {
    marpaWrapperAsf_freev(marpaWrapperAsfp);
  }
  if (marpaWrapperRecognizerp != NULL) {
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  }
  GENERICLOGGER_FREE(marpaWrapperAsfOption.genericLoggerp);
  GENERICLOGGER_FREE(marpaWrapperRecognizerOption.genericLoggerp);

  if (marpaWrapperGrammarp != NULL) {
    marpaWrapperGrammar_freev(marpaWrapperGrammarp);
  }
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
  case FIRSTS:
    s = strdup("FIRSTS");
    break;
  case FIRST:
    s = strdup("FIRST");
    break;
  case SECONDS:
    s = strdup("SECONDS");
    break;
  case SECOND:
    s = strdup("SECOND");
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
    break;
  }

  if (s == NULL) {
    GENERICLOGGER_ERRORF(traverseContextp->genericLoggerp, "SymbolIdi=%d returns NULL description", symbolIdi);
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
    break;
  }

  if (s == NULL) {
    GENERICLOGGER_ERRORF(traverseContextp->genericLoggerp, "ruleIdi=%d returns NULL description", ruleIdi);
  }

  return s;
}

/********************************************************************************/
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, short *okbp)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  short              okb = (rulei == FIRST_RULE02) ? 0 : 1;
  short              rcb = 1;
  char              *descs;

  descs = penn_tag_rules(traverseContextp, rulei);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "valueRuleCallback description failure for rule %d", rulei);
    goto err;
  }
  
  /* We want to rejected first ::= 'B' */
  if (okbp != NULL) {
    *okbp = okb;
  }

  if (! rcb) {
    GENERICLOGGER_TRACEF(genericLoggerp, "okRuleCallback called for rule %s says not ok", descs);
  }
  return rcb;

 err:
  GENERICLOGGER_WARNF(genericLoggerp, "okRuleCallback failure for rule %d", rulei);
  return 0;
}

/********************************************************************************/
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  int                i;
  char              *p;
  char              *q;
  char              *descs;
  char              *tmp;

  descs = penn_tag_rules(traverseContextp, rulei);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "valueRuleCallback description failure for rule %d", rulei);
    goto err;
  }

  /* "(descs" */
  p = (char *) malloc(1 + strlen(descs) + 1);
  if (p == NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback strdup, %s", strerror(errno));
    goto err;
  }
  strcpy(p, "(");
  strcat(p, descs);
  for (i = arg0i; i <= argni; i++) {
    q = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, i);
    if (GENERICSTACK_ERROR(traverseContextp->outputStackp)) {
      GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback outputStackp get failure, %s", strerror(errno));
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
    GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback outputStackp set failure, %s", strerror(errno));
    goto err;
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "valueRuleCallback called for rule %s, stack [%d..%d] => stack %d \"%s\" ", descs, arg0i, argni, resulti, p);
  return 1;

 err:
  return 0;
}

/********************************************************************************/
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  char              *s;
  char              *p;
  char              *descs;

  descs = penn_tag_symbols(traverseContextp, symboli);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "valueSymbolCallback description failure for symbol %d", symboli);
    goto err;
  }

  s = GENERICSTACK_GET_PTR(traverseContextp->inputStackp, argi);
  if (GENERICSTACK_ERROR(traverseContextp->inputStackp)) {
    GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback inputStackp get failure, %s", strerror(errno));
    goto err;
  }
  /* We generate "(descs s)" */
  p = (char *) malloc(1 + strlen(descs) + 1 + strlen(s) + 1 + 1);
  if (p == NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback strdup, %s", strerror(errno));
    goto err;
  }
  strcpy(p, "(");
  strcat(p, descs);
  strcat(p, " ");
  strcat(p, s);
  strcat(p, ")");
  GENERICSTACK_SET_PTR(traverseContextp->outputStackp, p, resulti);
  if (GENERICSTACK_ERROR(traverseContextp->outputStackp)) {
    GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback outputStackp set failure, %s", strerror(errno));
    goto err;
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "valueSymbolCallback called for symbol %s, stack %d \"%s\" => stack %d \"%s\" ", descs, argi, s, resulti, p);
  return 1;

 err:
  return 0;
}

/********************************************************************************/
static short valueNullingCallback(void *userDatavp, int symboli, int resulti)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;

  GENERICLOGGER_TRACEF(genericLoggerp, "valueNullingCallback called for symbol %s => stack %d", penn_tag_symbols(traverseContextp, symboli), resulti);

  GENERICLOGGER_ERROR(genericLoggerp, "valueNullingCallback is not supported in this test");
  return 0;
}
