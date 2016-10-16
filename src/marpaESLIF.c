#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include <genericStack.h>
#include <tconv.h>
#include <ctype.h>
#include "config.h"
#include "marpaESLIF/internal/grammar.h"
#include "marpaESLIF/internal/logging.h"

#ifndef MARPAESLIF_INITIAL_REPLACEMENT_LENGTH
#define MARPAESLIF_INITIAL_REPLACEMENT_LENGTH 8096  /* Subjective number */
#endif

/* ESLIF is nothing else but a stack of grammars, plus the options. */
/* Every grammar is in reality a stack of grammars: 0 is Marpa::R2's G1, 1 is Marpa::R2's L0, and so on */
struct marpaESLIF {
  genericStack_t     *grammarStackp;
  marpaESLIFOption_t  option;
};

marpaESLIFOption_t marpaESLIFOption_default = {
  NULL               /* genericLoggerp */
};

struct marpaESLIFGrammar {
  marpaESLIF_t             *marpaESLIFp;
  marpaESLIFGrammarOption_t marpaESLIFGrammarOption;
  marpaESLIF_grammar_t     *grammarp;
};

marpaESLIFGrammarOption_t marpaESLIFGrammarOption_default = {
  NULL, /* grammarcp */
  0,    /* grammarl */
  NULL  /* encodings */
};

struct marpaESLIFRecognizer {
  marpaESLIF_t                *marpaESLIFp;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaWrapperRecognizer_t    *marpaWrapperRecognizerp;
};

marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption_default = {
  NULL,              /* userDatavp */
  NULL,              /* marpaESLIFReaderCallbackp */
  0                  /* disableThresholdb */
};

typedef struct marpaESLIFReaderGrammarReaderContext {
  marpaESLIF_t *marpaESLIFp;
  char         *utf8s;
  size_t        utf8l;
  char         *p;
} marpaESLIFReaderGrammarReaderContext_t;

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, PCRE2_SPTR substitutionp, PCRE2_SIZE substitutionl, char *testFullMatchs, char *testPartialMatchs);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_meta_t     *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl);
static inline void                   _marpaESLIF_meta_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_meta_t *metap);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, unsigned int leveli, marpaESLIF_grammar_t *previousp);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp);

static inline genericStack_t        *_marpaESLIF_ruleStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_ruleStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *ruleStackp);

static inline marpaESLIF_rule_t     *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_symbol_t *symbolp);

static inline genericStack_t        *_marpaESLIF_symbolStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbolStack_freev(marpaESLIF_t *marpaESLIFp, genericStack_t *symbolStackp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
								   unsigned int leveli,
								   short warningIsErrorb,
								   short warningIsIgnoredb,
								   short autorankb,
								   int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
								   int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
								   int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep);
static inline short                  _marpaESLIF_validate_grammarb(marpaESLIF_t *marpaESLIFp);

static inline short                  _marpaESLIF_terminal_string_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, marpaESLIF_terminal_t *terminalp, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp);
static inline short                  _marpaESLIF_terminal_regex_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, marpaESLIF_terminal_t *terminalp, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp);
static inline short                  _marpaESLIF_meta_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, marpaESLIF_terminal_t *terminalp, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp);

const static  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *utf8lp);
static inline void                   _marpaESLIF_tconv_freev(marpaESLIF_t *marpaESLIFp, char *utf8s);

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(marpaESLIF_t *marpaESLIFp, char *utf82printableasciip);
static        short                 _marpaESLIFReader_grammarReader(void *userDatavp, char **inputcpp, size_t *inputlp);

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, PCRE2_SPTR substitutionp, PCRE2_SIZE substitutionl, char *testFullMatchs, char *testPartialMatchs)
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
  terminalp->descl        = 0;
  terminalp->asciidescs   = NULL;
  terminalp->type         = MARPAESLIF_TERMINAL_TYPE_NA;
  terminalp->matcherip    = NULL;

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
  terminalp->descl = descl;

  /* --------- Terminal ASCII Description -------- */
  terminalp->asciidescs = _marpaESLIF_utf82printableascii_newp(marpaESLIFp, descs, descl);

  /* ----------- Terminal Implementation ------------ */
  terminalp->type = type;
  switch (type) {

  case MARPAESLIF_TERMINAL_TYPE_STRING:
    terminalp->u.string.stringp = NULL;
    terminalp->u.string.stringl = 0;
    terminalp->matcherip = _marpaESLIF_terminal_string_matcheri;
    
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - invalid terminal origin", terminalp->asciidescs);
      goto err;
    }
    terminalp->u.string.stringp = (char *) malloc((size_t) originl);
    if (terminalp->u.string.stringp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
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
    terminalp->matcherip = _marpaESLIF_terminal_regex_matcheri;
    
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - invalid terminal origin", terminalp->asciidescs);
      goto err;
    }
    if ((substitutionp != NULL) && (substitutionl > 0)) {
      terminalp->u.regex.substitutionp = (PCRE2_SPTR) malloc((size_t) substitutionl);
      if (terminalp->u.regex.substitutionp == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
	goto err;
      }
      memcpy((void *) terminalp->u.regex.substitutionp, substitutionp, substitutionl);
      terminalp->u.regex.substitutionl = substitutionl;
    }
    pcre2Optioni = PCRE2_ANCHORED|PCRE2_UTF;      /* By default patterns are always anchored and in UTF mode - only UTF mode can be removed via the 'a' modifier */
    for (i = 0; i < _MARPAESLIF_REGEX_OPTION_ID_MAX; i++) {
      if ((opti & marpaESLIF_regex_option_map[i].opti) == marpaESLIF_regex_option_map[i].opti) {
	/* It is important to process pcre2OptionNoti first */
	if (marpaESLIF_regex_option_map[i].pcre2OptionNoti != 0) {
	  pcre2Optioni &= ~marpaESLIF_regex_option_map[i].pcre2OptionNoti;
	}
	if (marpaESLIF_regex_option_map[i].pcre2Optioni != 0) {
	  pcre2Optioni |= marpaESLIF_regex_option_map[i].pcre2Optioni;
	}
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
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_compile failure at offset %ld: %s", terminalp->asciidescs, (unsigned long) pcre2ErrorOffsetl, pcre2ErrorBuffer);
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
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_match_data_create_from_pattern failure, %s", terminalp->asciidescs, strerror(errno));
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
    MARPAESLIF_ERRORF(marpaESLIFp, "%s - unsupported terminal type %d", terminalp->asciidescs, type);
    goto err;
    break;
  }

#ifndef MARPAESLIF_NTRACE
  if (testFullMatchs != NULL) {
    marpaESLIF_matcher_value_t rci;
    char                      *outputp;
    size_t                     outputl;

    if (! terminalp->matcherip(marpaESLIFp, marpaESLIFGrammarp, terminalp, testFullMatchs, strlen(testFullMatchs), 1, &rci, &outputp, &outputl)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher general failure", terminalp->asciidescs);
      goto err;
    }
    if (rci != MARPAESLIF_MATCH_OK) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher returned rci = %d", terminalp->asciidescs, rci);
      goto err;
    }
    if (outputp == NULL) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing full match is successful on %s but result is NULL", terminalp->asciidescs, testFullMatchs);
      goto err;
    }
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing full match is successful on %s, result is %s", terminalp->asciidescs, testFullMatchs, outputp);
    free(outputp);
  }
  if (testPartialMatchs != NULL) {
    marpaESLIF_matcher_value_t rci;
    char                      *outputp;
    size_t                     outputl;

    if (! terminalp->matcherip(marpaESLIFp, marpaESLIFGrammarp, terminalp, testPartialMatchs, strlen(testPartialMatchs), 0, &rci, &outputp, &outputl)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher general failure", terminalp->asciidescs);
      goto err;
    }
    if (rci != MARPAESLIF_MATCH_AGAIN) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher returned rci = %d", terminalp->asciidescs, rci);
      goto err;
    }
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing partial match is successful on %s when not at EOF", terminalp->asciidescs, testPartialMatchs);
  }
