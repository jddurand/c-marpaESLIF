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

/* This is very internal: I use the genericLogger to generate strings */
typedef struct _marpaESLIF_stringGenerator {
  marpaESLIF_t *marpaESLIFp;
  char         *s;
  size_t        l;
} _marpaESLIF_stringGenerator_t;

const static char *GENERICSTACKITEMTYPE_NA_STRING      = "NA";
const static char *GENERICSTACKITEMTYPE_CHAR_STRING    = "CHAR";
const static char *GENERICSTACKITEMTYPE_SHORT_STRING   = "SHORT";
const static char *GENERICSTACKITEMTYPE_INT_STRING     = "INT";
const static char *GENERICSTACKITEMTYPE_LONG_STRING    = "LONG";
const static char *GENERICSTACKITEMTYPE_FLOAT_STRING   = "FLOAT";
const static char *GENERICSTACKITEMTYPE_DOUBLE_STRING  = "DOUBLE";
const static char *GENERICSTACKITEMTYPE_PTR_STRING     = "PTR";
const static char *GENERICSTACKITEMTYPE_ARRAY_STRING   = "ARRAY";
const static char *GENERICSTACKITEMTYPE_UNKNOWN_STRING = "UNKNOWN";

static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, char *testFullMatchs, char *testPartialMatchs);
static inline void                   _marpaESLIF_terminal_freev(marpaESLIF_terminal_t *terminalp);

static inline marpaESLIF_meta_t     *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl);
static inline void                   _marpaESLIF_meta_freev(marpaESLIF_meta_t *metap);

static inline marpaESLIF_grammar_t  *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, int leveli, char *descs, size_t descl);
static inline void                   _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *grammarp);

static inline void                   _marpaESLIF_ruleStack_freev(genericStack_t *ruleStackp);
static inline void                   _marpaESLIF_lexemeStack_freev(genericStack_t *lexemeStackp);
static inline void                   _marpaESLIF_lexemeStack_resetv(genericStack_t *lexemeStackp);
static inline short                  _marpaESLIF_lexemeStack_ix_resetb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix);
static inline short                  _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, size_t *sizelp);
static inline short                  _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, char **pp);
static inline const char            *_marpaESLIF_genericStack_ix_types(genericStack_t *stackp, int ix);

static inline marpaESLIF_rule_t     *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb);
static inline void                   _marpaESLIF_rule_freev(marpaESLIF_rule_t *rulep);

static inline marpaESLIF_symbol_t   *_marpaESLIF_symbol_newp(marpaESLIF_t *marpaESLIFp);
static inline void                   _marpaESLIF_symbol_freev(marpaESLIF_symbol_t *symbolp);

static inline void                   _marpaESLIF_symbolStack_freev(genericStack_t *symbolStackp);

static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_L0p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammar_G1p(marpaESLIF_t *marpaESLIFp);
static inline marpaESLIF_grammar_t  *_marpaESLIF_bootstrap_grammarp(marpaESLIF_t *marpaESLIFp,
                                                                    int leveli,
                                                                    char  *descs,
                                                                    size_t descl,
                                                                    short warningIsErrorb,
                                                                    short warningIsIgnoredb,
                                                                    short autorankb,
                                                                    int bootstrap_grammar_terminali, bootstrap_grammar_terminal_t *bootstrap_grammar_terminalp,
                                                                    int bootstrap_grammar_metai, bootstrap_grammar_meta_t *bootstrap_grammar_metap,
                                                                    int bootstrap_grammar_rulei, bootstrap_grammar_rule_t *bootstrap_grammar_rulep);
static inline short                  _marpaESLIFGrammar_validateb(marpaESLIFGrammar_t *marpaESLIFGrammar);

static inline short                  _marpaESLIFGrammar_terminal_string_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp);
static inline short                  _marpaESLIFGrammar_terminal_regex_matcherb(marpaESLIFGrammar_t *marpaESLIF_grammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp);
static inline short                  _marpaESLIFGrammar_meta_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp);
static inline short                  _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp);

const static  char                  *_marpaESLIF_utf82printableascii_defaultp = "<!NOT TRANSLATED!>";
#ifndef MARPAESLIF_NTRACE
static        void                   _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs);
#endif

static inline char                  *_marpaESLIF_tconv_newp(marpaESLIF_t *marpaESLIFp, char *toEncodings, char *fromEncodings, char *descs, size_t descl, size_t *utf8lp);
static inline void                   _marpaESLIF_tconv_freev(char *utf8s);

static inline char                  *_marpaESLIF_utf82printableascii_newp(marpaESLIF_t *marpaESLIFp, char *descs, size_t descl);
static inline void                   _marpaESLIF_utf82printableascii_freev(char *utf82printableasciip);
static        short                  _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp);
static        short                  _marpaESLIFReader_grammarSkipper(void *userDatavp, size_t inputl);
static        short                  _marpaESLIFReader_grammarMarker(void *userDatavp, size_t limitl);
static        short                  _marpaESLIFReader_grammarResetter(void *userDatavp);
static        short                  _marpaESLIFReader_grammarCloser(void *userDatavp);
static inline short                  _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short ignorePreviousEventsb, short *continuebp, short *exhaustedbp);
static inline marpaESLIF_symbol_t   *_marpaESLIFRecognizer_symbol_findp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *descs, size_t descl);
static inline short                  _marpaESLIFRecognizer_alternativeb(marpaESLIF_t *marpaESLIFp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, int valuei);
static inline void                   _marpaESLIF_freeCallbackv(void *userDatavp, void *p);
static inline void                   _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizer_t *marpaESLIFRecognizerp);
static inline short                  _marpaESLIFRecognizer_collect_grammar_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completedbp, short *nulledbp, short *predictedbp, short *exhaustedbp);
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp);
static inline short                  _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short *exhaustedbp);
static        void                   _marpaESLIF_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs);
#ifndef MARPAESLIF_NTRACE
static inline void                   _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *headers, char *asciidescs, const char *p, size_t lengthl);
#endif

static        short                  _marpaESLIFValueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti);
static        short                  _marpaESLIFValueSymbolCallback(void *userDatavp, int symboli, int argi, int resulti);
static        short                  _marpaESLIFValueNullingCallback(void *userDatavp, int symboli, int resulti);
static        short                  _marpaESLIFActionCallback(void *userDatavp, char *names, size_t namel, int arg0i, int argni, int resulti);
static inline void                   _marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, short onStackb);
static        char                  *_marpaESLIFGrammar_symbolDescriptionCallback(void *userDatavp, int symboli);
static        short                  _marpaESLIFGrammar_symbolOptionSetterNoEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);
static        short                  _marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp);

