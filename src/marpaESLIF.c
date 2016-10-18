#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <marpaESLIF.h>
#include <marpaWrapper.h>
#include <genericStack.h>
#include <tconv.h>
#include <ctype.h>
#include "config.h"
#include "marpaESLIF/internal/structures.h"
#include "marpaESLIF/internal/logging.h"

#ifndef MARPAESLIF_INITIAL_REPLACEMENT_LENGTH
#define MARPAESLIF_INITIAL_REPLACEMENT_LENGTH 8096  /* Subjective number */
#endif

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, PCRE2_SPTR substitutionp, PCRE2_SIZE substitutionl, char *testFullMatchs, char *testPartialMatchs);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_meta_t     *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl);
static inline void                   _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, unsigned int leveli);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *marpaESLIFGrammarp);

static inline void                   _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp);

static inline genericStack_t        *_marpaESLIF_valueStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_valueStack_freev(genericStack_t *valueStackp);

static inline marpaESLIF_rule_t     *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp);

static inline genericStack_t        *_marpaESLIFRecognizer_alternativeStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline void                   _marpaESLIFRecognizer_alternativeStack_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

static inline marpaESLIF_alternative_t *_marpaESLIF_alternative_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIFAlternative_t *alternativep);
static inline void                      _marpaESLIF_alternative_freev(marpaESLIF_alternative_t *marpaESLIF_alternativep);
static inline void                   _marpaESLIFAlternative_freev(marpaESLIFAlternative_t *marpaESLIFAlternativep);

static inline void                   _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp);

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

static inline short                  _marpaESLIF_terminal_string_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp);
static inline short                  _marpaESLIF_terminal_regex_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIF_grammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp);
static inline short                  _marpaESLIF_meta_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp);
static inline short                  _marpaESLIF_symbol_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_symbol_t *symbolp, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp);

