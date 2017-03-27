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
static short pruning_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip);
static short full_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip);
static short okSymbolCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi);
static short okNullableCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli);
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, int arg0i, int argni);
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static short valueNullingCallback(void *userDatavp, int symboli, int resulti);
static void  freeStringStackv(genericStack_t *stringStackp);
static void  freeStringArrayStackv(genericStack_t *stringStackp);

/*
      S   ::= NP  VP  period

      NP  ::= NN
            | NNS
            | DT  NN
            | NN  NNS
            | NNS CC NNS

      VP  ::= VBZ NP
            | VP VBZ NNS
            | VP CC VP
            | VP VP CC VP
            | VBZ

      period ~ '.'
      CC     ~ 'and'
      DT     ~ 'a' | 'an'
      NN     ~ 'panda'
      NNS    ~ 'shoots' | 'leaves'
      VBZ    ~ 'eats' | 'shoots' | 'leaves'
*/

enum { START = 0, S, NP, VP, period, NN, NNS, DT, CC, VBZ, MAX_SYMBOL };
enum { S_RULE = 0,
       NP_RULE01,
       NP_RULE02,
       NP_RULE03,
       NP_RULE04,
       NP_RULE05,
       VP_RULE01,
       VP_RULE02,
       VP_RULE03,
       VP_RULE04,
       VP_RULE05,
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
								  0  /* exhaustion */
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
  if ( /* start, S, NP, VP, period, NN, NNS, DT, CC, VBZ */
      ((symbolip[ START] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[     S] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[    NP] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[    VP] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[period] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[    NN] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[   NNS] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[    DT] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[    CC] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      ((symbolip[   VBZ] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0)
      ||
      /* S   ::= NP  VP  period */
      ((ruleip[S_RULE]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[S],
						       symbolip[NP], symbolip[VP], symbolip[period], -1)) < 0)
      ||
      /* NP  ::= NN  */
      ((ruleip[NP_RULE01] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NN], -1)) < 0)
      ||
      /* NP  ::= NNS  */
      ((ruleip[NP_RULE02]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NNS], -1)) < 0)
      ||
      /* NP  ::= DT NN */
      ((ruleip[NP_RULE03]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[DT], symbolip[NN], -1)) < 0)
      ||
      /* NP  ::= NN NNS */
      ((ruleip[NP_RULE04]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NN], symbolip[NNS], -1)) < 0)
      ||
      /* NP  ::= NNS CC NNS */
      ((ruleip[NP_RULE05]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NNS], symbolip[CC], symbolip[NNS], -1)) < 0)
      ||
      /* VP  ::= VBZ NP */
      ((ruleip[VP_RULE01]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VBZ], symbolip[NP], -1)) < 0)
      ||
      /* VP  ::= VP VBZ NNS */
      ((ruleip[VP_RULE02]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VP], symbolip[VBZ], symbolip[NNS], -1)) < 0)
      ||
      /* VP  ::= VP CC VP */
      ((ruleip[VP_RULE03]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VP], symbolip[CC], symbolip[VP], -1)) < 0)
      ||
      /* VP  ::= VP VP CC VP */
      ((ruleip[VP_RULE04]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VP], symbolip[VP], symbolip[CC], symbolip[VP], -1)) < 0)
      ||
      /* VP  ::= VBZ */
      ((ruleip[VP_RULE05]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VBZ], -1)) < 0)
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

  /* --------------------------------------------------------------- */
  /* a panda eats shoots and leaves.                                 */
  /* --------------------------------------------------------------- */
  
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
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[DT], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }

  /* ----- */
  /* panda */
  /* ----- */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "panda");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[NN], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }

  /* ---- */
  /* eats */
  /* ---- */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "eats");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[VBZ], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }

  /* ------ */
  /* shoots */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "shoots");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if ((marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symbolip[NNS], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0)
	||
	(marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symbolip[VBZ], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0)
	||
	(marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp) == 0)
	) {
      rci = 1;
    }
  }

  /* --- */
  /* and */
  /* --- */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "and");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[CC], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }

  /* ------ */
  /* leaves */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, "leaves");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if ((marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symbolip[NNS], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0)
	||
	(marpaWrapperRecognizer_alternativeb(marpaWrapperRecognizerp, symbolip[VBZ], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0)
	||
	(marpaWrapperRecognizer_completeb(marpaWrapperRecognizerp) == 0)
	) {
      rci = 1;
    }
  }

  /* ------ */
  /* period */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_PTR(traverseContext.inputStackp, ".");
    if (GENERICSTACK_ERROR(traverseContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[period], GENERICSTACK_USED(traverseContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
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
  if (marpaWrapperAsf_traverseb(marpaWrapperAsfp, pruning_traverserCallbacki, &traverseContext, &valuei)) {
    GENERICLOGGER_INFOF(traverseContext.genericLoggerp, "Pruning traverser returns:\n%s", GENERICSTACK_GET_PTR(traverseContext.outputStackp, valuei));
  } else {
    GENERICLOGGER_ERROR(traverseContext.genericLoggerp, "marpaWrapperAsf_traverseb (pruning) failure");
  }
  freeStringStackv(traverseContext.outputStackp);

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
    GENERICLOGGER_ERROR(traverseContext.genericLoggerp, "marpaWrapperAsf_traverseb (full) failure");
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
static short pruning_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp, int *valueip)
/********************************************************************************/
{
  char               funcs[] = "pruning_traverserCallbacki";
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  char              *symbolNames = NULL;
  char              *ruleNames = NULL;
  genericStack_t    *rhStackp = NULL;
  short              rcb = 0;
  int                valuei;
  int                ruleIdi;
  int                symbolIdi;
  size_t             stringl;
  char              *strings;

  /* This routine converts the glade into a list of Penn-tagged elements.  It is called recursively */

  if (! marpaWrapperAsf_traverse_ruleIdb(traverserp, &ruleIdi)) {
    goto err;
  }
  if (! marpaWrapperAsf_traverse_symbolIdb(traverserp, &symbolIdi)) {
    goto err;
  }
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s] => ruleIdi=%d, symbolIdi=%d", funcs, ruleIdi, symbolIdi);

  symbolNames = penn_tag_symbols(traverseContextp, symbolIdi);
  if (symbolNames == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... symbolNames is NULL", funcs, ruleIdi, symbolIdi);
    goto err;
  }

  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... symbol %s", funcs, ruleIdi, symbolIdi, symbolNames);

  /* A token is a single choice, and we know enough to fully Penn-tag it */
  if (ruleIdi < 0) {
    int     spanIdi;
    int     indicei;
    char   *tokenValues;
    
    if (! marpaWrapperAsf_traverse_rh_valueb(traverserp, 0, &spanIdi, NULL /* lengthip */)) {
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... spanIdi=%d", funcs, ruleIdi, symbolIdi, spanIdi);

    /* The spanId correspond to the inputstack indice spanId+1 */
    indicei = spanIdi + 1;
    if (! GENERICSTACK_IS_PTR(traverseContextp->inputStackp, indicei)) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... Nothing at input stack indice %d", funcs, ruleIdi, symbolIdi, (int) indicei);
      goto err;
    }
    tokenValues = GENERICSTACK_GET_PTR(traverseContextp->inputStackp, indicei);
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Token is \"%s\"", funcs, ruleIdi, symbolIdi, tokenValues);
    /* We want to generate the string "(penntag literal)" */
    stringl = 1 + strlen(symbolNames) + 1 + strlen(tokenValues) + 1 + 1;
    strings = malloc(stringl);
    if (strings == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... malloc failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
      goto err;
    }
    sprintf(strings, "(%s %s)", symbolNames, tokenValues);
    
  } else {
    int     lengthi;
    int     rhIxi;
    int     indicei;
    
    ruleNames = penn_tag_rules(traverseContextp, ruleIdi);
    if (ruleNames == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... ruleNames is NULL", funcs, ruleIdi, symbolIdi);
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... rule %s", funcs, ruleIdi, symbolIdi, ruleNames);

    lengthi = marpaWrapperAsf_traverse_rh_lengthi(traverserp);
    if (lengthi < 0) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... lengthl is < 0", funcs, ruleIdi, symbolIdi);
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... lengthi=%d", funcs, ruleIdi, symbolIdi, lengthi);

    GENERICSTACK_NEW(rhStackp);
    if (GENERICSTACK_ERROR(rhStackp)) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... rhStackp initialization failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
      goto err;
    }
    for (rhIxi = 0; rhIxi <= lengthi - 1; rhIxi++) {
      int   valuei;
      char *values;

      if (! marpaWrapperAsf_traverse_rh_valueb(traverserp, rhIxi, &valuei, NULL /* lengthip */)) {
        goto err;
      }

      /* We expect valuei to be in outputStack */
      indicei = valuei;
      if (! GENERICSTACK_IS_PTR(traverseContextp->outputStackp, indicei)) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... Nothing at output stack indice %d", funcs, ruleIdi, symbolIdi, (int) indicei);
	goto err;
      }
      values = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, indicei);
      GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Value is \"%s\" for rh No %d", funcs, ruleIdi, symbolIdi, values, rhIxi);

      GENERICSTACK_PUSH_INT(rhStackp, valuei);
      if (GENERICSTACK_ERROR(rhStackp)) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... rhStackp push failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
	goto err;
      }
    }

    /* Special case for the start rule */
    if (symbolIdi == START) {
      int  i;

      /* All rh values are concatenated with a space in between and a newline at the end */
      stringl = 0;
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	int rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  stringl++;
	}
	stringl += strlen((char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      stringl++; /* newline */
      strings = malloc(++stringl); /* null character */
      if (strings == NULL) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... malloc failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
	goto err;
      }

      strings[0] = '\0';
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	int rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  strcat(strings, " ");
	}
	strcat(strings, (char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      strcat(strings, "\n");
    } else {
      char  *joinWs = " ";
      int    i;

      if (symbolIdi == S) {
	joinWs = "\n  ";
      }

      /* All rh values are concatenated with joinWs in between, the whole beeing enclosed in (penntag XXX) */
      stringl = 1 + strlen(symbolNames) + 1; /* "(penntag " */
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	int rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  stringl += strlen(joinWs);
	}
	stringl += strlen((char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      stringl += 1; /* ")" */
      strings = malloc(++stringl); /* null character */
      if (strings == NULL) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... malloc failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
	goto err;
      }
      strcpy(strings, "(");
      strcat(strings, symbolNames);
      strcat(strings, " ");
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	int rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  strcat(strings, joinWs);
	}
	strcat(strings, (char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      strcat(strings, ")");
    }
  }
  
  GENERICSTACK_PUSH_PTR(traverseContextp->outputStackp, strings);
  if (GENERICSTACK_ERROR(traverseContextp->outputStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ... traverseContextp->outputStackp push failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
    goto err;
  }
  valuei = (int) (GENERICSTACK_USED(traverseContextp->outputStackp) - 1);
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Returning \"%s\" pushed at indice %d of output stack", funcs, ruleIdi, symbolIdi, strings, valuei);

  if (valueip != NULL) {
    *valueip = valuei;
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  if (ruleNames != NULL) {
    free(ruleNames);
  }
  if (symbolNames != NULL) {
    free(symbolNames);
  }
  /* rhStackp contains only integers */
  GENERICSTACK_FREE(rhStackp);

  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] return %d", funcs, ruleIdi, symbolIdi, (int) rcb);
  return rcb;

}

