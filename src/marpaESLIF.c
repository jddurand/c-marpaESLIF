#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include <tconv.h>
#include <ctype.h>
#include "config.h"
#include "marpaESLIF/internal/grammar.h"
#include "marpaESLIF/internal/logging.h"

#ifndef MARPAESLIF_INITIAL_REPLACEMENT_LENGTH
#define MARPAESLIF_INITIAL_REPLACEMENT_LENGTH 8096  /* Subjective number */
#endif

/* ESLIF is the internal and external grammars, plus the options */
struct marpaESLIF {
  marpaESLIF_grammar_t *internalGrammarp;
  marpaESLIF_grammar_t *externalGrammarp;
  marpaESLIFOption_t    option;
};

marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /* genericLoggerp */
};

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, PCRE2_SPTR substitutionp, PCRE2_SIZE substitutionl);
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

static inline short                  _marpaESLIF_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp, char *inputp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp);

const static  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(marpaESLIF_t *marpaESLIFp, char *utf82printableasciip);

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, PCRE2_SPTR substitutionp, PCRE2_SIZE substitutionl)
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
  terminalp->asciidescs   = NULL;
  terminalp->type         = MARPAESLIF_TERMINAL_TYPE_NA;

  marpaWrapperGrammarSymbolOption.terminalb = 1;
  marpaWrapperGrammarSymbolOption.startb    = startb;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Terminal Identifier ------------ */
  terminalp->idi = marpaWrapperGrammar_newSymboli(marpaESLIFGrammarp->marpaWrapperGrammarp, &marpaWrapperGrammarSymbolOption);
  if (terminalp->idi < 0) {
    goto err;
  }

  /* -------- Terminal UTF-8 Description -------- */
  if (descs == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No terminal description");
    goto err;
  }
  terminalp->descs = (char *) malloc(descl);
  if (terminalp->descs == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }
  memcpy(terminalp->descs, descs, descl);

  /* --------- Terminal ASCII Description -------- */
  terminalp->asciidescs = _marpaESLIF_utf82printableascii_newp(marpaESLIFp, descs, descl);

  /* ----------- Terminal Implementation ------------ */
  terminalp->type = type;
  switch (type) {

  case MARPAESLIF_TERMINAL_TYPE_STRING:
    terminalp->u.string.stringp = NULL;
    terminalp->u.string.stringl = 0;
    
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

  case MARPAESLIF_TERMINAL_TYPE_REGEX:
    terminalp->u.regex.patternp      = NULL;
    terminalp->u.regex.substitutionp = NULL;
    terminalp->u.regex.substitutionl = 0;
    terminalp->u.regex.match_datap   = NULL;
#ifdef PCRE2_CONFIG_JIT
    terminalp->u.regex.jitCompleteb = 0;
    terminalp->u.regex.jitPartialb  = 0;
#endif
    
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "Invalid terminal origin");
      goto err;
    }
    if ((substitutionp != NULL) && (substitutionl > 0)) {
      terminalp->u.regex.substitutionp = (PCRE2_SPTR) malloc((size_t) substitutionl);
      if (terminalp->u.regex.substitutionp == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
	goto err;
      }
      memcpy((void *) terminalp->u.regex.substitutionp, substitutionp, substitutionl);
      terminalp->u.regex.substitutionl = substitutionl;
    }
    pcre2Optioni = PCRE2_ANCHORED|PCRE2_UTF;      /* In our case, patterns are always anchored and in UTF mode */
    for (i = 0; i < _MARPAESLIF_REGEX_OPTION_UNGREEDY_ID_MAX; i++) {
      if ((opti & marpaESLIF_regex_option_map[i].opti) == marpaESLIF_regex_option_map[i].opti) {
	pcre2Optioni |= marpaESLIF_regex_option_map[i].pcre2Optioni;
      }
    }
    terminalp->u.regex.patternp = pcre2_compile(
						originp,      /* An UTF-8 pattern */
						originl,      /* containing originl code units (!= code points) - in UTF-8 a code unit is a byte */
						pcre2Optioni,
						&pcre2Errornumberi, /* for error number */
						&pcre2ErrorOffsetl, /* for error offset */
						NULL);        /* use default compile context */
    if (terminalp->u.regex.patternp == NULL) {
      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
      MARPAESLIF_ERRORF(marpaESLIFp, "pcre2_compile failure at offset %ld: %s", (unsigned long) pcre2ErrorOffsetl, pcre2ErrorBuffer);
      goto err;
    }
    if (terminalp->u.regex.substitutionp != NULL) {
      /* There is substitution: match_data must contain room for all the group matches */
      terminalp->u.regex.match_datap = pcre2_match_data_create_from_pattern(terminalp->u.regex.patternp,
							       NULL /* Default memory allocation */);
    } else {
      terminalp->u.regex.match_datap = pcre2_match_data_create(1 /* We are interested in the string that matched the full pattern */,
							       NULL /* Default memory allocation */);
    }
    if (terminalp->u.regex.match_datap == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "pcre2_match_data_create_from_pattern failure, %s", strerror(errno));
      goto err;
    }
    /* Determine if we can do JIT */
