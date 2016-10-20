#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <genericLogger.h>
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

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, char *testFullMatchs, char *testPartialMatchs);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_meta_t     *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl);
static inline void                   _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, unsigned int leveli);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *marpaESLIFGrammarp);

static inline void                   _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp);

static inline genericStack_t        *_marpaESLIF_inputStack_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_inputStack_freev(genericStack_t *inputStackp);

static inline marpaESLIF_rule_t     *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp);

static inline short                  _marpaESLIFRecognizer_alternativeStack_initb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *alternativeStackp);
static inline void                   _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *alternativeStackp);

static inline void                   _marpaESLIF_alternative_freev(marpaESLIF_alternative_t *marpaESLIF_alternativep);
static inline marpaESLIFActionValue_t *_marpaESLIFActionValue_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIFActionValue_freev(marpaESLIFActionValue_t *marpaESLIFActionValuep);
static inline short                  _marpaESLIFActionValue_initb(marpaESLIF_t *marpaESLIFp, marpaESLIFActionValue_t *marpaESLIFActionValuep);
static inline void                   _marpaESLIFActionValue_resetv(marpaESLIFActionValue_t *marpaESLIFActionValuep);

static inline void                   _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
                                                                    unsigned int leveli,
                                                                    short warningIsErrorb,
                                                                    short warningIsIgnoredb,
                                                                    short autorankb,
                                                                    int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
                                                                    int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
                                                                    int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep);
static inline short                  _marpaESLIF_validate_grammarb(marpaESLIF_t *marpaESLIFp);

static inline short                  _marpaESLIF_terminal_string_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep);
static inline short                  _marpaESLIF_terminal_regex_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIF_grammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep);
static inline short                  _marpaESLIF_meta_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp);
static inline short                  _marpaESLIF_symbol_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_symbol_t *symbolp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp);

const static  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *utf8lp);
static inline void                   _marpaESLIF_tconv_freev(char *utf8s);

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(char *utf82printableasciip);
static        short                  _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp);
static inline short                  _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb, short *continuebp, short *exhaustedbp);
static inline marpaESLIF_symbol_t   *_marpaESLIFRecognizer_symbol_desc_to_ptr(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl);
static inline short                  _marpaESLIFRecognizer_alternativeb(marpaESLIF_t *marpaESLIFp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIFActionValue_t *marpaESLIFActionValuep);
static inline void                   _marpaESLIF_freeCallbackv(void *userDatavp, void *p);
static inline void                   _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_collect_grammar_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *exhaustedbp);
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFParentRecognizerp);
static inline short                  _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFParentRecognizerp, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp);
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_hexdumpv_LoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
static inline void                   _marpaESLIF_hexdumpv(marpaESLIF_t *marpaESLIFp, char *headers, char *asciidescs, const char *p, size_t lengthl);
#endif

static        short                  _marpaESLIFValueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static        short                  _marpaESLIFValueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static        short                  _marpaESLIFValueNullingCallback(void *userDatavp, int symboli, int resulti);
static        void                   _marpaESLIFActionValue_regex_free(void *userDatavp, genericStack_t *stackp);

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *marpaESLIFGrammarp, short startb, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, char *testFullMatchs, char *testPartialMatchs)
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
#ifndef MARPAESLIF_NTRACE
  marpaESLIF_matcher_value_t        rci;
  marpaESLIFActionValue_t           marpaESLIFActionValue;
  genericStack_t                   *marpaESLIFActionValuepStackp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY   array;
#endif

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
  terminalp->matcherbp    = NULL;

  marpaWrapperGrammarSymbolOption.terminalb = 1;
  marpaWrapperGrammarSymbolOption.startb    = startb;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Terminal Identifier ------------ */
  terminalp->idi = marpaWrapperGrammar_newSymboli(marpaESLIFGrammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
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
    terminalp->matcherbp = _marpaESLIF_terminal_string_matcherb;
    
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
    terminalp->u.regex.match_datap   = NULL;
#ifdef PCRE2_CONFIG_JIT
    terminalp->u.regex.jitCompleteb = 0;
    terminalp->u.regex.jitPartialb  = 0;
#endif
    terminalp->matcherbp = _marpaESLIF_terminal_regex_matcherb;
    
    if ((originp == NULL) || (originl <= 0)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - invalid terminal origin", terminalp->asciidescs);
      goto err;
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
    terminalp->u.regex.match_datap = pcre2_match_data_create(1 /* We are interested in the string that matched the full pattern */,
                                                             NULL /* Default memory allocation */);
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
  if (! _marpaESLIFActionValue_initb(marpaESLIFp, &marpaESLIFActionValue)) {
    goto err;
  }

  if (testFullMatchs != NULL) {

    if (! terminalp->matcherbp(marpaESLIFp, NULL, NULL, terminalp, NULL, testFullMatchs, strlen(testFullMatchs), 1, &rci, &marpaESLIFActionValue)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher general failure", terminalp->asciidescs);
      goto err;
    }
    if (rci != MARPAESLIF_MATCH_OK) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher returned rci = %d", terminalp->asciidescs, rci);
      goto err;
    }
    marpaESLIFActionValuepStackp = &(marpaESLIFActionValue.stack);
    if (GENERICSTACK_USED(marpaESLIFActionValuepStackp) != 1) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing full match is successful on %s but result is empty", terminalp->asciidescs, testFullMatchs);
      goto err;
    }
    array = GENERICSTACK_GET_ARRAY(marpaESLIFActionValuepStackp, 0);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing full match is successful on %s", terminalp->asciidescs, testFullMatchs);
  }

  _marpaESLIFActionValue_resetv(&marpaESLIFActionValue);

  if (testPartialMatchs != NULL) {

    marpaESLIFActionValuepStackp = &(marpaESLIFActionValue.stack);
    if (! terminalp->matcherbp(marpaESLIFp, NULL, NULL, terminalp, NULL, testPartialMatchs, strlen(testPartialMatchs), 0, &rci, &marpaESLIFActionValue)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher general failure", terminalp->asciidescs);
      goto err;
    }
    if (rci != MARPAESLIF_MATCH_AGAIN) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher returned rci = %d", terminalp->asciidescs, rci);
      goto err;
    }
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing partial match is successful on %s when not at EOF", terminalp->asciidescs, testPartialMatchs);
  }

  _marpaESLIFActionValue_resetv(&marpaESLIFActionValue);

