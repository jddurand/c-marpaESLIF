#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include "config.h"
#include "marpaESLIF/internal/grammar.h"
#include "marpaESLIF/internal/logging.h"

/* ESLIF is the internal and external grammars, plus the options */
struct marpaESLIF {
  marpaESLIF_grammar_t *internalGrammarp;
  marpaESLIF_grammar_t *externalGrammarp;
  marpaESLIFOption_t    option;
};

marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /* genericLoggerp */
};

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, marpaESLIF_grammar_t *previousp);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp);

static inline genericStack_t        *_marpaESLIF_ruleStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_ruleStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp);

static inline void                   _marpaESLIF_rule_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp);

static inline genericStack_t        *_marpaESLIF_symbolStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbolStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp);

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_terminal_newp";
  marpaESLIF_terminal_t            *terminalp;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;
  marpaESLIF_uint32_t               pcre2Optioni;
  int                               pcre2Errornumberi;
  PCRE2_SIZE                        pcre2ErrorOffsetl;
  PCRE2_UCHAR                       pcre2ErrorBuffer[256];
  int                               i;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building terminal");

  terminalp = (marpaESLIF_terminal_t *) malloc(sizeof(marpaESLIF_terminal_t));
  if (terminalp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  terminalp->idi          = -1;
  terminalp->descs        = NULL;
  terminalp->type         = MARPAESLIF_TERMINAL_TYPE_NA;
  terminalp->initializerp = NULL;
  terminalp->matcherp     = NULL;

  marpaWrapperGrammarSymbolOption.terminalb = 1;
  marpaWrapperGrammarSymbolOption.startb    = startb;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Terminal Identifier ------------ */
  terminalp->idi = marpaWrapperGrammar_newSymboli(marpaESLIFGrammarp->marpaWrapperGrammarp, &marpaWrapperGrammarSymbolOption);
  if (terminalp->idi < 0) {
    goto err;
  }

  /* ----------- Terminal Description ------------ */
  if (descs == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No terminal description");
    goto err;
  }
  terminalp->descs = strdup(descs);
  if (terminalp->descs == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }

  /* ----------- Terminal Implementation ------------ */
  terminalp->type = type;
  switch (type) {

  case MARPAESLIF_TERMINAL_TYPE_STRING:
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Invalid terminal origin");
      goto err;
    }
    terminalp->u.string.stringp = (char *) malloc((size_t) originl);
    if (terminalp->u.string.stringp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy((void *) terminalp->u.string.stringp, originp, (size_t) originl);
    terminalp->u.string.stringl = originl;
    break;

  case MARPAESLIF_TERMINAL_TYPE_REGEXP:
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Invalid terminal origin");
      goto err;
    }
    pcre2Optioni = PCRE2_ANCHORED|PCRE2_UTF;      /* In our case, regexp are always anchored and in UTF mode */
    for (i = 0; i < _MARPAESLIF_REGEXP_OPTION_UNGREEDY_ID_MAX; i++) {
      if ((opti & marpaESLIF_regexp_option_map[i].opti) == marpaESLIF_regexp_option_map[i].opti) {
	pcre2Optioni |= marpaESLIF_regexp_option_map[i].pcre2Optioni;
      }
    }
    terminalp->u.regex.regexp = pcre2_compile(
					      originp,      /* An UTF-8 pattern */
					      originl,      /* containing originl code units (!= code points) - in UTF-8 a code unit is a byte */
					      pcre2Optioni,
					      &pcre2Errornumberi, /* for error number */
					      &pcre2ErrorOffsetl, /* for error offset */
					      NULL);        /* use default compile context */
    if (terminalp->u.regex.regexp == NULL) {
      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "PCRE2 regexp compilation failure at offset %ld: %s", (unsigned long) pcre2ErrorOffsetl, pcre2ErrorBuffer);
      goto err;
    }
    /* Determine if we can do JIT */
#ifdef PCRE2_CONFIG_JIT
    if ((pcre2_config(PCRE2_CONFIG_JIT, &pcre2Optioni) >= 0) && (pcre2Optioni == 1)) {
#ifdef PCRE2_JIT_COMPLETE
      terminalp->u.regex.jitCompleteb = (pcre2_jit_compile(terminalp->u.regex.regexp, PCRE2_JIT_COMPLETE) == 0) ? 1 : 0;
#else
      terminalp->u.regex.jitCompleteb = 0;
#endif
#ifdef PCRE2_JIT_PARTIAL_HARD
      terminalp->u.regex.jitPartialb = (pcre2_jit_compile(terminalp->u.regex.regexp, PCRE2_JIT_PARTIAL_HARD) == 0) ? 1 : 0;
#else
      terminalp->u.regex.jitPartialb = 0;
#endif
    } else {
      terminalp->u.regex.jitCompleteb = 0;
      terminalp->u.regex.jitPartialb = 0;
    }
