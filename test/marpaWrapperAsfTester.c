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
static int pruning_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp);

enum { START = 0, S, NP, VP, period, NN, NNS, DT, CC, VBZ, MAX_SYMBOL };
enum { START_RULE = 0,
       S_RULE,
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
       MAX_RULE };

int main(int argc, char **argv) {
  marpaWrapperGrammar_t         *marpaWrapperGrammarp = NULL;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp = NULL;
  marpaWrapperAsf_t             *marpaWrapperAsfp = NULL;
  int                            symbolip[MAX_SYMBOL];
  int                            ruleip[MAX_RULE];
  int                            rci = 0;
  int                           *symbolArrayp = NULL;
  size_t                         i;
  size_t                         outputStackSizel;
  int                            valuei;
  traverseContext_t              traverseContext = { NULL, symbolip, ruleip, NULL, NULL, GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG) };
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */,
								  0 /* autorankb */
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* disableThresholdb */
  };
  marpaWrapperAsfOption_t        marpaWrapperAsfOption        = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_DEBUG),
								  0 /* highRankOnlyb */,
								  0 /* orderByRankb */,
								  1 /* ambiguousb */
  };

  GENERICSTACK_NEW(traverseContext.inputStackp);
  if (traverseContext.inputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }
  GENERICSTACK_NEW(traverseContext.outputStackp);
  if (traverseContext.outputStackp == NULL) {
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
  traverseContext.marpaWrapperAsfp = marpaWrapperAsfp;
  valuei = marpaWrapperAsf_traversei(marpaWrapperAsfp, pruning_traverserCallbacki, &traverseContext);

  GENERICLOGGER_INFOF(traverseContext.genericLoggerp, "Pruning traverser returns:\n%s", GENERICSTACK_GET_PTR(traverseContext.outputStackp, (size_t) valuei));
 
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
  /* But output stack is an arrat of strings */
  while (GENERICSTACK_USED(traverseContext.outputStackp) > 0) {
    free(GENERICSTACK_POP_PTR(traverseContext.outputStackp));
  }
  GENERICSTACK_FREE(traverseContext.outputStackp);
  
  return rci;
}