const static  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *utf8lp);
static inline void                   _marpaESLIF_tconv_freev(char *utf8s);

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(char *utf82printableasciip);
static        short                  _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp);
static inline short                  _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb);
static inline marpaESLIF_symbol_t   *_marpaESLIFRecognizer_symbol_desc_to_ptr(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl);
static inline short                  _marpaESLIFRecognizer_alternativeb(marpaESLIF_t *marpaESLIFp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIFAlternative_t *alternativep);
static inline void                   _marpaESLIF_freeCallbackv(void *userDatavp, void *p);
static inline void                   _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_collect_grammar_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);

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

    if (! terminalp->matcherip(marpaESLIFp, NULL, NULL, terminalp, NULL, testFullMatchs, strlen(testFullMatchs), 1, &rci, NULL, &outputp, &outputl)) {
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

    if (! terminalp->matcherip(marpaESLIFp, NULL, NULL, terminalp, NULL, testPartialMatchs, strlen(testPartialMatchs), 0, &rci, NULL, &outputp, &outputl)) {
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
  _marpaESLIF_terminal_freev(terminalp);
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
  _marpaESLIF_meta_freev(metap);
  metap = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", metap);
  return metap;
}

/*****************************************************************************/
static inline void _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap)
/*****************************************************************************/
{
  if (metap != NULL) {
    if (metap->descs != NULL) {
      free(metap->descs);
    }
    if (metap->marpaWrapperGrammarClonep != NULL) {
      marpaWrapperGrammar_freev(metap->marpaWrapperGrammarClonep);
    }
    _marpaESLIF_utf82printableascii_freev(metap->asciidescs);
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
  marpaESLIF_grammar_t       *grammarp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  int                         i;
  int                         j;
  int                         symboli;
  char                       *descs;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Bootstrapping grammar at level %d", (int) leveli);

  marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->option.genericLoggerp;
  marpaWrapperGrammarOption.warningIsErrorb   = warningIsErrorb;
  marpaWrapperGrammarOption.warningIsIgnoredb = warningIsIgnoredb;
  marpaWrapperGrammarOption.autorankb         = autorankb;
  
  grammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, leveli);
  if (grammarp == NULL) {
    goto err;
  }

  /* First the terminals */
  for (i = 0; i < bootstrap_grammar_terminali; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }

    terminalp = _marpaESLIF_terminal_newp(marpaESLIFp,
					  grammarp,
					  0, /* startb */
					  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
					  bootstrap_grammar_terminalp[i].descs,
					  strlen(bootstrap_grammar_terminalp[i].descs),
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

    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->u.terminalp->idi);
    if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp push failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in grammarp->symbolStackp */
    symbolp = NULL;
  }

  /* Then the non-terminals */
  for (i = 0; i < bootstrap_grammar_metai; i++) {
    symbolp = _marpaESLIF_symbol_newp(marpaESLIFp);
    if (symbolp == NULL) {
      goto err;
    }

    metap = _marpaESLIF_meta_newp(marpaESLIFp,
				  grammarp,
				  bootstrap_grammar_metap[i].startb,
				  MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE,
				  bootstrap_grammar_metap[i].descs,
				  strlen(bootstrap_grammar_metap[i].descs)
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

    GENERICSTACK_SET_PTR(grammarp->symbolStackp, symbolp, symbolp->u.metap->idi);
    if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp set failure, %s", strerror(errno));
      goto err;
    }
    /* Push is ok: symbolp is in grammarp->symbolStackp */
    symbolp = NULL;
  }

  /* Then the rules */
  for (i = 0; i < bootstrap_grammar_rulei; i++) {
    if (bootstrap_grammar_rulep[i].discardb) {
      /* Grammar should have already made sure that a discard rule can have only one RHS */
      if (bootstrap_grammar_rulep[i].nrhsl != 1) {
        MARPAESLIF_ERRORF(marpaESLIFp, "A discard rule must have exactly one RHS");
        goto err;
      }
    }
    rulep = _marpaESLIF_rule_newp(marpaESLIFp,
				  grammarp,
				  bootstrap_grammar_rulep[i].descs,
				  strlen(bootstrap_grammar_rulep[i].descs),
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
    if (bootstrap_grammar_rulep[i].discardb) {
      symboli = bootstrap_grammar_rulep[i].rhsip[0];
      descs = NULL;
      for (j = 0; j < bootstrap_grammar_metai; j++) {
        if (bootstrap_grammar_metap[j].idi == symboli) {
          descs = bootstrap_grammar_metap[j].descs;
        }
      }
      if (descs == NULL) {
        MARPAESLIF_ERROR(marpaESLIFp, "Cannot find description of RHS of a discard rule");
        goto err;
      }
      symbolp = _marpaESLIFRecognizer_symbol_desc_to_ptr(marpaESLIFp, grammarp, descs, strlen(descs));
      if (symbolp == NULL) {
        goto err;
      }
      GENERICSTACK_PUSH_PTR(grammarp->discardSymbolStackp, symbolp);
      if (GENERICSTACK_ERROR(grammarp->discardSymbolStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "discardSymbolStackp push failure, %s", strerror(errno));
        goto err;
      }
    } else {
      GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
      if (GENERICSTACK_ERROR(grammarp->ruleStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
        goto err;
      }
    }
    /* Push is ok: rulep is in grammarp->ruleStackp or grammarp->discardSymbolStackp */
    rulep = NULL;
  }

  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  _marpaESLIF_meta_freev(metap);
  _marpaESLIF_rule_freev(rulep);
  _marpaESLIF_symbol_freev(symbolp);
  _marpaESLIF_grammar_freev(grammarp);
  grammarp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp);
  return grammarp;
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

    if (grammari == 0) {
      /* Only grammar at level 0 is systematically precomputed - all grammars at sub-levels */
      /* are precomputed as many times as needed. */
      if (! marpaWrapperGrammar_precomputeb(grammarp->marpaWrapperGrammarp)) {
        MARPAESLIF_ERROR(marpaESLIFp, "Failure to precompute grammar at level 0");
        goto err;
      }
    }

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
        if (metap->marpaWrapperGrammarClonep != NULL) {
          /* Meta grammar already precomputed for this symbol */
          continue;
        }
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
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, unsigned int leveli)
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
  grammarp->discardSymbolStackp  = NULL;

  grammarp->marpaWrapperGrammarp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarp == NULL) {
    goto err;
  }
  GENERICSTACK_NEW(grammarp->symbolStackp);
  if (GENERICSTACK_ERROR(grammarp->symbolStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "symbolStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(grammarp->ruleStackp);
  if (GENERICSTACK_ERROR(grammarp->ruleStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(grammarp->discardSymbolStackp);
  if (GENERICSTACK_ERROR(grammarp->discardSymbolStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "discardSymbolStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_grammar_freev(grammarp);
  grammarp = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp);
  return grammarp;
}

/*****************************************************************************/
static inline void _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  if (grammarp != NULL) {
    if (grammarp->marpaWrapperGrammarp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarp);
    }			       
    _marpaESLIF_symbolStack_freev(grammarp->symbolStackp);
    _marpaESLIF_ruleStack_freev(grammarp->ruleStackp);
    GENERICSTACK_FREE(grammarp->discardSymbolStackp);
    free(grammarp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp)
/*****************************************************************************/
{
  if (ruleStackp != NULL) {
    while (GENERICSTACK_USED(ruleStackp) > 0) {
      if (GENERICSTACK_IS_PTR(ruleStackp, GENERICSTACK_USED(ruleStackp) - 1)) {
	marpaESLIF_rule_t *rulep = (marpaESLIF_rule_t *) GENERICSTACK_POP_PTR(ruleStackp);
	_marpaESLIF_rule_freev(rulep);
      } else {
	GENERICSTACK_USED(ruleStackp)--;
      }
    }
    GENERICSTACK_FREE(ruleStackp);
  }
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIF_valueStack_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_valueStack_newp";
  genericStack_t      *stackp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building value stack");

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
static inline void _marpaESLIF_valueStack_freev(genericStack_t *valueStackp)
/*****************************************************************************/
{
  if (valueStackp != NULL) {
    while (GENERICSTACK_USED(valueStackp) > 0) {
      if (GENERICSTACK_IS_PTR(valueStackp, GENERICSTACK_USED(valueStackp) - 1)) {
	marpaESLIFAlternative_t *marpaESLIFAlternativep = (marpaESLIFAlternative_t *) GENERICSTACK_POP_PTR(valueStackp);
        _marpaESLIFAlternative_freev(marpaESLIFAlternativep);
      } else {
	GENERICSTACK_USED(valueStackp)--;
      }
    }
    GENERICSTACK_FREE(valueStackp);
  }
}

/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb)
/*****************************************************************************/
{
  const static char               *funcs        = "_marpaESLIF_rule_newp";
  genericStack_t                  *symbolStackp = grammarp->symbolStackp;
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
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS symbols is of type N/A", grammarp->leveli, rulep->asciidescs, symbolp->type);
      goto err;
    }
    if (symbolFoundb) {
      break;
    }
  }
  if (! symbolFoundb) {
    MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS symbol does not exist", grammarp->leveli, rulep->asciidescs, lhsi);
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
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, rhsip[i])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: No such RHS symbol No %d", grammarp->leveli, rulep->asciidescs, rhsip[i]);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rulep->rhsStackp, GENERICSTACK_GET_PTR(grammarp->symbolStackp, rhsip[i]));
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
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, exceptionip[i])) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: No such RHS exception symbol No %d", grammarp->leveli, rulep->asciidescs, exceptionip[i]);
      goto err;
    }
    GENERICSTACK_PUSH_PTR(rulep->exceptionStackp, GENERICSTACK_GET_PTR(grammarp->symbolStackp, exceptionip[i]));
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
  rulep->idi = marpaWrapperGrammar_newRulei(grammarp->marpaWrapperGrammarp, &marpaWrapperGrammarRuleOption, lhsi, nrhsl, rhsip);
  if (rulep->idi < 0) {
    goto err;
  }

  goto done;

 err:
  _marpaESLIF_rule_freev(rulep);
  rulep = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rulep);
  return rulep;
}

