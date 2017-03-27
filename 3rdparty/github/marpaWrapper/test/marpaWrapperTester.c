#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "marpaWrapper.h"
#include "genericLogger.h"
#include "genericStack.h"

enum { S = 0, E, op, number, MAX_SYMBOL };
enum { START_RULE = 0, OP_RULE, NUMBER_RULE, MAX_RULE };
static char *desc[] = { "S", "E", "op", "number", "?" };
typedef struct valueContext {
  marpaWrapperRecognizer_t *marpaWrapperRecognizerp;
  marpaWrapperValue_t      *marpaWrapperValuep;
  marpaWrapperAsfValue_t   *marpaWrapperAsfValuep;
  int                      *symbolip;
  int                      *ruleip;
  genericStack_t           *inputStackp;
  genericStack_t           *outputStackp;
  genericLogger_t          *genericLoggerp;
} valueContext_t;

static char *symbolDescription(void *userDatavp, int symboli);
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static void  dumpStacks(char *modes, valueContext_t *valueContextp);
static short grammarOptionSetter(void *userDatavp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp);
static short symbolOptionSetter(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);
static short ruleOptionSetter(void *userDatavp, int symboli, marpaWrapperGrammarRuleOption_t *marpaWrapperGrammarRuleOptionp);
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, int arg0i, int argni);
static short okSymbolCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi);

static marpaWrapperGrammarCloneOption_t marpaWrapperGrammarCloneOption = {
  NULL, /* userDatavp */
  grammarOptionSetter,
  symbolOptionSetter,
  ruleOptionSetter,
};

typedef struct stackValueAndDescription {
  int i;
  char s[100];
} stackValueAndDescription_t;