/*****************************************************************************/
static inline marpaESLIF_terminal_t *_marpaESLIF_terminal_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl, marpaESLIF_terminal_type_t type, marpaESLIF_uint32_t opti, PCRE2_SPTR originp, PCRE2_SIZE originl, char *testFullMatchs, char *testPartialMatchs)
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
#endif

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building terminal"); */

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
  marpaWrapperGrammarSymbolOption.startb    = 0;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Terminal Identifier ------------ */
  terminalp->idi = marpaWrapperGrammar_newSymboli(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
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
    terminalp->matcherbp = _marpaESLIFGrammar_terminal_string_matcherb;
    
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
    terminalp->matcherbp = _marpaESLIFGrammar_terminal_regex_matcherb;
    
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
  {
    marpaESLIFGrammar_t marpaESLIFGrammar;

    marpaESLIFGrammar.marpaESLIFp   = marpaESLIFp;
    marpaESLIFGrammar.grammarStackp = NULL;
    marpaESLIFGrammar.grammarp      = grammarp;

    /* Look to the implementations of terminal matchers: they NEVER use leveli, nor marpaWrapperGrammarp, nor grammarStackp -; */
    
    if (testFullMatchs != NULL) {

      if (! terminalp->matcherbp(&marpaESLIFGrammar, 0, NULL, terminalp, NULL, testFullMatchs, strlen(testFullMatchs), 1, &rci, NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher general failure", terminalp->asciidescs);
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_OK) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing full match: matcher returned rci = %d", terminalp->asciidescs, rci);
        goto err;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing full match is successful on %s", terminalp->asciidescs, testFullMatchs); */
    }

    if (testPartialMatchs != NULL) {

      if (! terminalp->matcherbp(&marpaESLIFGrammar, 0, NULL, terminalp, NULL, testPartialMatchs, strlen(testPartialMatchs), 0, &rci, NULL)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher general failure", terminalp->asciidescs);
        goto err;
      }
      if (rci != MARPAESLIF_MATCH_AGAIN) {
        MARPAESLIF_ERRORF(marpaESLIFp, "%s - testing partial match: matcher returned rci = %d", terminalp->asciidescs, rci);
        goto err;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "%s - testing partial match is successful on %s when not at EOF", terminalp->asciidescs, testPartialMatchs); */
    }
  }

#endif
  goto done;
  
 err:
  _marpaESLIF_terminal_freev(terminalp);
  terminalp = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", terminalp); */
  return terminalp;
}

/*****************************************************************************/
static inline marpaESLIF_meta_t *_marpaESLIF_meta_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, int eventSeti, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIF_meta_newp";
  marpaESLIF_meta_t                *metap;
  marpaWrapperGrammarSymbolOption_t marpaWrapperGrammarSymbolOption;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building meta"); */

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
  metap->marpaWrapperGrammarClonep = NULL; /* Eventually changed when validating the grammar */

  marpaWrapperGrammarSymbolOption.terminalb = 0;
  marpaWrapperGrammarSymbolOption.startb    = 0;
  marpaWrapperGrammarSymbolOption.eventSeti = eventSeti;

  /* ----------- Meta Identifier ------------ */
  metap->idi = marpaWrapperGrammar_newSymboli(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarSymbolOption);
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
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", metap); */
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
                                        "L0",
                                        strlen("L0"),
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
                                        "G1",
                                        strlen("G1"),
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
								   int leveli,
                                                                   char  *descs,
                                                                   size_t descl,
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
  marpaWrapperGrammarOption.exhaustionEventb  = 1;
  
  grammarp = _marpaESLIF_grammar_newp(marpaESLIFp, &marpaWrapperGrammarOption, leveli, descs, descl);
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
    metap = _marpaESLIF_meta_newp(marpaESLIFp,
				  grammarp,
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
    symbolp->startb     = bootstrap_grammar_metap[i].startb;
    symbolp->discardb   = bootstrap_grammar_metap[i].discardb;
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
				  bootstrap_grammar_rulep[i].maskbp,
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
    /* Push is ok: rulep is in grammarp->ruleStackp */
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
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp); */
  return grammarp;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_validateb(marpaESLIFGrammar_t *marpaESLIFGrammarp)
/*****************************************************************************/
{
  const static char     *funcs                     = "_marpaESLIFGrammar_validateb";
  marpaESLIF_t          *marpaESLIFp               = marpaESLIFGrammarp->marpaESLIFp;
  genericStack_t        *grammarStackp             = marpaESLIFGrammarp->grammarStackp;
  marpaWrapperGrammar_t *marpaWrapperGrammarClonep = NULL;
  marpaESLIF_meta_t     *metap;
  genericStack_t        *symbolStackp;
  genericStack_t        *subSymbolStackp;
  genericStack_t        *ruleStackp;
  genericStack_t        *rhsStackp;
  int                    grammari;
  int                    subGrammari;
  marpaESLIF_symbol_t   *symbolp;
  marpaESLIF_symbol_t   *tmpSymbolp;
  marpaESLIF_symbol_t   *subSymbolp;
  int                    symboli;
  int                    subSymboli;
  marpaESLIF_rule_t     *rulep;
  int                    rulei;
  marpaESLIF_grammar_t  *grammarp;
  marpaESLIF_grammar_t  *subGrammarp;
  marpaESLIF_grammar_t  *tmpGrammarp;
  short                  lhsb;
  marpaESLIF_symbol_t   *lhsp;
  int                    rhsi;
  int                    lookupLevelDeltai;
  char                  *lookupGrammarNamep;
  size_t                 lookupGrammarNamel;
  marpaESLIF_symbol_t   *startp;
  marpaESLIF_symbol_t   *discardp;
  short                  rcb;
  marpaESLIF_cloneContext_t marpaESLIF_cloneContext = {
    marpaESLIFp,
    NULL /* grammarp */
  };
  marpaWrapperGrammarCloneOption_t marpaWrapperGrammarCloneOption = {
    (void *) &marpaESLIF_cloneContext,
    NULL,
    NULL /* ruleOptionSetterp */
  };

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Validating ESLIF grammar"); */

  /* The rules are:

   1. There must be a grammar at level 0
   2. Grammar at level 0 must precompute at its start symbol, grammar at level n at its eventual discard symbol
     a. Only one symbol can have the start flag
     b. Only one symbol can have the discard flag
   3. At any grammar level n, if a symbol never appear as an LHS of a rule, then
      it must be an LHS of grammar at level leveli, which must de-factor must also exist.
   4. At any grammar level n, if there is a :discard it must be precompute at this symbol

      It is not illegal to have sparse items in grammarStackp.
  */

  /*
   1. There must be a grammar at level 0
  */
  if (! GENERICSTACK_IS_PTR(grammarStackp, 0)) {
    MARPAESLIF_ERROR(marpaESLIFp, "No top-level grammar");
    goto err;
  }
  grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, 0);
  /*
   2. Grammar at level 0 must precompute at its start symbol, grammar at level n at its eventual discard symbol
     a. Only one symbol can have the start flag
     b. Only one symbol can have the discard flag
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    symbolStackp = grammarp->symbolStackp;

    if (grammarp->leveli == 0) {
      /* :start meta symbol check */
      startp = NULL;
      for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
        if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
          /* Should never happen, but who knows */
          continue;
        }
        symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
        if (symbolp->startb) {
          if (startp == NULL) {
            startp = symbolp;
          } else {
            MARPAESLIF_TRACEF(marpaESLIFp, funcs, "More than one :start symbol at grammar level %d (%s): symbols No %d (%s) and %d (%s)", grammari, grammarp->asciidescs, startp->idi, startp->asciidescs, symbolp->idi, symbolp->asciidescs);
            goto err;
          }
        }
      }
      if (startp == NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) at its default start symbol", grammari, grammarp->asciidescs);
        if (! marpaWrapperGrammar_precomputeb(grammarp->marpaWrapperGrammarStartp)) {
          goto err;
        }
      } else {
        MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) at start symbol No %d (%s)", grammari, grammarp->asciidescs, startp->idi, startp->asciidescs);
        if (! marpaWrapperGrammar_precompute_startb(grammarp->marpaWrapperGrammarStartp, startp->idi)) {
          goto err;
        }
      }
    } else {
      /* We clone the grammar to remove ANY eventual event set */
      marpaESLIF_cloneContext.grammarp = grammarp;
      marpaWrapperGrammarCloneOption.symbolOptionSetterp = _marpaESLIFGrammar_symbolOptionSetterNoEvent;
      marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarCloneOption);
      if (marpaWrapperGrammarClonep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Cleaning of events for grammar level %d (%s): cloning failure", grammari, grammarp->asciidescs);
        goto err;
      }
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarStartp);
      grammarp->marpaWrapperGrammarStartp = marpaWrapperGrammarClonep;
      marpaWrapperGrammarClonep = NULL;
    }

    /* :discard meta symbol check */
    discardp = NULL;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      if (symbolp->discardb) {
        if (discardp == NULL) {
          discardp = symbolp;
        } else {
          MARPAESLIF_TRACEF(marpaESLIFp, funcs, "More than one :discard symbol at grammar level %d (%s): symbols No %d (%s) and %d (%s)", grammari, grammarp->asciidescs, discardp->idi, discardp->asciidescs, symbolp->idi, symbolp->asciidescs);
          goto err;
        }
      }
    }
    if (discardp != NULL) {
      MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Precomputing grammar level %d (%s) at discard symbol No %d (%s)", grammari, grammarp->asciidescs, discardp->idi, discardp->asciidescs);
      /* Please note that we disable ALL events in any discarded precomputed grammar except completion on start symbol */
      marpaESLIF_cloneContext.grammarp = grammarp;
      marpaWrapperGrammarCloneOption.symbolOptionSetterp = _marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent;
      marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(grammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarCloneOption);
      if (marpaWrapperGrammarClonep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Precomputing grammar level %d (%s) at discard symbol No %d (%s): cloning failure", grammari, grammarp->asciidescs, discardp->idi, discardp->asciidescs);
        goto err;
      }
      if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, discardp->idi)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Precomputing grammar level %d (%s) at discard symbol No %d (%s): precompute failure", grammari, grammarp->asciidescs, discardp->idi, discardp->asciidescs);
        goto err;
      }
      grammarp->marpaWrapperGrammarDiscardp = marpaWrapperGrammarClonep;
      marpaWrapperGrammarClonep = NULL;
    }
  }
  /*
    3. In any rule of any grammar, an RHS can be at any level as well. Default being the current one.
    When the RHS level is the current level, if this RHS never appear as an LHS of another rule at the
    same level, then it must be an LHS of grammar at current level+1, which must de-factor must also exist.
    When the RHS is not at current level, then it must exist as an LHS of its specified level.
    
    Therefore every grammar is first scanned to detect all symbols that are truely LHS's at this level.
    Then every RHS of every rule is verified: it is must an LHS at its specified grammar level.
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {
    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      /* Sparse item in grammarStackp -; */
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at symbols in grammar level %d (%s)", grammari, grammarp->asciidescs);

    /* Loop on symbols */
    symbolStackp = grammarp->symbolStackp;
    ruleStackp = grammarp->ruleStackp;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      /* Only meta symbols should be looked at: if not an LHS then it is a dependency on a LHS of another grammar */
      if (symbolp->type != MARPAESLIF_SYMBOL_TYPE_META) {
        continue;
      }

      lhsb = 0;
      for (rulei = 0; rulei < GENERICSTACK_USED(ruleStackp); rulei++) {
        if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
          /* Should never happen, but who knows */
          continue;
        }
        rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
        lhsp = rulep->lhsp;
        if (lhsp->lhsb) {
          /* Symbol already marked */
        }
        if (lhsp->descl == symbolp->descl) {
          if (memcmp(lhsp->descs, symbolp->descs, symbolp->descl) == 0) {
            /* Found */
            MARPAESLIF_TRACEF(marpaESLIFp, funcs, "... Grammar level %d (%s): symbol %d (%s) marked as LHS", grammari, grammarp->asciidescs, lhsp->idi, lhsp->asciidescs);
            lhsb = 1;
            break;
          }
        }
      }
      symbolp->lhsb = lhsb;
    }
  }

  /* From grammar point of view, an expected symbol will always be either symbols explicitely created as terminals,
     either symbols not being an LHS. Per definition symbols created as terminals cannot be LHS symbols: precomputing
     the grammar will automatically fail. This is made sure by always precomputing at least grammar at level 0, and
     by precomputing any needed grammar at any other level with an alternative starting symbol.
  */
  for (grammari = 0; grammari < GENERICSTACK_USED(grammarStackp); grammari++) {

    if (! GENERICSTACK_IS_PTR(grammarStackp, grammari)) {
      continue;
    }
    grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Looking at rules in grammar level %d (%s)", grammari, grammarp->asciidescs);

    symbolStackp = grammarp->symbolStackp;
    for (symboli = 0; symboli <= GENERICSTACK_USED(symbolStackp); symboli++) {
      if (symboli < GENERICSTACK_USED(symbolStackp)) {
        if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
          /* Should never happen, but who knows */
          continue;
        }
        symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      } else {
        /* Faked additional entry */
        if (rulep->separatorp == NULL) {
          break;
        } else {
          symbolp = rulep->separatorp;
        }
      }
      /* Only non LHS meta symbols should be looked at */
      if ((symbolp->type != MARPAESLIF_SYMBOL_TYPE_META) || symbolp->lhsb) {
        continue;
      }
      metap = symbolp->u.metap;
      /* Since we loop on symbols of every rule, it can very well happen that we hit */
      /* the same meta symbol more than once.                                        */
      if (metap->marpaWrapperGrammarClonep != NULL) {
        MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "... Grammar level %d (%s): symbol %d (%s) already processed", grammari, grammarp->asciidescs, symbolp->idi, symbolp->asciidescs);
        continue;
      }

      /* Level of RHS is either via a grammar description, either via a number */
      /* They are exclusive (c.f. the grammar).                                */

      lookupLevelDeltai  = symbolp->lookupLevelDeltai;
      lookupGrammarNamep = symbolp->lookupGrammarNamep;
      lookupGrammarNamel = symbolp->lookupGrammarNamel;
      subGrammarp = NULL;
      /* We always look to the eventual string first */
      if ((lookupGrammarNamep != NULL)) {
        /* Look for such a grammar description */
        for (subGrammari = 0; subGrammari < GENERICSTACK_USED(grammarStackp); subGrammari++) {
          if (! GENERICSTACK_IS_PTR(grammarStackp, subGrammari)) {
            continue;
          }
          tmpGrammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, grammari);
          if (tmpGrammarp->descs != NULL) {
            if (tmpGrammarp->descl == lookupGrammarNamel) {
              if (memcmp(tmpGrammarp->descs, lookupGrammarNamep, lookupGrammarNamel) == 0) {
                subGrammarp = tmpGrammarp;
                break;
              }
            }
          }
        }
      } else {
        /* RHS level is relative - if RHS level is 0 the we fall back to current grammar */
        subGrammari = grammari + lookupLevelDeltai;
        if ((subGrammari >= 0) && GENERICSTACK_IS_PTR(grammarStackp, subGrammari)) {
          subGrammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(grammarStackp, subGrammari);
        }
      }
      /* Impossible */
      if (subGrammarp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) rhs No %d (%s) is referencing a non-existing grammar ", grammari, grammarp->asciidescs, rulep->idi, rulep->asciidescs, rhsi + 1, symbolp->asciidescs);
        goto err;
      }
      /* Make sure this RHS is an LHS in sub grammar, ignoring the case where sub grammar would be current grammar */
      subSymbolStackp = subGrammarp->symbolStackp;
      subSymbolp = NULL;
      for (subSymboli = 0; subSymboli < GENERICSTACK_USED(subSymbolStackp); subSymboli++) {
        if (! GENERICSTACK_IS_PTR(subSymbolStackp, subSymboli)) {
          /* Should never happen, but who knows */
          continue;
        }
        tmpSymbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(subSymbolStackp, subSymboli);
        if (tmpSymbolp->descl == symbolp->descl) {
          if (memcmp(tmpSymbolp->descs, symbolp->descs, symbolp->descl) == 0) {
            subSymbolp = tmpSymbolp;
            break;
          }
        }
      }
      if (subSymbolp == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) rhs No %d (%s) is referencing a non-existing symbol at grammar level %d (%s) ", grammari, grammarp->asciidescs, rulep->idi, rulep->asciidescs, rhsi + 1, symbolp->asciidescs, subGrammarp->leveli, subGrammarp->asciidescs);
        goto err;
      }
      if (! subSymbolp->lhsb) {
        /* When sub grammar is current grammar, this mean that we require that this RHS is also an LHS - which is correct because we restricted symbol loop on meta symbols */
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) rhs No %d (%s) is referencing an existing symbol No %d (%s) at grammar level %d (%s) but it is not an LHS", grammari, grammarp->asciidescs, rulep->idi, rulep->asciidescs, rhsi + 1, symbolp->asciidescs, subSymbolp->idi, subSymbolp->asciidescs, subGrammarp->leveli, subGrammarp->asciidescs);
        goto err;
      }
      /* Very good - attach a precomputed grammar to this symbol, unless this is current grammar! */
      if (subGrammarp->leveli == grammarp->leveli) {
        continue;
      }
      /* MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "Looking at rules in grammar level %d (%s): rule %d (%s) rhs No %d (%s) is referencing an existing LHS symbol No %d (%s) at grammar level %d (%s) - precomputing corresponding grammar at this symbol", grammari, grammarp->asciidescs, rulep->idi, rulep->asciidescs, rhsi + 1, symbolp->asciidescs, subSymbolp->idi, subSymbolp->asciidescs, subGrammarp->leveli, subGrammarp->asciidescs); */
      marpaESLIF_cloneContext.grammarp = grammarp;
      marpaWrapperGrammarClonep = marpaWrapperGrammar_clonep(subGrammarp->marpaWrapperGrammarStartp, &marpaWrapperGrammarCloneOption);
      if (marpaWrapperGrammarClonep == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) rhs No %d (%s) is referencing an existing symbol No %d (%s) at grammar level %d (%s) and it is an LHS - cloning failure", grammari, grammarp->asciidescs, rulep->idi, rulep->asciidescs, rhsi + 1, symbolp->asciidescs, subSymbolp->idi, subSymbolp->asciidescs, subGrammarp->leveli, subGrammarp->asciidescs);
        goto err;
      }
      if (! marpaWrapperGrammar_precompute_startb(marpaWrapperGrammarClonep, subSymbolp->idi)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "Looking at rules in grammar level %d (%s): rule %d (%s) rhs No %d (%s) is referencing an existing symbol No %d (%s) at grammar level %d (%s) and it is an LHS - precompute failure", grammari, grammarp->asciidescs, rulep->idi, rulep->asciidescs, rhsi + 1, symbolp->asciidescs, subSymbolp->idi, subSymbolp->asciidescs, subGrammarp->leveli, subGrammarp->asciidescs);
        goto err;
      }
      /* Commit resolved level in symbol */
      symbolp->resolvedLeveli = subGrammarp->leveli;
      metap->marpaWrapperGrammarClonep = marpaWrapperGrammarClonep;
      marpaWrapperGrammarClonep = NULL;
      MARPAESLIF_TRACEF(marpaESLIFp,  funcs, "... Grammar level %d (%s): symbol %d (%s) have grammar resolved level set to %d", grammari, grammarp->asciidescs, symbolp->idi, symbolp->asciidescs, symbolp->resolvedLeveli);
    }
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  if (marpaWrapperGrammarClonep != NULL) {
    marpaWrapperGrammar_freev(marpaWrapperGrammarClonep);
  }

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIF_grammar_t *_marpaESLIF_grammar_newp(marpaESLIF_t *marpaESLIFp, marpaWrapperGrammarOption_t *marpaWrapperGrammarOptionp, int leveli, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char             *funcs          = "_marpaESLIF_grammar_newp";
  genericLogger_t               *genericLoggerp = NULL;
  marpaESLIF_grammar_t          *grammarp       = NULL;
  _marpaESLIF_stringGenerator_t  _marpaESLIF_stringGenerator;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building ESLIF grammar"); */

  grammarp = (marpaESLIF_grammar_t *) malloc(sizeof(marpaESLIF_grammar_t));
  if (grammarp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  grammarp->marpaESLIFp                 = marpaESLIFp;
  grammarp->leveli                      = leveli;
  grammarp->descs                       = NULL;
  grammarp->descl                       = 0;
  grammarp->asciidescs                  = NULL;
  grammarp->marpaWrapperGrammarStartp   = NULL;
  grammarp->marpaWrapperGrammarDiscardp = NULL;
  grammarp->discardSymbolp              = NULL;
  grammarp->symbolStackp                = NULL;
  grammarp->ruleStackp                  = NULL;

  grammarp->marpaWrapperGrammarStartp = marpaWrapperGrammar_newp(marpaWrapperGrammarOptionp);
  if (grammarp->marpaWrapperGrammarStartp == NULL) {
    goto err;
  }
  if (descs == NULL) {
    /* Generate a default description */
    _marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFp;
    _marpaESLIF_stringGenerator.s           = NULL;
    _marpaESLIF_stringGenerator.l           = 0;

    genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &_marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
    if (genericLoggerp == NULL) {
      goto err;
    }
    GENERICLOGGER_TRACEF(genericLoggerp, "Grammar level %d (autogenerated description)", leveli);
    if (_marpaESLIF_stringGenerator.s == NULL) {
      goto err;
    }
    grammarp->asciidescs = _marpaESLIF_stringGenerator.s;
    /* No need to check if it has to be freed at method return */
  } else {
    grammarp->asciidescs = _marpaESLIF_utf82printableascii_newp(marpaESLIFp, descs, descl);
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

  goto done;

 err:
  _marpaESLIF_grammar_freev(grammarp);
  grammarp = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", grammarp); */
  GENERICLOGGER_FREE(genericLoggerp);
  return grammarp;
}

/*****************************************************************************/
static inline void _marpaESLIF_grammar_freev(marpaESLIF_grammar_t *grammarp)
/*****************************************************************************/
{
  if (grammarp != NULL) {
    _marpaESLIF_utf82printableascii_freev(grammarp->asciidescs);
    if (grammarp->marpaWrapperGrammarStartp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarStartp);
    }			       
    if (grammarp->marpaWrapperGrammarDiscardp != NULL) {
      marpaWrapperGrammar_freev(grammarp->marpaWrapperGrammarDiscardp);
    }			       
    _marpaESLIF_symbolStack_freev(grammarp->symbolStackp);
    _marpaESLIF_ruleStack_freev(grammarp->ruleStackp);
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
static inline void _marpaESLIF_lexemeStack_freev(genericStack_t *lexemeStackp)
/*****************************************************************************/
{
  if (lexemeStackp != NULL) {
    _marpaESLIF_lexemeStack_resetv(lexemeStackp);
    GENERICSTACK_FREE(lexemeStackp);
  }
}

/*****************************************************************************/
static inline void _marpaESLIF_lexemeStack_resetv(genericStack_t *lexemeStackp)
/*****************************************************************************/
{
  if (lexemeStackp != NULL) {
    while (GENERICSTACK_USED(lexemeStackp) > 0) {
      if (GENERICSTACK_IS_ARRAY(lexemeStackp, GENERICSTACK_USED(lexemeStackp) - 1)) {
        GENERICSTACKITEMTYPE2TYPE_ARRAY array = GENERICSTACK_POP_ARRAY(lexemeStackp);
        if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
          free(GENERICSTACK_ARRAY_PTR(array));
        }
      } else {
	GENERICSTACK_USED(lexemeStackp)--;
      }
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIF_lexemeStack_ix_resetb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIF_lexemeStack_ix_resetb";
  short              rcb;

  if (lexemeStackp != NULL) {
    if (GENERICSTACK_IS_ARRAY(lexemeStackp, ix)) {
      GENERICSTACKITEMTYPE2TYPE_ARRAY array = GENERICSTACK_GET_ARRAY(lexemeStackp, ix);
      MARPAESLIF_TRACEF(marpaESLIFRecognizerp->marpaESLIFp, funcs, "Resetting at indice %d of lexemeStackp", ix);
      if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
        free(GENERICSTACK_ARRAY_PTR(array));
      }
      GENERICSTACK_SET_NA(lexemeStackp, ix);
      if (GENERICSTACK_ERROR(lexemeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "lexemeStackp set failure, %s", strerror(errno));
        goto err;
      }
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIF_rule_t *_marpaESLIF_rule_newp(marpaESLIF_t *marpaESLIFp, marpaESLIF_grammar_t *grammarp, char *descs, size_t descl, int lhsi, size_t nrhsl, int *rhsip, short *maskbp, size_t nexceptionl, int *exceptionip, int ranki, short nullRanksHighb, short sequenceb, int minimumi, int separatori, short properb)
/*****************************************************************************/
{
  const static char               *funcs        = "_marpaESLIF_rule_newp";
  genericStack_t                  *symbolStackp = grammarp->symbolStackp;
  short                            symbolFoundb = 0;
  short                            separatorFoundb;
  marpaESLIF_symbol_t             *symbolp;
  marpaESLIF_rule_t               *rulep;
  marpaWrapperGrammarRuleOption_t  marpaWrapperGrammarRuleOption;
  size_t                           i;
  short                            maskb;
  int                              symboli;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building rule"); */

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
  rulep->separatorp      = NULL;
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
    if (symbolp->idi == lhsi) {
      symbolFoundb = 1;
      break;
    }
  }
  if (! symbolFoundb) {
    MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS symbol No %d does not exist", grammarp->leveli, rulep->asciidescs, lhsi);
    goto err;
  }
  symbolp->lhsb = 1;
  rulep->lhsp = symbolp;

  /* Idem for the separator */
  if (sequenceb && (separatori >= 0)) {
    separatorFoundb = 0;
    for (symboli = 0; symboli < GENERICSTACK_USED(symbolStackp); symboli++) {
      if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
        /* Should never happen, but who knows */
        continue;
      }
      symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
      if (symbolp->idi == separatori) {
        separatorFoundb = 1;
        break;
      }
    }
    if (! separatorFoundb) {
      MARPAESLIF_ERRORF(marpaESLIFp, "At grammar level %d, rule %s: LHS separator No %d does not exist", grammarp->leveli, rulep->asciidescs, separatori);
      goto err;
    }
    rulep->separatorp = symbolp;
  }

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
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rulep); */
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

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building symbol"); */

  symbolp = (marpaESLIF_symbol_t *) malloc(sizeof(marpaESLIF_symbol_t));
  if (symbolp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
    goto done;
  }

  symbolp->type   = MARPAESLIF_SYMBOL_TYPE_NA;
  /* Union itself is undetermined at this stage */
  symbolp->startb             = 0;
  symbolp->discardb           = 0;
  symbolp->lhsb               = 0;
  symbolp->idi                = -1;
  symbolp->descs              = NULL;
  symbolp->descl              = 0;
  symbolp->asciidescs         = NULL;
  symbolp->matcherbp          = NULL;
  symbolp->pauseb             = 0;
  symbolp->pauseIsOnb         = 0;
  symbolp->pauses             = NULL;
  symbolp->pausel             = 0;
  symbolp->asciipauses        = NULL;
  symbolp->events             = NULL;
  symbolp->eventl             = 0;
  symbolp->asciievents        = NULL;
  symbolp->lookupLevelDeltai  = 1;   /* Default lookup is the next grammar level */
  symbolp->lookupGrammarNamep = NULL;
  symbolp->lookupGrammarNamel = 0;
  symbolp->resolvedLeveli     = -1; /* This will be overwriten by _marpaESLIFGrammar_validateb() and used only when symbol is a lexeme from another grammar */

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", symbolp); */
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
  const static char    *funcs       = "marpaESLIF_newp";
  marpaESLIF_grammar_t *grammarp    = NULL;
  marpaESLIF_t         *marpaESLIFp = NULL;
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
  marpaESLIFp->marpaESLIFOption   = *marpaESLIFOptionp;
  marpaESLIFp->marpaESLIFGrammarp = (marpaESLIFGrammar_t *) malloc(sizeof(marpaESLIFGrammar_t));

  if (marpaESLIFp->marpaESLIFGrammarp == NULL) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "malloc failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFp->marpaESLIFGrammarp->marpaESLIFp   = marpaESLIFp;
  marpaESLIFp->marpaESLIFGrammarp->grammarStackp = NULL;
  marpaESLIFp->marpaESLIFGrammarp->grammarp      = NULL;

  GENERICSTACK_NEW(marpaESLIFp->marpaESLIFGrammarp->grammarStackp);
  if (GENERICSTACK_ERROR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "marpaESLIFp->marpaESLIFGrammarp->grammarStackp initialization failure, %s", strerror(errno));
    goto err;
  }

  /* G1 */
  grammarp = _marpaESLIF_bootstrap_grammar_G1p(marpaESLIFp);
  if (grammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, grammarp, grammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "marpaESLIFp->marpaESLIFGrammarp->grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  grammarp = NULL;

  /* L0 */
  grammarp = _marpaESLIF_bootstrap_grammar_L0p(marpaESLIFp);
  if (grammarp == NULL) {
    goto err;
  }
  GENERICSTACK_SET_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, grammarp, grammarp->leveli);
  if (GENERICSTACK_ERROR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp)) {
    GENERICLOGGER_ERRORF(marpaESLIFOptionp->genericLoggerp, "marpaESLIFp->marpaESLIFGrammarp->grammarStackp set failure, %s", strerror(errno));
    goto err;
  }
  grammarp = NULL;

  /* Validate the bootstrap grammar */
  if (! _marpaESLIFGrammar_validateb(marpaESLIFp->marpaESLIFGrammarp)) {
    goto err;
  }

  /* Commit G1 grammar as the current grammar */
  if (! GENERICSTACK_IS_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, 0)) {
    GENERICLOGGER_ERROR(marpaESLIFOptionp->genericLoggerp, "No top level grammar after bootstrap");
    goto err;
  }
  marpaESLIFp->marpaESLIFGrammarp->grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFp->marpaESLIFGrammarp->grammarStackp, 0);
  goto done;
  
 err:
  marpaESLIF_freev(marpaESLIFp);
  marpaESLIFp = NULL;

 done:
  _marpaESLIF_grammar_freev(grammarp);
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
    marpaESLIFGrammar_freev(marpaESLIFp->marpaESLIFGrammarp);
    free(marpaESLIFp);
  }
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_terminal_string_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp)
/*****************************************************************************/
{
  const static char          *funcs = "_marpaESLIFGrammar_terminal_string_matcherb";
  marpaESLIF_t               *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIF_matcher_value_t  rci;
  marpaESLIF_string_t         marpaESLIF_string;
  size_t                      matchedLengthl;

  /*********************************************************************************/
  /* A matcher tries to match a terminal v.s. input that is eventually incomplete. */
  /* It return 1 on success, 0 on failure, -1 if more data is needed.              */
  /*********************************************************************************/

  if (inputl > 0) {

    marpaESLIF_string = terminalp->u.string;
    if (inputl >= marpaESLIF_string.stringl) {
      if (memcmp(inputs, marpaESLIF_string.stringp, marpaESLIF_string.stringl) == 0) {
        rci = MARPAESLIF_MATCH_OK;
        matchedLengthl = marpaESLIF_string.stringl;
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
    if (outputStackp != NULL) {
      /* Symbol callback will realize it is a PTR, i.e. a pointer to a marpaESLIF_string_t */
      GENERICSTACK_PUSH_PTR(outputStackp, &(terminalp->u.string));
      if (GENERICSTACK_ERROR(outputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "stack push failure, %s", strerror(errno));
        goto err;
      }
    }
  }

  goto done;

 err:
  return 0;

 done:
  /* Here it is guaranteed that tmps is always NULL as well */
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_terminal_regex_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, char *inputs, size_t inputl, short eofb, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp)
/*****************************************************************************/
{
  const static char              *funcs = "_marpaESLIFGrammar_terminal_regex_matcherb";
  marpaESLIF_t                   *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  char                           *tmps  = NULL;
  marpaESLIF_matcher_value_t      rci;
  marpaESLIF_regex_t              marpaESLIF_regex;
  int                             pcre2Errornumberi;
  PCRE2_UCHAR                     pcre2ErrorBuffer[256];
  PCRE2_SIZE                     *pcre2_ovectorp;
  size_t                          matchedLengthl;
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
        matchedLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchedLengthl <= 0) {
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
        matchedLengthl = pcre2_ovectorp[1] - pcre2_ovectorp[0];
        if (matchedLengthl <= 0) {
          MARPAESLIF_ERRORF(marpaESLIFp, "%s - Empty match when it is configured as not possible", terminalp->asciidescs);
          goto err;
        }
        if (matchedLengthl >= inputl) {
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
    if (outputStackp != NULL) {
      tmps = (char *) malloc(matchedLengthl);
      if (tmps == NULL) {
        MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
        goto err;
      }
      memcpy(tmps, inputs, matchedLengthl);
      GENERICSTACK_ARRAY_LENGTH(array) = matchedLengthl;
      GENERICSTACK_ARRAY_PTR(array) = tmps;
      /* Symbol callback will realize it is an ARRAY, i.e. allocated memory that should be released. */
      GENERICSTACK_PUSH_ARRAY(outputStackp, array);
      if (GENERICSTACK_ERROR(outputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "stack push failure, %s", strerror(errno));
        goto err;
      }
      tmps = NULL;
    }
  }

  goto done;

 err:
  /* Only here it is possible that tmps is != NULL */
  if (tmps != NULL) {
    free(tmps);
  }
  return 0;

 done:
  return 1;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_meta_matcherb(marpaESLIFGrammar_t *marpaESLIFGrammarp, int grammarLeveli, marpaWrapperGrammar_t *marpaWrapperGrammarp, marpaESLIF_terminal_t *terminalp, marpaESLIF_meta_t *metap, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp)
/*****************************************************************************/
{
  /* All in all, this routine is the core of this module, and the cause of recursion -; */
  const static char           *funcs       = "_marpaESLIFGrammar_meta_matcherb";
  marpaESLIF_t                *marpaESLIFp = marpaESLIFGrammarp->marpaESLIFp;
  short                        rcb;
  marpaESLIFGrammar_t          marpaESLIFGrammar; /* Fake marpaESLIFGrammar with the grammar sent in the stack */
  marpaESLIF_grammar_t         grammar;
  marpaESLIF_grammar_t        *grammarp;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption; /* This is an internal recognizer */
  marpaESLIFValueOption_t      marpaESLIFValueOption = marpaESLIFValueOption_default_template;
  
  /* A meta matcher is always using ANOTHER grammar at level grammarLeveli (validator guaranteed that is exists) that is sent on the stack. */
  /* Though the precomputed grammar is known to the symbol that called us, also sent on the stack. */
  if (! GENERICSTACK_IS_PTR(marpaESLIFGrammarp->grammarStackp, grammarLeveli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "At grammar No %d (%s), meta symbol No %d (%s) resolve to a grammar level %d that do not exist", marpaESLIFGrammarp->grammarp->leveli, marpaESLIFGrammarp->grammarp->asciidescs, metap->idi, metap->asciidescs, grammarLeveli);
    goto err;
  }
  grammarp                           = (marpaESLIF_grammar_t *) GENERICSTACK_GET_PTR(marpaESLIFGrammarp->grammarStackp, grammarLeveli);
  grammar                            = *grammarp;
  grammar.marpaWrapperGrammarStartp  = marpaWrapperGrammarp;
  marpaESLIFGrammar                  = *marpaESLIFGrammarp;
  marpaESLIFGrammar.grammarp         = &grammar;

  marpaESLIFRecognizerOption                   = marpaESLIFRecognizerParentp->marpaESLIFRecognizerOption;
  marpaESLIFRecognizerOption.disableThresholdb = 1;
  marpaESLIFRecognizerOption.exhaustedb        = 1;
  marpaESLIFRecognizerOption.latmb             = 1;

  marpaESLIFValueOption.userDatavp     = &marpaESLIFGrammar;
  marpaESLIFValueOption.valueCallbackp = _marpaESLIFActionCallback;
  marpaESLIFValueOption.outputStackp   = outputStackp;

  if (! _marpaESLIFGrammar_parseb(&marpaESLIFGrammar, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, 0 /* discardb */, marpaESLIFRecognizerParentp, exhaustedbp)) {
    goto err;
  }

  *rcip = MARPAESLIF_MATCH_OK;
  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, marpaESLIF_matcher_value_t *rcip, genericStack_t *outputStackp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char *funcs       = "_marpaESLIFRecognizer_symbol_matcherb";
  short              rcb;

  switch (symbolp->type) {
  case MARPAESLIF_SYMBOL_TYPE_TERMINAL:
    rcb = symbolp->matcherbp(marpaESLIFRecognizerp->marpaESLIFGrammarp,
                             -1, /* grammarLeveli */
                             NULL /* marpaESLIFGrammarp->grammarp->marpaWrapperGrammarStartp */,
                             symbolp->u.terminalp,
                             NULL /* metap */,
                             marpaESLIFRecognizerp->inputs,
                             marpaESLIFRecognizerp->inputl,
                             marpaESLIFRecognizerp->eofb,
                             rcip,
                             outputStackp);
      break;
    case MARPAESLIF_SYMBOL_TYPE_META:
      rcb = _marpaESLIFGrammar_meta_matcherb(marpaESLIFRecognizerp->marpaESLIFGrammarp,
                                             symbolp->resolvedLeveli, /* Computed by grammar validator */
                                             symbolp->u.metap->marpaWrapperGrammarClonep,
                                             NULL /* terminalp */,
                                             symbolp->u.metap,
                                             marpaESLIFRecognizerp,
                                             rcip,
                                             outputStackp,
                                             exhaustedbp);
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Unknown symbol type %d", symbolp->type);
      goto err;
    }

#ifndef MARPAESLIF_NTRACE
  if (rcb) {
    if (*rcip == MARPAESLIF_MATCH_OK) {
      size_t sizel;
      char  *p;

      if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, outputStackp, 0, &sizel)) {
        goto err;
      }
      if (! _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizerp, outputStackp, 0, &p)) {
        goto err;
      }

      if (sizel > 0) {
        _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizerp, "Match for ", symbolp->asciidescs, p, sizel);
      } else {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Match for %s is the empty string", symbolp->asciidescs);
      }
    }
  }