#endif
  goto done;
  
 err:
  _marpaESLIF_terminal_freev(marpaESLIFp, terminalp);
  terminalp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", terminalp);
  return terminalp;
}

/*****************************************************************************/
static inline marpaESLIF_meta_t *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_meta_newp";
  marpaESLIF_meta_t                *metap;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building meta");

  metap = (marpaESLIF_meta_t *) malloc(sizeof(marpaESLIF_meta_t));
  if (metap == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  metap->idi                       = -1;
  metap->descs                     = NULL;
  metap->descl                     = 0;
  metap->asciidescs                = NULL;
  metap->matcherip                 = NULL;
  metap->marpaWrapperGrammarClonep = NULL; /* Eventuallwy changed when validating the grammar */

  marpaWrapperGrammarSymbolOption.terminalb = 0;
  marpaWrapperGrammarSymbolOption.startb    = startb;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Meta Identifier ------------ */
  metap->idi = marpaWrapperGrammar_newSymboli(marpaESLIFGrammarp->marpaWrapperGrammarp, &marpaWrapperGrammarSymbolOption);
  if (metap->idi < 0) {
    goto err;
  }

  /* -------- Meta UTF-8 Description -------- */
  if (descs == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No meta description");
    goto err;
  }
  metap->descs = (char *) malloc(descl);
  if (metap->descs == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }
  memcpy(metap->descs, descs, descl);
  metap->descl = descl;

  /* --------- Meta ASCII Description -------- */
  metap->asciidescs = _marpaESLIF_utf82printableascii_newp(marpaESLIFp, descs, descl);

  goto done;

 err:
  _marpaESLIF_meta_freev(marpaESLIFp, metap);
  metap = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", metap);
  return metap;
}

/*****************************************************************************/
static inline void _marpaESLIF_meta_freev(marpaESLIF_t *marpaESLIFp, marpaESLIF_meta_t *metap)
/*****************************************************************************/
{
  const static char    *funcs = "_marpaESLIF_meta_freev";

  if (metap != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing meta %s at %p", metap->asciidescs != NULL ? metap->asciidescs : "(null)", metap);
    if (metap->descs != NULL) {
      free(metap->descs);
    }
    if (metap->marpaWrapperGrammarClonep != NULL) {
      marpaWrapperGrammar_freev(metap->marpaWrapperGrammarClonep);
    }
    _marpaESLIF_utf82printableascii_freev(marpaESLIFp, metap->asciidescs);
    /* All the rest are shallow pointers */
    free(metap);
  }
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_grammarp(marpaESLIFp,
					1, /* L0 in Marpa::R2 terminology is level No 1 for us */
					0, /* warningIsErrorb */
					1, /* warningIsIgnoredb */
					0, /* autorankb */
					sizeof(bootstrap_grammar_L0_terminals) / sizeof(bootstrap_grammar_L0_terminals[0]),
					bootstrap_grammar_L0_terminals,
					sizeof(bootstrap_grammar_L0_metas) / sizeof(bootstrap_grammar_L0_metas[0]),
					bootstrap_grammar_L0_metas,
					sizeof(bootstrap_grammar_L0_rules) / sizeof(bootstrap_grammar_L0_rules[0]),
					bootstrap_grammar_L0_rules);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  return _marpaESLIF_bootstrap_grammarp(marpaESLIFp,
					0, /* G1 in Marpa::R2 terminology is level No 0 for us */
					0, /* warningIsErrorb */
					1, /* warningIsIgnoredb */
					0, /* autorankb */
					sizeof(bootstrap_grammar_G1_terminals) / sizeof(bootstrap_grammar_G1_terminals[0]),
					bootstrap_grammar_G1_terminals,
					sizeof(bootstrap_grammar_G1_metas) / sizeof(bootstrap_grammar_G1_metas[0]),
					bootstrap_grammar_G1_metas,
					sizeof(bootstrap_grammar_G1_rules) / sizeof(bootstrap_grammar_G1_rules[0]),
					bootstrap_grammar_G1_rules);
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
								   unsigned int leveli,
								   short warningIsErrorb,
								   short warningIsIgnoredb,
								   short autorankb,
								   int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
								   int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
								   int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep)