/****************************************************************************/
int main(int argc, char **argv)
/****************************************************************************/
{
  marpaWrapperGrammar_t         *marpaWrapperGrammarp = NULL;
  marpaWrapperGrammar_t         *marpaWrapperGrammarOriginalp = NULL;
  marpaWrapperRecognizer_t      *marpaWrapperRecognizerp = NULL;
  marpaWrapperValue_t           *marpaWrapperValuep = NULL;
  marpaWrapperAsfValue_t        *marpaWrapperAsfValuep = NULL;
  int                            symbolip[MAX_SYMBOL];
  int                            ruleip[MAX_RULE];
  int                            rci = 0;
  int                           *symbolArrayp = NULL;
  size_t                         neventl;
  size_t                         nsymboll;
  int                            i;
  int                            outputStackSizei;
  valueContext_t                 valueContext = { NULL, NULL, NULL, symbolip, ruleip, NULL, NULL, GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE) };
  int                            symbolPropertyBitSet;
  int                            rulePropertyBitSet;
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */,
								  0 /* autorankb */,
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  0, /* disableThresholdb */
								  0 /* exhaustion */
  };
  marpaWrapperValueOption_t      marpaWrapperValueOption      = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  1 /* highRankOnlyb */,
								  1 /* orderByRankb */,
								  1 /* ambiguousb - We KNOW this test is ambiguous */,
								  0 /* nullb */,
								  0 /* maxParsesi */
  };
  marpaWrapperAsfOption_t        marpaWrapperAsfOption        = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  1 /* highRankOnlyb */,
								  1 /* orderByRankb */,
								  1 /* ambiguousb - We KNOW this test is ambiguous */,
								  0 /* maxParsesi */
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
    /* We will work on a cloned grammar */
    genericLogger_t *tmpLoggerp = GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE);
    marpaWrapperGrammarCloneOption.userDatavp = (void *) tmpLoggerp;
    marpaWrapperGrammar_t *marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(marpaWrapperGrammarp, &marpaWrapperGrammarCloneOption);
    GENERICLOGGER_FREE(tmpLoggerp);
    if (marpaWrapperGrammarClonep == NULL) {
      rci = 1;
    } else {
      marpaWrapperGrammarOriginalp = marpaWrapperGrammarp;
      marpaWrapperGrammarp         = marpaWrapperGrammarClonep;
      if (marpaWrapperGrammar_precomputeb(marpaWrapperGrammarp) == 0) {
	rci = 1;
      }
    }
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_eventb(marpaWrapperGrammarp, &neventl, NULL, 0 /* exhaustionEventb */, 0) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_symbolPropertyb(marpaWrapperGrammarp, symbolip[S], &symbolPropertyBitSet) == 0) {
      if ((symbolPropertyBitSet & MARPAWRAPPER_SYMBOL_IS_START) != MARPAWRAPPER_SYMBOL_IS_START) {
        perror("symbolip[S] does not have the MARPAWRAPPER_SYMBOL_IS_START bit set");
        rci = 1;
      }
    }
  }
  if (rci == 0) {
    if (marpaWrapperGrammar_rulePropertyb(marpaWrapperGrammarp, ruleip[NUMBER_RULE], &rulePropertyBitSet) == 0) {
      if ((rulePropertyBitSet & MARPAWRAPPER_RULE_IS_PRODUCTIVE) != MARPAWRAPPER_RULE_IS_PRODUCTIVE) {
        perror("ruleip[NUMBER_RULE] does not have the MARPAWRAPPER_RULE_IS_PRODUCTIVE bit set");
        rci = 1;
      }
    }
  }
  if (rci == 0) {
    marpaWrapperRecognizerp = marpaWrapperRecognizer_newp(marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
    if (marpaWrapperRecognizerp == NULL) {
      rci = 1;
    }
  }

  if (rci == 0) {
    valueContext.marpaWrapperRecognizerp = marpaWrapperRecognizerp;
    /* We use the stack indices directly into Marpa, and Marpa does not like indice 0 which means "unvalued". */
    /* Nevertheless, during the value phase, indice 0 will be used                                            */
    GENERICSTACK_PUSH_PTR(valueContext.inputStackp, NULL);
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }

  /* --------------------------------------------------------------- */
  /* 2 - 0 * 3 + 1                                                   */
  /* --------------------------------------------------------------- */
  
  if (rci == 0) {
    if (marpaWrapperRecognizer_progressLogb(marpaWrapperRecognizerp, 0, -1, GENERICLOGGER_LOGLEVEL_INFO, NULL, symbolDescription) == 0) {
      rci = 1;
    }
  }

  /* ------ */
  /* number */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_INT(valueContext.inputStackp, 2);
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }
  /* -- */
  /* op */
  /* -- */
  if (rci == 0) {
    GENERICSTACK_PUSH_CHAR(valueContext.inputStackp, '-');
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_INT(valueContext.inputStackp, 0);
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }
  /* -- */
  /* op */
  /* -- */
  if (rci == 0) {
    GENERICSTACK_PUSH_CHAR(valueContext.inputStackp, '*');
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_INT(valueContext.inputStackp, 3);
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }
  /* -- */
  /* op */
  /* -- */
  if (rci == 0) {
    GENERICSTACK_PUSH_CHAR(valueContext.inputStackp, '+');
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[op], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }
  /* ------ */
  /* number */
  /* ------ */
  if (rci == 0) {
    GENERICSTACK_PUSH_INT(valueContext.inputStackp, 1);
    if (GENERICSTACK_ERROR(valueContext.inputStackp)) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_readb(marpaWrapperRecognizerp, symbolip[number], GENERICSTACK_USED(valueContext.inputStackp) - 1 /* value */, 1 /* length */) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    int earleySetIdi;
    if (marpaWrapperRecognizer_latestb(marpaWrapperRecognizerp, &earleySetIdi) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Latest Earleme Set ID: %d", earleySetIdi);
    }
  }

  if (rci == 0) {
    if (marpaWrapperRecognizer_event_onoffb(marpaWrapperRecognizerp, symbolip[S], MARPAWRAPPERGRAMMAR_EVENTTYPE_PREDICTION, 0) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_exhaustedb(marpaWrapperRecognizerp, NULL) == 0) {
      rci = 1;
    }
  }
  if (rci == 0) {
    if (marpaWrapperRecognizer_expectedb(marpaWrapperRecognizerp, &nsymboll, &symbolArrayp) == 0) {
      rci = 1;
    } else {
      GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "Number of expected symbols: %ld", (unsigned long) nsymboll);
      if (nsymboll > 0) {
	for (i = 0; i < (int) nsymboll; i++) {
	  switch (symbolArrayp[i]) {
	  case S:
	    GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: S", i);
	    break;
	  case E:
	    GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: E", i);
	    break;
	  case op:
	    GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: op", i);
	    break;
	  case number:
	    GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: number", i);
	    break;
	  default:
	    GENERICLOGGER_TRACEF(marpaWrapperRecognizerOption.genericLoggerp, "... Expected symbol No %d: ????", i);
	    break;
	  }
	}
      }
    }
  }

  /* Using normal valuation */
  if (rci == 0) {
    marpaWrapperValuep = marpaWrapperValue_newp(marpaWrapperRecognizerp, &marpaWrapperValueOption);
    if (marpaWrapperValuep == NULL) {
      rci = 1;
    }
  }

  if (rci == 0) {
    valueContext.marpaWrapperValuep    = marpaWrapperValuep;
    valueContext.marpaWrapperAsfValuep = NULL;
    while (marpaWrapperValue_valueb(marpaWrapperValuep,
				    (void *) &valueContext,
				    valueRuleCallback,
				    valueSymbolCallback,
				    NULL) > 0) {
      stackValueAndDescription_t *resultp = GENERICSTACK_GET_PTR(valueContext.outputStackp, 0);
      GENERICLOGGER_INFOF(valueContext.genericLoggerp, "[Value mode] %s => %d", resultp->s, resultp->i);
    }
  }

  if (marpaWrapperValuep != NULL) {
    marpaWrapperValue_freev(marpaWrapperValuep);
  }
  GENERICLOGGER_FREE(marpaWrapperValueOption.genericLoggerp);

  /* Using ASF valuation */
  if (rci == 0) {
    marpaWrapperAsfValuep = marpaWrapperAsfValue_newp(marpaWrapperRecognizerp, &marpaWrapperAsfOption);
    if (marpaWrapperAsfValuep == NULL) {
      rci = 1;
    }
  }

  if (rci == 0) {
    valueContext.marpaWrapperValuep    = NULL;
    valueContext.marpaWrapperAsfValuep = marpaWrapperAsfValuep;
    while (marpaWrapperAsfValue_valueb(marpaWrapperAsfValuep,
                                       (void *) &valueContext,
                                       okRuleCallback,
                                       okSymbolCallback,
                                       NULL, /* okNullingCallbackp */
                                       valueRuleCallback,
                                       valueSymbolCallback,
                                       NULL /* nullingCallbackp */
                                       ) > 0) {
      stackValueAndDescription_t *resultp = GENERICSTACK_GET_PTR(valueContext.outputStackp, 0);
      GENERICLOGGER_INFOF(valueContext.genericLoggerp, "[Asf value mode] %s => %d", resultp->s, resultp->i);
    }
  }

  if (marpaWrapperAsfValuep != NULL) {
    marpaWrapperAsfValue_freev(marpaWrapperAsfValuep);
  }
  GENERICLOGGER_FREE(marpaWrapperAsfOption.genericLoggerp);

  if (marpaWrapperRecognizerp != NULL) {
    marpaWrapperRecognizer_freev(marpaWrapperRecognizerp);
  }
  GENERICLOGGER_FREE(marpaWrapperRecognizerOption.genericLoggerp);

  if (marpaWrapperGrammarp != NULL) {
    marpaWrapperGrammar_freev(marpaWrapperGrammarp);
  }
  if (marpaWrapperGrammarOriginalp != NULL) {
    marpaWrapperGrammar_freev(marpaWrapperGrammarOriginalp);
  }
  GENERICLOGGER_FREE(marpaWrapperGrammarOption.genericLoggerp);

  GENERICLOGGER_FREE(valueContext.genericLoggerp);

  /* Input stack has no inner PTR */
  GENERICSTACK_FREE(valueContext.inputStackp);

  /* Output stack have somme inner PTR */
  outputStackSizei = GENERICSTACK_USED(valueContext.outputStackp);
  if (outputStackSizei > 0) {
    for (i = 0; i < outputStackSizei; i++) {
      if (GENERICSTACK_IS_PTR(valueContext.outputStackp, i)) {
	free(GENERICSTACK_GET_PTR(valueContext.outputStackp, i));
      }
    }
  }
  GENERICSTACK_FREE(valueContext.outputStackp);
  
  return rci;
}

