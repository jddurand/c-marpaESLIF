#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include "marpaESLIF/internal/grammar.h"
#include "marpaESLIF/internal/logging.h"

/* ESLIF is the internal and external grammars, plus the options */
struct marpaESLIF {
  marpaESLIF_grammar_t *internalGrammarp;
  marpaESLIF_grammar_t *externalGrammarp;
  marpaESLIFOption_t    option;
};

static inline marpaESLIF_terminal_t *_marpaESLIF_terminalp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, int idi, char *descs, marpaESLIF_terminal_type_t type, char *originp, size_t originl);
static inline void                   _marpaESLIF_terminal_initv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);
static inline void                   _marpaESLIF_free_grammarv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp);
static inline void                   _marpaESLIF_free_ruleStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp);
static inline void                   _marpaESLIF_free_rulev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep);
static inline void                   _marpaESLIF_free_symbolv(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp);
static inline void                   _marpaESLIF_free_symbolStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp);
static inline void                   _marpaESLIF_free_terminalv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);
static inline short                  _marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_build_grammarp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *previousp);

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
static inline marpaESLIF_terminal_t *_marpaESLIF_terminalp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, int idi, char *descs, marpaESLIF_terminal_type_t type, char *originp, size_t originl)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_terminalp";
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
					(PCRE2_SPTR) originp, /* An UTF-8 pattern */
					(PCRE2_SIZE) originl, /* containing originl code units (!= code points) - in UTF-8 a code unit is a byte -; */
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
  _marpaESLIF_free_terminalv(marpaESLIFp, terminalp);
  terminalp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", terminalp);
  return terminalp;
}

/*****************************************************************************/
static inline void _marpaESLIF_terminal_initv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIF_terminal_initvp";

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Initializing terminal");

  terminalp->idi          = -1;
  terminalp->descs        = NULL;
  terminalp->type         = MARPAESLIF_TERMINAL_TYPE_NA;
  terminalp->initializerp = NULL;
  terminalp->matcherp     = NULL;
}

/*****************************************************************************/
static inline short _marpaESLIF_bootstrap_grammarb(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char    *funcs              = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_grammar_t *grammarp;
  short                 rcb;
  
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building internal grammar");

  grammarp = (marpaESLIF_grammar_t *) malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->idi          = 0;   /* Start grammar is level 0 */
  grammarp->descs        = NULL;
  grammarp->symbolStackp = NULL;
  grammarp->ruleStackp   = NULL;
  grammarp->previousp    = NULL;
  grammarp->nextp        = NULL;

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static inline void _marpaESLIF_free_grammarv(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_grammarv";

  if (grammarp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing grammar %s", grammarp->descs != NULL ? grammarp->descs : "(null)");
    if (grammarp->descs != NULL) {
      free(grammarp->descs);
    }
    _marpaESLIF_free_symbolStackv(marpaESLIFp, grammarp->symbolStackp);
    _marpaESLIF_free_ruleStackv(marpaESLIFp, grammarp->ruleStackp);
    _marpaESLIF_free_grammarv(marpaESLIFp, grammarp->nextp);
    free(grammarp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_free_ruleStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_ruleStackv";

  if (ruleStackp != NULL) {
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing rule stack");
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
static inline void _marpaESLIF_free_rulev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_rulev";

  if (rulep != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing rule %s", rulep->descs != NULL ? rulep->descs : "(null)");
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
static inline void _marpaESLIF_free_symbolv(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_symbolv";

  if (symbolp != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing symbol %s", symbolp->descs != NULL ? symbolp->descs : "(null)");
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
      default:
	break;
      }
    }
    free(symbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_free_symbolStackv(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_symbolStackv";

  if (symbolStackp != NULL) {
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "Freeing symbol stack");
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
static inline void _marpaESLIF_free_terminalv(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_free_terminalv";

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