/*****************************************************************************/
static inline void _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep)
/*****************************************************************************/
{
  if (rulep != NULL) {
    if (rulep->descs != NULL) {
      free(rulep->descs);
    }
    _marpaESLIF_utf82printableascii_freev(rulep->asciidescs);
    /* In the rule structure, lhsp, rhsStackp and exceptionStackp contain shallow pointers */
    /* Only the stack themselves should be freed. */
    /*
    _marpaESLIF_symbol_freev(rulep->lhsp);
    _marpaESLIF_symbolStack_freev(rulep->rhsStackp);
    _marpaESLIF_symbolStack_freev(marpaESLIFp, exceptionStackp);
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
  symbolp->isLhsb          = 0;
  symbolp->idi             =  -1;
  symbolp->descs           =  NULL;
  symbolp->descl           =  0;
  symbolp->asciidescs      =  NULL;
  symbolp->matcherip       =  NULL;
  symbolp->pauseb          = 0;
  symbolp->pauseIsOnb      = 0;
  symbolp->pauses          = NULL;
  symbolp->pausel          = 0;
  symbolp->asciipauses     = NULL;
  symbolp->events          = NULL;
  symbolp->eventl          = 0;
  symbolp->asciievents     = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", symbolp);
  return symbolp;
}

/*****************************************************************************/
static inline void _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp)
/*****************************************************************************/
{
  if (symbolp != NULL) {
    /* All pointers are the top level of this structure are shallow pointers */
    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      _marpaESLIF_terminal_freev(symbolp->u.terminalp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      _marpaESLIF_meta_freev(symbolp->u.metap);
      break;
    default:
      break;
    }
    if (symbolp->pauses != NULL) {
      free(symbolp->pauses);
    }
    if (symbolp->asciipauses != NULL) {
      free(symbolp->asciipauses);
    }
    if (symbolp->events != NULL) {
      free(symbolp->events);
    }
    if (symbolp->asciievents != NULL) {
      free(symbolp->asciievents);
    }
    free(symbolp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp)
/*****************************************************************************/
{
  if (symbolStackp != NULL) {
    while (GENERICSTACK_USED(symbolStackp) > 0) {
      if (GENERICSTACK_IS_PTR(symbolStackp, GENERICSTACK_USED(symbolStackp) - 1)) {
	marpaESLIF_symbol_t *symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_POP_PTR(symbolStackp);
	_marpaESLIF_symbol_freev(symbolp);
      } else {
	GENERICSTACK_USED(symbolStackp)--;
      }
    }
    GENERICSTACK_FREE(symbolStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp)
/*****************************************************************************/
{
  if (terminalp != NULL) {
    if (terminalp->descs != NULL) {
      free(terminalp->descs);
    }
    _marpaESLIF_utf82printableascii_freev(terminalp->asciidescs);
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
  marpaESLIF_grammar_t *grammarp = NULL;
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
  grammarp = _marpaESLIF_bootstrap_grammar_L0p(marpaESLIFp);
  if (grammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->grammarStackp, grammarp, grammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  grammarp = NULL;  /* It is in the stack of grammars */

  /* G1 */
  grammarp = _marpaESLIF_bootstrap_grammar_G1p(marpaESLIFp);
  if (grammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->grammarStackp, grammarp, grammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  grammarp = NULL; /* Ditto */

  /* Validate all the grammars together */
  if (! _marpaESLIF_validate_grammarb(marpaESLIFp)) {
    goto err;
  }
  goto done;
  
 err:
  _marpaESLIF_grammar_freev(grammarp);
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
    if (marpaESLIFp->grammarStackp != NULL) {
      while (GENERICSTACK_USED(marpaESLIFp->grammarStackp) > 0) {
	if (GENERICSTACK_IS_PTR(marpaESLIFp->grammarStackp, GENERICSTACK_USED(marpaESLIFp->grammarStackp) - 1)) {
	  marpaESLIF_grammar_t *grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_POP_PTR(marpaESLIFp->grammarStackp);
	  _marpaESLIF_grammar_freev(grammarp);
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
static inline short _marpaESLIF_terminal_string_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  const static char         *funcs             = "_marpaESLIF_matcheri";
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
      if (memcmp(inputs, marpaESLIF_string.stringp, marpaESLIF_string.stringl) == 0) {
        rci = MARPAESLIF_MATCH_OK;
        matchLengthl = marpaESLIF_string.stringl;
      } else {
        rci = MARPAESLIF_MATCH_FAILURE;
      }
    } else {
      /* Partial match never returns OK -; */
      rci = (memcmp(inputs, marpaESLIF_string.stringp, inputl) == 0) ? (eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN) : MARPAESLIF_MATCH_FAILURE;
    }
  } else {
    rci = eofb ? MARPAESLIF_MATCH_FAILURE : MARPAESLIF_MATCH_AGAIN;
  }

  if (rcip != NULL) {
    *rcip = rci;
  }

  if (rci == MARPAESLIF_MATCH_OK) {
    /* It is important to handle outputlp before outputpp, see below   */
    /* Note that when rci is MARPAESLIF_MATCH_OK, it is guaranteed that */
    /* matchLengthl is set to a value > 0.                              */
    if (matchedlp != NULL) {
      *matchedlp = matchLengthl;
    }
    if (outputlp != NULL) {
      *outputlp = matchLengthl;
    }
    if (outputpp != NULL) {
      char *outputp = (char *) malloc(matchLengthl + 1);
      /* Matched input is starting at inputs and its length is in matchLengthl */
      if (outputp == NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
        goto err;
      }
      memcpy(outputp, inputs, matchLengthl);
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
static inline short _marpaESLIF_terminal_regex_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  const static char         *funcs             = "_marpaESLIF_matcheri";
  short                      rcb               = 1;
  PCRE2_UCHAR               *pcre2_substitutep = NULL;
  PCRE2_SIZE                 pcre2_substitutel = MARPAESLIF_INITIAL_REPLACEMENT_LENGTH; /* In code units, not bytes, though in UTF-8 a code unit is one byte -; */
  marpaESLIF_matcher_value_t rci;
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
                                             (PCRE2_SPTR) inputs,            /* subject */
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
                                            (PCRE2_SPTR) inputs,          /* subject */
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
                                        (PCRE2_SPTR) inputs,          /* subject */
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
                                              (PCRE2_SPTR) inputs,          /* subject */
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
                                          (PCRE2_SPTR) inputs,          /* subject */
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
    if (matchedlp != NULL) {
      *matchedlp = matchLengthl;
    }
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
	/* Matched input is starting at inputs and its length is in matchLengthl */
	if (outputp == NULL) {
	  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - malloc failure, %s", terminalp->asciidescs, strerror(errno));
	  goto err;
	}
	memcpy(outputp, inputs, matchLengthl);
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
static inline short _marpaESLIF_meta_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  /* All in all, this routine is the core of this module, and the cause of recursion -; */
  const static char           *funcs                 = "_marpaESLIF_meta_matcheri";
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  short                        rcb;
  marpaESLIFGrammar_t          marpaESLIFGrammar; /* Fake marpaESLIFGrammar with the grammar sent in the stack */
  marpaESLIF_grammar_t         grammar;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption; /* This is an internal recognizer */

  marpaESLIFGrammar            = *marpaESLIFGrammarp;
  grammar                      = *(marpaESLIFGrammar.grammarp);
  grammar.marpaWrapperGrammarp = marpaWrapperGrammarp;
  marpaESLIFGrammar.grammarp   = &grammar;

  marpaESLIFRecognizerOption.userDatavp        = (void *) marpaESLIFp;
  marpaESLIFRecognizerOption.disableThresholdb = 1;
  marpaESLIFRecognizerOption.exhaustedb        = 1;
  marpaESLIFRecognizerOption.latmb             = 1;

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(&marpaESLIFGrammar, &marpaESLIFRecognizerOption);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_symbol_matcheri(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_symbol_t *symbolp, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, size_t *matchedlp, char **outputpp, size_t *outputlp)
/*****************************************************************************/
{
  const static char     *funcs = "_marpaESLIF_symbol_matcheri";

    switch (symbolp->type) {
    case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
      symbolp->matcherip(marpaESLIFp,
                         NULL /* marpaESLIFGrammarp */,
                         NULL /* marpaESLIFGrammarp->grammarp->marpaWrapperGrammarp */,
                         symbolp->u.terminalp,
                         NULL /* metap */,
                         inputs,
                         inputl,
                         eofb,
                         rcip,
                         matchedlp,
                         outputpp,
                         outputlp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      _marpaESLIF_meta_matcheri(marpaESLIFp,
                                marpaESLIFGrammarp,
                                symbolp->u.metap->marpaWrapperGrammarClonep,
                                NULL /* terminalp */,
                                symbolp->u.metap,
                                inputs,
                                inputl,
                                eofb,
                                rcip,
                                matchedlp,
                                outputpp,
                                outputlp);
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unknown symbol type %d", symbolp->type);
      goto err;
    }

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 1");
  return 1;

 err:
  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 0");
  return 0;
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
static inline void _marpaESLIF_utf82printableascii_freev(char *utf82printableasciip)
/*****************************************************************************/
{
  if ((utf82printableasciip != NULL) && (utf82printableasciip != _marpaESLIF_utf82printableascii_defaultp)) {
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

  /* We start with an output buffer of the same size of input buffer.                  */
  /* Whatever the destination encoding, we always reserve one byte more to place a NUL */
  /* just in case. This NUL is absolutetly harmless but is usefull if one want to look */
  /* at the variables via a debugger -;.                                               */
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
      /* Note the "+ 1" */
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

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}

/*****************************************************************************/
static inline void _marpaESLIF_tconv_freev(char *p)
/*****************************************************************************/
{
  if (p != NULL) {
    free(p);
  }
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFGrammar_newp(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammarOption_t *marpaESLIFGrammarOptionp)
/*****************************************************************************/
{
  const static char           *funcs                 = "marpaESLIFGrammar_newp";
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp    = NULL;
  char                        *utf8s                 = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarTmpp = NULL;
  size_t                       utf8l;
  marpaESLIF_readerContext_t   marpaESLIF_readerContext;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Grammar");

  if (marpaESLIFGrammarOptionp == NULL) {
    marpaESLIFGrammarOptionp = &marpaESLIFGrammarOption_default;
  }

  if (marpaESLIFGrammarOptionp->grammars == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Null source grammar pointer");
    goto err;
  }
  
  marpaESLIFGrammarp = (marpaESLIFGrammar_t *) malloc(sizeof(marpaESLIFGrammar_t));
  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFGrammarp->marpaESLIFp             = marpaESLIFp;
  marpaESLIFGrammarp->grammarp                = NULL;

  /* We want to parse the incoming grammar in UTF-8 */
  utf8s = _marpaESLIF_tconv_newp(marpaESLIFp, "UTF-8", marpaESLIFGrammarOptionp->encodings, marpaESLIFGrammarOptionp->grammars, marpaESLIFGrammarOptionp->grammarl, &utf8l);
  if (utf8s == NULL) {
    goto err;
  }

  marpaESLIF_readerContext.marpaESLIFp = marpaESLIFp;
  marpaESLIF_readerContext.utf8s       = utf8s;
  marpaESLIF_readerContext.utf8l       = utf8l;

  marpaESLIFRecognizerOption.userDatavp                = (void *) &marpaESLIF_readerContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp = _marpaESLIFReader_grammarReader;
  marpaESLIFRecognizerOption.disableThresholdb         = 1; /* No threshold warning when parsing a grammar */
  marpaESLIFRecognizerOption.exhaustedb                = 0; /* Exhaustion is not allowed */

  marpaESLIFGrammarTmpp = (marpaESLIFGrammar_t *) malloc(sizeof(marpaESLIFGrammar_t));
  if (marpaESLIFGrammarTmpp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }
  marpaESLIFGrammarTmpp->marpaESLIFp = marpaESLIFp;
  marpaESLIFGrammarTmpp->grammarp    = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFp->grammarStackp, 0);

  if (! marpaESLIFGrammar_parseb(marpaESLIFGrammarTmpp, &marpaESLIFRecognizerOption)) {
    goto err;
  }

  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  marpaESLIFGrammar_freev(marpaESLIFGrammarTmpp);
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  _marpaESLIF_tconv_freev(utf8s);
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
  marpaESLIFRecognizerp->valueStackp                = NULL;
  marpaESLIFRecognizerp->inputs                     = NULL;
  marpaESLIFRecognizerp->inputl                     = 0;
  marpaESLIFRecognizerp->eofb                       = 0;
  marpaESLIFRecognizerp->scanb                      = 0;
  marpaESLIFRecognizerp->stringArrayp               = NULL;
  marpaESLIFRecognizerp->stringArrayl               = 0;
  marpaESLIFRecognizerp->alternativeStackp          = NULL;

  marpaWrapperRecognizerOption.genericLoggerp       = marpaESLIFp->option.genericLoggerp;
  marpaWrapperRecognizerOption.disableThresholdb    = marpaESLIFRecognizerOptionp->disableThresholdb;

  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = marpaWrapperRecognizer_newp(marpaESLIFGrammarp->grammarp->marpaWrapperGrammarp, &marpaWrapperRecognizerOption);
  if (marpaESLIFRecognizerp->marpaWrapperRecognizerp == NULL) {
    goto err;
  }
  marpaESLIFRecognizerp->valueStackp = _marpaESLIF_valueStack_newp(marpaESLIFp);
  if (marpaESLIFRecognizerp->valueStackp == NULL) {
    goto err;
  }
  marpaESLIFRecognizerp->alternativeStackp = _marpaESLIFRecognizer_alternativeStack_newp(marpaESLIFp);
  if (marpaESLIFRecognizerp->alternativeStackp == NULL) {
    goto err;
  }
  
  /* Marpa does not like value stack at indice 0 */
  GENERICSTACK_PUSH_PTR(marpaESLIFRecognizerp->valueStackp, NULL);
  if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->valueStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "valueStackp push failure, %s", strerror(errno));
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
  marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;

  if (marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan can be one once only");
    return 0;
  }

  marpaESLIFRecognizerp->scanb = 1;
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0); /* Do not ignore pre-events */
}

/*****************************************************************************/
short marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 1); /* Ignore pre-events */
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb)
/*****************************************************************************/
{
  const static char          *funcs              = "_marpaESLIFRecognizer_resumeb";
  marpaESLIF_t               *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t       *grammarp           = marpaESLIFGrammarp->grammarp;
  genericStack_t             *symbolStackp       = grammarp->symbolStackp;
  void                       *userDatavp         = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.userDatavp;
  marpaESLIFReader_t          readerp            = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp;
  char                       *inputs            = marpaESLIFRecognizerp->inputs;
  size_t                      inputl             = marpaESLIFRecognizerp->inputl;
  short                       eofb               = marpaESLIFRecognizerp->eofb;
  short                       latmb              = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.latmb;
  char                       *outputp            = NULL;
  size_t                      maxMatchedl         = 0;
  size_t                      nSymboll;
  int                        *symbolArrayp;
  size_t                      symboll;
  int                         symboli;
  marpaESLIF_symbol_t        *symbolp;
  marpaESLIF_matcher_value_t  rci;
  size_t                      outputl;
  size_t                      matchedl;
  short                       rcb;
  marpaESLIFAlternative_t    *marpaESLIFAlternativep = NULL;
  marpaESLIF_alternative_t   *marpaESLIF_alternativep = NULL;

  if (! marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan must be called first");
    return 0;
  }

  /* Always reset events */
  _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizerp);
  
  if (! ignorePreviousEventsb) {
    if (! _marpaESLIFRecognizer_collect_grammar_events(marpaESLIFRecognizerp)) {
      goto err;
    }
    if (marpaESLIFRecognizerp->stringArrayl > 0) {
      /* Return immediately */
      goto done;
    }
    /* No grammar event - resume can continue */
  }
  
  /* Ask for expected lexemes */
  if (! marpaESLIFRecognizer_expectedb(marpaESLIFRecognizerp, &nSymboll, &symbolArrayp)) {
    goto err;
  }

  /* Alawys reset internal alternative stack */
  _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizerp);

  /* Try to match */
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - trying to match", symbolp->asciidescs);
  match_again:
    if (! _marpaESLIF_symbol_matcheri(marpaESLIFp, marpaESLIFGrammarp, symbolp, marpaESLIFRecognizerp->inputs, marpaESLIFRecognizerp->inputl, marpaESLIFRecognizerp->eofb, &rci, &matchedl, &outputp, &outputl)) {
      goto err;
    }
    switch (rci) {
    case MARPAESLIF_MATCH_AGAIN:
      /* We have to load more unless already at EOF */
      if (! eofb) {
        if (! readerp(userDatavp, &inputs, &inputl, &eofb)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - reader failure", symbolp->asciidescs);
          goto err;
        }
        marpaESLIFRecognizerp->inputs = inputs;
        marpaESLIFRecognizerp->inputl  = inputl;
        marpaESLIFRecognizerp->eofb    = eofb;
        goto match_again;
      }
      break;
    case MARPAESLIF_MATCH_FAILURE:
      /* No op */
      break;
    case MARPAESLIF_MATCH_OK:
      /* Only in this case, it is possible that outputp contains allocated memory */
      marpaESLIFAlternativep = (marpaESLIFAlternative_t *) malloc(sizeof(marpaESLIFAlternative_t));
      if (marpaESLIFAlternativep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - malloc failure, %s", symbolp->asciidescs, strerror(errno));
        goto err;
      }
      if (matchedl > maxMatchedl) {
        maxMatchedl = matchedl;
      }
      marpaESLIFAlternativep->outputp       = (void *) outputp;
      marpaESLIFAlternativep->outputl       = outputl;
      marpaESLIFAlternativep->matchedl      = matchedl;
      marpaESLIFAlternativep->userDatavp    = (void *) marpaESLIFp;
      marpaESLIFAlternativep->freeCallbackp = _marpaESLIF_freeCallbackv;
      outputp = NULL; /* Now outputp is in alternativep */

      marpaESLIF_alternativep = (marpaESLIF_alternative_t *) malloc(sizeof(marpaESLIF_alternative_t));
      if (marpaESLIF_alternativep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - malloc failure, %s", symbolp->asciidescs, strerror(errno));
        goto err;
      }
      marpaESLIF_alternativep->symbolp                = symbolp;
      marpaESLIF_alternativep->marpaESLIFAlternativep = marpaESLIFAlternativep;
      marpaESLIFAlternativep = NULL; /* Now alternativep is in localAlternativep */
      
      GENERICSTACK_PUSH_PTR(marpaESLIFRecognizerp->alternativeStackp, marpaESLIF_alternativep);
      if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->alternativeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - alternativeStackp push failure, %s", symbolp->asciidescs, strerror(errno));
        goto err;
      }
      marpaESLIF_alternativep = NULL; /* Now marpaESLIF_alternativep is in marpaESLIFRecognizerp->alternativeStackp */
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - unsupported matcher return code", rci);
      goto err;
    }
  }
  
  /* No alternative ? Per def if this is the end (exhaustion support is handled when collecting grammar events) this is ok */
  if (GENERICSTACK_USED(marpaESLIFRecognizerp->alternativeStackp) <= 0) {
    if (! eofb) {
      /* Try to match the discard symbols */
    }
    goto exhaustion;
  }

  /* Generate the eventual pre-lexeme events */
  /* Switch to user space now */

  /* Push alternatives */
  while (GENERICSTACK_USED(marpaESLIFRecognizerp->alternativeStackp) > 0) {
    marpaESLIF_alternative_t *marpaESLIF_alternative_shallowp = (marpaESLIF_alternative_t *) GENERICSTACK_POP_PTR(marpaESLIFRecognizerp->alternativeStackp);

    /* If latm mode is true, keep only the longests alternatives */
    if (latmb && (marpaESLIF_alternative_shallowp->marpaESLIFAlternativep->matchedl < maxMatchedl)) {
      MARPAESLIF_TRACEF(marpaESLIFp,
                        funcs,
                        "%s - alternative is skipped (length %ld < max length %ld)",
                        marpaESLIF_alternative_shallowp->symbolp->asciidescs,
                        (unsigned long) marpaESLIF_alternative_shallowp->marpaESLIFAlternativep->matchedl,
                        (unsigned long) maxMatchedl);
      continue;
      
    }
    if (! _marpaESLIFRecognizer_alternativeb(marpaESLIFp,
                                             marpaESLIFRecognizerp,
                                             marpaESLIF_alternative_shallowp->symbolp,
                                             marpaESLIF_alternative_shallowp->marpaESLIFAlternativep)) {
      goto err;
    }
  }

  /* Commit */
  if (! marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp)) {
    goto err;
  }

  rcb = 1;
  goto done;

 exhaustion:
  rcb = -1;
  goto done;

 err:
  rcb = 0;

 done:
  if (outputp != NULL) {
    free(outputp);
  }
  _marpaESLIF_alternative_freev(marpaESLIF_alternativep);
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbolnamecp, size_t symbolnamel, marpaESLIFAlternative_t *alternativep)
/*****************************************************************************/
{
  const static char    *funcs              = "marpaESLIFRecognizer_alternativeb";
  marpaESLIF_t         *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t  *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t *grammarp           = marpaESLIFGrammarp->grammarp;
  marpaESLIF_symbol_t  *symbolp;

  symbolp = _marpaESLIFRecognizer_symbol_desc_to_ptr(marpaESLIFp, grammarp, symbolnamecp, symbolnamel);
  if (symbolp == NULL) {
    return 0;
  }

  return _marpaESLIFRecognizer_alternativeb(marpaESLIFp, marpaESLIFRecognizerp, symbolp, alternativep);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_alternativeb(marpaESLIF_t *marpaESLIFp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIFAlternative_t *marpaESLIFAlternativep)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIFRecognizer_alternativeb";

  GENERICSTACK_PUSH_PTR(marpaESLIFRecognizerp->valueStackp, marpaESLIFAlternativep);
  if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->valueStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "valueStackp push failure, %s", strerror(errno));
    return 0;
  }

  return marpaWrapperRecognizer_alternativeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symbolp->idi, GENERICSTACK_USED(marpaESLIFRecognizerp->valueStackp) - 1, 1);
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
  if (marpaESLIFGrammarp != NULL) {
    free(marpaESLIFGrammarp);
  }
}

/*****************************************************************************/
void marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
    
    _marpaESLIF_valueStack_freev(marpaESLIFRecognizerp->valueStackp);
    marpaWrapperRecognizer_freev(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
    if (marpaESLIFRecognizerp->stringArrayp != NULL) {
      free(marpaESLIFRecognizerp->stringArrayp);
    }
    _marpaESLIFRecognizer_alternativeStack_freev(marpaESLIFRecognizerp);
    free(marpaESLIFRecognizerp);
  }
}

/*****************************************************************************/
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp)
/*****************************************************************************/
{
  const static char      *funcs                 = "marpaESLIF_parseb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = NULL;
  short                   rcb;

  marpaESLIFRecognizerp = marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp)) {
    goto err;
  }
  
  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarReader";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  *inputsp = marpaESLIF_readerContextp->utf8s;
  *inputlp  = marpaESLIF_readerContextp->utf8l;
  *eofbp    = 1;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1 (*inputsp=%p, *inputlp=%ld, *eofbp=%d)", *inputsp, (unsigned long) *inputlp, (int) *eofbp);
  return 1;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIFRecognizer_symbol_desc_to_ptr(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIFRecognizer_symbol_desc_to_idi";
  marpaESLIF_symbol_t *symbolp   = NULL;
  marpaESLIF_symbol_t *rcp   = NULL;
  int                  symboli;

  for (symboli = 0; symboli < GENERICSTACK_USED(grammarp->symbolStackp); symboli++) {
    symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(grammarp->symbolStackp, symboli);
    if (symbolp->descl != descl) {
      continue;
    }
    if (memcmp(symbolp->descs, descs, descl) == 0) {
      rcp = symbolp;
      break;
    }
  }

  if (rcp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "No such symbol name");
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}