/* One have to know that symbol and rule indices always start at 0 and increase by 1   */
/* so, even if the "safe" way would be to loop on the int array, it is by construction */
/* as safe to just check if the indice is below the maximum.                           */

/****************************************************************************/
static char *symbolDescription(void *userDatavp, int symboli)
/****************************************************************************/
{
  if (symboli < MAX_SYMBOL) {
    return desc[symboli];
  }
  
  return NULL;
}

/****************************************************************************/
static short valueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/****************************************************************************/
{
  static const char           funcs[] = "valueRuleCallback";
  valueContext_t             *valueContextp           = (valueContext_t *) userDatavp;
  marpaWrapperRecognizer_t   *marpaWrapperRecognizerp = valueContextp->marpaWrapperRecognizerp;
  marpaWrapperValue_t        *marpaWrapperValuep      = valueContextp->marpaWrapperValuep;
  marpaWrapperAsfValue_t     *marpaWrapperAsfValuep   = valueContextp->marpaWrapperAsfValuep;
  genericStack_t             *outputStackp            = valueContextp->outputStackp;
  genericLogger_t            *genericLoggerp          = valueContextp->genericLoggerp;
  int                        *ruleip                  = valueContextp->ruleip;
  stackValueAndDescription_t *resultp                 = NULL;
  char                       *modes                   = marpaWrapperValuep ? "Value mode" : "Asf value mode";
  short                       rcb;
  int                         starti;
  int                         lengthi;

  if (marpaWrapperValuep != NULL) {
    if (! marpaWrapperValue_value_startb(marpaWrapperValuep, &starti)) {
      goto err;
    }
    if (! marpaWrapperValue_value_lengthb(marpaWrapperValuep, &lengthi)) {
      goto err;
    }
  } else {
    if (! marpaWrapperAsfValue_value_startb(marpaWrapperAsfValuep, &starti)) {
      goto err;
    }
    if (! marpaWrapperAsfValue_value_lengthb(marpaWrapperAsfValuep, &lengthi)) {
      goto err;
    }
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] Earleme Set Id start/end: %d/%d", modes, funcs, starti, lengthi);
  GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] Stacks before:", modes, funcs);
  dumpStacks(modes, valueContextp);

  if (rulei >= MAX_RULE) {
    goto err;
  }

  resultp = malloc(sizeof(stackValueAndDescription_t));
  if (resultp == NULL) {
    GENERICLOGGER_ERRORF(genericLoggerp, "malloc error, %s", strerror(errno));
    goto err;
  }

  switch (rulei) {
  case START_RULE:
    {
      stackValueAndDescription_t *varp = GENERICSTACK_GET_PTR(outputStackp, arg0i);

      *resultp = *varp;
      GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] START_RULE: {s=%s,i=%d} at output stack No %d -> {s=%s,i=%d} at output stack No %d", modes, funcs, varp->s, varp->i, arg0i, resultp->s, resultp->i, resulti);
    }
    break;
  case OP_RULE:
    {
      stackValueAndDescription_t *var1p = GENERICSTACK_GET_PTR(outputStackp,  arg0i  );
      char                        var2c = GENERICSTACK_GET_CHAR(outputStackp, arg0i+1);
      stackValueAndDescription_t *var3p = GENERICSTACK_GET_PTR(outputStackp,  arg0i+2);

      sprintf(resultp->s, "(%s %c %s)", var1p->s, var2c, var3p->s);
      switch (var2c) {
      case '+':
	resultp->i = var1p->i + var3p->i;
	break;
      case '-':
	resultp->i = var1p->i - var3p->i;
	break;
      case '*':
	resultp->i = var1p->i * var3p->i;
	break;
      case '/':
	resultp->i = var1p->i / var3p->i;
	break;
      case '%':
	resultp->i = var1p->i % var3p->i;
	break;
      default:
	GENERICLOGGER_ERRORF(genericLoggerp, "op %c !?", var2c);
	goto err;
      }
      GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] OP_RULE: {s=%s,i=%d} %c {s=%s,i=%i} at output stack [%d-%d] -> {s=%s,i=%d} at output stack No %d", modes, funcs, var1p->s, var1p->i, var2c, var3p->s, var3p->i, arg0i, argni, resultp->s, resultp->i, resulti);
    }
    break;
  case NUMBER_RULE:
    {
      stackValueAndDescription_t *varp = GENERICSTACK_GET_PTR(outputStackp, arg0i);

      *resultp = *varp;
      GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] START_RULE: {s=%s,i=%d} at output stack No %d -> {s=%s,i=%d} at output stack No %d", modes, funcs, varp->s, varp->i, arg0i, resultp->s, resultp->i, resulti);
    }
    break;
  default:
    GENERICLOGGER_ERRORF(genericLoggerp, "Rule %d !?", rulei);
    goto err;
  }

  if (GENERICSTACK_IS_PTR(outputStackp, resulti)) {
    free(GENERICSTACK_GET_PTR(outputStackp, resulti));
    GENERICSTACK_SET_NA(outputStackp, resulti);
  }
  GENERICSTACK_SET_PTR(outputStackp, resultp, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    GENERICLOGGER_ERRORF(genericLoggerp, "generic stack error, %s", strerror(errno));
    goto err;
  }
  rcb = 1;
  goto done;

 err:
  if (resultp != NULL) {
    free(resultp);
  }
  rcb = 0;

 done:
  GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] Stacks after:", modes, funcs);
  dumpStacks(modes, valueContextp);
  return rcb;
}

