#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include "marpaESLIF/internal/grammar.h"
#include "marpaESLIF/internal/logging.h"

/* ESLIF is the internal and external grammars, plus the options */
struct marpaESLIF {
  marpaESLIF_grammar_t *internalGrammarp; /* This is the ESLIF itself */
  marpaESLIFOption_t    option;
};

marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /* genericLoggerp */
};

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, int idi, char *descs, marpaESLIF_terminal_type_t type, char *originp, size_t originl);
static inline void                   _marpaESLIF_terminal_initv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, genericStack_t *symbolStackp, genericStack_t *ruleStackp, marpaESLIF_grammar_t *previousp);
static inline void                   _marpaESLIF_grammar_initv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp);

static inline void                   _marpaESLIF_ruleStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp);
static inline void                   _marpaESLIF_symbolStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_initv(marpaESLIF_t *marpaESLIFp, marpaESLIFOption_t *marpaESLIFOptionp);

/* Internal grammar specification */
const static char *bootstrap_grammar_descs = "Internal Grammar";
const static enum boostrap_grammar_L0_symbol_id {
  RESERVED_EVENT_NAME = 0,
  WHITESPACE,
  HASH_COMMENT,
  TERMINATED_HASH_COMMENT,
  UNTERMINATED_FINAL_HASH_COMMENT,
  HASH_COMMENT_BODY,
  VERTICAL_SPACE_CHAR,
  HASH_COMMENT_CHAR,
  OP_DECLARE_BNF,
  OP_DECLARE_MATCH,
  OP_LOOSEN,
  OP_EQUAL_PRIORITY,
  BEFORE,
  AFTER,
  BEFORE_OR_AFTER,
  SIGNED_INTEGER,
  SIGN,
  INTEGER,
  BOOLEAN,
  RESERVED_ACTION_NAME,
  RESERVED_BLESSING_NAME,
  ONE_OR_MORE_WORD_CHARACTERS,
  ZERO_OR_MORE_WORD_CHARACTERS,
  PERL_IDENTIFIER,
  DOUBLE_COLON,
  PERL_NAME,
  BARE_NAME,
  STANDARD_NAME,
  BRACKETED_NAME,
  BRACKETED_NAME_STRING,
  ARRAY_DESCRIPTOR,
  ARRAY_DESCRIPTOR_LEFT_BRACKET,
  ARRAY_DESCRIPTOR_RIGHT_BRACKET,
  RESULT_ITEM_DESCRIPTOR_LIST,
  RESULT_ITEM_DESCRIPTOR_SEPARATOR,
  RESULT_ITEM_DESCRIPTOR,
  SINGLE_QUOTED_STRING,
  SINGLE_QUOTED_NAME,
  STRING_WITHOUT_SINGLE_QUOTE_OR_VERTICAL_SPACE,
  CHARACTER_CLASS,
  CC_ELEMENTS,
  CC_ELEMENT,
  SAFE_CC_CHARACTER,
  ESCAPED_CC_CHARACTER,
  CHARACTER_CLASS_MODIFIERS,
  CHARACTER_CLASS_MODIFIER,
  POSIX_CHAR_CLASS,
  NETAGED_POSIX_CHAR_CLASS,
  POSIX_CHAR_CLASS_NAME,
  HORIZONTAL_CHARACTER,
  _BOOTSTRAP_GRAMMAR_L0_SYMBOL_ID_MAX
} boostrap_grammar_L0_symbol_id_t;

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, int idi, char *descs, marpaESLIF_terminal_type_t type, char *originp, size_t originl)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_terminal_newp";
  marpaESLIF_terminal_t            *terminalp;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;
  PCRE2_SPTR                        pcre2_pattern;
  int                               errornumber;
  PCRE2_SIZE                        erroroffset;
  PCRE2_UCHAR                       errorBuffer[256];

  terminalp = (marpaESLIF_terminal_t *) sizeof(marpaESLIF_terminal_t);
  if (terminalp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  _marpaESLIF_terminal_initv(marpaESLIFp, terminalp);

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
    terminalp->u.stringp = (char *) malloc(originl);
    if (terminalp->u.stringp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy((void *) terminalp->u.stringp, originp, originl);
    break;

  case MARPAESLIF_TERMINAL_TYPE_REGEXP:
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Invalid terminal origin");
      goto err;
    }
    terminalp->u.regexp = pcre2_compile(
					(PCRE2_SPTR) originp,  /* An UTF-8 pattern */
					(PCRE2_SIZE) originl,  /* containing originl code units (!= code points) - in UTF-8 a code unit is a byte */
					0,                     /* default options */
					&errornumber,          /* for error number */
					&erroroffset,          /* for error offset */
					NULL);                 /* use default compile context */
    if (terminalp->u.regexp == NULL) {
      pcre2_get_error_message(errornumber, errorBuffer, sizeof(errorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "PCRE2 regexp compilation failure at offset %d: %s", (int) erroroffset, errorBuffer);
      goto err;
    }
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
static inline void _marpaESLIF_terminal_initv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  terminalp->idi          = -1;
  terminalp->descs        = NULL;
  terminalp->type         = MARPAESLIF_TERMINAL_TYPE_NA;
  terminalp->initializerp = NULL;
  terminalp->matcherp     = NULL;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char          *funcs              = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_grammar_t       *marpaESLIFGrammarp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building internal grammar");

  marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->option.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = 0;
  marpaWrapperGrammarOption.warningIsIgnoredb = 0;
  marpaWrapperGrammarOption.autorankb         = 0;
  
  marpaESLIFGrammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, NULL, NULL, NULL);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  goto done;
  
 err:
  _marpaESLIF_grammar_freev(marpaESLIFp, marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp);
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, genericStack_t *symbolStackp, genericStack_t *ruleStackp, marpaESLIF_grammar_t *previousp)
/*****************************************************************************/
{
  const static char    *funcs    = "_marpaESLIF_grammar_newp";
  marpaESLIF_grammar_t *grammarp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building grammar");

  grammarp = malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  _marpaESLIF_grammar_initv(marpaESLIFp, grammarp);

  grammarp->marpaWrapperGrammarp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarp == NULL) {
    goto err;
  }
  if (symbolStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No symbol");
    goto err;
  }
  grammarp->symbolStackp = symbolStackp;
  if (ruleStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No Rule");
    goto err;
  }
  grammarp->ruleStackp   = ruleStackp;
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
static inline void _marpaESLIF_grammar_initv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  marpaESLIFGrammarp->marpaWrapperGrammarp = NULL;
  marpaESLIFGrammarp->symbolStackp         = NULL;
  marpaESLIFGrammarp->ruleStackp           = NULL;
  marpaESLIFGrammarp->previousp            = NULL;
  marpaESLIFGrammarp->nextp                = NULL;
}