#endif

  goto done;

 err:
  rcb = 0;

 done:
  /*
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  */
  return rcb;
}

#ifndef MARPAESLIF_NTRACE
/*****************************************************************************/
static void _marpaESLIF_tconvTraceCallback(void *userDatavp, const char *msgs)
/*****************************************************************************/
{
  const static char *funcs  = "_marpaESLIF_tconvTraceCallback";
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

  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return \"%s\"", utf8s); */
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
    MARPAESLIF_ERRORF(marpaESLIFp, "tconv_open failure, %s", strerror(errno));
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
    MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
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
	MARPAESLIF_ERRORF(marpaESLIFp, "tconv failure, %s", strerror(errno));
	goto err;
      }
      /* Try to alloc more */
      outbuforigl *= 2;
      /* Will this ever happen ? */
      if (outbuforigl < descl) {
	MARPAESLIF_ERROR(marpaESLIFp, "size_t flip");
	goto err;
      }
      /* Note the "+ 1" */
      tmp = realloc(outbuforigp, outbuforigl + 1);
      if (tmp == NULL) {
	MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
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
      MARPAESLIF_ERRORF(marpaESLIFp, "tconv_close failure, %s", strerror(errno));
    }
  }

  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", rcp); */
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
  const static char           *funcs              = "marpaESLIFGrammar_newp";
  char                        *utf8s              = NULL;
  genericStack_t              *outputStackp       = NULL;
  marpaESLIFGrammar_t         *marpaESLIFGrammarp = NULL;
  size_t                       utf8l;
  marpaESLIF_readerContext_t   marpaESLIF_readerContext;
  marpaESLIFRecognizerOption_t marpaESLIFRecognizerOption;
  marpaESLIFValueOption_t      marpaESLIFValueOption;

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Grammar"); */

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

  marpaESLIFGrammarp->marpaESLIFp   = marpaESLIFp;
  marpaESLIFGrammarp->grammarStackp = NULL;
  marpaESLIFGrammarp->grammarp      = NULL;

  GENERICSTACK_NEW(marpaESLIFGrammarp->grammarStackp);
  if (GENERICSTACK_ERROR(marpaESLIFGrammarp->grammarStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "marpaESLIFGrammarp->grammarStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  /* We want to parse the incoming grammar in UTF-8 */
  utf8s = _marpaESLIF_tconv_newp(marpaESLIFp, "UTF-8", marpaESLIFGrammarOptionp->encodings, marpaESLIFGrammarOptionp->grammars, marpaESLIFGrammarOptionp->grammarl, &utf8l);
  if (utf8s == NULL) {
    goto err;
  }

  marpaESLIF_readerContext.marpaESLIFp = marpaESLIFp; /* Our reader is special and will always say eof */
  marpaESLIF_readerContext.utf8s       = utf8s;       /* Full buffer already known */
  marpaESLIF_readerContext.utf8l       = utf8l;       /* Full buffer size */
  marpaESLIF_readerContext.p           = utf8s;       /* Reader current position */
  marpaESLIF_readerContext.markp       = NULL;        /* Marked position */

  marpaESLIFRecognizerOption.userDatavp                  = (void *) &marpaESLIF_readerContext;
  marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp   = _marpaESLIFReader_grammarReader;
  marpaESLIFRecognizerOption.marpaESLIFSkipperCallbackp  = _marpaESLIFReader_grammarSkipper;
  marpaESLIFRecognizerOption.marpaESLIFMarkerCallbackp   = _marpaESLIFReader_grammarMarker;
  marpaESLIFRecognizerOption.marpaESLIFResetterCallbackp = _marpaESLIFReader_grammarResetter;
  marpaESLIFRecognizerOption.marpaESLIFCloserCallbackp   = _marpaESLIFReader_grammarCloser;
  marpaESLIFRecognizerOption.disableThresholdb           = 1; /* No threshold warning when parsing a grammar */
  marpaESLIFRecognizerOption.exhaustedb                  = 0; /* Exhaustion is not allowed */
  marpaESLIFRecognizerOption.latmb                       = 1; /* Our internal grammar is working in latm mode */
  marpaESLIFRecognizerOption.rejectionb                  = 1; /* Our internal grammar have no rejection */

  GENERICSTACK_NEW(outputStackp);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  marpaESLIFValueOption.userDatavp     = (void *) &marpaESLIF_readerContext;
  marpaESLIFValueOption.valueCallbackp = (void *) &marpaESLIF_readerContext;
  marpaESLIFValueOption.highRankOnlyb  = 1;
  marpaESLIFValueOption.orderByRankb   = 1;
  marpaESLIFValueOption.ambiguousb     = 0;
  marpaESLIFValueOption.nullb          = 0;
  marpaESLIFValueOption.maxParsesi     = 1;
  marpaESLIFValueOption.outputStackp   = outputStackp;

  /* Parser will automatically create marpaESLIFValuep */
  if (! _marpaESLIFGrammar_parseb(marpaESLIFp->marpaESLIFGrammarp, &marpaESLIFRecognizerOption, &marpaESLIFValueOption, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, NULL /* exhaustedbp */)) {
    goto err;
  }

  /* Per def result is in marpaESLIFGrammar.outputStackp at indice 0, itself beeing a grammar instance -; */
  if (! GENERICSTACK_IS_PTR(outputStackp, 0)) {
    MARPAESLIF_ERROR(marpaESLIFp, "Not a pointer in outputStackp at indice 0");
    goto err;
  }

  marpaESLIFGrammarp = GENERICSTACK_GET_PTR(outputStackp, 0);
  /* This case is vicious: we want to free everything but the first item on the outputstack... */
  GENERICSTACK_SET_NA(outputStackp, 0);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStack set failure, %s", strerror(errno));
    goto err;
  }
  goto done;

 err:
  marpaESLIFGrammar_freev(marpaESLIFGrammarp);
  marpaESLIFGrammarp = NULL;

 done:
  _marpaESLIF_lexemeStack_freev(outputStackp); /* We own this stack, and know what is in here */
  _marpaESLIF_tconv_freev(utf8s);
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFGrammarp); */
  return marpaESLIFGrammarp;
}