/****************************************************************************/
static short valueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/****************************************************************************/
{
  static const char           funcs[]                 = "valueSymbolCallback";
  valueContext_t             *valueContextp           = (valueContext_t *) userDatavp;
  marpaWrapperRecognizer_t   *marpaWrapperRecognizerp = valueContextp->marpaWrapperRecognizerp;
  marpaWrapperValue_t        *marpaWrapperValuep      = valueContextp->marpaWrapperValuep;
  marpaWrapperAsfValue_t     *marpaWrapperAsfValuep   = valueContextp->marpaWrapperAsfValuep;
  genericStack_t             *inputStackp             = valueContextp->inputStackp;
  genericStack_t             *outputStackp            = valueContextp->outputStackp;
  genericLogger_t            *genericLoggerp          = valueContextp->genericLoggerp;
  int                        *symbolip                = valueContextp->symbolip;
  stackValueAndDescription_t *resultp                 = NULL;
  char                       *modes                   = marpaWrapperValuep ? "Value mode" : "Asf value mode";
  short                       rcb;
  int                         starti;
  int                         lengthi;

  if (marpaWrapperValuep != NULL) {
    if (! marpaWrapperValue_value_startb(marpaWrapperValuep, &starti)) {
      goto err;
    }
    if (! marpaWrapperValue_value_lengthb(marpaWrapperValuep, &lengthi)) {
      goto err;
    }
  } else {
    if (! marpaWrapperAsfValue_value_startb(marpaWrapperAsfValuep, &starti)) {
      goto err;
    }
    if (! marpaWrapperAsfValue_value_lengthb(marpaWrapperAsfValuep, &lengthi)) {
      goto err;
    }
  }

  GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] Earleme Set Id start/end: %d/%d", modes, funcs, starti, lengthi);
  GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] Stacks before:", modes, funcs);
  dumpStacks(modes, valueContextp);

  if (symboli >= MAX_SYMBOL) {
    goto err;
  }

  switch (symboli) {
  case op:
    {
      char varc = GENERICSTACK_GET_CHAR(inputStackp, argi);
      GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] op: operator '0x%x' at input stack No %d -> output stack No %d", modes, funcs, varc, argi, resulti);

      if (GENERICSTACK_IS_PTR(outputStackp, resulti)) {
	free(GENERICSTACK_GET_PTR(outputStackp, resulti));
	GENERICSTACK_SET_NA(outputStackp, resulti);
      }

      GENERICSTACK_SET_CHAR(outputStackp, varc, resulti);
      if (GENERICSTACK_ERROR(outputStackp)) {
	GENERICLOGGER_ERRORF(genericLoggerp, "generic stack error, %s", strerror(errno));
      }
    }
    break;
  case number:
    {
      int vari = GENERICSTACK_GET_INT(inputStackp, argi);

      resultp = malloc(sizeof(stackValueAndDescription_t));
      if (resultp == NULL) {
	GENERICLOGGER_ERRORF(genericLoggerp, "malloc error, %s", strerror(errno));
	goto err;
      }

      resultp->i = vari;
      sprintf(resultp->s, "%d", vari);
      GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] number: value %d at input stack No %d -> {s=%s,i=%d} at output stack No %d", modes, funcs, vari, argi, resultp->s, resultp->i, resulti);

      if (GENERICSTACK_IS_PTR(outputStackp, resulti)) {
	free(GENERICSTACK_GET_PTR(outputStackp, resulti));
	GENERICSTACK_SET_NA(outputStackp, resulti);
      }

      GENERICSTACK_SET_PTR(outputStackp, resultp, resulti);
      if (GENERICSTACK_ERROR(outputStackp)) {
	GENERICLOGGER_ERRORF(genericLoggerp, "generic stack error, %s", strerror(errno));
      }
    }
    break;
  default:
    GENERICLOGGER_ERRORF(genericLoggerp, "Symbol %d !?", symboli);
    return 0;
  }

  rcb = 1;
  goto done;

 err:
  if (resultp != NULL) {
    free(resultp);
  }
  rcb = 0;

 done:
  GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] Stacks after:", modes, funcs);
  dumpStacks(modes, valueContextp);
  return rcb;
}