/* For code clarity I removed the error checkings in the full traverser               */
/* This traverser always return a valuei that is an indice in traversep->outputStackp */
/* At this indice there is a stack of strings.                                        */
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
          childValueStackp = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, childValuei);

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
        int  i;

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
        char           *joinWs = " ";
        int             i;

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
  case NP:
    s = strdup("NP");
    break;
  case VP:
    s = strdup("VP");
    break;
  case period:
    s = strdup(".");
    break;
  case NN:
    s = strdup("NN");
    break;
  case NNS:
    s = strdup("NNS");
    break;
  case DT:
    s = strdup("DT");
    break;
  case CC:
    s = strdup("CC");
    break;
  case VBZ:
    s = strdup("VBZ");
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
  case S_RULE:
    s = strdup("S_RULE");
    break;
  case NP_RULE01:
  case NP_RULE02:
  case NP_RULE03:
  case NP_RULE04:
  case NP_RULE05:
    s = strdup("NP");
    break;
  case VP_RULE01:
  case VP_RULE02:
  case VP_RULE03:
  case VP_RULE04:
  case VP_RULE05:
    s = strdup("VP");
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

/********************************************************************************/
static short okSymbolCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = traverseContextp->genericLoggerp;
  char              *s                = NULL;
  short              rcb;
  char              *descs;

  descs = penn_tag_symbols(traverseContextp, symboli);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "okSymbolCallback description failure for symbol %d", symboli);
    goto err;
  }
  
  s = GENERICSTACK_GET_PTR(traverseContextp->inputStackp, argi);
  if (GENERICSTACK_ERROR(traverseContextp->inputStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[valueSymbolCallback] inputStackp get failure, %s", strerror(errno));
    goto err;
  }

  /* We reject the symbol VBZ if it is not 'eats' */
  if ((symboli == VBZ) && (strcmp(s, "eats") != 0)) {
    goto reject;
  }
  
  /* We accept any symbol */
  rcb = 1;
  goto done;

 reject:
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
static short okNullableCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli)
/********************************************************************************/
{
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp   = traverseContextp->genericLoggerp;
  char              *s                = NULL;
  short              rcb;
  char              *descs;

  descs = penn_tag_symbols(traverseContextp, symboli);
  if (descs == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "okSymbolCallback description failure for symbol %d", symboli);
    goto err;
  }
  
  /* We do not have any nullable in our grammar (!?) */
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
  
  /* We accept any rule */
  rcb = 1;
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

  /* Special case for the start rule: we do not enclose within (desc) */
  if (rulei == START_RULE) {
    strcpy(p, "");
  } else {
    strcpy(p, "(");
    strcat(p, descs);
  }
  for (i = arg0i; i <= argni; i++) {
    q = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, i);
    GENERICLOGGER_DEBUGF(genericLoggerp, "[rule No %d][%s] PTR at indice %d is %p %s", rulei, descs, i, q, q != NULL ? q : "(null)");
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
    if (strlen(p) > 0) {
      strcat(p, " ");
    }
    strcat(p, q);
  }
  if (rulei != START_RULE) {
    /* Append ")" */
    tmp = (char *) realloc(p, strlen(p) + 2);
    if (tmp == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "valueRuleCallback realloc failure, %s", strerror(errno));
      goto err;
    }
    p = tmp;
    strcat(p, ")");
  }
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
  GENERICLOGGER_DEBUGF(genericLoggerp, "[valueSymbolCallback] Symbol %s, return %d, input stack %d \"%s\" => output stack %d %s ", descs != NULL ? descs : "???", (int) rcb, argi, s != NULL ? s : "???", resulti, rcb ? p : "");
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