/*****************************************************************************/
{
  const static char          *funcs     = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_symbol_t        *symbolp   = NULL;
  marpaESLIF_rule_t          *rulep     = NULL;
  marpaESLIF_terminal_t      *terminalp = NULL;
  marpaESLIF_meta_t          *metap     = NULL;
  marpaESLIF_grammar_t       *marpaESLIFGrammarp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  int                         i;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Bootstrapping grammar at level %d", (int) leveli);

  marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->option.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = warningIsErrorb;
  marpaWrapperGrammarOption.warningIsIgnoredb = warningIsIgnoredb;
  marpaWrapperGrammarOption.autorankb         = autorankb;
  
  marpaESLIFGrammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, leveli, NULL);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }

  /* First the terminals */
  for (i = 0; i < bootstrap_grammar_terminali; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }

    terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
					  marpaESLIFGrammarp,
					  0, /* startb */
					  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
					  bootstrap_grammar_terminalp[i].descs,
					  strlen(bootstrap_grammar_terminalp[i].descs) + 1,  /* Bootstrap's descs is a C string */
					  bootstrap_grammar_terminalp[i].terminalType,
					  bootstrap_grammar_terminalp[i].optioni,
					  bootstrap_grammar_terminalp[i].originp,
					  (bootstrap_grammar_terminalp[i].originp != NULL) ? strlen(bootstrap_grammar_terminalp[i].originp) : 0,
					  bootstrap_grammar_terminalp[i].substitutionp,
					  (bootstrap_grammar_terminalp[i].substitutionp != NULL) ? strlen(bootstrap_grammar_terminalp[i].substitutionp) : 0,
					  bootstrap_grammar_terminalp[i].testFullMatchs,
					  bootstrap_grammar_terminalp[i].testPartialMatchs
					  );
    if (terminalp == NULL) {
      goto err;
    }
    /* When bootstrapping the grammar, we expect terminal IDs to be exactly the value of the enum */
    if (terminalp->idi != bootstrap_grammar_terminalp[i].idi) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Got symbol ID %d from Marpa while we were expecting %d", terminalp->idi, bootstrap_grammar_terminalp[i].idi);
      goto err;
    }

    symbolp->type        = MARPAESLIF_SYMBOL_TYPE_TERMINAL;
    symbolp->u.terminalp = terminalp;
    symbolp->idi         = terminalp->idi;
    symbolp->descs       = terminalp->descs;
    symbolp->descl       = terminalp->descl;
    symbolp->asciidescs  = terminalp->asciidescs;
    symbolp->matcherip   = terminalp->matcherip;
    /* Terminal is now in symbol */
    terminalp = NULL;

    GENERICSTACK_SET_PTR(marpaESLIFGrammarp->symbolStackp, symbolp, symbolp->u.terminalp->idi);
    if (GENERICSTACK_ERROR(marpaESLIFGrammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in marpaESLIFGrammarp->symbolStackp */
    symbolp = NULL;
  }

  /* Then the non-terminals */
  for (i = 0; i < bootstrap_grammar_metai; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }

    metap = _marpaESLIF_meta_newp(marpaESLIFp,
				  marpaESLIFGrammarp,
				  0, /* startb */
				  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
				  bootstrap_grammar_metap[i].descs,
				  strlen(bootstrap_grammar_metap[i].descs) + 1 /* Bootstrap's descs is a C string */
				  );
    if (metap == NULL) {
      goto err;
    }
    /* When bootstrapping the grammar, we expect meta IDs to be exactly the value of the enum */
    if (metap->idi != bootstrap_grammar_metap[i].idi) {
      MARPAESLIF_ERRORF(marpaESLIFp, "Got symbol ID %d from Marpa while we were expecting %d", metap->idi, bootstrap_grammar_metap[i].idi);
      goto err;
    }

    symbolp->type       = MARPAESLIF_SYMBOL_TYPE_META;
    symbolp->u.metap    = metap;
    symbolp->idi        = metap->idi;
    symbolp->descs      = metap->descs;
    symbolp->descl      = metap->descl;
    symbolp->asciidescs = metap->asciidescs;
    symbolp->matcherip  = metap->matcherip;
    /* Terminal is now in symbol */
    metap = NULL;

    GENERICSTACK_SET_PTR(marpaESLIFGrammarp->symbolStackp, symbolp, symbolp->u.metap->idi);
    if (GENERICSTACK_ERROR(marpaESLIFGrammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in marpaESLIFGrammarp->symbolStackp */
    symbolp = NULL;
  }

  /* Then the rules */
  for (i = 0; i < bootstrap_grammar_rulei; i++) {
    rulep = _marpaESLIF_rule_newp(marpaESLIFp,
				  marpaESLIFGrammarp,
				  bootstrap_grammar_rulep[i].descs,
				  strlen(bootstrap_grammar_rulep[i].descs) + 1, /* Bootstrap's descs is a C string */
				  bootstrap_grammar_rulep[i].lhsi,
				  bootstrap_grammar_rulep[i].nrhsl,
				  bootstrap_grammar_rulep[i].rhsip,
				  (leveli == 0) ? bootstrap_grammar_rulep[i].maskbp : NULL, /* Masks are enabled only at the very top level */
				  0, /* nexceptionl */
				  NULL, /* exceptionip */
				  0, /* ranki */
				  0, /* nullRanksHighb */
				  (bootstrap_grammar_rulep[i].type == MARPAESLIF_RULE_TYPE_ALTERNATIVE) ? 0 : 1, /* sequenceb */
				  bootstrap_grammar_rulep[i].minimumi,
				  bootstrap_grammar_rulep[i].separatori,
				  bootstrap_grammar_rulep[i].properb
				  );
    if (rulep == NULL) {
      goto err;
    }

    GENERICSTACK_SET_PTR(marpaESLIFGrammarp->ruleStackp, rulep, rulep->idi);
    if (GENERICSTACK_ERROR(marpaESLIFGrammarp->ruleStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: rulep is in marpaESLIFGrammarp->ruleStackp */
    rulep = NULL;
  }

  goto done;
  
 err:
  _marpaESLIF_terminal_freev(marpaESLIFp, terminalp);
  _marpaESLIF_meta_freev(marpaESLIFp, metap);
  _marpaESLIF_rule_freev(marpaESLIFp, rulep);
  _marpaESLIF_symbol_freev(marpaESLIFp, symbolp);
  _marpaESLIF_grammar_freev(marpaESLIFp, marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp);
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
static inline short _marpaESLIF_validate_grammarb(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char     *funcs                     = "_marpaESLIF_validate_grammarb";
  genericStack_t        *grammarStackp             = marpaESLIFp->grammarStackp;
  marpaWrapperGrammar_t *marpaWrapperGrammarClonep = NULL;
  marpaESLIF_meta_t     *metap;
  genericStack_t        *symbolStackp;
  genericStack_t        *ruleStackp;
  int                    grammari;
  marpaESLIF_symbol_t   *symbolp;
  int                    symboli;
  marpaESLIF_rule_t     *rulep;
  int                    rulei;
  marpaESLIF_grammar_t  *grammarp;
  marpaESLIF_grammar_t  *nextGrammarp;
  short                  isLhsb;
  marpaESLIF_symbol_t   *lhsp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Validating ESLIF grammar");

  /* The rules are:

   - There must be a grammar at level 0
   - At any grammar level n, if a symbol never appear as an LHS of a rule, then
     it must be an LHS of grammar at level n+1, which must de-factor must exist
     
     It is not illegal to have sparse items in grammarStackp, provided that until the
     hole is reached, the whole is valid. Grammars after the hole are ignored per def.
  */
  if (! GENERICSTACK_IS_PTR(grammarStackp, 0)) {
    MARPAESLIF_ERROR(marpaESLIFp, "No top-level grammar");
    goto err;
  }

  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    /* De-facto, we always look at grammar level 0 at least */
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at grammar level %d", grammari);

    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    nextGrammarp = NULL;

    /* Loop on symbols */
    symbolStackp = grammarp->symbolStackp;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      /* Only meta symbols should be looked at: if not an LHS then it is a dependency on a LHS of a sub-grammar */
      if ((symbolp->type == MARPAESLIF_SYMBOL_TYPE_META) && (! symbolp->isLhsb)) {
        metap = symbolp->u.metap;
        if (nextGrammarp == NULL) {
          if (! GENERICSTACK_IS_PTR(grammarStackp, grammari+1)) {
            MARPAESLIF_ERRORF(marpaESLIFp, "Symbol %s at grammar level %d need a grammar definition at level %d", symbolp->asciidescs, grammari, grammari + 1);
            goto err;
          }
          nextGrammarp = GENERICSTACK_GET_PTR(grammarStackp, grammari+1);
        }
        ruleStackp = nextGrammarp->ruleStackp;
        isLhsb = 0;
        for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
          if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
            /* Should never happen, but who knows */
            continue;
          }
          rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
          lhsp = rulep->lhsp;
          if (lhsp->descl == symbolp->descl) {
            if (memcmp(lhsp->descs, symbolp->descs, symbolp->descl) == 0) {
              /* Found */
              isLhsb = 1;
              break;
            }
          }
        }
        if (! isLhsb) {
          MARPAESLIF_ERRORF(marpaESLIFp, "Symbol %s at grammar level %d need to be an LHS symbol in grammar at level %d", symbolp->asciidescs, grammari, grammari + 1);
          goto err;
        } else {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Cloning and precomputing grammar at level %d with start symbol %s", grammari + 1, symbolp->asciidescs);
          marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(nextGrammarp->marpaWrapperGrammarp);
          if (marpaWrapperGrammarClonep == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFp, "Failure to clone grammar at level %d", grammari + 1);
            goto err;
          }
          if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, lhsp->idi)) {
            MARPAESLIF_ERRORF(marpaESLIFp, "Failure to precompute grammar at level %d with start symbol %s", grammari + 1, symbolp->asciidescs);
            goto err;
          }
          metap->marpaWrapperGrammarClonep = marpaWrapperGrammarClonep;
        }
      }
    }
    if (nextGrammarp == NULL) {
      /* No need of a grammar next, whatever its level */
      break;
    }
  }

  goto done;
  
 err:
  marpaWrapperGrammar_freev(marpaWrapperGrammarClonep);
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 0");
  return 0;

 done:
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 1");
  return 1;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, unsigned int leveli, marpaESLIF_grammar_t *previousp)