#endif
  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  terminalp = NULL;

 done:
#ifndef MARPAESLIF_NTRACE
#endif
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
  metap->matcherbp                 = NULL;
  metap->marpaWrapperGrammarClonep = NULL; /* Eventuallwy changed when validating the grammar */

  marpaWrapperGrammarSymbolOption.terminalb = 0;
  marpaWrapperGrammarSymbolOption.startb    = startb;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Meta Identifier ------------ */
  metap->idi = marpaWrapperGrammar_newSymboli(marpaESLIFGrammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
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
  const static char          *funcs        = "_marpaESLIF_bootstrap_grammarp";
  marpaESLIF_symbol_t        *symbolp      = NULL;
  marpaESLIF_rule_t          *rulep        = NULL;
  marpaESLIF_terminal_t      *terminalp    = NULL;
  marpaESLIF_meta_t          *metap        = NULL;
  int                         lastStarti   = -1;
  int                         lastDiscardi = -1;
  marpaESLIF_grammar_t       *grammarp;
  marpaWrapperGrammarOption_t marpaWrapperGrammarOption;
  int                         i;
  short                       startb;
  short                       discardb;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Bootstrapping grammar at level %d", (int) leveli);

  marpaWrapperGrammarOption.genericLoggerp    = marpaESLIFp->marpaESLIFOption.genericLoggerp;
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
    symbolp->matcherbp   = terminalp->matcherbp;
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
    startb = bootstrap_grammar_metap[i].startb;
    if (startb) {
      if (lastStarti >= 0) {
        if (i != lastStarti) {
          MARPAESLIF_ERROR(marpaESLIFp, "There should be only one :start symbol");
          goto err;
        }
      } else {
        lastStarti = i;
      }
    }

    discardb = bootstrap_grammar_metap[i].discardb;
    if (discardb) {
      if (lastDiscardi >= 0) {
        if (i != lastDiscardi) {
          MARPAESLIF_ERROR(marpaESLIFp, "There should be only one :discard symbol");
          goto err;
        }
      } else {
        lastDiscardi = i;
        grammarp->discardSymbolp = symbolp;
      }
    }
    metap = _marpaESLIF_meta_newp(marpaESLIFp,
				  grammarp,
				  startb,
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
    symbolp->matcherbp  = metap->matcherbp;
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
    GENERICSTACK_SET_PTR(grammarp->ruleStackp, rulep, rulep->idi);
    if (GENERICSTACK_ERROR(grammarp->ruleStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "ruleStackp set failure, %s", strerror(errno));
      goto err;
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
      /* are precomputed as many times as needed: If level x is needing grammar at level x+1, then */
      /* - grammar at level x+1 is created and not precomputed */
      /* - for every symbol at level x, the precomputed grammar of level x+1 is done by forcing the start symbol */
      /*   The result is pushed at symbol level on level x. */
      /*   This case is happening ONLY for meta matchers. */
      if (! marpaWrapperGrammar_precomputeb(grammarp->marpaWrapperGrammarStartp)) {
        MARPAESLIF_ERROR(marpaESLIFp, "Failure to precompute grammar at level 0");
        goto err;
      }
      /* Clone and precompute the grammar if there is a discard symbol */
      if (grammarp->discardSymbolp != NULL) {
        marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(grammarp->marpaWrapperGrammarStartp);
        if (marpaWrapperGrammarClonep == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "Failure to clone grammar at level %d", grammari);
          goto err;
        }
        if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, grammarp->discardSymbolp->idi)) {
            MARPAESLIF_ERRORF(marpaESLIFp, "Failure to precompute grammar at level %d with start symbol %s", grammari, grammarp->discardSymbolp->asciidescs);
            goto err;
        }
        grammarp->marpaWrapperGrammarDiscardp = marpaWrapperGrammarClonep;
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
          marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(nextGrammarp->marpaWrapperGrammarStartp);
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

  grammarp->marpaESLIFp                 = marpaESLIFp;
  grammarp->leveli                      = leveli;
  grammarp->marpaWrapperGrammarStartp   = NULL;
  grammarp->marpaWrapperGrammarDiscardp = NULL;
  grammarp->discardSymbolp              = NULL;
  grammarp->symbolStackp                = NULL;
  grammarp->ruleStackp                  = NULL;
  grammarp->discardSymbolStackp         = NULL;

  grammarp->marpaWrapperGrammarStartp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarStartp == NULL) {
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
    if (grammarp->marpaWrapperGrammarStartp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarStartp);
    }			       
    if (grammarp->marpaWrapperGrammarDiscardp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarDiscardp);
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
static inline genericStack_t *_marpaESLIF_inputStack_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  const static char   *funcs = "_marpaESLIF_inputStack_newp";
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
static inline void _marpaESLIF_inputStack_freev(genericStack_t *inputStackp)
/*****************************************************************************/
{
  if (inputStackp != NULL) {
    while (GENERICSTACK_USED(inputStackp) > 0) {
      if (GENERICSTACK_IS_PTR(inputStackp, GENERICSTACK_USED(inputStackp) - 1)) {
	marpaESLIFActionValue_t *marpaESLIFActionValuep = (marpaESLIFActionValue_t *) GENERICSTACK_POP_PTR(inputStackp);
        _marpaESLIFActionValue_freev(marpaESLIFActionValuep);
      } else {
	GENERICSTACK_USED(inputStackp)--;
      }
    }
    GENERICSTACK_FREE(inputStackp);
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
  rulep->idi = marpaWrapperGrammar_newRulei(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarRuleOption, lhsi, nrhsl, rhsip);
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
  symbolp->matcherbp       =  NULL;
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
  marpaESLIFp->marpaESLIFOption = *marpaESLIFOptionp;
  marpaESLIFp->grammarStackp    = NULL;

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
static inline short _marpaESLIF_terminal_string_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  const static char              *funcs = "_marpaESLIF_terminal_string_matcherb";
  short                           rcb   = 1;
  marpaESLIF_matcher_value_t      rci;
  marpaESLIF_string_t             marpaESLIF_string;
  size_t                          matchLengthl;
  genericStack_t                 *marpaESLIFActionValuepStackp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;

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
    /* When we match a string, we match a "constant" : no need to duplicate the memory */
    GENERICSTACK_ARRAY_LENGTH(array) = matchLengthl;
    GENERICSTACK_ARRAY_PTR(array) = marpaESLIF_string.stringp;
    /* Say so in the output, by saying the freeing is NULL */
    marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);
    GENERICSTACK_PUSH_ARRAY(marpaESLIFActionValuepStackp, array);
    if (GENERICSTACK_ERROR(marpaESLIFActionValuepStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFActionValuep->stack push_array failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFActionValuep->freeCallbackp = NULL;   /* No need to set userDatavp is freeCallbackp is NULL */
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
static inline short _marpaESLIF_terminal_regex_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  const static char              *funcs             = "_marpaESLIF_terminal_regex_matcherb";
  short                           rcb               = 1;
  marpaESLIF_matcher_value_t      rci;
  marpaESLIF_regex_t              marpaESLIF_regex;
  int                             pcre2Errornumberi;
  PCRE2_UCHAR                     pcre2ErrorBuffer[256];
  PCRE2_SIZE                     *pcre2_ovectorp;
  size_t                          matchLengthl;
  genericStack_t                 *marpaESLIFActionValuepStackp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
 
  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*********************************************************************************/

  if (inputl > 0) {

    marpaESLIF_regex = terminalp->u.regex;

    /* --------------------------------------------------------- */
    /* EOF mode:                                                 */
    /* return full match status: OK or FAILURE.                  */
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

    if (eofb) {
      if (pcre2Errornumberi < 0) {
        /* Only PCRE2_ERROR_NOMATCH is an acceptable error. */
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
        /* Full match is successful. */
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
    /* We have to remember wat was matched. */
    GENERICSTACK_ARRAY_LENGTH(array) = matchLengthl;
    GENERICSTACK_ARRAY_PTR(array) = malloc(matchLengthl);
    if (GENERICSTACK_ARRAY_PTR(array) == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(GENERICSTACK_ARRAY_PTR(array), inputs, matchLengthl);
    /* Say in the output that this is allocated memory */
    marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);
    GENERICSTACK_PUSH_ARRAY(marpaESLIFActionValuepStackp, array);
    if (GENERICSTACK_ERROR(marpaESLIFActionValuepStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFActionValuep->stack push_array failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFActionValuep->userDatavp    = marpaESLIFp;                       /* No need to set userDatavp is freeCallbackp is NULL */
    marpaESLIFActionValuep->freeCallbackp = _marpaESLIFActionValue_regex_free; /* No need to set userDatavp is freeCallbackp is NULL */
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
static inline short _marpaESLIF_meta_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp)
/*****************************************************************************/
{
  /* All in all, this routine is the core of this module, and the cause of recursion -; */
  const static char           *funcs                 = "_marpaESLIF_meta_matcherb";
  short                        rcb;
  marpaESLIFGrammar_t          marpaESLIFGrammar; /* Fake marpaESLIFGrammar with the grammar sent in the stack */
  marpaESLIF_grammar_t         grammar;
  marpaESLIF_grammar_t        *grammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption; /* This is an internal recognizer */

  /* A meta matcher is always using the grammar one level higher - grammar validator guaranteed that is exists. */
  /* Still the corresponded precomputed grammar is at current level, and sent on the stack.                     */
  grammarp                          = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFp->grammarStackp, marpaESLIFGrammarp->grammarp->leveli + 1);
  grammar                           = *grammarp;
  grammar.marpaWrapperGrammarStartp = marpaWrapperGrammarp;
  marpaESLIFGrammar.marpaESLIFp     = marpaESLIFp;
  marpaESLIFGrammar.grammarp        = &grammar;

  marpaESLIFRecognizerOption                   = marpaESLIFRecognizerp->marpaESLIFRecognizerOption;
  marpaESLIFRecognizerOption.disableThresholdb = 1;
  marpaESLIFRecognizerOption.exhaustedb        = 1;
  marpaESLIFRecognizerOption.latmb             = 1;

  if (! _marpaESLIFGrammar_parseb(&marpaESLIFGrammar, &marpaESLIFRecognizerOption, 0 /* discardb */, marpaESLIFRecognizerp, marpaESLIFActionValuep, exhaustedbp)) {
    goto err;
  }

  *rcip = MARPAESLIF_MATCH_OK;
  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIF_symbol_matcherb(marpaESLIF_t *marpaESLIFp, marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIF_symbol_t *symbolp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_matcher_value_t *rcip, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp)
/*****************************************************************************/
{
  const static char              *funcs = "_marpaESLIF_symbol_matcherb";
  short                           rcb;
#ifndef MARPAESLIF_NTRACE
  genericStack_t                 *stackp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
#endif

  switch (symbolp->type) {
  case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
    rcb = symbolp->matcherbp(marpaESLIFp,
                             NULL /* marpaESLIFGrammarp */,
                             NULL /* marpaESLIFGrammarp->grammarp->marpaWrapperGrammarStartp */,
                             symbolp->u.terminalp,
                             NULL /* metap */,
                             marpaESLIFRecognizerp->inputs,
                             marpaESLIFRecognizerp->inputl,
                             marpaESLIFRecognizerp->eofb,
                             rcip,
                             marpaESLIFActionValuep);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      rcb = _marpaESLIF_meta_matcherb(marpaESLIFp,
                                      marpaESLIFGrammarp,
                                      symbolp->u.metap->marpaWrapperGrammarClonep,
                                      NULL /* terminalp */,
                                      symbolp->u.metap,
                                      marpaESLIFRecognizerp,
                                      rcip,
                                      marpaESLIFActionValuep,
                                      exhaustedbp);
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unknown symbol type %d", symbolp->type);
      goto err;
    }

  goto done;

 err:
  rcb = 0;

 done:
#ifndef MARPAESLIF_NTRACE
  if (rcb) {
    if (*rcip == MARPAESLIF_MATCH_OK) {
      stackp = &(marpaESLIFActionValuep->stack);
      array = GENERICSTACK_GET_ARRAY(stackp, 0);
      _marpaESLIF_hexdumpv(marpaESLIFp, "Match on", symbolp->asciidescs, GENERICSTACK_ARRAY_PTR(array), GENERICSTACK_ARRAY_LENGTH(array));
    }
  }
#endif
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
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
  const static char           *funcs                  = "marpaESLIFGrammar_newp";
  marpaESLIFRecognizer_t      *marpaESLIFRecognizerp  = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp     = NULL;
  char                        *utf8s                  = NULL;
  marpaESLIFActionValue_t      marpaESLIFActionValue;
  marpaESLIFGrammar_t          marpaESLIFGrammarTmp;
  size_t                       utf8l;
  marpaESLIF_readerContext_t   marpaESLIF_readerContext;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  genericStack_t              *marpaESLIFActionValuepStackp;

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
  marpaESLIFGrammarp->marpaESLIFValuep        = NULL;

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

  marpaESLIFGrammarTmp.marpaESLIFp = marpaESLIFp;
  marpaESLIFGrammarTmp.grammarp    = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFp->grammarStackp, 0);

  if (! marpaESLIFGrammar_parseb(&marpaESLIFGrammarTmp, &marpaESLIFRecognizerOption, &marpaESLIFActionValue, NULL /* exhaustedbp */)) {
    goto err;
  }

  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  _marpaESLIF_tconv_freev(utf8s);
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp);
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, 0 /* discardb */, NULL /* marpaESLIFParentRecognizerp */);
}

/*****************************************************************************/
short marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;

  if (marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan can be done one once only");
    return 0;
  }

  marpaESLIFRecognizerp->scanb = 1;
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0 /* Do not ignore pre-events */, continuebp, exhaustedbp);
}

/*****************************************************************************/
short marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 1 /* Ignore pre-events */, continuebp, exhaustedbp);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char              *funcs                   = "_marpaESLIFRecognizer_resumeb";
  marpaESLIF_t                   *marpaESLIFp             = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp      = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp                = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *symbolStackp            = grammarp->symbolStackp;
  void                           *userDatavp              = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.userDatavp;
  marpaESLIFReader_t              readerp                 = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp;
  char                           *inputs                  = marpaESLIFRecognizerp->inputs;
  size_t                          inputl                  = marpaESLIFRecognizerp->inputl;
  short                           eofb                    = marpaESLIFRecognizerp->eofb;
  short                           latmb                   = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.latmb;
  char                           *matchedp                 = NULL;
  size_t                          maxMatchedl             = 0;
  marpaESLIF_alternative_t       *marpaESLIF_alternativep = NULL;
  marpaESLIFActionValue_t        *marpaESLIFActionValuep  = NULL;
  genericStack_t                  alternativeStack;
  genericStack_t                 *alternativeStackp = &alternativeStack;
  size_t                          nSymboll;
  int                            *symbolArrayp;
  size_t                          symboll;
  int                             symboli;
  marpaESLIF_symbol_t            *symbolp;
  marpaESLIF_matcher_value_t      rci;
  size_t                          outputl;
  size_t                          matchedl;
  short                           rcb;
  short                           exhaustedb;
  short                           continueb;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  genericStack_t                 *marpaESLIFActionValuepStackp;
  marpaESLIFRecognizerOption_t    marpaESLIFRecognizerOptionDiscard = {
    userDatavp,
    readerp,
    1, /* disableThresholdb */
    1  /* exhaustedb */
  };

  if (! marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan must be called first");
    return 0;
  }

  /* Always initalize internal alternative stack */
  if (! _marpaESLIFRecognizer_alternativeStack_initb(marpaESLIFRecognizerp, alternativeStackp)) {
    goto err;
  }

  /* Always reset events */
  _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizerp);
  
  if (! ignorePreviousEventsb) {
    if (! _marpaESLIFRecognizer_collect_grammar_events(marpaESLIFRecognizerp, &exhaustedb)) {
      goto err;
    }
    if (exhaustedb || (marpaESLIFRecognizerp->stringArrayl > 0)) {
      /* Exhaustion or events: return immediately */
      rcb = exhaustedb ? 0 : 1;
      goto done;
    }
    /* No grammar event - resume can continue */
  }
  
  /* Ask for expected lexemes */
  if (! marpaESLIFRecognizer_expectedb(marpaESLIFRecognizerp, &nSymboll, &symbolArrayp)) {
    goto err;
  }

  /* Always reset internal alternative stack */
  _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizerp, alternativeStackp);

  /* Try to match */
 retry:
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - trying to match", symbolp->asciidescs);
  match_again:
    /* Always allocate a marpaESLIFActionValue if needed */
    if (marpaESLIFActionValuep == NULL) {
      marpaESLIFActionValuep = _marpaESLIFActionValue_newp(marpaESLIFp);
      if (marpaESLIFActionValuep == NULL) {
        goto err;
      }
      marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);
    } else {
      _marpaESLIFActionValue_resetv(marpaESLIFActionValuep);
      /* Note that its stack adress does not change */
    }
    if (! _marpaESLIF_symbol_matcherb(marpaESLIFp, marpaESLIFGrammarp, symbolp, marpaESLIFRecognizerp, &rci, marpaESLIFActionValuep, &exhaustedb)) {
      continue;
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
      /* Only in this case, it is possible that matchedp contains allocated memory */

      /* Our internal matchers always use an array as a result */
      if (! GENERICSTACK_IS_ARRAY(marpaESLIFActionValuepStackp, 0)) {
        MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFActionValuepStackp first member is not an genericStack array");
        goto err;
      }
      array = GENERICSTACK_GET_ARRAY(marpaESLIFActionValuepStackp, 0);
      if (GENERICSTACK_ARRAY_LENGTH(array) > maxMatchedl) {
        maxMatchedl = matchedl;
      }

      /* Remember that this alternative is associated to this symbol */
      marpaESLIF_alternativep = (marpaESLIF_alternative_t *) malloc(sizeof(marpaESLIF_alternative_t));
      if (marpaESLIF_alternativep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - malloc failure, %s", symbolp->asciidescs, strerror(errno));
        goto err;
      }
      marpaESLIF_alternativep->symbolp                = symbolp;
      marpaESLIF_alternativep->marpaESLIFActionValuep = marpaESLIFActionValuep;
      marpaESLIFActionValuep = NULL; /* Now marpaESLIFActionValuep is in marpaESLIF_alternativep */
      
      GENERICSTACK_PUSH_PTR(alternativeStackp, marpaESLIF_alternativep);
      if (GENERICSTACK_ERROR(alternativeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - alternativeStackp push failure, %s", symbolp->asciidescs, strerror(errno));
        goto err;
      }
      marpaESLIF_alternativep = NULL; /* Now marpaESLIF_alternativep is in alternativeStackp */
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported matcher return code %d", rci);
      goto err;
    }
  }
  
  /* No alternative ? unless exhaustion, we want to discard */
  if (GENERICSTACK_USED(alternativeStackp) <= 0) {
    /* Try to match the discard symbol */
    if (grammarp->marpaWrapperGrammarDiscardp != NULL) {
      /* Re-allocate a marpaESLIFActionValuep if needed */
      if (marpaESLIFActionValuep == NULL) {
        marpaESLIFActionValuep = _marpaESLIFActionValue_newp(marpaESLIFp);
        if (marpaESLIFActionValuep == NULL) {
          goto err;
        }
        marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);
      } else {
        _marpaESLIFActionValue_resetv(marpaESLIFActionValuep);
        /* Note that its stack adress does not change */
      }
      if (_marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOptionDiscard, 1 /* discardb */, marpaESLIFRecognizerp /* marpaESLIFParentRecognizerp */, marpaESLIFActionValuep, &exhaustedb)) {
        /* Discard symbol got matched. Retry our our own match */
        goto retry;
      }
    }
    MARPAESLIF_TRACE(marpaESLIFp, funcs, "No match");
    goto err;
  }

  /* Generate the eventual pre-lexeme events */
  /* Switch to user space now */

  /* Push alternatives */
  while (GENERICSTACK_USED(alternativeStackp) > 0) {
    marpaESLIF_alternative_t *marpaESLIF_alternative_shallowp = (marpaESLIF_alternative_t *) GENERICSTACK_POP_PTR(alternativeStackp);

    if (latmb) {
      /* If latm mode is true, keep only the longests alternatives */
      marpaESLIFActionValuep = marpaESLIF_alternative_shallowp->marpaESLIFActionValuep;
      marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);
      array = GENERICSTACK_GET_ARRAY(marpaESLIFActionValuepStackp, 0);
      if (GENERICSTACK_ARRAY_LENGTH(array) < maxMatchedl) {
        MARPAESLIF_TRACEF(marpaESLIFp,
                          funcs,
                          "%s - alternative is skipped (length %ld < max length %ld)",
                          marpaESLIF_alternative_shallowp->symbolp->asciidescs,
                          (unsigned long) GENERICSTACK_ARRAY_LENGTH(array),
                          (unsigned long) maxMatchedl);
        continue;
      }      
    }
    if (! _marpaESLIFRecognizer_alternativeb(marpaESLIFp,
                                             marpaESLIFRecognizerp,
                                             marpaESLIF_alternative_shallowp->symbolp,
                                             marpaESLIFActionValuep)) {
      goto err;
    }
  }

  /* Commit */
  if (! marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp)) {
    goto err;
  }

  /* We say to continue unless exhaustion or eof */
  continueb = exhaustedb ? exhaustedb : marpaESLIFRecognizerp->eofb;
  
  rcb = 1;
  if (exhaustedbp != NULL) {
    *exhaustedbp = exhaustedb;
  }
  if (continuebp != NULL) {
    *continuebp = continueb;
  }
  goto done;

 err:
  rcb = 0;

 done:
  if (matchedp != NULL) {
    free(matchedp);
  }
  _marpaESLIFActionValue_freev(marpaESLIFActionValuep);
  _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizerp, alternativeStackp);
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d (*exhaustedbp=%d)", (int) rcb, (int) exhaustedb);
  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbolnamecp, size_t symbolnamel, marpaESLIFActionValue_t *marpaESLIFActionValuep)
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

  return _marpaESLIFRecognizer_alternativeb(marpaESLIFp, marpaESLIFRecognizerp, symbolp, marpaESLIFActionValuep);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_alternativeb(marpaESLIF_t *marpaESLIFp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIFRecognizer_alternativeb";

  GENERICSTACK_PUSH_PTR(marpaESLIFRecognizerp->inputStackp, marpaESLIFActionValuep);
  if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->inputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "inputStackp push failure, %s", strerror(errno));
    return 0;
  }

  return marpaWrapperRecognizer_alternativeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symbolp->idi, GENERICSTACK_USED(marpaESLIFRecognizerp->inputStackp) - 1, 1);
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
    marpaESLIFValue_freev(marpaESLIFGrammarp->marpaESLIFValuep);
    free(marpaESLIFGrammarp);
  }
}

