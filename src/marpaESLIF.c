#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include "marpaESLIF/internal/grammar.h"
#include "marpaESLIF/internal/logging.h"

/* ESLIF is a grammar, plus the options */
struct marpaESLIF {
  marpaESLIF_grammar_t *grammarp;
  marpaESLIFOption_t    option;
};

const char *bootstrap_grammar_descs = "Internal Grammar";

static void                  _marpaESLIF_free_grammarv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp);
static void                  _marpaESLIF_free_ruleStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp);
static void                  _marpaESLIF_free_rulev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep);
static void                  _marpaESLIF_free_symbolv(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp);
static void                  _marpaESLIF_free_symbolStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp);
static void                  _marpaESLIF_free_terminalv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);
static short                 _marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp);
static marpaESLIF_grammar_t *_marpaESLIF_build_grammarp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *previousp);

/*****************************************************************************/
static short _marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;
  
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building internal grammar");

  grammarp = (marpaESLIF_grammar_t *) malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->idi        = 0;   /* Start grammar is level 0 */
  grammarp->descs      = NULL;
  grammarp->ruleStackp = NULL;
  grammarp->previousp  = NULL;
  grammarp->nextp      = NULL;
  
  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIF_free_grammarv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_grammarv";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing grammar");

  if (grammarp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "... Grammar name is %s", grammarp->descs != NULL ? grammarp->descs : "(null)");
    if (grammarp->descs != NULL) {
      free(grammarp->descs);
    }
    _marpaESLIF_free_ruleStackv(marpaESLIFp, grammarp->ruleStackp);
    free(grammarp);
  }
}

/*****************************************************************************/
static void _marpaESLIF_free_ruleStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_ruleStackv";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing rule stack");

  if (ruleStackp != NULL) {
    while (GENERICSTACK_USED(ruleStackp) > 0) {
      if (GENERICSTACK_IS_PTR(ruleStackp, GENERICSTACK_USED(ruleStackp) - 1)) {
	marpaESLIF_rule_t *rulep = (marpaESLIF_rule_t *) GENERICSTACK_POP_PTR(ruleStackp);
	_marpaESLIF_free_rulev(marpaESLIFp, rulep);
      }
    }
    GENERICSTACK_FREE(ruleStackp);
  }
}

/*****************************************************************************/
static void _marpaESLIF_free_rulev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_rulev";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing rule");

  if (rulep != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "... Rule is %s", rulep->descs != NULL ? rulep->descs : "(null)");
    if (rulep->descs != NULL) {
      free(rulep->descs);
    }
    _marpaESLIF_free_symbolv(marpaESLIFp, rulep->lhsp);
    _marpaESLIF_free_symbolStackv(marpaESLIFp, rulep->rhsStackp);
    _marpaESLIF_free_symbolStackv(marpaESLIFp, rulep->exceptionStackp);
    free(rulep);
  }
}

/*****************************************************************************/
static void _marpaESLIF_free_symbolv(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_symbolv";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing symbol");

  if (symbolp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "... Symbol is %s", symbolp->descs != NULL ? symbolp->descs : "(null)");
    if (symbolp->descs != NULL) {
      free(symbolp->descs);
    }
    if (! symbolp->shallowb) {
      switch (symbolp->type) {
      case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
	_marpaESLIF_free_terminalv(marpaESLIFp, symbolp->u.terminalp);
	break;
      case MARPAESLIF_SYMBOL_TYPE_META:
	_marpaESLIF_free_rulev(marpaESLIFp, symbolp->u.rulep);
	break;
      case MARPAESLIF_SYMBOL_TYPE_NA:
	break;
      default:
	break;
      }
    }
    free(symbolp);
  }
}

/*****************************************************************************/
static void _marpaESLIF_free_symbolStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_symbolStackv";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing symbol stack");

  if (symbolStackp != NULL) {
    while (GENERICSTACK_USED(symbolStackp) > 0) {
      if (GENERICSTACK_IS_PTR(symbolStackp, GENERICSTACK_USED(symbolStackp) - 1)) {
	marpaESLIF_symbol_t *symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_POP_PTR(symbolStackp);
	_marpaESLIF_free_symbolv(marpaESLIFp, symbolp);
      }
    }
    GENERICSTACK_FREE(symbolStackp);
  }
}

/*****************************************************************************/
static void _marpaESLIF_free_terminalv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_terminalv";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing terminal");

  if (terminalp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "... Terminal is %s", terminalp->descs != NULL ? terminalp->descs : "(null)");
    if (terminalp->descs != NULL) {
      free(terminalp->descs);
    }
    free(terminalp);
  }
}