#endif
    break;

  default:
    MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported terminal type %d", type);
    goto err;
    break;
  }

  goto done;
  
 err:
  _marpaESLIF_terminal_freev(marpaESLIFp, terminalp);
  terminalp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", terminalp);
  return terminalp;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char          *funcs              = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_symbol_t        *symbolp            = NULL;
  marpaESLIF_grammar_t       *marpaESLIFGrammarp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  int                         bootstrap_grammar_L0_symboli = sizeof(bootstrap_grammar_L0_symbols) / sizeof(bootstrap_grammar_L0_symbols[0]);
  int                         i;
  marpaESLIF_terminal_t      *terminalp;


  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Bootstrapping internal grammar");

  marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->option.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = 0;
  marpaWrapperGrammarOption.warningIsIgnoredb = 0;
  marpaWrapperGrammarOption.autorankb         = 0;
  
  marpaESLIFGrammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, NULL);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }
  for (i = 0; i < bootstrap_grammar_L0_symboli; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }

    switch (bootstrap_grammar_L0_symbols[i].symbolType) {

    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
					    marpaESLIFGrammarp,
					    0, /* startb */
					    MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
					    bootstrap_grammar_L0_symbols[i].descs,
					    bootstrap_grammar_L0_symbols[i].terminalType,
					    bootstrap_grammar_L0_symbols[i].optioni,
					    bootstrap_grammar_L0_symbols[i].originp,
					    strlen(bootstrap_grammar_L0_symbols[i].originp));
      if (terminalp == NULL) {
	goto err;
      }
      symbolp->type        = bootstrap_grammar_L0_symbols[i].symbolType;
      symbolp->u.terminalp = terminalp;

      GENERICSTACK_SET_PTR(marpaESLIFGrammarp->symbolStackp, symbolp, terminalp->idi);
      if (GENERICSTACK_ERROR(marpaESLIFGrammarp->symbolStackp)) {
	MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
	goto err;
      }
      /* Push is ok: symbolp is in marpaESLIFGrammarp->symbolStackp */
      symbolp = NULL;
      break;

    case MARPAESLIF_SYMBOL_TYPE_META:
      break;

    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Bad description of internal grammar: symbolType=%d", bootstrap_grammar_L0_symbols[i].symbolType);
      break;
    }
  }

  goto done;
  
 err:
  _marpaESLIF_symbol_freev(marpaESLIFp, symbolp);
  _marpaESLIF_grammar_freev(marpaESLIFp, marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp);
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, marpaESLIF_grammar_t *previousp)
/*****************************************************************************/
{
  const static char    *funcs    = "_marpaESLIF_grammar_newp";
  marpaESLIF_grammar_t *grammarp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building ESLIF grammar");

  grammarp = malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->marpaWrapperGrammarp = NULL;
  grammarp->symbolStackp         = NULL;
  grammarp->ruleStackp           = NULL;
  grammarp->previousp            = NULL;
  grammarp->nextp                = NULL;

  grammarp->marpaWrapperGrammarp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarp == NULL) {
    goto err;
  }
  grammarp->symbolStackp = _marpaESLIF_symbolStack_newp(marpaESLIFp);
  if (grammarp->symbolStackp == NULL) {
    goto err;
  }
  grammarp->ruleStackp = _marpaESLIF_ruleStack_newp(marpaESLIFp);
  if (grammarp->ruleStackp == NULL) {
    goto err;
  }
  grammarp->previousp    = previousp;

  goto done;

 err:
  _marpaESLIF_grammar_freev(marpaESLIFp, grammarp);
  grammarp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp);
  return grammarp;
}

/*****************************************************************************/
static inline void _marpaESLIF_grammar_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_grammar_freev";

  if (grammarp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing grammar at %p", grammarp);

    if (grammarp->marpaWrapperGrammarp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarp);
    }			       
    _marpaESLIF_symbolStack_freev(marpaESLIFp, grammarp->symbolStackp);
    _marpaESLIF_ruleStack_freev(marpaESLIFp, grammarp->ruleStackp);
    /* Previous grammar is ignored, it is our caller */
    _marpaESLIF_grammar_freev(marpaESLIFp, grammarp->nextp);
    free(grammarp);
  }
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIF_ruleStack_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_ruleStack_newp";
  genericStack_t      *stackp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building rule stack");

  GENERICSTACK_NEW(stackp);
  if (stackp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "stack new failure, %s", strerror(errno));
    goto done;
  }

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", stackp);
  return stackp;
}