/*****************************************************************************/
void marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *stringArraylp, marpaESLIFString_t **stringArraypp)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;

  if (stringArraylp != NULL) {
    *stringArraylp = marpaESLIFRecognizerp->stringArrayl;
  }
  if (stringArraypp != NULL) {
    *stringArraypp = marpaESLIFRecognizerp->stringArrayp;
  }
}

/*****************************************************************************/
static inline marpaESLIF_alternative_t *_marpaESLIF_alternative_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIFAlternative_t *marpaESLIFAlternativep)
/*****************************************************************************/
{
  const static char        *funcs       = "_marpaESLIF_alternative_newp";
  marpaESLIF_t             *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIF_alternative_t *rcp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building alternative structure");

  rcp = (marpaESLIF_alternative_t *) malloc(sizeof(marpaESLIF_alternative_t));
  if (rcp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto done;
  }

  rcp->symbolp                = symbolp;
  rcp->marpaESLIFAlternativep = marpaESLIFAlternativep;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}

/*****************************************************************************/
static inline void _marpaESLIF_alternative_freev(marpaESLIF_alternative_t *marpaESLIF_alternativep)
/*****************************************************************************/
{
  if (marpaESLIF_alternativep != NULL) {
    marpaESLIFAlternative_t *marpaESLIFAlternativep = marpaESLIF_alternativep->marpaESLIFAlternativep;

    _marpaESLIFAlternative_freev(marpaESLIFAlternativep);
    free(marpaESLIF_alternativep);
  }
}