/*****************************************************************************/
marpaESLIFRecognizer_t *marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */);
}

/*****************************************************************************/
short marpaESLIFRecognizer_scanb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  marpaESLIF_t *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;

  if (marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan can be done one once only");
    return 0;
  }

  marpaESLIFRecognizerp->scanb = 1;
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, initialEventsb, continuebp, exhaustedbp);
}

/*****************************************************************************/
short marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  return _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, 0 /* initialEventsb */, continuebp, exhaustedbp);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_resumeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short initialEventsb, short *continuebp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char               *funcs                             = "_marpaESLIFRecognizer_resumeb";
  marpaESLIF_t                    *marpaESLIFp                       = marpaESLIFRecognizerp->marpaESLIFp;
  genericStack_t                  *lexemeInputStackp                 = marpaESLIFRecognizerp->lexemeInputStackp;
  marpaESLIFGrammar_t             *marpaESLIFGrammarp                = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t            *grammarp                          = marpaESLIFGrammarp->grammarp;
  genericStack_t                  *symbolStackp                      = grammarp->symbolStackp;
  void                            *userDatavp                        = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.userDatavp;
  marpaESLIFReader_t               readerp                           = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.marpaESLIFReaderCallbackp;
  short                            latmb                             = marpaESLIFRecognizerp->marpaESLIFRecognizerOption.latmb;
  short                            exhaustedb                        = 0;
  short                            completedb                        = 0;
  short                            collectb                          = 0;
  marpaESLIFValueOption_t          marpaESLIFValueOptionDiscard      = marpaESLIFValueOption_default_template;
  marpaESLIFRecognizerOption_t     marpaESLIFRecognizerOptionDiscard = marpaESLIFRecognizerp->marpaESLIFRecognizerOption; /* Things overwriten, see below */
  short                            continueb;
  size_t                           maxMatchedl;
  genericStack_t                  *matchedStackp;
  genericStack_t                  *alternativeStackp;
  int                              alternativeOki;
  size_t                           nSymboll;
  int                             *symbolArrayp;
  size_t                           symboll;
  int                              symboli;
  marpaESLIF_symbol_t             *symbolp;
  marpaESLIF_matcher_value_t       rci;
  size_t                           matchedl;
  short                            rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAYP arrayp;
  marpaESLIF_string_t             *marpaESLIF_stringp;
  int                              valuei;
  size_t                           sizel;