/*****************************************************************************/
static inline void _marpaESLIF_ruleStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_ruleStack_freev";

  if (ruleStackp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing rule stack at %p, ruleStackp", ruleStackp);
    while (GENERICSTACK_USED(ruleStackp) > 0) {
      if (GENERICSTACK_IS_PTR(ruleStackp, GENERICSTACK_USED(ruleStackp) - 1)) {
	marpaESLIF_rule_t *rulep = (marpaESLIF_rule_t *) GENERICSTACK_POP_PTR(ruleStackp);
	_marpaESLIF_rule_freev(marpaESLIFp, rulep);
      }
    }
    GENERICSTACK_FREE(ruleStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_rule_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_rule_freev";

  if (rulep != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing rule %s at %p", rulep->descs != NULL ? rulep->descs : "(null)", rulep);
    if (rulep->descs != NULL) {
      free(rulep->descs);
    }
    _marpaESLIF_symbol_freev(marpaESLIFp, rulep->lhsp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, rulep->rhsStackp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, rulep->exceptionStackp);
    free(rulep);
  }
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_symbol_newp";
  marpaESLIF_symbol_t *symbolp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building symbol");

  symbolp = (marpaESLIF_symbol_t *) malloc(sizeof(marpaESLIF_symbol_t));
  if (symbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto done;
  }

  symbolp->type = MARPAESLIF_SYMBOL_TYPE_NA;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", symbolp);
  return symbolp;
}

/*****************************************************************************/
static inline void _marpaESLIF_symbol_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_symbol_freev";

  if (symbolp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing symbol at %p", symbolp);
    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      _marpaESLIF_terminal_freev(marpaESLIFp, symbolp->u.terminalp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      _marpaESLIF_rule_freev(marpaESLIFp, symbolp->u.rulep);
      break;
    default:
      break;
    }
    free(symbolp);
  }
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIF_symbolStack_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_symbolStack_newp";
  genericStack_t      *stackp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building symbol stack");

  GENERICSTACK_NEW(stackp);
  if (stackp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "stack new failure, %s", strerror(errno));
    goto done;
  }

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", stackp);
  return stackp;
}

/*****************************************************************************/
static inline void _marpaESLIF_symbolStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_symbolStack_freev";

  if (symbolStackp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing symbol stack at %p", symbolStackp);
    while (GENERICSTACK_USED(symbolStackp) > 0) {
      if (GENERICSTACK_IS_PTR(symbolStackp, GENERICSTACK_USED(symbolStackp) - 1)) {
	marpaESLIF_symbol_t *symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_POP_PTR(symbolStackp);
	_marpaESLIF_symbol_freev(marpaESLIFp, symbolp);
      }
    }
    GENERICSTACK_FREE(symbolStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_terminal_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_terminal_freev";

  if (terminalp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing terminal %s at %p", terminalp->descs != NULL ? terminalp->descs : "(null)", terminalp);
    if (terminalp->descs != NULL) {
      free(terminalp->descs);
    }
    switch (terminalp->type) {
    case MARPAESLIF_TERMINAL_TYPE_STRING:
      if (terminalp->u.string.stringp != NULL) {
	free((void *) terminalp->u.string.stringp);
      }
      break;
    case MARPAESLIF_TERMINAL_TYPE_REGEXP:
      if (terminalp->u.regex.regexp != NULL) {
	pcre2_code_free(terminalp->u.regex.regexp);
      }
      break;
    default:
      break;
    }
    free(terminalp);
  }
}

/*****************************************************************************/
marpaESLIF_t *marpaESLIF_newp(marpaESLIFOption_t *marpaESLIFOptionp)
/*****************************************************************************/
{
  const static char *funcs = "marpaESLIF_newp";
  marpaESLIF_t      *marpaESLIFp;
  genericLogger_t   *genericLoggerp;

  if (marpaESLIFOptionp == NULL) {
    marpaESLIFOptionp = &marpaESLIFOption_default;
  }

  genericLoggerp = marpaESLIFOptionp->genericLoggerp;

#ifndef MARPAESLIF_NTRACE
  if (genericLoggerp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Building ESLIF", funcs);
  }
#endif

  marpaESLIFp = malloc(sizeof(marpaESLIF_t));
  if (marpaESLIFp == NULL) {
    if (marpaESLIFOptionp->genericLoggerp != NULL) {
      GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "malloc failure, %s", strerror(errno));
      goto err;
    }
  }

  marpaESLIFp->option = *marpaESLIFOptionp;
  marpaESLIFp->internalGrammarp = NULL;
  
  /* Create internal ESLIF grammar - it is important to set the option first */
  marpaESLIFp->internalGrammarp = _marpaESLIF_bootstrap_grammarb(marpaESLIFp);
  if (marpaESLIFp->internalGrammarp == NULL) {
    goto err;
  }

  goto done;
  
 err:
  marpaESLIF_freev(marpaESLIFp);
  marpaESLIFp = NULL;

 done:
#ifndef MARPAESLIF_NTRACE
  if (genericLoggerp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "[%s] return %p", funcs, marpaESLIFp);
  }
#endif
	
  return marpaESLIFp;
}


/*****************************************************************************/
void marpaESLIF_freev(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  if (marpaESLIFp != NULL) {
    _marpaESLIF_grammar_freev(marpaESLIFp, marpaESLIFp->internalGrammarp);
    free(marpaESLIFp);
  }
}

