#include <stdlib.h>
#include <stdio.h>
#include "marpaWrapper.h"
#include "genericLogger.h"
#include "genericStack.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE, MAX_RULE };
static char *desc[] = { "S", "E", "op", "number", "?" };
typedef struct valueContext {
  int             *symbolip;
  int             *ruleip;
  genericStack_t  *inputStackp;
  genericStack_t  *outputStackp;
  genericLogger_t *genericLoggerp;
} valueContext_t;

static char *symbolDescription(void *userDatavp, int symboli);
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);

typedef struct stackValueAndDescription {
  int i;
  char s[100];
} stackValueAndDescription_t;

static void *stackValueAndDescriptionClone(void *p);
static void stackValueAndDescriptionFree(void *p);

int main(int argc, char **argv) {
  marpaWrapperGrammar_t         *marpaWrapperGrammarp;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp;
  marpaWrapperValue_t           *marpaWrapperValuep;
  int                            symbolip[MAX_SYMBOL];
  int                            ruleip[MAX_RULE];
  int                            rci = 0;
  int                           *symbolArrayp = NULL;
  size_t                         neventl;
  size_t                         nsymboll;
  size_t                         i;
  valueContext_t                 valueContext = { symbolip, ruleip, NULL, NULL, GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE) };
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  1 /* latm */
  };
  marpaWrapperValueOption_t      marpaWrapperValueOption      = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  1 /* highRankOnlyb */,
								  1 /* orderByRankb */,
								  1 /* ambiguousb */,
								  0 /* nullb */
  };

  GENERICSTACK_NEW(valueContext.inputStackp);
  if (valueContext.inputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }
  GENERICSTACK_NEW(valueContext.outputStackp);
  if (valueContext.outputStackp == NULL) {
    perror("GENERICSTACK_NEW");
    exit(1);
  }

  marpaWrapperGrammarp = marpaWrapperGrammar_newp(&marpaWrapperGrammarOption);
  if ( /* S (start symbol automatically), E, op, number */
      ((symbolip[     S] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[     E] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    op] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[number] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      /* S ::= E      */
      /* E ::= E op E */
      /* E ::= number */
      ((ruleip[ START_RULE] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[S], symbolip[E],                            -1)) < 0) ||
      ((ruleip[    OP_RULE] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[E], symbolip[E], symbolip[op], symbolip[E], -1)) < 0) ||
      ((ruleip[NUMBER_RULE] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp, symbolip[E], symbolip[number],                       -1)) < 0)
      ) {
    rci = 1;
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_precomputeb(marpaWrapperGrammarp) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_eventb(marpaWrapperGrammarp, &neventl, NULL, 0) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
    if (marpaWrapperRecognizerp == NULL) {
      rci = 1;
    }
  }

  /* We use the stack indices directly into Marpa, and Marpa does not like indice 0 which means "unvalued". */
  /* Nevertheless, during the value phase, indice 0 will be used                                            */
  GENERICSTACK_PUSH_PTR(valueContext.inputStackp, NULL);

  /* --------------------------------------------------------------- */
  /* 2 - 0 * 3 + 1                                                   */
  /* --------------------------------------------------------------- */
  
  if (rci == 0) {
    if (marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, -1, 1, GENERICLOGGER_LOGLEVEL_INFO, (void *) symbolip, symbolDescription) == 0) {
      rci = 1;
    }
  }

  /* ------ */
  /* number */
  /* ------ */
  GENERICSTACK_PUSH_INT(valueContext.inputStackp, 2);
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  /* -- */
  /* op */
  /* -- */
  GENERICSTACK_PUSH_CHAR(valueContext.inputStackp, '-');
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  GENERICSTACK_PUSH_INT(valueContext.inputStackp, 0);
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  /* -- */
  /* op */
  /* -- */
  GENERICSTACK_PUSH_CHAR(valueContext.inputStackp, '*');
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  GENERICSTACK_PUSH_INT(valueContext.inputStackp, 3);
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  /* -- */
  /* op */
  /* -- */
  GENERICSTACK_PUSH_CHAR(valueContext.inputStackp, '+');
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  GENERICSTACK_PUSH_INT(valueContext.inputStackp, 1);
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_SIZE(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }

  if (rci == 0) {
    if (marpaWrapperRecognizer_event_onoffb(marpaWrapperRecognizerp, symbolip[S], MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION, 0) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nsymboll, &symbolArrayp) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "Number of expected symbols: %ld", (unsigned long) nsymboll);
      if (nsymboll > 0) {
	for (i = 0; i < nsymboll; i++) {
	  GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: %d", i, symbolArrayp[i]);
	}
      }
    }
  }

  if (rci == 0) {
    marpaWrapperValuep = marpaWrapperValue_newp(marpaWrapperRecognizerp, &marpaWrapperValueOption);
    if (marpaWrapperValuep == NULL) {
      rci = 1;
    }
  }

  if (rci == 0) {
    while (marpaWrapperValue_valueb(marpaWrapperValuep,
				    (void *) &valueContext,
				    valueRuleCallback,
				    valueSymbolCallback,
				    NULL) > 0) {
      stackValueAndDescription_t *resultp = GENERICSTACK_GET_ANY(valueContext.outputStackp, 0);
      GENERICLOGGER_INFOF(valueContext.genericLoggerp, "%s => %d", resultp->s, resultp->i);
    }
  }

  marpaWrapperValue_freev(marpaWrapperValuep);
  GENERICLOGGER_FREE(marpaWrapperValueOption.genericLoggerp);

  marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  GENERICLOGGER_FREE(marpaWrapperRecognizerOption.genericLoggerp);

  marpaWrapperGrammar_freev(marpaWrapperGrammarp);
  GENERICLOGGER_FREE(marpaWrapperGrammarOption.genericLoggerp);

  GENERICLOGGER_FREE(valueContext.genericLoggerp);
  GENERICSTACK_FREE(valueContext.inputStackp);
  GENERICSTACK_FREE(valueContext.outputStackp);
  
  return rci;
}