/********************************************************************************/
static int pruning_traverserCallbacki(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp)
/********************************************************************************/
{
  char               funcs[] = "pruning_traverserCallbacki";
  traverseContext_t *traverseContextp = (traverseContext_t *) userDatavp;
  genericLogger_t   *genericLoggerp = traverseContextp->genericLoggerp;
  char              *symbolNames = NULL;
  char              *ruleNames = NULL;
  genericStack_t    *rhStackp = NULL;
  int                ruleIdi;
  int                symbolIdi;
  int                rci = -1;
  size_t             stringl;
  char              *strings;

  /* This routine converts the glade into a list of Penn-tagged elements.  It is called recursively */

  ruleIdi = marpaWrapperAsf_traverse_ruleIdi(traverserp);
  symbolIdi = marpaWrapperAsf_traverse_symbolIdi(traverserp);
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s] => ruleIdi=%d, symbolIdi=%d", funcs, ruleIdi, symbolIdi);

  if (symbolIdi < 0) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...symbolIdi is < 0", funcs, ruleIdi, symbolIdi);
    goto err;
  }

  symbolNames = penn_tag_symbols(traverseContextp, symbolIdi);
  if (symbolNames == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...symbolNames is NULL", funcs, ruleIdi, symbolIdi);
    goto err;
  }

  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ...symbol %s", funcs, ruleIdi, symbolIdi, symbolNames);

  /* A token is a single choice, and we know enough to fully Penn-tag it */
  if (ruleIdi < 0) {
    int     spanIdi;
    size_t  indicel;
    char   *tokenValues;
    
    spanIdi = marpaWrapperAsf_traverse_rh_valuei(traverserp, 0);
    if (spanIdi < 0) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...spanIdi is %d < 0", funcs, ruleIdi, symbolIdi, spanIdi);
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... spanIdi=%d", funcs, ruleIdi, symbolIdi, spanIdi);

    /* The spanId correspond to the inputstack indice spanId+1 */
    indicel = spanIdi + 1;
    if (! GENERICSTACK_IS_PTR(traverseContextp->inputStackp, indicel)) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...Nothing at input stack indice %d", funcs, ruleIdi, symbolIdi, (int) indicel);
      goto err;
    }
    tokenValues = GENERICSTACK_GET_PTR(traverseContextp->inputStackp, indicel);
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Token is \"%s\"", funcs, ruleIdi, symbolIdi, tokenValues);
    /* We want to generate the string "(penntag literal)" */
    stringl = 1 + strlen(symbolNames) + 1 + strlen(tokenValues) + 1 + 1;
    strings = malloc(stringl);
    if (strings == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...malloc failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
      goto err;
    }
    sprintf(strings, "(%s %s)", symbolNames, tokenValues);
    
  } else {
    size_t  lengthl;
    size_t  rhIxi;
    size_t  indicel;
    
    ruleNames = penn_tag_rules(traverseContextp, ruleIdi);
    if (ruleNames == NULL) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...ruleNames is NULL", funcs, ruleIdi, symbolIdi);
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ...rule %s", funcs, ruleIdi, symbolIdi, ruleNames);

    lengthl = marpaWrapperAsf_traverse_rh_lengthl(traverserp);
    if (lengthl == (size_t) -1) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...lengthl is (size_t)-1", funcs, ruleIdi, symbolIdi);
      goto err;
    }
    GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... lengthl=%d", funcs, ruleIdi, symbolIdi, (int) lengthl);

    GENERICSTACK_NEW(rhStackp);
    if (GENERICSTACK_ERROR(rhStackp)) {
      GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...rhStackp initialization failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
      goto err;
    }
    for (rhIxi = 0; rhIxi <= lengthl - 1; rhIxi++) {
      int   valuei;
      char *values;

      valuei = marpaWrapperAsf_traverse_rh_valuei(traverserp, rhIxi);
      if (valuei < 0) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...valuei is %d < 0", funcs, ruleIdi, symbolIdi, valuei);
        goto err;
      }

      /* We expect valuei to be in outputStack */
      indicel = valuei;
      if (! GENERICSTACK_IS_PTR(traverseContextp->outputStackp, indicel)) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...Nothing at output stack indice %d", funcs, ruleIdi, symbolIdi, (int) indicel);
	goto err;
      }
      values = GENERICSTACK_GET_PTR(traverseContextp->outputStackp, indicel);
      GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Value is \"%s\" for rh No %d", funcs, ruleIdi, symbolIdi, values, rhIxi);

      GENERICSTACK_PUSH_INT(rhStackp, valuei);
      if (GENERICSTACK_ERROR(rhStackp)) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...rhStackp push failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
	goto err;
      }
    }

    /* Special case for the start rule */
    if (symbolIdi == START) {
      size_t  i;

      /* All rh values are concatenated with a space in between and a newline at the end */
      stringl = 0;
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	size_t rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  stringl++;
	}
	stringl += strlen((char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      stringl++; /* newline */
      strings = malloc(++stringl); /* null character */
      if (strings == NULL) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...malloc failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
	goto err;
      }

      strings[0] = '\0';
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	size_t rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  strcat(strings, " ");
	}
	strcat(strings, (char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      strcat(strings, "\n");
    } else {
      char  *joinWs = " ";
      size_t i;

      if (symbolIdi == S) {
	joinWs = "\n  ";
      }

      /* All rh values are concatenated with joinWs in between, the whole beeing enclosed in (penntag XXX) */
      stringl = 1 + strlen(symbolNames) + 1; /* "(penntag " */
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	size_t rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
	if (i > 0) {
	  stringl += strlen(joinWs);
	}
	stringl += strlen((char *) GENERICSTACK_GET_PTR(traverseContextp->outputStackp, rhValuei));
      }
      stringl += 1; /* ")" */
      strings = malloc(++stringl); /* null character */
      if (strings == NULL) {
	GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...malloc failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
	goto err;
      }
      strcpy(strings, "(");
      strcat(strings, symbolNames);
      strcat(strings, " ");
      for (i = 0; i < GENERICSTACK_USED(rhStackp); i++) {
	size_t rhValuei = GENERICSTACK_GET_INT(rhStackp, i);
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
    GENERICLOGGER_ERRORF(genericLoggerp, "[%s][%d:%d] ...traverseContextp->outputStackp push failure: %s", funcs, ruleIdi, symbolIdi, strerror(errno));
    goto err;
  }
  rci = (int) (GENERICSTACK_USED(traverseContextp->outputStackp) - 1);
  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] ... Returning \"%s\" pushed at indice %d of output stack", funcs, ruleIdi, symbolIdi, strings, rci);

  goto done;
  
 err:
  rci = -1;

 done:
  if (ruleNames != NULL) {
    free(ruleNames);
  }
  if (symbolNames != NULL) {
    free(symbolNames);
  }
  /* rhStackp contains only integers */
  GENERICSTACK_FREE(rhStackp);

  GENERICLOGGER_DEBUGF(genericLoggerp, "[%s][%d:%d] return %d", funcs, ruleIdi, symbolIdi, rci);
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