#ifdef PCRE2_CONFIG_JIT
    if ((pcre2_config(PCRE2_CONFIG_JIT, &pcre2Optioni) >= 0) && (pcre2Optioni == 1)) {
#ifdef PCRE2_JIT_COMPLETE
      terminalp->u.regex.jitCompleteb = (pcre2_jit_compile(terminalp->u.regex.patternp, PCRE2_JIT_COMPLETE) == 0) ? 1 : 0;
#else
      terminalp->u.regex.jitCompleteb = 0;
#endif
#ifdef PCRE2_JIT_PARTIAL_HARD
      terminalp->u.regex.jitPartialb = (pcre2_jit_compile(terminalp->u.regex.patternp, PCRE2_JIT_PARTIAL_HARD) == 0) ? 1 : 0;
#else
      terminalp->u.regex.jitPartialb = 0;
#endif /*  PCRE2_CONFIG_JIT */
    } else {
      terminalp->u.regex.jitCompleteb = 0;
      terminalp->u.regex.jitPartialb = 0;
    }
#endif /*  PCRE2_CONFIG_JIT */
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
					    strlen(bootstrap_grammar_L0_symbols[i].descs) + 1,  /* Bootstrap's descs is a C string */
					    bootstrap_grammar_L0_symbols[i].terminalType,
					    bootstrap_grammar_L0_symbols[i].optioni,
					    bootstrap_grammar_L0_symbols[i].originp,
					    (bootstrap_grammar_L0_symbols[i].originp != NULL) ? strlen(bootstrap_grammar_L0_symbols[i].originp) : 0,
					    bootstrap_grammar_L0_symbols[i].substitutionp,
					    (bootstrap_grammar_L0_symbols[i].substitutionp != NULL) ? strlen(bootstrap_grammar_L0_symbols[i].substitutionp) : 0
					    );
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
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing rule %s at %p", rulep->asciidescs != NULL ? rulep->asciidescs : "(null)", rulep);
    if (rulep->descs != NULL) {
      free(rulep->descs);
    }
    _marpaESLIF_utf82printableascii_freev(marpaESLIFp, rulep->asciidescs);
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
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing terminal %s at %p", terminalp->asciidescs != NULL ? terminalp->asciidescs : "(null)", terminalp);
    if (terminalp->descs != NULL) {
      free(terminalp->descs);
    }
    _marpaESLIF_utf82printableascii_freev(marpaESLIFp, terminalp->asciidescs);
    switch (terminalp->type) {
    case MARPAESLIF_TERMINAL_TYPE_STRING:
      if (terminalp->u.string.stringp != NULL) {
	free((void *) terminalp->u.string.stringp);
      }
      break;
    case MARPAESLIF_TERMINAL_TYPE_REGEX:
      if (terminalp->u.regex.match_datap != NULL) {
	pcre2_match_data_free(terminalp->u.regex.match_datap);
      }
      if (terminalp->u.regex.patternp != NULL) {
	pcre2_code_free(terminalp->u.regex.patternp);
      }
      if (terminalp->u.regex.substitutionp != NULL) {
	free((void *) terminalp->u.regex.substitutionp);
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

/*****************************************************************************/
static inline short _marpaESLIF_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp, char *inputp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  const static char         *funcs = "_marpaESLIF_matcher";
  short                      rcb   = 1;
  marpaESLIF_matcher_value_t rci;
  marpaESLIF_string_t        marpaESLIF_string;
  marpaESLIF_regex_t         marpaESLIF_regex;
  int                        pcre2Errornumberi;
  PCRE2_UCHAR                pcre2ErrorBuffer[256];
  PCRE2_SIZE                *pcre2_ovectorp;
  size_t                     matchLengthl;
  PCRE2_UCHAR               *pcre2_substitutep;
  PCRE2_SIZE                 pcre2_substitutel = MARPAESLIF_INITIAL_REPLACEMENT_LENGTH; /* In code units, not bytes, though in UTF-8 a code unit is one byte -; */
  PCRE2_SIZE                 pcre2_substituteoutputl;
  PCRE2_UCHAR               *tmpp;
 
  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*********************************************************************************/

  if (inputl > 0) {

    switch (terminalp->type) {
      
    case MARPAESLIF_TERMINAL_TYPE_STRING:
      marpaESLIF_string = terminalp->u.string;
      if (inputl >= marpaESLIF_string.stringl) {
	rci = (memcmp(inputp, marpaESLIF_string.stringp, marpaESLIF_string.stringl) == 0) ? MARPAESLIF_MATCH_OK : MARPAESLIF_MATCH_FAILURE;
      } else {
	rci = (memcmp(inputp, marpaESLIF_string.stringp, inputl) == 0) ? MARPAESLIF_MATCH_AGAIN : MARPAESLIF_MATCH_FAILURE;
      }
      break;
      
    case MARPAESLIF_TERMINAL_TYPE_REGEX:
      marpaESLIF_regex = terminalp->u.regex;

      /* If there is substitution, prepare memory. */
      if (marpaESLIF_regex.substitutionp != NULL) {
	pcre2_substitutep = (PCRE2_UCHAR *) malloc(pcre2_substitutel);
	if (pcre2_substitutep == NULL) {
	  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "malloc failure, %s", strerror(errno));
	  goto err;
	}
      }
      /* --------------------------------------------------------- */
      /* EOF mode:                                                 */
      /* return full match status: OK or FAILURE.                  */
      /*                                                           */
      /* Please note that substitution is supported only in the    */
      /* full match case.                                          */
      /* For substitution, except the error cases, no replacement  */
      /* means PCRE2_ERROR_NOMATCH.                                */
      /* --------------------------------------------------------- */
      /* NOT EOF mode:                                             */
      /* If the full match is successful:                          */
      /* - if it reaches the end of the buffer, return EGAIN.      */
      /* - if it does not reach the end of the buffer, return OK.  */
      /* Else if the partial match is successul:                   */
      /* - return EGAIN.                                           */
      /* Else                                                      */
      /* - return FAILURE.                                         */
      /*                                                           */
      /* In conclusion we always start with the full match.        */
      /* --------------------------------------------------------- */
      if (pcre2_substitutep != NULL) {
	/* When there is substitution, only pcre2_substitute() is supported. */
	/* JIT will be used automatically if available.                      */
	marpaESLIF_uint32_t optioni =
	  PCRE2_NOTEMPTY
	  |
	  PCRE2_SUBSTITUTE_OVERFLOW_LENGTH; /* If the output is PCRE2_ERROR_NOMEMORY, then wanted length is in pcre2_substitutel */
	do {
	  pcre2_substituteoutputl = pcre2_substitutel;
	  pcre2Errornumberi = pcre2_substitute(marpaESLIF_regex.patternp,      /* code */
					       (PCRE2_SPTR) inputp,            /* subject */
					       (PCRE2_SIZE) inputl,            /* length */
					       (PCRE2_SIZE) 0,                 /* startoffset */
					       optioni,                        /* options */
					       marpaESLIF_regex.match_datap,   /* match data */
					       NULL,                           /* match context - used default */
					       marpaESLIF_regex.substitutionp, /* Substitute specification */
					       marpaESLIF_regex.substitutionl, /* Substitute specification length (UTF-8: number of bytes) */
					       pcre2_substitutep,
					      &pcre2_substituteoutputl
					       );
#ifdef PCRE2_CONFIG_JIT
	  if (marpaESLIF_regex.jitCompleteb && (pcre2Errornumberi == PCRE2_ERROR_JIT_STACKLIMIT) && ((optioni & PCRE2_NO_JIT) != PCRE2_NO_JIT)) {
	    /* Back luck, out of stack for JIT - retry without JIT. */
	    /* Please note that is NOT documented how internally pcre2_substitute() reacts in this case... */
	    /* This is why it is coded explicitely here. */
	    pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
	    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "pcre2_substitute failure: %s - switching to non-JIT version", pcre2ErrorBuffer);
	    optioni |= PCRE2_NO_JIT;
	    continue;
	  }
#endif
	  switch (pcre2Errornumberi) {
	  case PCRE2_ERROR_NOMEMORY:
	    /* We will continue the do {} while () loop */

	    /* pcre2_substituteoutputl contains the minimum length needed, include space for a trailing zero */
	    /* Here length is a code unit, i.e. a byte since we are in UTF-8. */

	    /* Detect overflow -; */
	    if (pcre2_substituteoutputl < pcre2_substitutel) {
	      MARPAESLIF_ERROR(marpaESLIFp, "PCRE2_SIZE overflow detected");
	      goto err;
	    }
	    tmpp = realloc(pcre2_substitutep, pcre2_substituteoutputl);
	    if (tmpp == NULL) {
	      MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
	      goto err;
	    }
	    pcre2_substitutep = tmpp;
	    pcre2_substitutel = pcre2_substituteoutputl;
	    break;
	  case 0:
	    /* We will exit the do {} while () loop: no match */
	    pcre2Errornumberi = PCRE2_ERROR_NOMATCH;
	    break;
	  default:
	    /* We will exit the do {} while () loop: either match or uncauchgt failure */
	    if (pcre2Errornumberi >= 1) {
	      /* At least one match: this is a success */
	      /* We try to release non-needed memory. realloc() failure here is not fatal.    */
	      /* Please note that pcre2_substituteoutputl exclude the trailing zero, which is */
	      /* fine for us.                                                                 */
	      tmpp = realloc(pcre2_substitutep, pcre2_substituteoutputl);
	      if (tmpp == NULL) {
		MARPAESLIF_TRACEF(marpaESLIFp, funcs, "realloc failure (non fatal), %s", strerror(errno));
	      } else {
		pcre2_substitutep = tmpp;
		pcre2_substitutel = pcre2_substituteoutputl;
	      }
	    }
	    break;
	  }
	  /* Loop until we have enough memory - any other value is causing a loop exit. */
	} while (pcre2Errornumberi == PCRE2_ERROR_NOMEMORY);
      } else {
#ifdef PCRE2_CONFIG_JIT
	if (marpaESLIF_regex.jitCompleteb) {
	  pcre2Errornumberi = pcre2_jit_match(marpaESLIF_regex.patternp,     /* code */
					      (PCRE2_SPTR) inputp,          /* subject */
					      (PCRE2_SIZE) inputl,          /* length */
					      (PCRE2_SIZE) 0,               /* startoffset */
					      PCRE2_NOTEMPTY,               /* options - this one is supported in JIT mode */
					      marpaESLIF_regex.match_datap, /* match data */
					      NULL                          /* match context - used default */
					      );
	  if (pcre2Errornumberi == PCRE2_ERROR_JIT_STACKLIMIT) {
	    /* Back luck, out of stack for JIT */
	    pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
	    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "pcre2_jit_match failure: %s - switching to non-JIT version", pcre2ErrorBuffer);
	    goto eof_nojitcomplete;
	  }
	} else {
	eof_nojitcomplete:
#endif
	  pcre2Errornumberi = pcre2_match(marpaESLIF_regex.patternp,    /* code */
					  (PCRE2_SPTR) inputp,          /* subject */
					  (PCRE2_SIZE) inputl,          /* length */
					  (PCRE2_SIZE) 0,               /* startoffset */
					  PCRE2_NOTEMPTY,               /* options */
					  marpaESLIF_regex.match_datap, /* match data */
					  NULL                          /* match context - used default */
					  );
#ifdef PCRE2_CONFIG_JIT
	}
#endif
      }
      if (eofb) {
	if (pcre2Errornumberi < 0) {
	  /* Only PCRE2_ERROR_NOMATCH is an acceptable error. */
	  /* In case of substitution, remember we overwrote pcre2Errornumberi to PCRE2_ERROR_NOMATCH */
	  /* if there was no match -; */
	  if (pcre2Errornumberi != PCRE2_ERROR_NOMATCH) {
	    pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
	    MARPAESLIF_WARNF(marpaESLIFp, "Uncaught pcre2 match failure: %s", pcre2ErrorBuffer);
	  }
	  rci = MARPAESLIF_MATCH_FAILURE;
	} else {
	  /* Check the length of matched data */
	  if (pcre2_get_ovector_count(marpaESLIF_regex.match_datap) <= 0) {
	    MARPAESLIF_ERROR(marpaESLIFp, "pcre2_get_ovector_count returned no number of pairs of values");
	    goto err;
	  }
	  pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
	  if (pcre2_ovectorp == NULL) {
	    MARPAESLIF_ERROR(marpaESLIFp, "pcre2_get_ovector_pointer returned NULL");
	    goto err;
	  }
	  /* We said PCRE2_NOTEMPTY so this cannot be empty */
	  matchLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
	  if (matchLengthl <= 0) {
	    MARPAESLIF_ERROR(marpaESLIFp, "Empty match when it is configured as not possible");
	    goto err;
	  }
	  /* Very good -; */
	  rci = MARPAESLIF_MATCH_OK;
	}
      } else {
	if (pcre2Errornumberi >= 0) {
	  /* Full match is successful (this include an eventual substitution, that is also a full match). */
	  /* Check the length of matched data */
	  if (pcre2_get_ovector_count(marpaESLIF_regex.match_datap) <= 0) {
	    MARPAESLIF_ERROR(marpaESLIFp, "pcre2_get_ovector_count returned no number of pairs of values");
	    goto err;
	  }
	  pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
	  if (pcre2_ovectorp == NULL) {
	    MARPAESLIF_ERROR(marpaESLIFp, "pcre2_get_ovector_pointer returned NULL");
	    goto err;
	  }
	  /* We said PCRE2_NOTEMPTY so this cannot be empty */
	  matchLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
	  if (matchLengthl <= 0) {
	    MARPAESLIF_ERROR(marpaESLIFp, "Empty match when it is configured as not possible");
	    goto err;
	  }
	  if (matchLengthl >= inputl) {
	    /* But end of the buffer is reached, and we are not at the eof! We have to ask for more bytes. */
	    rci = MARPAESLIF_MATCH_AGAIN;
	  } else {
	    /* And end of the buffer is not reached */
	    rci = MARPAESLIF_MATCH_OK;
	  }
	} else {
	  /* Do a partial match. This section cannot return MARPAESLIF_MATCH_OK. */
#ifdef PCRE2_CONFIG_JIT
	  if (marpaESLIF_regex.jitPartialb) {
	    pcre2Errornumberi = pcre2_jit_match(marpaESLIF_regex.patternp,     /* code */
						(PCRE2_SPTR) inputp,          /* subject */
						(PCRE2_SIZE) inputl,          /* length */
						(PCRE2_SIZE) 0,               /* startoffset */
						PCRE2_NOTEMPTY                /* options - this one is supported in JIT mode */
						|
						PCRE2_PARTIAL_HARD,           /* and this one as well, we want partial match to have precedence */
						marpaESLIF_regex.match_datap, /* match data */
						NULL                          /* match context - used default */
						);
	    if (pcre2Errornumberi == PCRE2_ERROR_JIT_STACKLIMIT) {
	      /* Back luck, out of stack for JIT */
	      pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
	      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "pcre2_jit_match failure: %s - switching to non-JIT version", pcre2ErrorBuffer);
	      goto eof_nojitpartial;
	    }
	  } else {
	  eof_nojitpartial:
#endif
	    pcre2Errornumberi = pcre2_match(marpaESLIF_regex.patternp,    /* code */
					    (PCRE2_SPTR) inputp,          /* subject */
					    (PCRE2_SIZE) inputl,          /* length */
					    (PCRE2_SIZE) 0,               /* startoffset */
					    PCRE2_NOTEMPTY                /* options */
					    |
					    PCRE2_PARTIAL_HARD,           /* we want partial match to have precedence */
					    marpaESLIF_regex.match_datap, /* match data */
					    NULL                          /* match context - used default */
					    );
#ifdef PCRE2_CONFIG_JIT
	  }
#endif
	  /* Only PCRE2_ERROR_PARTIAL is an acceptable error */
	  if (pcre2Errornumberi == PCRE2_ERROR_PARTIAL) {
	    /* Partial match is successful */
	    rci = MARPAESLIF_MATCH_AGAIN;
	  } else {
	    /* Partial match is not successful */
	    rci = MARPAESLIF_MATCH_FAILURE;
	  }
	}
      }
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Unsupported terminal type %d", terminalp->type);
      goto err;
    }
  } else {
    rci = eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN;
  }

  if (rcip != NULL) {
    *rcip = rci;
  }

  if (rci == MARPAESLIF_MATCH_OK) {
    /* It is important to handle outputlp before outputpp, see below    */
    /* Note that when rci is MARPAESLIF_MATCH_OK, it is guaranteed that */
    /* matchLenthl is set to a value > 0 (while substitutel may be 0).  */
    if (outputlp != NULL) {
      *outputlp = (pcre2_substitutep != NULL) ? pcre2_substitutel : matchLengthl;
    }
    if (outputpp != NULL) {
      /* When there is a substitution result, memory was already allocated */
      if (pcre2_substitutep != NULL) {
	*outputpp = pcre2_substitutep;
	/* Prevent a memory free at the end of this routine */
	pcre2_substitutep = NULL;
      } else {
	char *outputp = (char *) malloc(matchLengthl);
	/* Matched input is starting at inputp and its length is in matchLengthl */
	if (outputp == NULL) {
	  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "malloc failure, %s", strerror(errno));
	  goto err;
	}
	memcpy(outputp, inputp, matchLengthl);
	*outputpp = outputp;
      }
    }
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1, status is %s", (rci == MARPAESLIF_MATCH_FAILURE) ? "MARPAESLIF_MATCH_FAILURE" : ((rci == MARPAESLIF_MATCH_OK) ? "MARPAESLIF_MATCH_OK" : "MARPAESLIF_MATCH_AGAIN"));
  goto done;

 err:
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 0");
  rcb = 0;

 done:
  /* In any case release substitution result */
  if (pcre2_substitutep != NULL) {
    free((void *) pcre2_substitutep);
  }
  return rci;
}