/* One have to know that swmbol and rule indices always start at 0 and increase by 1   */
/* so, even if the "safe" way would be to loop on the int array, it is by construction */
/* as safe to just check if the indice is below the maximum.                           */

static char *symbolDescription(void *userDatavp, int symboli)
{
  int *symbolip = (int *) userDatavp;

  if (symboli < MAX_SYMBOL) {
    return desc[symboli];
  }
  
  return NULL;
}

static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti) {
  valueContext_t             *valueContextp   = (valueContext_t *) userDatavp;
  genericStack_t             *genericStackp   = valueContextp->outputStackp;
  genericLogger_t            *genericLoggerp = valueContextp->genericLoggerp;
  int                        *ruleip          = valueContextp->ruleip;
  stackValueAndDescription_t  result;

  if (rulei >= MAX_RULE) {
    return 0;
  }

  switch (rulei) {
  case START_RULE:
    {
      stackValueAndDescription_t *varp = GENERICSTACK_GET_ANY(genericStackp, arg0i);

      result = *varp;
      GENERICLOGGER_TRACEF(genericLoggerp, "START_RULE: {s=%s,i=%d} at output stack No %d -> {s=%s,i=%d} at output stack No %d", varp->s, varp->i, arg0i, result.s, result.i, resulti);
      GENERICSTACK_SET_ANY(genericStackp, &result, stackValueAndDescriptionClone, stackValueAndDescriptionFree, resulti);
    }
    break;
  case OP_RULE:
    {
      stackValueAndDescription_t *var1p = GENERICSTACK_GET_ANY(genericStackp,  arg0i  );
      char                        var2c = GENERICSTACK_GET_CHAR(genericStackp, arg0i+1);
      stackValueAndDescription_t *var3p = GENERICSTACK_GET_ANY(genericStackp,  arg0i+2);

      sprintf(result.s, "(%s %c %s)", var1p->s, var2c, var3p->s);
      switch (var2c) {
      case '+':
	result.i = var1p->i + var3p->i;
	break;
      case '-':
	result.i = var1p->i - var3p->i;
	break;
      case '*':
	result.i = var1p->i * var3p->i;
	break;
      case '/':
	result.i = var1p->i / var3p->i;
	break;
      case '%':
	result.i = var1p->i % var3p->i;
	break;
      default:
	GENERICLOGGER_ERRORF(genericLoggerp, "op %c !?", var2c);
	return 0;
      }
      GENERICLOGGER_TRACEF(genericLoggerp, "OP_RULE: {s=%s,i=%d} %c {s=%s,i=%i} at output stack [%d-%d] -> {s=%s,i=%d} at output stack No %d", var1p->s, var1p->i, var2c, var3p->s, var3p->i, arg0i, argni, result.s, result.i, resulti);
      GENERICSTACK_SET_ANY(genericStackp, &result, stackValueAndDescriptionClone, stackValueAndDescriptionFree, resulti);
    }
    break;
  case NUMBER_RULE:
    {
      stackValueAndDescription_t *varp = GENERICSTACK_GET_ANY(genericStackp, arg0i);

      result = *varp;
      GENERICLOGGER_TRACEF(genericLoggerp, "START_RULE: {s=%s,i=%d} at output stack No %d -> {s=%s,i=%d} at output stack No %d", varp->s, varp->i, arg0i, result.s, result.i, resulti);
      GENERICSTACK_SET_ANY(genericStackp, &result, stackValueAndDescriptionClone, stackValueAndDescriptionFree, resulti);
    }
    break;
  default:
    GENERICLOGGER_ERRORF(genericLoggerp, "Rule %d !?", rulei);
    return 0;
  }

  return 1;
}

