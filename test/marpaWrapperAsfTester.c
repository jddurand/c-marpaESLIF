#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "marpaWrapper.h"
#include "genericLogger.h"
#include "genericStack.h"

static void *traverserCallback(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp);

enum { S = 0, NP, VP, period, NN, NNS, DT, CC, VBZ, MAX_SYMBOL };
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
       MAX_RULE };

typedef struct traverseContext {
  marpaWrapperAsf_t        *marpaWrapperAsfp;
  int                      *symbolip;
  int                      *ruleip;
  genericStack_t           *inputStackp;
  genericStack_t           *outputStackp;
  genericLogger_t          *genericLoggerp;
} traverseContext_t;

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
  void                          *valuep;
  traverseContext_t              traverseContext = { NULL, symbolip, ruleip, NULL, NULL, GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE) };
  
  marpaWrapperGrammarOption_t    marpaWrapperGrammarOption    = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  0 /* warningIsErrorb */,
								  0 /* warningIsIgnoredb */,
								  0 /* autorankb */
  };
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
								  0 /* disableThresholdb */
  };
  marpaWrapperAsfOption_t        marpaWrapperAsfOption        = { GENERICLOGGER_NEW(GENERICLOGGER_LOGLEVEL_TRACE),
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
  if ( /* S (start symbol automatically), E, op, number */
      ((symbolip[     S] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    NP] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    VP] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[period] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    NN] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[   NNS] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    DT] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    CC] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[   VBZ] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      ((symbolip[    VP] = MARPAWRAPPERGRAMMAR_NEWSYMBOL(marpaWrapperGrammarp)) < 0) ||
      /* S   ::= NP  VP  period */
      ((ruleip[S_RULE]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[S],
						       symbolip[NP], symbolip[VP], symbolip[period], -1)) < 0) ||
      /* NP  ::= NN  */
      ((ruleip[NP_RULE01] = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NN], -1)) < 0) ||
      /* NP  ::= NNS  */
      ((ruleip[NP_RULE02]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NNS], -1)) < 0) ||
      /* NP  ::= DT NN */
      ((ruleip[NP_RULE03]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[DT], symbolip[NN], -1)) < 0) ||
      /* NP  ::= NN NNS */
      ((ruleip[NP_RULE04]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NN], symbolip[NNS], -1)) < 0) ||
      /* NP  ::= NNS CC NNS */
      ((ruleip[NP_RULE05]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[NP],
						       symbolip[NNS], symbolip[CC], symbolip[NNS], -1)) < 0) ||
      /* VP  ::= VBZ NP */
      ((ruleip[VP_RULE01]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VBZ], symbolip[NP], -1)) < 0) ||
      /* VP  ::= VP VBZ NNS */
      ((ruleip[VP_RULE02]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VP], symbolip[VBZ], symbolip[NNS], -1)) < 0) ||
      /* VP  ::= VP CC VP */
      ((ruleip[VP_RULE03]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VP], symbolip[CC], symbolip[VP], -1)) < 0) ||
      /* VP  ::= VP VP CC VP */
      ((ruleip[VP_RULE04]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VP], symbolip[VP], symbolip[CC], symbolip[VP], -1)) < 0) ||
      /* VP  ::= VBZ */
      ((ruleip[VP_RULE05]   = MARPAWRAPPERGRAMMAR_NEWRULE(marpaWrapperGrammarp,
						       symbolip[VP],
						       symbolip[VBZ], -1)) < 0)
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
  valuep = marpaWrapperAsf_traversep(marpaWrapperAsfp, traverserCallback, &traverseContext);
 
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
  GENERICSTACK_FREE(traverseContext.outputStackp);
  
  return rci;
}

/********************************************************************************/
static void *traverserCallback(marpaWrapperAsfTraverser_t *traverserp, void *userDatavp)
/********************************************************************************/
{
  return NULL;
}