#undef MARPAESLIFRECOGNIZER_COLLECT_EVENTS
#define MARPAESLIFRECOGNIZER_COLLECT_EVENTS(forceb, exhaustedbp)  do {  \
    if (forceb || ! collectb) {                                         \
      _marpaESLIFRecognizer_reset_events(marpaESLIFRecognizerp);        \
      if (! _marpaESLIFRecognizer_collect_grammar_eventsb(marpaESLIFRecognizerp, NULL, NULL, NULL, exhaustedbp)) { \
        goto err;                                                       \
      }                                                                 \
      collectb = 1;                                                     \
    }                                                                   \
  } while (0)

  /* Checks */
  if (! marpaESLIFRecognizerp->scanb) {
    MARPAESLIF_ERROR(marpaESLIFp, "Scan must be called first");
    return 0;
  }

  /* Initializations */
  GENERICSTACK_NEW(alternativeStackp);
  if (GENERICSTACK_ERROR(alternativeStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp init failure, %s", strerror(errno));
    goto err;
  }
  GENERICSTACK_NEW(matchedStackp);
  if (GENERICSTACK_ERROR(matchedStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "matchedStackp init failure, %s", strerror(errno));
    goto err;
  }

  marpaESLIFRecognizerp->resumeCounterl++; /* Increment internal counter for tracing */
  marpaESLIFRecognizerOptionDiscard.disableThresholdb = 1; /* If discard, prepare the option to disable threshold */
  marpaESLIFRecognizerOptionDiscard.exhaustedb        = 1; /* ... and have the exhausted event */

  if (initialEventsb) {
    MARPAESLIFRECOGNIZER_COLLECT_EVENTS(0, &exhaustedb);
    if (marpaESLIFRecognizerp->eventArrayl > 0) {
      continueb = ! exhaustedb;
      rcb = 1;
      goto done;
    }
  }
  
  /* Ask for expected lexemes */
  if (! marpaESLIFRecognizer_expectedb(marpaESLIFRecognizerp, &nSymboll, &symbolArrayp)) {
    goto err;
  }

  if (nSymboll <= 0) {
    /* No symbol expected: this is an error unless:
       - discard mode and completion is reached, or
       - exhaustion
    */
    MARPAESLIFRECOGNIZER_COLLECT_EVENTS(0, &exhaustedb);
    if ((marpaESLIFRecognizerp->discardb && completedb) || exhaustedb) {
      continueb = 0;
      rcb = 1;
      goto done;
    }
    goto err;
  }

  /* Try to match */
  alternativeOki = 0;
  maxMatchedl = 0;
  retry:
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Trying to match %s", symbolp->asciidescs);
  match_again:
    _marpaESLIF_lexemeStack_resetv(matchedStackp);
    if (! _marpaESLIFRecognizer_symbol_matcherb(marpaESLIFRecognizerp, symbolp, &rci, matchedStackp, &exhaustedb)) {
      GENERICSTACK_PUSH_NA(alternativeStackp);
      if (GENERICSTACK_ERROR(alternativeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
        goto err;
      } else {
        continue;
      }
    }
    switch (rci) {
    case MARPAESLIF_MATCH_AGAIN:
      /* We have to load more unless already at EOF */
      if (! marpaESLIFRecognizerp->eofb) {
        if (! readerp(userDatavp, &(marpaESLIFRecognizerp->inputs), &(marpaESLIFRecognizerp->inputl), &(marpaESLIFRecognizerp->eofb))) {
          MARPAESLIF_ERROR(marpaESLIFp, "reader failure");
          goto err;
        }
        goto match_again;
      } else {
        GENERICSTACK_PUSH_NA(alternativeStackp);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
          goto err;
        }
      }
      break;
    case MARPAESLIF_MATCH_FAILURE:
      /* No op */
      GENERICSTACK_PUSH_NA(alternativeStackp);
      if (GENERICSTACK_ERROR(alternativeStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
        goto err;
      }
      break;
    case MARPAESLIF_MATCH_OK:
      /* Only in this case, it is possible that matchedp contains allocated memory */

      /* When we do internal matching, we impose that the result is a genericStack ARRAY or a genericStack PTR */
      /* Per def marpaESLIFValuep is defined. */
      if (GENERICSTACK_IS_ARRAY(matchedStackp, 0)) {
        arrayp = GENERICSTACK_GET_ARRAYP(matchedStackp, 0);
        matchedl = GENERICSTACK_ARRAYP_LENGTH(arrayp);
        /* Remember this alternative */
        GENERICSTACK_PUSH_ARRAYP(alternativeStackp, arrayp);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
          goto err;
        }
        /* Prevent it to be freed if there is an error afterwards -; */
        GENERICSTACK_SET_NA(matchedStackp, 0);
        /* IMHO this can never fail because indice 0 is proved to exist */
      } else if (GENERICSTACK_IS_PTR(matchedStackp, 0)) {
        marpaESLIF_stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(matchedStackp, 0);
        matchedl = marpaESLIF_stringp->stringl;
        /* Remember this alternative */
        GENERICSTACK_PUSH_PTR(alternativeStackp, marpaESLIF_stringp);
        if (GENERICSTACK_ERROR(alternativeStackp)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "alternativeStackp push failure, %s", strerror(errno));
          goto err;
        }
        GENERICSTACK_SET_NA(matchedStackp, 0);
        /* IMHO this can never fail because indice 0 is proved to exist */
      } else {
        MARPAESLIF_ERROR(marpaESLIFp, "matchedStackp first member is not a genericStack array");
        goto err;
      }
      /* Remember max matched length */
      if (matchedl > maxMatchedl) {
        maxMatchedl = matchedl;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Setted maxMatchedl to %ld", (unsigned long) maxMatchedl);
      }
      /* Count */
      ++alternativeOki;
      break;
    default:
      MARPAESLIF_ERRORF(marpaESLIFp, "Unsupported matcher return code %d", rci);
      goto err;
    }
  }
  
  if (alternativeOki <= 0) {
    /* If we already doing discard and discard failed, this is end of the story */
    if (marpaESLIFRecognizerp->discardb) {
      goto err;
    }
    if (grammarp->marpaWrapperGrammarDiscardp != NULL) {
      /* We reuse the matched Stack even if this is no-op here for us */
      _marpaESLIF_lexemeStack_resetv(matchedStackp);
      marpaESLIFValueOptionDiscard.userDatavp     = marpaESLIFGrammarp;
      marpaESLIFValueOptionDiscard.valueCallbackp = _marpaESLIFActionCallback;
      marpaESLIFValueOptionDiscard.outputStackp   = matchedStackp;
      if (_marpaESLIFGrammar_parseb(marpaESLIFGrammarp, &marpaESLIFRecognizerOptionDiscard, &marpaESLIFValueOptionDiscard, 1 /* discardb */, marpaESLIFRecognizerp /* marpaESLIFRecognizerParentp */, NULL /* exhaustedbp */)) {
        /* Discard symbol got matched. Retry our our own match */
        /* This our own stack, so we know that per definition output will an genericStack's ARRAY at indice 0 of matchedStackp */
        if (! GENERICSTACK_IS_ARRAY(matchedStackp, 0)) {
          MARPAESLIF_ERRORF(marpaESLIFp, "Bad type %s in matched stack at indice 0", _marpaESLIF_genericStack_ix_types(alternativeStackp, 0));
          goto err;
        }
        arrayp = GENERICSTACK_GET_ARRAYP(matchedStackp, 0);
        matchedl = GENERICSTACK_ARRAYP_LENGTH(arrayp);
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Advancing stream internal position by %ld bytes", (unsigned long) matchedl);
        marpaESLIFRecognizerp->inputs += matchedl;
        marpaESLIFRecognizerp->inputl -= matchedl;
        /* Well, we KNOW that no lexeme matched. This mean that alternative stack contains only N/A's: no need to free anything */
        GENERICSTACK_USED(alternativeStackp) = 0;
        /* _marpaESLIF_lexemeStack_resetv(alternativeStackp); */
        goto retry;
      }
    }

    /* Discard failure - this is an error unless lexemes were read */
    continueb = 0;
    rcb = marpaESLIFRecognizerp->haveLexemeb;
    goto done;
  }

  /* Generate the eventual pre-lexeme events */
  /* Switch to user space now */

  /* Push alternatives - there are as many entries in alternativeStackp than the number of expected symbols */
  /* We have done it like this to be able to recover symbolp easily. */
  for (symboll = 0; symboll < nSymboll; symboll++) {
    symboli = symbolArrayp[symboll];
    if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
      MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol ID %d", symboli);
      goto err;
    }
    symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);

    if (GENERICSTACK_IS_NA(alternativeStackp, (int) symboll)) {
      continue;
    }

    if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, alternativeStackp, (int) symboll, &sizel)) {
      goto err;
    }

    if (latmb) {
      /* If latm mode is true, keep only the longests alternatives */
      if (sizel < maxMatchedl) {
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp,
                                    funcs,
                                    "Alternative %s is skipped (length %ld < max length %ld)",
                                    symbolp->asciidescs,
                                    (unsigned long) sizel,
                                    (unsigned long) maxMatchedl);
        continue;
      }      
    }

    /* Commit in the lexeme input stack */
    if (GENERICSTACK_IS_PTR(alternativeStackp, (int) symboll)) {
      GENERICSTACK_PUSH_PTR(lexemeInputStackp, GENERICSTACK_GET_PTR(alternativeStackp, (int) symboll));
      if (GENERICSTACK_ERROR(lexemeInputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp array push failure, %s", strerror(errno));
        goto err;
      }
      /* IMHO this can never fail because indice (int) symboll is proved to exist */
      GENERICSTACK_SET_NA(alternativeStackp, (int) symboll);
    } else if (GENERICSTACK_IS_ARRAY(alternativeStackp, (int) symboll)) {
      GENERICSTACK_PUSH_ARRAY(lexemeInputStackp, GENERICSTACK_GET_ARRAY(alternativeStackp, (int) symboll));
      if (GENERICSTACK_ERROR(lexemeInputStackp)) {
        MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp array push failure, %s", strerror(errno));
        goto err;
      }
      /* IMHO this can never fail because indice (int) symboll is proved to exist */
      GENERICSTACK_SET_NA(alternativeStackp, (int) symboll);
    } else {
      /* Should never happen */
      MARPAESLIF_ERRORF(marpaESLIFp, "Bad type %s in alternative stack at indice %ld", _marpaESLIF_genericStack_ix_types(alternativeStackp, (int) symboll), (unsigned long) symboll);
      goto err;
    }

    valuei = GENERICSTACK_USED(lexemeInputStackp) - 1;

    /* And to Marpa */
    if (! _marpaESLIFRecognizer_alternativeb(marpaESLIFp,
                                             marpaESLIFRecognizerp,
                                             symbolp,
                                             valuei)) {
      goto err;
    }
  }

  /* Commit */
  if (! marpaESLIFRecognizer_completeb(marpaESLIFRecognizerp)) {
#ifndef MARPAESLIF_NTRACE
    marpaWrapperRecognizer_progressLogb(marpaESLIFRecognizerp->marpaWrapperRecognizerp,
                                        0, -1,
                                        GENERICLOGGER_LOGLEVEL_TRACE,
                                        marpaESLIFGrammarp,
                                        _marpaESLIFGrammar_symbolDescriptionCallback);
#endif
    goto err;
  }

  /* Remember this recognizer have at least one lexeme */
  marpaESLIFRecognizerp->haveLexemeb = 1;
  
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Advancing stream internal position by %ld bytes", (unsigned long) maxMatchedl);
  marpaESLIFRecognizerp->inputs += maxMatchedl;
  marpaESLIFRecognizerp->inputl -= maxMatchedl;

  /* Collect events */
  MARPAESLIFRECOGNIZER_COLLECT_EVENTS(1, &exhaustedb);

  rcb = 1;
  continueb = ! exhaustedb;
  goto done;

 err:
  rcb = 0;

 done:
  _marpaESLIF_lexemeStack_freev(alternativeStackp);
  _marpaESLIF_lexemeStack_freev(matchedStackp);
  if (rcb) {
    if (exhaustedbp != NULL) {
      *exhaustedbp = exhaustedb;
    }
    if (continuebp != NULL) {
      *continuebp = continueb;
    }
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d (*continuebp=%d, *exhaustedbp=%d)", (int) rcb, (int) continueb, (int) exhaustedb);
  } else {
    MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "return 0");
  }

  return rcb;
}

/*****************************************************************************/
short marpaESLIFRecognizer_alternativeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *symbolnamecp, size_t symbolnamel, int valuei)
/*****************************************************************************/
{
  const static char    *funcs              = "marpaESLIFRecognizer_alternativeb";
  marpaESLIF_t         *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t  *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t *grammarp           = marpaESLIFGrammarp->grammarp;
  marpaESLIF_symbol_t  *symbolp;

  symbolp = _marpaESLIFRecognizer_symbol_findp(marpaESLIFRecognizerp, symbolnamecp, symbolnamel);
  if (symbolp == NULL) {
    return 0;
  }

  return _marpaESLIFRecognizer_alternativeb(marpaESLIFp, marpaESLIFRecognizerp, symbolp, valuei);
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_alternativeb(marpaESLIF_t *marpaESLIFp, marpaESLIFRecognizer_t *marpaESLIFRecognizerp, marpaESLIF_symbol_t *symbolp, int valuei)
/*****************************************************************************/
{
  const static char *funcs = "_marpaESLIFRecognizer_alternativeb";

  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "Committing alternative %s at input stack %d", symbolp->asciidescs, valuei);
  return marpaWrapperRecognizer_alternativeb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, symbolp->idi, valuei, 1);
}

/*****************************************************************************/
short marpaESLIFRecognizer_completeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  const static char *funcs = "marpaESLIFRecognizer_completeb";

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Completing alternatives");
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
  _marpaESLIFGrammar_freev(marpaESLIFGrammarp, 0 /* onStackb */);
}