/*****************************************************************************/
static inline genericStack_t *_marpaESLIFRecognizer_alternativeStack_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_alternativeStack_newp";
  genericStack_t      *stackp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building alternative stack");

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
static inline void _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  genericStack_t *alternativeStackp = marpaESLIFRecognizerp->alternativeStackp;

  if (alternativeStackp != NULL) {
    while (GENERICSTACK_USED(alternativeStackp) > 0) {
      if (GENERICSTACK_IS_PTR(alternativeStackp, GENERICSTACK_USED(alternativeStackp) - 1)) {
	marpaESLIF_alternative_t *alternativep = (marpaESLIF_alternative_t *) GENERICSTACK_POP_PTR(alternativeStackp);

        _marpaESLIF_alternative_freev(alternativep);
      } else {
	GENERICSTACK_USED(alternativeStackp)--;
      }
    }
  }

  marpaESLIFRecognizerp->alternativeStackp = alternativeStackp;
}

/*****************************************************************************/
static inline void _marpaESLIFRecognizer_alternativeStack_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp->alternativeStackp != NULL) {
    _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizerp);
    GENERICSTACK_FREE(marpaESLIFRecognizerp->alternativeStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_freeCallbackv(void *userDatavp, void *p)
/*****************************************************************************/
{
  if (p != NULL) {
    free(p);
  }
}

/*****************************************************************************/
static inline void _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp->stringArrayp != NULL) {
    free(marpaESLIFRecognizerp->stringArrayp);
    marpaESLIFRecognizerp->stringArrayp = NULL;
  }
  marpaESLIFRecognizerp->stringArrayl = 0;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_collect_grammar_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  const static char          *funcs              = "_marpaESLIFRecognizer_collect_grammar_events";
  marpaESLIF_t               *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t       *grammarp           = marpaESLIFGrammarp->grammarp;
  genericStack_t             *symbolStackp       = grammarp->symbolStackp;
  short                       exhaustedb         = 0;
  marpaESLIF_symbol_t        *symbolp;
  int                         symboli;
  size_t                      grammarEventl;
  marpaWrapperGrammarEvent_t *grammarEventp;
  short                       rcb;
  char                       *events;
  size_t                      eventl;
  char                       *asciievents;
  size_t                      i;
  size_t                      okeventl;
  marpaESLIFEventType_t       type;

  /* Collect grammar native events and push them in the events stack */
  if (! marpaWrapperGrammar_eventb(grammarp->marpaWrapperGrammarp, &grammarEventl, &grammarEventp, 0)) {
    goto err;
  }
  if (grammarEventl > 0) {
    marpaESLIFRecognizerp->stringArrayp = malloc(grammarEventl * sizeof(marpaESLIFString_t));
    if (marpaESLIFRecognizerp->stringArrayp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFRecognizerp->stringArrayl = 0;

    for (i = 0, okeventl = 0; i < grammarEventl; i++) {
      symboli = grammarEventp[i].symboli;
      type    = MARPAESLIF_EVENTTYPE_NONE;
      events  = NULL;
      if (symboli >= 0) {
        /* Look for the symbol */
        if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
          goto err;
        }
        symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
      } else {
        symbolp = NULL;
      }
      
      /* Our grammar made sure there can by only one named event per symbol */
      /* In addition, marpaWrapper guarantee there is a symbol associated to */
      /* complation, nulled or prediction events */
      switch (grammarEventp[i].eventType) {
      case MARPAWRAPPERGRAMMAR_EVENT_COMPLETED:
        type    = MARPAESLIF_EVENTTYPE_COMPLETED;
        events      = symbolp->events;
        eventl      = symbolp->eventl;
        asciievents = symbolp->asciievents;
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - event %s", symbolp->asciidescs, asciievents);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
        type    = MARPAESLIF_EVENTTYPE_NULLED;
        events      = symbolp->events;
        eventl      = symbolp->eventl;
        asciievents = symbolp->asciievents;
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - event %s", symbolp->asciidescs, asciievents);
      case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
        type    = MARPAESLIF_EVENTTYPE_PREDICTED;
        events      = symbolp->events;
        eventl      = symbolp->eventl;
        asciievents = symbolp->asciievents;
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - event %s", symbolp->asciidescs, asciievents);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_EXHAUSTED:
        /* This is ok only if the recognizer is ok with exhaustion */
        if (! marpaESLIFRecognizerp->marpaESLIFRecognizerOption.exhaustedb) {
          MARPAESLIF_ERROR(marpaESLIFp, "Parse is exhausted");
          goto err;
        }
        type    = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        events      = NULL;
        eventl      = 0;
        asciievents = 0;
        MARPAESLIF_TRACE(marpaESLIFp, funcs, "Exhausted event");
        /* symboli will be -1 as per marpaWrapper spec */
        break;
      default:
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - unsupported event type %d", symbolp->asciidescs, grammarEventp[eventl].eventType);
        break;
      }

      if (events != NULL) {
        marpaESLIFRecognizerp->stringArrayp[okeventl].type   = type;
        marpaESLIFRecognizerp->stringArrayp[okeventl].events = events;
        marpaESLIFRecognizerp->stringArrayp[okeventl].eventl = eventl;
        marpaESLIFRecognizerp->stringArrayl = ++okeventl;
      }
    }
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static inline void _marpaESLIFAlternative_freev(marpaESLIFAlternative_t *marpaESLIFAlternativep)
/*****************************************************************************/
{
  if (marpaESLIFAlternativep != NULL) {
    if (marpaESLIFAlternativep->outputp != NULL) {
      if (marpaESLIFAlternativep->freeCallbackp != NULL) {
        marpaESLIFAlternativep->freeCallbackp(marpaESLIFAlternativep->userDatavp, marpaESLIFAlternativep->outputp);
      }
    }
    free(marpaESLIFAlternativep);
  }
}