/*****************************************************************************/
void marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIF_t           *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
    marpaESLIFRecognizer_t *marpaESLIFParentRecognizerp = marpaESLIFRecognizerp->parentRecognizerp;
    
    _marpaESLIF_inputStack_freev(marpaESLIFRecognizerp->inputStackp);
    marpaWrapperRecognizer_freev(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
    if (marpaESLIFRecognizerp->stringArrayp != NULL) {
      free(marpaESLIFRecognizerp->stringArrayp);
    }
    /* If there is a parent recognizer, update stream information */
    if (marpaESLIFParentRecognizerp != NULL) {
      marpaESLIFParentRecognizerp->inputs = marpaESLIFRecognizerp->inputs;
      marpaESLIFParentRecognizerp->inputl = marpaESLIFRecognizerp->inputl;
      marpaESLIFParentRecognizerp->eofb   = marpaESLIFRecognizerp->eofb;
    }
    free(marpaESLIFRecognizerp);
  }
}

/*****************************************************************************/
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp)
/*****************************************************************************/
{
  short rcb;

  if (! _marpaESLIFActionValue_initb(marpaESLIFGrammarp->marpaESLIFp, marpaESLIFActionValuep)) {
    goto err;
  }

  if (! _marpaESLIFGrammar_parseb(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, 0 /* discardb */, NULL /* marpaESLIFParentRecognizerp */, marpaESLIFActionValuep, exhaustedbp)) {
    goto err;
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
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
static inline marpaESLIFActionValue_t *_marpaESLIFActionValue_newp(marpaESLIF_t *marpaESLIFp)
/*****************************************************************************/
{
  marpaESLIFActionValue_t *marpaESLIFActionValuep;
  genericStack_t          *marpaESLIFActionValuepStackp;

  marpaESLIFActionValuep = (marpaESLIFActionValue_t *) malloc(sizeof(marpaESLIFActionValue_t));
  if (marpaESLIFActionValuep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    return NULL;
  }

  if (! _marpaESLIFActionValue_initb(marpaESLIFp, marpaESLIFActionValuep)) {
    free(marpaESLIFActionValuep);
    return NULL;
  }

  return marpaESLIFActionValuep;
}

/*****************************************************************************/
static inline void _marpaESLIF_alternative_freev(marpaESLIF_alternative_t *marpaESLIF_alternativep)
/*****************************************************************************/
{
  if (marpaESLIF_alternativep != NULL) {
    marpaESLIFActionValue_t *marpaESLIFActionValuep = marpaESLIF_alternativep->marpaESLIFActionValuep;

    _marpaESLIFActionValue_freev(marpaESLIFActionValuep);
    free(marpaESLIF_alternativep);
  }
}

/*****************************************************************************/
static inline void _marpaESLIFActionValue_freev(marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  if (marpaESLIFActionValuep != NULL) {
    _marpaESLIFActionValue_resetv(marpaESLIFActionValuep);
    free(marpaESLIFActionValuep);
  }
}

/*****************************************************************************/
static inline short _marpaESLIFActionValue_initb(marpaESLIF_t *marpaESLIFp, marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  if (marpaESLIFActionValuep != NULL) {
    genericStack_t *marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);

    GENERICSTACK_INIT(marpaESLIFActionValuepStackp);
    if (GENERICSTACK_ERROR(marpaESLIFActionValuepStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFActionValuepStackp initialization failure, %s", strerror(errno));
      return 0;
    }
    marpaESLIFActionValuep->userDatavp    = NULL;
    marpaESLIFActionValuep->freeCallbackp = NULL;
  } else {
    MARPAESLIF_ERROR(marpaESLIFp, "marpaESLIFActionValuep is NULL");
    return 0;
  }

  return 1;
}

/*****************************************************************************/
static inline void _marpaESLIFActionValue_resetv(marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  if (marpaESLIFActionValuep != NULL) {
    genericStack_t *marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);

    if (GENERICSTACK_USED(marpaESLIFActionValuepStackp) > 0) {
      if (marpaESLIFActionValuep->freeCallbackp != NULL) {
        marpaESLIFActionValuep->freeCallbackp(marpaESLIFActionValuep->userDatavp, marpaESLIFActionValuepStackp);
      }
      GENERICSTACK_USED(marpaESLIFActionValuepStackp) = 0;
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_alternativeStack_initb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *alternativeStackp)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIF_alternativeStack_initb";
  marpaESLIF_t      *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  short              rcb;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building alternative stack");

  GENERICSTACK_INIT(alternativeStackp);
  if (GENERICSTACK_ERROR(alternativeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp init failure, %s", strerror(errno));
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
static inline void _marpaESLIFRecognizer_alternativeStack_reset(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *alternativeStackp)
/*****************************************************************************/
{
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
static inline short _marpaESLIFRecognizer_collect_grammar_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *exhaustedbp)
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
  if (! marpaWrapperGrammar_eventb(grammarp->marpaWrapperGrammarStartp, &grammarEventl, &grammarEventp, 0)) {
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
        exhaustedb = 1;
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
  *exhaustedbp = exhaustedb;
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d (*exhaustedbp=%d)", (int) rcb, (int) exhaustedb);
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFParentRecognizerp)
/*****************************************************************************/
{
  const static char             *funcs       = "_marpaESLIFRecognizer_newp";
  marpaESLIF_t                  *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Building %s Recognizer", discardb ? ":discard" : ":start");

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

  marpaESLIFRecognizerp->marpaESLIFp                = marpaESLIFp;
  marpaESLIFRecognizerp->marpaESLIFGrammarp         = marpaESLIFGrammarp;
  marpaESLIFRecognizerp->marpaESLIFRecognizerOption = *marpaESLIFRecognizerOptionp;
  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = NULL;
  marpaESLIFRecognizerp->inputStackp                = NULL;
  /* If this is a parent recognizer get its stream information */
  if (marpaESLIFParentRecognizerp != NULL) {
    marpaESLIFRecognizerp->inputs                     = marpaESLIFParentRecognizerp->inputs;
    marpaESLIFRecognizerp->inputl                     = marpaESLIFParentRecognizerp->inputl;
    marpaESLIFRecognizerp->eofb                       = marpaESLIFParentRecognizerp->eofb;
  } else {
    marpaESLIFRecognizerp->inputs                     = NULL;
    marpaESLIFRecognizerp->inputl                     = 0;
    marpaESLIFRecognizerp->eofb                       = 0;
  }
  marpaESLIFRecognizerp->scanb                      = 0;
  marpaESLIFRecognizerp->stringArrayp               = NULL;
  marpaESLIFRecognizerp->stringArrayl               = 0;
  marpaESLIFRecognizerp->parentRecognizerp          = marpaESLIFParentRecognizerp;

  marpaWrapperRecognizerOption.genericLoggerp       = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperRecognizerOption.disableThresholdb    = marpaESLIFRecognizerOptionp->disableThresholdb;

  marpaESLIFRecognizerp->marpaWrapperRecognizerp    = marpaWrapperRecognizer_newp(discardb ?
                                                                                  marpaESLIFGrammarp->grammarp->marpaWrapperGrammarDiscardp
                                                                                  :
                                                                                  marpaESLIFGrammarp->grammarp->marpaWrapperGrammarStartp
                                                                                  , &marpaWrapperRecognizerOption);
  if (marpaESLIFRecognizerp->marpaWrapperRecognizerp == NULL) {
    goto err;
  }
  marpaESLIFRecognizerp->inputStackp = _marpaESLIF_inputStack_newp(marpaESLIFp);
  if (marpaESLIFRecognizerp->inputStackp == NULL) {
    goto err;
  }
  
  /* Marpa does not like value stack at indice 0 */
  GENERICSTACK_PUSH_PTR(marpaESLIFRecognizerp->inputStackp, NULL);
  if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->inputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "inputStackp push failure, %s", strerror(errno));
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
static inline short _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFParentRecognizerp, marpaESLIFActionValue_t *marpaESLIFActionValuep, short *exhaustedbp)
/*****************************************************************************/
{
  const static char      *funcs                 = "marpaESLIF_parseb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = NULL;
  short                   exhaustedb            = 0;
  short                   continueb             = 0;
  marpaESLIFValueOption_t marpaESLIFValueOption = marpaESLIFValueOption_default;
  short                   rcb;

  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, discardb, marpaESLIFParentRecognizerp);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
    goto err;
  }
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
      goto err;
    }
  }

  /* Free any eventual previous call */
  marpaESLIFValue_freev(marpaESLIFGrammarp->marpaESLIFValuep);
  marpaESLIFGrammarp->marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFGrammarp->marpaESLIFValuep == NULL) {
    goto err;
  }
  /* No loop because we ask for a non-ambigous parse tree value */
  if (! marpaESLIFValue_valueb(marpaESLIFGrammarp->marpaESLIFValuep, marpaESLIFActionValuep)) {
    goto err;
  }

  rcb = 1;
  if (exhaustedbp != NULL) {
    *exhaustedbp = exhaustedb;
  }
  goto done;
  
 err:
  rcb = 0;

 done:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