/*****************************************************************************/
void marpaESLIFRecognizer_freev(marpaESLIFRecognizer_t *marpaESLIFRecognizerp)
/*****************************************************************************/
{
  if (marpaESLIFRecognizerp != NULL) {
    marpaESLIF_t           *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
    marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp = marpaESLIFRecognizerp->parentRecognizerp;
    
    _marpaESLIF_lexemeStack_freev(marpaESLIFRecognizerp->lexemeInputStackp);
    marpaWrapperRecognizer_freev(marpaESLIFRecognizerp->marpaWrapperRecognizerp);
    if (marpaESLIFRecognizerp->eventArrayp != NULL) {
      free(marpaESLIFRecognizerp->eventArrayp);
    }
    free(marpaESLIFRecognizerp);
  }
}

/*****************************************************************************/
short marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short *exhaustedbp)
/*****************************************************************************/
{
  return _marpaESLIFGrammar_parseb(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, marpaESLIFValueOptionp, 0 /* discardb */, NULL /* marpaESLIFRecognizerParentp */, exhaustedbp);
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarReader(void *userDatavp, char **inputsp, size_t *inputlp, short *eofbp)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarReader";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  if (marpaESLIF_readerContextp->p == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Stream is closed");
    return 0;
  }
  
  *inputsp = marpaESLIF_readerContextp->p;
  *inputlp  = (marpaESLIF_readerContextp->p - marpaESLIF_readerContextp->utf8s) + marpaESLIF_readerContextp->utf8l;
  *eofbp    = 1;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1 (*inputsp=%p, *inputlp=%ld, *eofbp=%d)", *inputsp, (unsigned long) *inputlp, (int) *eofbp);
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarSkipper(void *userDatavp, size_t inputl)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarSkipper";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  if (marpaESLIF_readerContextp->p == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Stream is closed");
    return 0;
  }
  
  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1 (inputl=%ld)", (unsigned long) inputl);
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarMarker(void *userDatavp, size_t limitl)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarMarker";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  if (marpaESLIF_readerContextp->p == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Stream is closed");
    return 0;
  }
  
  marpaESLIF_readerContextp->markp  = marpaESLIF_readerContextp->p;

  MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return 1 (limitl=%ld)", (unsigned long) limitl);
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarResetter(void *userDatavp)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarMarker";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  if (marpaESLIF_readerContextp->p == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, funcs, "Stream is closed");
    return 0;
  }
  
  marpaESLIF_readerContextp->p      = marpaESLIF_readerContextp->markp;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 1");
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFReader_grammarCloser(void *userDatavp)
/*****************************************************************************/
{
  const static char          *funcs                     = "marpaESLIFReader_grammarCloser";
  marpaESLIF_readerContext_t *marpaESLIF_readerContextp = (marpaESLIF_readerContext_t *) userDatavp;
  marpaESLIF_t               *marpaESLIFp               = marpaESLIF_readerContextp->marpaESLIFp;

  marpaESLIF_readerContextp->p = NULL;

  MARPAESLIF_TRACE(marpaESLIFp, funcs, "return 1");
  return 1;
}

/*****************************************************************************/
static inline marpaESLIF_symbol_t *_marpaESLIFRecognizer_symbol_findp(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *descs, size_t descl)
/*****************************************************************************/
{
  const static char    *funcs       = "_marpaESLIFRecognizer_symbol_findp";
  marpaESLIF_grammar_t *grammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp->grammarp;
  marpaESLIF_symbol_t  *symbolp     = NULL;
  marpaESLIF_symbol_t  *rcp         = NULL;
  int                  symboli;

  for (symboli = 0; symboli < GENERICSTACK_USED(grammarp->symbolStackp); symboli++) {
    if (! GENERICSTACK_IS_PTR(grammarp->symbolStackp, symboli)) {
      /* Should never happen */
      continue;
    }
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
    MARPAESLIF_ERROR(marpaESLIFRecognizerp->marpaESLIFp, "No such symbol name");
  }

  MARPAESLIF_TRACEF(marpaESLIFRecognizerp->marpaESLIFp, funcs, "return %p", rcp);
  return rcp;
}

/*****************************************************************************/
void marpaESLIFRecognizer_eventb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, size_t *eventArraylp, marpaESLIFEvent_t **eventArraypp)
/*****************************************************************************/
{
  marpaESLIFGrammar_t *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;

  if (eventArraylp != NULL) {
    *eventArraylp = marpaESLIFRecognizerp->eventArrayl;
  }
  if (eventArraypp != NULL) {
    *eventArraypp = marpaESLIFRecognizerp->eventArrayp;
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
  if (marpaESLIFRecognizerp->eventArrayp != NULL) {
    free(marpaESLIFRecognizerp->eventArrayp);
    marpaESLIFRecognizerp->eventArrayp = NULL;
  }
  marpaESLIFRecognizerp->eventArrayl = 0;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_collect_grammar_eventsb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, short *completedbp, short *nulledbp, short *predictedbp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char          *funcs              = "_marpaESLIFRecognizer_collect_grammar_eventsb";
  marpaESLIF_t               *marpaESLIFp        = marpaESLIFRecognizerp->marpaESLIFp;
  marpaESLIFGrammar_t        *marpaESLIFGrammarp = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t       *grammarp           = marpaESLIFGrammarp->grammarp;
  genericStack_t             *symbolStackp       = grammarp->symbolStackp;
  short                       completedb         = 0;
  short                       nulledb            = 0;
  short                       predictedb         = 0;
  short                       exhaustedb         = 0;
  size_t                      okeventl           = 0;
  marpaESLIF_symbol_t        *symbolp;
  int                         symboli;
  size_t                      grammarEventl;
  marpaWrapperGrammarEvent_t *grammarEventp;
  short                       rcb;
  char                       *events;
  size_t                      eventl;
  char                       *asciievents;
  size_t                      i;
  marpaESLIFEventType_t       type;
  marpaESLIFEvent_t           *eventArrayp;      /* For eventual realloc */

  /* Collect grammar native events and push them in the events stack */
  if (! marpaWrapperGrammar_eventb(marpaESLIFRecognizerp->discardb ? grammarp->marpaWrapperGrammarDiscardp : grammarp->marpaWrapperGrammarStartp, &grammarEventl, &grammarEventp, 0)) {
    goto err;
  }
  if (grammarEventl > 0) {
    marpaESLIFRecognizerp->eventArrayp = malloc(grammarEventl * sizeof(marpaESLIFEvent_t));
    if (marpaESLIFRecognizerp->eventArrayp == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    marpaESLIFRecognizerp->eventArrayl = 0;

    for (i = 0; i < grammarEventl; i++) {
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
        type        = MARPAESLIF_EVENTTYPE_COMPLETED;
        events      = symbolp->events;
        eventl      = symbolp->eventl;
        asciievents = symbolp->asciievents;
        completedb  = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - completion event", symbolp->asciidescs);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_NULLED:
        type        = MARPAESLIF_EVENTTYPE_NULLED;
        events      = symbolp->events;
        eventl      = symbolp->eventl;
        asciievents = symbolp->asciievents;
        nulledb     = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - nulling event", symbolp->asciidescs);
      case MARPAWRAPPERGRAMMAR_EVENT_EXPECTED:
        type        = MARPAESLIF_EVENTTYPE_PREDICTED;
        events      = symbolp->events;
        eventl      = symbolp->eventl;
        asciievents = symbolp->asciievents;
        predictedb  = 1;
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - prediction event", symbolp->asciidescs);
        break;
      case MARPAWRAPPERGRAMMAR_EVENT_EXHAUSTED:
        /* This is ok at EOF or if the recognizer is ok with exhaustion */
        if ((! marpaESLIFRecognizerp->eofb) && (! marpaESLIFRecognizerp->marpaESLIFRecognizerOption.exhaustedb)) {
          MARPAESLIF_ERROR(marpaESLIFp, "Grammar is exhausted but lexeme remains");
          goto err;
        }
        type        = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        events      = NULL;
        eventl      = 0;
        asciievents = 0;
        exhaustedb  = 1;
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Exhausted event");
        /* symboli will be -1 as per marpaWrapper spec */
        break;
      default:
        MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s - unsupported event type %d", symbolp->asciidescs, grammarEventp[eventl].eventType);
        break;
      }

      if (events != NULL) {
        marpaESLIFRecognizerp->eventArrayp[okeventl].type   = type;
        marpaESLIFRecognizerp->eventArrayp[okeventl].events = events;
        marpaESLIFRecognizerp->eventArrayp[okeventl].eventl = eventl;
        marpaESLIFRecognizerp->eventArrayl = ++okeventl;
      }
    }
  }

  if (! exhaustedb) {
    /* The event on exhaustion only occurs if needed to provide a reason to return. */
    /* If not sent by the grammar, we check explicitely ourself.                    */
    if (! marpaWrapperRecognizer_exhaustedb(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &exhaustedb)) {
      goto err;
    }
    if (exhaustedb) {
      /* This is ok at EOF or if the recognizer is ok with exhaustion */
      if ((! marpaESLIFRecognizerp->eofb) && (! marpaESLIFRecognizerp->marpaESLIFRecognizerOption.exhaustedb)) {
        MARPAESLIF_ERROR(marpaESLIFp, "Grammar is exhausted but lexeme remains");
        goto err;
      }
      /* Extend of create the array */
      if (okeventl <= 0) {
        marpaESLIFRecognizerp->eventArrayp = malloc(sizeof(marpaESLIFEvent_t));
        if (marpaESLIFRecognizerp->eventArrayp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
          goto err;
        }
        marpaESLIFRecognizerp->eventArrayp[0].type   = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        marpaESLIFRecognizerp->eventArrayp[0].events = NULL;
        marpaESLIFRecognizerp->eventArrayp[0].eventl = 0;
        marpaESLIFRecognizerp->eventArrayl = 1;
      } else {
        eventArrayp = realloc(marpaESLIFRecognizerp->eventArrayp, (okeventl + 1) + sizeof(marpaESLIFEvent_t));
        if (eventArrayp == NULL) {
          MARPAESLIF_ERRORF(marpaESLIFp, "realloc failure, %s", strerror(errno));
          goto err;
        }
        marpaESLIFRecognizerp->eventArrayp = eventArrayp;
        marpaESLIFRecognizerp->eventArrayp[okeventl].type   = MARPAESLIF_EVENTTYPE_EXHAUSTED;
        marpaESLIFRecognizerp->eventArrayp[okeventl].events = NULL;
        marpaESLIFRecognizerp->eventArrayp[okeventl].eventl = 0;
        marpaESLIFRecognizerp->eventArrayl++;
      }
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "Exhausted event");
    }
  }

  rcb = 1;
  goto done;
  
 err:
  rcb = 0;

 done:
  if (completedbp != NULL) {
    *completedbp = completedb;
  }
  if (nulledbp != NULL) {
    *nulledbp = nulledb;
  }
  if (predictedbp != NULL) {
    *predictedbp = predictedb;
  }
  if (exhaustedbp != NULL) {
    *exhaustedbp = exhaustedb;
  }
  /* MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d (*exhaustedbp=%d)", (int) rcb, (int) exhaustedb); */
  return rcb;
}