/****************************************************************************/
static void dumpStacks(char *modes, valueContext_t *valueContextp)
/****************************************************************************/
{
  static const char           funcs[] = "dumpStacks";
  genericStack_t             *inputStackp     = valueContextp->inputStackp;
  genericStack_t             *outputStackp    = valueContextp->outputStackp;
  genericLogger_t            *genericLoggerp  = valueContextp->genericLoggerp;
  int                         inputStackSizei = GENERICSTACK_USED(inputStackp);
  int                         outputStackSizei = GENERICSTACK_USED(outputStackp);
  int                         i;

  if (inputStackSizei > 0) {
    for (i = 0; i < inputStackSizei; i++) {
      if (GENERICSTACK_IS_NA(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is NA", modes, funcs, i);
      } else if (GENERICSTACK_IS_CHAR(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is CHAR", modes, funcs, i);
      } else if (GENERICSTACK_IS_SHORT(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is SHORT", modes, funcs, i);
      } else if (GENERICSTACK_IS_INT(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is INT", modes, funcs, i);
      } else if (GENERICSTACK_IS_LONG(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is LONG", modes, funcs, i);
      } else if (GENERICSTACK_IS_FLOAT(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is FLOAT", modes, funcs, i);
      } else if (GENERICSTACK_IS_DOUBLE(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is DOUBLE", modes, funcs, i);
      } else if (GENERICSTACK_IS_PTR(inputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is PTR", modes, funcs, i);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... input stack No %d is ?????", modes, funcs, i);
      }
    }
  }

  if (outputStackSizei > 0) {
    for (i = 0; i < outputStackSizei; i++) {
      if (GENERICSTACK_IS_NA(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is NA", modes, funcs, i);
      } else if (GENERICSTACK_IS_CHAR(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is CHAR", modes, funcs, i);
      } else if (GENERICSTACK_IS_SHORT(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is SHORT", modes, funcs, i);
      } else if (GENERICSTACK_IS_INT(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is INT", modes, funcs, i);
      } else if (GENERICSTACK_IS_LONG(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is LONG", modes, funcs, i);
      } else if (GENERICSTACK_IS_FLOAT(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is FLOAT", modes, funcs, i);
      } else if (GENERICSTACK_IS_DOUBLE(outputStackp, i)) {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is DOUBLE", modes, funcs, i);
      } else if (GENERICSTACK_IS_PTR(outputStackp, i)) {
	stackValueAndDescription_t *p = GENERICSTACK_GET_PTR(outputStackp, i);
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is PTR {\"%s\", %d}", modes, funcs, i, p->s, p->i);
      } else {
	GENERICLOGGER_TRACEF(genericLoggerp, "[%s][%s] ... output stack No %d is ?????", modes, funcs, i);
      }
    }
  }
}

/****************************************************************************/
static short grammarOptionSetter(void *userDatavp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp)
/****************************************************************************/
{
  genericLogger_t *genericLoggerp = (genericLogger_t *) userDatavp;
  GENERICLOGGER_TRACE(genericLoggerp, "grammarOptionSetter clone callback");
  return 1;
}

/****************************************************************************/
static short symbolOptionSetter(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/****************************************************************************/
{
  genericLogger_t *genericLoggerp = (genericLogger_t *) userDatavp;
  GENERICLOGGER_TRACEF(genericLoggerp, "symbolOptionSetter clone callback for symboli %d", symboli);
  return 1;
}

/****************************************************************************/
static short ruleOptionSetter(void *userDatavp, int rulei, marpaWrapperGrammarRuleOption_t *marpaWrapperGrammarRuleOptionp)
/****************************************************************************/
{
  genericLogger_t *genericLoggerp = (genericLogger_t *) userDatavp;
  GENERICLOGGER_TRACEF(genericLoggerp, "ruleOptionSetter clone callback for rulei %d", rulei);
  return 1;
}

/****************************************************************************/
static short okRuleCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int rulei, int arg0i, int argni)
/****************************************************************************/
{
  return 1;
}

/****************************************************************************/
static short okSymbolCallback(void *userDatavp, genericStack_t *parentRuleiStackp, int symboli, int argi)
/****************************************************************************/
{
  return 1;
}