#ifndef MARPAESLIF_NTRACE
/* C.f. http://grapsus.net/blog/post/Hexadecimal-dump-in-C */
#ifndef MARPAESLIF_HEXDUMP_COLS
#define MARPAESLIF_HEXDUMP_COLS 16
#endif
typedef struct marpaESLIF_hexdump_context {
  marpaESLIF_t *marpaESLIFp;
  char         *s;
  size_t        l;
} marpaESLIF_hexdump_context_t;
/*****************************************************************************/
static void _marpaESLIF_hexdumpv_LoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  marpaESLIF_hexdump_context_t *contextp = (marpaESLIF_hexdump_context_t *) userDatavp;
  char                         *tmps;

  if (contextp->s == NULL) {
    contextp->s = strdup(msgs);
  } else {
    tmps = (char *) realloc(contextp->s, strlen(contextp->s) + strlen(msgs) + 1);
    if (tmps != NULL) {
      strcat(tmps, msgs);
      contextp->s = tmps;
    }
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_hexdumpv(marpaESLIF_t *marpaESLIFp, char *headers, char *asciidescs, const char *p, size_t lengthl)
/*****************************************************************************/
{
  const static char           *funcs = "_marpaESLIF_hexdumpv";
  marpaESLIF_hexdump_context_t marpaESLIF_hexdump_context;
  genericLogger_t             *genericLoggerp;
  size_t  i;
  size_t  j;

  marpaESLIF_hexdump_context.marpaESLIFp = marpaESLIFp;
  marpaESLIF_hexdump_context.s           = NULL;
  marpaESLIF_hexdump_context.l           = 0;

  genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_hexdumpv_LoggerCallback, (void *) &marpaESLIF_hexdump_context, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    return;
  }
  
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s on %s, length %ld", headers, asciidescs, (unsigned long) lengthl);
  for (i = 0; i < lengthl + ((lengthl % MARPAESLIF_HEXDUMP_COLS) ? (MARPAESLIF_HEXDUMP_COLS - lengthl % MARPAESLIF_HEXDUMP_COLS) : 0); i++) {
    /* print offset */
    if (i % MARPAESLIF_HEXDUMP_COLS == 0) {
      GENERICLOGGER_TRACEF(genericLoggerp, "0x%06x: ", i);
    }
    /* print hex data */
    if (i < lengthl) {
      GENERICLOGGER_TRACEF(genericLoggerp, "%02x ", 0xFF & ((char*)p)[i]);
    } else { /* end of block, just aligning for ASCII dump */
      GENERICLOGGER_TRACE(genericLoggerp, "   ");
    }
    /* print ASCII dump */
    if (i % MARPAESLIF_HEXDUMP_COLS == (MARPAESLIF_HEXDUMP_COLS - 1)) {
      for (j = i - (MARPAESLIF_HEXDUMP_COLS - 1); j <= i; j++) {
        if(j >= lengthl) { /* end of block, not really printing */
          GENERICLOGGER_TRACE(genericLoggerp, " ");
        }
        else if (isprint(0xFF & ((char*)p)[j])) { /* printable char */
          GENERICLOGGER_TRACEF(genericLoggerp, "%c", 0xFF & ((char*)p)[j]);
        }
        else { /* other char */
          GENERICLOGGER_TRACE(genericLoggerp, ".");
        }
      }
      if (marpaESLIF_hexdump_context.s != NULL) {
        MARPAESLIF_TRACE(marpaESLIFp, funcs, marpaESLIF_hexdump_context.s);
        free(marpaESLIF_hexdump_context.s);
        marpaESLIF_hexdump_context.s = NULL;
      }
    }
  }

  GENERICLOGGER_FREE(genericLoggerp);
}
#endif