#ifndef MARPAESLIF_NTRACE
/*****************************************************************************/
static void _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_tconvTraceCallback";
  marpaESLIF_t *marpaESLIFp = (marpaESLIF_t *) userDatavp;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s", msgs);
}
#endif

/*****************************************************************************/
static inline char *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_utf82printableascii_newp";
  char              *rcp         = (char *) _marpaESLIF_utf82printableascii_defaultp;
  tconv_t            tconvp      = NULL;
  char              *inbuforigp  = descs;
  size_t             inleftorigl = descl;
  char              *outbuforigp = NULL;
  size_t             outbuforigl = 0;
  tconv_option_t     tconvOption = { NULL /* charsetp */, NULL /* convertp */, NULL /* traceCallbackp */, NULL /* traceUserDatavp */ };
  char              *inbufp;
  size_t             inleftl;
  char              *outbufp;
  size_t             outleftl;
  size_t             nconvl;
  char              *p;
  unsigned char      c;

  /* ------- Our input is always a well formed UTF-8 */
#ifndef MARPAESLIF_NTRACE
  tconvOption.traceCallbackp  = _marpaESLIF_tconvTraceCallback;
  tconvOption.traceUserDatavp = marpaESLIFp;
#endif
  tconvp = tconv_open_ext("ASCII//TRANSLIT//IGNORE", "UTF-8", &tconvOption);
  if (tconvp == NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "tconv_open failure, %s", strerror(errno));
    goto fallback;
  }