/*****************************************************************************/
static inline marpaESLIFRecognizer_t *_marpaESLIFRecognizer_newp(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp)
/*****************************************************************************/
{
  const static char             *funcs                 = "_marpaESLIFRecognizer_newp";
  marpaESLIF_t                  *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t        *marpaESLIFRecognizerp = NULL;
  marpaWrapperRecognizerOption_t marpaWrapperRecognizerOption;

#ifndef MARPAESLIF_NTRACE
  /*
  if (marpaESLIFRecognizerParentp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerParentp, funcs, "Building %s Recognizer", discardb ? ":discard" : ":start");
  } else {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "Building %s Recognizer", discardb ? ":discard" : ":start");
  }
  */
#endif
  
  if (marpaESLIFRecognizerOptionp == NULL) {
    marpaESLIFRecognizerOptionp = &marpaESLIFRecognizerOption_default;
  }

  if (marpaESLIFGrammarp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null grammar pointer");
    goto err;
  }

  /* We request all the stream callbacks */
  if (marpaESLIFRecognizerOptionp->marpaESLIFReaderCallbackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null reader callback");
    goto err;
  }
  if (marpaESLIFRecognizerOptionp->marpaESLIFSkipperCallbackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null skipper callback");
    goto err;
  }
  if (marpaESLIFRecognizerOptionp->marpaESLIFMarkerCallbackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null marker callback");
    goto err;
  }
  if (marpaESLIFRecognizerOptionp->marpaESLIFResetterCallbackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null resetter callback");
    goto err;
  }
  if (marpaESLIFRecognizerOptionp->marpaESLIFCloserCallbackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Null closer callback");
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
  marpaESLIFRecognizerp->lexemeInputStackp          = NULL;
  marpaESLIFRecognizerp->eventArrayp                = NULL;
  marpaESLIFRecognizerp->eventArrayl                = 0;
  marpaESLIFRecognizerp->parentRecognizerp          = marpaESLIFRecognizerParentp;
  marpaESLIFRecognizerp->resumeCounterl             = 0;
  /* If this is a parent recognizer get its stream information */
  if (marpaESLIFRecognizerParentp != NULL) {
    marpaESLIFRecognizerp->inputs                     = marpaESLIFRecognizerParentp->inputs;
    marpaESLIFRecognizerp->inputl                     = marpaESLIFRecognizerParentp->inputl;
    marpaESLIFRecognizerp->eofb                       = marpaESLIFRecognizerParentp->eofb;
  } else {
    marpaESLIFRecognizerp->inputs                     = NULL;
    marpaESLIFRecognizerp->inputl                     = 0;
    marpaESLIFRecognizerp->eofb                       = 0;
  }
  marpaESLIFRecognizerp->scanb                      = 0;
  marpaESLIFRecognizerp->discardb                   = discardb;
  marpaESLIFRecognizerp->haveLexemeb                = 0;

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
  GENERICSTACK_NEW(marpaESLIFRecognizerp->lexemeInputStackp);
  if (marpaESLIFRecognizerp->lexemeInputStackp == NULL) {
    MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp initialization failure, %s", strerror(errno));
    goto err;
  }
  
  /* Marpa does not like the indice 0 */
  GENERICSTACK_PUSH_NA(marpaESLIFRecognizerp->lexemeInputStackp);
  if (GENERICSTACK_ERROR(marpaESLIFRecognizerp->lexemeInputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "lexemeInputStackp push failure, %s", strerror(errno));
    goto err;
  }

  goto done;

 err:
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  marpaESLIFRecognizerp = NULL;

 done:
#ifndef MARPAESLIF_NTRACE
  /*
  if (marpaESLIFRecognizerParentp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerParentp, funcs, "return %p", marpaESLIFRecognizerp);
  } else {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFRecognizerp);
  }
  */
#endif
  return marpaESLIFRecognizerp;
}

/*****************************************************************************/
static inline short _marpaESLIFGrammar_parseb(marpaESLIFGrammar_t *marpaESLIFGrammarp, marpaESLIFRecognizerOption_t *marpaESLIFRecognizerOptionp, marpaESLIFValueOption_t *marpaESLIFValueOptionp, short discardb, marpaESLIFRecognizer_t *marpaESLIFRecognizerParentp, short *exhaustedbp)
/*****************************************************************************/
{
  const static char      *funcs                 = "_marpaESLIFGrammar_parseb";
  marpaESLIF_t           *marpaESLIFp           = marpaESLIFGrammarp->marpaESLIFp;
  marpaESLIFRecognizer_t *marpaESLIFRecognizerp = NULL;
  marpaESLIFValueOption_t marpaESLIFValueOption = (marpaESLIFValueOptionp != NULL) ? *marpaESLIFValueOptionp : marpaESLIFValueOption_default_template;
  marpaESLIFValue_t      *marpaESLIFValuep      = NULL;
  short                   exhaustedb;
  short                   continueb;
  short                   rcb;

  marpaESLIFRecognizerp = _marpaESLIFRecognizer_newp(marpaESLIFGrammarp, marpaESLIFRecognizerOptionp, discardb, marpaESLIFRecognizerParentp);
  if (marpaESLIFRecognizerp == NULL) {
    goto err;
  }
  if (! marpaESLIFRecognizer_scanb(marpaESLIFRecognizerp, 1 /* initialEventsb */, &continueb, &exhaustedb)) {
    goto err;
  }
  while (continueb) {
    if (! marpaESLIFRecognizer_resumeb(marpaESLIFRecognizerp, &continueb, &exhaustedb)) {
      goto err;
    }
  }

  /* Force unambiguity */
  marpaESLIFValueOption.ambiguousb = 0;
  marpaESLIFValuep = marpaESLIFValue_newp(marpaESLIFRecognizerp, &marpaESLIFValueOption);
  if (marpaESLIFValuep == NULL) {
    goto err;
  }
  /* No loop because we ask for a non-ambigous parse tree value */
  if (! marpaESLIFValue_valueb(marpaESLIFValuep)) {
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
  marpaESLIFValue_freev(marpaESLIFValuep);
#ifndef MARPAESLIF_NTRACE
  if (marpaESLIFRecognizerp != NULL) {
    MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "return %d", (int) rcb);
  } else {
    MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %d", (int) rcb);
  }
#endif
  marpaESLIFRecognizer_freev(marpaESLIFRecognizerp);
  return rcb;
}

/*****************************************************************************/
static void _marpaESLIF_generateStringWithLoggerCallback(void *userDatavp, genericLoggerLevel_t logLeveli, const char *msgs)
/*****************************************************************************/
{
  _marpaESLIF_stringGenerator_t *contextp = (_marpaESLIF_stringGenerator_t *) userDatavp;
  char                         *tmps;
  size_t                        l;

  if (contextp->s == NULL) {
    contextp->l = strlen(msgs) + 1;
    contextp->s = strdup(msgs);
    if (contextp->s == NULL) {
      contextp->l = 0;
      MARPAESLIF_ERRORF(contextp->marpaESLIFp, "strdup failure, %s", strerror(errno));
    }
  } else {
    l = contextp->l;
    contextp->l = strlen(contextp->s) + strlen(msgs) + 1;
    tmps = (char *) realloc(contextp->s, contextp->l);
    if (tmps != NULL) {
      strcat(tmps, msgs);
      contextp->s = tmps;
    } else {
      contextp->l = l;
      MARPAESLIF_ERRORF(contextp->marpaESLIFp, "realloc failure, %s", strerror(errno));
    }
  }
}

#ifndef MARPAESLIF_NTRACE
/* C.f. http://grapsus.net/blog/post/Hexadecimal-dump-in-C */
#ifndef MARPAESLIF_HEXDUMP_COLS
#define MARPAESLIF_HEXDUMP_COLS 16
#endif
/*****************************************************************************/
static inline void _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, char *headers, char *asciidescs, const char *p, size_t lengthl)
/*****************************************************************************/
{
  const static char             *funcs       = "_marpaESLIFRecognizer_hexdumpv";
  marpaESLIF_t                  *marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  _marpaESLIF_stringGenerator_t _marpaESLIF_stringGenerator;
  genericLogger_t               *genericLoggerp;
  size_t  i;
  size_t  j;
  size_t  prevl;

  _marpaESLIF_stringGenerator.marpaESLIFp = marpaESLIFRecognizerp->marpaESLIFp;
  _marpaESLIF_stringGenerator.s           = NULL;
  _marpaESLIF_stringGenerator.l           = 0;

  genericLoggerp = GENERICLOGGER_CUSTOM(_marpaESLIF_generateStringWithLoggerCallback, (void *) &_marpaESLIF_stringGenerator, GENERICLOGGER_LOGLEVEL_TRACE);
  if (genericLoggerp == NULL) {
    return;
  }
  
  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "--------------------------------------------");
  MARPAESLIFRECOGNIZER_TRACEF(marpaESLIFRecognizerp, funcs, "%s%s, length %ld", headers, asciidescs, (unsigned long) lengthl);
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
        if (j >= lengthl) { /* end of block, not really printing */
          GENERICLOGGER_TRACE(genericLoggerp, " ");
        }
        else if (isprint(0xFF & ((char*)p)[j])) { /* printable char */
          GENERICLOGGER_TRACEF(genericLoggerp, "%c", 0xFF & ((char*)p)[j]);
        }
        else { /* other char */
          GENERICLOGGER_TRACE(genericLoggerp, ".");
        }
      }
      if (_marpaESLIF_stringGenerator.s != NULL) {
        MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, _marpaESLIF_stringGenerator.s);
        free(_marpaESLIF_stringGenerator.s);
        _marpaESLIF_stringGenerator.s = NULL;
      }
    }
  }

  MARPAESLIFRECOGNIZER_TRACE(marpaESLIFRecognizerp, funcs, "--------------------------------------------");
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

  /* MARPAESLIF_TRACE(marpaESLIFp, funcs, "Building Value"); */

  if (marpaESLIFValueOptionp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Value option structure must not be NULL");
    goto err;
  }
  if (marpaESLIFValueOptionp->outputStackp == NULL) {
    MARPAESLIF_ERROR(marpaESLIFp, "Value option structure's outputStackp must not be NULL");
    goto err;
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
  marpaESLIFValuep->context.ruleIdi        = -1;

  marpaWrapperValueOption.genericLoggerp = marpaESLIFp->marpaESLIFOption.genericLoggerp;
  marpaWrapperValueOption.highRankOnlyb  = marpaESLIFValueOptionp->highRankOnlyb;
  marpaWrapperValueOption.orderByRankb   = marpaESLIFValueOptionp->orderByRankb;
  marpaWrapperValueOption.ambiguousb     = marpaESLIFValueOptionp->ambiguousb;
  marpaWrapperValueOption.nullb          = marpaESLIFValueOptionp->nullb;
  marpaWrapperValueOption.maxParsesi     = marpaESLIFValueOptionp->maxParsesi;
  marpaWrapperValuep = marpaWrapperValue_newp(marpaESLIFRecognizerp->marpaWrapperRecognizerp, &marpaWrapperValueOption);
  if (marpaWrapperValuep == NULL) {
#ifndef MARPAESLIF_NTRACE
    marpaWrapperRecognizer_progressLogb(marpaESLIFRecognizerp->marpaWrapperRecognizerp,
                                        0, -1,
                                        GENERICLOGGER_LOGLEVEL_TRACE,
                                        marpaESLIFRecognizerp->marpaESLIFGrammarp,
                                        _marpaESLIFGrammar_symbolDescriptionCallback);
#endif
    goto err;
  }

  marpaESLIFValuep->marpaWrapperValuep = marpaWrapperValuep;
  goto done;

 err:
  marpaESLIFValue_freev(marpaESLIFValuep);
  marpaESLIFValuep = NULL;

 done:
  /* MARPAESLIF_TRACEF(marpaESLIFp, funcs, "return %p", marpaESLIFValuep); */
  return marpaESLIFValuep;
}

/*****************************************************************************/
short marpaESLIFValue_valueb(marpaESLIFValue_t *marpaESLIFValuep)
/*****************************************************************************/
{
  return marpaWrapperValue_valueb(marpaESLIFValuep->marpaWrapperValuep,
                                  marpaESLIFValuep, /* Our value context */
                                  _marpaESLIFValueRuleCallback,
                                  _marpaESLIFValueSymbolCallback,
                                  _marpaESLIFValueNullingCallback);
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
    free(marpaESLIFValuep);
  }
}

/*****************************************************************************/
static short _marpaESLIFValueRuleCallback(void *userDatavp, int rulei, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  const static char              *funcs                 = "marpaESLIFValueRuleCallback";
  marpaESLIFValue_t              *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp              = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *ruleStackp            = grammarp->ruleStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->marpaESLIFValueOption.outputStackp;
  size_t                          totalSizel = 0;
  size_t                          previousTotalSizel = 0;
  size_t                          sizel;
  marpaESLIF_rule_t              *rulep;
  int                             i;
  short                           rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  char                           *p;
  char                           *q;

  /* First thing to do -; */
  GENERICSTACK_ARRAY_PTR(array) = NULL;
  GENERICSTACK_ARRAY_LENGTH(array) = 0;

  if (! GENERICSTACK_IS_PTR(ruleStackp, rulei)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such rule No %d", rulei);
    goto err;
  }
  rulep = (marpaESLIF_rule_t *) GENERICSTACK_GET_PTR(ruleStackp, rulei);
  MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "%s [%d..%d] -> [%d]", rulep->asciidescs, arg0i, argni, resulti);

  /* Collect final size */
  for (i = arg0i; i <= argni; i++) {
    if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, outputStackp, i, &sizel)) {
      goto err;
    }
    /* Who knows */
    previousTotalSizel = totalSizel;
    totalSizel += sizel;
    if (totalSizel < previousTotalSizel) {
      MARPAESLIF_ERROR(marpaESLIFp, "size_t turnaround");
      goto err;
    }
  }

  if (totalSizel > 0) {
    q = GENERICSTACK_ARRAY_PTR(array) = malloc(totalSizel);
    if (GENERICSTACK_ARRAY_PTR(array) == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    for (i = arg0i; i <= argni; i++) {
      if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, outputStackp, i, &sizel)) {
        goto err;
      }
      if (! _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizerp, outputStackp, i, &p)) {
        goto err;
      }
      if ((sizel > 0) && (p == NULL)) {
        MARPAESLIF_ERROR(marpaESLIFp, "sizel > 0 but p is NULL");
        goto err;
      }
      if (p != NULL) {
        memcpy(q, p, sizel);
        GENERICSTACK_ARRAY_LENGTH(array) += sizel;
        q += sizel;
      }
    }
  }

  /* It can happen that resulti is already existing in the output stack */
  if (! _marpaESLIF_lexemeStack_ix_resetb(marpaESLIFRecognizerp, outputStackp, resulti)) {
    goto err;
  }
  GENERICSTACK_SET_ARRAY(outputStackp, array, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp push failure, %s", strerror(errno));
    goto err;
  }
  
  rcb = 1;
  _marpaESLIFRecognizer_hexdumpv(marpaESLIFRecognizerp, "Value result ", rulep->asciidescs, GENERICSTACK_ARRAY_PTR(array), GENERICSTACK_ARRAY_LENGTH(array));
  goto done;

 err:
  if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
    free(GENERICSTACK_ARRAY_PTR(array));
  }
  rcb = 0;

 done:
  /* MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "return %d", (int) rcb); */
  return 1;
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
  genericStack_t                 *inputStackp           = marpaESLIFRecognizerp->lexemeInputStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->marpaESLIFValueOption.outputStackp;
  size_t                          sizel;
  marpaESLIF_symbol_t            *symbolp;
  short                           rcb;
  GENERICSTACKITEMTYPE2TYPE_ARRAY array;
  char                           *p;

  /* First thing to do -; */
  GENERICSTACK_ARRAY_PTR(array) = NULL;
  GENERICSTACK_ARRAY_LENGTH(array) = 0;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
  MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "%s [%d] -> [%d]", symbolp->asciidescs, argi, resulti);

  /* Get size */
  if (! _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizerp, inputStackp, argi, &sizel)) {
    goto err;
  }

  if (sizel > 0) {
    GENERICSTACK_ARRAY_PTR(array) = malloc(sizel);
    if (GENERICSTACK_ARRAY_PTR(array) == NULL) {
      MARPAESLIF_ERRORF(marpaESLIFp, "malloc failure, %s", strerror(errno));
      goto err;
    }
    if (! _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizerp, inputStackp, argi, &p)) {
      goto err;
    }
    if ((sizel > 0) && (p == NULL)) {
      MARPAESLIF_ERROR(marpaESLIFp, "sizel > 0 but p is NULL");
      goto err;
    }
    memcpy(GENERICSTACK_ARRAY_PTR(array), p, sizel);
    GENERICSTACK_ARRAY_LENGTH(array) = sizel;
  }

  /* It can happen that resulti is already existing in the output stack */
  if (! _marpaESLIF_lexemeStack_ix_resetb(marpaESLIFRecognizerp, outputStackp, resulti)) {
    goto err;
  }
  GENERICSTACK_SET_ARRAY(outputStackp, array, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp push failure, %s", strerror(errno));
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  if (GENERICSTACK_ARRAY_PTR(array) != NULL) {
    free(GENERICSTACK_ARRAY_PTR(array));
  }
  rcb = 0;

 done:
  /* MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "return %d", (int) rcb); */
  return 1;
}