/*****************************************************************************/
static inline void _marpaESLIF_grammar_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_grammar_freev";

  if (grammarp != NULL) {
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing grammar");

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
static inline void _marpaESLIF_ruleStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_ruleStack_freev";

  if (ruleStackp != NULL) {
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing rule stack");
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
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing rule %s", rulep->descs != NULL ? rulep->descs : "(null)");
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
static inline void _marpaESLIF_symbol_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_symbol_freev";

  if (symbolp != NULL) {
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing symbol");
    if (! symbolp->shallowb) {
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
    }
    free(symbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_symbolStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_symbolStack_freev";

  if (symbolStackp != NULL) {
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing symbol stack");
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
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing terminal %s", terminalp->descs != NULL ? terminalp->descs : "(null)");
    if (terminalp->descs != NULL) {
      free(terminalp->descs);
    }
    switch (terminalp->type) {
    case MARPAESLIF_TERMINAL_TYPE_STRING:
      if (terminalp->u.stringp != NULL) {
	free((void *) terminalp->u.stringp);
      }
      break;
    case MARPAESLIF_TERMINAL_TYPE_REGEXP:
      if (terminalp->u.regexp != NULL) {
	pcre2_code_free(terminalp->u.regexp);
      }
      break;
    default:
      break;
    }
    free(terminalp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_initv(marpaESLIF_t *marpaESLIFp, marpaESLIFOption_t *marpaESLIFOptionp)
/*****************************************************************************/
{
  marpaESLIFp->option = *marpaESLIFOptionp;
  marpaESLIFp->internalGrammarp = NULL;
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

  _marpaESLIF_initv(marpaESLIFp, marpaESLIFOptionp);
  
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