/*****************************************************************************/
{
  const static char    *funcs    = "_marpaESLIF_grammar_newp";
  marpaESLIF_grammar_t *grammarp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building ESLIF grammar");

  grammarp = (marpaESLIF_grammar_t *) malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->marpaESLIFp          = marpaESLIFp;
  grammarp->leveli               = leveli;
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
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing rule stack at %p", ruleStackp);
    while (GENERICSTACK_USED(ruleStackp) > 0) {
      if (GENERICSTACK_IS_PTR(ruleStackp, GENERICSTACK_USED(ruleStackp) - 1)) {
	marpaESLIF_rule_t *rulep = (marpaESLIF_rule_t *) GENERICSTACK_POP_PTR(ruleStackp);
	_marpaESLIF_rule_freev(marpaESLIFp, rulep);
      } else {
	GENERICSTACK_USED(ruleStackp)--;
      }
    }
    GENERICSTACK_FREE(ruleStackp);
  }
}

/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb)
/*****************************************************************************/
{
  const static char               *funcs        = "_marpaESLIF_rule_newp";
  genericStack_t                  *symbolStackp = marpaESLIFGrammarp->symbolStackp;
  short                            symbolFoundb = 0;
  marpaESLIF_symbol_t             *symbolp;
  marpaESLIF_rule_t               *rulep;
  marpaWrapperGrammarRuleOption_t  marpaWrapperGrammarRuleOption;
  size_t                           i;
  short                            maskb;
  int                              symboli;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building rule");

  rulep = (marpaESLIF_rule_t *) malloc(sizeof(marpaESLIF_rule_t));
  if (rulep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  rulep->idi             = -1;
  rulep->descs           = NULL;
  rulep->descl           = 0;
  rulep->asciidescs      = NULL;
  rulep->lhsp            = NULL;
  rulep->rhsStackp       = NULL;
  rulep->maskStackp      = NULL;
  rulep->exceptionStackp = NULL;

  /* -------- Meta UTF-8 Description -------- */
  if (descs == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No rule description");
    goto err;
  }
  rulep->descs = (char *) malloc(descl);
  if (rulep->descs == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "strdup failure, %s", strerror(errno));
    goto err;
  }
  memcpy(rulep->descs, descs, descl);
  rulep->descl = descl;

  /* --------- Meta ASCII Description -------- */
  rulep->asciidescs = _marpaESLIF_utf82printableascii_newp(marpaESLIFp, descs, descl);

  /* Look to the symbol itself, and remember it is an LHS - this is used when validating the grammar */
  for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      /* Should never happen, but who knows */
      continue;
    }
    symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      symbolFoundb = (symbolp->u.terminalp->idi == lhsi);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      symbolFoundb = (symbolp->u.metap->idi == lhsi);
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS symbols is of type N/A", marpaESLIFGrammarp->leveli, rulep->asciidescs, symbolp->type);
      goto err;
    }
    if (symbolFoundb) {
      break;
    }
  }
  if (! symbolFoundb) {
    MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS symbol does not exist", marpaESLIFGrammarp->leveli, rulep->asciidescs, lhsi);
    goto err;
  }
  symbolp->isLhsb = 1;
  rulep->lhsp = symbolp;

  GENERICSTACK_NEW(rulep->rhsStackp);
  if (GENERICSTACK_ERROR(rulep->rhsStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(rulep->maskStackp);
  if (GENERICSTACK_ERROR(rulep->maskStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "maskStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  GENERICSTACK_NEW(rulep->exceptionStackp);
  if (GENERICSTACK_ERROR(rulep->exceptionStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "exceptionStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* Fill rhs symbol stack */
  for (i = 0; i < nrhsl; i++) {
    if (! GENERICSTACK_IS_PTR(marpaESLIFGrammarp->symbolStackp, rhsip[i])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: No such RHS symbol No %d", marpaESLIFGrammarp->leveli, rulep->asciidescs, rhsip[i]);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rulep->rhsStackp, GENERICSTACK_GET_PTR(marpaESLIFGrammarp->symbolStackp, rhsip[i]));
    if (GENERICSTACK_ERROR(rulep->rhsStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "rhsStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* It is ASSUMED that the caller allocated as many items as there are RHSs */
    maskb = (maskbp != NULL) ? maskbp[i] : 0;
    GENERICSTACK_PUSH_SHORT(rulep->maskStackp, maskb);
    if (GENERICSTACK_ERROR(rulep->maskStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "maskStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  
  /* Fill exception symbol stack */
  for (i = 0; i < nexceptionl; i++) {
    if (! GENERICSTACK_IS_PTR(marpaESLIFGrammarp->symbolStackp, exceptionip[i])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: No such RHS exception symbol No %d", marpaESLIFGrammarp->leveli, rulep->asciidescs, exceptionip[i]);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rulep->exceptionStackp, GENERICSTACK_GET_PTR(marpaESLIFGrammarp->symbolStackp, exceptionip[i]));
    if (GENERICSTACK_ERROR(rulep->exceptionStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "exceptionStackp push failure, %s", strerror(errno));
      goto err;
    }
  }
  
  marpaWrapperGrammarRuleOption.ranki            = ranki;
  marpaWrapperGrammarRuleOption.nullRanksHighb   = nullRanksHighb;
  marpaWrapperGrammarRuleOption.sequenceb        = sequenceb;
  marpaWrapperGrammarRuleOption.separatorSymboli = separatori;
  marpaWrapperGrammarRuleOption.properb          = properb;
  marpaWrapperGrammarRuleOption.minimumi         = minimumi;

  /* ----------- Meta Identifier ------------ */
  rulep->idi = marpaWrapperGrammar_newRulei(marpaESLIFGrammarp->marpaWrapperGrammarp, &marpaWrapperGrammarRuleOption, lhsi, nrhsl, rhsip);
  if (rulep->idi < 0) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_rule_freev(marpaESLIFp, rulep);
  rulep = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rulep);
  return rulep;
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
    /* In the rule structure, lhsp, rhsStackp and exceptionStackp contain shallow pointers */
    /* Only the stack themselves should be freed. */
    /*
    _marpaESLIF_symbol_freev(marpaESLIFp, rulep->lhsp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, rulep->rhsStackp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, rulep->exceptionStackp);
    */
    GENERICSTACK_FREE(rulep->maskStackp);
    GENERICSTACK_FREE(rulep->rhsStackp);
    GENERICSTACK_FREE(rulep->exceptionStackp);
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

  symbolp->type   = MARPAESLIF_SYMBOL_TYPE_NA;
  /* Union itself is undetermined at this stage */
  symbolp->isLhsb = 0;
  symbolp->idi   =  -1;
  symbolp->descs =  NULL;
  symbolp->descl =  0;
  symbolp->asciidescs =  NULL;

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
    /* All pointers are the top level of this structure are shallow pointers */
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing symbol at %p", symbolp);
    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      _marpaESLIF_terminal_freev(marpaESLIFp, symbolp->u.terminalp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      _marpaESLIF_meta_freev(marpaESLIFp, symbolp->u.metap);
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
      } else {
	GENERICSTACK_USED(symbolStackp)--;
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
  const static char    *funcs              = "marpaESLIF_newp";
  marpaESLIF_grammar_t *marpaESLIFGrammarp = NULL;
  marpaESLIF_t         *marpaESLIFp;
  genericLogger_t      *genericLoggerp;

  if (marpaESLIFOptionp == NULL) {
    marpaESLIFOptionp = &marpaESLIFOption_default;
  }

  genericLoggerp = marpaESLIFOptionp->genericLoggerp;

#ifndef MARPAESLIF_NTRACE
  if (genericLoggerp != NULL) {
    GENERICLOGGER_TRACEF(genericLoggerp, "[%s] Building ESLIF", funcs);
  }
#endif

  marpaESLIFp = (marpaESLIF_t *) malloc(sizeof(marpaESLIF_t));
  if (marpaESLIFp == NULL) {
    if (marpaESLIFOptionp->genericLoggerp != NULL) {
      GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "malloc failure, %s", strerror(errno));
      goto err;
    }
  }

  /* Create internal ESLIF grammar - it is important to set the option first */
  marpaESLIFp->option = *marpaESLIFOptionp;
  marpaESLIFp->grammarStackp = NULL;

  GENERICSTACK_NEW(marpaESLIFp->grammarStackp);
  if (GENERICSTACK_ERROR(marpaESLIFp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "grammarStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* L0 */
  marpaESLIFGrammarp = _marpaESLIF_bootstrap_grammar_L0p(marpaESLIFp);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->grammarStackp, marpaESLIFGrammarp, marpaESLIFGrammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  marpaESLIFGrammarp = NULL;  /* It is in the stack of grammars */

  /* G1 */
  marpaESLIFGrammarp = _marpaESLIF_bootstrap_grammar_G1p(marpaESLIFp);
  if (marpaESLIFGrammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->grammarStackp, marpaESLIFGrammarp, marpaESLIFGrammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  marpaESLIFGrammarp = NULL; /* Ditto */

  /* Validate all the grammars together */
  if (! _marpaESLIF_validate_grammarb(marpaESLIFp)) {
    goto err;
  }
  goto done;
  
 err:
  _marpaESLIF_grammar_freev(marpaESLIFp, marpaESLIFGrammarp);
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
  const static char    *funcs = "marpaESLIF_freev";

  if (marpaESLIFp != NULL) {
    if (marpaESLIFp->grammarStackp != NULL) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing grammar stack at %p", marpaESLIFp->grammarStackp);
      while (GENERICSTACK_USED(marpaESLIFp->grammarStackp) > 0) {
	if (GENERICSTACK_IS_PTR(marpaESLIFp->grammarStackp, GENERICSTACK_USED(marpaESLIFp->grammarStackp) - 1)) {
	  marpaESLIF_grammar_t *grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_POP_PTR(marpaESLIFp->grammarStackp);
	  _marpaESLIF_grammar_freev(marpaESLIFp, grammarp);
	} else {
	  GENERICSTACK_USED(marpaESLIFp->grammarStackp)--;
	}
      }
      GENERICSTACK_FREE(marpaESLIFp->grammarStackp);
    }
    free(marpaESLIFp);
  }
}

/*****************************************************************************/
static inline short _marpaESLIF_terminal_string_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, marpaESLIF_terminal_t *terminalp, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  const static char         *funcs             = "_marpaESLIF_matcher";
  short                      rcb               = 1;
  marpaESLIF_matcher_value_t rci;
  marpaESLIF_string_t        marpaESLIF_string;
  size_t                     matchLengthl;
 
  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*********************************************************************************/

  if (inputl > 0) {

    marpaESLIF_string = terminalp->u.string;
    if (inputl >= marpaESLIF_string.stringl) {
      if (memcmp(inputcp, marpaESLIF_string.stringp, marpaESLIF_string.stringl) == 0) {
        rci = MARPAESLIF_MATCH_OK;
        matchLengthl = marpaESLIF_string.stringl;
      } else {
        rci = MARPAESLIF_MATCH_FAILURE;
      }
    } else {
      /* Partial match never returns OK -; */
      rci = (memcmp(inputcp, marpaESLIF_string.stringp, inputl) == 0) ? (eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN) : MARPAESLIF_MATCH_FAILURE;
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
      *outputlp = matchLengthl;
    }
    if (outputpp != NULL) {
      char *outputp = (char *) malloc(matchLengthl + 1);
      /* Matched input is starting at inputcp and its length is in matchLengthl */
      if (outputp == NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
        goto err;
      }
      memcpy(outputp, inputcp, matchLengthl);
      outputp[matchLengthl] = '\0'; /* Trailing zero in any case (even if there CAN BE other zeroes before) */
      *outputpp = outputp;
    }
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - return 1, status is %s", terminalp->asciidescs, (rci == MARPAESLIF_MATCH_FAILURE) ? "MARPAESLIF_MATCH_FAILURE" : ((rci == MARPAESLIF_MATCH_OK) ? "MARPAESLIF_MATCH_OK" : "MARPAESLIF_MATCH_AGAIN"));
  goto done;

 err:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - return 0", terminalp->asciidescs);
  rcb = 0;

 done:
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIF_terminal_regex_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, marpaESLIF_terminal_t *terminalp, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  const static char         *funcs             = "_marpaESLIF_matcher";
  short                      rcb               = 1;
  PCRE2_UCHAR               *pcre2_substitutep = NULL;
  PCRE2_SIZE                 pcre2_substitutel = MARPAESLIF_INITIAL_REPLACEMENT_LENGTH; /* In code units, not bytes, though in UTF-8 a code unit is one byte -; */
  marpaESLIF_matcher_value_t rci;
  marpaESLIF_string_t        marpaESLIF_string;
  marpaESLIF_regex_t         marpaESLIF_regex;
  int                        pcre2Errornumberi;
  PCRE2_UCHAR                pcre2ErrorBuffer[256];
  PCRE2_SIZE                *pcre2_ovectorp;
  size_t                     matchLengthl;
  PCRE2_SIZE                 pcre2_substituteoutputl;
  PCRE2_UCHAR               *tmpp;
  PCRE2_SIZE                 tmpl;
 
  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*********************************************************************************/

  if (inputl > 0) {

    marpaESLIF_regex = terminalp->u.regex;

    /* If there is substitution, prepare memory. */
    if (marpaESLIF_regex.substitutionp != NULL) {
      pcre2_substitutep = (PCRE2_UCHAR *) malloc(pcre2_substitutel);
      if (pcre2_substitutep == NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
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
                                             (PCRE2_SPTR) inputcp,            /* subject */
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
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - pcre2_substitute failure: %s - switching to non-JIT version", terminalp->asciidescs, pcre2ErrorBuffer);
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
            MARPAESLIF_ERRORF(marpaESLIFp, "%s - PCRE2_SIZE overflow detected", terminalp->asciidescs);
            goto err;
          }
          tmpp = realloc(pcre2_substitutep, pcre2_substituteoutputl);
          if (tmpp == NULL) {
            MARPAESLIF_ERRORF(marpaESLIFp, "%s - realloc failure, %s", terminalp->asciidescs, strerror(errno));
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
            /* We try to release non-needed memory. realloc() failure here is not fatal. */
            /* Please note that pcre2_substituteoutputl exclude the trailing zero, so we */
            /* are sure that pcre2_substituteoutputl+1 number of ok, as per the doc.     */
            tmpl = pcre2_substituteoutputl + 1;
            tmpp = realloc(pcre2_substitutep, tmpl);
            if (tmpp == NULL) {
              MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - realloc failure (non fatal), %s", terminalp->asciidescs, strerror(errno));
            } else {
              pcre2_substitutep = tmpp;
              pcre2_substitutel = tmpl;
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
                                            (PCRE2_SPTR) inputcp,          /* subject */
                                            (PCRE2_SIZE) inputl,          /* length */
                                            (PCRE2_SIZE) 0,               /* startoffset */
                                            PCRE2_NOTEMPTY,               /* options - this one is supported in JIT mode */
                                            marpaESLIF_regex.match_datap, /* match data */
                                            NULL                          /* match context - used default */
                                            );
        if (pcre2Errornumberi == PCRE2_ERROR_JIT_STACKLIMIT) {
          /* Back luck, out of stack for JIT */
          pcre2_get_error_message(pcre2Errornumberi, pcre2ErrorBuffer, sizeof(pcre2ErrorBuffer));
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - pcre2_jit_match failure: %s - switching to non-JIT version", terminalp->asciidescs, pcre2ErrorBuffer);
          goto eof_nojitcomplete;
        }
      } else {
      eof_nojitcomplete:
#endif
        pcre2Errornumberi = pcre2_match(marpaESLIF_regex.patternp,    /* code */
                                        (PCRE2_SPTR) inputcp,          /* subject */
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
          MARPAESLIF_WARNF(marpaESLIFp, "%s - Uncaught pcre2 match failure: %s", terminalp->asciidescs, pcre2ErrorBuffer);
        }
        rci = MARPAESLIF_MATCH_FAILURE;
      } else {
        /* Check the length of matched data */
        if (pcre2_get_ovector_count(marpaESLIF_regex.match_datap) <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_count returned no number of pairs of values", terminalp->asciidescs);
          goto err;
        }
        pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
        if (pcre2_ovectorp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_pointer returned NULL", terminalp->asciidescs);
          goto err;
        }
        /* We said PCRE2_NOTEMPTY so this cannot be empty */
        matchLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - Empty match when it is configured as not possible", terminalp->asciidescs);
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
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_count returned no number of pairs of values", terminalp->asciidescs);
          goto err;
        }
        pcre2_ovectorp = pcre2_get_ovector_pointer(marpaESLIF_regex.match_datap);
        if (pcre2_ovectorp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - pcre2_get_ovector_pointer returned NULL", terminalp->asciidescs);
          goto err;
        }
        /* We said PCRE2_NOTEMPTY so this cannot be empty */
        matchLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - Empty match when it is configured as not possible", terminalp->asciidescs);
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
                                              (PCRE2_SPTR) inputcp,          /* subject */
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
            MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - pcre2_jit_match failure: %s - switching to non-JIT version", terminalp->asciidescs, pcre2ErrorBuffer);
            goto eof_nojitpartial;
          }
        } else {
        eof_nojitpartial:
#endif
          pcre2Errornumberi = pcre2_match(marpaESLIF_regex.patternp,    /* code */
                                          (PCRE2_SPTR) inputcp,          /* subject */
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
      /* When there is a substitution result, memory was already allocated, trailing zero included */
      if (pcre2_substitutep != NULL) {
	*outputpp = pcre2_substitutep;
	/* Prevent a memory free at the end of this routine */
	pcre2_substitutep = NULL;
      } else {
	char *outputp = (char *) malloc(matchLengthl + 1);
	/* Matched input is starting at inputcp and its length is in matchLengthl */
	if (outputp == NULL) {
	  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
	  goto err;
	}
	memcpy(outputp, inputcp, matchLengthl);
	outputp[matchLengthl] = '\0'; /* Trailing zero in any case (even if there CAN BE other zeroes before) */
	*outputpp = outputp;
      }
    }
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - return 1, status is %s", terminalp->asciidescs, (rci == MARPAESLIF_MATCH_FAILURE) ? "MARPAESLIF_MATCH_FAILURE" : ((rci == MARPAESLIF_MATCH_OK) ? "MARPAESLIF_MATCH_OK" : "MARPAESLIF_MATCH_AGAIN"));
  goto done;

 err:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - return 0", terminalp->asciidescs);
  rcb = 0;

 done:
  /* In any case release substitution result */
  if (pcre2_substitutep != NULL) {
    free((void *) pcre2_substitutep);
  }
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIF_meta_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, marpaESLIF_terminal_t *terminalp, char *inputcp, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  /* All in all, this routine is the core of this module -; */
  const static char *funcs = "_marpaESLIF_meta_matcheri";
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
  char              *p;
  char              *utf8s;
  size_t             utf8l;
  unsigned char      c;

  utf8s = _marpaESLIF_tconv_newp(marpaESLIFp, "ASCII//TRANSLIT//IGNORE", "UTF-8", descs, descl, &utf8l);
  if (utf8s == NULL) {
    utf8s = (char *) _marpaESLIF_utf82printableascii_defaultp;
    utf8l = strlen(utf8s);
  } else {
    /* Remove by hand any ASCII character not truely printable.      */
    /* Only the historical ASCII table [0-127] is a portable thingy. */
    p = utf8s;
    while ((c = (unsigned char) *p) != '\0') {
      if ((c >= 128) || (! isprint(c & 0xFF))) {
        *p = ' ';
      }
      p++;
    }
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return \"%s\"", utf8s);
  return utf8s;
}

/*****************************************************************************/
static inline void _marpaESLIF_utf82printableascii_freev(marpaESLIF_t *marpaESLIFp, char *utf82printableasciip)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIF_utf82printableascii_freev";

  if ((utf82printableasciip != NULL) && (utf82printableasciip != _marpaESLIF_utf82printableascii_defaultp)) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing ASCII description \"%s\"", utf82printableasciip);
    free(utf82printableasciip);
  }
}

/*****************************************************************************/
static inline char *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *utf8lp)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_utf8_newp";
  tconv_t            tconvp      = NULL;
  char              *inbuforigp  = descs;
  size_t             inleftorigl = descl;
  char              *outbuforigp = NULL;
  size_t             outbuforigl = 0;
  tconv_option_t     tconvOption = { NULL /* charsetp */, NULL /* convertp */, NULL /* traceCallbackp */, NULL /* traceUserDatavp */ };
  char              *rcp;
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
  tconvp = tconv_open_ext(toEncodings, fromEncodings, &tconvOption);
  if (tconvp == NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "tconv_open failure, %s", strerror(errno));
    goto err;
  }
#ifndef MARPAESLIF_NTRACE
  tconv_trace_on(tconvp);
#endif

  /* We start with an output buffer of the same size of input buffer */
  /* Whatever the destination encoding, we always reserve one byte more to place a NUL */
  outbuforigp = (char *) malloc(descl + 1);
  if (outbuforigp == NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "malloc failure, %s", strerror(errno));
    goto err;
  }
  outbuforigp[descl] = '\0';
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
	goto err;
      }
      /* Try to alloc more */
      outbuforigl *= 2;
      /* Will this ever happen ? */
      if (outbuforigl < descl) {
	MARPAESLIF_TRACE(marpaESLIFp, funcs, "size_t flip");
	goto err;
      }
      /* No + 1 : it is legal to have \0 and we are not doing to print this */
      tmp = realloc(outbuforigp, outbuforigl + 1);
      if (tmp == NULL) {
	MARPAESLIF_TRACEF(marpaESLIFp, funcs, "realloc failure, %s", strerror(errno));
	goto err;
      }
      outbuforigp[outbuforigl] = '\0';
      outbuforigp = tmp;
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
  *utf8lp = outbuforigl;
  goto done;

 err:
  rcp = NULL;

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
static inline void _marpaESLIF_tconv_freev(marpaESLIF_t *marpaESLIFp, char *p)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIF_tconv_freev";

  if (p != NULL) {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing converted string at %p", p);
    free(p);
  }
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp)
/*****************************************************************************/
{
  const static char                     *funcs    = "marpaESLIFGrammar_newp";
  marpaESLIFRecognizer_t                *marpaESLIFRecognizerp = NULL;
  marpaESLIFGrammar_t                   *marpaESLIFGrammarp = NULL;
  char                                  *utf8s = NULL;
  size_t                                 utf8l;
  marpaESLIFReaderGrammarReaderContext_t marpaESLIFReaderGrammarReaderContext;
  marpaESLIFRecognizerOption_t           marpaESLIFRecognizerOption;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Grammar");

  if (marpaESLIFGrammarOptionp == NULL) {
    marpaESLIFGrammarOptionp = &marpaESLIFGrammarOption_default;
  }

  if (marpaESLIFGrammarOptionp->grammarcp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Null source grammar pointer");
    goto err;
  }
  
  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) malloc(sizeof(marpaESLIFGrammar_t));
  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFGrammarp->marpaESLIFp             = marpaESLIFp;
  marpaESLIFGrammarp->marpaESLIFGrammarOption = *marpaESLIFGrammarOptionp;
  marpaESLIFGrammarp->grammarp                = NULL;

  /* We want to parse the incoming grammar in UTF-8 */
  utf8s = _marpaESLIF_tconv_newp(marpaESLIFp, "UTF-8", marpaESLIFGrammarOptionp->encodings, marpaESLIFGrammarOptionp->grammarcp, marpaESLIFGrammarOptionp->grammarl, &utf8l);
  if (utf8s == NULL) {
    goto err;
  }

  marpaESLIFReaderGrammarReaderContext.marpaESLIFp = marpaESLIFp;
  marpaESLIFReaderGrammarReaderContext.utf8s       = utf8s;
  marpaESLIFReaderGrammarReaderContext.utf8l       = utf8l;
  marpaESLIFReaderGrammarReaderContext.p           = NULL;

  marpaESLIFRecognizerOption.userDatavp                = (void *) &marpaESLIFReaderGrammarReaderContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = _marpaESLIFReader_grammarReader;
  marpaESLIFRecognizerOption.disableThresholdb         = 1;

  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  _marpaESLIF_tconv_freev(marpaESLIFp, utf8s);
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp);
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp)
/*****************************************************************************/
{
  const static char             *funcs       = "marpaESLIFRecognizer_newp";
  marpaESLIF_t                  *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Recognizer");

  if (marpaESLIFRecognizerOptionp == NULL) {
    marpaESLIFRecognizerOptionp = &marpaESLIFRecognizerOption_default;
  }

  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Null grammar pointer");
    goto err;
  }
  
  if (marpaESLIFRecognizerOptionp->marpaESLIFReaderCallbackp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Null reader callback");
    goto err;
  }
  
  marpaESLIFRecognizerp = (marpaESLIFRecognizer_t *) malloc(sizeof(marpaESLIFRecognizer_t));
  if (marpaESLIFRecognizerp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFRecognizerp->marpaESLIFp                = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizerp->marpaESLIFGrammarp         = marpaESLIFGrammarp;
  marpaESLIFRecognizerp->marpaESLIFRecognizerOption = *marpaESLIFRecognizerOptionp;
  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = NULL;

  marpaWrapperRecognizerOption.genericLoggerp       = marpaESLIFp->option.genericLoggerp;
  marpaWrapperRecognizerOption.disableThresholdb    = marpaESLIFRecognizerOptionp->disableThresholdb;

  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = marpaWrapperRecognizer_newp(marpaESLIFGrammarp->grammarp->marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
  if (marpaESLIFRecognizerp->marpaWrapperRecognizerp == NULL) {
    goto err;
  }

  goto done;

 err:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFRecognizerp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFRecognizerp);
  return marpaESLIFRecognizerp;
}

/*****************************************************************************/
short marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  const static char *funcs       = "marpaESLIFRecognizer_scanb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  size_t             nSymboll;
  int               *symbolArrayp;

  if (! marpaWrapperRecognizer_expectedb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &nSymboll, &symbolArrayp)) {
    goto err;
  }

  goto done;
  
 err:
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 0");
  return 0;

 done:
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 1");
  return 0;
}

/*****************************************************************************/
short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, int valuei)
/*****************************************************************************/
{
  return marpaWrapperRecognizer_alternativeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symboli, valuei, 1);
}