/*****************************************************************************/
static short _marpaESLIFValueNullingCallback(void *userDatavp, int symboli, int resulti)
/*****************************************************************************/
{
  const static char              *funcs                 = "marpaESLIFValueNullingCallback";
  marpaESLIFValue_t              *marpaESLIFValuep      = (marpaESLIFValue_t *) userDatavp;
  marpaESLIF_t                   *marpaESLIFp           = marpaESLIFValuep->marpaESLIFp;
  marpaESLIFRecognizer_t         *marpaESLIFRecognizerp = marpaESLIFValuep->marpaESLIFRecognizerp;
  marpaESLIFGrammar_t            *marpaESLIFGrammarp    = marpaESLIFRecognizerp->marpaESLIFGrammarp;
  marpaESLIF_grammar_t           *grammarp              = marpaESLIFGrammarp->grammarp;
  genericStack_t                 *symbolStackp          = grammarp->symbolStackp;
  genericStack_t                 *outputStackp          = marpaESLIFValuep->marpaESLIFValueOption.outputStackp;
  marpaESLIF_symbol_t            *symbolp;
  short                           rcb;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "No such symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);
  MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "%s -> [%d]", symbolp->asciidescs, resulti);

  /* It can happen that resulti is already existing in the output stack */
  if (! _marpaESLIF_lexemeStack_ix_resetb(marpaESLIFRecognizerp, outputStackp, resulti)) {
    goto err;
  }
  GENERICSTACK_SET_PTR(outputStackp, NULL, resulti);
  if (GENERICSTACK_ERROR(outputStackp)) {
    MARPAESLIF_ERRORF(marpaESLIFp, "outputStackp push failure, %s", strerror(errno));
    goto err;
  }
  
  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  /* MARPAESLIFVALUE_TRACEF(marpaESLIFValuep, funcs, "return %d", (int) rcb); */
  return 1;
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
static short _marpaESLIFActionCallback(void *userDatavp, char *names, size_t namel, int arg0i, int argni, int resulti)
/*****************************************************************************/
{
  const static char   *funcs              = "_marpaESLIFActionCallback";
  marpaESLIFGrammar_t *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
}

/*****************************************************************************/
static inline void _marpaESLIFGrammar_freev(marpaESLIFGrammar_t *marpaESLIFGrammarp, short onStackb)
/*****************************************************************************/
{
  if (marpaESLIFGrammarp != NULL) {
    if (marpaESLIFGrammarp->grammarStackp != NULL) {
      while (GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp) > 0) {
	if (GENERICSTACK_IS_PTR(marpaESLIFGrammarp->grammarStackp, GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp) - 1)) {
	  marpaESLIF_grammar_t *grammarp = (marpaESLIF_grammar_t *) GENERICSTACK_POP_PTR(marpaESLIFGrammarp->grammarStackp);
	  _marpaESLIF_grammar_freev(grammarp);
	} else {
	  GENERICSTACK_USED(marpaESLIFGrammarp->grammarStackp)--;
	}
      }
      GENERICSTACK_FREE(marpaESLIFGrammarp->grammarStackp);
    }
    if (! onStackb) {
      free(marpaESLIFGrammarp);
    }
  }
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_ix_sizeb(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, size_t *sizelp)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIFRecognizer_lexemeStack_ix_sizeb";
  marpaESLIF_string_t              *marpaESLIF_stringp;
  GENERICSTACKITEMTYPE2TYPE_ARRAYP  arrayp;
  size_t                            sizel;
  short                             rcb;

  if (GENERICSTACK_IS_PTR(lexemeStackp, ix)) {
    marpaESLIF_stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(lexemeStackp, ix);
    if (marpaESLIF_stringp == NULL) {
      /* Nulling */
      sizel = 0;
    } else {
      sizel = marpaESLIF_stringp->stringl;
      if (sizel == 0) {
        /* It is illegal to have a marpaESLIF_string with a zero size */
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad string size in lexeme stack at indice %d", ix);
        goto err;
      }
    }
  } else if (GENERICSTACK_IS_ARRAY(lexemeStackp, ix)) {
    arrayp = GENERICSTACK_GET_ARRAYP(lexemeStackp, ix);
    sizel = GENERICSTACK_ARRAYP_LENGTH(arrayp);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexeme stack at indice %d", _marpaESLIF_genericStack_ix_types(lexemeStackp, ix), ix);
    goto err;
  }

  rcb = 1;
  *sizelp = sizel;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline short _marpaESLIFRecognizer_lexemeStack_ix_p(marpaESLIFRecognizer_t *marpaESLIFRecognizerp, genericStack_t *lexemeStackp, int ix, char **pp)
/*****************************************************************************/
{
  const static char                *funcs = "_marpaESLIFRecognizer_lexemeStack_ix_p";
  marpaESLIF_string_t              *marpaESLIF_stringp;
  GENERICSTACKITEMTYPE2TYPE_ARRAYP  arrayp;
  short                             rcb;
  char                             *p;

  if (GENERICSTACK_IS_PTR(lexemeStackp, ix)) {
    marpaESLIF_stringp = (marpaESLIF_string_t *) GENERICSTACK_GET_PTR(lexemeStackp, ix);
    if (marpaESLIF_stringp == NULL) {
      /* Nulling */
      p = NULL;
    } else {
      p = marpaESLIF_stringp->stringp;
      if (p == NULL) {
        /* It is illegal to have a marpaESLIF_string with a NULL pointer */
        MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad string pointer in lexeme stack at indice %d", ix);
        goto err;
      }
    }
  } else if (GENERICSTACK_IS_ARRAY(lexemeStackp, ix)) {
    arrayp = GENERICSTACK_GET_ARRAYP(lexemeStackp, ix);
    p = GENERICSTACK_ARRAYP_PTR(arrayp);
  } else {
    MARPAESLIF_ERRORF(marpaESLIFRecognizerp->marpaESLIFp, "Bad type %s in lexeme stack at indice %d", _marpaESLIF_genericStack_ix_types(lexemeStackp, ix), ix);
    goto err;
  }

  rcb = 1;
  *pp = p;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
static inline const char *_marpaESLIF_genericStack_ix_types(genericStack_t *stackp, int ix)
/*****************************************************************************/
{
  const char *s;

  switch (GENERICSTACKITEMTYPE(stackp, ix)) {
  case GENERICSTACKITEMTYPE_NA:
    s = GENERICSTACKITEMTYPE_NA_STRING;
    break;
  case GENERICSTACKITEMTYPE_CHAR:
    s = GENERICSTACKITEMTYPE_CHAR_STRING;
    break;
  case GENERICSTACKITEMTYPE_SHORT:
    s = GENERICSTACKITEMTYPE_SHORT_STRING;
    break;
  case GENERICSTACKITEMTYPE_INT:
    s = GENERICSTACKITEMTYPE_INT_STRING;
    break;
  case GENERICSTACKITEMTYPE_LONG:
    s = GENERICSTACKITEMTYPE_LONG_STRING;
    break;
  case GENERICSTACKITEMTYPE_FLOAT:
    s = GENERICSTACKITEMTYPE_FLOAT_STRING;
    break;
  case GENERICSTACKITEMTYPE_DOUBLE:
    s = GENERICSTACKITEMTYPE_DOUBLE_STRING;
    break;
  case GENERICSTACKITEMTYPE_PTR:
    s = GENERICSTACKITEMTYPE_PTR_STRING;
    break;
  case GENERICSTACKITEMTYPE_ARRAY:
    s = GENERICSTACKITEMTYPE_ARRAY_STRING;
    break;
  default:
    s = GENERICSTACKITEMTYPE_UNKNOWN_STRING;
    break;
  }

  return s;
}

/*****************************************************************************/
static char *_marpaESLIFGrammar_symbolDescriptionCallback(void *userDatavp, int symboli)
/*****************************************************************************/
{
  const static char    *funcs              = "_marpaESLIFGrammar_symbolDescriptionCallback";
  marpaESLIFGrammar_t  *marpaESLIFGrammarp = (marpaESLIFGrammar_t *) userDatavp;
  marpaESLIF_grammar_t *grammarp           = marpaESLIFGrammarp->grammarp;
  genericStack_t       *symbolStackp       = grammarp->symbolStackp;
  marpaESLIF_symbol_t  *symbolp;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    return NULL;
  }
  symbolp = GENERICSTACK_GET_PTR(symbolStackp, symboli);

  return symbolp->asciidescs;
}

/*****************************************************************************/
 static short _marpaESLIFGrammar_symbolOptionSetterNoEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/*****************************************************************************/
{
  const static char         *funcs                    = "_marpaESLIFGrammar_symbolOptionSetterNoEvent";
  marpaESLIF_cloneContext_t *marpaESLIF_cloneContextp = (marpaESLIF_cloneContext_t *) userDatavp;
  marpaESLIF_grammar_t      *grammarp                 = marpaESLIF_cloneContextp->grammarp;
  genericStack_t            *symbolStackp             = grammarp->symbolStackp;
  marpaESLIF_symbol_t       *symbolp;
  short                     rcb;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIF_cloneContextp->marpaESLIFp, "Cannot find symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);

  /* Consistenty check */
  if (symbolp->idi != symboli) {
    MARPAESLIF_ERRORF(marpaESLIF_cloneContextp->marpaESLIFp, "Clone symbol callback for symbol No %d while we have %d !?", symboli, symbolp->idi);
    goto err;
  }

  if (marpaWrapperGrammarSymbolOptionp->eventSeti != MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE) {
    MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Resetting event set for symbol No %d (%s) at grammar level %d (%s)", symbolp->idi, symbolp->asciidescs, grammarp->leveli, grammarp->asciidescs);
    marpaWrapperGrammarSymbolOptionp->eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE;
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

/*****************************************************************************/
 static short _marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent(void *userDatavp, int symboli, marpaWrapperGrammarSymbolOption_t *marpaWrapperGrammarSymbolOptionp)
/*****************************************************************************/
{
  const static char         *funcs                    = "_marpaESLIFGrammar_symbolOptionSetterOnlyStartCompletionEvent";
  marpaESLIF_cloneContext_t *marpaESLIF_cloneContextp = (marpaESLIF_cloneContext_t *) userDatavp;
  marpaESLIF_grammar_t      *grammarp                 = marpaESLIF_cloneContextp->grammarp;
  genericStack_t            *symbolStackp             = grammarp->symbolStackp;
  marpaESLIF_symbol_t       *symbolp;
  short                     rcb;

  if (! GENERICSTACK_IS_PTR(symbolStackp, symboli)) {
    MARPAESLIF_ERRORF(marpaESLIF_cloneContextp->marpaESLIFp, "Cannot find symbol No %d", symboli);
    goto err;
  }
  symbolp = (marpaESLIF_symbol_t *) GENERICSTACK_GET_PTR(symbolStackp, symboli);

  /* Consistenty check */
  if (symbolp->idi != symboli) {
    MARPAESLIF_ERRORF(marpaESLIF_cloneContextp->marpaESLIFp, "Clone symbol callback for symbol No %d while we have %d !?", symboli, symbolp->idi);
    goto err;
  }

  if (symbolp->discardb) {
    if (marpaWrapperGrammarSymbolOptionp->eventSeti != MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION) {
      MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Setting event set for symbol No %d (%s) at grammar level %d (%s) on completion only", symbolp->idi, symbolp->asciidescs, grammarp->leveli, grammarp->asciidescs);
      marpaWrapperGrammarSymbolOptionp->eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_COMPLETION;
    }
  } else {
    if (marpaWrapperGrammarSymbolOptionp->eventSeti != MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE) {
      MARPAESLIF_TRACEF(marpaESLIF_cloneContextp->marpaESLIFp, funcs, "Resetting event set for symbol No %d (%s) at grammar level %d (%s)", symbolp->idi, symbolp->asciidescs, grammarp->leveli, grammarp->asciidescs);
      marpaWrapperGrammarSymbolOptionp->eventSeti = MARPAWRAPPERGRAMMAR_EVENTTYPE_NONE;
    }
  }

  rcb = 1;
  goto done;

 err:
  rcb = 0;

 done:
  return rcb;
}