/*****************************************************************************/
marpaESLIFValue_t *marpaESLIFValue_newp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIFValueOption_t *marpaESLIFValueOptionp)
/*****************************************************************************/
{
  const static char        *funcs             = "marpaESLIFValue_newp";
  marpaESLIF_t             *marpaESLIFp       = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFValue_t        *marpaESLIFValuep  = NULL;
  marpaWrapperValue_t      *marpaWrapperValuep = NULL;
  marpaWrapperValueOption_t marpaWrapperValueOption;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Value");

  if (marpaESLIFValueOptionp == NULL) {
    marpaESLIFValueOptionp = &marpaESLIFValueOption_default;
  }

  marpaESLIFValuep = (marpaESLIFValue_t *) malloc(sizeof(marpaESLIFValue_t));
  if (marpaESLIFValuep == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFValuep->marpaESLIFp            = marpaESLIFp;
  marpaESLIFValuep->marpaESLIFRecognizerp  = marpaESLIFRecognizerp;
  marpaESLIFValuep->marpaESLIFValueOption  = *marpaESLIFValueOptionp;
  marpaESLIFValuep->marpaWrapperValuep     = NULL;
  marpaESLIFValuep->outputStackp           = NULL;
  marpaESLIFValuep->context.ruleIdi        = -1;

  GENERICSTACK_NEW(marpaESLIFValuep->outputStackp);
  if (GENERICSTACK_ERROR(marpaESLIFValuep->outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaWrapperValueOption.genericLoggerp = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperValueOption.highRankOnlyb  = marpaESLIFValueOptionp->highRankOnlyb;
  marpaWrapperValueOption.orderByRankb   = marpaESLIFValueOptionp->orderByRankb;
  marpaWrapperValueOption.ambiguousb     = marpaESLIFValueOptionp->ambiguousb;
  marpaWrapperValueOption.nullb          = marpaESLIFValueOptionp->nullb;
  marpaWrapperValueOption.maxParsesi     = marpaESLIFValueOptionp->maxParsesi;
  marpaWrapperValuep = marpaWrapperValue_newp(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &marpaWrapperValueOption);
  if (marpaWrapperValuep == NULL) {
    goto err;
  }

  marpaESLIFValuep->marpaWrapperValuep = marpaWrapperValuep;
  goto done;

 err:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFValuep = NULL;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFValuep);
  return marpaESLIFValuep;
}

/*****************************************************************************/
short marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep, marpaESLIFActionValue_t *marpaESLIFActionValuep)
/*****************************************************************************/
{
  const static char        *funcs       = "marpaESLIFValue_valueb";
  marpaESLIF_t             *marpaESLIFp = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFActionValue_t  *marpaESLIFActionValue0p;
  short                     rcb;
  
  if (! marpaWrapperValue_valueb(marpaESLIFValuep->marpaWrapperValuep,
                                 marpaESLIFValuep, /* Our value context */
                                 _marpaESLIFValueRuleCallback,
                                 _marpaESLIFValueSymbolCallback,
                                 _marpaESLIFValueNullingCallback)) {
    goto err;
  }

  if (marpaESLIFActionValuep != NULL) {
    /* The value is always at indice 0 of output stack */
    if (! GENERICSTACK_IS_PTR(marpaESLIFValuep->outputStackp, 0)) {
      MARPAESLIF_ERROR(marpaESLIFp, "No entry in outputStack at indice 0");
      goto err;
    }
    marpaESLIFActionValue0p = GENERICSTACK_GET_PTR(marpaESLIFValuep->outputStackp, 0);
    *marpaESLIFActionValuep = *marpaESLIFActionValue0p;
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
void marpaESLIFValue_freev(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  if (marpaESLIFValuep != NULL) {
    marpaWrapperValue_t *marpaWrapperValuep = marpaESLIFValuep->marpaWrapperValuep;
    if (marpaWrapperValuep != NULL) {
      marpaWrapperValue_freev(marpaWrapperValuep);
    }
    GENERICSTACK_FREE(marpaESLIFValuep->outputStackp);
    free(marpaESLIFValuep);
  }
}

/*****************************************************************************/
static short _marpaESLIFValueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  marpaESLIFValue_t *marpaESLIFValuep = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t      *marpaESLIFp       = marpaESLIFValuep->marpaESLIFp;
}

/*****************************************************************************/
static short _marpaESLIFValueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti)
/*****************************************************************************/
{
  const static char              *funcs                 = "marpaESLIFValueSymbolCallback";
  marpaESLIFValue_t              *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp              = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *symbolStackp          = grammarp->symbolStackp;
  genericStack_t                 *inputStackp           = marpaESLIFRecognizerp->inputStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->outputStackp;
  char                           *tmpp                  = NULL;
  marpaESLIF_symbol_t            *symbolp;
  marpaESLIFActionValue_t        *marpaESLIFActionValuep;
  genericStack_t                 *marpaESLIFActionValuepStackp;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  size_t                          matchedl;
  short                           rcb;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);

  if (! GENERICSTACK_IS_PTR(inputStackp, argi)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such entry in inputStackp at indice %d", argi);
    goto err;
  }
  marpaESLIFActionValuep = (marpaESLIFActionValue_t *) GENERICSTACK_GET_PTR(inputStackp, argi);
  marpaESLIFActionValuepStackp = &(marpaESLIFActionValuep->stack);

  if (grammarp->leveli > 0) {
    /* In a subgrammar, the actionvalue stack is always a single item, that is the concatenation */
    /* of all matches. */
    if (! GENERICSTACK_IS_ARRAY(marpaESLIFActionValuepStackp, 0)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "%s - Not an array at indice 0 of action value stack", symbolp->asciidescs);
      goto err;
    }
    array = GENERICSTACK_GET_ARRAY(marpaESLIFActionValuepStackp, 0);

    /* And by definition, the length is always > 0 */
    matchedl = GENERICSTACK_ARRAY_LENGTH(array);
    tmpp = (char *) malloc(matchedl);
    if (tmpp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    memcpy(tmpp, GENERICSTACK_ARRAY_PTR(array), matchedl);
    GENERICSTACK_SET_PTR(outputStackp, tmpp, resulti);
    if (GENERICSTACK_ERROR(outputStackp)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp set failure, %s", strerror(errno));
      goto err;
    }
    tmpp = NULL; /* tmpp is now in the output stack */
  } else {
  }

  rcb = 1;
  goto done;

 err:
  if (tmpp != NULL) {
    free(tmpp);
  }
  rcb = 0;

 done:
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static short _marpaESLIFValueNullingCallback(void *userDatavp, int symboli, int resulti)
/*****************************************************************************/
{
}


/*****************************************************************************/
int marpaESLIFValue_rulei(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  return marpaESLIFValuep->context.ruleIdi;
}

/*****************************************************************************/
marpaESLIFGrammar_t *marpaESLIFValue_grammarp(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  return marpaESLIFValuep->marpaESLIFRecognizerp->marpaESLIFGrammarp;
}

/*****************************************************************************/
static void _marpaESLIFActionValue_regex_free(void *userDatavp, genericStack_t *stackp)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIFActionValue_regex_free";

  if (GENERICSTACK_IS_ARRAY(stackp, 0)) {
    GENERICSTACKITEMTYPE2TYPE_ARRAY array = GENERICSTACK_GET_ARRAY(stackp, 0);
    
    if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
      free(GENERICSTACK_ARRAY_PTR(array));
#ifndef MARPAESLIF_NTRACE
    } else {
      /* Should never happen, this is why is get userDatavp only how */
      marpaESLIF_t *marpaESLIFp = (marpaESLIF_t *) userDatavp;
      MARPAESLIF_TRACE(marpaESLIFp, funcs, "Trying to free a NULL regex match");      
    }
#endif
  }
}