/*****************************************************************************/
short marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  return marpaWrapperRecognizer_completeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
}

/*****************************************************************************/
short marpaESLIFRecognizer_event_onoffb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, int symboli, marpaESLIFEventType_t eventSeti, int onoffb)
/*****************************************************************************/
{
  /* Of course, our marpaESLIFEventType_t is strictly equivalent to marpaWrapperGrammarEventType_t -; */
  return marpaWrapperRecognizer_event_onoffb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symboli, eventSeti, onoffb);
}

/*****************************************************************************/
short marpaESLIFRecognizer_expectedb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *nSymbollp, int **symbolArraypp)
/*****************************************************************************/
{
  return marpaWrapperRecognizer_expectedb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, nSymbollp, symbolArraypp);
}

/*****************************************************************************/
void marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  const static char *funcs = "marpaESLIFGrammar_freev";
  marpaESLIF_t      *marpaESLIFp;
  
  if (marpaESLIFGrammarp != NULL) {
    marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing grammar %p", marpaESLIFGrammarp);
    free(marpaESLIFGrammarp);
  }
}

/*****************************************************************************/
void marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  const static char *funcs = "marpaESLIFRecognizer_freev";
  marpaESLIF_t      *marpaESLIFp;
  
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
    marpaESLIFGrammar_freev(marpaESLIFRecognizerp->marpaESLIFGrammarp);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Freeing recognizer %p", marpaESLIFRecognizerp);
    marpaWrapperRecognizer_freev(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
    free(marpaESLIFRecognizerp);
  }
}

/*****************************************************************************/
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp)
/*****************************************************************************/
{
  const static char    *funcs              = "marpaESLIF_parseb";
  marpaESLIF_t         *marpaESLIFp        = marpaESLIFGrammarp->marpaESLIFp;
  short                 rcb;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building parser");
  
  if (marpaESLIFRecognizerOptionp == NULL) {
    marpaESLIFRecognizerOptionp = &marpaESLIFRecognizerOption_default;
  }

  if (marpaESLIFRecognizerOptionp->marpaESLIFReaderCallbackp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Null reader callback");
    goto err;
  }
	
  goto done;

 err:
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 0");
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarReader(void *userDatavp, char **inputcpp, size_t *inputlp)
/*****************************************************************************/
{
}