#ifndef MARPAESLIF_NTRACE
  tconv_trace_on(tconvp);
#endif

  /* We start with an output buffer of the same size of input buffer */
  /* We want to make sure the ASCII string is always ending with 0.  */
  outbuforigp = (char *) calloc(descl + 1, 1);
  if (outbuforigp == NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "malloc failure, %s", strerror(errno));
    goto fallback;
  }
  outbuforigl = descl;

  /* We want to translate descriptions in trace or error cases - these are short things, and */
  /* it does not really harm if we redo the whole translation stuff in case of E2BIG:        */
  /* - in trace mode it is documented that performance is awful                              */
  /* - in error mode this call will happen once                                              */

  inbufp   = inbuforigp;
  inleftl  = inleftorigl;
  outbufp  = outbuforigp;
  outleftl = outbuforigl;
  
  while (1) {

    nconvl = tconv(tconvp, (inbufp != NULL) ? &inbufp : NULL, &inleftl, &outbufp, &outleftl);

    if (nconvl == (size_t) -1) {
      char  *tmp;
      size_t deltal = outbuforigl;
      
      /* The only acceptable error is E2BIG */
      if (errno != E2BIG) {
	MARPAESLIF_TRACEF(marpaESLIFp, funcs, "tconv failure, %s", strerror(errno));
	goto fallback;
      }
      /* Try to alloc more */
      outbuforigl *= 2;
      /* Will this ever happen ? */
      if (outbuforigl < descl) {
	MARPAESLIF_TRACE(marpaESLIFp, funcs, "size_t flip");
	goto fallback;
      }
      tmp = realloc(outbuforigp, outbuforigl + 1);
      if (tmp == NULL) {
	MARPAESLIF_TRACEF(marpaESLIFp, funcs, "realloc failure, %s", strerror(errno));
	goto fallback;
      }
      outbuforigp = tmp;
      outbuforigp[outbuforigl] = '\0';
      outleftl   += deltal;
    }

    if (inbufp == NULL) {
      /* This was the last round */
      break;
    }

    if (inleftl <= 0) {
      /* Next round is the last one */
      inbufp = NULL;
    }
  }

  rcp = outbuforigp;

  /* Remove by hand any ASCII character not truely printable.      */
  /* Only the historical ASCII table [0-127] is a portable thingy. */
  p = outbuforigp;
  while ((c = (unsigned char) *p) != '\0') {
    if ((c >= 128) || (! isprint(c & 0xFF))) {
      *p = ' ';
    }
    p++;
  }
  goto done;

 fallback:
  if (outbuforigp != NULL) {
    free(outbuforigp);
  }

 done:
  if (tconvp != NULL) {
    if (tconv_close(tconvp) != 0) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "tconv_close failure, %s", strerror(errno));
    }
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return \"%s\"", rcp);
  return rcp;
}

/*****************************************************************************/
static inline void _marpaESLIF_utf82printableascii_freev(marpaESLIF_t *marpaESLIFp, char *utf82printableasciip)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_utf82printableascii_freev";

  if ((utf82printableasciip != NULL) && (utf82printableasciip != _marpaESLIF_utf82printableascii_defaultp)) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing ASCII description \"%s\"", utf82printableasciip);
    free(utf82printableasciip);
  }
}