static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti) {
  valueContext_t             *valueContextp   = (valueContext_t *) userDatavp;
  genericStack_t             *inputStackp   = valueContextp->inputStackp;
  genericStack_t             *outputStackp   = valueContextp->outputStackp;
  genericLogger_t            *genericLoggerp = valueContextp->genericLoggerp;
  int                        *symbolip        = valueContextp->symbolip;
  stackValueAndDescription_t  result;

  if (symboli >= MAX_SYMBOL) {
    return 0;
  }

  switch (symboli) {
  case op:
    {
      char varc = GENERICSTACK_GET_CHAR(inputStackp, argi);
      GENERICLOGGER_TRACEF(genericLoggerp, "op: '%c' at input stack No %d -> output stack No %d", varc, argi, resulti);
      GENERICSTACK_SET_CHAR(outputStackp, varc, resulti);
    }
    break;
  case number:
    {
      int vari = GENERICSTACK_GET_INT(inputStackp, argi);

      result.i = vari;
      sprintf(result.s, "%d", vari);
      GENERICLOGGER_TRACEF(genericLoggerp, "number: %d at input stack No %d -> {s=%s,i=%d} at output stack No %d", vari, argi, result.s, result.i, resulti);
      GENERICSTACK_SET_ANY(outputStackp, &result, stackValueAndDescriptionClone, stackValueAndDescriptionFree, resulti);
    }
    break;
  default:
    GENERICLOGGER_ERRORF(genericLoggerp, "Symbol %d !?", symboli);
    return 0;
  }

  return 1;
}

static void *stackValueAndDescriptionClone(void *p) {
  stackValueAndDescription_t *p1 = (stackValueAndDescription_t *) p;
  stackValueAndDescription_t *p2 = malloc(sizeof(stackValueAndDescription_t));

  *p2 = *p1;

  return p2;
}

static void stackValueAndDescriptionFree(void *p) {
  stackValueAndDescription_t *p1 = (stackValueAndDescription_t *) p;

  free(p1);
}

